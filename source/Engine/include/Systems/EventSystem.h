#pragma once

#include <vector>
#include <string>

#include "Systems/EngineSystem.h"

namespace CE
{
	class EventSystem : public EngineSystem
	{

		
	public:
		/* EngineSystem Interface */

		std::string Name() const override { return "EventSystem"; }
		std::vector<std::string> GetDependencies() const override
		{
			return {};
		};

		~EventSystem() override;

	protected:

		void Startup() override;
		void Shutdown() override;
	};
}