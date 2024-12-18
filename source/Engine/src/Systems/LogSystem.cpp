#include "Systems/LogSystem.h"

#include "imgui.h"
#include <iostream>

namespace CE
{
	void LogSystem::Log(LogLevel level, LogChannel channel, std::string message)
	{
		std::cout << "[" << GetLogLevelName(level) << "]";
		std::cout << "[" << GetLogChannelName(channel) << "]";
		std::cout << message << std::endl;
	}

	void LogSystem::Startup()
	{
		g_log = this;

		LogInfo(LogChannel::LOG_SYSTEM, "Startup");
	}

	void LogSystem::Shutdown()
	{
		LogInfo(LogChannel::LOG_SYSTEM, "Shutdown");

		g_log = nullptr;
	}

	void LogSystem::DrawGUI()
	{
		if (!m_showDebug)
			return;

		ImGui::SetNextWindowSize(ImVec2(700.f, 400.f), ImGuiCond_Appearing);
		ImGui::Begin("Log");

		ImGui::Text("This is where the logging system will go");

		ImGui::End();
	}
}