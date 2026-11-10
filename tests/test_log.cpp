
#include <cstdlib>

#include "../include/drwalin_util/allocator.hpp"
#include "../include/drwalin_util/log.hpp"
#include "../include/drwalin_util/assert.hpp"
#include "../include/drwalin_util/random.hpp"

#define print_stats()                                                          \
	({                                                                         \
		auto &s = dutl::alloc::stats();                                        \
		int64_t al =                                                           \
			s.small_allocations + s.medium_allocations + s.large_allocations;  \
		LOG_INFO("allocated: %li   deallocated: %li    bytes: %li", al,        \
				 s.deallocations.load(), s.allocated_bytes.load());            \
	})

void sleep()
{
	static dutl::random_t r(123456);
	float s = r.normal(0, 1.0, 3);
	dutl::time::sleep(dutl::time::seconds(s * s));
}

int main()
{
	{
		static dutl::random_t r(1234560);
		uint64_t delta = 4354354lu;
		int64_t v = 0;
		for (int i = 0; i < 1000; ++i) {
			const dutl::time::timestamp a = {v};
			const auto str = dutl::time::timestamp_to_string(a, 9);
			const auto b = dutl::time::string_to_timestamp(str.str);
			if (a.ns < 0) {
				break;
			}
			if (a != b) {
				const auto bstr = dutl::time::timestamp_to_string(b, 9);
				LOG_FATAL("[%i] timestamp: %lu -> %s != %lu -> %s", i, a.ns,
						 str.str, b.ns, bstr.str);
				exit(1);
			}
			ASSERT_EXIT(a == b);
			if (str.str[0] != '1' && str.str[0] != '2') {
				break;
			}
			v += (r.randu() % delta);
			delta += (r.randu() % delta);
		}
	}

	{
		LOG_INFO("info");
		LOG_TRACE("trace");
		LOG_WARN("warn");
		LOG_ERROR("error");
		LOG_FATAL("fatal");

		print_stats();

		LOG_INFO("Changing to file logging with log rotate by size");
		dutl::log::default_logger.init("test_log", true, false, 300);
		LOG_INFO("Done: logging to a file with log rotate by size");

		for (int i = 0; i < 10; ++i) {
			auto a = dutl::time::now();
			auto b = dutl::time::now();
			auto c = dutl::time::now();
			LOG_INFO("dt %li ns", (b - a).ns);
			LOG_INFO("dt %li ns", (c - b).ns);
		}

		LOG_INFO("Changing to file logging with log rotate by time");
		dutl::log::default_logger.init("test_log", true, false, 300000,
									   dutl::time::seconds(1));
		LOG_INFO("Done: logging to a file with log rotate by time");

		for (int i = 0; i < 10; ++i) {
			auto a = dutl::time::now();
			auto b = dutl::time::now();
			auto c = dutl::time::now();
			LOG_INFO("dt %li ns", (b - a).ns);
			sleep();
			LOG_INFO("dt %li ns", (c - b).ns);
			sleep();
		}

		print_stats();
	}
	print_stats();

	return 0;
}
