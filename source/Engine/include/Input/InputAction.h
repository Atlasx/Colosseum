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

		void AddKeyBind(KeyType key) { m_keyBinds.push_back(key); }
		void AddAxisBind(AxisType axis) { m_axisBinds.push_back(axis); }

		bool IsKeyBound(KeyType key) const
		{
			return std::find(m_keyBinds.begin(), m_keyBinds.end(), key) != m_keyBinds.end();
		}

		bool IsAxisBound(AxisType axis) const
		{
			return std::find(m_axisBinds.begin(), m_axisBinds.end(), axis) != m_axisBinds.end();
		}

		KeyType GetBinding() const
		{
			return m_keyBinds[0];
		}

		virtual void ProcessEvent(const InputEvent& event) = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void Trigger()
		{
			m_triggered = false;
			if (m_singleUse) { m_ready = false; }
		};

	protected:
		std::vector<KeyType> m_keyBinds;
		std::vector<AxisType> m_axisBinds;
		bool m_triggered = false;
		bool m_singleUse = false;
		bool m_ready = true;
	};

	class PressedAction : public InputAction
	{
	public:
		explicit PressedAction(KeyType binding, Callback cb) : m_callback(std::move(cb))
		{
			AddKeyBind(binding);
		}

		void ProcessEvent(const InputEvent& event) override
		{
			if (m_ready == false) { return; }

			if (event.GetKey() && IsKeyBound(event.GetKey()->key) && event.GetKey()->state == KeyState::PRESSED)
			{
				m_triggered = true;
			}
		}

		void Update(float deltaTime) override
		{
			if (m_triggered)
			{
				if (m_singleUse) { m_ready = false; }
				if (m_callback) { Trigger(); }
			}
		}

		void Trigger() override
		{
			InputAction::Trigger();
			m_callback();
		}

	private:
		Callback m_callback;
	};

	class HoldAction : public InputAction
	{
	public:
		HoldAction(KeyType binding, float holdThreshold, Callback cb) : m_callback(std::move(cb)), m_threshold(holdThreshold)
		{
			AddKeyBind(binding);
		}

		void ProcessEvent(const InputEvent& event) override
		{
			if (m_ready == false) { return; }

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
				if (m_holdTime >= m_threshold)
				{
					Trigger();
				}
			}
		}

		void Trigger() override
		{
			InputAction::Trigger();
			m_callback();
			m_holdTime = 0.f;
			m_bHolding = false;
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

		void AddAxisBind(AxisType axis)
		{
			m_axisBinds.push_back(axis);
		}

		void ProcessEvent(const InputEvent& event) override
		{
			if (m_ready == false) { return; }

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

		void Update(float deltaTime) override
		{

		}

		void Reset()
		{
			accumulatedValue = 0.0f;
		}

		void Trigger() override
		{
			callback(0.0f);
		}

	private:
		CallbackFloat callback;
		std::unordered_map<KeyType, float> keyBinds; // Map of digital key values
		float accumulatedValue = 0.0f; // Stores sum of keybind values
	};
}