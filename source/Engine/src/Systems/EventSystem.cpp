#include "Systems/EventSystem.h"

#include "Engine.h"
#include "Systems/InputSystem.h"

namespace CE
{
	void EventSystem::Startup()
	{

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

	void EventSystem::TestEventSystem()
	{
		TestEvent testA;
		testA.someData = 5;

		RegisterGlobalListener<TestEvent>([](TestEvent e) {
			LOG(EVENTS, "Listener works!");
			});

		RegisterGlobalListener<TestEvent, EventSystem>(this, &EventSystem::OnTestEvent);

		PostEvent(testA);

		std::shared_ptr<InputSystem> IS = m_engine->GetSystem<InputSystem>();
		IS->RegisterAction("Event System Hook", KeyType::P, [this]() {
			TestEvent inputActionEventTest;
			inputActionEventTest.moreData = 5;
			this->PostEvent(inputActionEventTest);
			});
	}

	void EventSystem::OnTestEvent(TestEvent e)
	{
		LOG(EVENTS, "Member Listener Works!");
	}
}