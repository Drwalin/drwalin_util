
#include "../include/drwalin_util/config_storage.hpp"
#include "../include/drwalin_util/log.hpp"
#include "../include/drwalin_util/assert.hpp"

#define TEST(METHOD_SET, METHOD_GET, TYPE, V1, V2)                             \
	{                                                                          \
		std::string key_base = #METHOD_SET #TYPE;                              \
		std::string key1 = key_base + std::to_string(++counter);               \
		std::string key2 = key_base + std::to_string(++counter);               \
		std::string key3 = key_base + std::to_string(++counter);               \
		storage.METHOD_SET(key1, V1);                                          \
		storage.METHOD_SET(key2, V2);                                          \
                                                                               \
		TYPE v = V2;                                                           \
		ASSERT_EXIT(storage.METHOD_GET(key1, &v) == true);                     \
		ASSERT_EXIT(v == V1);                                                  \
		v = V1;                                                                \
		ASSERT_EXIT(storage.METHOD_GET(key2, &v) == true);                     \
		ASSERT_EXIT(v == V2);                                                  \
		ASSERT_EXIT(storage.METHOD_GET(key3, &v) == false);                    \
	}

int main()
{
	int counter = 0;

	dutl::config::storage storage;
	storage.set_bool("flag1", true);
	storage.set_bool("flag2", false);

	bool vb = false;
	ASSERT_EXIT(storage.get_bool("flag1", &vb) == true);
	ASSERT_EXIT(vb == true);
	vb = true;
	ASSERT_EXIT(storage.get_bool("flag2", &vb) == true);
	ASSERT_EXIT(vb == false);
	vb = true;
	ASSERT_EXIT(storage.get_bool("flag3", &vb) == false);

	TEST(set_bool, get_bool, bool, true, false);
	TEST(set_integer, get_integer, int64_t, 13, 17);
	TEST(set_float, get_float, double, 123.4567, 98765.4321);
	TEST(set_string, get_string, std::string, "asdf", "fghj");
	TEST(set_vec, get_vec, std::vector<float>, (std::vector<float>{1, 2, 3}),
		 (std::vector<float>{9, 8}));

	dutl::log::print_line_sync("Success");

	if (ASSERT_COND(!"this prooves asserts work")) {
		LOG_FATAL("Assert does not work");
		exit(1);
	}
}
