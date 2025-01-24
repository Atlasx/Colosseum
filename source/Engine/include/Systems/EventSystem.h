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
#include <any>
#include <string>
#include <queue>
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
	
	// Used for compile-time assurance of events
	struct Event { };

	template <typename T>
	concept IsEvent = std::is_base_of_v<Event, T>;

	struct TestEvent : public Event
	{
		int someData;
		float moreData;
		std::string someString;

		TestEvent() : someData(10), moreData(2.f), someString("Test") {}
	};

	template <IsEvent EType>
	class Listener
	{
	public:
		Listener(std::function<void(const EType&)> callable) : m_callable(callable) {}

		void Fire(const EType& e)
		{
			if (m_callable)
			{
				m_callable(e);
			}
		}

		std::function<void(const EType& )> m_callable;
	};

	class IEventQueue
	{
	public:
		virtual void Process() = 0;
	};

	template <IsEvent EType>
	class EventQueue : public IEventQueue
	{
	public:

		void PostEvent(EType event)
		{
			// Copy event into queue
			m_queue.push(std::move(event));
		}

		void RegisterGlobalListener(std::function<void(const EType&)> callback)
		{
			// Create listener, return handle?
			m_listeners.emplace_back(Listener<EType>(callback));
		}

		template <typename Instance>
		void RegisterGlobalListener(Instance* instance, void (Instance::*memberFunc)(const EType&))
		{
			auto callback = [instance, memberFunc](const EType& event) {
				(instance->*memberFunc)(event);
				};
			RegisterGlobalListener(callback);
		}

		void Process() override
		{
			while (!m_queue.empty())
			{
				auto top = m_queue.front();
				m_queue.pop();

				for (auto listener : m_listeners)
				{
					listener.Fire(top);
				}
			}
		}

	private:
		std::queue<EType> m_queue;
		std::vector<Listener<EType>> m_listeners;
	};

	class EventSystem final : public EngineSystem
	{
	public:
		
		template <typename EType>
		void AddQueue()
		{
			auto typeIndex = std::type_index(typeid(EType));
			m_queues[typeIndex] = new EventQueue<EType>();
		}

		template <typename EType>
		EventQueue<EType>* GetQueue()
		{
			auto typeIndex = std::type_index(typeid(EType));
			auto it = m_queues.find(typeIndex);
			if (it != m_queues.end())
			{
				return static_cast<EventQueue<EType>*>(it->second);
			}
			return nullptr;
		}

		template <typename EType>
		void PostEvent(const EType& event)
		{
			auto queue = GetQueue<EType>();
			if (queue)
			{
				queue->PostEvent(event);
			}
			else
			{
				throw std::runtime_error("No queue found for this event type.");
			}
		}

		template <typename EType>
		void RegisterGlobalListener(std::function<void(const EType&)> callback)
		{
			auto queue = GetQueue<EType>();
			if (queue)
			{
				queue->RegisterGlobalListener(callback);
			}
			else
			{
				throw std::runtime_error("No queue found for this event type.");
			}
		}

		template <typename EType, typename Instance>
		void RegisterGlobalListener(Instance* instance, void (Instance::* memberFunc)(const EType&))
		{
			auto queue = GetQueue<EType>();
			if (queue)
			{
				queue->RegisterGlobalListener(instance, memberFunc);
			}
			else
			{
				throw std::runtime_error("No queue found for this event type.");
			}
		}

		void ProcessEvents();
		void TestEventSystem();

	private:
		void OnTestEvent(const TestEvent& e);

		std::unordered_map<std::type_index, IEventQueue*> m_queues;
		
	public:
		/* EngineSystem Interface */

		virtual std::string Name() const override { return "Event System"; }
		virtual void DrawGUI() override { return; }

		EventSystem(Engine* engine) : EngineSystem(engine) {}

	protected:

		virtual void Startup() override;
		virtual void Shutdown() override;
		
		// More friendship to allow Engine to access protected functions on derived class pointers
		friend class Engine;
	};
}