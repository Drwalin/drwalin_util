// Copyright (C) 2019-2026 Marek Zalewski aka Drwalin
//
// This file is part of Drwalin_util project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_SHARED_OBJECT_HPP
#define DRWUTIL_SHARED_OBJECT_HPP

#include <string>

namespace dutl
{
class shared_object final
{
public:
	shared_object();
	~shared_object();

	bool open(const std::string &file);
	void close();

	template <typename T> T get_symbol(const std::string &symbol)
	{
		return (T)get_void_symbol(symbol);
	}

	void *get_void_symbol(const std::string &symbol);

	inline bool is_valid() const { return handle != nullptr; }
	inline const std::string &get_error_message() const
	{
		return error_message;
	}
	inline const std::string &get_file_path() const { return file_path; }

public:
	void *handle = nullptr;
	std::string error_message;
	std::string file_path;
};
} // namespace dutl

#endif
