#pragma once

#include "stdlibincl.h"

namespace CE
{
	class Engine;

	class EngineSystem
	{
	public:

		virtual std::string Name() const = 0;

		bool m_showDebug = false;

		virtual ~EngineSystem() = 0;

	protected:
		EngineSystem() = delete;
		EngineSystem(Engine* engine) : m_engine(engine) {};

		virtual void Startup() = 0;
		virtual void Shutdown() = 0;

		// Engine can access protected interface on polymorphic pointers
		friend class Engine;
	
		Engine* m_engine;
	};
}