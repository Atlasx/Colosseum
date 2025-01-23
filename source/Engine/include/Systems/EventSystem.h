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

		TestEvent() : someData(10), moreData(2.f) {}
	};

	template <IsEvent EType>
	class Listener
	{
	public:
		void Setup(std::function<void(EType)> callable)
		{
			m_callable = callable;
		}

		void Fire(EType e)
		{
			if (m_callable)
			{
				m_callable(e);
			}
		}

		std::function<void(EType)> m_callable;
	};

	class EventSystem final : public EngineSystem
	{
	public:
		
		template <IsEvent EType>
		void PostEvent(EType event)
		{
			// Copy event to local storage
			const std::type_index eventIndex = typeid(EType);
			auto listeners = m_listenerStorage[eventIndex];
			if (listeners.size() > 0)
			{
				for (auto listener : listeners)
				{
					auto l = static_cast<Listener<EType>*>(listener);
					if (l != nullptr)
					{
						l->Fire(event);
					}
				}
			}
		}

		template <IsEvent EType>
		void RegisterListener(std::function<void(EType)> callback)
		{
			auto eventListener = new Listener<EType>();
			eventListener->Setup(std::move(callback));

			const std::type_index eventIndex = typeid(EType);
			if (m_listenerStorage.find(eventIndex) != m_listenerStorage.end())
			{
				m_listenerStorage[eventIndex].emplace_back(eventListener);
			}
			else
			{
				m_listenerStorage[eventIndex].emplace_back(eventListener);
			}
		}

		std::unordered_map<std::type_index, std::vector<void*>> m_listenerStorage;

		void ProcessEvents();
		void OnTestEvent(const Event& e);

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