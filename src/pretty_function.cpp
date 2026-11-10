// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include "../include/drwalin_util/pretty_function.hpp"

#include <shared_mutex>
#include <unordered_map>

namespace dutl
{
namespace log
{
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

inline const static std::string IGNORE_COMMON_PATH =
	DRWUTIL_LOG_IGNORE_COMMON_PATH;

std::string pretty_file_path(std::string path, bool ignore_path)
{
	if (ignore_path) {
		auto it = path.rfind("\\");
		if (it == std::string::npos) {
			it = 0;
		}
		auto it2 = path.rfind("/");
		if (it2 == std::string::npos) {
			it2 = 0;
		}
		if (it < it2) {
			it = it2;
		}
		if (it != 0) {
			return path.substr(it + 1);
		}
		return path;
	} else {
		auto pos = path.find(IGNORE_COMMON_PATH);
		if (pos == 0) {
			path.replace(pos, IGNORE_COMMON_PATH.size(), "");
		}
		while (true) {
			auto pos = path.find("\\");
			if (pos == std::string::npos) {
				break;
			}
			path.replace(pos, 1, "/");
		}
		while (true) {
			auto pos = path.find("//");
			if (pos == std::string::npos) {
				break;
			}
			path.replace(pos, 2, "/");
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
