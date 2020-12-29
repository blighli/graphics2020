// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Signal.hpp
* @author JXMaster
* @date 2020/9/22
*/
#pragma once
#include <Runtime/Sync.hpp>
#include <Runtime/Interface.hpp>
#include <Runtime/Assert.hpp>

#ifdef LUNA_PLATFORM_POSIX

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

namespace Luna
{
	class Signal : public ISignal
    {
    public:
        lucid("{c4805dc9-be98-48ae-b736-825273c5d23d}");
        luiimpl(Signal, ISignal, IWaitable, IObject);
        
        pthread_cond_t m_cond;
        pthread_mutex_t m_mutex;
        volatile bool m_signaled;
        volatile bool m_manual_reset;
        
        Signal(bool manual_reset)
        {
            m_signaled = false;
            m_manual_reset = manual_reset;
            luna_eval_and_assert_equal(pthread_mutex_init(&m_mutex, NULL), 0);
            luna_eval_and_assert_equal(pthread_cond_init(&m_cond, NULL), 0);
        }
        ~Signal()
        {
            pthread_cond_destroy(&m_cond);
            pthread_mutex_destroy(&m_mutex);
        }
        virtual RV wait() override
        {
            luna_eval_and_assert_equal(pthread_mutex_lock(&m_mutex), 0);
            while(!m_signaled)
            {
                luna_eval_and_assert_equal(pthread_cond_wait(&m_cond, &m_mutex), 0);
            }
            if(!m_manual_reset)
            {
                m_signaled = false;
            }
            luna_eval_and_assert_equal(pthread_mutex_unlock(&m_mutex), 0);
            return s_ok;
        }
        virtual RV try_wait() override
        {
            int rc = 0;
            struct timespec abstime;
            struct timeval tv;
            gettimeofday(&tv, NULL);
            abstime.tv_sec  = tv.tv_sec;
            abstime.tv_nsec = tv.tv_usec * 1000;
            if (abstime.tv_nsec >= 1000000000)
            {
                abstime.tv_nsec -= 1000000000;
                abstime.tv_sec++;
            }
            luna_eval_and_assert_equal(pthread_mutex_lock(&m_mutex), 0);
            while(!m_signaled)
            {
                rc = pthread_cond_timedwait(&m_cond, &m_mutex, &abstime);
                if(rc != 0)
                {
                    break;
                }
            }
            if(rc == 0 && !m_manual_reset)
            {
                m_signaled = false;
            }
            luna_eval_and_assert_equal(pthread_mutex_unlock(&m_mutex), 0);
            return rc == 0 ? s_ok : e_timeout;
        }
        virtual void trigger() override
        {
            luna_eval_and_assert_equal(pthread_mutex_lock(&m_mutex), 0);
            m_signaled = true;
            if(m_manual_reset)
            {
                luna_eval_and_assert_equal(pthread_cond_broadcast(&m_cond), 0);
            }
            else
            {
                luna_eval_and_assert_equal(pthread_cond_signal(&m_cond), 0);
            }
            luna_eval_and_assert_equal(pthread_mutex_unlock(&m_mutex), 0);
        }
        virtual void reset() override
        {
            luna_eval_and_assert_equal(pthread_mutex_lock(&m_mutex), 0);
            m_signaled = false;
            luna_eval_and_assert_equal(pthread_mutex_unlock(&m_mutex), 0);
        }
    };
}
#endif
