// Copyright (C) 2024-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <cassert>
#include <cstdlib>

#include "../include/drwalin_util/log.hpp"
#include "../include/drwalin_util/assert.hpp"

namespace dutl
{
bool assert_impl2(assert_strength strength, bool condition,
				  const char *condition_text, const char *file, int line,
				  const char *function)
{
	if (condition == true) {
		return true;
	}

	log::log_level level = log::STRONG_ASSERT;
	switch (strength) {
	case EXIT_ON_FAIL:
		level = log::STRONG_ASSERT;
		break;
	case CONDITION:
		level = log::DEBUG_ASSERT;
		break;
	case DEBUG_ONLY:
		level = log::DEBUG_ASSERT;
		break;
	default:
		LOG_FATAL("Undefined assert strength");
	}

	log::log(level, true, file, line, function, "Assert condition `%s` failed",
			 condition_text);

	assert(false);
	if (strength == EXIT_ON_FAIL) {
		exit(1);
	}
	return false;
}
} // namespace dutl
