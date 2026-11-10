// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <mutex>

#include "../include/drwalin_util/time.hpp"
#include "../include/drwalin_util/pretty_function.hpp"
#include "../include/drwalin_util/log.hpp"

#ifdef __unix__
#include <sys/syscall.h>
#include <unistd.h>
#ifdef SYS_gettid
static uint64_t gen_thread_id() { return syscall(SYS_gettid); }
static uint64_t get_process_id() { return getpid(); }
#define GEN_THREAD_ID() gen_thread_id()
#endif
#else
#include <windows.h>
static uint64_t gen_thread_id() { return GetCurrentThreadId(); }
static uint64_t get_process_id() { return GetCurrentProcessId(); }
#define GEN_THREAD_ID() gen_thread_id()
#endif
#if !defined(GEN_THREAD_ID)
#include <windows.h>
#include <atomic>
static uint64_t gen_thread_id()
{
	return get_current_thread_id();
	static std::atomic<uint64_t> glob_id = 1;
	return glob_id++;
}
static uint64_t get_process_id() { return 0; }
#warn UNKNWON ENVIRNMENT : not *nix nor windows
#define GEN_THREAD_ID() gen_thread_id()
#endif

#ifndef DRWUTIL_LOG_USE_PRETTY_FUNCTION
#define DRWUTIL_LOG_USE_PRETTY_FUNCTION 1
#endif

#ifndef DRWUTIL_LOG_DATETIME_SUBSECONDS_DIGITS
#define DRWUTIL_LOG_DATETIME_SUBSECONDS_DIGITS 5
#endif

namespace dutl
{
namespace log
{
#ifndef DRWUTIL_LOG_DEFAULT_LOG_LEVEL
#define DRWUTIL_LOG_DEFAULT_LOG_LEVEL dutl::log::LL_IGNORE
#endif

static log_level global_log_level = DRWUTIL_LOG_DEFAULT_LOG_LEVEL;

log_level get_global_log_level() { return global_log_level; }

void set_global_log_level(log_level level) { global_log_level = level; }

bool is_log_level_applicable(log_level level)
{
	return level <= global_log_level;
}

const char *log_level_to_name(log_level level)
{
#ifdef DRWUTIL_LOG_LEVEL_SINGLE_CHARACTER
	static char ar[256][4];
	switch (level) {
	case FATAL:
		return "F";
	case STRONG_ASSERT:
		return "A";
	case LL_ERROR:
		return "E";
	case WARN:
		return "W";
	case INFO:
		return "I";
	case DEBUG_ASSERT:
		return "A";
	case DEBUG:
		return "D";
	case TRACE:
		return "T";
	default:
		sprintf(ar[level], "%3u", (int)level);
		return ar[level];
	}
	return "UNDEF";
#else
	static char ar[256][4];
	switch (level) {
	case FATAL:
		return "FATAL";
	case STRONG_ASSERT:
		return "ASERT";
	case LL_ERROR:
		return "ERROR";
	case WARN:
		return "WARN ";
	case INFO:
		return "INFO ";
	case DEBUG_ASSERT:
		return "ASRTD";
	case DEBUG:
		return "DEBUG";
	case TRACE:
		return "TRACE";
	default:
		sprintf(ar[level], "%3u", (int)level);
		return ar[level];
	}
	return "UNDEF";
#endif
}

static bool enable_printing_time = false;

void global_disable_printing_time(bool disable_time)
{
	enable_printing_time = !disable_time;
}

static thread_local uint64_t thread_id = GEN_THREAD_ID();
static std::mutex mutex;

int32_t log_snprintf(char *buf, int32_t BYTES, log_level level, bool print_time,
					 const char *file, int line,
					 const char *function, const char *fmt, va_list va)
{
	print_time |= enable_printing_time;
	if (is_log_level_applicable(level) == false) {
		return 0;
	}

	time::time_string timestamp;
	timestamp.str[0] = 0;
	if (print_time) {
		timestamp = time::get_current_timestamp_string(
			DRWUTIL_LOG_DATETIME_SUBSECONDS_DIGITS);
	}

	std::string func_name = pretty_function(function);

	int offset = 0;

	snprintf(buf + offset, BYTES - offset, "%s ", log_level_to_name(level));

	if (print_time) {
		offset = strlen(buf);
		snprintf(buf + offset, BYTES - offset, "%s [%3lu] ", timestamp.str,
				 thread_id);
	}

	if (file != nullptr) {
		std::string file_path =
			file != nullptr ? pretty_file_path(file, false) : "";
		offset = strlen(buf);
		snprintf(buf + offset, BYTES - offset, "%s:%i ", file_path.c_str(),
				 line);
	}
	offset = strlen(buf);
	snprintf(buf + offset, BYTES - offset, "%s : ", func_name.c_str());
	offset = strlen(buf);
	vsnprintf(buf + offset, BYTES - offset, fmt, va);
	offset = strlen(buf);
	snprintf(buf + offset, BYTES - offset, "\n");
	offset = strlen(buf);

	return offset;
}

int32_t log_snprintf(char *buffer, int32_t buffer_size, log_level level,
					 bool print_time, const char *file,
					 int line, const char *function, const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int32_t ret = log_snprintf(buffer, buffer_size, level, print_time,
							   file, line, function, fmt, va);
	va_end(va);
	return ret;
}

void log(log_level level, bool print_time, const char *file,
		 int line, const char *function, const char *fmt, ...)
{
	if (is_log_level_applicable(level) == false) {
		return;
	}

	constexpr int BYTES = 16 * 1024;
	char buf[BYTES];

	va_list va;
	va_start(va, fmt);

	int32_t offset = log_snprintf(buf, BYTES, level, print_time,
								  file, line, function, fmt, va);

	if (offset > 0) {
		write_sync(buf, offset);
	}
	va_end(va);
}

void hex_dump(void *buf, int bytes) { default_logger.hex_dump(buf, bytes); }

void print_line_sync(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	default_logger.vprintf_line_sync(fmt, va);
	va_end(va);
}

void write_sync(const char *buf, int bytes)
{
	default_logger.write_sync(buf, bytes);
}
} // namespace log
} // namespace dutl
