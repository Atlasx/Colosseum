#pragma once

#include <vector>
#include <string>

namespace CE
{
	class CEngineSystem
	{
	public:
		virtual void Startup() = 0;
		virtual void Shutdown() = 0;

		virtual std::vector<std::string> GetDependencies() const = 0;

		virtual ~CEngineSystem() = 0;
	};
}