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

		// Register some components for testing, not sure where to do this normally
		RegisterComponent<TransformComponent>();
		RegisterComponent<RenderComponent>();

		// Test out registering some components
		EntityHandle handle;
		for (int i = 0; i < 1500; i++)
			AddComponent<TransformComponent>(handle);
	}

	void WorldSystem::Shutdown()
	{
		LOG_INFO(WORLD, "Shutdown Game World System");
	}

	Entity& WorldSystem::CreateEntity()
	{
		return (*new Entity());
	}

	void WorldSystem::DestroyEntity(const EntityHandle& handle)
	{

	}

	Entity& WorldSystem::GetEntity(const EntityHandle& handle)
	{
		return (*new Entity());
	}

	void WorldSystem::OnDrawGUI()
	{
		ImGui::Begin("World System Debug");

		m_components.DrawComponentPools();

		ImGui::End();
	}
}