#pragma once

#include "Systems/EngineSystem.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace CE 
{
	struct WindowData
	{
		GLFWwindow* window;
		ImGuiContext* imguiContext;
	};

	class RenderSystem : public EngineSystem
	{
		struct WindowCreationParams
		{
			int width;
			int height;

			char title[64];
		};

		void CreateWindow(const WindowCreationParams& params);
		void DestroyWindow(const WindowData& data);

		void Render();

	private:
		bool m_initialized = false;

		std::vector<WindowData> m_windows;

		/* EngineSystem Interface */
	public:
		std::string Name() const override { return "Render System"; }

		RenderSystem(Engine* engine) : EngineSystem(engine) {}

	protected:

		void Startup() override;
		void Shutdown() override;

		friend class Engine;
	};

	
}