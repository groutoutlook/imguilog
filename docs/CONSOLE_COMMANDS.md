# imguilog console commands

This document describes this repository's `ILog::ImGuiConsole`. Application
commands belong to the application registering them; imguilog owns discovery,
completion, history, and dispatch.

## Keyboard and mouse

| Action | Behavior |
| --- | --- |
| Type a command prefix | Show matching registered commands and their usage hints. |
| Tab | Complete a unique match, or extend the common prefix of several matches. |
| Tab again | Cycle the original matches in alphabetical order. |
| Ctrl+Space | Refresh suggestions; an empty input lists all registered command names. |
| Click a suggestion | Fill the input and restore keyboard focus. |
| Up / Down | Browse the last 100 submitted commands; Down restores the unsent draft. |
| Enter / Send | Execute the input. Completion itself never executes anything. |
| `help` | Print all registered commands and their hints in the log. |
| `clear` | Clear the message log. |

Matching is case sensitive and uses prefixes. Completion uses the text before
the cursor, replaces through the end of that token, and preserves later
arguments. Completing a whole candidate at the end adds a space for the next
argument. Escape retains Dear ImGui's normal input cancellation behavior.

## Existing registrations

Three-field registrations continue to work:

```cpp
ILog::ImGuiConsole::addCommand({
    "reload",
    "reload - schedule a reload of current files",
    [this](const std::string& line) { execute_ui_command(line); }
});
```

`rel` + Tab becomes `reload `. Both `cmd` and `cmdHint` drive discovery; no
separate command-name list is needed. Hints are displayed as usage text, not
parsed as a grammar. Without extra metadata, an application command still has
name completion and usage hints, but no argument completion.

Dispatch requires the registered name to end at whitespace or the end of input:
`reload-other` cannot execute `reload`. Leading whitespace is ignored, blank
input is ignored, and unknown commands print a warning. When registrations
overlap, the longest matching name wins. Callbacks receive the full input with
leading whitespace removed.

## Application-provided subcommands and values

`CommandType::complete` is an optional fourth field. It receives the full command
prefix up to the cursor, without leading whitespace, and returns **full command
lines**, rather than just the next word. imguilog filters those lines by the
current prefix, checks that they belong to the registered command, removes
duplicates, and sorts them. The callback runs only after the command name and a
whitespace separator have been entered.

For example, a `sled` integration can register `view` using the same view names
as its dispatcher:

```cpp
ILog::ImGuiConsole::addCommand({
    "view",
    "view list | view <name> [show|hide|toggle]",
    [this](const std::string& line) { execute_ui_command(line); },
    [](const std::string&) {
        std::vector<std::string> candidates{"view list"};
        for (const char* name : {"controls", "console", "framebuffer", "runtime",
                                 "timeline", "projection", "obstruction", "fonts",
                                 "network", "shortcuts"})
        {
            candidates.emplace_back(std::string{"view "} + name);
            for (const char* action : {"show", "hide", "toggle"})
                candidates.emplace_back(std::string{"view "} + name + " " + action);
        }
        return candidates;
    }
});
```

This is an integration example, not a built-in `view` command. In the application,
prefer generating candidates from the same view enumeration/name function used
by execution so newly added views cannot disappear from completion. Other useful
providers include `playback` actions, `ui stats on|off|toggle`, and current scene
IDs. Numeric frame values, arbitrary JSON, and paths still need application
knowledge; imguilog does not invent values from a help string.

Callbacks run synchronously on the UI thread when input/cursor changes or the
user explicitly requests completion. Keep them fast and free of side effects;
do not execute commands, mutate the command registry, block on I/O, or return
placeholders such as `<frame>`. Capture application state with an appropriate
lifetime, just as for execution callbacks. Ctrl+Space refreshes dynamic values
even when the input has not changed. Rebuild consumers after changing this
header: the extra field is source compatible with old initializers, but changes
the C++ struct layout.

For custom front ends, query the same candidates without an ImGui context:

```cpp
auto suggestions = ILog::ImGuiConsole::getCommandSuggestions("view con");
for (const auto& suggestion : suggestions)
    consume(suggestion.command, suggestion.hint);
```

Registration, suggestion queries, execution, and display share the existing
process-wide logger/console state. Synchronize access externally if using
multiple threads. The existing console input and history are shared between
console instances.

## Focused validation

The optional test target creates an ImGui context without a rendering backend.
It checks discovery/filtering, dispatch boundaries, Tab cycling, Ctrl+Space,
history/draft restoration, cursor-aware replacement, string growth, and mouse
completion. With ImGui and GLFW available to CMake, configure a separate build:

```powershell
cmake -S . -B out/build/console-tests -G Ninja `
    -DILOG_BUILD_EXAMPLES=OFF -DILOG_BUILD_TESTS=ON
cmake --build out/build/console-tests --target imguilog_console_tests
ctest --test-dir out/build/console-tests --output-on-failure
```

On Windows, run this in a developer shell and supply your vcpkg toolchain or
package search path as needed. For a manual check, focus the console, press
Ctrl+Space, type an ambiguous prefix, press Tab repeatedly, then verify Up/Down
still restores your history and draft. An application should respect ImGui's
keyboard-capture state before dispatching its own global shortcuts.
