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
	concept ValidEventType = std::is_base_of_v<BaseEvent, EventType>;

	template<typename T>
	concept ValidClass = std::is_class_v<T>;

	struct TestEvent : BaseEvent
	{
		float someData;
		char name[10] = "TestEvent";
	};


	template <typename Derived, ValidEventType EventType>
	struct BaseListener
	{
		void HandleEvent(const EventType& e)
		{
			static_cast<const Derived*>(this)->HandleEventImpl(e);
		}
	};

	template <ValidEventType EventType>
	struct GlobalListener : BaseListener<GlobalListener<EventType>, EventType>
	{
		using Callback = std::function<void(const EventType&)>;

		GlobalListener(Callback cb) : m_callback(std::move(cb)) {}

		void HandleEventImpl(const EventType& e)
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

		ObjectListener(ObjectHandle target, Callback cb) :
			m_callback(std::move(cb)),
			m_target(target)
		{}

		void HandleEventImpl(const EventType& e)
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

	template <typename T, ValidEventType EventType>
	struct MemberListener : BaseListener<MemberListener<T, EventType>, EventType>
	{
		using MemberCallback = void (T::*)(const EventType&);

		MemberListener(T* instance, MemberCallback cb) :
			m_instance(instance),
			m_callback(cb)
		{}

		void HandleEventImpl(const EventType& e)
		{
			if (m_instance != nullptr)
			{
				m_instance->*m_callback(e);
			}
		}

	private:
		T* m_instance;
		MemberCallback m_callback;
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
			// TODO store the listener
		}

		template<ValidClass T, ValidEventType EventType>
		void RegisterListener(T* instance, void (T::* memberFunc)(const EventType&))
		{
			using Listener = MemberListener<T, EventType>;

			Listener listener(instance, memberFunc);
		}
		 
		template<ValidEventType EventType>
		void FireEvent(EventType e)
		{
			// Lookup listeners
			// Fire callback on each listener
			LOG(EVENTS, "Firing Event!");
		}

	private:

		// Test member function for event callback
		void OnTestEvent(const TestEvent& e);
		

		// Store reference to object system for handle lookup on targeted events
		ObjectSystem* p_ObjSystem = nullptr;

		// More friendship to allow Engine to access protected functions on derived class pointers
		friend class Engine;
		
	public:
		/* EngineSystem Interface */

		virtual std::string Name() const override { return "Event System"; }
		virtual void DrawGUI() override { return; }

		EventSystem(Engine* engine) : EngineSystem(engine) {}

	protected:

		virtual void Startup() override;
		virtual void Shutdown() override;
	};
}