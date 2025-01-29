#pragma once

#include "Handle.h"
#include "ObjectPool.h"

namespace CE
{
	class Entity
	{
	public:
		void OnBegin() {}
		void Update(float dt) {}
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
			return m_entityPool->Create();
		}

		void DestroyEntity(const EntityHandle& handle)
		{
			m_entityPool->Destroy(handle);
		}

		Entity& Get(const EntityHandle& handle)
		{
			return m_entityPool->Get(handle);
		}

	private:
		std::unique_ptr<EntityPool> m_entityPool;
	};
}