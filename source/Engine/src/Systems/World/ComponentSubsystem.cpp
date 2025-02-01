#include "Systems/World/ComponentSubsystem.h"

#include "imgui.h"

namespace CE
{
	void ComponentSubsystem::DrawComponentPools()
	{
		// Within an imgui window context already here
		if (ImGui::CollapsingHeader("Registered Components"))
		{
			for (std::size_t i = 0; i < m_componentIDs.size(); i++)
			{
				ImGui::Text("%d: %d", i, m_componentIDs[i]);
			}
		}

	}
}