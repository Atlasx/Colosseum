#pragma once

#include "Systems/EngineSystem.h"

#include "GUI/DebugGUISubscriber.h"
#include "ObjectPool.h"

namespace CE
{
	using EntityHandle = Handle<std::uint32_t, 20, 11>;
	using ComponentHandle = Handle<std::uint32_t, 20, 11>;

	class EntitySystem final : public EngineSystem, IDebugGUISubscriber
	{
	public:
		EntitySystem(Engine* engine) : EngineSystem(engine) {};

		/* EngineSystem Interface */
	public:
		virtual std::string Name() const override { return "Entity System"; }
	protected:
		friend class Engine;
		virtual void Startup() override;
		virtual void Shutdown() override;

		/* IDebugGUISubscriber Interface */
	public:
		virtual void OnDrawGUI() override;
		virtual std::string_view GetDebugMenuName() override { return "Entity System"; }
	
		/* Entity System API */
	
	};
}