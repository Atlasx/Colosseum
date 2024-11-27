#pragma once

#include <iostream>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <typeinfo>

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

		//
		// Start
		// 
		// Entry point into the engine, causes systems to be created and initialized
		//
		void Start();

		//
		// GetSystem<T>
		// 
		// Allows for a "singleton-like" design where anyone can query the engine's systems by type given an engine
		//
		template<typename System>
		std::shared_ptr<System> GetSystem() {
			auto iter = m_systems.find(typeid(System));
			if (iter != m_systems.end()) {
				// Dyn cast here to translate from the polymorphic map to the requested system type
				return std::dynamic_pointer_cast<System>(iter->second);
			}
			return nullptr;
		}

	private:
		
		// Add individual systems here regardless of dependency order
		void AddSystems();

		// Systems are initialized and ready to use
		void InitSystems();

#ifdef CDEBUG
		// Some debug only test code during system development
		void TestSystems();
#endif

		void ShutdownSystems();

		std::unordered_map<std::type_index, std::shared_ptr<EngineSystem>> m_systems;

		bool m_exit = false;

		void CoreLoop();
		void Update();
		void Render();
	};
}