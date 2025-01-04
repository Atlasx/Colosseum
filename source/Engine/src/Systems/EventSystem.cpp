#include "Systems/EventSystem.h"

namespace CE
{
	void EventSystem::Startup()
	{
		RegisterListener<TestEvent>([](const TestEvent& e) {
			LOG(EVENTS, "Lambda Listener Works! {}", e.someString);
		});

		RegisterMemberListener(this, &EventSystem::OnTestEvent);

		TestEvent testE;
		testE.someData = 5.f;
		FireEvent(testE); // preferred syntax (auto event type)

		testE.someString = "CHANGE";
		FireEvent(testE);

		EventA testA;
		testA.x = 5;
		testA.y = 10;
		FireEvent(testA);

		RegisterListener<EventA>([](const EventA& e) {
			LOG(EVENTS, "EventA Listener Works!");
			});

		EventB testB;
		testB.x = 10.f;
		FireEvent(testB);
	}

	void EventSystem::Shutdown()
	{

	}

	void EventSystem::ProcessEvents()
	{
		for (auto& e : m_eventQueue)
		{
			auto it = m_listeners.find(e.type);
			if (it != m_listeners.end()) {
				for (auto& listener : it->second)
				{
					listener(e);  // Call all registered listeners
				}
			}
			else {
				// Debating on if this should even be a problem, many events might
				// just not have any listeners
				LOG_WARN(EVENTS, "No listener found for event! {}", e.type.name());
			}
		}
		m_eventQueue.clear();
	}

	void EventSystem::OnTestEvent(const TestEvent& e)
	{
		LOG(EVENTS, "Member Listener Works! {}", e.someString);
	}
}