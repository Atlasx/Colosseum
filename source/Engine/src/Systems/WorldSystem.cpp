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
#include "Globals.h"
#include "stdlibincl.h"
#include "Systems/LogSystem.h"
#include "Systems/InputSystem.h"
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
		CreateTestComponents(10);

		std::shared_ptr<InputSystem> IS = m_engine->GetSystem<InputSystem>();
		if (IS)
		{
			IS->RegisterAction("Add Test Components", KeyType::EQUAL, [&]() {
				this->OnAddTestComponents();
			});
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

	void WorldSystem::CreateTestComponents(std::size_t amount)
	{
		std::array<EntityHandle, 15> someEntities{};
		for (int en = 0; en < 15; en++)
		{
			Entity& entity = CreateEntity();
			someEntities[en] = entity.m_handle;
		}
		for (int i = 0; i < amount; i++)
		{
			TransformComponent* comp = AddComponent<TransformComponent>(someEntities[i % 15]);
			comp->SetPosition(glm::vec3(
				CE::Globals::g_rand.GetFloat(-100.f, 100.f),
				CE::Globals::g_rand.GetFloat(-100.f, 100.f),
				CE::Globals::g_rand.GetFloat(-100.f, 100.f)
			));
		}
	}

	void WorldSystem::OnAddTestComponents()
	{
		CreateTestComponents(100);
	}
}