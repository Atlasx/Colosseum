#pragma once

#include "stdlibincl.h"
#include "Handle.h"
#include "ObjectPool.h"
#include "Systems/LogSystem.h"

namespace CE
{
	class Entity
	{
	public:
		EntityHandle m_handle;

		Entity() : m_handle(EntityHandle::INVALID) {}

		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;

		Entity(Entity&&) = delete;
		Entity& operator=(Entity&&) = delete;

		void OnBegin() {}
		void Update(float dt) {}

		void AddComponent(const ComponentHandle& component)
		{
			if (std::find(m_components.begin(), m_components.end(), component) != m_components.end())
			{
				LOG_WARN(ENTITY, "Tried adding a component already registered to this entity.");
				return;
			}
			m_components.push_back(component);

			// TODO notify entity and component of add
		}

		void RemoveComponent(const ComponentHandle& component)
		{
			m_components.erase(std::remove(m_components.begin(), m_components.end(), component), m_components.end());
			
			// TODO notify entity and component of removal
		}

		std::vector<ComponentHandle>& GetComponentList()
		{
			return m_components;
		}

	private:
		std::vector<ComponentHandle> m_components;
	};

	class EntitySubsystem
	{
		using EntityPool = ObjectPool<Entity, 100000, EntityHandle>;

	public:
		EntitySubsystem()
		{
			m_entityPool = std::make_unique<EntityPool>();
		}

		EntityHandle CreateEntity()
		{
			EntityHandle handle = m_entityPool->Create();
			Entity& entity = m_entityPool->Get(handle);
			entity.m_handle = handle;
			return handle;
		}

		void DestroyEntity(const EntityHandle& handle)
		{
			m_entityPool->Destroy(handle);
		}

		Entity& Get(const EntityHandle& handle)
		{
			return m_entityPool->Get(handle);
		}

		void AddComponentToEntity(const EntityHandle& handle, const ComponentHandle& component)
		{
			auto& entity = Get(handle);
			entity.AddComponent(component);
		}

		void RemoveComponentFromEntity(const EntityHandle& handle, const ComponentHandle& component)
		{
			auto& entity = Get(handle);
			entity.RemoveComponent(component);
		}

		void DrawEntityPool();

	private:
		std::unique_ptr<EntityPool> m_entityPool;
	};
}