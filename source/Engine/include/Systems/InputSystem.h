#pragma once

#include "Systems/EngineSystem.h"

namespace CE
{
	class InputSystem : public EngineSystem
	{
		// More friendship to allow Engine to access protected functions on derived class pointers
		friend class Engine;

	public:

		/* CEngineSystem Interface */
		std::string Name() const override { return "InputSystem"; }

		InputSystem(Engine* engine) : EngineSystem(engine) {};

		/* Input System */

	protected:

		void Startup() override;
		void Shutdown() override;
	};
}