// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_LOG_HPP
#define DRWUTIL_LOG_HPP

#include "logger.hpp"

#ifdef DRWUTIL_LOG_USE_PRETTY_FUNCTION
#define __FUNCTION_NAME__ __PRETTY_FUNCTION__
#else
#define __FUNCTION_NAME__ __FUNCTION__
#endif

#define DEBUG(...)                                                             \
	dutl::log::log(dutl::log::DEBUG, true, true, __FILE__, __LINE__,           \
				   __FUNCTION_NAME__, __VA_ARGS__)

#define LOG_FULL(LEVEL, USE_TIME, ...)                                         \
	dutl::log::log(dutl::log::log_level::LEVEL, USE_TIME, __FILE__, __LINE__,  \
				   __FUNCTION_NAME__, __VA_ARGS__)

#define LOG(LEVEL, ...) LOG_FULL(LEVEL, true, __VA_ARGS__)
#define LOG_(LEVEL, ...) LOG_FULL(LEVEL, false, __VA_ARGS__)
#define LOG_T(LEVEL, ...) LOG_FULL(LEVEL, true, __VA_ARGS__)

#define LOG_FATAL_ARGS(USE_TIME, ...) LOG_FULL(FATAL, USE_TIME, __VA_ARGS__)
#define LOG_FATAL(...) LOG_FATAL_ARGS(true, __VA_ARGS__)
#define LOG_FATAL_(...) LOG_FATAL_ARGS(false, __VA_ARGS__)
#define LOG_FATAL_T(...) LOG_FATAL_ARGS(true, __VA_ARGS__)

#define LOG_ERROR_ARGS(USE_TIME, ...) LOG_FULL(LL_ERROR, USE_TIME, __VA_ARGS__)
#define LOG_ERROR(...) LOG_ERROR_ARGS(true, __VA_ARGS__)
#define LOG_ERROR_(...) LOG_ERROR_ARGS(false, __VA_ARGS__)
#define LOG_ERROR_T(...) LOG_ERROR_ARGS(true, __VA_ARGS__)

#ifndef DRWUTIL_DISABLE_LOG_LEVEL_WARN
#define LOG_WARN_ARGS(USE_TIME, ...) LOG_FULL(WARN, USE_TIME, __VA_ARGS__)
#else
#define DRWUTIL_DISABLE_LOG_LEVEL_INFO
#define LOG_WARN_ARGS(USE_TIME, ...)
#endif
#define LOG_WARN(...) LOG_WARN_ARGS(true, __VA_ARGS__)
#define LOG_WARN_(...) LOG_WARN_ARGS(false, __VA_ARGS__)
#define LOG_WARN_T(...) LOG_WARN_ARGS(true, __VA_ARGS__)

#ifndef DRWUTIL_DISABLE_LOG_LEVEL_INFO
#define LOG_INFO_ARGS(USE_TIME, ...) LOG_FULL(INFO, USE_TIME, __VA_ARGS__)
#else
#define DRWUTIL_DISABLE_LOG_LEVEL_DEBUG
#define LOG_INFO_ARGS(USE_TIME, ...)
#endif
#define LOG_INFO(...) LOG_INFO_ARGS(true, __VA_ARGS__)
#define LOG_INFO_(...) LOG_INFO_ARGS(false, __VA_ARGS__)
#define LOG_INFO_T(...) LOG_INFO_ARGS(true, __VA_ARGS__)

#ifndef DRWUTIL_DISABLE_LOG_LEVEL_DEBUG
#define LOG_DEBUG_ARGS(USE_TIME, ...) LOG_FULL(DEBUG, USE_TIME, __VA_ARGS__)
#else
#define DRWUTIL_DISABLE_LOG_LEVEL_TRACE
#define LOG_DEBUG_ARGS(USE_TIME, ...)
#endif
#define LOG_DEBUG(...) LOG_DEBUG_ARGS(true, __VA_ARGS__)
#define LOG_DEBUG_(...) LOG_DEBUG_ARGS(false, __VA_ARGS__)
#define LOG_DEBUG_T(...) LOG_DEBUG_ARGS(true, __VA_ARGS__)

#ifndef DRWUTIL_DISABLE_LOG_LEVEL_TRACE
#define LOG_TRACE_ARGS(USE_TIME, ...) LOG_FULL(TRACE, USE_TIME, __VA_ARGS__)
#else
#define LOG_TRACE_ARGS(USE_TIME, ...)
#endif
#define LOG_TRACE(...) LOG_TRACE_ARGS(true, __VA_ARGS__)
#define LOG_TRACE_(...) LOG_TRACE_ARGS(false, __VA_ARGS__)
#define LOG_TRACE_T(...) LOG_TRACE_ARGS(true, __VA_ARGS__)

namespace dutl
{
namespace log
{
// returns number of written bytes
int32_t log_snprintf(char *buffer, int32_t buffer_size, log_level level,
					 bool print_time, const char *file, int line,
					 const char *function, const char *fmt, ...);

void global_disable_printin_time(bool disable_time);

const char *log_level_to_name(log_level level);

log_level get_global_log_level();
void set_global_log_level(log_level level);

bool is_log_level_applicable(log_level level);

void log(log_level level, bool print_time, const char *file, int line,
		 const char *function, const char *fmt, ...);

void hex_dump(void *buf, int bytes);

void print_line_sync(const char *fmt, ...);
void write_sync(const char *buf, int bytes);
} // namespace log
} // namespace dutl

#endif
