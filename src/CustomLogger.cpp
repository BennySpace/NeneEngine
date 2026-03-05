// CustomLogger.cpp

#include "CustomLogger.h"

#include <iostream>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace NeneEngine
{

	CustomLogger& CustomLogger::GetInstance()
	{
		static CustomLogger instance;
		return instance;
	}

	bool CustomLogger::Initialize(
		const std::string&			logFileName,
		bool						async,
		spdlog::level::level_enum	logLevel,
		bool						consoleWithColor)
	{
		try {
			std::vector<spdlog::sink_ptr> sinks;
			auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

			if (consoleWithColor)
			{
				console->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
				sinks.push_back(console);
			}
			else
			{
				console->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
				sinks.push_back(console);
			}

			if (!logFileName.empty())
			{
				auto file = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFileName.c_str(), true);
				file->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
				sinks.push_back(file);
			}

			if (async)
			{
				spdlog::init_thread_pool(8192, 1);
				m_logger = std::make_shared<spdlog::async_logger>(
					"game",
					sinks.begin(), sinks.end(),
					spdlog::thread_pool(),
					spdlog::async_overflow_policy::block
				);
			}
			else
			{
				m_logger = std::make_shared<spdlog::logger>("game", sinks.begin(), sinks.end());
			}

			m_logger->set_level(logLevel);
			m_logger->flush_on(spdlog::level::err);

			spdlog::set_default_logger(m_logger);
			m_logger->info("Logger initialized (async={}, level={})", async, spdlog::level::to_string_view(logLevel));

			return true;
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			std::cerr << "spdlog initialization failed: " << ex.what() << std::endl;
			return false;
		}
	}

	void CustomLogger::SetLevel(spdlog::level::level_enum lvl)
	{
		if (m_logger) {
			m_logger->set_level(lvl);
		}
	}

} // namespace engine