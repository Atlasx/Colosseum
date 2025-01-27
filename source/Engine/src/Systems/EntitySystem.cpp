/*
* Entity System Notes
* 
* Core Data:
*	- Entity
*	- Component
* 
*/


#include "Systems/EntitySystem.h"

#include "Engine.h"
#include "Systems/LogSystem.h"
#include "GUI/Editor.h"

namespace CE
{
	void EntitySystem::Startup()
	{
		LOG_INFO(ENTITY, "Startup Game Entity System");

		std::shared_ptr<DebugSystem> DS = m_engine->GetSystem<DebugSystem>();
		if (DS)
		{
			DS->Subscribe(this);
		}
	}

	void EntitySystem::Shutdown()
	{
		LOG_INFO(ENTITY, "Shutdown Game Entity System");
	}

	void EntitySystem::OnDrawGUI()
	{
		ImGui::Begin("Entity System Debug");

		ImGui::Text("This is where the Entity System Debug window will go");

		ImGui::End();
	}
}