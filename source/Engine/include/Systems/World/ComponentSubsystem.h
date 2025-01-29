#pragma once

#include "ObjectPool.h"
#include "Globals.h"

#include <glm/glm.hpp>

namespace CE
{
	// Base class for components CRTP access for common funcs
	template <typename Derived>
	class Component
	{
	public:
		void IUpdate(float dt)
		{
			static_cast<Derived*>(this)->Update(dt);
		}
	};

	class TransformComponent : public Component<TransformComponent>
	{
	public:
		glm::mat4 transform;

		glm::vec3 Position()
		{
			return transform[3];
		}

		glm::vec3 Scale()
		{
			glm::vec3 scale;
			scale.x = glm::length(glm::vec3(transform[0]));
			scale.y = glm::length(glm::vec3(transform[1]));
			scale.z = glm::length(glm::vec3(transform[2]));
			return scale;
		}
	};

	class IComponentPool
	{
	public:
		virtual ~IComponentPool() = default;
	};

	template <typename ComponentType>
	class ComponentPool : public IComponentPool
	{
		using ComponentPoolType = ObjectPool<ComponentType, 2048, ComponentHandle>;

	public:
		void Add();
		void Remove();
		ComponentType& Get();
	private:
		ComponentPoolType m_pool;
	};

	// Component
	class ComponentSubsystem
	{
	public:

		template <typename ComponentType>
		void RegisterComponent()
		{
			const std::type_index index = typeid(ComponentType);
			if (m_componentStorage.find(index) != m_componentStorage.end())
			{
				return;
			}
			auto newPool = std::make_shared<ComponentPool<ComponentType>>();
			m_componentStorage.insert({ index, newPool });
		}
	private:
		std::unordered_map<std::type_index, std::shared_ptr<IComponentPool>> m_componentStorage;
	
		template<typename T>
		std::shared_ptr<ComponentPool<T>> GetComponentPool()
		{
			std::type_index typeIndex = typeid(T);

			return std::static_pointer_cast<ComponentPool<T>>(m_componentStorage[typeIndex]);
		}
	};
}

