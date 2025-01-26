#include "Systems/RenderSystem.h"

#include "Engine.h"
#include "Systems/LogSystem.h"

namespace CE
{
	void RenderSystem::Render()
	{
		BeginFrame();
		DoFrame();
		EndFrame();
	}

	void RenderSystem::BeginFrame()
	{
		glClear(GL_COLOR_BUFFER_BIT);

		ImGuiBeginFrame();
		NotifyOnBeginFrame();
	}

	void RenderSystem::DoFrame()
	{
		NotifyOnDoFrame();
	}

	void RenderSystem::EndFrame()
	{
		NotifyOnEndFrame();
		ImGuiEndFrame();

		glfwSwapBuffers(m_window);
	}

	void RenderSystem::Startup()
	{
		LOG(RENDER, "Startup");

		// Most of this moved back out to the engine
		m_window = m_engine->GetWindow();
		assert(m_window != nullptr);
	}

	void RenderSystem::Shutdown()
	{
		LOG(RENDER, "Shutdown");
		// Most of this moved back up to engine
	}

	void RenderSystem::Subscribe(IFrameEventSubscriber* sub)
	{
		m_frameSubscribers.push_back(sub);
	}

	void RenderSystem::NotifyOnBeginFrame()
	{
		for (auto sub : m_frameSubscribers)
		{
			if (sub)
			{
				sub->OnBeginFrame();
			}
		}
	}

	void RenderSystem::NotifyOnDoFrame()
	{
		for (auto sub : m_frameSubscribers)
		{
			if (sub)
			{
				sub->OnDoFrame();
			}
		}
	}

	void RenderSystem::NotifyOnEndFrame()
	{
		for (auto sub : m_frameSubscribers)
		{
			if (sub)
			{
				sub->OnEndFrame();
			}
		}
	}

	void RenderSystem::CleanFrameSubscribers()
	{
		m_frameSubscribers.erase(std::remove_if(m_frameSubscribers.begin(), m_frameSubscribers.end(), [](IFrameEventSubscriber* s) {
			return s == nullptr;
			}));
	}

	void RenderSystem::ImGuiBeginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void RenderSystem::ImGuiEndFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}