#include "Systems/ObjectSystem.h"

#include "imgui.h"
#include <iostream>

namespace CE
{
	void ObjectSystem::Startup()
	{
		std::cout << "Startup Game Object System" << std::endl;
	}

	void ObjectSystem::Shutdown()
	{
		std::cout << "Shutdown Game Object System" << std::endl;
	}

	void ObjectSystem::DrawGUI()
	{
		if (!m_showDebug)
			return;

		ImGui::Begin("Object System Debug");

		ImGui::Text("This is where the Object System Debug window will go");

		ImGui::End();
	}

}