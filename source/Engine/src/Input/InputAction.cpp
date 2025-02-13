#include "Input/InputAction.h"

#include "Systems/LogSystem.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

namespace CE
{
	// ==================================
	//			  PressedAction
	// ==================================
	void PressedAction::ProcessEvents(const std::vector<InputEvent>& events)
	{
		for (const auto& ev : events)
		{
			if (auto info = ev.GetKey())
			{
				if (info->key == m_key)
				{
					m_bTriggered = true;
				}
			}
		}
	}

	void PressedAction::ExecuteCallback()
	{
		if (m_callback)
		{
			m_callback();
		}
		else
		{
			LOG_ERROR(INPUT, "No callback registered for Hold InputAction!");
		}
	}

	// ==================================
	//				HoldAction
	// ==================================
	void HoldAction::ProcessEvents(const std::vector<InputEvent>& events)
	{
		for (const auto& ev : events)
		{
			if (auto info = ev.GetKey())
			{
				if (info->key == m_key)
				{
					if (info->state == KeyState::PRESSED)
					{
						m_holdTime = 0.0f;
						m_holding = true;
					}
					else if (info->state == KeyState::RELEASED)
					{
						m_holding = false;
					}
				}
			}
		}
	}

	void HoldAction::Update(float deltaTime)
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

	void HoldAction::ExecuteCallback()
	{
		if (m_callback)
		{
			m_callback(m_holdTime);
		}
		else
		{
			LOG_ERROR(INPUT, "No callback registered for Hold InputAction!");
		}
	}


	// ==================================
	//				AxisAction
	// ==================================
	void AxisAction::AddKeyBind(KeyType key, float value)
	{
		m_axisBinds[m_numBindings++] = { key, value };
	}

	void AxisAction::ProcessEvents(const std::vector<InputEvent>& events)
	{
		m_bTriggered = false;
		m_targetValue = 0.0f;
		for (const auto& ev : events)
		{
			if (auto info = ev.GetKey())
			{
				if (HasBinding(info->key))
				{
					m_targetValue += GetBindingValue(info->key);
					m_bTriggered = true;
				}
			}
		}
	}

	void AxisAction::Update(float deltaTime)
	{
		if (m_bTriggered || glm::epsilonNotEqual(m_value, 0.0f, glm::epsilon<float>()) )
		{
			m_value = glm::mix(m_value, m_targetValue, deltaTime * m_sensitivity);
			glm::clamp(m_value, -1.f, 1.f);
		}
	}

	void AxisAction::ExecuteCallback()
	{
		if (m_callback)
		{
			m_callback(m_value);
		}
		else
		{
			LOG_ERROR(INPUT, "Axis Action triggered without callback!");
		}
	}

	bool AxisAction::HasBinding(KeyType key) const
	{
		for (auto& [keyBind, val] : m_axisBinds)
		{
			if (keyBind == key)
			{
				return true;
			}
		}
		return false;
	}

	float AxisAction::GetBindingValue(KeyType key) const
	{
		for (auto& [keyBind, val] : m_axisBinds)
		{
			if (keyBind == key)
			{
				return val;
			}
		}
		return 0.0f;
	}
}