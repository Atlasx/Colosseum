#pragma once

#include "Systems/EngineSystem.h"

namespace CE
{

	class EntitySystem final : public EngineSystem
	{
	public:

	private:


		/* EngineSystem Interface */
	public:
		virtual std::string Name() const override { return "Entity System"; }

		EntitySystem(Engine* engine) : EngineSystem(engine) {};

	private:
		friend class Engine;
		virtual void Startup() override;
		virtual void Shutdown() override;
	};
}