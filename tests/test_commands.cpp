
#include "../include/drwalin_util/config_storage.hpp"
#include "../include/drwalin_util/log.hpp"
#include "../include/drwalin_util/assert.hpp"
#include "../include/drwalin_util/command_parser.hpp"

dutl::config::storage config;
dutl::command_parser parser;

void set_bool(std::string key, bool value)
{
	char buf[1024];
	snprintf(buf, 1024, "bool %s %i", key.c_str(), value ? 1 : 0);
	parser.parse_single_command(buf);
}

void set_string(std::string key, std::string value)
{
	char buf[1024];
	snprintf(buf, 1024, "string %s %s", key.c_str(), value.c_str());
	parser.parse_single_command(buf);
}

void set_int(std::string key, int64_t value)
{
	char buf[1024];
	snprintf(buf, 1024, "int %s %li", key.c_str(), value);
	parser.parse_single_command(buf);
}

void set_float(std::string key, double value)
{
	char buf[1024];
	snprintf(buf, 1024, "float %s %f", key.c_str(), value);
	parser.parse_single_command(buf);
}

void set_vec(std::string key, std::vector<float> value)
{
	char buf[1025];
	int end = snprintf(buf, 1024, "vec %s", key.c_str());
	for (float v : value) {
		end += snprintf(buf + end, 1024 - end, " %f", v);
	}
	buf[end] = 0;
	parser.parse_single_command(buf);
}

#define TEST(METHOD_SET, METHOD_GET, TYPE, V1, V2)                             \
	{                                                                          \
		std::string key_base = #METHOD_SET #TYPE;                              \
		std::string key1 = key_base + std::to_string(++counter);               \
		std::string key2 = key_base + std::to_string(++counter);               \
		std::string key3 = key_base + std::to_string(++counter);               \
		TYPE v = V2;                                                           \
		ASSERT_EXIT(config.METHOD_GET(key1, &v) == false);                     \
		ASSERT_EXIT(config.METHOD_GET(key2, &v) == false);                     \
                                                                               \
		METHOD_SET(key1, V1);                                                  \
		METHOD_SET(key2, V2);                                                  \
                                                                               \
		v = V2;                                                                \
		ASSERT_EXIT(config.METHOD_GET(key1, &v) == true);                      \
		ASSERT_EXIT(v == V1);                                                  \
		v = V1;                                                                \
		ASSERT_EXIT(config.METHOD_GET(key2, &v) == true);                      \
		ASSERT_EXIT(v == V2);                                                  \
		ASSERT_EXIT(config.METHOD_GET(key3, &v) == false);                     \
	}

int main()
{
	config.initialise_commands(&parser);
	int counter = 0;

	TEST(set_string, get_string, std::string, "asdffdfs", "_fghj");
	TEST(set_bool, get_bool, bool, true, false);
	TEST(set_int, get_integer, int64_t, 13, 17);
	TEST(set_float, get_float, double, 123.125, 98765.375);
	TEST(set_float, get_float, double, 123.125, 98765.375);
	TEST(set_string, get_string, std::string, "asdf", "fghj");
	TEST(set_vec, get_vec, std::vector<float>, (std::vector<float>{1, 2, 3}),
		 (std::vector<float>{9, 8}));

	dutl::log::print_line_sync("Success");

	if (ASSERT_COND(!"this prooves asserts work")) {
		LOG_FATAL("Assert does not work");
		exit(1);
	}
}
