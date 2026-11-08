// Copyright (C) 2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#define DRWUTIL_USE_RPMALLOC 1

#include "../include/drwutil/stats.hpp"

namespace dutl
{
void memory_stats::collect_single_allocation(size_t bytes)
{
	uint64_t total_allocated = allocated_bytes += bytes;
	if (bytes <= memory_stats::MAX_BYTES_FOR_SMALL_ALLOCATIONS) {
		small_allocations += 1;
	} else if (bytes <= memory_stats::MAX_BYTES_FOR_MEDIUM_ALLOCATIONS) {
		medium_allocations += 1;
	} else {
		large_allocations += 1;
	}
}

void memory_stats::collect_single_deallocation(size_t bytes)
{
#if DRWUTIL_USE_RPMALLOC
	deallocated_bytes += bytes;
	if (bytes <= memory_stats::MAX_BYTES_FOR_SMALL_ALLOCATIONS) {
		small_deallocations += 1;
	} else if (bytes <= memory_stats::MAX_BYTES_FOR_MEDIUM_ALLOCATIONS) {
		medium_deallocations += 1;
	} else {
		large_deallocations += 1;
	}
#else
	deallocations += 1;
#endif
}
}
