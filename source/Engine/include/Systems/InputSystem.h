#pragma once

#include "Systems/EngineSystem.h"

#include "imgui.h"

#include <cstdint>
#include <iostream>
#include <functional>
#include <set>

class GLFWwindow;

namespace CE
{	
	enum class KeyType : std::uint8_t
	{
		// WARNING if this enum changes, be sure to audit translation function for GLFW!
		UNKNOWN,

		// Numbers
		NUMBER_KEYS,
		NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9,
		NUMBER_KEYS_MAX,

		// Alphabet (A-Z)
		ALPHABET_KEYS,
		A, B, C, D, E, F, G, H, I, J, K, L, M,
		N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		ALPHABET_KEYS_MAX,

		// Symbol Keys
		SYMBOL_KEYS,
		APOSTROPHE,		// ' 
		COMMA,			// , 
		MINUS,			// - 
		PERIOD,			// . 
		SLASH,			// / 
		SEMICOLON,		// ; 
		EQUAL,			// = 
		LEFT_BRACKET,	// [
		BACKSLASH,		// \ 
		RIGHT_BRACKET,	// ]
		TILDE,			// ~ 
		SYMBOL_KEYS_MAX,

		// Control Keys
		CONTROL_KEYS,
		ESCAPE,
		ENTER,
		TAB,
		BACKSPACE,
		INSERT,
		DELETE,
		ARROW_RIGHT,
		ARROW_LEFT,
		ARROW_DOWN,
		ARROW_UP,
		PAGE_UP,
		PAGE_DOWN,
		HOME,
		END,
		CAPS_LOCK,
		LEFT_SHIFT,
		LEFT_CTRL,
		LEFT_ALT,
		RIGHT_SHIFT,
		RIGHT_CTRL,
		RIGHT_ALT,
		SPACE,
		CONTROL_KEYS_MAX,

		// Function Keys
		FN_KEYS,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		FN_KEYS_MAX,
		KEYS_MAX
	};

	enum class KeyState : unsigned char {
		RELEASED,
		PRESSED,
		HELD,
	};

	namespace InputUtilities 
	{
		constexpr KeyType operator+(KeyType key, int offset)
		{
			using Underlying = std::underlying_type_t<KeyType>;
			return static_cast<KeyType>(static_cast<Underlying>(key) + offset);
		}

		constexpr KeyType operator+(int offset, KeyType key)
		{
			return key + offset;
		}

		constexpr int operator-(KeyType lhs, KeyType rhs)
		{
			using Underlying = std::underlying_type_t<KeyType>;
			return static_cast<Underlying>(lhs) - static_cast<Underlying>(rhs);
		}

		// Translation globals
		static bool g_bHasKeymap = false;
		static int g_glfwKeyToKeyType[348];
		static int g_keyTypeToGLFWKey[92];

		KeyType GLFWKeyToKeyType(int key);
		int KeyTypeToGLFWKey(const KeyType key);

		KeyState GLFWActionToKeyState(int action);

		const char* GetKeyName(const KeyType key);
	}

	struct KeyboardState
	{
	public:
		KeyState GetKey(const KeyType key) const
		{
			return keys[static_cast<std::underlying_type_t<KeyType>>(key)];
		}

		bool GetKeyHeld(const KeyType key) const
		{
			return keys[static_cast<std::underlying_type_t<KeyType>>(key)] == KeyState::HELD;
		}

	private:
		constexpr static unsigned int KEYBOARD_MAX = static_cast<unsigned int>(KeyType::KEYS_MAX);
		
		inline void SetKey(const KeyType key, const KeyState state)
		{
			keys[static_cast<std::underlying_type_t<KeyType>>(key)] = state;
		}

		KeyState keys[KEYBOARD_MAX];

		friend class InputSystem;
	};

	/*
	* Keys, state changes between keys
	* multiple key bindings for the same action
	* accumulate held timers
	* 
	* thresholds for duration on held
	* continuous firing and discrete firing
	* 
	* Jump -> bound to space, instantaneous, on pressed
	* Interact -> bound to e, start timer on pressed, fire trigger on duration
	* WASD -> bound to WASD, continuously reporting on 2D axis, fires every frame
	* Mouse Delta -> bound to mouse move, accumulate and report every frame
	*	side note: consider mid-frame mouse input and partial rotation to input angle
	* 
	* Actions are composed of binding, trigger, callback
	* trigger is updated with key state deltas and per-frame updates
	* actions are checked every frame if trigger?
	* maybe actions call in to update their triggers and fire callback if trigger is met
	* actions can have callbacks with values, axis float values or others
	* 
	* input system manages list of actions, sends key state updates, frametime updates
	* should inputsystem delay callbacks until update tick?
	* 
	* 
	*/



	class BaseInputAction
	{
	public:
		virtual ~BaseInputAction() = default;
		virtual void Execute() = 0;
		//virtual void Update(const KeyType inputKey);

		bool IsBoundTo(const KeyType key) 
		{ 
			return m_bindings.contains(key) && key != KeyType::UNKNOWN; 
		}

		void AddBinding(const KeyType key)
		{ 
			if (key != KeyType::UNKNOWN) m_bindings.insert(key);
		}

	protected:
		std::set<KeyType> m_bindings;
		BaseInputAction(KeyType binding) {
			m_bindings.insert(binding);
		}
	};

	class InputAction : public BaseInputAction
	{
	public:
		using Callback = std::function<void()>;

		InputAction(KeyType keyBinding, Callback callback)
			: BaseInputAction(keyBinding), m_callback(std::move(callback)) {}

		void Execute() override {
			if (m_callback) {
				m_callback();
			}
		}

	private:
		Callback m_callback;
	};

	class InputAxisAction : public BaseInputAction 
	{
	public:
		using Callback = std::function<void(float)>;

		InputAxisAction(KeyType keyBinding, Callback callback)
			: BaseInputAction(keyBinding), m_callback(std::move(callback)) {}

		void Execute() override {
			if (m_callback) {
				m_callback(m_axisValue);
			}
		}

		void SetAxisValue(float value) {
			m_axisValue = value;
		}

	private:
		Callback m_callback;
		float m_axisValue = 0.0f;
	};

	// Going to use a global here for now, glfw seems to require some way of accessing which engine it is referring to when using the input functions.
	// Not a fan of this, but I can't think of a better way at the moment
	class InputSystem;
	static InputSystem* g_input = nullptr;

	class InputSystem : public EngineSystem
	{
		
		using AxisActionCallback = std::function<void(float)>;

	public:
		void PollInput();
		void ProcessKeyStateChange(const KeyType key, const KeyState prevState, const KeyState newState);

		void RegisterAction(KeyType keyBinding, InputAction::Callback callback)
		{
			// Dynamic allocation here is not desirable. TODO allocation-free and handles
			m_actions.push_back(std::make_unique<InputAction>(keyBinding, std::move(callback)));
		}

		void RegisterAxisAction(KeyType keyBinding, InputAxisAction::Callback callback)
		{
			m_axisActions.push_back(std::make_unique<InputAxisAction>(keyBinding, std::move(callback)));
		}

	private:
		GLFWwindow* m_window = nullptr;

		void OnCursorMoved(GLFWwindow* window, double xPos, double yPos);
		void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);
		void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
		void OnScroll(GLFWwindow* window, double xOffset, double yOffset);
		void OnWindowClose(GLFWwindow* window);

		KeyboardState m_keyboardState{};
		KeyboardState m_prevKeyboardState{};

		void DrawKeyboardState(const KeyboardState& state, const ImVec2& offset = ImVec2(0, 0), const ImVec2& size = ImVec2(0, 0)) const;

		void UpdateKeyState(const KeyType key, const KeyState newState);

		// Storage for our actions
		std::vector<std::unique_ptr<InputAction>> m_actions;
		std::vector<std::unique_ptr<InputAxisAction>> m_axisActions;

		// Queue for processing action input events
		//std::queue<> need this eventually, not sure what type

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