// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <cstdarg>
#include <cstdio>

#include <mutex>

#include "../include/drwutil/time.hpp"
#include "../include/drwutil/log.hpp"
#include "../include/drwutil/log_rotate.hpp"
#include "../include/drwutil/logger.hpp"

namespace dutl
{
namespace log
{
static std::mutex mutex_global;
static std::mutex mutex_stderr;
static std::mutex mutex_stdout;

logger_lock_guard::logger_lock_guard(log::logger *lg) : lg(lg)
{
	if (lg->il) {
		mtx = true;
		lg->mutex->lock();
	}
	if (lg->to_stdout) {
		out = true;
		mutex_stdout.lock();
	}
	if (lg->to_stderr) {
		err = true;
		mutex_stderr.lock();
	}
}

logger_lock_guard::~logger_lock_guard()
{
	if (err) {
		mutex_stderr.unlock();
	}
	if (out) {
		mutex_stdout.unlock();
	}
	if (mtx) {
		lg->mutex->unlock();
	}
}

logger default_logger;

logger::logger()
{
	il = nullptr;
	mutex = new std::mutex;
}

logger::logger(const char *file, bool to_stderr, bool to_stdout,
			   int64_t max_bytes, time::duration max_duration,
			   bool reset_at_midnight)
	: logger()
{
	init(file, to_stderr, to_stdout, max_bytes, max_duration,
		 reset_at_midnight);
}

logger::~logger()
{
	std::lock_guard lock(mutex_global);
	if (il) {
		delete il;
		il = nullptr;
	}
	if (mutex) {
		delete mutex;
		mutex = nullptr;
	}
}

void logger::init(const char *file, bool to_stderr, bool to_stdout,
				  int64_t max_bytes, time::duration max_duration,
				  bool reset_at_midnight)
{
	this->to_stderr = to_stderr;
	this->to_stdout = to_stdout;
	if (file != nullptr && il == nullptr) {
		log_rotate *lr = new log_rotate;
		std::lock_guard lock(mutex_global);
		if (il == nullptr) {
			lr->init(file, max_bytes, max_duration, reset_at_midnight);
			il = lr;
			return;
		}
		delete lr;
	}
	std::lock_guard lock(*mutex);
	if (file != nullptr) {
		il->init(file, max_bytes, max_duration, reset_at_midnight);
	} else {
		std::lock_guard lock(mutex_global);
		if (il) {
			delete il;
			il = nullptr;
		}
	}
}

void logger::log(log_level level, bool print_time, bool print_file,
				 const char *file, int line, const char *function,
				 const char *fmt, ...)
{
	if (is_log_level_applicable(level) == false) {
		return;
	}

	constexpr int BYTES = 16 * 1024;
	char buf[BYTES];
	va_list va;
	va_start(va, fmt);
	int32_t offset = log_snprintf(buf, BYTES, level, print_time, print_file,
								  file, line, function, fmt, va);
	va_end(va);

	if (offset <= 0) {
		return;
	}

	write_sync(buf, offset);
}

int32_t logger::vprintf_line_sync(const char *fmt, va_list va)
{
	constexpr int BYTES = 16 * 1024;
	char buf[BYTES];
	int32_t offset = vsnprintf(buf, BYTES, fmt, va);
	offset += snprintf(buf + offset, BYTES - offset, "\n");
	write_sync(buf, offset);
	return offset;
}

int32_t logger::printf_line_sync(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int32_t ret = vprintf_line_sync(fmt, va);
	va_end(va);
	return ret;
}

void logger::write_sync(const char *buf, int bytes)
{
	logger_lock_guard lock(this);
	write_not_sync(buf, bytes);
	flush_not_sync();
}

int32_t logger::fprintf_not_sync(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int32_t ret = vfprintf_not_sync(fmt, va);
	va_end(va);
	return ret;
}

int32_t logger::vfprintf_not_sync(const char *fmt, va_list va)
{
	constexpr int BYTES = 16 * 1024;
	char buf[BYTES];
	int32_t offset = vsnprintf(buf, BYTES, fmt, va);
	write_not_sync(buf, offset);
	return offset;
}

int32_t logger::vprintf_line_not_sync(const char *fmt, va_list va)
{
	constexpr int BYTES = 16 * 1024;
	char buf[BYTES];
	int32_t offset = vsnprintf(buf, BYTES, fmt, va);
	offset += snprintf(buf + offset, BYTES - offset, "\n");
	write_not_sync(buf, offset);
	return offset;
}

int32_t logger::printf_line_not_sync(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int32_t ret = vprintf_line_not_sync(fmt, va);
	va_end(va);
	return ret;
}

void logger::write_not_sync(const char *buf, int bytes)
{
	if (il) {
		il->write(buf, bytes);
	}
	if (to_stdout) {
		fwrite(buf, 1, bytes, stdout);
	}
	if (to_stderr) {
		fwrite(buf, 1, bytes, stderr);
	}
}

void logger::flush_not_sync()
{
	if (il) {
		il->flush();
	}
	if (to_stdout) {
		fflush(stdout);
	}
	if (to_stderr) {
		fflush(stderr);
	}
}

void logger::hex_dump(void *buf, int bytes)
{
	logger_lock_guard lock(this);
	fprintf_not_sync("Hex dump from [%p] bytes %i\n", buf, bytes);
	for (int i = 0; i < bytes; i += 16) {
		for (int j = 0; j < 16 && j + i < bytes; ++j) {
			char c = ((char *)buf)[i + j];
			if (c >= ' ' && c < 127) {
				fprintf_not_sync("  %c", c);
			} else {
				fprintf_not_sync("   ");
			}
		}
		fprintf_not_sync("\n");
		for (int j = 0; j < 16 && j + i < bytes; ++j) {
			fprintf_not_sync(" %2.2X",
							 (unsigned int)(((unsigned char *)buf)[i + j]));
		}
		fprintf_not_sync("\n");
	}
	flush_not_sync();
}
} // namespace log
} // namespace dutl
