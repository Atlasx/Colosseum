#pragma once

#include "Systems/EngineSystem.h"

#define LOG(systemName, message, ...) \
	g_log->LogSystem::Log(LogLevel::INFO, LogChannel::systemName, message, ##__VA_ARGS__);

#define LOG_INFO(systemName, message, ...) \
	LogSystem::LogInfo(LogChannel::systemName, message, ##__VA_ARGS__);

#define LOG_WARN(systemName, message, ...) \
	LogSystem::LogWarning(LogChannel::systemName, message, ##__VA_ARGS__);

#define LOG_ERROR(systemName, message, ...) \
	LogSystem::LogError(LogChannel::systemName, message, ##__VA_ARGS__);

namespace CE
{

	enum LogLevel : std::uint8_t
	{
		INFO,
		WARNING,
		ERROR
	};

	// Add new log channels here before use or compile error
	enum LogChannel : std::uint8_t
	{
		ENGINE,
		LOG_SYSTEM,
		INPUT_SYSTEM,
		EVENT_SYSTEM,
		GAMEPLAY
	};
	
	class LogSystem final : public EngineSystem
	{
	public:


		void Log(LogLevel level, LogChannel channel, std::string_view msg, auto&&... msgArgs);

		static void LogInfo(LogChannel channel, std::string_view msg, auto&&... args);

		static void LogWarning(LogChannel channel, std::string_view msg, auto... args);

		static void LogError(LogChannel channel, std::string_view msg, auto... args);

		bool m_bShowLevel = true;
		bool m_bShowChannel = true;
		bool m_bShowTimestamp = true;

		bool m_bLogToCout = true;

	private:

		constexpr const char* GetLogLevelName(LogLevel lvl)
		{
			switch (lvl)
			{
			case LogLevel::INFO: return "INFO";
			case LogLevel::WARNING: return "WARN";
			case LogLevel::ERROR: return "ERRR";
			default: return "";
			}
		}

		constexpr const char* GetLogChannelName(LogChannel ch)
		{
			switch (ch)
			{
			case LogChannel::ENGINE: return "Engine";
			case LogChannel::LOG_SYSTEM: return "LogSystem";
			case LogChannel::INPUT_SYSTEM: return "InputSystem";
			case LogChannel::EVENT_SYSTEM: return "EventSystem";
			case LogChannel::GAMEPLAY: return "Gameplay";
			default: return "";
			}
		}

		/* EngineSystem Interface */
	public:
		virtual std::string Name() const override { return "Log System"; }
		virtual void DrawGUI() override;

		LogSystem(Engine* engine) : EngineSystem(engine) {};

	private:
		friend class Engine;
		virtual void Startup() override;
		virtual void Shutdown() override;
	};

}