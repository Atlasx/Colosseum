#pragma once

#include "stdlibincl.h"

namespace CE
{
	class Engine;

	class EngineSystem
	{
	protected:
		explicit EngineSystem(Engine* engine) : m_engine(engine) {};
		friend class Engine;

		virtual ~EngineSystem() = 0;

	public:
		virtual void Startup() = 0;
		virtual void Shutdown() = 0;

		virtual std::string Name() const = 0;

		Engine* m_engine;
	};
}