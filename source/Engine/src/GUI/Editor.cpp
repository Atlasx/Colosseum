#include "GUI/Editor.h"

#include "Engine.h"
#include "Systems/RenderSystem.h"

namespace CE
{

	void DebugSystem::Startup()
	{
		// Subscribe to frame updates
		std::shared_ptr<RenderSystem> RS = m_engine->GetSystem<RenderSystem>();
		if (RS)
		{
			RS->Subscribe(this);
		}

	}

	void DebugSystem::Shutdown()
	{

	}

	void DebugSystem::Subscribe(IDebugGUISubscriber* sub)
	{
		m_debugSubscribers.push_back(sub);
	}

	void DebugSystem::OnDoFrame()
	{
		DrawMainMenu();
		NotifyOnDrawGUI();
	}

	void DebugSystem::DrawMainMenu()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Debug"))
			{
				for (auto sub : m_debugSubscribers)
				{
					auto name = std::string(sub->GetDebugMenuName());
					if (ImGui::MenuItem(name.c_str(), NULL, sub->IsDrawEnabled()))
					{
						sub->SetDrawEnabled(!sub->IsDrawEnabled());
					}
				}
				/*for (auto [key, system] : m_systems)
				{
					ImGui::MenuItem(system->Name().c_str(), NULL, &system->m_showDebug);
				}*/
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void DebugSystem::NotifyOnDrawGUI()
	{
		for (auto sub : m_debugSubscribers)
		{
			if (sub)
			{
				sub->OnDrawGUI();
			}
		}
	}
}