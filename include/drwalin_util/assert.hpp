// Copyright (C) 2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_ASSERT_HPP
#define DRWUTIL_ASSERT_HPP

#define ASSERT_EXIT(_CONDITION_)                                                 \
	dutl::assert_impl(dutl::EXIT_ON_FAIL, (_CONDITION_), #_CONDITION_, __FILE__,   \
					  __LINE__, __PRETTY_FUNCTION__)
#define ASSERT_COND(_CONDITION_)                                                 \
	dutl::assert_impl(dutl::CONDITION, (_CONDITION_), #_CONDITION_, __FILE__,      \
					  __LINE__, __PRETTY_FUNCTION__)
#define ASSERT_DEBUG(_CONDITION_)                                                \
	dutl::assert_impl(dutl::DEBUG_ONLY, (_CONDITION_), #_CONDITION_, __FILE__,     \
					  __LINE__, __PRETTY_FUNCTION__)

namespace dutl
{
enum assert_strength {
	EXIT_ON_FAIL = 0,
	CONDITION = 1,
	DEBUG_ONLY = 2,
};
bool assert_impl2(assert_strength strength, bool condition,
				  const char *condition_text, const char *file, int line,
				  const char *function);
inline bool assert_impl(assert_strength strength, bool condition,
						const char *condition_text, const char *file, int line,
						const char *function)
{
	if (condition == true) {
		return true;
	}
	assert_impl2(strength, condition, condition_text, file, line, function);
	return false;
}
} // namespace dutl

#endif
