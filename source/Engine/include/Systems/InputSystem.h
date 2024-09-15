#pragma once

#include "Systems/EngineSystem.h"

namespace CE
{
	class InputSystem : public EngineSystem
	{
	public:

		/* CEngineSystem Interface */
		std::string Name() const override { return "InputSystem"; }

		std::vector<std::string> GetDependencies() const override
		{
			return { "EventSystem" };
		};

		~InputSystem() override;

		/* Input System */

	protected:

		void Startup() override;
		void Shutdown() override;
	};
}