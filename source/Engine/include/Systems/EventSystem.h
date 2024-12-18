#pragma once

#include <vector>
#include <string>

#include "Systems/EngineSystem.h"

namespace CE
{


	/*
	* Event System Notes
	* 
	* Core Features:
	*  - Declare new events (dynamic or static?) (variable expected params?)
	*  - Register listener for global events
	*  - Register listener for targeted events (subset of global with ptr param?)
	*  - Trigger events
	*  - Process all triggered events (stores event data until firing callback?)
	*  - Remove event listeners (handle for tracking? find by object and type?)
	*  - Pass values or arbitrary struct data through events (maximums?)
	*  - Prerequisite tags checked on listener or before fire?
	* 
	* 
	* Ideal Event Invocation:
	* 
	* SomeEventName event;
	* event.floatValue = 5.f;
	* event.otherData = someObjectHandle;
	* event.moreData = "You died!";
	* PostEvent(event); // Base Object Class inherited method?
	* 
	*/
	class EventSystem final : public EngineSystem
	{
	public:


		// More friendship to allow Engine to access protected functions on derived class pointers
		friend class Engine;
		
	public:
		/* EngineSystem Interface */

		virtual std::string Name() const override { return "Event System"; }
		virtual void DrawGUI() override { return; }

		EventSystem(Engine* engine) : EngineSystem(engine) {};

	protected:

		virtual void Startup() override;
		virtual void Shutdown() override;
	};
}