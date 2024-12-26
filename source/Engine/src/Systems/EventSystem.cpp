#include "Systems/EventSystem.h"

namespace CE
{
	void EventSystem::Startup()
	{
		RegisterListener<TestEvent>([](const TestEvent& e) {
			LOG(EVENTS, "Lambda Listener Works! {}", e.name);
		});

		//RegisterListener(this, &EventSystem::OnTestEvent);

		TestEvent testE;
		testE.someData = 5.f;
		DispatchEvent(testE); // preferred syntax (auto event type)
	}

	void EventSystem::Shutdown()
	{

	}

	void EventSystem::OnTestEvent(const TestEvent& e)
	{
		LOG(EVENTS, "Member Listener Works! {}", e.name);
	}
}