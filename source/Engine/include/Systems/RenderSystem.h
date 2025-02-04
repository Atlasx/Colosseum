#pragma once

#include "Systems/EngineSystem.h"
#include "stdlibincl.h"
#include "glad/glad.h" // TODO Remove this dependency

struct GLFWwindow;

namespace CE 
{
	class IFrameEventSubscriber
	{
	public:
		virtual void OnBeginFrame() {};
		virtual void OnDoFrame() {};
		virtual void OnEndFrame() {};
	};

	class RenderSystem final : public EngineSystem
	{
	public:
		void Render();

	private:
		GLFWwindow* m_window = nullptr;

		void BeginFrame();
		void DoFrame();
		void EndFrame();

		// Frame Events
	public:
		void Subscribe(IFrameEventSubscriber* sub);
	private:
		void NotifyOnBeginFrame();
		void NotifyOnDoFrame();
		void NotifyOnEndFrame();
		void CleanFrameSubscribers();

		std::vector<IFrameEventSubscriber*> m_frameSubscribers;

		// Testing OpenGL
	private:
		GLuint VertexArrayID;
		GLuint programID;
		GLuint MatrixID;
		GLuint vertexbuffer;

		GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

		// ImGui
	private:
		void ImGuiBeginFrame();
		void ImGuiEndFrame();

		/* EngineSystem Interface */
	public:
		virtual std::string Name() const override { return "Render System"; }
		//virtual void DrawGUI() override { return; }

		RenderSystem(Engine* engine) :
			EngineSystem(engine)
		{}

	protected:

		virtual void Startup() override;
		virtual void Shutdown() override;

		friend class Engine;
	};

	
}