#include "Systems/World/EntitySubsystem.h"

#include "imgui.h"

namespace CE
{
	void EntitySubsystem::DrawEntityPool()
	{
		if (ImGui::CollapsingHeader("Entities", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& entityPool = (*m_entityPool.get());
			for (auto& [handle, entity] : entityPool)
			{
				ImGui::PushID(handle.raw());
				ImGui::Text("Handle: 0x%X", handle);
				if (ImGui::CollapsingHeader("Components"))
				{
					auto& components = entity.GetComponentList();
					for (auto compHandle : components)
					{
						ImGui::Text("Component Handle: 0x%X", compHandle.raw());
					}
				}
				ImGui::PopID();
			}
		}
	}
}