// Copyright (C) 2024-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_ALLOCATOR_HPP
#define DRWUTIL_ALLOCATOR_HPP

#include <cstdio>

#include <utility>

#include "stats.hpp"

namespace dutl
{
template <typename T> struct allocated_object {
	T *object;
	size_t capacity;
};

namespace alloc
{
__attribute__((noinline)) allocated_object<void> allocate(size_t bytes);
__attribute__((noinline)) void release(void *ptr, size_t bytes);

template <typename T, typename... Args> T *allocate_typed(Args &&...args)
{
	allocated_object<void> v = allocate(sizeof(T));
	allocated_object<T> ret;
	ret.capacity = v.capacity;
	ret.object = new (v.object) T(std::move(args)...);
	return ret.object;
}

template <typename T> void release_typed(T *ptr, size_t bytes)
{
	ptr->~T();
	release(ptr, bytes);
}

memory_stats &stats();
} // namespace alloc
} // namespace dutl

#endif
