#pragma once

#include "Systems/EngineSystem.h"

#include "ObjectPool.h"
#include "GUI/DebugGUISubscriber.h"

#include "imgui.h"

#include "stdlibincl.h"

class GLFWwindow;

namespace CE
{	
	enum class KeyType : std::uint8_t
	{
		// WARNING if this enum changes, be sure to audit translation function for GLFW!
		UNKNOWN = 0,

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

	enum class KeyState : unsigned char
	{
		UNKNOWN		= 0,
		RELEASED	= 1 << 0,
		PRESSED		= 1 << 1,
		HELD		= 1 << 2,
		ANY			= 1 << 3
	};

	enum class MouseButtonType : std::uint8_t
	{
		UNKNOWN = 0,
		MOUSE_BUTTON_1,
		MOUSE_BUTTON_2,
		MOUSE_BUTTON_3,
		MOUSE_BUTTON_4,
		MOUSE_BUTTON_5,
		MOUSE_BUTTON_6,
		MOUSE_BUTTON_7,
		MOUSE_BUTTON_8,
		BUTTONS_MAX
	};

	namespace InputUtilities 
	{	
		constexpr KeyState operator|(KeyState lhs, KeyState rhs)
		{
			return static_cast<KeyState>(static_cast<unsigned char>(lhs) | static_cast<unsigned char>(rhs));
		}

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

		MouseButtonType GLFWMouseButtonToMouseButtonType(int button);
		int MouseButtonTypeToGLFWMouseButton(const MouseButtonType button);

		KeyState GLFWActionToKeyState(int action);

		const char* GetKeyStateName(const KeyState state);
		const char* GetKeyName(const KeyType key);
		const char* GetMouseButtonName(const MouseButtonType button);
	}

	struct KeyboardState
	{
	public:
		KeyboardState() : keys() {}

		KeyState GetKey(const KeyType key) const
		{
			return keys[static_cast<std::underlying_type_t<KeyType>>(key)];
		}

		bool GetKeyHeld(const KeyType key) const
		{
			return keys[static_cast<std::underlying_type_t<KeyType>>(key)] == KeyState::HELD;
		}

	private:
		constexpr static std::size_t KEYBOARD_MAX = static_cast<std::size_t>(KeyType::KEYS_MAX) - 1;
		
		inline void SetKey(const KeyType key, const KeyState state)
		{
			keys[static_cast<std::underlying_type_t<KeyType>>(key)] = state;
		}

		KeyState keys[KEYBOARD_MAX];

		friend class InputSystem;
	};

	struct MouseState
	{
	public:
		MouseState() : buttons() {}

		KeyState GetButton(const MouseButtonType button) const
		{
			return buttons[static_cast<std::underlying_type_t<MouseButtonType>>(button)];
		}

	private:
		constexpr static std::size_t MOUSE_BUTTON_MAX = static_cast<std::size_t>(MouseButtonType::BUTTONS_MAX) - 1;

		inline void SetButton(const MouseButtonType button, const KeyState state)
		{
			buttons[static_cast<std::underlying_type_t<MouseButtonType>>(button)] = state;
		}

		KeyState buttons[MOUSE_BUTTON_MAX];

		friend class InputSystem;
	};

	/*
	* MISC NOTES
	* 
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
	* input action maps key bindings to triggers, passing along update information
	*	carries and calls callback once trigger is met
	* 
	* input trigger checks if conditions to fire are met
	*	tracks duration
	*	tracks and matches state transitions (OnPressed, OnReleased)
	* 
	* pass a reference object or another way of getting input pre-post state
	* could pass a raw pointed back to the input system or a premade info struct
	* input system could keep previous frame data and only check inputs on update
	*	this would be simpler, but would miss out on any input between frames
	*	bad the more I think about it, taps between frames wouldn't be picked up at all
	* 
	* input system and triggers need to process every input, but maybe only send callbacks
	*	once per frame, at a specific time in the update cycle.
	* 
	*/

	struct InputKnowledge
	{
		InputKnowledge() :
			previousBoardState(),
			currentBoardState(),
			previousMouseState(),
			currentMouseState(),
			currentTime(0) {}

		KeyboardState previousBoardState;
		KeyboardState currentBoardState;

		KeyType lastKey = KeyType::UNKNOWN;
		KeyState lastKeyState = KeyState::UNKNOWN;

		MouseState currentMouseState;
		MouseState previousMouseState;

		MouseButtonType lastButton = MouseButtonType::UNKNOWN;
		KeyState lastButtonState = KeyState::UNKNOWN;

		unsigned long long currentTime;
		bool bNeedsUpdate = true;
	};

	enum class InputEventType { UNKNOWN, PRESSED, RELEASED, HELD, AXIS };

	struct InputEvent {
	private:
		InputEventType type;
		union
		{
			KeyType key = KeyType::UNKNOWN;
			AxisType axis = AxisType::UNKNOWN;
			float value1D = 0.0f;  // 1D input 
			struct {
				float valueX = 0.0f;
				float valueY = 0.0f;
			} value2D;
		};

	public:
		InputEvent() : type(InputEventType::UNKNOWN) {}

		void SetKey(KeyType k)
		{
			
		}
	};
	
	class InputAction
	{
	public:
		virtual ~InputAction() = default;
		virtual void ProcessEvents(const std::vector<InputEvent>& events) = 0;
		virtual void ExecuteCallback() = 0;
		virtual void Update(float deltaTime) {};

		bool m_bTriggered = false;
	};

	class ButtonAction : public InputAction
	{
	public:
		explicit ButtonAction(KeyType key) : m_key(key) {}

		void ProcessEvents(const std::vector<InputEvent>& events, float deltaTime) override
		{
			for (const auto& event : events)
			{
				if (event.key == m_key && event.type == InputEventType::PRESSED)
				{
					m_activated = true;
				}
			}
		}

		bool IsActive() const { return m_activated; }

	private:
		KeyType m_key;
		bool m_activated = false;
	};

	class HoldAction : public InputAction
	{
		using Callback = std::function<void(float)>;

	public:
		explicit HoldAction(KeyType key) : m_key(key) {}

		void ProcessEvents(const std::vector<InputEvent>& events) override
		{
			for (const auto& event : events)
			{
				if (event.key == m_key)
				{
					if (event.type == InputEventType::PRESSED)
					{
						m_holdTime = 0.0f;
						m_holding = true;
					}
					else if (event.type == InputEventType::RELEASED)
					{
						m_holding = false;
					}
				}
			}
		}

		float GetHoldTime() const { return m_holdTime; }

		void Update(float deltaTime) override
		{
			if (m_holding)
			{
				m_holdTime += deltaTime;
				if (m_holdTime > m_threshold)
				{
					m_bTriggered = true;
				}
			}
		}

		void ExecuteCallback() override
		{
			if (m_callback)
			{
				m_callback(m_holdTime);
			}
			else
			{
				LOG_ERROR(INPUT, "No callback registered for InputAction!");
			}
		}

	private:
		Callback m_callback;
		KeyType m_key;
		bool m_holding = false;
		float m_holdTime = 0.f;
		float m_threshold = 0.f;
	};

	class AxisAction : public InputAction
	{
	public:
		explicit AxisAction(KeyType key) : m_key(key) {}

		void ProcessEvents(const std::vector<InputEvent>& events, float deltaTime) override
		{
			for (const auto& event : events)
			{
				if (event.key == m_key && event.type == InputEventType::AXIS_MOVED)
				{
					m_value += event.value * deltaTime;
				}
			}
		}

		float GetValue() const { return m_value; }

	private:
		KeyType m_key;
		float m_value = 0.0f;
	};

	class InputSystem;
	extern InputSystem* g_input;

	class InputSystem final : public EngineSystem, IDebugGUISubscriber
	{		
		using InputActionHandle = GenericHandle;

	public:
		InputSystem(Engine* engine) : EngineSystem(engine) {};
	
		/* CEngineSystem Interface */
	public:
		virtual std::string Name() const override { return "Input System"; }
	protected:
		virtual void Startup() override;
		virtual void Shutdown() override;
		friend class Engine;
		

		/* IDebugGUISubscriber Interface */
	public:
		virtual void OnDrawGUI() override;
		virtual std::string_view GetDebugMenuName() override { return "Input"; }


		/* Input System API */
	public:
		void PollInput();
		
		InputActionHandle RegisterAction(std::string actionName, KeyType keyBinding, InputAction::Callback callback, KeyState to = KeyState::PRESSED, KeyState from = KeyState::RELEASED)
		{
			return m_actions.Create(actionName, keyBinding, std::move(callback), to, from);
		}

		/* Removing Mouse Input Actions for now, TODO mouse input implementation
		InputActionHandle RegisterAction(std::string actionName, MouseButtonType mouseBinding, InputAction::Callback callback, KeyState to = KeyState::PRESSED, KeyState from KeyState::RELEASED)
		{
			return m_actions.Create(actionName, mouseBinding, std::move(callback), to, from);
		}
		*/

		bool RemoveAction(InputActionHandle actionHandle);

	private:
		GLFWwindow* m_window = nullptr;

		void OnCursorMoved(GLFWwindow* window, double xPos, double yPos);
		void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);
		void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
		void OnScroll(GLFWwindow* window, double xOffset, double yOffset);
		void OnWindowClose(GLFWwindow* window);

		// Does this require thread mutex locks? input changes knowledge base while we're processing
		InputKnowledge m_inputKnowledge;

		const InputKnowledge& GetKnowledge()
		{
			return m_inputKnowledge;
		}

		// Manually updates our input knowledge state, used only on startup
		void UpdateInputKnowledge();

		// Update input knowledge for a KeyType key at a particular KeyState
		void UpdateKeyState(const KeyType key, const KeyState newState);
		
		// Update input knowledge for a mouse button with a keystate
		void UpdateMouseButtonState(const MouseButtonType button, const KeyState newState);

		// Iterates through actions and updates
		void ProcessActions();

		void ProcessTriggers();

		// Iterates through triggered actions and fires callbacks
		void ProcessCallbacks();

		std::vector<ActionTriggerBase*> m_triggers;
		ObjectPool<InputAction, 20> m_actions;
		std::queue<InputActionHandle> m_triggeredActions;


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