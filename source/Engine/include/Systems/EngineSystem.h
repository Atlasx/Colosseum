#pragma once

#include <vector>
#include <string>

namespace CE
{
	class Engine;

	class EngineSystem
	{
	public:

		virtual std::string Name() const = 0;

		virtual ~EngineSystem() = 0;

	protected:
		EngineSystem() = delete;
		EngineSystem(Engine* engine) : m_engine(engine) {};

		virtual void Startup() = 0;
		virtual void Shutdown() = 0;

		// Engine can access protected interface on polymorphic pointers
		friend class Engine;
	
	private:
		Engine* m_engine;
	};
}