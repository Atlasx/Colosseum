#pragma once

#include "Systems/EngineSystem.h"

namespace CE
{
	class InputSystem : public CEngineSystem
	{
	public:
		void Startup() override;
		void Shutdown() override;

		std::vector<std::string> GetDependencies() const override;

		~InputSystem() override;
	};
}