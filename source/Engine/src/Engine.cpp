#include "Engine.h"

#include "Systems/ResourceSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/EventSystem.h"
#include "Systems/RenderSystem.h"

namespace CE
{
	Engine::Engine() :
		m_exit(false),
		m_window(nullptr)
	{
		std::cout << "Good Morning Engine" << std::endl;
	}

	Engine::~Engine()
	{
		std::cout << "Good Night Engine" << std::endl;

		ShutdownSystems();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		glfwTerminate();
	}

	void Engine::Start()
	{
		if (Initialize() == false)
		{
			return;
		}

		AddSystems();
		InitSystems();

		PostSystemInitialize();

#ifdef CDEBUG
		TestSystems();
#endif

		CoreLoop();
	}

	bool Engine::Initialize()
	{
		if (!glfwInit()) {
			std::cerr << "Failed to initialize GLFW" << std::endl;
			return false;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Request OpenGL version 3.x
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use the core profile

		m_window = glfwCreateWindow(1280, 720, "Render Window", nullptr, nullptr);
		if (m_window == nullptr) {
			std::cerr << "Failed to create a GLFW window" << std::endl;
			glfwTerminate();
			return false;
		}

		glfwMakeContextCurrent(m_window);
		glfwSwapInterval(1);

		if (!gladLoadGL()) {
			std::cerr << "Failed to initialize GLAD" << std::endl;
			glfwTerminate();
			return false;
		}

		return true;
	}

	void Engine::PostSystemInitialize()
	{
		// ImGui init needs to be after InputSetup for callback chaining
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(m_window, true);
		ImGui_ImplOpenGL3_Init("#version 130");
	}

	void Engine::Stop()
	{
		m_exit = true;
	}

	void Engine::AddSystems()
	{
		m_systems[typeid(ResourceSystem)] = std::make_unique<ResourceSystem>(this);
		m_systems[typeid(InputSystem)] = std::make_unique<InputSystem>(this);
		m_systems[typeid(EventSystem)] = std::make_unique<EventSystem>(this);
		m_systems[typeid(RenderSystem)] = std::make_unique<RenderSystem>(this);
	}

	void Engine::InitSystems()
	{
		// Manual Order
		GetSystem<ResourceSystem>()->Startup();

		GetSystem<RenderSystem>()->Startup();

		GetSystem<EventSystem>()->Startup();

		GetSystem<InputSystem>()->Startup();
	}

#ifdef CDEBUG
	void Engine::TestSystems()
	{
		// Run some tests on systems like loading specific files etc
		GetSystem<ResourceSystem>()->RunTests();

		auto IS = GetSystem<InputSystem>();
		if (IS)
		{
			IS->RegisterAction(KeyType::T, []() 
				{
					std::cout << "Hey this action system works!" << std::endl;
				});
			IS->RegisterAction(KeyType::B, []()
				{
					std::cout << "This one also works, not a fluke!" << std::endl;
				});
		}
	}
#endif

	void Engine::CoreLoop()
	{
		while (m_exit == false)
		{
			Update();
			Render();
			ProcessInput();
		}
	}

	void Engine::Update()
	{
		//std::cout << "Updating!" << std::endl;

		// TODO Calculate delta time
	}
	
	void Engine::Render()
	{
		// Just kind of throwing all rendering in here for now
		// needs the window system?
		// put this on a new thread?
		// cache RS pointer

		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		GetSystem<RenderSystem>()->Render();

		m_debugMenu.ShowMenuItems();
		
		// Main Menu
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Debug"))
			{
				for (auto [key, system] : m_systems)
				{
					ImGui::MenuItem(system->Name().c_str(), NULL, &system->m_showDebug);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
		
		// System Debug GUI
		for (auto [key, system] : m_systems)
		{
			system->DrawGUI();
		}


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_window);
	}

	void Engine::ProcessInput()
	{
		// caching IS pointer?
		GetSystem<InputSystem>()->PollInput();
	}

	void Engine::ShutdownSystems()
	{
		for (auto& [key, system] : m_systems)
		{
			system->Shutdown();
		}
	}
}