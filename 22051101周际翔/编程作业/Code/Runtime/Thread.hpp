// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Thread.hpp
* @author JXMaster
* @date 2020/12/10
*/
#pragma once
#include "Base.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	//! Suspends current thread for a specific period of time. The actual suspended time may be longer than required.
	//! @param[in] timeMillisecounds The time, in milliseconds, that this thread needs to suspend.
	LUNA_RUNTIME_API void sleep(u32 time_milliseconds);
	//! Delay the execution of this thread for a very shout time by yielding this thread several times.
	//! This is more accurate to `sleep` method and will not suspend current thread unless the specified time is larger than several milliseconds.
	//! @param[in] time_microseconds The time, in microseconds, that this thread needs to delay.
	LUNA_RUNTIME_API void fast_sleep(u32 time_microseconds);
	//! Yields the remain time slice of the current thread and let OS to schedule other threads.
	//! There is no way to resume a thread from user mode, since threads are scheduled by OS automatically.
	LUNA_RUNTIME_API void yield_current_thread();
}