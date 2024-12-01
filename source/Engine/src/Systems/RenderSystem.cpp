#include "Systems/RenderSystem.h"

#include <iostream>

#include "Engine.h"

namespace CE
{

	void RenderSystem::Render()
	{
		// Render everything in game...
		//ImGui::ShowDemoWindow();
	}

	void RenderSystem::Startup()
	{
		// Most of this moved back out to the engine
		m_window = m_engine->GetWindow();
		assert(m_window != nullptr);
	}

	void RenderSystem::Shutdown()
	{
		// Most of this moved back up to engine
	}
}