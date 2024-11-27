#include "Systems/RenderSystem.h"

#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Engine.h"

namespace CE
{

	void RenderSystem::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Render everything in game...
		ImGui::ShowDemoWindow();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_window);
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