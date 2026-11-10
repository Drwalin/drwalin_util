// Copyright (C) 2025-2026 Marek Zalewski aka Drwalin
//
// This file is part of DrwalinUtil project under MIT License
// You should have received a copy of the MIT License along with this program.

#include <cstdio>

#include <new>

#include "../include/drwutil/allocator.hpp"

extern void operator delete(void *p) noexcept { dutl::alloc::release(p, 0); }

extern void operator delete[](void *p) noexcept { dutl::alloc::release(p, 0); }

extern void *operator new(std::size_t size) noexcept(false)
{
	return dutl::alloc::allocate(size).object;
}

extern void *operator new[](std::size_t size) noexcept(false)
{
	return dutl::alloc::allocate(size).object;
}

extern void *operator new(std::size_t size, const std::nothrow_t &tag) noexcept
{
	(void)sizeof(tag);
	return dutl::alloc::allocate(size).object;
}

extern void *operator new[](std::size_t size,
							const std::nothrow_t &tag) noexcept
{
	(void)sizeof(tag);
	return dutl::alloc::allocate(size).object;
}

extern void operator delete(void *p, std::size_t size) noexcept
{
	(void)sizeof(size);
	dutl::alloc::release(p, 0);
}

extern void operator delete[](void *p, std::size_t size) noexcept
{
	(void)sizeof(size);
	dutl::alloc::release(p, 0);
}
