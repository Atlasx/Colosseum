#pragma once

#include "Systems/EngineSystem.h"

namespace CE
{
	class LogSystem final : public EngineSystem
	{
	public:

	private:


		/* EngineSystem Interface */
	public:
		virtual std::string Name() const override { return "Log System"; }
		virtual void DrawGUI() override;

		LogSystem(Engine* engine) : EngineSystem(engine) {};

	private:
		friend class Engine;
		virtual void Startup() override;
		virtual void Shutdown() override;
	};

}