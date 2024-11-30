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

			ImGui::Text("Keyboard Input");
			DrawKeyboardState(m_keyboardState);

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
		//std::cout << "Cursor Moved Action: " << xPos << ", " << yPos << std::endl;
		
		// TODO build move deltas

		ImGuiIO& io = ImGui::GetIO();
		io.AddMousePosEvent(xPos, yPos);
	}

	void InputSystem::OnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (window != m_window) return;
		//std::cout << "Key Action: " << key << std::endl;

		m_keyboardState.keys[key] = static_cast<KeyboardState::KeyState>(action);

		//ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

		//ImGuiIO& io = ImGui::GetIO();
		//io.AddKeyEvent(static_cast<ImGuiKey>(key + ImGuiKey_NamedKey_BEGIN), action == GLFW_PRESS);
	}

	void InputSystem::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		if (window != m_window) return;
		//std::cout << "Mouse Button Action: " << button << std::endl;

		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseButtonEvent(button, action == GLFW_PRESS);
	}

	void InputSystem::OnScroll(GLFWwindow* window, double xOffset, double yOffset)
	{
		if (window != m_window) return;
		//std::cout << "Scroll Action" << std::endl;

		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseWheelEvent(xOffset, yOffset);
	}

	void InputSystem::OnWindowClose(GLFWwindow* window)
	{
		if (window != m_window) return;
		//std::cout << "Window Close Action" << std::endl;

		m_engine->Stop();
	}

	void InputSystem::DrawKeyboardState(const KeyboardState& state) const
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 startDraw = ImGui::GetCursorScreenPos();

		static ImColor inactiveColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
		static ImColor activeColor = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);
		static float keyWidth = 45.f;
		static float keyHeight = 45.f;
		static float keyPadding = 3.f;
		static float externalKeyPadding = 10.f;
		static ImVec2 halfKey = { keyWidth * 0.5f, keyHeight * 0.5f };

		static std::vector<std::vector<int>> DrawOrder = {
			{GLFW_KEY_ESCAPE, GLFW_KEY_F1, GLFW_KEY_F2, GLFW_KEY_F3, GLFW_KEY_F4, GLFW_KEY_F5, GLFW_KEY_F6,
			 GLFW_KEY_F7, GLFW_KEY_F8, GLFW_KEY_F9, GLFW_KEY_F10, GLFW_KEY_F11, GLFW_KEY_F12, GLFW_KEY_F13},

			{GLFW_KEY_GRAVE_ACCENT, GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5, GLFW_KEY_6,
			 GLFW_KEY_7, GLFW_KEY_8, GLFW_KEY_9, GLFW_KEY_0, GLFW_KEY_MINUS, GLFW_KEY_EQUAL, GLFW_KEY_BACKSPACE},

			{GLFW_KEY_TAB, GLFW_KEY_Q, GLFW_KEY_W, GLFW_KEY_E, GLFW_KEY_R, GLFW_KEY_T, GLFW_KEY_Y, GLFW_KEY_U,
			 GLFW_KEY_I, GLFW_KEY_O, GLFW_KEY_P, GLFW_KEY_LEFT_BRACKET, GLFW_KEY_RIGHT_BRACKET, GLFW_KEY_BACKSLASH},

			{GLFW_KEY_CAPS_LOCK, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H,
			 GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_SEMICOLON, GLFW_KEY_APOSTROPHE, GLFW_KEY_ENTER},

			{GLFW_KEY_LEFT_SHIFT, GLFW_KEY_Z, GLFW_KEY_X, GLFW_KEY_C, GLFW_KEY_V, GLFW_KEY_B, GLFW_KEY_N,
			 GLFW_KEY_M, GLFW_KEY_COMMA, GLFW_KEY_PERIOD, GLFW_KEY_SLASH, GLFW_KEY_RIGHT_SHIFT},

			{GLFW_KEY_LEFT_CONTROL, GLFW_KEY_LEFT_ALT, GLFW_KEY_SPACE, GLFW_KEY_RIGHT_ALT, GLFW_KEY_RIGHT_CONTROL}
		};

		struct ExtraKeyData
		{
			float width;
			char name[8];
		};

		static std::unordered_map<int, ExtraKeyData> extraKeyData = {
			{GLFW_KEY_ESCAPE, {30.f, "esc"}},
			{GLFW_KEY_BACKSPACE, {30.f, "delete"}},
			{GLFW_KEY_TAB, {20.f, "tab"}},
			{GLFW_KEY_BACKSLASH, {10.f, "\\"}},
			{GLFW_KEY_CAPS_LOCK, {30.f, "caps"}},
			{GLFW_KEY_ENTER, {48.f, "return"}},
			{GLFW_KEY_LEFT_SHIFT, {63.f, "shift"}},
			{GLFW_KEY_RIGHT_SHIFT, {63.f, "shift"}},
			{GLFW_KEY_LEFT_CONTROL, {10.f, "ctrl"}},
			{GLFW_KEY_LEFT_ALT, {10.f, "alt"}},
			{GLFW_KEY_SPACE, {422.f, "space"}},
			{GLFW_KEY_RIGHT_ALT, {10.f, "alt"}},
			{GLFW_KEY_RIGHT_CONTROL, {10.f, "ctrl"}},
			{GLFW_KEY_F1, {0.f, "f1"}},
			{GLFW_KEY_F2, {0.f, "f2"}},
			{GLFW_KEY_F3, {0.f, "f3"}},
			{GLFW_KEY_F4, {0.f, "f4"}},
			{GLFW_KEY_F5, {0.f, "f5"}},
			{GLFW_KEY_F6, {0.f, "f6"}},
			{GLFW_KEY_F7, {0.f, "f7"}},
			{GLFW_KEY_F8, {0.f, "f8"}},
			{GLFW_KEY_F9, {0.f, "f9"}},
			{GLFW_KEY_F10, {0.f, "f10"}},
			{GLFW_KEY_F11, {0.f, "f11"}},
			{GLFW_KEY_F12, {0.f, "f12"}},
			{GLFW_KEY_F13, {0.f, "f13"}},
			{GLFW_KEY_F14, {0.f, "f14"}}
		};
		
		float rowPos = startDraw.y + externalKeyPadding;
		float rowExtraHeight = 0;
		for (int y = 0; y < DrawOrder.size(); y++)
		{
			// simple make func row smaller
			rowExtraHeight = y == 0 ? -15.f : 0.f;

			float columnPos = startDraw.x + externalKeyPadding;
			for (int x = 0; x < DrawOrder[y].size(); x++)
			{
				int key = DrawOrder[y][x];
				float extraWidth = extraKeyData.contains(key) ? extraKeyData[key].width : 0.f;
				ImVec2 minPos(columnPos, rowPos);
				ImVec2 maxPos(columnPos + keyWidth + extraWidth, rowPos + keyHeight + rowExtraHeight);
				ImColor keyColor = state.keys[key] ? activeColor : inactiveColor;

				drawList->AddRectFilled(minPos, maxPos, keyColor);
				columnPos += keyWidth + extraWidth + keyPadding;

				// Draw Key Name
				const char* keyName = glfwGetKeyName(key, 0);
				if (keyName == nullptr) keyName = extraKeyData.contains(key) ? extraKeyData[key].name : nullptr;
				if (keyName)
				{
					ImVec2 dim = ImGui::CalcTextSize(keyName);
					float textMinPosX = minPos.x + halfKey.x + extraWidth * 0.5f - dim.x * 0.5f;
					float textMinPosY = minPos.y + halfKey.y + rowExtraHeight * 0.5f - dim.y * 0.5f;
					ImVec2 textMinPos(textMinPosX, textMinPosY);
					drawList->AddText(textMinPos, ImGui::GetColorU32(ImGuiCol_Text), keyName);
				}
			}

			rowPos += keyHeight + keyPadding + rowExtraHeight;
		}
	}
}