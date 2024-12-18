#include "Systems/LogSystem.h"

#include <iostream>

namespace CE
{
	void LogSystem::Startup()
	{
		std::cout << "LogSystem Startup" << std::endl;
	}

	void LogSystem::Shutdown()
	{
		std::cout << "LogSystem Shutdown" << std::endl;
	}
}