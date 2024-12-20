#include "Engine.h"

#include "Systems/ResourceSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/EventSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/ObjectSystem.h"
#include "Systems/LogSystem.h"

#include <random>

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

		m_window = glfwCreateWindow(1920, 1080, "Render Window", nullptr, nullptr);
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
		m_systems[typeid(ObjectSystem)] = std::make_unique<ObjectSystem>(this);
		m_systems[typeid(LogSystem)] = std::make_unique<LogSystem>(this);
	}

	void Engine::InitSystems()
	{
		// Manual Order
		GetSystem<LogSystem>()->Startup();
		
		GetSystem<ResourceSystem>()->Startup();

		GetSystem<RenderSystem>()->Startup();

		GetSystem<EventSystem>()->Startup();

		GetSystem<InputSystem>()->Startup();

		GetSystem<ObjectSystem>()->Startup();
	}

#ifdef CDEBUG
	void Engine::TestSystems()
	{
		// Some logging tests
		LOG(ENGINE, "asdf", "asfd");
		LOG_INFO(ENGINE, "Another test");
		LOG_INFO(ENGINE, "Formatting {} {}", "test", 15);
		LOG_WARN(ENGINE, "Warning! {}", "Exceeded something!");
		LOG_ERROR(ENGINE, "This is a big bad error!", "asdf");

		GetSystem<ResourceSystem>()->RunTests();

		auto IS = GetSystem<InputSystem>();
		if (IS)
		{
			IS->RegisterAction("TestAction One", KeyType::T, []()
				{
					std::cout << "Hey this action system works!" << std::endl;
				}, KeyState::PRESSED);
			IS->RegisterAction("TestAction Two", KeyType::B, []()
				{
					std::cout << "This one also works, not a fluke!" << std::endl;
				}, KeyState::RELEASED, KeyState::PRESSED);
			IS->RegisterAction("Add Action", KeyType::N, [IS]()
				{
					std::cout << "Creating new keybinding!" << std::endl;

					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_int_distribution<int> dist(static_cast<int>(KeyType::A), static_cast<int>(KeyType::Z));
					KeyType randBinding = static_cast<KeyType>(dist(gen));

					std::cout << "This is a random binding: " << InputUtilities::GetKeyName(randBinding) << std::endl;
					auto randBindingLambda = []
						{
							std::cout << "Random binding pressed!" << std::endl;
						};

					IS->RegisterAction("Random Action", randBinding, randBindingLambda);
				}, KeyState::PRESSED, KeyState::RELEASED);
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
		// Manual order once again
		GetSystem<ObjectSystem>()->Shutdown();
		
		GetSystem<EventSystem>()->Shutdown();
		GetSystem<InputSystem>()->Shutdown();
		GetSystem<RenderSystem>()->Shutdown();
		GetSystem<ResourceSystem>()->Shutdown();

		GetSystem<LogSystem>()->Shutdown();
	}
}