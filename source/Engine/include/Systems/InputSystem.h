#pragma once

#include "Systems/EngineSystem.h"

class GLFWwindow;

namespace CE
{	
	struct KeyboardState
	{
		constexpr static unsigned int KEYBOARD_MAX = 348;

		enum KeyState : unsigned char {
			KS_RELEASED,
			KS_PRESSED
		};

		KeyState keys[KEYBOARD_MAX];
	};

	// Going to use a global here for now, glfw seems to require some way of accessing which engine it is referring to when using the input functions.
	// Not a fan of this, but I can't think of a better way at the moment
	class InputSystem;
	static InputSystem* g_input = nullptr;

	class InputSystem : public EngineSystem
	{
	public:
		void PollInput();

	private:
		GLFWwindow* m_window = nullptr;

		void OnCursorMoved(GLFWwindow* window, double xPos, double yPos);
		void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);
		void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
		void OnScroll(GLFWwindow* window, double xOffset, double yOffset);
		void OnWindowClose(GLFWwindow* window);

		KeyboardState m_keyboardState{};
		KeyboardState m_prevKeyboardState{};

		void DrawKeyboardState(const KeyboardState& state) const;

	public:

		/* CEngineSystem Interface */
		std::string Name() const override { return "Input System"; }
		void DrawGUI() override;

		InputSystem(Engine* engine) : EngineSystem(engine) 
		{
			m_showDebug = true;
		};

	protected:

		void Startup() override;
		void Shutdown() override;
		
		// More friendship to allow Engine to access protected functions on derived class pointers
		friend class Engine;

		/* Handle Global Input Processing */
	private:
		static void GOnCursorMoved(GLFWwindow* window, double xPos, double yPos)
		{
			g_input->OnCursorMoved(window, xPos, yPos);
		};

		static void GOnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			g_input->OnKey(window, key, scancode, action, mods);
		}

		static void GOnMouseButton(GLFWwindow* window, int button, int action, int mods)
		{
			g_input->OnMouseButton(window, button, action, mods);
		}

		static void GOnScroll(GLFWwindow* window, double xOffset, double yOffset)
		{
			g_input->OnScroll(window, xOffset, yOffset);
		}

		static void GOnWindowClose(GLFWwindow* window)
		{
			g_input->OnWindowClose(window);
		}
	};
}