#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace rl
{
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
	};

	class Mesh
	{
	public:
		Mesh();
		~Mesh();

		void SetBuffers(const std::vector<rl::Vertex>& vertices, const std::vector<size_t>& indices);
		void Bind() const;
		void Unbind() const;
		void Draw() const;

		GLuint GetVAO() const { return VAO; }
		size_t GetIndexCount() const { return indexCount; }

	private:
		GLuint VAO = 0;
		GLuint VBO = 0;
		GLuint EBO = 0;
		size_t indexCount = 0;
	};
}