#include "Systems/RenderSystem.h"

#include <iostream>

namespace CE
{
	void RenderSystem::CreateWindow(const WindowCreationParams& params)
	{
		if (!m_initialized) {
			//CE_ERROR("Failed to create window, creation before initialization.");
			return;
		}

		WindowData newData;

		GLFWwindow* newWindow = glfwCreateWindow(params.width, params.height, params.title, nullptr, nullptr);
		if (newWindow == nullptr) {
			std::cerr << "Failed to create a GLFW window" << std::endl;
			glfwTerminate();
			return;
		}
		newData.window = newWindow;

		glfwMakeContextCurrent(newWindow);
		glfwSwapInterval(1);

		IMGUI_CHECKVERSION();
		ImGuiContext* ctx = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(newWindow, true);
		ImGui_ImplOpenGL3_Init("#version 130");

		newData.imguiContext = ctx;
		m_windows.push_back(newData);
	}

	void RenderSystem::DestroyWindow(const WindowData& data)
	{
		if (data.window == nullptr) {
			return;
		}

		ImGui::DestroyContext(data.imguiContext);

		glfwDestroyWindow(data.window);
	}

	void RenderSystem::Render()
	{
		for (auto windowData : m_windows)
		{	
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui::SetCurrentContext(windowData.imguiContext);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// Render everything in game...
			ImGui::ShowDemoWindow();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(windowData.window);
		}
	}

	void RenderSystem::Startup()
	{
		if (!glfwInit()) {
			std::cerr << "Failed to initialize GLFW" << std::endl;
			return;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Request OpenGL version 3.x
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use the core profile

		if (!gladLoadGL()) {
			std::cerr << "Failed to initialize GLAD" << std::endl;
			return;
		}

		m_initialized = true;
	}

	void RenderSystem::Shutdown()
	{
		for (int i = 0; i < m_windows.size(); i++) {
			DestroyWindow(m_windows[i]);
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		glfwTerminate();
	}
}