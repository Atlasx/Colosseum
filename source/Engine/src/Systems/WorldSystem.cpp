/*
* Entity System Notes
* 
* Core Data:
*	- Entity
*	- Component
* 
*/


#include "Systems/WorldSystem.h"

#include "Engine.h"
#include "Systems/LogSystem.h"
#include "GUI/Editor.h"

namespace CE
{
	void WorldSystem::Startup()
	{
		LOG_INFO(WORLD, "Startup Game World System");

		std::shared_ptr<DebugSystem> DS = m_engine->GetSystem<DebugSystem>();
		if (DS)
		{
			DS->Subscribe(this);
		}
	}

	void WorldSystem::Shutdown()
	{
		LOG_INFO(WORLD, "Shutdown Game World System");
	}

	Entity& WorldSystem::CreateEntity()
	{
		
	}

	void WorldSystem::DestroyEntity(const EntityHandle& handle)
	{

	}

	Entity& WorldSystem::GetEntity(const EntityHandle& handle)
	{
		
	}

	void WorldSystem::OnDrawGUI()
	{
		ImGui::Begin("World System Debug");

		ImGui::Text("This is where the World System Debug window will go");

		ImGui::End();
	}
}