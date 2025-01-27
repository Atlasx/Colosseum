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
		DebugView dv;
		dv.subscriber = sub;
		dv.showDebug = true;

		m_debugSubscribers.push_back(dv);
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
				for (auto& dv : m_debugSubscribers)
				{
					auto sub = dv.subscriber;
					auto name = std::string(sub->GetDebugMenuName());
					if (ImGui::MenuItem(name.c_str(), NULL, dv.showDebug))
					{
						dv.showDebug = !dv.showDebug;
					}
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void DebugSystem::NotifyOnDrawGUI()
	{
		for (auto& dv : m_debugSubscribers)
		{
			if (dv.subscriber && dv.showDebug)
			{
				dv.subscriber->OnDrawGUI();
			}
		}
	}
}