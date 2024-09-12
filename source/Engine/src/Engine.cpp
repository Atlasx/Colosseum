#include "Engine.h"

#include "Systems/ResourceSystem.h"
#include "Systems/InputSystem.h"

namespace CE
{
	Engine::Engine()
	{
		std::cout << "Good Morning Engine" << std::endl;
	}

	Engine::~Engine()
	{
		std::cout << "Good Night Engine" << std::endl;
	}

	void Engine::Start()
	{
		AddSystems();
		InitSystems();
	}

	void Engine::AddSystems()
	{
		m_systems.push_back(std::make_unique<ResourceSystem>());
		m_systems.push_back(std::make_unique<InputSystem>());
	}

	CEResult Engine::InitSystems()
	{
		// TODO dependency sort
		/*
		CEResult result = CEResult::Success;
		for (auto system : m_systems) {
			result |= system->Startup();
		}

		return result;
		*/
		return CEResult::Success;
	}
}