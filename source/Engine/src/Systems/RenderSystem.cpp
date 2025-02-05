#include "Systems/RenderSystem.h"

#include "Engine.h"
#include "stdlibincl.h"
#include "Systems/LogSystem.h"
#include "Systems/WorldSystem.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace CE
{
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
	};

	void RenderSystem::Render()
	{
		BeginFrame();
		DoFrame();
		EndFrame();
	}

	void RenderSystem::BeginFrame()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		ImGuiBeginFrame();
		NotifyOnBeginFrame();
	}

	void RenderSystem::DoFrame()
	{
		glUseProgram(programID);
	
		glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
		// Camera matrix
		glm::mat4 View = glm::lookAt(
			glm::vec3(4, 3, -3),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * View * Model;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		

		NotifyOnDoFrame();

		std::shared_ptr<WorldSystem> WS = m_engine->GetSystem<WorldSystem>();
		if (WS)
		{
			WS->ForEachComponent<TransformComponent>([&](TransformComponent* component) {
				
				// This is hitting every Transform Component
				component->GetPosition();

				glm::mat4 Model = component->m_transform;//glm::mat4(1.0f);
				glm::mat4 MVP = Projection * View * Model;
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
				glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
				});
		}
	}

	void RenderSystem::EndFrame()
	{
		glDisableVertexAttribArray(0);

		NotifyOnEndFrame();
		ImGuiEndFrame();

		glfwSwapBuffers(m_window);
	}

	void RenderSystem::Startup()
	{
		LOG(RENDER, "Startup");

		// Most of this moved back out to the engine
		m_window = m_engine->GetWindow();
		assert(m_window != nullptr);

		// Ripping this from OpenGL tutorial
		VertexArrayID;
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		programID = LoadShaders("C:/Users/cjdou/source/repos/colosseum/assets/shaders/shader.vert", "C:/Users/cjdou/source/repos/colosseum/assets/shaders/shader.frag");

		MatrixID = glGetUniformLocation(programID, "MVP");

		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	}

	void RenderSystem::Shutdown()
	{
		LOG(RENDER, "Shutdown");
		
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteProgram(programID);
		glDeleteVertexArrays(1, &VertexArrayID);
	}

	void RenderSystem::Subscribe(IFrameEventSubscriber* sub)
	{
		m_frameSubscribers.push_back(sub);
	}

	void RenderSystem::NotifyOnBeginFrame()
	{
		for (auto sub : m_frameSubscribers)
		{
			if (sub)
			{
				sub->OnBeginFrame();
			}
		}
	}

	void RenderSystem::NotifyOnDoFrame()
	{
		for (auto sub : m_frameSubscribers)
		{
			if (sub)
			{
				sub->OnDoFrame();
			}
		}
	}

	void RenderSystem::NotifyOnEndFrame()
	{
		for (auto sub : m_frameSubscribers)
		{
			if (sub)
			{
				sub->OnEndFrame();
			}
		}
	}

	void RenderSystem::CleanFrameSubscribers()
	{
		m_frameSubscribers.erase(std::remove_if(m_frameSubscribers.begin(), m_frameSubscribers.end(), [](IFrameEventSubscriber* s) {
			return s == nullptr;
			}));
	}

	GLuint RenderSystem::LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
	{
		// Create the shaders
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		// Read the Vertex Shader code from the file
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
		if (VertexShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << VertexShaderStream.rdbuf();
			VertexShaderCode = sstr.str();
			VertexShaderStream.close();
		}
		else {
			printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
			getchar();
			return 0;
		}

		// Read the Fragment Shader code from the file
		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
		if (FragmentShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << FragmentShaderStream.rdbuf();
			FragmentShaderCode = sstr.str();
			FragmentShaderStream.close();
		}

		GLint Result = GL_FALSE;
		int InfoLogLength;


		// Compile Vertex Shader
		printf("Compiling shader : %s\n", vertex_file_path);
		char const* VertexSourcePointer = VertexShaderCode.c_str();
		glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(VertexShaderID);

		// Check Vertex Shader
		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			printf("%s\n", &VertexShaderErrorMessage[0]);
		}



		// Compile Fragment Shader
		printf("Compiling shader : %s\n", fragment_file_path);
		char const* FragmentSourcePointer = FragmentShaderCode.c_str();
		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(FragmentShaderID);

		// Check Fragment Shader
		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			printf("%s\n", &FragmentShaderErrorMessage[0]);
		}



		// Link the program
		printf("Linking program\n");
		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("%s\n", &ProgramErrorMessage[0]);
		}


		glDetachShader(ProgramID, VertexShaderID);
		glDetachShader(ProgramID, FragmentShaderID);

		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);

		return ProgramID;
	}

	void RenderSystem::ImGuiBeginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void RenderSystem::ImGuiEndFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}