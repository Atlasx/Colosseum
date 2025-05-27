#pragma once

#include "imgui.h"

#include <vector>
#include <unordered_map>
#include <functional>

#include "DebugGUISubscriber.h"
#include "Systems/EngineSystem.h"
#include "Systems/RenderSystem.h"

namespace CE
{

	class DebugSystem : public EngineSystem, public IFrameEventSubscriber
	{
	public:
		DebugSystem(Engine* engine) :
			EngineSystem(engine),
			m_debugSubscribers()
		{}

		/* EngineSystem Interface */
	public:
		virtual std::string Name() const override { return "Debug System"; }
	protected:
		friend class Engine;
		virtual void Startup() override;
		virtual void Shutdown() override;


		/* IFrameEventSubscriber Interface */
	public:
		virtual void OnDoFrame() override;
	private:
		void DrawMainMenu();
		void NotifyOnDrawGUI();


		/* Debug System */
	public:
		void Subscribe(IDebugGUI* sub);
	private:
		struct DebugView
		{
			bool showDebug;
			IDebugGUI* subscriber;

			DebugView() : showDebug(true), subscriber(nullptr) {}
		};

		std::vector<DebugView> m_debugSubscribers;
	};
}