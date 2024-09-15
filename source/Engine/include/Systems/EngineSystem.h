#pragma once

#include <vector>
#include <string>

namespace CE
{
	class Engine;

	class EngineSystem
	{
	public:
		virtual std::vector<std::string> GetDependencies() const = 0;
		virtual std::string Name() const = 0;

		virtual ~EngineSystem() = 0;

	protected:

		virtual void Startup() = 0;
		virtual void Shutdown() = 0;

		// Engine can access protected interface
		friend class Engine;
	};
}