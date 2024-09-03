#include <iostream>

#include "engine.h"
#include "GLFW/glfw3.h"

int main() {
	std::cout << "Hello Main!" << std::endl;

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Request OpenGL version 3.x
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use the core profile

	GLFWwindow* window = glfwCreateWindow(800, 600, "Colosseum", nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Failed to create a GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	CEngine my_engine;
	my_engine.HelloWorld();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		//glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}