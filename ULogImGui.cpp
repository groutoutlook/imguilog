#include "ULogImGui.hpp"
#ifdef ULOG_IMGUI
#include "utils/imgui_stdlib.h"

void ULog::ImGuiConsole::setLogColour(const ImVec4 colour, const LogType type) noexcept
{
	switch (type)
	{
	case ULOG_LOG_TYPE_WARNING:
		warning = colour;
		return;
	case ULOG_LOG_TYPE_ERROR:
		error = colour;
		return;
	case ULOG_LOG_TYPE_NOTE:
		note = colour;
		return;
	case ULOG_LOG_TYPE_SUCCESS:
		success = colour;
		return;
	case ULOG_LOG_TYPE_MESSAGE:
		message = colour;
		return;
	}
}
void ULog::ImGuiConsole::executeCommand(const std::string& cmd) noexcept
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

void ULog::ImGuiConsole::display(bool* bInteractingWithTextbox) const noexcept
{
	auto& logger = LoggerInternal::get();
	for (const auto& a : logger.messageLog)
	{
		ImVec4 colour;
		switch (a.second)
		{
		case ULOG_LOG_TYPE_WARNING:
			colour = warning;
			break;
		case ULOG_LOG_TYPE_ERROR:
			colour = error;
			break;
		case ULOG_LOG_TYPE_NOTE:
			colour = note;
			break;
		case ULOG_LOG_TYPE_SUCCESS:
			colour = success;
			break;
		case ULOG_LOG_TYPE_MESSAGE:
			colour = message;
			break;
		}

		ImGui::TextColored(colour, "%s", a.first.c_str());
	}

	static std::string command;
	if (ImGui::InputTextWithHint("##Input", "Enter any cmd here", &command, ImGuiInputTextFlags_EnterReturnsTrue,
		[](ImGuiInputTextCallbackData* data)->int {
			return 0;
		})) {
		ImGui::SetKeyboardFocusHere(-1);
		executeCommand(command);
		command.clear();
		if (bInteractingWithTextbox != nullptr) *bInteractingWithTextbox = true;
		
	}
	ImGui::SameLine();
	if (ImGui::Button("Send##consoleCommand"))
	{
		executeCommand(command);
		command.clear();
	}
	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);
}

void ULog::ImGuiConsole::addToMessageLog(const std::string& msg, LogType type) noexcept
{
	LoggerInternal::get().messageLog.emplace_back(msg, type);
}

void ULog::ImGuiConsole::addCommand(const CommandType& cmd) noexcept
{
	LoggerInternal::get().commands.emplace_back(cmd);
}

void ULog::ImGuiConsole::showHelpMessage(const std::string&) noexcept
{
	for (const auto& a : LoggerInternal::get().commands)
		addToMessageLog(std::string(a.cmd) + " - " + a.cmdHint, ULOG_LOG_TYPE_MESSAGE);
}

void ULog::ImGuiConsole::displayFull(bool& bOpen, bool* bInteractingWithTextbox) const noexcept
{
	ImGui::Begin("Developer Console", &bOpen);
	display(bInteractingWithTextbox);
	ImGui::End();
}
#endif
