// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <shared_mutex>
#include <unordered_map>

#include "../include/drwalin_util/pretty_function.hpp"

namespace dutl
{
namespace log
{
static std::string _clear_path(std::string path)
{
	int a = 0, b = 0;
	for (; a < path.size();) {
		char c = path[a];
		if (c == '\\') {
			c = '/';
		}
		path[b] = path[a];
		if (a != 0) {
			if (path[a - 1] == '/' && path[a] == '/') {
				++a;
			}
		}
		++a;
		++b;
	}
	path.resize(b);
	return path;
}

const std::string IGNORE_COMMON_PATH =
	_clear_path(DRWUTIL_LOG_IGNORE_COMMON_PATH "/");

const std::string &pretty_function(const char *ugly_function)
{
	std::string function = ugly_function;

	static std::shared_mutex mutex;
	static std::unordered_map<std::string, std::string> names;
	{
		std::shared_lock lock(mutex);
		auto it = names.find(function);
		if (it != names.end()) {
			return it->second;
		}
	}

	std::string funcName = function;

	for (int i = 0; i < funcName.size(); ++i) {
		if (funcName[i] == '[') {
			for (int j = i + 1; j < funcName.size(); ++j) {
				if (funcName[j] == '[') {
					break;
				}
				if (funcName[j] == ']') {
					funcName.erase(i, j + 1 - i);
					i = 0;
					break;
				}
			}
		}
	}

	for (;;) {
		auto p = funcName.find("static");

		if (p == std::string::npos) {
			p = funcName.find("virtual");
			if (p == std::string::npos) {
				p = funcName.find("const");
				if (p == std::string::npos) {
					break;
				} else {
					funcName.erase(p, 5);
				}
			} else {
				funcName.erase(p, 7);
			}
		} else {
			funcName.erase(p, 6);
		}

		if (p < funcName.size()) {
			if (funcName[p] == ' ') {
				funcName.erase(p, 1);
			}
		}

		if (p - 1 < funcName.size() && p - 1 > 0) {
			if (funcName[p - 1] == ' ') {
				funcName.erase(p - 1, 1);
			}
		}
	}

	for (int i = 0; i < funcName.size(); ++i) {
		if (funcName[i] == '(') {
			for (int j = i + 1; j < funcName.size(); ++j) {
				if (funcName[j] == '(') {
					break;
				}
				if (funcName[j] == ')') {
					if (j == i + 1) {
						break;
					}
					funcName.replace(i, j - i + 1, "()");
					i = 0;
					break;
				}
			}
		}
	}

	while (funcName.ends_with(" ")) {
		funcName.erase(funcName.size() - 1, 1);
	}

	if (funcName.ends_with("()") == false) {
		funcName += "()";
	}

	for (auto p = funcName.find("()()"); p != std::string::npos;
		 p = funcName.find("()()")) {
		funcName.erase(p, 2);
	}

	for (auto p = funcName.find("::()::"); p != std::string::npos;
		 p = funcName.find("::()::")) {
		funcName.erase(p, 4);
	}

	std::lock_guard lock(mutex);
	std::string &ret = names[function];
	ret = funcName;
	names[function] = funcName;
	return ret;
}

std::string pretty_file_path(std::string path, bool ignore_path)
{
	path = _clear_path(path);
	if (ignore_path) {
		auto it = path.rfind("/");
		if (it == std::string::npos) {
			it = 0;
		}
		if (it != 0) {
			return path.substr(it + 1);
		}
		return path;
	} else {
		auto pos = path.find(IGNORE_COMMON_PATH);
		// 		if (pos == 0) {
		// 			path.replace(pos, IGNORE_COMMON_PATH.size(), "");
		// 		}

		{
			int last_slash = 0;
			int i = 0;
			for (; i < path.size() && i < IGNORE_COMMON_PATH.size() &&
				   path[i] == IGNORE_COMMON_PATH[i];
				 ++i) {
				if (path[i] == '/' || path[i] == '\\') {
					last_slash = i;
				}
			}
			if (i > 0 && last_slash != 0) {
				path = path.substr(last_slash + 1);
			}
		}

		auto oldSize = path.size() + 1;
		while (oldSize != path.size()) {
			oldSize = path.size();
			const auto pos = path.find("./");
			if (pos != std::string::npos) {
				if (pos >= 0) {
					if ((pos > 0 && path[pos - 1] == '/') || pos == 0) {
						path.replace(pos, 2, "");
					}
				}
			}
		}

		oldSize = path.size() + 1;
		while (oldSize != path.size()) {
			oldSize = path.size();

			const auto pos = path.find("/../");
			if (pos != std::string::npos) {
				if (pos > 0) {
					auto pos2 = path.rfind('/', pos - 1);
					if (pos2 == std::string::npos) {
						pos2 = 0;
					}
					path.replace(pos2, pos + 3 - pos2, "");
				}
			}
		}

		if (pos == 0 && path.size() > 0) {
			if (path[0] == '/' || path[0] == '\\') {
				path.erase(path.begin());
			}
		}
		return path;
	}
}

} // namespace log
} // namespace dutl
