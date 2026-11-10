// Copyright (C) 2024-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <mutex>
#include <sstream>

#include "../include/drwalin_util/log.hpp"
#include "../include/drwalin_util/string_util.hpp"
#include "../include/drwalin_util/command_parser.hpp"
#include "../include/drwalin_util/config_storage.hpp"

config_storage::config_storage() {}

config_storage::~config_storage()
{
	std::string config_save_file_name;
	if (get_string("config.store_config.file_path", &config_save_file_name)) {
		FILE *file = fopen(config_save_file_name.c_str(), "wb");
		if (file) {
			std::string str = all_configs_command();
			fprintf(file, "%s\n", str.c_str());
			fclose(file);
		} else {
			LOG_ERROR("Failed to open file `%s` for writing",
					  config_save_file_name.c_str());
		}
	}
}

template <typename T>
static bool get_or_default(const std::unordered_map<std::string, T> &map,
						   const std::string_view key, T *ret)
{
	auto it = map.find(std::to_string(key));
	if (it == map.end()) {
		return false;
	}
	*ret = it->second;
	return true;
}

bool config_storage::get_bool(const std::string_view key, bool *ret)
{
	std::shared_lock lock(ms);
	return get_or_default<bool>(bools, key, ret);
}

bool config_storage::get_string(const std::string_view key, std::string *ret)
{
	std::shared_lock lock(ms);
	return get_or_default<std::string>(strings, key, ret);
}

bool config_storage::get_integer(const std::string_view key, int64_t *ret)
{
	std::shared_lock lock(mi);
	return get_or_default<int64_t>(ints, key, ret);
}

bool config_storage::get_float(const std::string_view key, double *ret)
{
	std::shared_lock lock(mf);
	return get_or_default<double>(floats, key, ret);
}

bool config_storage::get_vec(const std::string_view key,
							 std::vector<float> *ret)
{
	std::shared_lock lock(mv);
	return get_or_default<std::vector<float>>(vec, key, ret);
}

bool config_storage::get_vec(const std::string_view key, float *ret, int n)
{
	std::shared_lock lock(mv);
	auto it = vec.find(std::to_string(key));
	if (it == vec.end()) {
		return false;
	}
	if (n != it->second.size()) {
		return false;
	}
	const float *ptr = it->second.data();
	for (int i = 0; i < n; ++i) {
		ret[i] = ptr[i];
	}
	return true;
}

void config_storage::set_bool(const std::string_view key, bool value)
{
	std::lock_guard<std::shared_mutex> lock(mb);
	bools[std::to_string(key)] = value;
}

void config_storage::set_string(const std::string_view key,
								const std::string &value)
{
	std::lock_guard<std::shared_mutex> lock(ms);
	strings[std::to_string(key)] = value;
}

void config_storage::set_string(const std::string_view key,
								const std::string_view value)
{
	std::lock_guard<std::shared_mutex> lock(ms);
	strings[std::to_string(key)] = value;
}

void config_storage::set_integer(const std::string_view key, int64_t value)
{
	std::lock_guard<std::shared_mutex> lock(mi);
	ints[std::to_string(key)] = value;
}

void config_storage::set_float(const std::string_view key, double value)
{
	std::lock_guard<std::shared_mutex> lock(mf);
	floats[std::to_string(key)] = value;
}

bool config_storage::set_vec(const std::string_view key,
							 const std::vector<float> &value)
{
	return set_vec(key, value.data(), value.size());
}

bool config_storage::set_vec(const std::string_view key, const float *value,
							 int n)
{
	std::lock_guard<std::shared_mutex> lock(mv);
	std::vector<float> &v = vec[std::to_string(key)];
	if (v.size() == 0 || v.size() == n) {
		v.resize(n);
		if (n != 0) {
			memcpy(v.data(), value, n * sizeof(float));
		}
		return true;
	}
	return false;
}

bool config_storage::get_or_set_bool(const std::string_view key,
									 bool default_value)
{
	bool ret;
	if (get_bool(key, &ret)) {
		return ret;
	} else {
		set_bool(key, default_value);
		return default_value;
	}
}

std::string config_storage::get_or_set_string(const std::string_view key,
											  std::string_view default_value)
{
	std::string ret;
	if (get_string(key, &ret)) {
		return ret;
	} else {
		set_string(key, default_value);
		return std::to_string(default_value);
	}
}

int64_t config_storage::get_or_set_integer(const std::string_view key,
										   int64_t default_value)
{
	int64_t ret;
	if (get_integer(key, &ret)) {
		return ret;
	} else {
		set_integer(key, default_value);
		return default_value;
	}
}

double config_storage::get_or_set_float(const std::string_view key,
										double default_value)
{
	double ret;
	if (get_float(key, &ret)) {
		return ret;
	} else {
		set_float(key, default_value);
		return default_value;
	}
}

std::vector<float>
config_storage::get_or_set_vec(const std::string_view key,
							   const std::vector<float> &default_value)
{
	std::vector<float> ret;
	if (get_vec(key, &ret)) {
		return ret;
	} else {
		set_vec(key, default_value);
		return default_value;
	}
}

void config_storage::get_or_set_vec(const std::string_view key,
									const float *default_value, float *ret,
									int n)
{
	if (get_vec(key, ret, n)) {
		return;
	} else {
		set_vec(key, default_value, n);
		memcpy(ret, default_value, n * sizeof(float));
	}
}

std::string config_storage::all_configs_command()
{
	std::ostringstream ss;

	mb.lock();
	for (const auto &it : bools) {
		if (ss.tellp() > 0)
			ss << "\n";
		ss << "bool " << it.first << " " << (it.second ? 1 : 0);
	}
	mb.unlock();

	ms.lock();
	for (const auto &it : strings) {
		if (ss.tellp() > 0)
			ss << "\n";
		ss << "string " << it.first << " \"" << it.second << "\"";
	}
	ms.unlock();

	mi.lock();
	for (const auto &it : ints) {
		if (ss.tellp() > 0)
			ss << "\n";
		ss << "int " << it.first << " " << it.second;
	}
	mi.unlock();

	ss << std::fixed;

	mf.lock();
	for (const auto &it : floats) {
		if (ss.tellp() > 0)
			ss << "\n";
		ss << "float " << it.first << " " << it.second;
	}
	mf.unlock();

	mv.lock();
	for (const auto &it : vec) {
		if (ss.tellp() > 0)
			ss << "\n";
		ss << "vec " << it.first;
		for (float v : it.second) {
			ss << " " << v;
		}
	}
	mv.unlock();

	return ss.str();
}

void config_storage::initialise_commands(command_parser *com)
{
	com->register_custom_command(
		{"bool"},
		"Sets named config variable with boolean value (0 or 1)\n"
		"arguments:\n"
		"  - variable name\n"
		"  - new boolean value",
		[this](const std::vector<std::string_view> &args) {
			if (args.size() < 3) {
				LOG_ERROR("Not enaught arguments to set boolean");
				return;
			}
			set_bool(args[1], atoi(std::string(args[2]).c_str()));
		});

	com->register_custom_command(
		{"string"},
		"Sets named config variable with string value\n"
		"arguments:\n"
		"  - variable name\n"
		"  - new string value",
		[this](const std::string_view args) {
			std::string_view key, value;
			size_t p1 = args.find_first_of(" \n\r\t\v");
			size_t key_beg = args.find_first_not_of(" \n\r\t\v", p1);
			size_t key_end = args.find_first_of(" \n\r\t\v", key_beg);
			size_t value_beg = args.find_first_not_of(" \n\r\t\v", key_end);
			if (value_beg == std::string_view::npos) {
				if (key_end == std::string_view::npos) {
					LOG_ERROR("Not enaught arguments to sets");
					return;
				}
			} else {
				value = args.substr(value_beg);
			}
			key = args.substr(key_beg, key_end - key_beg);

			set_string(key, value);
		});

	com->register_custom_command(
		{"int"},
		"Sets named config variable with integer value\n"
		"arguments:\n"
		"  - variable name\n"
		"  - new integer value",
		[this](const std::vector<std::string_view> &args) {
			if (args.size() < 3) {
				LOG_ERROR("Not enaught arguments to set integer");
				return;
			}
			set_integer(args[1], std::stoull(std::string(args[2]).c_str()));
		});

	com->register_custom_command({"float"},
								 "Sets named config variable with float value\n"
								 "arguments:\n"
								 "  - variable name\n"
								 "  - new float value",
								 [this](const std::vector<std::string_view> &args) {
									 if (args.size() < 3) {
										 LOG_ERROR(
											 "Not enaught arguments to setf");
										 return;
									 }
			if (args.size() < 3) {
				LOG_ERROR("Not enaught arguments to set float");
				return;
			}
			set_float(args[1], std::stod(std::string(args[2]).c_str()));
								 });

	com->register_custom_command(
		{"vec"},
		"Sets named config variable with vec2 value\n"
		"arguments:\n"
		"  - variable name\n"
		"  - value\n"
		"  ... - more values",
		[this](const std::vector<std::string_view> &args) {
			if (args.size() < 4) {
				LOG_ERROR("Not enaught arguments to setv2");
				return;
			}
			set_vec2(args[1], {std::stof(args[2]), std::stof(args[3])});
		});

	com->register_custom_command(
		{"setv3"},
		"Sets named config variable with vec3 value\n"
		"arguments:\n"
		"  - variable name\n"
		"  - new X value\n"
		"  - new Y value\n"
		"  - new Z value",
		[this](const std::vector<std::string> &args) {
			if (args.size() < 4) {
				LOG_ERROR("Not enaught arguments to setv3");
				return;
			}
			set_vec3(args[1], {std::stof(args[2]), std::stof(args[3]),
							   std::stof(args[4])});
		});

	com->register_custom_command(
		{"setv4"},
		"Sets named config variable with vec4 value\n"
		"arguments:\n"
		"  - variable name\n"
		"  - new W value\n"
		"  - new X value\n"
		"  - new Y value\n"
		"  - new Z value",
		[this](const std::vector<std::string> &args) {
			if (args.size() < 4) {
				LOG_ERROR("Not enaught arguments to setv4");
				return;
			}
			set_vec4(args[1], {std::stof(args[2]), std::stof(args[3]),
							   std::stof(args[4]), std::stof(args[5])});
		});

	com->register_custom_command(
		{"print_all_variables", "print_all"}, "Prints all variables",
		[this](const std::vector<std::string> &args) {
			std::string str = all_configs_command();
			std::string ret = std::replace_all(str, "\n", "\n  ");
			printf("All stored variables:\n  %s\n", ret.c_str());
		});

	com->register_custom_command(
		{"save_variables_to_file"},
		"Saves all variables to a file\n"
		"arguments:\n"
		"  - file name to save variables",
		[this](const std::vector<std::string> &args) {
			if (args.size() < 2) {
				LOG_ERROR("Not enaught arguments to save_variables_to_file");
				return;
			}
			FILE *file = fopen(args[1].c_str(), "wb");
			if (file) {
				std::string str = all_configs_command();
				fprintf(file, "%s\n", str.c_str());
				fclose(file);
			} else {
				LOG_ERROR("Failed to open file `%s` for writing",
						  args[1].c_str());
			}
		});
}
