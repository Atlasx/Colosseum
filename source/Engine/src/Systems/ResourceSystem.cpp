
#include "Systems/ResourceSystem.h"

namespace CE
{
	void ResourceSystem::Startup() {

	}

	void ResourceSystem::Shutdown() {

	}

	ResourceSystem::~ResourceSystem() {

	}

	bool ResourceSystem::IsValidResourcePath(const std::filesystem::path& rPath) {
		// TODO perform validation for resource paths
		return true;
	}

	//////////////////////////////////////////////
	//											//
	//  Load Resource Template Specializations	//
	//											//
	//////////////////////////////////////////////

	// Shaders
	template<>
	std::shared_ptr<Shader> ResourceSystem::LoadResource<Shader>(const std::filesystem::path& resourcePath) {
		return std::make_shared<Shader>();
	}

	// Textures
	template<>
	std::shared_ptr<Texture> ResourceSystem::LoadResource<Texture>(const std::filesystem::path& resourcePath) {
		return std::make_shared<Texture>();
	}

	// Meshes
	template<>
	std::shared_ptr<Mesh> ResourceSystem::LoadResource<Mesh>(const std::filesystem::path& resourcePath) {
		return std::make_shared<Mesh>();
	}
}