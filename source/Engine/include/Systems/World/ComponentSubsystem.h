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

	class RenderComponent
	{
	public:
		// Rendering Data here
		int woof;
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
		ComponentPool(std::uint8_t cID) : m_pool({})
		{
			m_componentID = cID;
		}

		ComponentType* Add()
		{
			ComponentHandle compHandle = m_pool.CreateWithType(m_componentID, 0);
			ComponentType* comp = m_pool.Get(compHandle);
			comp->m_handle = compHandle;
			return comp;
		}

		void Remove(const ComponentHandle& handle)
		{
			m_pool.Destroy(handle);
		}

		ComponentType* Get(const ComponentHandle& handle)
		{
			return &(m_pool.Get(handle));
		}

	private:
		ComponentPoolType m_pool;
		std::uint8_t m_componentID;
	};

	/*
	* ComponentSubsystem
	*	- Abstracts away the per-component object pool
	*/
	class ComponentSubsystem
	{
	public:
		ComponentSubsystem() : m_componentIDs(), m_componentStorage() {}

		template <typename ComponentType>
		void RegisterComponent()
		{
			const std::type_index index = typeid(ComponentType);
			if (m_componentStorage.find(index) != m_componentStorage.end()) { return; }

			// Create the type_index to ID map here, components in order of registering
			const std::uint8_t componentID = m_componentIDs.size();
			m_componentIDs.push_back(index);

			auto newPool = std::make_shared<ComponentPool<ComponentType>>(componentID);
			m_componentStorage.insert({ index, newPool });
		}

		template<typename T>
		T* AddComponent()
		{
			const std::type_index cType = typeid(T);
			if (m_componentStorage.find(cType) == m_componentStorage.end()) { return nullptr; }
			std::shared_ptr<ComponentPool<T>> pool = GetComponentPool<T>();
			return pool->Add();
		}
		
		template<typename T>
		T* GetComponent(const ComponentHandle& handle)
		{
			if (!handle.IsValid()) { return nullptr; }

			const std::type_index cType = m_componentIDs[handle.GetType()];
			if (m_componentStorage.find(cType) == m_componentStorage.end()) { return nullptr; }

			std::shared_ptr<ComponentPool<T>> pool = GetComponentPool<T>();
			return pool->Get(handle);
		}

		void DrawComponentPools();

	private:
		std::vector<std::type_index> m_componentIDs;
		std::unordered_map<std::type_index, std::shared_ptr<IComponentPool>> m_componentStorage;

		template<typename T>
		std::shared_ptr<ComponentPool<T>> GetComponentPool()
		{
			std::type_index typeIndex = typeid(T);

			return std::static_pointer_cast<ComponentPool<T>>(m_componentStorage[typeIndex]);
		}
	};
}

