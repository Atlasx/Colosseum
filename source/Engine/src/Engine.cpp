#include "Engine.h"

#include "Systems/ResourceSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/EventSystem.h"
#include "Systems/RenderSystem.h"

namespace CE
{
	Engine::Engine() :
		m_exit(false)
	{
		std::cout << "Good Morning Engine" << std::endl;
	}

	Engine::~Engine()
	{
		std::cout << "Good Night Engine" << std::endl;

		ShutdownSystems();
	}

	void Engine::Start()
	{
		AddSystems();
		InitSystems();

#ifdef CDEBUG
		TestSystems();
#endif

		auto renderSystem = GetSystem<RenderSystem>();
		if (renderSystem) {
			renderSystem->CreateWindow({ 1280, 720, "Test" });
		}

		CoreLoop();
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
	}
#endif

	void Engine::CoreLoop()
	{
		int count = 0;
		while (m_exit == false)
		{
			Update();
			Render();

			// For now just a few frames and then shutdown
			count++;
			m_exit = count == 10;
		}
	}

	void Engine::Update()
	{
		std::cout << "Updating!" << std::endl;

		// TODO Calculate delta time
	}
	
	void Engine::Render()
	{
		// Just kind of throwing all rendering in here for now
		// needs the window system?
		// put this on a new thread?
		// cache rs pointer
		auto renderSystem = GetSystem<RenderSystem>();
		if (renderSystem) {
			renderSystem->Render();
		}
	}

	void Engine::ShutdownSystems()
	{
		for (auto& [key, system] : m_systems)
		{
			system->Shutdown();
		}
	}
}