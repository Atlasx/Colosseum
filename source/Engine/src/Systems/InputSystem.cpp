#include "Systems/InputSystem.h"

#include "Engine.h"

#include <iostream>
#include <algorithm>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"

namespace CE
{
	void InputSystem::Startup()
	{
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
			ImGui::Text("TODO mouse input");

			static bool bShowKeyboard = false;
			ImGui::Checkbox("Show Keyboard", &bShowKeyboard);
			if (bShowKeyboard) {
				ImGui::Begin("Keyboard Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
				DrawKeyboardState(m_keyboardState);
				ImGui::End();
			}
			ImGui::End();
		}
	}

	void InputSystem::PollInput()
	{
		glfwPollEvents();
	}

	void InputSystem::UpdateKeyState(const KeyType key, const KeyState newState)
	{
		KeyState prevKeyState = m_keyboardState.GetKey(key);
		if (newState != prevKeyState)
		{
			// Something has changed in the state! woohoo!
			ProcessKeyStateChange(key, prevKeyState, newState);
		}
		m_keyboardState.SetKey(key, newState);
	}

	void InputSystem::ProcessKeyStateChange(const KeyType key, const KeyState prevState, const KeyState newState)
	{
		//KeyTrigger trigger(prevState, newState);

		// Check if we should fire any actions
		for (auto& action : m_actions)
		{
			if (action->IsBoundTo(key))
			{
				action->Execute();
			}
		}

		for (auto& axisAction : m_axisActions)
		{
			if (axisAction->IsBoundTo(key))
			{

			}
		}
	}

	void InputSystem::OnCursorMoved(GLFWwindow* window, double xPos, double yPos)
	{
		if (window != m_window) return;

		// TODO build move deltas
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

	void InputSystem::DrawKeyboardState(const KeyboardState& state, const ImVec2& offset, const ImVec2& size) const
	{
		// This assumes we will use the qwerty layout, magic number essentially
		static const ImVec2 s_maxSize(719.f, 295.f);
		static ImColor s_inactiveColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
		static ImColor s_activeColor = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);
		static float s_keyWidth = 45.f;
		static float s_keyHeight = 45.f;
		static float s_keyPadding = 3.f;
		static float s_externalKeyPadding = 10.f;

		// Translated QWERTY Draw Order from GLFW to KeyType enum
		static std::vector<std::vector<KeyType>> s_qwertyDrawOrder = {
			{KeyType::ESCAPE, KeyType::F1, KeyType::F2, KeyType::F3, KeyType::F4, KeyType::F5, KeyType::F6, KeyType::F7, KeyType::F8, KeyType::F9, KeyType::F10, KeyType::F11, KeyType::F12, KeyType::UNKNOWN},
			{KeyType::TILDE, KeyType::NUM1, KeyType::NUM2, KeyType::NUM3, KeyType::NUM4, KeyType::NUM5, KeyType::NUM6, KeyType::NUM7, KeyType::NUM8, KeyType::NUM9, KeyType::NUM0, KeyType::MINUS, KeyType::EQUAL, KeyType::BACKSPACE},
			{KeyType::TAB, KeyType::Q, KeyType::W, KeyType::E, KeyType::R, KeyType::T, KeyType::Y, KeyType::U, KeyType::I, KeyType::O, KeyType::P, KeyType::LEFT_BRACKET, KeyType::RIGHT_BRACKET, KeyType::BACKSLASH},
			{KeyType::CAPS_LOCK, KeyType::A, KeyType::S, KeyType::D, KeyType::F, KeyType::G, KeyType::H, KeyType::J, KeyType::K, KeyType::L, KeyType::SEMICOLON, KeyType::APOSTROPHE, KeyType::ENTER},
			{KeyType::LEFT_SHIFT, KeyType::Z, KeyType::X, KeyType::C, KeyType::V, KeyType::B, KeyType::N, KeyType::M, KeyType::COMMA, KeyType::PERIOD, KeyType::SLASH, KeyType::RIGHT_SHIFT},
			{KeyType::LEFT_CTRL, KeyType::LEFT_ALT, KeyType::SPACE, KeyType::RIGHT_ALT, KeyType::RIGHT_CTRL}
		};

		static std::unordered_map<KeyType, float> s_keyData = {
			{KeyType::ESCAPE, 30.f},
			{KeyType::BACKSPACE, 30.f},
			{KeyType::TAB, 20.f},
			{KeyType::BACKSLASH, 10.f},
			{KeyType::CAPS_LOCK, 30.f},
			{KeyType::ENTER, 48.f},
			{KeyType::LEFT_SHIFT, 63.f},
			{KeyType::RIGHT_SHIFT, 63.f},
			{KeyType::LEFT_CTRL, 10.f},
			{KeyType::LEFT_ALT, 10.f},
			{KeyType::SPACE, 422.f},
			{KeyType::RIGHT_ALT, 10.f},
			{KeyType::RIGHT_CTRL, 10.f}
		};



		bool bSizeProvided = !(size.x == 0 || size.y == 0);

		ImGui::PushID(state.keys);
		ImGui::BeginChild("keyboard", bSizeProvided ? size : s_maxSize);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 startDraw = ImGui::GetCursorScreenPos();

		startDraw.x += offset.x;
		startDraw.y += offset.y;

		float rowPos = startDraw.y + s_externalKeyPadding;
		float columnPos = 0.f;
		float rowExtraHeight = 0;
		for (int y = 0; y < s_qwertyDrawOrder.size(); y++)
		{
			// simple make func row smaller
			rowExtraHeight = y == 0 ? -15.f : 0.f;

			columnPos = startDraw.x + s_externalKeyPadding;
			for (int x = 0; x < s_qwertyDrawOrder[y].size(); x++)
			{
				KeyType key = s_qwertyDrawOrder[y][x];
				ImVec2 minPos(columnPos, rowPos);
				ImVec2 maxPos(columnPos, rowPos);

				float width = s_keyWidth;
				width += s_keyData.contains(key) ? s_keyData[key] : 0.f;
				float height = s_keyHeight + rowExtraHeight;

				ImColor keyColor = state.GetKey(key) == KeyState::PRESSED ? s_activeColor : s_inactiveColor;
		
				maxPos.x += width;
				maxPos.y += height;
				drawList->AddRectFilled(minPos, maxPos, keyColor);
				columnPos += width + s_keyPadding;

				// Draw Key Name
				const char* keyName = InputUtilities::GetKeyName(key);
				if (keyName)
				{
					ImVec2 dim = ImGui::CalcTextSize(keyName);
					float textMinPosX = minPos.x + width * 0.5f - dim.x * 0.5f;
					float textMinPosY = minPos.y + height * 0.5f - dim.y * 0.5f;
					ImVec2 textMinPos(textMinPosX, textMinPosY);
					drawList->AddText(textMinPos, ImGui::GetColorU32(ImGuiCol_Text), keyName);
				}
			}

			rowPos += s_keyHeight + s_keyPadding + rowExtraHeight;
		}

		// Cleanup for proper formatting
		ImGui::SetCursorScreenPos(ImVec2(startDraw.x, rowPos + s_externalKeyPadding));

		ImGui::EndChild();
		ImGui::PopID();
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

		KeyState GLFWActionToKeyState(int action)
		{
			return static_cast<KeyState>(action);
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