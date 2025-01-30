#pragma once

#include "Systems/EngineSystem.h"

#include "GUI/DebugGUISubscriber.h"
#include "Handle.h"
#include "ObjectPool.h"
#include "Systems/World/EntitySubsystem.h"

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
		ComponentType& AddComponent(const EntityHandle& handle, ComponentType component)
		{
			return ComponentType();
		}

		void RemoveComponent(const EntityHandle& entityHandle, const ComponentHandle& componentHandle)
		{
			return;
		}

		template <typename ComponentType>
		ComponentType& GetComponent(const EntityHandle& entityHandle)
		{
			return ComponentType();
		}
	};
}