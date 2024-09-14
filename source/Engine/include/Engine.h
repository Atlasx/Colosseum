#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "Systems/EngineSystem.h"

namespace CE
{
	enum class CEResult {
		Failed = 0,
		Success = 1
	};

	class Engine
	{
	public:
		Engine();
		~Engine();

		void Start();

	private:
		
		// Add individual systems here regardless of dependency order
		void AddSystems();

		// Systems are initialized and ready to use
		void InitSystems();

#ifdef CDEBUG
		// Some debug only test code during system development
		void TestSystems();
#endif

		std::vector<std::shared_ptr<EngineSystem>> m_systems;

	};
}