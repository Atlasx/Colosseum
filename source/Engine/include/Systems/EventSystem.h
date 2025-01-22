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
	/*
	* 
	* Some thoughts on this event system:
	* 
	* Not a fan of the discrete event type enum, this makes the code for the users of the
	* event system ugly. Now we need to check against event type when firing in the
	* listener. I wanted this setup to be simple for the user, just define a new event
	* type or macro and have the function signature match. Couldn't get it figured out and
	* I want to continue working on other parts of the codebase. Might revisit this later.
	* 
	*/
	enum class EventType
	{
		ET_Default = 0,
		ET_Test,
		ET_EventA
	};

	struct Event
	{
		Event() : m_type(EventType::ET_Test), m_data(nullptr) {}

		EventType m_type;
		void* m_data;
	};

	class Listener
	{
	public:
		using Callback = std::function<void(const Event&)>;

		Listener() : m_callback(nullptr), m_type() {}

		template <typename F>
		void Bind(EventType et, F&& func)
		{
			m_type = et;
			m_callback = [func](const Event& e)
				{
					func(e);
				};
		}

		template <typename C>
		void Bind(EventType et, C* instance, void (C::* method)(const Event&))
		{
			m_type = et;
			m_callback = [instance, method](const Event& e)
				{
					(instance->*method)(e);
				};
		}

		void operator()(const Event& e) const
		{
			if (m_callback && e.m_type == m_type)
			{
				m_callback(e);
			}
		}

	private:
		Callback m_callback;
		EventType m_type;
	};

	class ObjectSystem;

	class EventSystem final : public EngineSystem
	{
	private:
		std::vector<Event> m_eventQueue;
		std::vector<Listener> m_listeners;

	public:
		void FireEvent(const Event& e)
		{
			m_eventQueue.emplace_back(e);
		}

		template <typename F>
		void RegisterListener(EventType et, F&& func)
		{
			Listener listener;
			listener.Bind(et, std::forward<F>(func));
			m_listeners.emplace_back(std::move(listener));
		}

		template <typename C>
		void RegisterMemberListener(EventType et, C* instance, void (C::* method)(const Event&))
		{
			Listener listener;
			listener.Bind(et, instance, method);
			m_listeners.emplace_back(std::move(listener));
		}

		void ProcessEvents();

	private:

		// Test member function for event callback
		void OnTestEvent(const Event& e);

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