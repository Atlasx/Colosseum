#include "Systems/LogSystem.h"

#include "imgui.h"
#include <iostream>

namespace CE
{
	void LogSystem::Log(LogLevel level, std::string channel, std::string message)
	{

	}

	void LogSystem::Startup()
	{
		std::cout << "LogSystem Startup" << std::endl;
	}

	void LogSystem::Shutdown()
	{
		std::cout << "LogSystem Shutdown" << std::endl;
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