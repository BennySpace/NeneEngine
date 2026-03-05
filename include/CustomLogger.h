// CustomLogger.h

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>
#include <EASTL/string.h>
#include <EASTL/memory.h>
#include <memory>
#include <string_view>

namespace NeneEngine
{

	class CustomLogger final
	{
	public:
		CustomLogger(const CustomLogger&) = delete;
		CustomLogger& operator=(const CustomLogger&) = delete;
		CustomLogger(CustomLogger&&) = delete;
		CustomLogger& operator=(CustomLogger&&) = delete;

		static CustomLogger& GetInstance();

		bool Initialize(
			const std::string&			logFileName			= "NeneEngine.log",
			bool						async				= true,
			spdlog::level::level_enum	logLevel			= spdlog::level::level_enum::info,
			bool						consoleWithColor	= true
		);

		void SetLevel(spdlog::level::level_enum lvl);

		template<typename... Args>
		void Trace(std::string_view fmt, Args&&... args) const
		{
			if (m_logger) m_logger->trace(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void Debug(std::string_view fmt, Args&&... args) const
		{
			if (m_logger) m_logger->debug(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void Info(std::string_view fmt, Args&&... args) const
		{
			if (m_logger) m_logger->info(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void Warn(std::string_view fmt, Args&&... args) const
		{
			if (m_logger) m_logger->warn(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void Error(std::string_view fmt, Args&&... args) const
		{
			if (m_logger) m_logger->error(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void Critical(std::string_view fmt, Args&&... args) const
		{
			if (m_logger) m_logger->critical(fmt, std::forward<Args>(args)...);
		}

		std::shared_ptr<spdlog::logger> GetRawLogger() const
		{
			return m_logger;
		}

	private:
		CustomLogger() = default;
		~CustomLogger() = default;

		std::shared_ptr<spdlog::logger> m_logger;
	};

} // namespace NeneEngine

#define LOG_TRACE(...)   ::NeneEngine::CustomLogger::Instance().Trace(__VA_ARGS__)
#define LOG_DEBUG(...)   ::NeneEngine::CustomLogger::Instance().Debug(__VA_ARGS__)
#define LOG_INFO(...)    ::NeneEngine::CustomLogger::Instance().Info(__VA_ARGS__)
#define LOG_WARN(...)    ::NeneEngine::CustomLogger::Instance().Warn(__VA_ARGS__)
#define LOG_ERROR(...)   ::NeneEngine::CustomLogger::Instance().Error(__VA_ARGS__)
#define LOG_CRITICAL(...)::NeneEngine::CustomLogger::Instance().Critical(__VA_ARGS__)