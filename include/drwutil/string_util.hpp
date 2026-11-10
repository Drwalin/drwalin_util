// Copyright (C) 2024-2026 Marek Zalewski aka Drwalin
//
// This file is part of Drwalin_util project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_STRING_UTIL_HPP
#define DRWUTIL_STRING_UTIL_HPP

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

inline std::string_view to_sv(const std::string &s) {
	return std::string(s.data(), s.size());
}

} // namespace std
#endif
