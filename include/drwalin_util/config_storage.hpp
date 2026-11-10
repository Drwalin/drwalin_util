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

namespace dutl
{
class command_parser;

namespace config
{
enum entry_enum {
	EMPTY = 0,
	STRING = 2,
	SHORT_STRING = STRING | 1,
	VECTOR = 4,
	SHORT_VECTOR = VECTOR | 1,
	REAL = 8,
	INTEGER = 16,
	BOOLEAN = 32,
};

struct entry {
	struct short_string_t {
		uint8_t size = 0;
		char str[27] = {0};
	};
	struct string_t {
		uint32_t size = 0;
		uint32_t capacity = 0;
		char *str = nullptr;
	};

	struct short_vector_t {
		uint8_t size = 0;
		float vecs[7];
	};
	struct vector_t {
		uint32_t size = 0;
		float *vec = nullptr;
	};

	union {
		short_string_t short_string;
		string_t string;
		short_vector_t short_vector;
		vector_t vector;
		double real;
		int64_t integer;
		bool boolean;
	};
	entry_enum type = EMPTY;
};

class storage
{
public:
	storage();
	~storage();

	void initialise_commands(command_parser *commands_parser);

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
	void set_string(const std::string_view key, const char *value);
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
} // namespace config
} // namespace dutl
#endif
