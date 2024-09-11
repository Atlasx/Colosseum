#pragma once

#include "glad/glad.h"
#include "stb_image.h"
#include <filesystem>

class Texture
{
public:
	Texture(std::filesystem::path);

	void load();

	void useTexture(GLuint slot);

private:
	GLuint textureID;
	GLuint width, height;

};