#pragma once

#include "Systems/EngineSystem.h"

namespace CE
{

	class ObjectSystem final : public EngineSystem
	{
	public:

	private:


		/* EngineSystem Interface */
	public:
		virtual std::string Name() const override { return "Object System"; }
		virtual void DrawGUI() override;

		ObjectSystem(Engine* engine) : EngineSystem(engine) {};

	private:
		friend class Engine;
		virtual void Startup() override;
		virtual void Shutdown() override;
	};
}