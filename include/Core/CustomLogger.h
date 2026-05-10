// CustomLogger.h

#pragma once

#include <EASTL/memory.h>
#include <EASTL/string.h>
#include <memory>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
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
			if (m_logger) m_logger->trace(spdlog::fmt_lib::runtime(fmt), std::forward<Args>(args)...);
		}

		template<typename... Args>
		void Debug(std::string_view fmt, Args&&... args) const
		{
			if (m_logger) m_logger->debug(spdlog::fmt_lib::runtime(fmt), std::forward<Args>(args)...);
		}

		template<typename... Args>
		void Info(std::string_view fmt, Args&&... args) const
		{
			if (m_logger) m_logger->info(spdlog::fmt_lib::runtime(fmt), std::forward<Args>(args)...);
		}

		template<typename... Args>
		void Warn(std::string_view fmt, Args&&... args) const
		{
			if (m_logger) m_logger->warn(spdlog::fmt_lib::runtime(fmt), std::forward<Args>(args)...);
		}

		template<typename... Args>
		void Error(std::string_view fmt, Args&&... args) const
		{
			if (m_logger) m_logger->error(spdlog::fmt_lib::runtime(fmt), std::forward<Args>(args)...);
		}

		template<typename... Args>
		void Critical(std::string_view fmt, Args&&... args) const
		{
			if (m_logger) m_logger->critical(spdlog::fmt_lib::runtime(fmt), std::forward<Args>(args)...);
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

#ifdef NENE_LOG_TRACE
#undef NENE_LOG_TRACE
#endif
#ifdef NENE_LOG_DEBUG
#undef NENE_LOG_DEBUG
#endif
#ifdef NENE_LOG_INFO
#undef NENE_LOG_INFO
#endif
#ifdef NENE_LOG_WARN
#undef NENE_LOG_WARN
#endif
// Fix define conflict with DiligentEngine
#ifdef NENE_LOG_ERROR
#undef NENE_LOG_ERROR
#endif
#ifdef NENE_LOG_CRITICAL
#undef NENE_LOG_CRITICAL
#endif

#define NENE_LOG_TRACE(...)    ::NeneEngine::CustomLogger::GetInstance().Trace(__VA_ARGS__)
#define NENE_LOG_DEBUG(...)    ::NeneEngine::CustomLogger::GetInstance().Debug(__VA_ARGS__)
#define NENE_LOG_INFO(...)     ::NeneEngine::CustomLogger::GetInstance().Info(__VA_ARGS__)
#define NENE_LOG_WARN(...)     ::NeneEngine::CustomLogger::GetInstance().Warn(__VA_ARGS__)
#define NENE_LOG_ERROR(...)    ::NeneEngine::CustomLogger::GetInstance().Error(__VA_ARGS__)
#define NENE_LOG_CRITICAL(...) ::NeneEngine::CustomLogger::GetInstance().Critical(__VA_ARGS__)
