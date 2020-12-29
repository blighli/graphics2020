// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Mutex.hpp
* @author JXMaster
* @date 2020/9/22
*/
#pragma once
#include <Runtime/Sync.hpp>
#include <Runtime/Interface.hpp>
#include <Runtime/Assert.hpp>

#ifdef LUNA_PLATFORM_POSIX

#include <pthread.h>

namespace Luna
{
    class Mutex : public IMutex
    {
    public:
        lucid("{1e6c9de4-cf1b-4115-90bb-65869cb9624d}");
        luiimpl(Mutex, IMutex, IWaitable, IObject);
        
        pthread_mutex_t m_mutex;
        
        Mutex()
        {
            pthread_mutexattr_t        attr;
            luna_eval_and_assert_equal(pthread_mutexattr_init(&attr), 0);
            luna_eval_and_assert_equal(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE), 0);
            luna_eval_and_assert_equal(pthread_mutex_init(&m_mutex, &attr), 0);
            pthread_mutexattr_destroy(&attr);
        }
        ~Mutex()
        {
            pthread_mutex_destroy(&m_mutex);
        }
        virtual RV wait() override
        {
            luna_eval_and_assert_equal(pthread_mutex_lock(&m_mutex), 0);
            return s_ok;
        }
        virtual RV try_wait() override
        {
            int rv = pthread_mutex_trylock(&m_mutex);
            return (rv == 0) ? s_ok : e_timeout;
        }
        virtual void unlock() override
        {
            luna_eval_and_assert_equal(pthread_mutex_unlock(&m_mutex), 0);
        }
    };
}

#endif
