#include "Systems/LogSystem.h"

#include "Systems/InputSystem.h"

#include "imgui.h"
#include <iostream>
#include <format>
#include <string>
#include <chrono>
#include <sstream>
#include <bitset>

namespace CE
{
	LogSystem* g_log = nullptr;

	void LogSystem::LogImpl(LogLevel level, LogChannel channel, std::string_view msg)
	{
		std::string message;
		try {
			std::stringstream tag;
			if (m_bShowTimestamp)
			{
				auto now = std::chrono::system_clock::now();
				auto time = std::chrono::system_clock::to_time_t(now);

				tag << std::put_time(std::localtime(&time), "[%Y-%m-%d %H:%M:%S]");
			}
			if (m_bShowLevel)
			{
				tag << std::vformat("[{}]", std::make_format_args(GetLogLevelName(level)));
			}
			if (m_bShowChannel)
			{
				tag << std::vformat("[{}]", std::make_format_args(GetLogChannelName(channel)));
			}

			message = std::vformat("{} {}\n",
				std::make_format_args(tag.str(), msg)
			);
		}
		catch (const std::format_error& e)
		{
			std::cerr << "Formatting Issue! Please resolve this\n";
			return;
		}
		catch (...)
		{
			std::cerr << "Something went wrong when logging!\n";
			return;
		}

		if (m_bLogToCout)
		{
			std::cout << message;
		}

		if (m_bLogToLog)
		{
			LogElement elem(level, channel, message);
			m_log.push_back(elem);
		}
	}

	void LogSystem::Startup()
	{
		g_log = this;

		LOG(LOGGER, "Startup");
	}

	void LogSystem::Shutdown()
	{
		LOG(LOGGER, "Shutdown");

		g_log = nullptr;
	}

	constexpr static ImVec4 GetWarningColor(LogLevel level)
	{
		switch (level)
		{
		case WARNING: return ImVec4(1.f, 0.94f, 0.f, 1.f);
		case ERROR: return ImVec4(0.95f, 0.15f, 0.07f, 1.f);
		case INFO: 
		default: return ImVec4(1.f, 1.f, 1.f, 1.f);
		}
	}

	static void DrawChannelSelector(std::bitset<static_cast<size_t>(LogChannel::MAX)>& selectedChannels)
	{
		std::string currentSelection;
		if (selectedChannels.all())
		{
			currentSelection = "All";
		}
		else if (selectedChannels.none())
		{
			currentSelection = "None";
		}
		else
		{
			for (int i = 0; i < static_cast<int>(LogChannel::MAX); i++)
			{
				if (selectedChannels.test(i))
				{
					if (!currentSelection.empty()) currentSelection += ", ";
					currentSelection += GetLogChannelName(static_cast<LogChannel>(i));
				}
			}
		}

		if (ImGui::BeginCombo("Log Channels", currentSelection.c_str()))
		{
			for (int i = 0; i < static_cast<int>(LogChannel::MAX); i++)
			{
				LogChannel channel = static_cast<LogChannel>(i);
				bool isSelected = selectedChannels.test(i);
				if (ImGui::Checkbox(GetLogChannelName(channel), &isSelected))
				{
					selectedChannels.set(i, isSelected);
				}
			}
			ImGui::EndCombo();
		}
	}

	void LogSystem::DrawGUI()
	{
		static bool s_bLogInfo = true;
		static bool s_bLogWarning = true;
		static bool s_bLogError = true;
		
		static std::bitset<static_cast<size_t>(LogChannel::MAX)> selectedChannels = {1};
		
		if (!m_showDebug)
			return;
		
		ImGui::SetNextWindowSize(ImVec2(700.f, 400.f), ImGuiCond_Appearing);
		ImGui::Begin("Log");

		DrawChannelSelector(selectedChannels);

		ImGui::SameLine();
		if (ImGui::Button("Select All"))
		{
			selectedChannels.set();
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			selectedChannels.reset();
		}

		ImGui::Checkbox("Info", &s_bLogInfo);
		ImGui::SameLine();
		ImGui::Checkbox("Warnings", &s_bLogWarning);
		ImGui::SameLine();
		ImGui::Checkbox("Errors", &s_bLogError);

		ImGui::BeginChild("Log");

		for (LogElement elem : m_log)
		{
			bool bMatchLevel = (elem.level == LogLevel::INFO && s_bLogInfo) ||
				(elem.level == LogLevel::WARNING && s_bLogWarning) ||
				(elem.level == LogLevel::ERROR && s_bLogError);
			bool bMatchChannel = selectedChannels.test(static_cast<size_t>(elem.channel));
				
			if (bMatchChannel && bMatchLevel)
			{
				ImGui::TextColored(GetWarningColor(elem.level), elem.message.c_str());
			}
		}

		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
			ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();
		ImGui::End();
	}
}