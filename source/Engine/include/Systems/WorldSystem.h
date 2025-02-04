#pragma once

#include "Systems/EngineSystem.h"

#include "GUI/DebugGUISubscriber.h"
#include "Handle.h"
#include "ObjectPool.h"
#include "Systems/World/EntitySubsystem.h"
#include "Systems/World/ComponentSubsystem.h"
#include "Systems/LogSystem.h"

#define WORLD_SYSTEM "World System"

namespace CE
{
	class Entity;

	class WorldSystem final : public EngineSystem, IDebugGUISubscriber
	{
	public:
		WorldSystem(Engine* engine) : EngineSystem(engine) {};

		/* EngineSystem Interface */
	public:
		virtual std::string Name() const override { return WORLD_SYSTEM; }
	protected:
		friend class Engine;
		virtual void Startup() override;
		virtual void Shutdown() override;

		/* IDebugGUISubscriber Interface */
	public:
		virtual void OnDrawGUI() override;
		virtual std::string_view GetDebugMenuName() override { return WORLD_SYSTEM; }
	
		/* World System API */
	
		Entity& CreateEntity();
		void DestroyEntity(const EntityHandle& handle);
		Entity& GetEntity(const EntityHandle& handle);

		template <typename ComponentType>
		ComponentType* AddComponent(const EntityHandle& handle)
		{
			// Create new component
			ComponentType* component = m_components.AddComponent<ComponentType>();
			// Add component handle to entity
			m_entities.AddComponentToEntity(handle, component->m_handle);
			return component;
		}

		void RemoveComponent(const EntityHandle& entityHandle, const ComponentHandle& componentHandle)
		{
			// remove component handle from entity
			m_entities.RemoveComponentFromEntity(entityHandle, componentHandle);
			// Remove component from pools
			m_components.RemoveComponent(componentHandle);
		}

		template <typename ComponentType>
		ComponentType& GetComponent(const EntityHandle& entityHandle)
		{
			return ComponentType();
		}

		template <typename ComponentType>
		void RegisterComponent()
		{
			LOG_INFO(WORLD, "Registering Component: {}", typeid(ComponentType).name());
			m_components.RegisterComponent<ComponentType>();
		}

		template <typename ComponentType>
		void ForEachComponent(std::function<void(ComponentType*)> function)
		{
			m_components.ForEachComponent<ComponentType>(std::move(function));
		}

	private:
		ComponentSubsystem m_components;
		EntitySubsystem m_entities;
	};
}