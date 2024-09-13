#pragma once

#include <vector>
#include <string>

#include "Systems/EngineSystem.h"

namespace CE
{
	class EventSystem : public CEngineSystem
	{
	public:

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