// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Sync.cpp
* @author JXMaster
* @date 2020/9/22
 */
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX

#define LUNA_RUNTIME_API LUNA_EXPORT

#include "Signal.hpp"
#include "Mutex.hpp"
#include "Semaphore.hpp"
#include <Runtime/Assert.hpp>
namespace Luna
{
	LUNA_RUNTIME_API P<ISignal> new_signal(bool manual_reset)
	{
		P<Signal> signal = memnewref<Signal>(manual_reset);
		return signal;
	}
	LUNA_RUNTIME_API P<IMutex> new_mutex()
	{
		P<Mutex> mtx = memnewref<Mutex>();
		return mtx;
	}
	LUNA_RUNTIME_API P<ISemaphore> new_semaphore(i32 initial_count, i32 max_count)
	{
		P<Semaphore> sema = memnewref<Semaphore>(initial_count, max_count);
		return sema;
	}
}

#endif
