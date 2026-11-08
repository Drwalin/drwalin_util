// Copyright (C) 2025-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef DRWUTIL_STATS_HPP
#define DRWUTIL_STATS_HPP

#include <cstdint>

#include <atomic>

namespace dutl
{
struct memory_stats {
	alignas(64) std::atomic<int64_t> small_allocations = 0;
	alignas(64) std::atomic<int64_t> medium_allocations = 0;
	alignas(64) std::atomic<int64_t> large_allocations = 0;
	alignas(64) std::atomic<int64_t> allocated_bytes = 0;
#if DRWUTIL_USE_RPMALLOC
	alignas(64) std::atomic<int64_t> small_deallocations = 0;
	alignas(64) std::atomic<int64_t> medium_deallocations = 0;
	alignas(64) std::atomic<int64_t> large_deallocations = 0;
	alignas(64) std::atomic<int64_t> deallocated_bytes = 0;
#else
	alignas(64) std::atomic<int64_t> deallocations = 0;
#endif

	void collect_single_allocation(size_t bytes);
	void collect_single_deallocation(size_t bytes);

	inline const static size_t MAX_BYTES_FOR_SMALL_ALLOCATIONS = 256;
	inline const static size_t MAX_BYTES_FOR_MEDIUM_ALLOCATIONS = 4096;
};
} // namespace dutl

#endif
