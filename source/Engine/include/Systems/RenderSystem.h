#pragma once

#include "Systems/EngineSystem.h"

struct GLFWwindow;

namespace CE 
{
	class RenderSystem final : public EngineSystem
	{
	public:
		void Render();


	private:
		GLFWwindow* m_window = nullptr;

		/* EngineSystem Interface */
	public:
		virtual std::string Name() const override { return "Render System"; }
		virtual void DrawGUI() override { return; }

		RenderSystem(Engine* engine) :
			EngineSystem(engine)
		{}

	protected:

		virtual void Startup() override;
		virtual void Shutdown() override;

		friend class Engine;
	};

	
}