#include "Systems/LogSystem.h"

#include "Systems/InputSystem.h"
#include "GUI/Editor.h"
#include "Engine.h"

#include "imgui.h"

#ifdef CDEBUG
#include "Systems/Debug/LogSystemDebug.h"
#endif

namespace CE
{
	LogSystem* g_log = nullptr;

	void LogSystem::LogImpl(LogLevel level, LogChannel channel, std::string_view msg)
	{
		std::string message;
		try {
			std::stringstream tag;
			if (m_bShowTimestamp)
			{
				auto now = std::chrono::system_clock::now();
				auto time = std::chrono::system_clock::to_time_t(now);

				tag << std::put_time(std::localtime(&time), "[%Y-%m-%d %H:%M:%S]");
			}
			if (m_bShowLevel)
			{
				std::string_view logLevelStr(GetLogLevelName(level));
				tag << std::vformat("[{}]", std::make_format_args(logLevelStr));
			}
			if (m_bShowChannel)
			{
				std::string_view logChannelStr(GetLogChannelName(channel));
				tag << std::vformat("[{}]", std::make_format_args(logChannelStr));
			}

			std::string tagStr(tag.str());
			message = std::vformat("{} {}\n",
				std::make_format_args(tagStr, msg)
			);
		}
		catch (const std::format_error& e)
		{
			std::cerr << "Formatting Issue! Please resolve this\n";
			return;
		}
		catch (...)
		{
			std::cerr << "Something went wrong when logging!\n";
			return;
		}

		if (m_bLogToCout)
		{
			std::cout << message;
		}

		if (m_bLogToLog)
		{
			// Reset log every x msgs TODO: store to file here or something reasonable
			if (m_log.size() >= 1000)
			{
				m_log.clear();
				LOG_WARN(INPUT, "Exceeded Log Storage! Clearing.");
			}

			LogElement elem(level, channel, message);
			m_log.push_back(elem);
		}
	}

	void LogSystem::Startup()
	{
		g_log = this;

		LOG(LOGGER, "Startup");

#ifdef CDEBUG
		m_debugger = new LogSystemDebug(this);
		std::shared_ptr<DebugSystem> DS = m_engine->GetSystem<DebugSystem>();
		if (DS)
		{
			DS->Subscribe(m_debugger);
		}
#endif
	}

	void LogSystem::Shutdown()
	{
		LOG(LOGGER, "Shutdown");

		g_log = nullptr;

#ifdef CDEBUG
		delete m_debugger;
#endif
	}

}