#pragma once

#include "Systems/EngineSystem.h"

namespace CE
{
	enum LogLevel : std::uint8_t
	{
		INFO,
		WARNING,
		ERROR
	};

	enum LogChannel : std::uint8_t
	{
		ENGINE,
		LOG_SYSTEM,
		INPUT_SYSTEM,
		EVENT_SYSTEM
	};

	class LogSystem;
	static LogSystem* g_log;

	class LogSystem final : public EngineSystem
	{
	public:


		void Log(LogLevel level, LogChannel channel, std::string message);

		static void LogInfo(LogChannel channel, std::string message)
		{
			if (g_log == nullptr)
				return;

			g_log->Log(LogLevel::INFO, channel, message);
		}

		static void LogWarning(LogChannel channel, std::string message)
		{
			if (g_log == nullptr)
				return;

			g_log->Log(LogLevel::WARNING, channel, message);
		}

		static void LogError(LogChannel channel, std::string message)
		{
			if (g_log == nullptr)
				return;

			g_log->Log(LogLevel::ERROR, channel, message);
		}


	private:

		constexpr const char* GetLogLevelName(LogLevel lvl)
		{
			switch (lvl)
			{
			case LogLevel::INFO: return "INFO";
			case LogLevel::WARNING: return "WARNING";
			case LogLevel::ERROR: return "ERROR";
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