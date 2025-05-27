#include "Systems/LogSystem.h"

#include "Systems/InputSystem.h"
#include "GUI/Editor.h"
#include "Engine.h"

#include "imgui.h"

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
				std::string_view logLevelStr(GetLogLevelName(level));
				tag << std::vformat("[{}]", std::make_format_args(logLevelStr));
			}
			if (m_bShowChannel)
			{
				std::string_view logChannelStr(GetLogChannelName(channel));
				tag << std::vformat("[{}]", std::make_format_args(logChannelStr));
			}

			std::string tagStr(tag.str());
			message = std::vformat("{} {}\n",
				std::make_format_args(tagStr, msg)
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
			// Reset log every x msgs TODO: store to file here or something reasonable
			if (m_log.size() >= 1000)
			{
				m_log.clear();
				LOG_WARN(INPUT, "Exceeded Log Storage! Clearing.");
			}

			LogElement elem(level, channel, message);
			m_log.push_back(elem);
		}
	}

	void LogSystem::Startup()
	{
		g_log = this;

		LOG(LOGGER, "Startup");

#ifdef CDEBUG
		m_debugger = new LogSystemDebug(this);
		std::shared_ptr<DebugSystem> DS = m_engine->GetSystem<DebugSystem>();
		if (DS)
		{
			DS->Subscribe(m_debugger);
		}
#endif
	}

	void LogSystem::Shutdown()
	{
		LOG(LOGGER, "Shutdown");

		g_log = nullptr;

#ifdef CDEBUG
		delete m_debugger;
#endif
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

		if (ImGui::BeginCombo("Channel Filter", currentSelection.c_str()))
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
	
	void LogSystemDebug::OnDrawGUI()
	{
		static bool s_bLogInfo = true;
		static bool s_bLogWarning = true;
		static bool s_bLogError = true;
	
		static std::bitset<static_cast<size_t>(LogChannel::MAX)> selectedChannels = {17};
	
		if (m_owner == nullptr) { return; }

		ImGui::SetNextWindowSize(ImVec2(800.f, 400.f), ImGuiCond_Appearing);
		ImGui::Begin("Log");

		// Making right-aligned things in ImGui not the best...
		// Somehow lost 13 pixels to something?
		float availableRoom = ImGui::GetContentRegionAvail().x;
		float spacing = 5.f;
		float buttonWidths = 13.f + spacing + ImGui::CalcTextSize("Channel Filter").x + spacing + ImGui::CalcTextSize("Select All").x + spacing + ImGui::CalcTextSize("Clear").x;
		float selectorWidth = availableRoom - buttonWidths;

		ImGui::SetNextItemWidth(selectorWidth);
		DrawChannelSelector(selectedChannels);

		ImGui::SameLine(0.f, spacing);
		if (ImGui::Button("Select All"))
		{
			selectedChannels.set();
		}
		ImGui::SameLine(0.f, spacing);
		if (ImGui::Button("Clear"))
		{
			selectedChannels.reset();
		}

		ImGui::Text("Level Filter:");
		ImGui::SameLine();

		ImGui::Checkbox("Info", &s_bLogInfo);
		ImGui::SameLine();
		ImGui::Checkbox("Warnings", &s_bLogWarning);
		ImGui::SameLine();
		ImGui::Checkbox("Errors", &s_bLogError);
		
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 90.f);
		ImGui::SetNextItemWidth(100.f);
		if (ImGui::BeginCombo("##dropdown", "Settings"))
		{			
			ImGui::SeparatorText("Tag Settings");
			ImGui::Checkbox("Error Level Tag", &(m_owner->m_bShowLevel));
			ImGui::Checkbox("Channel Tag", &(m_owner->m_bShowChannel));
			ImGui::Checkbox("Timestamp Tag", &(m_owner->m_bShowTimestamp));
			ImGui::SeparatorText("Other");
			ImGui::Checkbox("Debug Break", &CE::Globals::bDebugBreakOnError);
			ImGui::EndCombo();
		}

		ImGui::SeparatorText("Log");
		ImGui::BeginChild("Log", ImVec2(0,0), true);

		for (LogSystem::LogElement& elem : m_owner->m_log)
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