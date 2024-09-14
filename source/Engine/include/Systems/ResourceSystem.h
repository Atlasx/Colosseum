#pragma once

#include "Systems/EngineSystem.h"

#include <functional>
#include <filesystem>
#include <queue>
#include <type_traits>

namespace CE
{
	/* Resource Type Wizardry */
	template<typename T>
	struct IsResourceType : std::false_type {};

	template<typename T>
	concept ResourceTypeConcept = IsResourceType<T>::value;

	// Add more template specializations here for each new resource
	class Shader {};
	template<>
	struct IsResourceType<Shader> : std::true_type {};

	class Texture {};
	template<>
	struct IsResourceType<Texture> : std::true_type {};

	class Mesh {};
	template <>
	struct IsResourceType<Mesh> : std::true_type {};


	template<ResourceTypeConcept ResourceType>
	struct ResourceRequest {
		std::filesystem::path m_resourcePath;
		std::function<void(std::shared_ptr<ResourceType>)> m_callback;

		ResourceRequest(const std::filesystem::path rPath, std::function<void(std::shared_ptr<ResourceType>)> callback) :
			m_resourcePath(rPath),
			m_callback(std::move(callback))
		{};
	};

	//
	//	Resource System
	// 
	//	Requirements:
	//		- Request resources to load from disk
	//		- Clean up resources that are not being used
	//		- Handle many types of resources
	//			- Shaders, Game Data, Levels, Textures, Materials, Strings?, Audio
	//		- Track memory usage
	//		- Multithreaded to not block main thread with disk loads
	// 
	//	Example Case 1:
	//		1. Game creates Object
	//		2. Object has resources it requires (material, texture, shader, mesh, components, data)
	//		3. ResourceSystem searches for already loaded resource
	//		4. Load required resources from disk
	//		5. Signal back that resources are all ready
	// 
	//	Example Case 2:
	//		1. Game destroys Object
	//		2. Object lists resources it no longer needs
	//		3. Resource use count decrement
	//		4. Unused resources are freed
	//
	class ResourceSystem : public EngineSystem
	{
	public:
		// Request Resource
		// Begins the process of loading a requested resource, adds the request to the queue
		template<ResourceTypeConcept ResourceType>
		bool RequestResource(const std::filesystem::path& resourcePath, std::function<void(std::shared_ptr<ResourceType>)> callback) {

			if (!IsValidResourcePath(resourcePath)) {
				// TODO Logging
				return false;
			}

			ResourceRequest req(resourcePath, callback);
			m_requests.push([req]() {
				// TODO Logging
				ProcessRequest<ResourceType>(req);
			});

			return true;
		}

	private:

		static bool IsValidResourcePath(const std::filesystem::path& rPath);

		template<ResourceTypeConcept ResourceType>
		std::shared_ptr<ResourceType> LoadResource(const std::filesystem::path& resourcePath);

		template<ResourceTypeConcept ResourceType>
		void ProcessRequest(const ResourceRequest<ResourceType>& request) {
			// TODO Check for already loaded resources here
			std::shared_ptr<ResourceType> loadedResource = std::move(LoadResource<ResourceType>(request.m_resourcePath));
			request.m_callback(loadedResource);
		}

		std::queue<std::function<void()>> m_requests;

		/* EngineSystem Interface */
	public:	
		std::vector<std::string> GetDependencies() const override {
			return {};
		}

		~ResourceSystem() override;
	
	protected:
		void Startup() override;
		void Shutdown() override;
	};

}