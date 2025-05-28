#pragma once

#ifdef CDEBUG

#include "GUI/DebugGUI.h"

namespace CE
{
	class InputSystem;

	class InputSystemDebug : public IDebugGUI
	{
		InputSystem* m_owner;

	public:
		InputSystemDebug(InputSystem* owner) : m_owner(owner) {}

		void OnDrawGUI() override;
		std::string_view GetDebugMenuName() override { return "Input"; }
	};
}

#endif