#include "Engine.h"

#include "Systems/ResourceSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/EventSystem.h"

#include "DependencyGraph.h"

namespace CE
{
	Engine::Engine() :
		m_shutdown(false)
	{
		std::cout << "Good Morning Engine" << std::endl;
	}

	Engine::~Engine()
	{
		std::cout << "Good Night Engine" << std::endl;

		for (auto [type, system] : m_systems) {
			system->Shutdown();
		}
	}

	void Engine::Start()
	{
		AddSystems();
		InitSystems();

#ifdef CDEBUG
		TestSystems();
#endif

		//CoreLoop();
	}

	void Engine::AddSystems()
	{
		m_systems[typeid(ResourceSystem)] = std::make_unique<ResourceSystem>();
		m_systems[typeid(InputSystem)] = std::make_unique<InputSystem>();
		m_systems[typeid(EventSystem)] = std::make_unique<EventSystem>();
	}

	void Engine::InitSystems()
	{
		DependencyGraph graph;
		for (auto [type, system] : m_systems) {
			graph.AddNode(system->Name(), system->GetDependencies());
		}

		// TODO dependency sort
		for (auto [type, system] : m_systems) {
			system->Startup();
		}
	}

#ifdef CDEBUG
	void Engine::TestSystems()
	{
		// Run some tests on systems like loading specific files etc

	}
#endif

	void Engine::CoreLoop()
	{
		while (m_shutdown == false) {
			Update();
		}
	}

	void Engine::Update()
	{

	}
}