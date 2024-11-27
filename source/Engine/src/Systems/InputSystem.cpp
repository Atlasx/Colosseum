#include "Systems/InputSystem.h"

#include "Engine.h"

#include <iostream>
#include "GLFW/glfw3.h"

namespace CE
{
	void InputSystem::Startup()
	{
		m_window = m_engine->GetWindow();
		assert(m_window);

		g_input = this;

		glfwSetCursorPosCallback(m_window, &GOnCursorMoved);
	}

	void InputSystem::Shutdown()
	{
		g_input = nullptr;
	}

	void InputSystem::PollInput()
	{
		glfwPollEvents();
	}

	void InputSystem::OnCursorMoved(GLFWwindow* window, double xPos, double yPos)
	{
		std::cout << "Cursor moved" << std::endl;
		if (window != m_window) return;
	}

	void InputSystem::OnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
	{

	}

	void InputSystem::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
	{

	}

	void InputSystem::OnScroll(GLFWwindow* window, double xOffset, double yOffset)
	{

	}

}