// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include "../include/drwutil/log_rotate.hpp"

namespace dutl
{
namespace log
{
log_rotate::log_rotate() {}

log_rotate::~log_rotate()
{
	if (file) {
		fclose(file);
		file = nullptr;
	}
}

void log_rotate::init(const char *base_file_name,
					  int64_t max_bytes_in_single_file,
					  time::duration max_duration_of_open_file,
					  bool reset_at_midnight)
{
	if (file) {
		flush();
		fclose(file);
		file = nullptr;
	}
	this->base_file_name = base_file_name;
	this->max_bytes_in_single_file = max_bytes_in_single_file;
	this->max_duration_of_open_file = max_duration_of_open_file;
	this->reset_at_midnight = reset_at_midnight;
	rotate_file();
}

void log_rotate::write(const char *str, long long bytes)
{
	rotate_file();
	fwrite(str, 1, bytes, file);
	file_size += bytes;
}

void log_rotate::flush()
{
	if (file) {
		fflush(file);
	}
}

void log_rotate::rotate_file()
{
	if (file == nullptr) {
		open_file();
		return;
	}

	if (max_bytes_in_single_file != 0 &&
		max_bytes_in_single_file <= file_size) {
		open_file();
		return;
	}

	time::point now = time::now();
	if (max_duration_of_open_file != time::duration{0} &&
		(last_open + max_duration_of_open_file) <= now) {
		open_file();
		return;
	}

	if (reset_at_midnight) {
		time::timestamp ts = time::temporary_timestamp_to_timestamp(now);
		int Y, M, D;
		time::ymd_from_timestamp(ts, D, M, Y);
		if (D != d || M != m || Y != y) {
			open_file();
			return;
		}
	}
}

void log_rotate::open_file()
{
	if (file) {
		flush();
		fclose(file);
		file = nullptr;
	}
	last_open = time::now();
	file_size = 0;
	time::timestamp ts = time::temporary_timestamp_to_timestamp(last_open);
	time::ymd_from_timestamp(ts, d, m, y);

	for (int i=0; file == nullptr && i<100000; ++i) {
		std::string num = "";
		if (i) {
			num += ".";
			num += std::to_string(i);
		}
		std::string file_name =
			base_file_name + time::timestamp_to_string(ts, 0).str + ".log" + num;

		file = fopen(file_name.c_str(), "wxb");
	}
}
} // namespace log
} // namespace dutl
