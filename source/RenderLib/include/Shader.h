#pragma once

// Note this is all from the OpenGL tutorial, ideally a different manager should
// handle loading in resources from and shaders don't worry about that kind of stuff

#include "glad/glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace RL {
	class Shader {
	public:
		unsigned int ID;

		Shader(std::filesystem::path vertexPath, std::filesystem::path fragmentPath);

		void use();
		void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setFloat3(const std::string& name, float x, float y, float z) const;
};
}