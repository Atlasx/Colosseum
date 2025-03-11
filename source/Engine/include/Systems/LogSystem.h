#pragma once

#include "Systems/EngineSystem.h"
#include "Globals.h"
#include "GUI/DebugGUISubscriber.h"

#include "stdlibincl.h"

#define DEBUG_BREAK()			\
	if (CE::Globals::bDebugBreakOnError) {	\
		PLATFORM_BREAK();		\
	}

#if defined(_WIN32) || defined(_WIN64)
	#include <intrin.h>
	#define PLATFORM_BREAK() __debugbreak()
#else
	#define PLATFORM_BREAK() ((void)0)
#endif

#define LOG(systemName, message, ...) \
	CE::LogSystem::Log(CE::LogLevel::INFO, CE::LogChannel::systemName, message, ##__VA_ARGS__);

#define LOG_INFO(systemName, message, ...) \
	CE::LogSystem::LogInfo(CE::LogChannel::systemName, message, ##__VA_ARGS__);

#define LOG_WARN(systemName, message, ...) \
	CE::LogSystem::LogWarning(CE::LogChannel::systemName, message, ##__VA_ARGS__);

#define LOG_ERROR(systemName, message, ...)						\
    do {														\
        CE::LogSystem::LogError(CE::LogChannel::systemName,		\
			message, ##__VA_ARGS__);							\
        DEBUG_BREAK();											\
    } while (0)


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
		RENDER,
		LOGGER,
		INPUT,
		EVENTS,
		RESOURCES,
		GAMEPLAY,
		WORLD,
		ENTITY,
		MAX
	};

	// Chicken and egg here
	class LogSystem;
	extern LogSystem* g_log;
	
	class LogSystem final : public EngineSystem, private IDebugGUISubscriber
	{
	public:
		LogSystem(Engine* engine) :
			EngineSystem(engine),
			m_log() 
		{};


		/* EngineSystem Interface */
	public:
		virtual std::string Name() const override { return "Log System"; }
	private:
		friend class Engine;
		virtual void Startup() override;
		virtual void Shutdown() override;


		/* IDebugGUISubscriber Interface */
		virtual void OnDrawGUI() override;
		virtual std::string_view GetDebugMenuName() { return "Debug Log"; }


		/* Log System */
	public:
		template<typename... Args>
		static void Log(LogLevel level, LogChannel channel, std::string msg, Args&&... msgArgs)
		{
			assert(g_log != nullptr);
			std::string message;
			if constexpr (sizeof...(Args) == 0)
			{
				message = std::move(msg);
			}
			else
			{
				message = std::vformat(std::string_view(msg),
					std::make_format_args(std::forward<Args>(msgArgs)...)
				);
			}
			g_log->LogImpl(level, channel, message);
		}

		template<typename... Args>
		static void LogInfo(LogChannel channel, std::string msg, Args&&... args)
		{
			Log(LogLevel::INFO, channel, msg, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void LogWarning(LogChannel channel, std::string msg, Args&&... args)
		{
			Log(LogLevel::WARNING, channel, msg, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void LogError(LogChannel channel, std::string msg, Args&&... args)
		{
			Log(LogLevel::ERROR, channel, msg, std::forward<Args>(args)...);
		}

		bool m_bShowLevel = true;
		bool m_bShowChannel = true;
		bool m_bShowTimestamp = true;

		bool m_bLogToCout = true;
		bool m_bLogToLog = true;

	private:

		struct LogElement
		{
			LogLevel level;
			LogChannel channel;
			std::string message;

			LogElement(LogLevel l, LogChannel c, std::string msg) :
				level(l), channel(c), message(std::move(msg))
			{};
		};

		std::vector<LogElement> m_log;

		void LogImpl(LogLevel level, LogChannel channel, std::string_view message);


	};

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
		case LogChannel::RENDER: return "Render";
		case LogChannel::LOGGER: return "Logger";
		case LogChannel::INPUT: return "Input";
		case LogChannel::EVENTS: return "Events";
		case LogChannel::RESOURCES: return "Resources";
		case LogChannel::GAMEPLAY: return "Gameplay";
		case LogChannel::ENTITY: return "Entity";
		case LogChannel::WORLD: return "World";
		default: return "";
		}
	}



}