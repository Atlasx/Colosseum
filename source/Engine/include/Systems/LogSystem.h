#pragma once

#include "Systems/EngineSystem.h"

namespace CE
{
	class LogSystem final : public EngineSystem
	{
	public:

		enum class LogLevel : std::uint8_t
		{
			INFO,
			WARNING,
			ERROR
		};

		void Log(LogLevel level, std::string channel, std::string message);

		void LogInfo(std::string channel, std::string message)
		{
			Log(LogLevel::INFO, channel, message);
		}

		void LogWarning(std::string channel, std::string message)
		{
			Log(LogLevel::WARNING, channel, message);
		}

		void LogError(std::string channel, std::string message)
		{
			Log(LogLevel::ERROR, channel, message);
		}

		void DeclareChannel(std::string name);

	private:

		

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