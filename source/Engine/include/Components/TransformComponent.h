#pragma once

#include "Systems/World/ComponentSubsystem.h"

namespace CE
{
	class TransformComponent : public Component
	{
	public:
		TransformComponent() : m_transform(glm::mat4(1.0)) {}

		glm::mat4 m_transform;

		void SetPosition(glm::vec3 pos)
		{
			m_transform[3] = glm::vec4(pos, 1.f);
		}

		glm::vec3 GetPosition()
		{
			return m_transform[3];
		}

		glm::vec3 GetScale()
		{
			glm::vec3 scale;
			scale.x = glm::length(glm::vec3(m_transform[0]));
			scale.y = glm::length(glm::vec3(m_transform[1]));
			scale.z = glm::length(glm::vec3(m_transform[2]));
			return scale;
		}

	};
}