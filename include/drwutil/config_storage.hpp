// Copyright (C) 2024-2026 Marek Zalewski aka Drwalin
//
// This file is part of Drwalin_util project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_CONFIG_STORAGE_HPP
#define DRWUTIL_CONFIG_STORAGE_HPP

#include <unordered_map>
#include <string>
#include <shared_mutex>
#include <vector>

class config_storage
{
public:
	config_storage();
	~config_storage();

	void initialise_commands(class command_parser *commands_parser);

	bool get_bool(const std::string_view key, bool *ret);
	bool get_string(const std::string_view key, std::string *ret);
	bool get_integer(const std::string_view key, int64_t *ret);
	bool get_float(const std::string_view key, double *ret);
	bool get_vec(const std::string_view key, std::vector<float> *vec);
	bool get_vec(const std::string_view key, float *vec, int n);

	bool get_or_set_bool(const std::string_view key, bool default_value);
	std::string get_or_set_string(const std::string_view key,
								  std::string_view default_value);
	int64_t get_or_set_integer(const std::string_view key,
							   int64_t default_value);
	double get_or_set_float(const std::string_view key, double default_value);
	std::vector<float> get_or_set_vec(const std::string_view key,
									  const std::vector<float> &default_value);
	void get_or_set_vec(const std::string_view key, const float *default_value,
						float *ret, int n);

	void set_bool(const std::string_view key, bool value);
	void set_string(const std::string_view key, const std::string &value);
	void set_string(const std::string_view key, const std::string_view value);
	void set_integer(const std::string_view key, int64_t value);
	void set_float(const std::string_view key, double value);
	bool set_vec(const std::string_view key, const std::vector<float> &value);
	bool set_vec(const std::string_view key, const float *value, int n);

	std::string all_configs_command();

public:
	std::unordered_map<std::string, bool> bools;
	std::unordered_map<std::string, std::string> strings;
	std::unordered_map<std::string, int64_t> ints;
	std::unordered_map<std::string, double> floats;
	std::unordered_map<std::string, std::vector<float>> vec;

	std::shared_mutex mb, ms, mi, mf, mv;
};
#endif
