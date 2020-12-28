// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Thread.cpp
* @author JXMaster
* @date 2020/12/10
*/
#include "../PlatformDefines.hpp"
#define LUNA_RUNTIME_API LUNA_EXPORT
#include "OS.hpp"
#include "../Thread.hpp"

namespace Luna
{
	LUNA_RUNTIME_API void sleep(u32 time_milliseconds)
	{
		OS::sleep(time_milliseconds);
	}
	LUNA_RUNTIME_API void fast_sleep(u32 time_microseconds)
	{
		OS::fast_sleep(time_microseconds);
	}
	LUNA_RUNTIME_API void yield_current_thread()
	{
		OS::yield_current_thread();
	}
}