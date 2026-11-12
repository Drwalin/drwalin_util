// Copyright (C) 2024-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <cassert>

#if DRWUTIL_USE_RPMALLOC
#include "../include/drwalin_util/log.hpp"
#include "../rpmalloc/rpmalloc/rpmalloc.h"
#else
#include <cstdlib>
#endif

#include "../include/drwalin_util/allocator.hpp"

namespace dutl
{
namespace alloc
{
static memory_stats _stats;

allocated_object<void> allocate(size_t bytes)
{
#if DRWUTIL_USE_RPMALLOC
	void *ptr = rpmalloc(bytes);

	assert((((uint64_t)ptr) & 0xF) == 0);

	assert(ptr);
	size_t _bytes = rpmalloc_usable_size(ptr);
	if (_bytes < bytes) {
		rpfree(ptr);
		LOG_FATAL("Allocated (%lu) memory is smaller than requested (%lu)",
				  _bytes, bytes);
		return {nullptr, 0};
	} else {
		bytes = _bytes;
	}

#ifdef DRWUTIL_MEMORY_COLLECT_STATS
	_stats.collect_single_allocation(_bytes);
#endif

	return {ptr, _bytes};
#else
	void *ptr = malloc(bytes);

#ifdef DRWUTIL_MEMORY_COLLECT_STATS
	_stats.collect_single_allocation(bytes);
#endif

	assert((((uint64_t)ptr) & 0xF) == 0);

	assert(ptr);
	return {ptr, bytes};
#endif
}

void release(void *ptr, size_t bytes)
{
#if DRWUTIL_USE_RPMALLOC
	bytes = rpmalloc_usable_size(ptr);

	rpfree(ptr);
#else
	free(ptr);
#endif
#ifdef DRWUTIL_MEMORY_COLLECT_STATS
	_stats.collect_single_deallocation(bytes);
#endif
}

memory_stats &stats() { return _stats; }
} // namespace alloc
} // namespace dutl
