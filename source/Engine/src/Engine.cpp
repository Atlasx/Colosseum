#include "Engine.h"

#include "Systems/ResourceSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/EventSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/WorldSystem.h"
#include "Systems/LogSystem.h"

#include "GUI/Editor.h"
#include "GUI/FrameCounter.h"


namespace CE
{
	Engine::Engine() :
		m_exit(false),
		m_window(nullptr),
		m_frameCounter(nullptr)
	{
		std::cout << "Good Morning Engine" << std::endl;
	}

	Engine::~Engine()
	{
		std::cout << "Good Night Engine" << std::endl;

		delete m_frameCounter;

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
		SystemInitialize();

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
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		style.IndentSpacing = 10.f;
		ImGui_ImplGlfw_InitForOpenGL(m_window, true);
		ImGui_ImplOpenGL3_Init("#version 130");

		m_frameCounter = new FrameCounter();
		std::shared_ptr<DebugSystem> DS = GetSystem<DebugSystem>();
		if (DS)
		{
			//DS->Subscribe(m_frameCounter);
		}
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
		m_systems[typeid(WorldSystem)] = std::make_unique<WorldSystem>(this);
		m_systems[typeid(LogSystem)] = std::make_unique<LogSystem>(this);

		m_systems[typeid(DebugSystem)] = std::make_unique<DebugSystem>(this);
	}

	void Engine::SystemInitialize()
	{
		// Manual Order
		GetSystem<LogSystem>()->Startup();
		
		GetSystem<ResourceSystem>()->Startup();

		GetSystem<RenderSystem>()->Startup();

		GetSystem<EventSystem>()->Startup();

		GetSystem<InputSystem>()->Startup();

		GetSystem<WorldSystem>()->Startup();

		GetSystem<DebugSystem>()->Startup();
	}

#ifdef CDEBUG
	void Engine::TestSystems()
	{
		// Some logging tests
		LOG(ENGINE, "Running Tests!");
		LOG(ENGINE, "asdf", "asfd");
		LOG_INFO(ENGINE, "Another test");
		LOG_INFO(ENGINE, "Formatting {} {}", "test", 15);
		LOG_WARN(ENGINE, "Warning! {}", "Exceeded something!");
		LOG_ERROR(ENGINE, "This is a big bad error!", "asdf");

		GetSystem<ResourceSystem>()->RunTests();

		auto IS = GetSystem<InputSystem>();
		if (IS)
		{
			
			IS->RegisterAction<PressedAction>("TestAction One", KeyType::T, []()
				{
					std::cout << "Hey this action system works!" << std::endl;
				});
			IS->RegisterAction<PressedAction>("TestAction Two", KeyType::B, []()
				{
					std::cout << "This one also works, not a fluke!" << std::endl;
				});
			IS->RegisterAction<PressedAction>("Add Action", KeyType::N, [IS]()
				{
					LOG_INFO(INPUT, "Creating new keybinding!");

					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_int_distribution<int> dist(static_cast<int>(KeyType::A), static_cast<int>(KeyType::Z));
					KeyType randBinding = static_cast<KeyType>(dist(gen));

					std::cout << "This is a random binding: " << InputUtilities::GetKeyName(randBinding) << std::endl;
					auto randBindingLambda = []
						{
							LOG_INFO(INPUT, "Random binding pressed!");
						};

					IS->RegisterAction<PressedAction>("Random Action", randBinding, randBindingLambda);
				});
		}

		GetSystem<EventSystem>()->TestEventSystem();

		//auto RS = GetSystem<ResourceSystem>();
		//g_testCube = RS->LoadResource<rl::Mesh>("C:/Users/cjdou/source/repos/colosseum/assets/meshes/cube.obj");
	}
#endif

	void Engine::CoreLoop()
	{
		while (m_exit == false)
		{
			m_frameCounter->FrameStart();

			Update();
			Render();
			ProcessInput();

			

			double excess = m_frameCounter->FrameEnd();
			if (excess > 0)
			{
				// This caused my frames to skyrocket? TODO investigate
				//std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(excess)));
			}
		}
	}

	void Engine::Update()
	{
		//std::cout << "Updating!" << std::endl;

		// TODO Calculate delta time

		GetSystem<InputSystem>()->UpdateActions();
		GetSystem<EventSystem>()->ProcessEvents();
	}
	
	void Engine::Render()
	{
		GetSystem<RenderSystem>()->Render();

		/*
		// BEGIN FRAME
		NotifyOnBeginFrame();
		ImGuiBeginFrame();


		// DO FRAME
		NotifyOnDoFrame();
		DrawGUI();


		// END FRAME
		NotifyOnEndFrame();
		ImGuiEndFrame();
		

		// NOTHING ELSE PAST HERE
		glfwSwapBuffers(m_window);
		*/
	}

	void Engine::ProcessInput()
	{
		// caching IS pointer?
		GetSystem<InputSystem>()->PollInput();
		GetSystem<InputSystem>()->ProcessActions();
	}

	void Engine::ShutdownSystems()
	{
		// Manual order once again
		GetSystem<DebugSystem>()->Shutdown();

		GetSystem<WorldSystem>()->Shutdown();
		
		GetSystem<EventSystem>()->Shutdown();
		GetSystem<InputSystem>()->Shutdown();
		GetSystem<RenderSystem>()->Shutdown();
		GetSystem<ResourceSystem>()->Shutdown();

		GetSystem<LogSystem>()->Shutdown();
	}
}