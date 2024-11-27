#pragma once

#include <vector>
#include <string>

#include "Systems/EngineSystem.h"

namespace CE
{
	class EventSystem : public EngineSystem
	{
		// More friendship to allow Engine to access protected functions on derived class pointers
		friend class Engine;
		
	public:
		/* EngineSystem Interface */

		std::string Name() const override { return "EventSystem"; }

		EventSystem(Engine* engine) : EngineSystem(engine) {};

	protected:

		void Startup() override;
		void Shutdown() override;
	};
}