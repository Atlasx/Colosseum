#pragma once

#include "Engine.h"

namespace CE
{
	class ICEngineSubsystem
	{
		virtual CEResult Startup() = 0;
		virtual CEResult Shutdown() = 0;

		virtual ~ICEngineSubsystem() = delete;
	};
}