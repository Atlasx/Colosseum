#pragma once

#include "stdlibincl.h"

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
		UNKNOWN = 0,
		RELEASED = 1 << 0,
		PRESSED = 1 << 1,
		HELD = 1 << 2,
		ANY = 1 << 3
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

		MouseState currentMouseState;
		MouseState previousMouseState;

		unsigned long long currentTime;
		bool bNeedsUpdate = true;
	};

	enum class InputEventType
	{
		UNKNOWN = 0,
		KEY,
		AXIS,
		AXIS_2D,
		MOUSE_BUTTON
	};

	enum class AxisType
	{
		UNKNOWN = 0,
		MOUSE_X,
		MOUSE_Y,
		MOUSE_WHEEL,
		TRIGGER_LEFT,
		TRIGGER_RIGHT
	};

	enum class Axis2DType
	{
		UNKNOWN = 0,
		MOUSE_XY,
		STICK_LEFT,
		STICK_RIGHT
	};

	struct InputEvent {

	public:

		InputEvent() : type(InputEventType::UNKNOWN), keyInfo({KeyType::UNKNOWN, KeyState::UNKNOWN}) {}

		struct KeyInfo
		{
			KeyType key = KeyType::UNKNOWN;
			KeyState state = KeyState::UNKNOWN;
		};

		struct AxisInfo
		{
			AxisType axis = AxisType::UNKNOWN;
			float value = 0.0f;
		};

		struct Axis2DInfo
		{
			Axis2DType axis = Axis2DType::UNKNOWN;
			struct { float X = 0.0f; float Y = 0.0f; } value;
		};

		struct MouseButtonInfo
		{
			MouseButtonType button = MouseButtonType::UNKNOWN;
			KeyState state = KeyState::UNKNOWN;
		};

		void SetKey(KeyType k, KeyState s)
		{
			type = InputEventType::KEY;
			keyInfo.key = k;
			keyInfo.state = s;
		}

		void SetAxis(AxisType a, float v)
		{
			type = InputEventType::AXIS;
			axisInfo.value = v;
		}

		void SetAxis2D(Axis2DType a, float x, float y)
		{
			type = InputEventType::AXIS_2D;
			axis2DInfo.axis = a;
			axis2DInfo.value.X = x;
			axis2DInfo.value.Y = y;
		}

		void SetMouseButton(MouseButtonType mb, KeyState s)
		{
			type = InputEventType::MOUSE_BUTTON;
			mouseButtonInfo.button = mb;
			mouseButtonInfo.state = s;
		}

		std::optional<const KeyInfo> GetKey() const
		{
			if (type == InputEventType::KEY)
			{
				return keyInfo;
			}
			return std::nullopt;
		}

		std::optional<const AxisInfo> GetAxis() const
		{
			if (type == InputEventType::AXIS)
			{
				return axisInfo;
			}
			return std::nullopt;
		}

		std::optional<const Axis2DInfo> GetAxis2D() const
		{
			if (type == InputEventType::AXIS_2D)
			{
				return axis2DInfo;
			}
			return std::nullopt;
		}

		std::optional<const MouseButtonInfo> GetMouseButton() const
		{
			if (type == InputEventType::MOUSE_BUTTON)
			{
				return mouseButtonInfo;
			}
			return std::nullopt;
		}

	private:
		InputEventType type;
		union
		{
			KeyInfo keyInfo;
			AxisInfo axisInfo;
			Axis2DInfo axis2DInfo;
			MouseButtonInfo mouseButtonInfo;
		};

	};
}