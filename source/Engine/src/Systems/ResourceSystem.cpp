#include "Systems/ResourceSystem.h"

#include "Systems/LogSystem.h"

namespace CE
{
	void ResourceSystem::Startup()
	{
		LOG(RESOURCES, "Startup");
	}

	void ResourceSystem::Shutdown()
	{
		LOG(RESOURCES, "Shutdown");
	}

	bool ResourceSystem::IsValidResourcePath(const std::filesystem::path& rPath)
	{
		return std::filesystem::exists(rPath);
	}

	//////////////////////////////////////////////
	//											//
	//  Load Resource Template Specializations	//
	//											//
	//////////////////////////////////////////////

	// Shaders
	template<>
	std::shared_ptr<Shader> ResourceSystem::LoadResource<Shader>(const std::filesystem::path& resourcePath)
	{
		return std::make_shared<Shader>();
	}

	// Textures
	template<>
	std::shared_ptr<Texture> ResourceSystem::LoadResource<Texture>(const std::filesystem::path& resourcePath)
	{
		return std::make_shared<Texture>();
	}

	enum class OBJ_PREFIX
	{
		V,
		VT,
		VN,
		F,
		INVALID
	};

	OBJ_PREFIX GetPrefix(std::string prefix)
	{
		if (prefix == "v") { return OBJ_PREFIX::V; }
		if (prefix == "vt") { return OBJ_PREFIX::VT; }
		if (prefix == "vn") { return OBJ_PREFIX::VN; }
		if (prefix == "f") { return OBJ_PREFIX::F; }
		return OBJ_PREFIX::INVALID;
	}

	struct float3
	{
		float x, y, z;
	};

	struct float2
	{
		float u, v;
	};

	// Meshes
	template<>
	std::shared_ptr<rl::Mesh> ResourceSystem::LoadResource<rl::Mesh>(const std::filesystem::path& resourcePath)
	{
		auto mesh = std::make_shared<rl::Mesh>();

		std::ifstream file(resourcePath);
		if (!file)
		{
			LOG_ERROR(RESOURCES, "Failed to open file.");
			return nullptr;
		}

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texcoords;
		std::vector<glm::vec3> normals;

		struct VertexKey
		{
			int posIndex;
			int texIndex;
			int normIndex;

			bool operator==(const VertexKey& other) const
			{
				return posIndex == other.posIndex &&
					texIndex == other.texIndex &&
					normIndex == other.normIndex;
			}
		};

		struct VertexHasher
		{
			size_t operator()(const VertexKey& k) const
			{
				size_t h1 = std::hash<int>{}(k.posIndex);
				size_t h2 = std::hash<int>{}(k.texIndex);
				size_t h3 = std::hash<int>{}(k.normIndex);
				return h1 ^ (h2 << 1) ^ (h3 << 2);
			}
		};

		std::unordered_map<VertexKey, size_t, VertexHasher> vertexMap;
		std::vector<rl::Vertex> finalVertices;
		std::vector<size_t> indices;

		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream ss(line);
			std::string prefix;
			ss >> prefix;

			if (prefix == "v") {
				glm::vec3 pos;
				ss >> pos.x >> pos.y >> pos.z;
				positions.push_back(pos);
			}
			else if (prefix == "vt") {
				glm::vec2 uv;
				ss >> uv.x >> uv.y;
				texcoords.push_back(uv);
			}
			else if (prefix == "vn") {
				glm::vec3 norm;
				ss >> norm.x >> norm.y >> norm.z;
				normals.push_back(norm);
			}
			else if (prefix == "f") {
				std::vector<VertexKey> faceVertices;
				std::string token;
				while (ss >> token) {
					VertexKey key = { -1, -1, -1 };
					size_t pos1 = token.find('/');
					size_t pos2 = token.find('/', pos1 + 1);

					if (pos1 == std::string::npos) {
						key.posIndex = std::stoi(token) - 1;
					}
					else {
						key.posIndex = std::stoi(token.substr(0, pos1)) - 1;

						if (pos2 > pos1 + 1)
							key.texIndex = std::stoi(token.substr(pos1 + 1, pos2 - pos1 - 1)) - 1;

						if (pos2 != std::string::npos && pos2 + 1 < token.size())
							key.normIndex = std::stoi(token.substr(pos2 + 1)) - 1;
					}
					faceVertices.push_back(key);

					auto it = vertexMap.find(key);
					if (it == vertexMap.end()) {
						rl::Vertex v{};
						v.position = positions[key.posIndex];
						v.texcoord = (key.texIndex >= 0 && key.texIndex < texcoords.size()) ? texcoords[key.texIndex] : glm::vec2(0.0f);
						v.normal = (key.normIndex >= 0 && key.normIndex < normals.size()) ? normals[key.normIndex] : glm::vec3(0.0f);

						size_t index = static_cast<size_t>(finalVertices.size());
						vertexMap[key] = index;
						finalVertices.push_back(v);
						indices.push_back(index);
					}
					else {
						indices.push_back(it->second);
					}
				}
			}
		}

		mesh->SetBuffers(finalVertices, indices);
		return mesh;
	}

#ifdef CDEBUG
	void ResourceSystem::RunTests()
	{
		
	}
#endif
}