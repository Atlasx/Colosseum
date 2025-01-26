#pragma once

#include "Systems/EngineSystem.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "stdlibincl.h"

struct GLFWwindow;

namespace CE
{
	class ITickEventSubscriber
	{
	public:
		virtual void OnTick() = 0;
	};

	template <typename T>
	concept IsSystem = std::is_base_of_v<EngineSystem, T>;

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
		template<IsSystem System>
		std::shared_ptr<System> GetSystem()
		{
			if (m_systems.contains(typeid(System)))
			{
				return std::dynamic_pointer_cast<System>(m_systems[typeid(System)]);
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
		
		// Create necessary glfw window
		bool Initialize();

		// Add individual systems here regardless of dependency order
		void AddSystems();

		// Systems are initialized and ready to use
		void SystemInitialize();

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