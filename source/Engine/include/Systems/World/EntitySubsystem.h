#pragma once

#include "Handle.h"

namespace CE
{
	class Entity
	{
	public:
	private:
		
	};

	class EntitySubsystem
	{
	public:
		Entity& CreateEntity();
		void DestroyEntity(const EntityHandle& handle);

	private:
		ObjectPool<Entity, 100000, EntityHandle> m_entityPool;
	};
}