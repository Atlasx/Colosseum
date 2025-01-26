/*
* Entity System Notes
* 
* Core Data:
*	- Entity
*	- Component
* 
*/


#include "Systems/EntitySystem.h"

#include "imgui.h"
#include <iostream>

#include "Systems/LogSystem.h"

namespace CE
{
	void EntitySystem::Startup()
	{
		LOG_INFO(ENTITY, "Startup Game Entity System");
	}

	void EntitySystem::Shutdown()
	{
		LOG_INFO(ENTITY, "Shutdown Game Entity System");
	}

	/*
	void EntitySystem::DrawGUI()
	{
		if (!m_showDebug)
			return;

		ImGui::Begin("Entity System Debug");

		ImGui::Text("This is where the Entity System Debug window will go");

		ImGui::End();
	}
	*/

}