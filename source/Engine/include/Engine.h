#pragma once

#include <iostream>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <cassert>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Systems/EngineSystem.h"

#include "GUI/Editor.h"

struct GLFWwindow;

namespace CE
{
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

		void Stop();

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
#ifdef CDEBUG
				assert(iter->second);
#endif
				return std::dynamic_pointer_cast<System>(iter->second);
			}
#ifdef CDEBUG
			assert(false);
#endif
			return nullptr;
		}

		// Dangerous handing out free pointers to our window object
		GLFWwindow* GetWindow() { return m_window; }

	private:

		GLFWwindow* m_window;
		DebugMenu m_debugMenu;
		
		// Create necessary glfw window
		bool Initialize();

		// Add individual systems here regardless of dependency order
		void AddSystems();

		// Systems are initialized and ready to use
		void InitSystems();

#ifdef CDEBUG
		// Some debug only test code during system development
		void TestSystems();
#endif

		void PostSystemInitialize();

		void ShutdownSystems();

		std::unordered_map<std::type_index, std::shared_ptr<EngineSystem>> m_systems;

		bool m_exit = false;

		void CoreLoop();
		void Update();
		void Render();
		void ProcessInput();
	};
}