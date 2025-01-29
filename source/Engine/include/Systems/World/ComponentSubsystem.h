#pragma once

#include <glm/glm.hpp>

namespace CE
{
	// Base class for components CRTP access for common funcs
	template <typename Derived>
	class Component
	{
	public:
		void IUpdate(float dt)
		{
			static_cast<Derived*>(this)->Update(dt);
		}
	};

	class TransformComponent : public Component<TransformComponent>
	{
	public:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

	};
	// Component
	class ComponentSubsystem
	{

	};
}

