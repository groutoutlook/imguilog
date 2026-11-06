#include "ILogImGui.hpp"
#ifdef ILOG_IMGUI
#	include "utils/imgui_stdlib.h"
#	include <algorithm>
#	include <cctype>
#	include <vector>

namespace
{
struct ConsoleCommandHistory
{
	std::vector<std::string> commands;
	std::size_t              position{0};
	std::string              draft;
};

ConsoleCommandHistory console_command_history;
constexpr std::size_t max_console_history = 100;

struct ConsoleCompletion
{
	std::string input;
	int cursor{0};
	int replaceEnd{0};
	std::vector<ILog::CommandSuggestion> suggestions;
	int selected{-1};
	bool cycling{false};
	bool visible{false};
	bool focusInput{false};
	bool insertPending{false};
	std::string pending;
};

ConsoleCompletion console_completion;

bool is_space(char character)
{
	return std::isspace(static_cast<unsigned char>(character)) != 0;
}

std::string trim_start(const std::string& input)
{
	const auto first = input.find_first_not_of(" \t\r\n\f\v");
	return first == std::string::npos ? std::string{} : input.substr(first);
}

bool matches_command(const std::string& input, const std::string& name)
{
	return !name.empty() && input.compare(0, name.size(), name) == 0 &&
	       (input.size() == name.size() || is_space(input[name.size()]));
}

void remember_input(ImGuiInputTextCallbackData* data)
{
	auto& completion = console_completion;
	completion.input.assign(data->Buf, static_cast<std::size_t>(data->BufTextLen));
	completion.cursor = data->CursorPos;
	completion.replaceEnd = data->CursorPos;
	while (completion.replaceEnd < data->BufTextLen && !is_space(data->Buf[completion.replaceEnd]))
		++completion.replaceEnd;
}

void replace_completion(ImGuiInputTextCallbackData* data, const std::string& candidate, bool finish)
{
	// Replace through the end of the token under the cursor, preserving later arguments.
	int first = 0;
	while (first < data->CursorPos && is_space(data->Buf[first]))
		++first;
	std::string replacement = candidate;
	if (finish && (console_completion.replaceEnd == data->BufTextLen ||
	               !is_space(data->Buf[console_completion.replaceEnd])))
		replacement += ' ';
	data->DeleteChars(first, console_completion.replaceEnd - first);
	data->InsertChars(first, replacement.c_str());
	data->CursorPos = first + static_cast<int>(replacement.size());
	data->SelectionStart = data->SelectionEnd = data->CursorPos;
	remember_input(data);
}

void refresh_suggestions(ImGuiInputTextCallbackData* data)
{
	remember_input(data);
	auto& completion = console_completion;
	completion.suggestions = ILog::ImGuiConsole::getCommandSuggestions(
	    completion.input.substr(0, static_cast<std::size_t>(completion.cursor)));
	completion.selected = -1;
	completion.cycling = false;
}

int console_input_callback(ImGuiInputTextCallbackData* data)
{
	auto& completion = console_completion;
	if (completion.insertPending)
	{
		replace_completion(data, completion.pending, true);
		completion.insertPending = false;
		refresh_suggestions(data);
	}

	if (completion.input != data->Buf || completion.cursor != data->CursorPos)
	{
		refresh_suggestions(data);
		completion.visible = !trim_start(completion.input).empty();
	}

	if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion)
	{
		if (!completion.cycling)
		{
			refresh_suggestions(data);
			if (completion.suggestions.size() == 1)
			{
				replace_completion(data, completion.suggestions.front().command, true);
				refresh_suggestions(data);
			}
			else if (!completion.suggestions.empty())
			{
				std::string common = completion.suggestions.front().command;
				for (const auto& suggestion : completion.suggestions)
				{
					std::size_t length = 0;
					while (length < common.size() && length < suggestion.command.size() &&
					       common[length] == suggestion.command[length])
						++length;
					common.resize(length);
				}
				// Never split a UTF-8 code point when completing a shared prefix.
				while (!common.empty() && common.size() < completion.suggestions.front().command.size() &&
				       (static_cast<unsigned char>(completion.suggestions.front().command[common.size()]) & 0xc0) == 0x80)
					common.pop_back();
				if (common.size() > trim_start(completion.input.substr(0, completion.cursor)).size())
					replace_completion(data, common, false);
				completion.cycling = true;
			}
		}
		else if (!completion.suggestions.empty())
		{
			completion.selected = (completion.selected + 1) % static_cast<int>(completion.suggestions.size());
			replace_completion(data, completion.suggestions[completion.selected].command, true);
		}
		completion.visible = true;
	}
	else if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
	{
		auto& history = console_command_history;
		if (data->EventKey == ImGuiKey_UpArrow)
		{
			if (history.commands.empty())
				return 0;
			if (history.position == history.commands.size())
				history.draft.assign(data->Buf, static_cast<std::size_t>(data->BufTextLen));
			if (history.position > 0)
				--history.position;
		}
		else if (data->EventKey == ImGuiKey_DownArrow)
		{
			if (history.position == history.commands.size())
				return 0;
			++history.position;
		}
		else
			return 0;
		const auto& replacement = history.position == history.commands.size() ? history.draft : history.commands[history.position];
		data->DeleteChars(0, data->BufTextLen);
		data->InsertChars(0, replacement.c_str());
		data->CursorPos = data->BufTextLen;
		data->SelectionStart = data->SelectionEnd = data->CursorPos;
		refresh_suggestions(data);
		completion.visible = !replacement.empty();
	}
	else if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Space, false))
	{
		refresh_suggestions(data);
		completion.visible = true;
	}
	return 0;
}

void remember_command(const std::string& command)
{
	auto& history = console_command_history;
	if (!trim_start(command).empty() && (history.commands.empty() || history.commands.back() != command))
	{
		history.commands.emplace_back(command);
		if (history.commands.size() > max_console_history)
			history.commands.erase(history.commands.begin());
	}
	history.position = history.commands.size();
	history.draft.clear();
}
}

void ILog::ImGuiConsole::setLogColour(const ImVec4 colour, const LogType type) noexcept
{
	switch (type)
	{
	case ILOG_LOG_TYPE_WARNING:
		warning = colour;
		return;
	case ILOG_LOG_TYPE_ERROR:
		error = colour;
		return;
	case ILOG_LOG_TYPE_NOTE:
		note = colour;
		return;
	case ILOG_LOG_TYPE_SUCCESS:
		success = colour;
		return;
	case ILOG_LOG_TYPE_MESSAGE:
		message = colour;
		return;
	}
}
void ILog::ImGuiConsole::executeCommand(const std::string& cmd) noexcept
{
	auto& logger = LoggerInternal::get();
	const auto input = trim_start(cmd);
	if (input.empty())
		return;
	const CommandType* match = nullptr;
	for (const auto& a : logger.commands)
	{
		if (matches_command(input, a.cmd) && (!match || a.cmd.size() > match->cmd.size()))
			match = &a;
	}
	if (match)
	{
		const auto execute = match->func;
		if (execute)
			execute(input);
	}
	else
		addToMessageLog("Unknown command: " + input + ". Type help or press Ctrl+Space for commands.", ILOG_LOG_TYPE_WARNING);
}

std::vector<ILog::CommandSuggestion> ILog::ImGuiConsole::getCommandSuggestions(const std::string& input)
{
	const auto prefix = trim_start(input);
	std::vector<CommandSuggestion> suggestions;
	for (const auto& command : LoggerInternal::get().commands)
	{
		if (command.cmd.empty())
			continue;
		if (command.cmd.compare(0, prefix.size(), prefix) == 0)
			suggestions.push_back({command.cmd, command.cmdHint});
		if (command.complete && prefix.size() > command.cmd.size() && matches_command(prefix, command.cmd))
		{
			for (const auto& candidate : command.complete(prefix))
				if (matches_command(candidate, command.cmd) && candidate.compare(0, prefix.size(), prefix) == 0)
					suggestions.push_back({candidate, command.cmdHint});
		}
	}
	std::sort(suggestions.begin(), suggestions.end(), [](const auto& left, const auto& right) {
		return left.command < right.command;
	});
	suggestions.erase(std::unique(suggestions.begin(), suggestions.end(), [](const auto& left, const auto& right) {
		return left.command == right.command;
	}), suggestions.end());
	return suggestions;
}

void ILog::ImGuiConsole::display(bool* bInteractingWithTextbox) const noexcept
{
	auto& logger = LoggerInternal::get();
	for (const auto& a : logger.messageLog)
	{
		ImVec4 colour;
		switch (a.second)
		{
		case ILOG_LOG_TYPE_WARNING:
			colour = warning;
			break;
		case ILOG_LOG_TYPE_ERROR:
			colour = error;
			break;
		case ILOG_LOG_TYPE_NOTE:
			colour = note;
			break;
		case ILOG_LOG_TYPE_SUCCESS:
			colour = success;
			break;
		case ILOG_LOG_TYPE_MESSAGE:
			colour = message;
			break;
		}

		ImGui::TextColored(colour, "%s", a.first.c_str());
	}

	static std::string command;
	if (console_completion.focusInput)
	{
		ImGui::SetKeyboardFocusHere();
		console_completion.focusInput = false;
	}
	const bool entered = ImGui::InputTextWithHint(
	        "##Input",
	        "Command (Tab to complete, Ctrl+Space to browse)",
	        &command,
	        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory |
	            ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackAlways,
	        console_input_callback);
	const bool inputActive = ImGui::IsItemActive();
	const bool inputDeactivated = ImGui::IsItemDeactivated();
	if (entered)
		ImGui::SetKeyboardFocusHere(-1);
	ImGui::SameLine();
	const bool sent = ImGui::Button("Send##consoleCommand");
	if (entered || sent)
	{
		remember_command(command);
		executeCommand(command);
		command.clear();
		console_completion = {};
		console_completion.focusInput = sent;
		if (bInteractingWithTextbox != nullptr)
			*bInteractingWithTextbox = true;
	}
	else if (console_completion.visible &&
	         (inputActive || inputDeactivated || ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)))
	{
		ImGui::TextDisabled("Tab: complete/cycle | Ctrl+Space: browse | Up/Down: history");
		const auto prefix = trim_start(console_completion.input.substr(0, console_completion.cursor));
		for (const auto& registered : logger.commands)
			if (matches_command(prefix, registered.cmd))
			{
				ImGui::TextWrapped("%s", registered.cmdHint.c_str());
				break;
			}
		const auto& suggestions = console_completion.suggestions;
		if (suggestions.empty())
			ImGui::TextDisabled("No matching completions. Type help to list commands.");
		else
		{
			const auto rows = (std::min)(suggestions.size(), std::size_t{6});
			if (ImGui::BeginChild("##CommandSuggestions", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * rows + 8),
			                      false, ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus))
			{
				for (std::size_t index = 0; index < suggestions.size(); ++index)
				{
					const auto& suggestion = suggestions[index];
					ImGui::PushID(static_cast<int>(index));
					const bool selected = console_completion.selected == static_cast<int>(index);
					const float commandWidth = ImGui::GetContentRegionAvail().x * 0.35f;
					if (ImGui::Selectable(suggestion.command.c_str(), selected, 0, ImVec2(commandWidth, 0)))
					{
						console_completion.pending = suggestion.command;
						console_completion.insertPending = true;
						console_completion.focusInput = true;
					}
					if (selected)
						ImGui::SetScrollHereY();
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 40);
						ImGui::TextUnformatted(suggestion.command.c_str());
						ImGui::TextUnformatted(suggestion.hint.c_str());
						ImGui::PopTextWrapPos();
						ImGui::EndTooltip();
					}
					ImGui::SameLine();
					ImGui::TextDisabled("%s", suggestion.hint.c_str());
					ImGui::PopID();
				}
			}
			ImGui::EndChild();
		}
	}
	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);
}

void ILog::ImGuiConsole::addToMessageLog(const std::string& msg, LogType type) noexcept
{
	LoggerInternal::get().messageLog.emplace_back(msg, type);
}

void ILog::ImGuiConsole::addCommand(const CommandType& cmd) noexcept
{
	LoggerInternal::get().commands.emplace_back(cmd);
}

void ILog::ImGuiConsole::showHelpMessage(const std::string&) noexcept
{
	for (const auto& a : LoggerInternal::get().commands)
		addToMessageLog(std::string(a.cmd) + " - " + a.cmdHint, ILOG_LOG_TYPE_MESSAGE);
}

void ILog::ImGuiConsole::displayFull(bool& bOpen, bool* bInteractingWithTextbox) const noexcept
{
	const auto common_flag = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
	ImGui::Begin("Developer Console", &bOpen, common_flag);
	display(bInteractingWithTextbox);
	ImGui::End();
}
#endif
