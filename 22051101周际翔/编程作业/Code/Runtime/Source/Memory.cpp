// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Memory.cpp
* @author JXMaster
* @date 2020/12/10
*/
#include "../PlatformDefines.hpp"

#define LUNA_RUNTIME_API LUNA_EXPORT

#include "../Memory.hpp"
#include "OS.hpp"
#include "../Atomic.hpp"

namespace Luna
{
	usize g_allocated_memory = 0;

	LUNA_RUNTIME_API void* memalloc(usize size, usize alignment)
	{
		void* mem = OS::memalloc(size, alignment);
		atom_add_usize(&g_allocated_memory, OS::memsize(mem, alignment));
		return mem;
	}
	LUNA_RUNTIME_API void memfree(void* ptr, usize alignment)
	{
		atom_add_usize(&g_allocated_memory, -(isize)OS::memsize(ptr, alignment));
		OS::memfree(ptr, alignment);
	}
	LUNA_RUNTIME_API void* memrealloc(void* ptr, usize size, usize alignment)
	{
		usize old_allocated = ptr ? OS::memsize(ptr, alignment) : 0;
		void* mem = OS::memrealloc(ptr, size, alignment);
		usize new_allocated = mem ? OS::memsize(mem, alignment) : 0;
		atom_add_usize(&g_allocated_memory, (isize)new_allocated - (isize)old_allocated);
		return mem;
	}
	LUNA_RUNTIME_API usize memsize(void* ptr, usize alignment)
	{
		return OS::memsize(ptr, alignment);
	}
	LUNA_RUNTIME_API usize get_allocated_memory()
	{
		return g_allocated_memory;
	}
}