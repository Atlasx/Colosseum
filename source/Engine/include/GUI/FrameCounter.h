#pragma once

class FrameCounter : public IDebugGUISubscriber
{
	using TimePoint = std::chrono::steady_clock::time_point;

	inline static const double targetFrameDuration = 1000.0 / 60.f;

public:
	FrameCounter() : m_fps(0), m_frameCount(0), m_totalTime(0), m_frameStart(), m_frameEnd() {}

	void FrameStart()
	{
		m_frameStart = std::chrono::high_resolution_clock::now();
	}

	double FrameEnd()
	{
		m_frameEnd = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::milli> frameDuration = m_frameEnd - m_frameStart;

		m_deltaTime = frameDuration.count();
		m_fps = 1000.f / m_deltaTime;

		if (frameDuration.count() < targetFrameDuration)
		{
			return targetFrameDuration - frameDuration.count();
		}
		return 0;
	}

	virtual void OnDrawGUI() override
	{
		ImGuiWindowFlags statsFlags = 0;
		statsFlags |= ImGuiWindowFlags_NoCollapse;
		statsFlags |= ImGuiWindowFlags_NoResize;
		statsFlags |= ImGuiWindowFlags_NoDocking;
		statsFlags |= ImGuiWindowFlags_NoSavedSettings;

		ImGui::SetNextWindowPos(ImVec2(10, 30));
		ImGui::SetNextWindowSize(ImVec2(160, 70));
		ImGui::Begin("Stats", 0, statsFlags);

		ImGui::Text("FPS: %d", m_fps);
		ImGui::Text("DeltaTime: %.2fms", m_deltaTime);

		ImGui::End();
	}

	virtual std::string_view GetDebugMenuName() override { return "FPS"; }

private:
	int m_fps;
	int m_frameCount;
	double m_totalTime;
	double m_deltaTime;
	TimePoint m_frameStart;
	TimePoint m_frameEnd;
};