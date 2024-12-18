#include "Systems/ObjectSystem.h"

#include <iostream>

namespace CE
{
	void ObjectSystem::Startup()
	{
		std::cout << "Startup Game Object System" << std::endl;
	}

	void ObjectSystem::Shutdown()
	{
		std::cout << "Shutdown Game Object System" << std::endl;
	}


}