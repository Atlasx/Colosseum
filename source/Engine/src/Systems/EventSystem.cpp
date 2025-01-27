#include "Systems/EventSystem.h"

#include "Engine.h"
#include "Systems/InputSystem.h"
#include "GUI/Editor.h"

namespace CE
{
	void EventSystem::Startup()
	{
		AddQueue<GameplayEvent>();

		std::shared_ptr<DebugSystem> DS = m_engine->GetSystem<DebugSystem>();
		if (DS)
		{
			DS->Subscribe(this);
		}

		m_showDebug = true;
	}

	void EventSystem::Shutdown()
	{
		for (auto [index, queue] : m_queues)
		{
			delete queue;
		}
	}

	void EventSystem::ProcessEvents()
	{
		for (auto& [typeIndex, queue] : m_queues)
		{
			queue->Process();
		}
	}

	void EventSystem::TestEventSystem()
	{
		AddQueue<TestEvent>();

		TestEvent testA;
		testA.someData = 5;
		testA.someString = "Test A";

		TestEvent testB;
		testB.someString = "Test B";

		TestEvent testC;
		testC.someString = "Test C";

		RegisterGlobalListener<TestEvent>([](TestEvent e) {
			LOG(EVENTS, "Listener works! {}", e.someString);
			});

		RegisterGlobalListener<TestEvent, EventSystem>(this, &EventSystem::OnTestEvent);

		PostEvent(testA);
		PostEvent(testB);
		PostEvent(testC);
		PostEvent(testB);
		PostEvent(testA);

		// Register a lambda to post an event on input action
		std::shared_ptr<InputSystem> IS = m_engine->GetSystem<InputSystem>();
		IS->RegisterAction("Event System Hook", KeyType::P, [this]() {
			TestEvent inputActionEventTest;
			inputActionEventTest.moreData = 5;
			inputActionEventTest.someString = "Input Hook";
			PostEvent(inputActionEventTest);
		});
	}

	void EventSystem::OnTestEvent(const TestEvent& e)
	{
		LOG(EVENTS, "Member Listener Works! {}", e.someString);
	}

	void EventSystem::OnDrawGUI()
	{
		if (m_showDebug == false) return;

		ImGui::Begin("Event System Debug", &m_showDebug);
		if (ImGui::CollapsingHeader("Queues", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto [typeIndex, queue] : m_queues)
			{
				std::string queueName = std::string(queue->GetQueueName());
				ImGui::Bullet();
				ImGui::Text(queueName.c_str());
			}
		}

		if (ImGui::CollapsingHeader("Pending Events", ImGuiTreeNodeFlags_DefaultOpen))
		{
			
			ImGui::Text("Show event queues here...");
		}

		if (ImGui::CollapsingHeader("Listeners", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Show listeners here...");
		}

		ImGui::End();
	}
}