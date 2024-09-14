#include "Engine.h"

#include "Systems/ResourceSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/EventSystem.h"

namespace CE
{
	Engine::Engine()
	{
		std::cout << "Good Morning Engine" << std::endl;
	}

	Engine::~Engine()
	{
		std::cout << "Good Night Engine" << std::endl;

		for (auto system : m_systems) {
			system->Shutdown();
		}
	}

	void Engine::Start()
	{
		AddSystems();
		InitSystems();
#ifdef C_DEBUG
		TestSystems();
#endif
	}

	void Engine::AddSystems()
	{
		m_systems.push_back(std::make_unique<ResourceSystem>());
		m_systems.push_back(std::make_unique<InputSystem>());
		m_systems.push_back(std::make_unique<EventSystem>());
	}

	void Engine::InitSystems()
	{
		// TODO dependency sort
		for (auto system : m_systems) {
			system->Startup();
		}
	}

#ifdef C_DEBUG
	void Engine::TestSystems() {
		// Run some tests on systems like loading specific files etc

	}
#endif
}