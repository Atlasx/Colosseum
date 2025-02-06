#include "Systems/World/ComponentSubsystem.h"

#include "imgui.h"

namespace CE
{
	void ComponentSubsystem::DrawComponentPools()
	{
		// Already within an imgui window context 
		for (std::size_t i = 0; i < m_componentIDs.size(); i++)
		{
			ImGui::PushID(m_componentIDs[i].hash_code());
			ImGui::Text(m_componentNames[i].c_str());

			PoolInfo pi = m_componentStorage[m_componentIDs[i]]->GetPoolInfo();
			const float percentFull = pi.maxCount > 0 ? (float)pi.currentCount / (float)pi.maxCount : 0.f;

			char barBuffer[16];
			sprintf(barBuffer, "%zu/%zu", pi.currentCount, pi.maxCount);
			ImGui::ProgressBar(percentFull, ImVec2(0,0), barBuffer);
			ImGui::PopID();
		}

		if (ImGui::CollapsingHeader("Registered Components"))
		{
			for (std::size_t i = 0; i < m_componentIDs.size(); i++)
			{

				ImGui::Text("%d: %s", m_componentIDs[i], m_componentNames[i].c_str());
			}
		}
	}
}