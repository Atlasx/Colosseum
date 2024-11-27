#include <iostream>

#include "Engine.h"
#include "Shader.h"
#include "Systems/EngineSystem.h"
#include "Systems/ResourceSystem.h"

#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace CE;

//void processInput(GLFWwindow* window);

int main() {
	std::cout << "Hello Main!" << std::endl;

	//// GLFW Setup
	//if (!glfwInit()) {
	//	std::cerr << "Failed to initialize GLFW" << std::endl;
	//	return -1;
	//}

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Request OpenGL version 3.x
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use the core profile

	//GLFWwindow* window = glfwCreateWindow(1280, 720, "Colosseum", nullptr, nullptr);
	//if (window == nullptr) {
	//	std::cerr << "Failed to create a GLFW window" << std::endl;
	//	glfwTerminate();
	//	return -1;
	//}
	//glfwMakeContextCurrent(window);

	//// GLAD Setup
	////if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	//if (!gladLoadGL()) {
	//	std::cerr << "Failed to initialize GLAD" << std::endl;
	//	return -1;
	//}

	//glfwSwapInterval(1);
	//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	//// ImGui Setup
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//ImGui::StyleColorsDark();
	//ImGui_ImplGlfw_InitForOpenGL(window, true);
	//ImGui_ImplOpenGL3_Init("#version 130");

	// Engine Setup
	Engine my_engine;
	my_engine.Start();

	/*
	std::filesystem::path assetsPath = ASSETS_PATH;
	std::filesystem::path vertPath = assetsPath / "shaders/shader.vert";
	std::filesystem::path fragPath = assetsPath / "shaders/shader.frag";

	//Test ResourceSystem
	auto RS = my_engine.GetSystem<ResourceSystem>();
	if (RS) {
		RS->RequestResource<CE::Shader>(vertPath, nullptr);
	}

	RL::Shader myShader(vertPath, fragPath);

	// Load first testing image
	std::filesystem::path texturePath = assetsPath / "images/container.jpg";

	int width, height, nrChannels;
	// Note stbi seems to only like Unix style formatting from filesystem
	unsigned char* data = stbi_load((char*)texturePath.generic_string().c_str(), &width, &height, &nrChannels, 0);
	if (!data) {
		std::cerr << "Failed to load texture!" << std::endl;
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	data = nullptr;

	// Load second testing image
	std::filesystem::path altTexturePath = assetsPath / "images/awesomeface.png";

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load((char*)altTexturePath.generic_string().c_str(), &width, &height, &nrChannels, 0);
	if (!data) {
		std::cerr << "Failed to load alt texture image" << std::endl;
	}

	unsigned int altTexture;
	glGenTextures(1, &altTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, altTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	float vertices[] = {
		// Positions			Colors				Textures
		0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f,	// top right
		0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,	// bottom right
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,		0.0f, 0.0f, 0.0f,	0.0f, 1.0f,	// top left
	};

	int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Load VBO with vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Load EBO with elements
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Set VAO vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	// Unbind VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Tunables
	float mixValue = 0.f;

	while (!glfwWindowShouldClose(window)) {

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		processInput(window);

		ImGui::SetNextWindowPos(ImVec2(10, 10));
		ImGui::SetNextWindowSize(ImVec2(250, 150));
		ImGui::Begin("Tunables");

		// OpenGL Drawing
		myShader.use();
		
		ImGui::SliderFloat("Mix", &mixValue, 0.0f, 1.0f);
		myShader.setFloat("mixPercent", mixValue);

		ImGui::End();

		// Bind texture slots
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, altTexture);
	
		glBindVertexArray(VAO);			// Re-bind our VAO

		// Set uniform values
		myShader.setInt("texture1", 0);
		myShader.setInt("texture2", 1);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw from the EBO (referenced from VAO)


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	//ImGui_ImplOpenGL3_Shutdown();
	//ImGui_ImplGlfw_Shutdown();
	//ImGui::DestroyContext();

	//glfwDestroyWindow(window);
	//glfwTerminate();

	*/
}

/*
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}
*/