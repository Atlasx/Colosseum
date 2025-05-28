#pragma once

#include "GUI/DebugGUI.h"
#include "Systems/EventSystem.h"

namespace CE
{
	class EventSystemDebug : public IDebugGUI
	{
	public:
		EventSystemDebug(EventSystem* owner) : m_owner(owner) {}

	private:
		EventSystem* m_owner;

		/* IDebugGUISubscriber Interface */
	public:
		void OnDrawGUI() override;
		std::string_view GetDebugMenuName() override { return "Events"; }
	};
}