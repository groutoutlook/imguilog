#include "ILogImGui.hpp"
#ifdef ILOG_IMGUI
#	include "utils/imgui_stdlib.h"
#	include <algorithm>
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
	for (const auto& a : logger.commands)
	{
		if (cmd.rfind(a.cmd, 0) == 0)
		{
			a.func(cmd);
			break;
		}
	}
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
	const auto history_callback = [](ImGuiInputTextCallbackData* data) -> int {
		if (data->EventFlag != ImGuiInputTextFlags_CallbackHistory)
			return 0;

		auto& history = *static_cast<ConsoleCommandHistory*>(data->UserData);
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
			if (history.position < history.commands.size())
				++history.position;
			else
				return 0;
		}
		else
		{
			return 0;
		}

		const auto& replacement = history.position == history.commands.size() ?
		                              history.draft :
		                              history.commands[history.position];
		data->DeleteChars(0, data->BufTextLen);
		data->InsertChars(0, replacement.c_str());
		data->CursorPos = data->BufTextLen;
		data->SelectionStart = data->SelectionEnd = data->CursorPos;
		return 0;
	};
	if (ImGui::InputTextWithHint(
	        "##Input",
	        "Enter any cmd here",
	        &command,
	        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory,
	        history_callback,
	        &console_command_history))
	{
		ImGui::SetKeyboardFocusHere(-1);
		if (!command.empty() && (console_command_history.commands.empty() || console_command_history.commands.back() != command))
		{
			console_command_history.commands.emplace_back(command);
			if (console_command_history.commands.size() > max_console_history)
				console_command_history.commands.erase(console_command_history.commands.begin());
		}
		console_command_history.position = console_command_history.commands.size();
		console_command_history.draft.clear();
		executeCommand(command);
		command.clear();
		if (bInteractingWithTextbox != nullptr)
			*bInteractingWithTextbox = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Send##consoleCommand"))
	{
		if (!command.empty() && (console_command_history.commands.empty() || console_command_history.commands.back() != command))
		{
			console_command_history.commands.emplace_back(command);
			if (console_command_history.commands.size() > max_console_history)
				console_command_history.commands.erase(console_command_history.commands.begin());
		}
		console_command_history.position = console_command_history.commands.size();
		console_command_history.draft.clear();
		executeCommand(command);
		command.clear();
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
