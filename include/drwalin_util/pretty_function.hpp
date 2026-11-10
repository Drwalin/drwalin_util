// Copyright (C) 2023-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_PRETTY_FUNCTION_HPP
#define DRWUTIL_PRETTY_FUNCTION_HPP

#include <string>

#ifdef DRWUTIL_LOG_USE_PRETTY_FUNCTION
#define DRWUTIL_PRETTY_FUNCTION() pretty_function(__PRETTY_FUNCTION__)
#else
#define DRWUTIL_PRETTY_FUNCTION() (std::string(__PRETTY_FUNCTION__) + "()")
#endif

#ifndef DRWUTIL_LOG_IGNORE_COMMON_PATH
#define DRWUTIL_LOG_IGNORE_COMMON_PATH ""
#endif
namespace dutl
{
namespace log
{
extern const std::string IGNORE_COMMON_PATH;

const std::string &pretty_function(const char *ugly_function);
std::string pretty_file_path(std::string path, bool ignore_path);
} // namespace log
} // namespace dutl

#endif
