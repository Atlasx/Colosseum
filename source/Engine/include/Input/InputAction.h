#pragma once

#include "Input/Input.h"

#include "stdlibincl.h"

namespace CE
{
	class InputAction
	{
	public:
		virtual ~InputAction() = default;
		virtual void ProcessEvents(const std::vector<InputEvent>& events) = 0;
		virtual void ExecuteCallback() = 0;
		virtual void Update(float deltaTime) {};

		bool IsTriggered() const { return m_bTriggered; }
	protected:
		bool m_bTriggered = false;
	};

	class PressedAction : public InputAction
	{
		using Callback = std::function<void()>;

	public:
		explicit PressedAction(KeyType key, Callback cb) : m_key(key), m_callback(std::move(cb)) {}

		void ProcessEvents(const std::vector<InputEvent>& events) override;
		void ExecuteCallback() override;

	private:
		KeyType m_key;
		Callback m_callback;
	};

	class HoldAction : public InputAction
	{
		using Callback = std::function<void(float)>;

	public:
		explicit HoldAction(KeyType key, Callback cb) : m_key(key), m_callback(std::move(cb)) {}

		void ProcessEvents(const std::vector<InputEvent>& events) override;
		void Update(float deltaTime) override;
		void ExecuteCallback() override;

		float GetHoldTime() const { return m_holdTime; }

	private:
		Callback m_callback;
		KeyType m_key;
		bool m_holding = false;
		float m_holdTime = 0.f;
		float m_threshold = 0.f;
	};

	// Axis actions are a little special, they require a set of bindings.
	// Each binding pairs with a value to add to the internal value of the action.
	// Ex. W = 1.f, S = -1.f for forward/backward.

	// Multiple binds are allowed (might want to do this for all actions too) So W, Up Arrow = 1.f
	class AxisAction : public InputAction
	{
		using Callback = std::function<void(float)>;


	public:
		explicit AxisAction(KeyType key, float sense, Callback cb) :
			m_sensitivity(sense),
			m_callback(std::move(cb)),
			m_axisBinds()
		{
			// Test KeyBind
			AddKeyBind(key, 1.f);
		}

		void ProcessEvents(const std::vector<InputEvent>& events) override;
		void Update(float deltaTime) override;
		void ExecuteCallback() override;

		void AddKeyBind(KeyType key, float value);
		float GetValue() const { return m_value; }

	private:
		struct AxisBind
		{
			KeyType key;
			float value;
		};

		Callback m_callback;
		float m_value = 0.0f;
		float m_targetValue = 0.0f;
		float m_sensitivity = 15.f;


		std::array<AxisBind, 4> m_axisBinds;
		size_t m_numBindings = 0;

		bool HasBinding(KeyType key) const;
		float GetBindingValue(KeyType key) const;
	};
}