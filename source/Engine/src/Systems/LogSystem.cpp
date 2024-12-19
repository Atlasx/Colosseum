#include "Systems/LogSystem.h"

#include "imgui.h"
#include <iostream>
#include <format>
#include <string>
#include <chrono>
#include <sstream>

namespace CE
{
	LogSystem* g_log = nullptr;

	void LogSystem::Log(LogLevel level, LogChannel channel, std::string_view msg, auto&&... msgArgs)
	{

		// TODO build tag string separately
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
				tag << std::vformat("[{}]", std::make_format_args(GetLogLevelName(level)));
			}
			if (m_bShowChannel)
			{
				tag << std::vformat("[{}]", std::make_format_args(GetLogChannelName(channel)));
			}

			message = std::vformat(std::string(msg),
				std::make_format_args(std::forward<decltype(msgArgs)>(msgArgs)...)
			);
			
			message = std::vformat("{} {}\n",
				std::make_format_args(tag.str(), message)
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


	}

	void LogSystem::LogInfo(LogChannel channel, std::string_view msg, auto&&... args)
	{
		if (g_log == nullptr) return;
		g_log->Log(LogLevel::INFO, channel, msg, std::forward<decltype(args)>(args)...);
	}

	void LogSystem::LogWarning(LogChannel channel, std::string_view msg, auto... args)
	{
		if (g_log == nullptr) return;
		g_log->Log(LogLevel::WARNING, channel, msg, std::forward<decltype(args)>(args)...);
	}

	void LogSystem::LogError(LogChannel channel, std::string_view msg, auto... args)
	{
		if (g_log == nullptr) return;
		g_log->Log(LogLevel::ERROR, channel, msg, std::forward<decltype(args)>(args)...);
	}

	void LogSystem::Startup()
	{
		g_log = this;

		LOG(LOG_SYSTEM, "Startup {}", 0.f);

		LOG_INFO(LOG_SYSTEM, "Another test");
		LOG_INFO(LOG_SYSTEM, "Formatting {} {}", "test", 15);
		LOG_WARN(LOG_SYSTEM, "Warning! {}", "Exceeded something!");
		LOG_ERROR(LOG_SYSTEM, "This is a big bad error!", "asdf");
	}

	void LogSystem::Shutdown()
	{
		LogInfo(LogChannel::LOG_SYSTEM, "Shutdown");

		g_log = nullptr;
	}

	void LogSystem::DrawGUI()
	{
		if (!m_showDebug)
			return;

		ImGui::SetNextWindowSize(ImVec2(700.f, 400.f), ImGuiCond_Appearing);
		ImGui::Begin("Log");

		ImGui::Text("This is where the logging system will go");

		ImGui::End();
	}
}