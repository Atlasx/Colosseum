#pragma once

#ifdef CDEBUG

#include "GUI/DebugGUI.h"

namespace CE
{
	class WorldSystem;

	class WorldSystemDebug : public IDebugGUI
	{
		WorldSystem* m_owner;

	public:
		WorldSystemDebug(WorldSystem* owner) : m_owner(owner) {}
		~WorldSystemDebug() override = default;

		void OnDrawGUI() override;
		std::string_view GetDebugMenuName() override { return "World"; }
	};
}

#endif