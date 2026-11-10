// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_LOGGER_HPP
#define DRWUTIL_LOGGER_HPP

#include <cstdarg>

#include "time.hpp"

namespace std
{
class mutex;
}

namespace dutl
{
namespace log
{
enum log_level : unsigned int {
	FATAL = 1,
	LL_ERROR = 2,
	WARN = 3,
	INFO = 4,
	DEBUG = 5,
	TRACE = 6,

	LL_IGNORE = 127
};

struct logger;
struct logger_lock_guard {
	logger_lock_guard(logger *lg);
	~logger_lock_guard();

private:
	logger *lg;
	bool err = false, out = false, mtx = false;
};

struct log_rotate;
struct logger {
	logger();
	logger(const char *file, bool to_stderr = true, bool to_stdout = false,
		   int64_t max_bytes = 32 * 1024 * 1024,
		   time::duration max_duration = time::hours(25),
		   bool reset_at_midnight = true);
	~logger();

	void init(const char *file = nullptr, bool to_stderr = true,
			  bool to_stdout = false, int64_t max_bytes = 32 * 1024 * 1024,
			  time::duration max_duration = time::hours(25),
			  bool reset_at_midnight = true);
	void deinit();

	void log(log_level level, bool print_time, bool print_file,
			 const char *file, int line, const char *function, const char *fmt,
			 ...);

	int32_t vprintf_line_sync(const char *fmt, va_list va);
	int32_t printf_line_sync(const char *fmt, ...);
	void write_sync(const char *buf, int bytes);
	void hex_dump(void *buf, int bytes);

private:
	int32_t fprintf_not_sync(const char *fmt, ...);
	int32_t vfprintf_not_sync(const char *fmt, va_list va);
	int32_t vprintf_line_not_sync(const char *fmt, va_list va);
	int32_t printf_line_not_sync(const char *fmt, ...);
	void write_not_sync(const char *buf, int bytes);
	void flush_not_sync();

	friend class logger_lock_guard;

private:
	log_rotate *il = nullptr;
	std::mutex *mutex;

public:
	bool to_stdout = false;
	bool to_stderr = true;
};

extern logger default_logger;
} // namespace log
} // namespace dutl

#endif
