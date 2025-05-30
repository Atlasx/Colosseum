#include "Systems/InputSystem.h"

#include "Engine.h"
#include "Systems/LogSystem.h"
#include "GUI/Editor.h"

#ifdef CDEBUG
#include "Systems/Debug/InputSystemDebug.h"
#endif

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "glm/ext.hpp"

#include "stdlibincl.h"

namespace CE
{
	InputSystem* g_input = nullptr;

	void InputSystem::Startup()
	{
		LOG_INFO(INPUT, "Startup");

		m_window = m_engine->GetWindow();
		assert(m_window);

		// Save this instance as the global instance for GLFW callbacks
		g_input = this;

		// Register callbacks for discrete input
		glfwSetCursorPosCallback(m_window, &GOnCursorMoved);
		glfwSetKeyCallback(m_window, &GOnKey);
		glfwSetMouseButtonCallback(m_window, &GOnMouseButton);
		glfwSetScrollCallback(m_window, &GOnScroll);
		glfwSetWindowCloseCallback(m_window, &GOnWindowClose);

#ifdef CDEBUG
		// Register debug GUI
		m_debugger = new InputSystemDebug(this);
		std::shared_ptr<DebugSystem> DS = m_engine->GetSystem<DebugSystem>();
		if (DS)
		{
			DS->Subscribe(m_debugger);
		}
#endif

		// Test action
		std::shared_ptr<InputSystem> IS = m_engine->GetSystem<InputSystem>();
		IS->RegisterAction<PressedAction>("Event System Hook", KeyType::P, []() {
			LOG_INFO(INPUT, "Successfully fired action callback");
		});

		IS->RegisterAction<HoldAction>("Hold Action Test", KeyType::H, 10.f, []() {
			LOG_INFO(INPUT, "Fired Hold Action");
			});
	}

	void InputSystem::Shutdown()
	{
		LOG_INFO(INPUT, "Shutdown");

		g_input = nullptr;
	}

	void InputSystem::PollInput()
	{
		if (m_inputKnowledge.bNeedsUpdate)
		{
			UpdateInputKnowledge();
		}

		glfwPollEvents();
	}

	/*
	 *	Update Key State
	 * 
	 *	Updates internal keyboard representation and performs updates on all actions
	 * 
	*/
	void InputSystem::UpdateKeyState(const KeyType key, const KeyState newState)
	{
		// Update our knowledge base of inputs
		const KeyState prevState = m_inputKnowledge.currentBoardState.GetKey(key);
		m_inputKnowledge.currentBoardState.SetKey(key, newState);
		m_inputKnowledge.previousBoardState.SetKey(key, prevState);

		if (newState != prevState)
		{
			InputEvent state;
			state.SetKey(key, newState);
			m_events.push(state);
		}
	}

	void InputSystem::UpdateMouseButtonState(const MouseButtonType button, const KeyState newState)
	{
		const KeyState prevState = m_inputKnowledge.currentMouseState.GetButton(button);
		m_inputKnowledge.currentMouseState.SetButton(button, newState);
		m_inputKnowledge.previousMouseState.SetButton(button, prevState);

		if (newState != prevState)
		{
			InputEvent state;
			state.SetMouseButton(button, newState);
			m_events.push(state);
		}
	}

	void InputSystem::UpdateInputKnowledge()
	{
		for (int i = 0; i < static_cast<int>(KeyType::KEYS_MAX); i++)
		{
			KeyType key = static_cast<KeyType>(i);

			// A little translation weirdness
			int glfwKeyAction = glfwGetKey(m_window, InputUtilities::KeyTypeToGLFWKey(key));
			UpdateKeyState(key, InputUtilities::GLFWActionToKeyState(glfwKeyAction));
		}

		for (uint8_t i = 0; i < static_cast<uint8_t>(MouseButtonType::BUTTONS_MAX); i++)
		{
			MouseButtonType button = static_cast<MouseButtonType>(i);
			int glfwButtonAction = glfwGetMouseButton(m_window, InputUtilities::MouseButtonTypeToGLFWMouseButton(button));
			UpdateMouseButtonState(button, InputUtilities::GLFWActionToKeyState(glfwButtonAction));
		}
	}


	void InputSystem::ProcessActions()
	{
		// Flush Event Queue
		while (!m_events.empty())
		{
			for (auto& [handle, action] : m_actionPool)
			{
				action->ProcessEvent(m_events.front());
			}
			m_events.pop();
		}
	}

	void InputSystem::UpdateActions()
	{
		for (auto& [handle, action] : m_actionPool)
		{
			action->Update(0.16f);
		}
	}

	bool InputSystem::RemoveAction(InputActionHandle handle)
	{
		if (handle == InputActionHandle::INVALID)
			return false;

		m_actionPool.Destroy(handle);
		return true;	
	}

	void InputSystem::OnCursorMoved(GLFWwindow* window, double xPos, double yPos)
	{
		if (window != m_window) return;

		// TODO Add to knowledge base
		/*
		InputEvent eventX;
		eventX.SetAxis(AxisType::MOUSE_X, xPos);
		m_events.push(eventX);

		InputEvent eventY;
		eventY.SetAxis(AxisType::MOUSE_Y, yPos);
		m_events.push(eventY);

		InputEvent eventXY;
		eventXY.SetAxis2D(Axis2DType::MOUSE_XY, xPos, yPos);
		m_events.push(eventXY);
		*/
	}

	void InputSystem::OnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (window != m_window) return;

		const KeyType keyT = InputUtilities::GLFWKeyToKeyType(key);
		const KeyState keyS = InputUtilities::GLFWActionToKeyState(action);

		UpdateKeyState(keyT, keyS);
	}

	void InputSystem::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		if (window != m_window) return;

		const MouseButtonType buttonT = InputUtilities::GLFWMouseButtonToMouseButtonType(button);
		const KeyState buttonS = InputUtilities::GLFWActionToKeyState(action);

		UpdateMouseButtonState(buttonT, buttonS);
	}

	void InputSystem::OnScroll(GLFWwindow* window, double xOffset, double yOffset)
	{
		if (window != m_window) return;
	}

	void InputSystem::OnWindowClose(GLFWwindow* window)
	{
		if (window != m_window) return;

		m_engine->Stop();
	}


	namespace InputUtilities
	{
		constexpr void TranslateKeys()
		{
			// Had an idea to make either the compiler or cmake write these translation arrays for me.
			// Going to save the idea for later as it has a near zero benefit other than being cool.

			auto translate = [](int key) -> KeyType
				{
					if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
						return static_cast<KeyType>(key - GLFW_KEY_A + KeyType::A);
					if (key == GLFW_KEY_SPACE)
						return KeyType::SPACE;
					if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
						return static_cast<KeyType>(key - GLFW_KEY_0 + KeyType::NUM0);
					if (key >= GLFW_KEY_COMMA && key <= GLFW_KEY_SLASH)
						return static_cast<KeyType>(key - GLFW_KEY_COMMA + KeyType::COMMA);
					if (key >= GLFW_KEY_LEFT_BRACKET && key <= GLFW_KEY_RIGHT_BRACKET)
						return static_cast<KeyType>(key - GLFW_KEY_LEFT_BRACKET + KeyType::LEFT_BRACKET);
					if (key >= GLFW_KEY_ESCAPE && key <= GLFW_KEY_END)
						return static_cast<KeyType>(key - GLFW_KEY_ESCAPE + KeyType::ESCAPE);
					if (key >= GLFW_KEY_LEFT_SHIFT && key <= GLFW_KEY_LEFT_ALT)
						return static_cast<KeyType>(key - GLFW_KEY_LEFT_SHIFT + KeyType::LEFT_SHIFT);
					if (key >= GLFW_KEY_RIGHT_SHIFT && key <= GLFW_KEY_RIGHT_ALT)
						return static_cast<KeyType>(key - GLFW_KEY_RIGHT_SHIFT + KeyType::RIGHT_SHIFT);
					if (key == GLFW_KEY_SEMICOLON)
						return KeyType::SEMICOLON;
					if (key == GLFW_KEY_CAPS_LOCK)
						return KeyType::CAPS_LOCK;
					if (key == GLFW_KEY_EQUAL)
						return KeyType::EQUAL;
					if (key == GLFW_KEY_APOSTROPHE)
						return KeyType::APOSTROPHE;
					if (key == GLFW_KEY_GRAVE_ACCENT)
						return KeyType::TILDE;
					if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F12)
						return static_cast<KeyType>(key - GLFW_KEY_F1 + KeyType::F1);
					return KeyType::UNKNOWN;
				};

			for (int i = 0; i < 348; i++)
			{
				KeyType keyType = translate(i);
				if (keyType != KeyType::UNKNOWN)
				{
					g_glfwKeyToKeyType[i] = static_cast<std::underlying_type_t<KeyType>>(keyType);
					g_keyTypeToGLFWKey[static_cast<std::underlying_type_t<KeyType>>(keyType)] = i;
				}
			}

			g_bHasKeymap = true;
		}

		KeyType GLFWKeyToKeyType(int key)
		{
			if (g_bHasKeymap == false) TranslateKeys();
			return static_cast<KeyType>(g_glfwKeyToKeyType[key]);
		}

		int KeyTypeToGLFWKey(const KeyType key)
		{
			if (g_bHasKeymap == false) TranslateKeys();
			return g_keyTypeToGLFWKey[static_cast<std::underlying_type_t<KeyType>>(key)];
		}

		MouseButtonType GLFWMouseButtonToMouseButtonType(int button)
		{
			// Off by one due to zero index with GLFW and my Unknown button type
			return static_cast<MouseButtonType>(button + 1);
		}

		int MouseButtonTypeToGLFWMouseButton(const MouseButtonType button)
		{
			return static_cast<int>(button) - 1;
		}

		KeyState GLFWActionToKeyState(int action)
		{
			switch (action)
			{
			case 0:
				return KeyState::RELEASED;
			case 1:
				return KeyState::PRESSED;
			case 2:
				return KeyState::HELD;
			default:
				return KeyState::UNKNOWN;
			}
		}

		const char* GetKeyStateName(const KeyState state)
		{
			switch (state)
			{
			case KeyState::ANY: return "Any";
			case KeyState::PRESSED: return "Pressed";
			case KeyState::RELEASED: return "Released";
			case KeyState::HELD: return "Held";
			default: return "UNK";
			}
		}

		const char* GetMouseButtonName(const MouseButtonType button)
		{
			switch (button)
			{
			case MouseButtonType::MOUSE_BUTTON_1: return "Mouse Button 1";
			case MouseButtonType::MOUSE_BUTTON_2: return "Mouse Button 2";
			case MouseButtonType::MOUSE_BUTTON_3: return "Mouse Button 3";
			case MouseButtonType::MOUSE_BUTTON_4: return "Mouse Button 4";
			case MouseButtonType::MOUSE_BUTTON_5: return "Mouse Button 5";
			case MouseButtonType::MOUSE_BUTTON_6: return "Mouse Button 6";
			case MouseButtonType::MOUSE_BUTTON_7: return "Mouse Button 7";
			case MouseButtonType::MOUSE_BUTTON_8: return "Mouse Button 8";
			default: return "UNK";
			}
		}

		const char* GetKeyName(const KeyType key)
		{
			switch (key)
			{
				// Numbers
			case KeyType::NUM0: return "0";
			case KeyType::NUM1: return "1";
			case KeyType::NUM2: return "2";
			case KeyType::NUM3: return "3";
			case KeyType::NUM4: return "4";
			case KeyType::NUM5: return "5";
			case KeyType::NUM6: return "6";
			case KeyType::NUM7: return "7";
			case KeyType::NUM8: return "8";
			case KeyType::NUM9: return "9";

				// Alphabet (A-Z)
			case KeyType::A: return "A";
			case KeyType::B: return "B";
			case KeyType::C: return "C";
			case KeyType::D: return "D";
			case KeyType::E: return "E";
			case KeyType::F: return "F";
			case KeyType::G: return "G";
			case KeyType::H: return "H";
			case KeyType::I: return "I";
			case KeyType::J: return "J";
			case KeyType::K: return "K";
			case KeyType::L: return "L";
			case KeyType::M: return "M";
			case KeyType::N: return "N";
			case KeyType::O: return "O";
			case KeyType::P: return "P";
			case KeyType::Q: return "Q";
			case KeyType::R: return "R";
			case KeyType::S: return "S";
			case KeyType::T: return "T";
			case KeyType::U: return "U";
			case KeyType::V: return "V";
			case KeyType::W: return "W";
			case KeyType::X: return "X";
			case KeyType::Y: return "Y";
			case KeyType::Z: return "Z";

				// Symbol Keys
			case KeyType::APOSTROPHE: return "'";
			case KeyType::COMMA: return ",";
			case KeyType::MINUS: return "-";
			case KeyType::PERIOD: return ".";
			case KeyType::SLASH: return "/";
			case KeyType::SEMICOLON: return ";";
			case KeyType::EQUAL: return "=";
			case KeyType::LEFT_BRACKET: return "[";
			case KeyType::BACKSLASH: return "\\";
			case KeyType::RIGHT_BRACKET: return "]";
			case KeyType::TILDE: return "~";

				// Control Keys
			case KeyType::ESCAPE: return "esc";
			case KeyType::ENTER: return "ent";
			case KeyType::TAB: return "tab";
			case KeyType::BACKSPACE: return "bksp";
			case KeyType::INSERT: return "ins";
			case KeyType::DELETE: return "del";
			case KeyType::ARROW_RIGHT: return "right";
			case KeyType::ARROW_LEFT: return "left";
			case KeyType::ARROW_DOWN: return "down";
			case KeyType::ARROW_UP: return "up";
			case KeyType::PAGE_UP: return "PgUp";
			case KeyType::PAGE_DOWN: return "PgDn";
			case KeyType::HOME: return "home";
			case KeyType::END: return "end";
			case KeyType::CAPS_LOCK: return "caps";
			case KeyType::LEFT_SHIFT: return "shift";
			case KeyType::LEFT_CTRL: return "ctrl";
			case KeyType::LEFT_ALT: return "alt";
			case KeyType::RIGHT_SHIFT: return "shift";
			case KeyType::RIGHT_CTRL: return "ctrl";
			case KeyType::RIGHT_ALT: return "alt";
			case KeyType::SPACE: return "space";

				// Function Keys
			case KeyType::F1: return "F1";
			case KeyType::F2: return "F2";
			case KeyType::F3: return "F3";
			case KeyType::F4: return "F4";
			case KeyType::F5: return "F5";
			case KeyType::F6: return "F6";
			case KeyType::F7: return "F7";
			case KeyType::F8: return "F8";
			case KeyType::F9: return "F9";
			case KeyType::F10: return "F10";
			case KeyType::F11: return "F11";
			case KeyType::F12: return "F12";

				// Default case for unknown values
			default: return "UNK";
			}
		}


	}
}