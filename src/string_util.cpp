// Copyright (C) 2024-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <string_view>
#include <vector>
#include <string>

#include "../include/drwalin_util/string_util.hpp"

namespace std
{
std::string_view trim(std::string_view _str,
					  const std::string_view whitespace)
{
	std::string_view str = _str;
	trim_inplace(str, whitespace);
	return str;
}

void trim_inplace(std::string_view &str, const std::string_view whitespace)
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string_view::npos) {
		str = "";
	} else {
		const auto strEnd = str.find_last_not_of(whitespace);
		str = std::string_view(str.data() + strBegin, strEnd - strBegin + 1);
	}
}

void trim_inplace(std::string &str, const std::string_view whitespace)
{
	std::string_view sv(str);
	trim_inplace(sv, whitespace);
	str.resize(sv.data() - str.data() + sv.size());
	str.erase(0, sv.data() - str.data());
}

std::vector<std::string_view> convert_to_args_list(std::string_view str)
{
	std::vector<std::string_view> args;
	while (str.size() > 0) {
		size_t offset = str.find_first_not_of(" \t\n\r\v");
		if (offset == std::string::npos) {
			break;
		}
		str = str.substr(offset);
		if (str[0] == '"') {
			size_t end = str.find_first_of("\"", 1);
			args.push_back(str.substr(1, end - 1));
			if (end == std::string::npos) {
				break;
			}
			str = str.substr(end + 1);
		} else if (str[0] == '\'') {
			size_t end = str.find_first_of("'", 1);
			args.push_back(str.substr(1, end - 1));
			if (end == std::string::npos) {
				break;
			}
			str = str.substr(end + 1);
		} else {
			size_t end = str.find_first_of(" \t\n\r\v");
			args.push_back(str.substr(0, end));
			if (end == std::string::npos) {
				break;
			}
			str = str.substr(end);
		}
	}
	return args;
}

std::string replace_all(const std::string &str, const std::string &src,
						const std::string &dst)
{
	if (src.size() == 0) {
		return str;
	}
	std::string_view sv{str.c_str(), str.size()};
	std::string ret;
	ret.reserve(str.size() * 2);
	while (sv.size() > 0) {
		size_t pos = sv.find(src);
		if (pos == std::string::npos) {
			ret.insert(ret.end(), sv.begin(), sv.end());
			break;
		} else {
			ret.insert(ret.end(), sv.begin(), sv.begin() + pos);
			pos += src.size();
			ret += dst;
		}
		sv = sv.substr(pos);
	}
	return ret;
}

std::string to_string(std::string_view sv)
{
	return std::string(sv.data(), sv.size());
}
} // namespace std
