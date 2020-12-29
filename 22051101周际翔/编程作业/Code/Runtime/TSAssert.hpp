// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file TSAssert.hpp
* @author JXMaster
* @date 2019/1/15
*/
#pragma once
#include "Assert.hpp"
#include "Atomic.hpp"
#include "Thread.hpp"

namespace Luna
{
	struct TSLock
	{
		IThread* m_owning_thread;

		TSLock() :
			m_owning_thread(nullptr) {}
	};

	struct TSGuard
	{
		IThread* m_last_thread;
		TSLock& m_lock;

		TSGuard(TSLock& lock) :
			m_lock(lock)
		{
			IThread* cur = get_current_thread();
			m_last_thread = (IThread*)atom_exchange_pointer((void* volatile*)(&(lock.m_owning_thread)), cur);
			luassert_msg_always(!m_last_thread || (m_last_thread == cur), "Data race detected!");
		}

		~TSGuard()
		{
			atom_exchange_pointer((void* volatile*)(&(m_lock.m_owning_thread)), m_last_thread);
		}
	};
}

#ifdef LUNA_ENABLE_THREAD_SAFE_ASSERTION

#define lutsassert_lock() Luna::TSLock m_tsassert_lock;

#define lutsassert() Luna::TSGuard _tsguard(m_tsassert_lock);

#else

#define lutsassert_lock() 

#define lutsassert() 

#endif