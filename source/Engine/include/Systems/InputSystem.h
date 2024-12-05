#pragma once

#include "Systems/EngineSystem.h"

#include "ObjectPool.h"

#include "imgui.h"

#include <cstdint>
#include <iostream>
#include <functional>
#include <queue>
#include <iterator>

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

	enum class KeyState : unsigned char {
		UNKNOWN = 0,
		RELEASED = 1 << 0,
		PRESSED = 1 << 1,
		HELD = 1 << 2
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
		KeyboardState previousBoardState;
		KeyboardState currentBoardState;

		KeyType lastKey = KeyType::UNKNOWN;
		KeyState lastKeyState = KeyState::UNKNOWN;

		unsigned long long currentTime;
		bool bNeedsUpdate = true;

		InputKnowledge() : previousBoardState(), currentBoardState(), currentTime(0) {}
	};

	class IInputActionBase {
	public:
		virtual ~IInputActionBase() = default;
		virtual void Execute() = 0;
		virtual void Update(const InputKnowledge& knowledge) = 0;
		virtual bool TryConsumeTrigger() = 0;
		virtual bool IsBoundTo(const KeyType key) const = 0;
		virtual KeyType GetBinding() const = 0;
	};

	class InputAction : public IInputActionBase
	{
	public:
		using Callback = std::function<void()>;

		InputAction() : m_callback([] {}) {}
		InputAction(KeyType binding, Callback cb, KeyState to, KeyState from) :
			m_binding(binding),
			m_callback(std::move(cb)),
			m_toState(to),
			m_fromState(from)
		{}

		void Execute() override 
		{
			m_callback();
			m_bIsTriggered = false;
		}

		bool TryConsumeTrigger() override
		{
			if (m_bIsTriggered)
			{
				m_bIsTriggered = false;
				return true;
			}
			return false;
		}

		bool IsBoundTo(const KeyType key) const override
		{
			return key == m_binding;
		}

		KeyType GetBinding() const override
		{
			return m_binding;
		}

		void Update(const InputKnowledge& knowledge) override
		{
			if (knowledge.lastKey != m_binding)
			{
				return;
			}

			if (knowledge.lastKeyState == m_toState
				&& knowledge.previousBoardState.GetKey(knowledge.lastKey) == m_fromState)
			{
				m_bIsTriggered = true;
			}
			else
			{
				m_bIsTriggered = false;
			}
		}
	private:
		bool m_bIsTriggered = false;
		Callback m_callback;
		KeyType m_binding = KeyType::UNKNOWN;
		KeyState m_fromState = KeyState::UNKNOWN;
		KeyState m_toState = KeyState::UNKNOWN;
	};

	class InputAxisAction : public IInputActionBase
	{
	public:
		using Callback = std::function<void(float)>;

		InputAxisAction() : m_callback([](float) {}) {}
		InputAxisAction(KeyType binding, Callback callback)
			: m_binding(binding), m_callback(std::move(callback)) {}

		bool TryConsumeTrigger() override
		{
			if (m_bIsTriggered)
			{
				m_bIsTriggered = false;
				return true;
			}
			return false;
		}

		bool IsBoundTo(const KeyType key) const override
		{
			return key == m_binding;
		}

		void Execute() override
		{
			m_callback(m_axisValue);
		}

		void Update(const InputKnowledge& knowledge) override
		{
			if (knowledge.lastKey == m_binding)
			{
				if (knowledge.lastKeyState == KeyState::PRESSED || knowledge.lastKeyState == KeyState::HELD)
				{
					m_axisValue += 0.05f;
				}
			}
		}

		void SetAxisValue(float value)
		{
			m_axisValue = value;
		}

	private:
		float m_axisValue = 0.0f;
		bool m_bIsTriggered = false;
		Callback m_callback;
		KeyType m_binding = KeyType::UNKNOWN;
	};

	// A fun idea! Essentially a wrapper for any derived class of BaseInputAction that uses the IInputAction
	// interface. But now with contiguous storage! List of InputActionWrappers can be created and stored sequentially (this assumes all derived input actions are roughly of similar size).
	// A macro here could help automate updating this class... might do that later
	/*
	class InputActionWrapper
	{
	private:
		union Storage
		{
			// Add new action types here
			InputAction u_action;
			InputAxisAction u_axisAction;

			Storage() {}
			~Storage() {}
		} m_storage;

		enum class ActionType
		{
			None,
			// Add new action types here
			InputAction,
			InputAxisAction
		} m_actionType;

	public:
		InputActionWrapper() : m_actionType(ActionType::None) {}
		~InputActionWrapper() 
		{
			switch (m_actionType)
			{
			case ActionType::InputAction:
				m_storage.u_action.~InputAction(); break;
			case ActionType::InputAxisAction:
				m_storage.u_axisAction.~InputAxisAction(); break;
			default:
				// nothing in m_storage, all good
				break;
			}
		}

		template<typename DerivedType, typename... Args>
		static InputActionWrapper Create(Args&&... args)
		{
			InputActionWrapper wrapper;
			if constexpr (std::is_same_v<DerivedType, InputAction>)
			{
				new (&wrapper.m_storage.u_action) InputAction(std::forward<Args>(args)...);
				wrapper.m_actionType = ActionType::InputAction;
			}
			else if constexpr (std::is_same_v<DerivedType, InputAxisAction>)
			{
				new (&wrapper.m_storage.u_axisAction) InputAxisAction(std::forward<Args>(args)...);
				wrapper.m_actionType = ActionType::InputAxisAction;
			}
			return wrapper;
		}

		IInputActionBase& GetAction()
		{
			switch (m_actionType)
			{
			case ActionType::InputAction:
				return m_storage.u_action;
			case ActionType::InputAxisAction:
				return m_storage.u_axisAction;
			default:
				// uh oh
				break;
			}
		}

		const IInputActionBase& GetAction() const
		{
			switch (m_actionType)
			{
			case ActionType::InputAction:
				return m_storage.u_action;
			case ActionType::InputAxisAction:
				return m_storage.u_axisAction;
			default:
				break;
			}
		}

		void Execute() { GetAction().Execute(); }
		void Update(const InputKnowledge& knowledge) { GetAction().Update(knowledge); }
		bool IsTriggered() const { return GetAction().IsTriggered(); };
		bool IsBoundTo(const KeyType key) const { return GetAction().IsBoundTo(key); }
	};
	*/


	// Going to use a global here for now, glfw seems to require some way of accessing which engine it is referring to when using the input functions.
	// Not a fan of this, but I can't think of a better way at the moment
	class InputSystem;
	static InputSystem* g_input = nullptr;

	class InputSystem : public EngineSystem
	{
		
		using InputActionHandle = GenericHandle;
		

	public:
		void PollInput();
		
		void RegisterAction(KeyType keyBinding, InputAction::Callback callback, KeyState to = KeyState::PRESSED, KeyState from = KeyState::RELEASED)
		{
			m_actions.push_back(std::make_shared<InputAction>(keyBinding, std::move(callback), to, from));
		}

		void RegisterAxisAction(KeyType keyBinding, InputAxisAction::Callback callback)
		{
			//m_axisActions.push_back(std::make_unique<InputAxisAction>(keyBinding, std::move(callback)));
		}

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
		
		// Iterates through actions and updates
		void ProcessActions();

		// Iterates through triggered actions and fires callbacks
		void ProcessCallbacks();

		// Polymorphic storage option for our actions. Undesirable for cache coherency
		std::vector<std::shared_ptr<IInputActionBase>> m_actions;

		// This was a test of a allocation free InputAction setup, seemed like overkill for only a few actions
		//ObjectPool<InputActionWrapper, InputActionHandle, 100> m_actions;

		// Queue for processing action input events
		std::queue<std::weak_ptr<IInputActionBase>> m_triggeredActions;

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