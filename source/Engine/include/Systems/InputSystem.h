#pragma once

#include "Systems/EngineSystem.h"

class GLFWwindow;

namespace CE
{
	class InputSystem : public EngineSystem
	{
	public:
		void AttachToWindow(GLFWwindow* window);

	private:
		GLFWwindow* m_window;
	public:

		/* CEngineSystem Interface */
		std::string Name() const override { return "InputSystem"; }

		InputSystem(Engine* engine) : EngineSystem(engine) {};

		/* Input System */

	protected:

		void Startup() override;
		void Shutdown() override;
		
		// More friendship to allow Engine to access protected functions on derived class pointers
		friend class Engine;
	};
}