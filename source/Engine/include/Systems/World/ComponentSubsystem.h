#pragma once

#include "ObjectPool.h"
#include "Globals.h"
#include "Systems/LogSystem.h"

#include <glm/glm.hpp>

namespace CE
{
	class Component
	{
	public:
		ComponentHandle m_handle;
	};

	class TransformComponent : public Component
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

	class RenderComponent : public Component
	{
	public:
		// Rendering Data here
		int woof;
	};

	struct PoolInfo
	{
		std::size_t currentCount;
		std::size_t maxCount;

		PoolInfo() : currentCount(0), maxCount(0) {}
	};

	class IComponentPool
	{
	public:
		virtual ~IComponentPool() = default;
		virtual PoolInfo GetPoolInfo() { return PoolInfo(); }
		virtual void Remove(const ComponentHandle& handle) = 0;
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

		PoolInfo GetPoolInfo() override
		{
			PoolInfo pi;
			pi.maxCount = m_pool.Capacity();
			pi.currentCount = m_pool.GetFill();
			return pi;
		}

		ComponentType* Add()
		{
			ComponentHandle compHandle = m_pool.CreateWithType(m_componentID, 0);
			ComponentType& comp = m_pool.Get(compHandle);
			comp.m_handle = compHandle;
			return &comp;
		}

		void Remove(const ComponentHandle& handle) override
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
			const std::type_info& componentInfo = typeid(ComponentType);
			const std::type_index index = std::type_index(componentInfo);
			if (m_componentStorage.find(index) != m_componentStorage.end()) { return; }

			// Create the type_index to ID map here, components in order of registering
			const std::uint8_t componentID = m_componentIDs.size();
			m_componentIDs.push_back(index);
			m_componentNames.push_back(std::string(typeid(ComponentType).name()));

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

		void RemoveComponent(const ComponentHandle& handle)
		{
			if (!handle.IsValid()) { return; }
			const std::type_index cType = m_componentIDs[handle.GetType()];
			if (m_componentStorage.find(cType) == m_componentStorage.end()) { return; }

			std::shared_ptr<IComponentPool> pool = m_componentStorage[cType];
			if (pool)
			{
				pool->Remove(handle);
			}
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
		std::vector<std::string> m_componentNames;
		std::unordered_map<std::type_index, std::shared_ptr<IComponentPool>> m_componentStorage;

		template<typename T>
		std::shared_ptr<ComponentPool<T>> GetComponentPool()
		{
			std::type_index typeIndex = typeid(T);

			return std::static_pointer_cast<ComponentPool<T>>(m_componentStorage[typeIndex]);
		}
	};
}

