#pragma once
#ifdef CDEBUG

#include "GUI/DebugGUI.h"

namespace CE
{
	class LogSystem;

	class LogSystemDebug : public IDebugGUI
	{
		LogSystem* m_owner;

	public:
		LogSystemDebug(LogSystem* owner) : m_owner(owner) {}
		~LogSystemDebug() override = default;

		/* IDebugGUISubscriber Interface */
		void OnDrawGUI() override;
		std::string_view GetDebugMenuName() override { return "Debug Log"; }
	};
}
#endif