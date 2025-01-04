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
#include <unordered_map>
#include <typeindex>

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

	// All valid events must inherit from BaseEvent
	template<typename EventType>
	concept ValidEventType = std::is_base_of_v<BaseEvent, EventType>;

	struct TestEvent : BaseEvent
	{
		float someData;
		std::string someString;
	};
	
	struct EventA : BaseEvent
	{
		int x, y;
	};

	struct EventB : BaseEvent
	{
		float x;
	};

	struct EventWrapper
	{
		std::vector<std::byte> data;
		std::type_index type;

		template <typename T>
		EventWrapper(const T& event) : data(sizeof(T)), type(typeid(T))
		{
			std::memcpy(data.data(), &event, sizeof(T));
		}

		template <typename T>
		T Get() const
		{
			if (type != typeid(T))
			{
				throw std::bad_cast();
			}
			T event;
			std::memcpy(&event, data.data(), sizeof(T));
			return event;
		}
	};


	class Listener
	{
	public:
		using Callback = std::function<void(const EventWrapper&)>;

		template <typename T, typename F>
		requires ValidEventType<T>
		void Bind(F&& func)
		{
			m_callback = [func = std::forward<F>(func)](const EventWrapper& e)
				{
					func(e.Get<T>());
				};
		}

		template <typename T, typename C>
		requires ValidEventType<T>
		void Bind(C* instance, void (C::* method)(const T&))
		{
			m_callback = [instance, method](const EventWrapper& e)
				{
					(instance->*method)(e.Get<T>());
				};
		}

		void operator()(const EventWrapper& e) const
		{
			if (m_callback)
			{
				m_callback(e);
			}
		}

	private:
		Callback m_callback;
	};

	class ObjectSystem;

	class EventSystem final : public EngineSystem
	{
	private:
		std::vector<EventWrapper> m_eventQueue;
		std::unordered_map<std::type_index, std::vector<Listener>> m_listeners;

	public:
		template <typename T>
		requires ValidEventType<T>
		void FireEvent(const T& event)
		{
			m_eventQueue.emplace_back(event);
		}

		template <typename T, typename F>
		requires ValidEventType<T>
		void RegisterListener(F&& func)
		{
			Listener listener;
			listener.Bind<T>(std::forward<F>(func));
			m_listeners[typeid(T)].emplace_back(std::move(listener));
		}

		template <typename T, typename C>
		requires ValidEventType<T>
		void RegisterMemberListener(C* instance, void (C::* method)(const T&))
		{
			Listener listener;
			listener.Bind<T>(instance, method);
			m_listeners[typeid(T)].emplace_back(std::move(listener));
		}

		void ProcessEvents();

	private:

		// Test member function for event callback
		void OnTestEvent(const TestEvent& e);

		// Store reference to object system for handle lookup on targeted events
		ObjectSystem* p_objSystem = nullptr;

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