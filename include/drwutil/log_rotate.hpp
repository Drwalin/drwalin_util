// Copyright (C) 2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_LOG_ROTATE_HPP
#define DRWUTIL_LOG_ROTATE_HPP

#include <cstdint>
#include <cstdio>

#include <string>

#include "time.hpp"

namespace dutl
{
namespace log
{
class log_rotate
{
public:
	log_rotate();
	~log_rotate();

	void init(const char *base_file_name, int64_t max_bytes_in_single_file,
			  time::duration max_duration_of_open_file, bool reset_at_midnight);

	void write(const char *str, long long bytes);
	void flush();

	void rotate_file();
	void open_file();

public:
	std::string base_file_name;

	FILE *file = nullptr;

	time::point last_open = {0};
	int64_t file_size = 0;

	int64_t max_bytes_in_single_file = 0;
	time::duration max_duration_of_open_file = {0};
	bool reset_at_midnight = false;

	struct {
		int y = 0;
		int m = 0;
		int d = 0;
	};
};
} // namespace log
} // namespace dutl

#endif
