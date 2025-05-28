#include "Systems/Debug/InputSystemDebug.h"

#include "Systems/InputSystem.h"
#include "imgui.h"

#ifdef CDEBUG

namespace CE
{
	static void DrawKeyboardState(const KeyboardState& state, const ImVec2& offset, const ImVec2& size)
	{
		// This assumes we will use the qwerty layout, magic number essentially
		static const ImVec2 s_maxSize(719.f, 295.f);
		static ImColor s_inactiveColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
		static ImColor s_activeColor = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);
		static ImColor s_heldColor = ImGui::GetColorU32(ImGuiCol_FrameBgActive);
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

		static auto GetKeyStateColor = [](KeyState state) -> ImColor
			{
				switch (state)
				{
				case KeyState::PRESSED:
					return s_activeColor;
				case KeyState::HELD:
					return s_heldColor;
				case KeyState::RELEASED:
				default:
					return s_inactiveColor;
				}
			};


		bool bSizeProvided = !(size.x == 0 || size.y == 0);

		ImGui::PushID(0);
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

				ImColor keyColor = GetKeyStateColor(state.GetKey(key));
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

	void InputSystemDebug::OnDrawGUI()
	{
		if (m_owner == nullptr) { return; }

		ImGui::SetNextWindowSize(ImVec2(200.f, 500.f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(10.f, 110.f), ImGuiCond_Appearing);
		ImGui::SetNextWindowSizeConstraints(ImVec2(50.f, 50.f), ImVec2(FLT_MAX, FLT_MAX));
		ImGui::Begin("Input System Debug");
		if (ImGui::CollapsingHeader("Actions", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (m_owner->m_actionPool.GetFill() == 0)
			{
				ImGui::Text("No Actions Bound!");
			}
			ImGui::Indent();

			for (auto& [handle, action] : m_owner->m_actionPool)
			{
				std::string_view actionName = m_owner->m_actionNames[handle];
				ImGui::PushID(handle.GetIndex());
				if (ImGui::CollapsingHeader(actionName.data(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet))
				{
					ImGui::Indent();
					const char* bindingName = InputUtilities::GetKeyName(action->GetBinding());
					ImGui::Text("Action: 0x%X", handle);
					ImGui::Text("Key Binding: %s", bindingName);
					if (ImGui::Button("Fire"))
					{
						action->Trigger();
					}
					ImGui::SameLine();
					if (ImGui::Button("Remove"))
					{
						m_owner->RemoveAction(handle);
					}
					ImGui::Unindent();
				}
				ImGui::PopID();
			}

			ImGui::Unindent();
		}

		if (ImGui::CollapsingHeader("Mouse"))
		{
			ImGui::Text("Mouse Input");
			ImGui::BeginChild("MouseInput", ImVec2(0, 0), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY);

			auto& knowledge = m_owner->GetKnowledge();
			for (uint8_t i = 0; i < 8; i++)
			{
				MouseButtonType button = static_cast<MouseButtonType>(i);
				KeyState state = knowledge.currentMouseState.GetButton(button);
				ImGui::Text("%s: %s",
					InputUtilities::GetMouseButtonName(button),
					InputUtilities::GetKeyStateName(state)
				);
			}


			ImGui::EndChild();
		}

		if (ImGui::CollapsingHeader("Keyboard"))
		{
			static bool bShowKeyboard = false;
			ImGui::Checkbox("Show Keyboard", &bShowKeyboard);
			if (bShowKeyboard) {
				ImGui::Begin("Keyboard Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
				DrawKeyboardState(m_owner->GetKnowledge().currentBoardState, ImVec2(), ImVec2());
				ImGui::End();
			}
		}
		ImGui::End();
	}

}

#endif