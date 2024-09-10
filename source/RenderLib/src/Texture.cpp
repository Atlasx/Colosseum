#include "Texture.h"

Texture::Texture(std::filesystem::path texturePath)
{
	int width, height, nrChannels;
	// Note stbi seems to only like Unix style formatting from filesystem
	unsigned char* data = stbi_load((char*)texturePath.generic_string().c_str(), &width, &height, &nrChannels, 0);
	if (!data) {
		std::cerr << "Failed to load texture!" << std::endl;
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	data = nullptr;
}

void Texture::useTexture(GLuint slot)
{
	glActiveTexture(slot);


}