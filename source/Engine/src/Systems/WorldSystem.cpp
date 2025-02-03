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
#include "stdlibincl.h"
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
		std::array<EntityHandle, 15> someEntities{};
		for (int en = 0; en < 15; en++)
		{
			Entity& entity = CreateEntity();
			someEntities[en] = entity.m_handle;
		}
		for (int i = 0; i < 1500; i++)
		{
			AddComponent<TransformComponent>(someEntities[i % 15]);
		}
	}

	void WorldSystem::Shutdown()
	{
		LOG_INFO(WORLD, "Shutdown Game World System");
	}

	Entity& WorldSystem::CreateEntity()
	{
		EntityHandle handle = m_entities.CreateEntity();
		return m_entities.Get(handle);
	}

	void WorldSystem::DestroyEntity(const EntityHandle& handle)
	{
		m_entities.DestroyEntity(handle);
	}

	Entity& WorldSystem::GetEntity(const EntityHandle& handle)
	{
		return m_entities.Get(handle);
	}

	void WorldSystem::OnDrawGUI()
	{
		ImGui::Begin("World System Debug");

		m_components.DrawComponentPools();

		m_entities.DrawEntityPool();

		ImGui::End();
	}
}