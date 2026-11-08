// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <mutex>

#include "../include/drwutil/time.hpp"
#include "../include/drwutil/pretty_function.hpp"
#include "../include/drwutil/log.hpp"

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
	case LL_ERROR:
		return "E";
	case WARN:
		return "W";
	case INFO:
		return "I";
	case DEBUG:
		return "D";
	case TRACE:
		return "T";
	default:
		sprintf(ar[level], "%5u", (int)level);
		return ar[level];
	}
	return "UNDEF";
#else
	static char ar[256][4];
	switch (level) {
	case FATAL:
		return "FATAL";
	case LL_ERROR:
		return "ERROR";
	case WARN:
		return "WARN ";
	case INFO:
		return "INFO ";
	case DEBUG:
		return "DEBUG";
	case TRACE:
		return "TRACE";
	default:
		sprintf(ar[level], "%5u", (int)level);
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

void log(log_level level, bool print_time, bool print_file, const char *file,
		 int line, const char *function, const char *fmt, ...)
{
	print_time |= enable_printing_time;
	if (is_log_level_applicable(level) == false) {
		return;
	}

	std::string timestamp;
	if (print_time) {
		timestamp = time::get_current_timestamp_string(
						DRWUTIL_LOG_DATETIME_SUBSECONDS_DIGITS)
						.str;
	}

	std::string func_name = pretty_function(function);

	va_list va;
	va_start(va, fmt);

	constexpr int BYTES = 16 * 1024;
	char buf[BYTES];
	int offset = 0;

	snprintf(buf + offset, BYTES - offset, "%s ", log_level_to_name(level));

	if (print_time) {
		offset = strlen(buf);
		snprintf(buf + offset, BYTES - offset, "%s [%3lu] ", timestamp.c_str(),
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

	write_sync(buf, offset);

	va_end(va);
}

static FILE *get_file()
{
#ifdef DRWUTIL_LOG_ONLY_ON_STDERR
	return stderr;
#else
	static FILE *file = nullptr;
	static time::timestamp open_time = {0};
	time::timestamp now = time::get_timestamp();
	if (file == nullptr || open_time + time::seconds(3600 * 24) < now) {
		if (file) {
			fflush(file);
			if (file != stdout && file != stderr) {
				fclose(file);
			}
			file = nullptr;
		}
		constexpr uint32_t BYTES = 128;
		char file_name[BYTES] = {0};
		open_time = now;
		std::string timestamp_str = time::timestamp_to_string(now, 0).str;
		int written = snprintf(file_name, BYTES, "%s_%lu.log",
							   timestamp_str.c_str(), get_process_id());
		if (written > 64 || written <= 0) {
			printf("DUPA1 = %i\n", written);
			file = nullptr;
		} else {
			file = fopen(file_name, "w");
		}
		if (file == nullptr) {
			printf("DUPA2\n");
			file = stderr;
		}
	}
	return file;
#endif
}

#define DRWUTIL_MULTI_FILE_PRINT(func, ...)                                    \
	{                                                                          \
		if (output != stderr && output != stdout) {                            \
			func(__VA_ARGS__);                                                 \
		}                                                                      \
		FILE *tmp = output;                                                    \
		output = stderr;                                                       \
		func(__VA_ARGS__);                                                     \
		output = tmp;                                                          \
	}

void hex_dump(void *buf, int bytes)
{
	std::lock_guard lock(mutex);
	FILE *output = get_file();
	DRWUTIL_MULTI_FILE_PRINT(fprintf, output, "Hex dump from [%p] bytes %i\n",
							 buf, bytes);
	for (int i = 0; i < bytes; i += 16) {
		for (int j = 0; j < 16 && j + i < bytes; ++j) {
			char c = ((char *)buf)[i + j];
			if (c >= ' ' && c < 127) {
				DRWUTIL_MULTI_FILE_PRINT(fprintf, output, "  %c", c);
			} else {
				DRWUTIL_MULTI_FILE_PRINT(fprintf, output, "   ");
			}
		}
		DRWUTIL_MULTI_FILE_PRINT(fprintf, output, "\n");
		for (int j = 0; j < 16 && j + i < bytes; ++j) {
			DRWUTIL_MULTI_FILE_PRINT(
				fprintf, output, " %2.2X",
				(unsigned int)(((unsigned char *)buf)[i + j]));
		}
		DRWUTIL_MULTI_FILE_PRINT(fprintf, output, "\n");
	}
	DRWUTIL_MULTI_FILE_PRINT(fflush, output);
}

void print_line_sync(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	{
		std::lock_guard lock(mutex);
		FILE *output = get_file();
		DRWUTIL_MULTI_FILE_PRINT(vfprintf, output, fmt, va);
		DRWUTIL_MULTI_FILE_PRINT(fwrite, "\n", 1, 1, output);
		DRWUTIL_MULTI_FILE_PRINT(fflush, output);
	}
	va_end(va);
}

void write_sync(const char *buf, int bytes)
{
	std::lock_guard lock(mutex);
	FILE *output = get_file();
	DRWUTIL_MULTI_FILE_PRINT(fwrite, buf, 1, bytes, output);
	DRWUTIL_MULTI_FILE_PRINT(fflush, output);
}
#undef DRWUTIL_MULTI_FILE_PRINT

struct internal_logger {
	void init(const char *file = nullptr, bool to_stderr = true,
			  bool to_stdout = false, int64_t max_bytes = 32 * 1024 * 1024,
			  time::duration max_duration = time::hours(25),
			  bool reset_at_midnight = true);
	void deinit();

	std::mutex mutex;
	FILE *file = nullptr;
	
	std::string file_mame;
	bool to_stderr = true;
	bool to_stdout = false;
	int64_t max_bytes = 32*1024*1024;
	time::duration max_duration = time::hours(25);
	bool reset_at_midnight = true;
};

logger::logger(const char *file, bool to_stderr, bool to_stdout,
			   int64_t max_bytes, time::duration max_duration,
			   bool reset_at_midnight)
{
	init(file, to_stderr, to_stdout, max_bytes, max_duration,
		 reset_at_midnight);
}

logger::~logger()
{
	if (il) {
		delete il;
		il = nullptr;
	}
}

void logger::init(const char *file, bool to_stderr, bool to_stdout,
				  int64_t max_bytes, time::duration max_duration,
				  bool reset_at_midnight)
{
}

void logger::log(log_level level, bool print_time, bool print_file,
				 const char *file, int line, const char *function,
				 const char *fmt, ...)
{
	if (il == nullptr) {
		
	}
	
}

} // namespace log
} // namespace dutl
