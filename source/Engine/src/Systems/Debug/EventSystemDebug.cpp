#include "Systems/Debug/EventSystemDebug.h"

#include "imgui.h"
#include "stdlibincl.h"

#ifdef CDEBUG
namespace CE
{
	void EventSystemDebug::OnDrawGUI()
	{
		if (m_owner == nullptr) { return; }

		ImGui::Begin("Event System Debug");
		if (ImGui::CollapsingHeader("Queues", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto [typeIndex, queue] : m_owner->m_queues)
			{
				std::string queueName = std::string(queue->GetQueueName());
				ImGui::Bullet();
				ImGui::Text(queueName.c_str());
			}
		}

		if (ImGui::CollapsingHeader("Pending Events", ImGuiTreeNodeFlags_DefaultOpen))
		{

			ImGui::Text("Show event queues here...");
		}

		if (ImGui::CollapsingHeader("Listeners", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Show listeners here...");
		}

		ImGui::End();
	}
}
#endif