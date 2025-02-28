#pragma once

#include "Input/Input.h"

#include "stdlibincl.h"

namespace CE
{
	class InputAction {
	public:
		using Callback = std::function<void()>;
		using CallbackFloat = std::function<void(float)>;

		virtual ~InputAction() = default;

		void AddKeyBind(KeyType key) { keyBinds.push_back(key); }
		void AddAxisBind(AxisType axis) { axisBinds.push_back(axis); }

		bool IsKeyBound(KeyType key) const
		{
			return std::find(keyBinds.begin(), keyBinds.end(), key) != keyBinds.end();
		}

		bool IsAxisBound(AxisType axis) const
		{
			return std::find(axisBinds.begin(), axisBinds.end(), axis) != axisBinds.end();
		}

		virtual void ProcessEvent(const InputEvent& event) = 0;
		virtual void Update(float deltaTime) = 0;

	protected:
		std::vector<KeyType> keyBinds;
		std::vector<AxisType> axisBinds;
		bool triggered = false;
	};

	class PressedAction : public InputAction
	{
	public:
		explicit PressedAction(Callback cb) : m_callback(std::move(cb)) {}

		void ProcessEvent(const InputEvent& event) override
		{
			if (event.GetKey() && IsKeyBound(event.GetKey()->key) && event.GetKey()->state == KeyState::PRESSED)
			{
				triggered = true;
			}
		}

		void Update(float deltaTime) override
		{
			if (triggered)
			{
				triggered = false;
				if (m_callback)
				{
					m_callback();
				}
			}
		}

	private:
		Callback m_callback;
	};

	class HoldAction : public InputAction
	{
	public:
		HoldAction(Callback cb, float holdThreshold = 0.f)
			: m_callback(std::move(cb)), m_threshold(holdThreshold) {}

		void ProcessEvent(const InputEvent& event) override
		{
			if (event.GetKey() && IsKeyBound(event.GetKey()->key))
			{
				if (event.GetKey()->state == KeyState::PRESSED)
				{
					m_bHolding = true;
					m_holdTime = 0.f;
				}
				if (event.GetKey()->state == KeyState::RELEASED)
				{
					m_bHolding = false;
					m_holdTime = 0.f;
				}
			}
		}

		void Update(float deltaTime) override
		{
			if (m_bHolding)
			{
				m_holdTime += deltaTime;
				if (m_holdTime >= threshold)
				{
					m_callback();
				}
			}
		}

	private:
		Callback m_callback;
		bool m_bHolding = false;
		float m_holdTime = 0.f;
		float m_threshold = 0.f;
	};

	// Axis actions are a little special, they require a set of bindings.
	// Each binding pairs with a value to add to the internal value of the action.
	// Ex. W = 1.f, S = -1.f for forward/backward.

	// Multiple binds are allowed (might want to do this for all actions too) So W, Up Arrow = 1.f
	class AxisAction : public InputAction
	{
	public:
		explicit AxisAction(CallbackFloat cb) : callback(std::move(cb)) {}

		void AddKeyBind(KeyType key, float value)
		{
			keyBinds[key] = value;
		}

		void AddAxisBind(AxisType axis) {
			axisBinds.push_back(axis);
		}

		void ProcessEvent(const InputEvent& event) override
		{
			// Handle digital key input
			if (event.GetKey()) {
				KeyType key = event.GetKey()->key;
				if (keyBinds.find(key) != keyBinds.end()) {
					float value = keyBinds[key] * (event.GetKey()->state == KeyState::PRESSED ? 1.0f : 0.0f);
					accumulatedValue += value; // Adjust value when pressed/released
					callback(accumulatedValue);
				}
			}

			// Handle analog axis input
			if (event.GetAxis() && IsAxisBound(event.GetAxis()->axis)) {
				callback(event.GetAxis()->value);
			}
		}

		void Reset() {
			accumulatedValue = 0.0f;
		}

	private:
		CallbackFloat callback;
		std::unordered_map<KeyType, float> keyBinds; // Map of digital key values
		float accumulatedValue = 0.0f; // Stores sum of keybind values
	};
}