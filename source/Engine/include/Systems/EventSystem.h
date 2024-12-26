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

	// Use this concept to provide compiler errors if user code tries to use a bad type
	template<typename EventType>
	concept ValidEventType = std::is_base_of_v<BaseEvent, EventType>;

	struct TestEvent : BaseEvent
	{
		float someData;
		char name[10] = "TestEvent";
	};

	template<typename Derived>
	class BaseListener
	{
	protected:
		~BaseListener() = default;
	public:

		template <typename... Args>
		void HandleEvent(Args&&... args)
		{
			static_cast<Derived*>(this)->HandleEventImpl(std::forward<Args>(args)...);
		}
	};

	template<ValidEventType _ET>
	class GlobalListener : public BaseListener<GlobalListener<_ET>>
	{
	public:
		using EventType = _ET;
		using Callback = std::function<void(const EventType&)>;

		explicit GlobalListener(Callback cb) : m_callback(cb) {}
		~GlobalListener() = default;

		void HandleEventImpl(const EventType& event)
		{
			m_callback(event);
		}

	private:
		Callback m_callback;
	};

	template<ValidEventType _ET, typename _T>
	class MemberFuncListener : public BaseListener<MemberFuncListener<_ET, _T>>
	{
	public:
		using InstanceType = _T;
		using EventType = _ET;
		using Callback = void (InstanceType::*)(const EventType&);

		MemberFuncListener(InstanceType* instance, Callback cb)
			: m_instance(instance), m_callback(cb) {}

		~MemberFuncListener() = default;

		void HandleEventImpl(const EventType& event)
		{
			(m_instance->*m_callback)(event);
		}

	private:
		InstanceType* m_instance;
		Callback m_callback;
	};
	
	class ListenerWrapper
	{
	public:
		template <typename ListenerType>
		ListenerWrapper(ListenerType listener)
			: m_listener(std::make_shared<ListenerType>(std::move(listener))),
			m_typeInfo(typeid(ListenerType)) {}

		template <typename... Args>
		void HandleEvent(Args&&... args)
		{
			// Dispatch to the listener's HandleEvent function
			// The correct HandleEventImpl will be invoked for the specific listener type
			m_listener->HandleEvent(std::forward<Args>(args)...);
		}

		const std::type_info& GetTypeInfo() const { return m_typeInfo; }

	private:
		std::shared_ptr<void> m_listener;
		const std::type_info& m_typeInfo;
	};

	// ListenerRegistry for storing and dispatching events
	class ListenerRegistry
	{
	public:
		template <typename EventType, typename CallbackType>
		void RegisterListener(CallbackType callback)
		{
			auto listener = GlobalListener<EventType>(std::move(callback));
			auto wrapper = ListenerWrapper(std::move(listener));
			m_listeners[typeid(EventType)].push_back(std::move(wrapper));
		}

		template <typename EventType, typename ObjectType, typename MemberFunc>
		void RegisterMemberListener(ObjectType* obj, MemberFunc func)
		{
			auto listener = MemberFuncListener<EventType, ObjectType, MemberFunc>(obj, func);
			auto wrapper = ListenerWrapper(std::move(listener));
			m_listeners[typeid(EventType)].push_back(std::move(wrapper));
		}

		template <typename EventType>
		std::vector<ListenerWrapper>& GetListeners()
		{
			return m_listeners[typeid(EventType)];
		}

	private:
		std::unordered_map<std::type_index, std::vector<ListenerWrapper>> m_listeners;
	};

	class ObjectSystem;

	class EventSystem final : public EngineSystem
	{
	public:
		
		template <typename EventType>
		void DispatchEvent(const EventType& event)
		{
			auto& listeners = m_registry.GetListeners<EventType>();

			for (auto& listener : listeners)
			{
				listener.HandleEvent(event);
			}
		}

		// Delegate listener registration to ListenerRegistry
		template <typename EventType, typename CallbackType>
		void RegisterListener(CallbackType callback)
		{
			m_registry.RegisterListener<EventType>(std::move(callback));
		}

		// Delegate member listener registration to ListenerRegistry
		template <typename EventType, typename ObjectType, typename MemberFunc>
		void RegisterMemberListener(ObjectType* obj, MemberFunc func)
		{
			m_registry.RegisterMemberListener<EventType>(obj, func);
		}

	private:
		ListenerRegistry m_registry;

	private:

		// Test member function for event callback
		void OnTestEvent(const TestEvent& e);
		
		//ObjectPool<GlobalListener, 1024, ListenerHandle> m_listeners;

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