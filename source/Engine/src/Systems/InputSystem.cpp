#include "Systems/InputSystem.h"

#include "Engine.h"

#include <iostream>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"

namespace CE
{
	void InputSystem::Startup()
	{
		m_window = m_engine->GetWindow();
		assert(m_window);

		g_input = this;

		// Register callbacks for discrete input
		glfwSetCursorPosCallback(m_window, &GOnCursorMoved);
		glfwSetKeyCallback(m_window, &GOnKey);
		glfwSetMouseButtonCallback(m_window, &GOnMouseButton);
		glfwSetScrollCallback(m_window, &GOnScroll);
		glfwSetWindowCloseCallback(m_window, &GOnWindowClose);
	}

	void InputSystem::Shutdown()
	{
		g_input = nullptr;
	}

	void InputSystem::DrawGUI()
	{
		if (m_showDebug) 
		{
			ImGui::Begin("Input System Debug", &m_showDebug);
			ImGui::Text("Mouse Input");
			ImGui::Separator();
			ImGui::Text("TODO mouse input");
			ImGui::Separator();

			ImGui::Text("Keyboard Input");
			ImGui::Separator();
			DrawKeyboardState(m_keyboardState);
			ImGui::Separator();

			ImGui::End();
		}
	}

	void InputSystem::PollInput()
	{
		glfwPollEvents();

		// Build current input state
		m_prevKeyboardState = m_keyboardState;
		m_keyboardState = {};

		for (int i = 0; i < KeyboardState::KEYBOARD_MAX; i++)
		{
			m_keyboardState.keys[i] = static_cast<KeyboardState::KeyState>(glfwGetKey(m_window, i));
		}
		
	}

	void InputSystem::OnCursorMoved(GLFWwindow* window, double xPos, double yPos)
	{
		if (window != m_window) return;
		std::cout << "Cursor Moved Action: " << xPos << ", " << yPos << std::endl;
		
		// TODO build move deltas

		ImGuiIO& io = ImGui::GetIO();
		io.AddMousePosEvent(xPos, yPos);
	}

	void InputSystem::OnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (window != m_window) return;
		std::cout << "Key Action: " << key << std::endl;

		m_keyboardState.keys[key] = static_cast<KeyboardState::KeyState>(action);

		//ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

		//ImGuiIO& io = ImGui::GetIO();
		//io.AddKeyEvent(static_cast<ImGuiKey>(key + ImGuiKey_NamedKey_BEGIN), action == GLFW_PRESS);
	}

	void InputSystem::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		if (window != m_window) return;
		std::cout << "Mouse Button Action: " << button << std::endl;

		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseButtonEvent(button, action == GLFW_PRESS);
	}

	void InputSystem::OnScroll(GLFWwindow* window, double xOffset, double yOffset)
	{
		if (window != m_window) return;
		std::cout << "Scroll Action" << std::endl;

		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseWheelEvent(xOffset, yOffset);
	}

	void InputSystem::OnWindowClose(GLFWwindow* window)
	{
		if (window != m_window) return;
		std::cout << "Window Close Action" << std::endl;

		m_engine->Stop();
	}

	void InputSystem::DrawKeyboardState(const KeyboardState& state) const
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 startDraw = ImGui::GetCursorScreenPos();

		for (int i = 65; i < 139; i++) {
			char key = static_cast<char>(i);
			ImGui::Text("Key: %c, State: %d", key, state.keys[i]);
		}
	}
}