// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Thread.hpp
* @author JXMaster
* @date 2020/9/28
*/
#pragma once
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX
#include <Runtime/IThread.hpp>
#include "Fiber.hpp"
#include <Runtime/Sync.hpp>

#include <pthread.h>

namespace Luna
{
    struct IPOSIXThread : public IThread
    {
        lucid("{1b10f2a0-029d-4914-98a0-ecc7b0ca5fdb}");
        
        virtual pthread_t get_handle() = 0;
        virtual void set_handle(pthread_t handle) = 0;
        virtual Fiber* get_native_fiber() = 0;
        virtual void set_native_fiber(Fiber* handle) = 0;
        virtual Fiber* get_fiber() = 0;
        virtual void set_fiber(Fiber* handle) = 0;
    };

    class UserThread : public IPOSIXThread
    {
    public:
        lucid("{0070e0df-2dc2-4f04-9c06-eb718f03b987}");
        luiimpl(UserThread, IPOSIXThread, IThread, IWaitable, IObject);
            

        pthread_t m_handle;        // Thread handle.
        int m_sched_policy;
        sched_param m_sched_param;

        P<ISignal> m_signal;
        P<IRunnable> m_callback;
        P<Fiber> m_fiber;            // The fiber running on this thread.
        P<Fiber> m_native_fiber;    // The fiber converted from thread.

        UserThread() :
            m_handle(NULL),
            m_signal(new_signal(true)) {}

        ~UserThread()
        {
            if (m_handle)
            {
                // Waits the thread to exit before releasing the last reference.
                // The thread is detached automatically, so there is no need to join one thread.
                luassert_always(succeeded(wait()));
                m_handle = nullptr;
            }
        }

        virtual void set_priority(EThreadPriority priority) override
        {
            sched_param param = m_sched_param;
            switch (priority)
            {
            case EThreadPriority::low:
                param.sched_priority = (param.sched_priority + sched_get_priority_min(m_sched_policy)) >> 1;
                pthread_setschedparam(m_handle, m_sched_policy, &param);
                break;
            case EThreadPriority::normal:
                pthread_setschedparam(m_handle, m_sched_policy, &param);
                break;
            case EThreadPriority::high:
                param.sched_priority = (param.sched_priority + sched_get_priority_max(m_sched_policy)) >> 1;
                pthread_setschedparam(m_handle, m_sched_policy, &param);
                break;
            case EThreadPriority::critical:
                param.sched_priority = sched_get_priority_max(m_sched_policy);
                pthread_setschedparam(m_handle, m_sched_policy, &param);
                break;
            }
        }
        virtual RV wait() override
        {
            return m_signal->wait();
        }
        virtual RV try_wait() override
        {
            return m_signal->try_wait();
        }

        virtual pthread_t get_handle() override
        {
            return m_handle;
        }
        virtual void set_handle(pthread_t handle) override
        {
            m_handle = handle;
        }
        virtual Fiber* get_native_fiber() override
        {
            return m_native_fiber.get();
        }
        virtual void set_native_fiber(Fiber* handle) override
        {
            m_native_fiber = handle;
        }
        virtual Fiber* get_fiber() override
        {
            return m_fiber.get();
        }
        virtual void set_fiber(Fiber* handle) override
        {
            m_fiber = handle;
        }
    };

    class MainThread : public IPOSIXThread
    {
    public:
        lucid("{7927c958-efdb-44e0-b708-e02b764c85b8}");
        luiimpl_static(MainThread, IPOSIXThread, IThread, IWaitable, IObject);
            
        pthread_t m_handle;
        int m_sched_policy;
        sched_param m_sched_param;
        
        P<Fiber> m_fiber;            // The fiber running on this thread.
        P<Fiber> m_native_fiber;    // The fiber converted from thread.

        MainThread() :
            m_handle(NULL) {}

        virtual void set_priority(EThreadPriority priority) override
        {
            sched_param param = m_sched_param;
            switch (priority)
            {
            case EThreadPriority::low:
                param.sched_priority = (param.sched_priority + sched_get_priority_min(m_sched_policy)) >> 1;
                pthread_setschedparam(m_handle, m_sched_policy, &param);
                break;
            case EThreadPriority::normal:
                pthread_setschedparam(m_handle, m_sched_policy, &param);
                break;
            case EThreadPriority::high:
                param.sched_priority = (param.sched_priority + sched_get_priority_max(m_sched_policy)) >> 1;
                pthread_setschedparam(m_handle, m_sched_policy, &param);
                break;
            case EThreadPriority::critical:
                param.sched_priority = sched_get_priority_max(m_sched_policy);
                pthread_setschedparam(m_handle, m_sched_policy, &param);
                break;
            }
        }
        virtual RV wait() override
        {
            lupanic_msg_always("The main thread cannot be waited, since it never returns.");
            return e_not_supported;
        }
        virtual RV try_wait() override
        {
            // The main thread cannot be waited, because it will never return.
            return e_timeout;
        }

        virtual pthread_t get_handle() override
        {
            return m_handle;
        }
        virtual void set_handle(pthread_t handle) override
        {
            m_handle = handle;
        }
        virtual Fiber* get_native_fiber() override
        {
            return m_native_fiber.get();
        }
        virtual void set_native_fiber(Fiber* handle) override
        {
            m_native_fiber = handle;
        }
        virtual Fiber* get_fiber() override
        {
            return m_fiber.get();
        }
        virtual void set_fiber(Fiber* handle) override
        {
            m_fiber = handle;
        }
    };

    void thread_init();
    void thread_close();
}

#endif
