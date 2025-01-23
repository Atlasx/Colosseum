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
	
	struct Event
	{
	public:
		Event() = default;
	};

	struct TestEvent : public Event
	{
		int someData;
		float moreData;

		TestEvent() : someData(10), moreData(2.f) {}
	};

	struct AnotherTestEvent
	{
		int otherData;
		std::string_view someString;

		AnotherTestEvent() : otherData(0), someString("A Literal") {}
	};
	namespace EventUtil
	{
		namespace Constants
		{
			static const std::size_t EVENT_DATA_MAX = 64;
		}

		enum class EventType
		{
			ET_Base = 0,
			ET_TestEvent,
			ET_AnotherTestEvent
		};

		template<typename EType>
		constexpr EventType GetTypeOfEvent()
		{
			if constexpr (std::is_same_v<EType, Event>) { return EventType::ET_Base; }
			if constexpr (std::is_same_v<EType, TestEvent>) { return EventType::ET_TestEvent; }
			if constexpr (std::is_same_v<EType, AnotherTestEvent>) { return EventType::ET_AnotherTestEvent; }
		}

		struct EventWrapper
		{
			std::byte m_data[Constants::EVENT_DATA_MAX];
			std::size_t m_dataSize;

			EventType m_type;

			template <typename EType>
			auto Get() -> EType
			{
				assert(GetTypeOfEvent<EType>() == m_type); // need to do a runtime assert
				return *reinterpret_cast<EType*>(m_data); // trust me bro lol
			}

			EventWrapper() : m_type(EventType::ET_Base), m_dataSize(Constants::EVENT_DATA_MAX)
			{
				// May not need this as we should overwrite any needed data
				memset(m_data, 0, sizeof(m_data));
			}
		};

		template <typename EType>
		EventWrapper MakeWrapper(EType event)
		{
			static_assert(sizeof(EType) <= Constants::EVENT_DATA_MAX);
	
			EventWrapper wrapper;
			wrapper.m_type = GetTypeOfEvent<EType>();
			wrapper.m_dataSize = sizeof(EType);

			memcpy(wrapper.m_data, &event, sizeof(EType));
			return wrapper;
		}
	}

	class EventSystem final : public EngineSystem
	{
	public:
		
		template <typename EType>
		void PostEvent(EType event)
		{
			// Copy event to local storage
		}

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