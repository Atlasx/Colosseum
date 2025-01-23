#include "Systems/EventSystem.h"

namespace CE
{
	void EventSystem::Startup()
	{
		TestEvent testA;
		testA.moreData = 10.f;

		EventUtil::EventWrapper wrappedTestA = EventUtil::MakeWrapper(testA);

		TestEvent testB = wrappedTestA.Get<TestEvent>();

		AnotherTestEvent testC = wrappedTestA.Get<AnotherTestEvent>();

		/*
		RegisterListener(EventType::ET_Test, [](const Event& e) {
			LOG(EVENTS, "Lambda Listener Works!");
		});

		RegisterMemberListener(EventType::ET_Test, this, &EventSystem::OnTestEvent);

		Event testE;
		testE.m_type = EventType::ET_Test;
		FireEvent(testE);

		FireEvent(testE);

		Event testA;
		testA.m_type = EventType::ET_EventA;
		FireEvent(testA);

		RegisterListener(EventType::ET_EventA, [](const Event& e) {
			LOG(EVENTS, "EventA Listener Works!");
			});

		FireEvent(testA);
		*/
	}

	void EventSystem::Shutdown()
	{

	}

	void EventSystem::ProcessEvents()
	{
		/*
		for (auto& e : m_eventQueue)
		{
			for (auto& listener : m_listeners)
			{
				listener(e);  // Call all registered listeners
			}
		}
		m_eventQueue.clear();
		*/
	}

	void EventSystem::OnTestEvent(const Event& e)
	{
		LOG(EVENTS, "Member Listener Works!");
	}
}