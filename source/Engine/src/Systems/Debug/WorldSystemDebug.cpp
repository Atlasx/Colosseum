#include "Systems/Debug/WorldSystemDebug.h"

#ifdef CDEBUG

#include "Systems/WorldSystem.h"
#include "imgui.h"

namespace CE
{
	void WorldSystemDebug::OnDrawGUI()
	{
		if (m_owner == nullptr) { return; }

		ImGui::Begin("World System Debug");

		m_owner->m_components.DrawComponentPools();

		m_owner->m_entities.DrawEntityPool();

		ImGui::End();
	}
}

#endif