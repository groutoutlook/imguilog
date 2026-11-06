#include "ILogImGui.hpp"
#include "imgui_internal.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>

namespace
{
void check(bool condition, const std::string& message)
{
    if (!condition)
    {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

class ConsoleHarness
{
public:
    ConsoleHarness()
    {
        ImGui::CreateContext();
        auto& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        io.DisplaySize = ImVec2(1200, 900);
        io.DeltaTime = 1.0f / 60.0f;
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        frame(true);
        frame(true);
        frame();
    }

    ~ConsoleHarness() { ImGui::DestroyContext(); }

    void frame(bool focus = false)
    {
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(1100, 800));
        ImGui::Begin("Console completion test", nullptr, ImGuiWindowFlags_NoSavedSettings);
        inputId = ImGui::GetID("##Input");
        if (focus)
            ImGui::SetKeyboardFocusHere();
        console.display(&submitted);
        ImGui::End();
        ImGui::Render();
    }

    void key(ImGuiKey key)
    {
        ImGui::GetIO().AddKeyEvent(key, true);
        frame();
        ImGui::GetIO().AddKeyEvent(key, false);
        frame();
    }

    void control(ImGuiKey key)
    {
        ImGui::GetIO().AddKeyEvent(ImGuiMod_Ctrl, true);
        frame();
        this->key(key);
        ImGui::GetIO().AddKeyEvent(ImGuiMod_Ctrl, false);
        frame();
    }

    void type(const char* text)
    {
        ImGui::GetIO().AddInputCharactersUTF8(text);
        frame();
    }

    void replace(const char* text)
    {
        control(ImGuiKey_A);
        key(ImGuiKey_Backspace);
        type(text);
    }

    std::string text() const
    {
        auto* state = ImGui::GetInputTextState(inputId);
        check(state != nullptr, "input state exists");
        return state->GetText();
    }

    ImGuiWindow* suggestions() const
    {
        for (auto* window : ImGui::GetCurrentContext()->Windows)
            if (window->Active && std::string(window->Name).find("##CommandSuggestions") != std::string::npos)
                return window;
        return nullptr;
    }

    void clickFirstSuggestion()
    {
        auto* window = suggestions();
        check(window != nullptr, "suggestion list visible before click");
        const ImVec2 point(window->DC.CursorStartPos.x + 20, window->DC.CursorStartPos.y + 5);
        ImGui::GetIO().AddMousePosEvent(point.x, point.y);
        frame();
        ImGui::GetIO().AddMouseButtonEvent(0, true);
        frame();
        ImGui::GetIO().AddMouseButtonEvent(0, false);
        frame();
        frame();
        frame();
    }

    bool submitted{false};

private:
    ILog::ImGuiConsole console;
    ImGuiID inputId{0};
};
}

int main()
{
    using ILog::ImGuiConsole;
    int executions = 0;
    int providerCalls = 0;
    std::string executed;
    const auto execute = [&](const std::string& line) { ++executions; executed = line; };
    // Existing three-field aggregate registration must remain source compatible.
    ImGuiConsole::addCommand({"playback", "playback play|pause", execute});
    ImGuiConsole::addCommand({"protocol", "protocol status", execute});
    ImGuiConsole::addCommand({
        "view", "view <name> [show|hide|toggle]", execute,
        [&](const std::string& prefix) {
            ++providerCalls;
            check(prefix.rfind("view ", 0) == 0, "provider receives full prefix");
            return std::vector<std::string>{"view controls", "view console", "view controls",
                                             "view fonts", "viewing", "playback play"};
        }});
    const std::string longCandidate = "long " + std::string(400, 'x');
    ImGuiConsole::addCommand({"long", "long <value>", execute,
        [&](const std::string&) { return std::vector<std::string>{longCandidate}; }});

    auto candidates = ImGuiConsole::getCommandSuggestions("");
    check(candidates.size() == 6, "empty prefix discovers built-ins and app commands");
    check(providerCalls == 0, "argument providers are not called for root discovery");
    check(std::is_sorted(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
        return a.command < b.command;
    }), "suggestions sorted");
    candidates = ImGuiConsole::getCommandSuggestions("  p");
    check(candidates.size() == 2 && candidates[0].command == "playback" && candidates[1].command == "protocol",
          "leading whitespace and ambiguous root prefix");
    candidates = ImGuiConsole::getCommandSuggestions("view con");
    check(candidates.size() == 2 && candidates[0].command == "view console" && candidates[1].command == "view controls",
          "provider results filtered, sorted, and deduplicated");
    check(candidates[0].hint == "view <name> [show|hide|toggle]", "usage hint retained");
    check(ImGuiConsole::getCommandSuggestions("view ").size() == 3, "foreign and partial command names rejected");
    check(ImGuiConsole::getCommandSuggestions("missing").empty(), "unknown prefix has no matches");
    check(ImGuiConsole::getCommandSuggestions("PLAY").empty(), "matching is case sensitive like dispatch");

    ImGuiConsole::executeCommand("   playback pause");
    check(executions == 1 && executed == "playback pause", "dispatch trims leading whitespace and preserves arguments");
    ImGuiConsole::executeCommand("playback-extra");
    check(executions == 1, "partial command names never execute");
    check(ILog::LoggerInternal::get().messageLog.back().second == ILOG_LOG_TYPE_WARNING, "unknown command is reported");
    ImGuiConsole::executeCommand(" \t ");
    check(executions == 1, "blank command is ignored");
    ILog::LoggerInternal::get().messageLog.clear();

    ConsoleHarness ui;
    ui.control(ImGuiKey_Space);
    check(ui.text().empty(), "Ctrl+Space leaves input unchanged");
    check(ui.suggestions() != nullptr, "Ctrl+Space opens command discovery on empty input");

    ui.type("pla");
    check(ui.suggestions() != nullptr, "typing shows suggestions automatically");
    ui.key(ImGuiKey_Tab);
    check(ui.text() == "playback ", "Tab completes unique command and appends a space");
    check(executions == 1, "completion never executes");
    ui.type("pause");
    ui.key(ImGuiKey_Enter);
    check(executions == 2 && executed == "playback pause" && ui.submitted, "Enter executes completed input");
    ui.frame();
    check(ui.text().empty(), "submission clears the input");

    ui.type("draft");
    ui.key(ImGuiKey_UpArrow);
    check(ui.text() == "playback pause", "Up recalls history");
    ui.key(ImGuiKey_DownArrow);
    check(ui.text() == "draft", "Down restores unsent draft");

    ui.replace("p");
    ui.key(ImGuiKey_Tab);
    check(ui.text() == "p", "ambiguous completion does not choose arbitrarily");
    ui.key(ImGuiKey_Tab);
    check(ui.text() == "playback ", "second Tab selects first candidate");
    ui.key(ImGuiKey_Tab);
    check(ui.text() == "protocol ", "repeated Tab cycles original matches");
    ui.key(ImGuiKey_Tab);
    check(ui.text() == "playback ", "completion cycling wraps");

    ui.replace("view c");
    ui.key(ImGuiKey_Tab);
    check(ui.text() == "view con", "Tab extends shared subcommand prefix");
    ui.key(ImGuiKey_Tab);
    check(ui.text() == "view console ", "Tab cycles subcommands");
    ui.key(ImGuiKey_Tab);
    check(ui.text() == "view controls ", "Tab retains sibling subcommands after insertion");

    ui.replace("vieXYZ controls show");
    ui.key(ImGuiKey_Home);
    ui.key(ImGuiKey_RightArrow);
    ui.key(ImGuiKey_RightArrow);
    ui.key(ImGuiKey_RightArrow);
    ui.key(ImGuiKey_Tab);
    check(ui.text() == "view controls show", "completion at cursor replaces token suffix and preserves later arguments");

    ui.replace("long ");
    ui.key(ImGuiKey_Tab);
    check(ui.text() == longCandidate + " ", "completion grows std::string beyond original capacity");

    ui.replace("view f");
    ui.clickFirstSuggestion();
    check(ui.text() == "view fonts ", "mouse selection fills input and restores focus (actual: " + ui.text() + ")");
    check(ImGui::GetCurrentContext()->ActiveId != 0, "input remains focused after mouse completion");
    check(executions == 2, "mouse selection does not execute");

    // Multi-byte shared prefixes must not leave invalid UTF-8 in the input.
    ImGuiConsole::addCommand({"accent", "accent <word>", execute, [](const std::string&) {
        return std::vector<std::string>{"accent \xc3\xa9" "clair", "accent \xc3\xaa" "cole"};
    }});
    ui.replace("accent ");
    ui.key(ImGuiKey_Tab);
    check(ui.text() == "accent ", "shared prefix cannot split a UTF-8 code point");
    ui.key(ImGuiKey_Tab);
    check(ui.text() == "accent \xc3\xa9" "clair ", "cycling inserts the complete UTF-8 candidate");

    ImGuiConsole::addCommand({"view console", "specific command", [&](const std::string&) { executions += 10; }});
    ImGuiConsole::executeCommand("view console show");
    check(executions == 12, "longest registered command wins on a token boundary");
    std::cout << "Console completion and headless keyboard/mouse checks passed.\n";
}
