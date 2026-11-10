// Copyright (C) 2019-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include "../include/drwutil/shared_object.hpp"

#if defined __WIN32__ || defined _WIN64 || defined _WIN32
#include <windows.h>
#elif defined __unix__ || defined __linux__ || defined __FreeBSD__
#include <dlfcn.h>
#else
#error "Unknown or undefined operating system (not windows nor *nix)"
#endif

namespace dutl
{
shared_object::shared_object()
{
	handle = nullptr;
	error_message = "";
}

shared_object::~shared_object() { close(); }

#if defined __WIN32__ || defined _WIN64

bool shared_object::open(const std::string &file)
{
	Close();
	handle = LoadLibrary(file.c_str());
	const int msg_size = 4 * 1024;
	char msg[msg_size];
	DWORD err = GetLastError();
	if (err == 0) {
		error_message = "";
	} else {
		FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
			GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), msg,
			msg_size, NULL);
		error_message = msg;
	}
	file_path = file;
	return handle != nullptr;
}

void shared_object::close()
{
	if (handle) {
		FreeLibrary((HINSTANCE)handle);
		handle = nullptr;
	}
	error_message = "";
}

void *shared_object::get_void_symbol(const std::string &symbol)
{
	if (handle)
		return (void *)GetProcAddress((HINSTANCE)handle, symbol.c_str());
	return nullptr;
}

#elif defined __unix__

bool shared_object::open(const std::string &file)
{
	handle = dlopen(file.c_str(), RTLD_NOW);
	const char *err_str = dlerror();
	if (err_str) {
		error_message = err_str;
	} else {
		error_message = "";
	}
	file_path = file;
	return handle != nullptr;
}

void shared_object::close()
{
	if (handle) {
		dlclose(handle);
		handle = nullptr;
	}
	error_message = "";
}

void *shared_object::get_void_symbol(const std::string &symbol)
{
	if (handle) {
		return dlsym(handle, symbol.c_str());
	}
	return nullptr;
}

#endif
} // namespace dutl
