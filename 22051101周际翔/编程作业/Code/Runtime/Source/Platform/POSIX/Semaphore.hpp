
// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Semaphore.hpp
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
    class Semaphore : public ISemaphore
    {
    public:
        lucid("{dcce3ba3-e07c-4783-b4ee-d01ed55607c4}");
        luiimpl(Semaphore, ISemaphore, IWaitable, IObject);
    
        pthread_mutex_t m_mutex;
        pthread_cond_t m_cond;
        i32 m_counter;
        i32 m_max_count;
        
        Semaphore(i32 initial_count, i32 max_count)
        {
            m_max_count = max_count;
            m_counter = initial_count;
            luna_eval_and_assert_equal(pthread_mutex_init(&m_mutex, NULL), 0);
            luna_eval_and_assert_equal(pthread_cond_init(&m_cond, NULL), 0);
        }
        ~Semaphore()
        {
            pthread_cond_destroy(&m_cond);
            pthread_mutex_destroy(&m_mutex);
        }
        virtual RV wait() override
        {
            luna_eval_and_assert_equal(pthread_mutex_lock(&m_mutex), 0);
            if(m_counter > 0)
            {
                atom_dec_i32(&m_counter);
                luna_eval_and_assert_equal(pthread_mutex_unlock(&m_mutex), 0);
                return s_ok;
            }
            atom_dec_i32(&m_counter);
            luna_eval_and_assert_equal(pthread_cond_wait(&m_cond, &m_mutex), 0);
            atom_inc_i32(&m_counter);
            luna_eval_and_assert_equal(pthread_mutex_unlock(&m_mutex), 0);
            return s_ok;
        }
        virtual RV try_wait() override
        {
            luna_eval_and_assert_equal(pthread_mutex_lock(&m_mutex), 0);
            if(m_counter > 0)
            {
                atom_dec_i32(&m_counter);
                luna_eval_and_assert_equal(pthread_mutex_unlock(&m_mutex), 0);
                return s_ok;
            }
            luna_eval_and_assert_equal(pthread_mutex_unlock(&m_mutex), 0);
            return e_timeout;
        }
        virtual void unacquire() override
        {
            luna_eval_and_assert_equal(pthread_mutex_lock(&m_mutex), 0);
            if(m_counter < m_max_count)
            {
                if(m_counter < 0)
                {
                    luna_eval_and_assert_equal(pthread_cond_signal(&m_cond), 0);
                }
                else
                {
                    atom_inc_i32(&m_counter);
                }
            }
            luna_eval_and_assert_equal(pthread_mutex_unlock(&m_mutex), 0);
        }
    };
}

#endif
