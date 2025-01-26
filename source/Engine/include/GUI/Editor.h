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
		/* EngineSystem Interface */
	public:
		virtual std::string Name() const override { return "Debug System"; }

		DebugSystem(Engine* engine) :
			EngineSystem(engine),
			m_debugSubscribers()
		{}

	protected:

		virtual void Startup() override;
		virtual void Shutdown() override;

		friend class Engine;

	public:

		void Subscribe(IDebugGUISubscriber* sub);
	private:
		std::vector<IDebugGUISubscriber*> m_debugSubscribers;

		/* IFrameEventSubscriber Interface */
	public:
		virtual void OnDoFrame() override;
	private:
		void DrawMainMenu();
		void NotifyOnDrawGUI();

	};
}