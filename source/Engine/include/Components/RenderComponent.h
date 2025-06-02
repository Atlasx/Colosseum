#pragma once

#include "Systems/World/ComponentSubsystem.h"

namespace CE
{
	class RenderComponent : public Component
	{
	public:
		RenderComponent() : woof(0) {}

		// Rendering Data here
		int woof;
	};
}