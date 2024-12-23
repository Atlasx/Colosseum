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
* 
* Types of events:
*  - Targeted to a gameobject
*  - Global events
*  - Each has multi-variable options
* 
*/

#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <type_traits>

#include "Systems/EngineSystem.h"
#include "Systems/LogSystem.h"
#include "ObjectPool.h"

namespace CE
{
	// All events must inherit from this to pass checks in event listener
	// We don't want instantiation or deletion from outside derived classes
	struct BaseEvent
	{
	protected:
		BaseEvent() = default;
		~BaseEvent() = default;
	};

	// Use this concept to provide compiler errors if user code tries to use a bad type
	template<typename EventType>
	concept ValidEventType = std::is_base_of_v<EventType, BaseEvent>;

	struct TestEvent : BaseEvent
	{
		float someData;
	};


	// CRTP for unified handling of events either global or targeted
	// Also templated for varying event types
	template <typename Derived, ValidEventType EventType>
	struct BaseListener
	{
		void handleEvent(const EventType& e)
		{
			static_cast<const Derived*>(this)->handleEventImpl(e);
		}
	};

	template <ValidEventType EventType>
	struct GlobalListener : BaseListener<GlobalListener<EventType>, EventType>
	{
		using Callback = std::function<void(const EventType&)>;

		GlobalListener(Callback cb) : m_callback(std::move(cb)) {}

		void handleEventImpl(const EventType& e)
		{
			if (m_callback) m_callback(e);
		}

	private:
		Callback m_callback;
	};

	template <ValidEventType EventType>
	struct ObjectListener : BaseListener<ObjectListener<EventType>, EventType>
	{
		using Callback = std::function<void(ObjectHandle, const EventType&)>;

		ObjectListener(Callback cb, ObjectHandle target) :
			m_callback(std::move(cb)),
			m_target(target)
		{}

		void handleEventImpl(const EventType& e)
		{
			if (m_callback)
			{
				// TODO Get pointer to target
				m_callback(e);
			}
		}

	private:
		Callback m_callback;
		ObjectHandle m_target;
	};
		
	class ObjectSystem;

	class EventSystem final : public EngineSystem
	{
	public:
		
		template<ValidEventType EventType>
		void RegisterListener(std::function<void(const EventType&)> cb)
		{
			using Listener = GlobalListener<EventType>;
			// Create a global listener, listening to an event with a callback func
			Listener listener(cb);
			auto& baseListen = static_cast<BaseListener<Listener, EventType>>(listener);
		}
		 
		template<typename EventType>
		void FireEvent(EventType e)
		{
			// Lookup listeners
			// Fire callback on each listener
		}

	private:
		

		// Store reference to object system for handle lookup on targeted events
		ObjectSystem* p_ObjSystem = nullptr;

		// More friendship to allow Engine to access protected functions on derived class pointers
		friend class Engine;
		
	public:
		/* EngineSystem Interface */

		virtual std::string Name() const override { return "Event System"; }
		virtual void DrawGUI() override { return; }

		EventSystem(Engine* engine) : EngineSystem(engine) 
		{
			RegisterListener<TestEvent>([](const TestEvent& e) {
				LOG(EVENTS, "Test Event Fired!");
			});

			TestEvent testE;
			testE.someData = 5.f;
			FireEvent(testE); // preferred syntax (auto event type)
		};

	protected:

		virtual void Startup() override;
		virtual void Shutdown() override;
	};
}