#pragma once

#include "Systems/EngineSystem.h"

struct GLFWwindow;

namespace CE 
{
	class RenderSystem : public EngineSystem
	{
	public:
		void Render();


	private:
		GLFWwindow* m_window = nullptr;

		/* EngineSystem Interface */
	public:
		std::string Name() const override { return "Render System"; }

		RenderSystem(Engine* engine) :
			EngineSystem(engine)
		{}

	protected:

		void Startup() override;
		void Shutdown() override;

		friend class Engine;
	};

	
}