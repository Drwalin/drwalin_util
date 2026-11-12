// Copyright (C) 2024-2026 Marek Zalewski aka Drwalin
//
// This file is part of Drwalin_util project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_STRING_UTIL_HPP
#define DRWUTIL_STRING_UTIL_HPP

#include <cstdint>

#include <string_view>
#include <vector>
#include <string>

namespace std
{
std::string_view trim(std::string_view str,
					  const std::string_view whitespace = " \t\n\r\v");
void trim_inplace(std::string_view &str,
				  const std::string_view whitespace = " \t\n\r\v");
void trim_inplace(std::string &str,
				  const std::string_view whitespace = " \t\n\r\v");
std::vector<std::string_view> convert_to_args_list(std::string_view str);
std::string replace_all(const std::string &str, const std::string &src,
						const std::string &dst);

std::string to_string(std::string_view sv);
std::string_view to_sv(const std::string &s);

// return false if failed, modifies str to start after read value
bool read(string_view &str, int32_t &value);
bool read(string_view &str, int64_t &value);
bool read(string_view &str, uint32_t &value);
bool read(string_view &str, uint64_t &value);
bool read(string_view &str, float &value);
bool read(string_view &str, double &value);
bool read(string_view &str, std::string_view &value);

int32_t read_default_int32(string_view &str, int32_t default_value);
int64_t read_default_int64(string_view &str, int64_t default_value);
uint32_t read_default_uint32(string_view &str, uint32_t default_value);
uint64_t read_default_uint64(string_view &str, uint64_t default_value);
float read_default_float(string_view &str, float default_value);
double read_default_double(string_view &str, double default_value);
string_view read_default_string(string_view &str, string_view default_value);
} // namespace std
#endif
