#include "Systems/World/EntitySubsystem.h"

#include "imgui.h"

namespace CE
{
	void EntitySubsystem::DrawEntityPool()
	{
		if (ImGui::CollapsingHeader("Entities", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Indent();
			auto& entityPool = (*m_entityPool.get());
			for (auto& [handle, entity] : entityPool)
			{
				ImGui::PushID(handle.raw());
				char headerName[16];
				sprintf(headerName, "Entity: 0x%X8", static_cast<std::uint64_t>(handle.raw()));
				if (ImGui::CollapsingHeader(headerName))
				{
					ImGui::Indent();
					if (ImGui::CollapsingHeader("Components"))
					{
						auto& components = entity.GetComponentList();
						for (auto compHandle : components)
						{
							ImGui::Text("Component Handle: 0x%X8", compHandle.raw());
						}
					}
					ImGui::Unindent();
				}
				ImGui::PopID();
			}
			ImGui::Unindent();
		}
	}
}