#pragma once
#ifdef CDEBUG

#include "GUI/DebugGUI.h"

namespace CE
{
	class EventSystem;

	class EventSystemDebug : public IDebugGUI
	{
		EventSystem* m_owner;

	public:
		EventSystemDebug(EventSystem* owner) : m_owner(owner) {};
		~EventSystemDebug() override = default;

		/* IDebugGUISubscriber Interface */
		void OnDrawGUI() override;
		std::string_view GetDebugMenuName() override { return "Events"; }
	};
}
#endif