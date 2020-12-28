// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Thread.cpp
* @author JXMaster
* @date 2020/9/28
*/
#include "Thread.hpp"

#ifdef LUNA_PLATFORM_POSIX

#include <Runtime/Thread.hpp>
#include <Runtime/HashMap.hpp>

#include <unistd.h>

#include <Runtime/Time.hpp>

namespace Luna
{

    template <> struct hash<unsigned long>
    {
        usize operator()(unsigned long val) const { return static_cast<usize>(val); }
    };
    
    Unconstructed<MainThread> g_main_thread;
    Unconstructed<HashMap<usize, tls_free_func_t*>> g_free_func_map;
    P<IMutex> g_free_func_map_mtx;

    constexpr usize num_tls_slots = 512;

    struct ThreadLocalData
    {
        IPOSIXThread* m_thread;
        usize m_tls[num_tls_slots]; // Stores pointer as usize.
    };

    thread_local ThreadLocalData g_tls;

    u8 g_tls_alloc_table[num_tls_slots / 8];
    Unconstructed<HashMap<usize, usize*>> g_tls_maps;   // IPOSIXThread* -> tls.
    P<IMutex> g_tls_alloc_table_mtx;

    static void posix_uctx_main(u32 low, u32 high)
    {
        Fiber* f = (Fiber*)((usize)low | ((usize)high << 32));
        f->m_callback->run();
        f->m_state = EFiberState::finished;
        while (true)
        {
            auto _ = yield_current_fiber();
        }
    }

    static void* posix_thread_main(void* cookie)
    {
        UserThread* t = (UserThread*)cookie;
        g_tls.m_thread = (IPOSIXThread*)t;
        
        memzero(g_tls.m_tls, sizeof(g_tls.m_tls));
        
        // Add tls to maps.
        MutexGuard tlsg(g_tls_alloc_table_mtx);
        g_tls_maps.get().insert(Pair<usize, usize*>((usize)g_tls.m_thread, (usize*)g_tls.m_tls));
        tlsg.unlock();
        
        t->m_callback->run();
        
        t->m_signal->trigger();

        MutexGuard g(g_free_func_map_mtx.get());
        auto maps = g_free_func_map.get();
        for (auto& i : maps)
        {
            if (i.second)
            {
                i.second((handle_t)i.first);
            }
        }
        g.unlock();
        
        // Remvoe tls from maps.
        MutexGuard tlsg2(g_tls_alloc_table_mtx);
        g_tls_maps.get().erase((usize)g_tls.m_thread);
        tlsg2.unlock();
        
        return 0;
    }

    LUNA_RUNTIME_API RP<IThread> new_thread(IRunnable* callback, u32 stack_size)
    {
        P<UserThread> t = memnewref<UserThread>();
        t->m_callback = callback;
        if(stack_size == 0)
        {
            stack_size = 2_mb;
        }
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_getschedpolicy(&attr, &(t->m_sched_policy));
        pthread_attr_getschedparam(&attr, &(t->m_sched_param));
        
        pthread_attr_setstacksize(&attr, stack_size);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        int r = pthread_create(&(t->m_handle), &attr, &posix_thread_main, t.get());
        pthread_attr_destroy(&attr);
        if(r != 0)
        {
            return e_bad_system_call;
        }
        return P<IThread>(t);
    }

    void thread_init()
    {
        // Init free func map.
        g_free_func_map.construct();
        g_free_func_map_mtx = new_mutex();
        
        // Init main thread object.
        g_main_thread.construct();
        g_main_thread.get().m_handle = pthread_self();
        pthread_getschedparam(pthread_self(), &g_main_thread.get().m_sched_policy, &g_main_thread.get().m_sched_param);
        
        // Init tls alloc table & maps.
        g_tls_alloc_table_mtx = new_mutex();
        g_tls_maps.construct();
        memzero(g_tls_alloc_table, sizeof(g_tls_alloc_table));
        
        // Init tls block for main thread.
        g_tls.m_thread = (IPOSIXThread*)&g_main_thread.get();
        memzero(g_tls.m_tls, sizeof(g_tls.m_tls));
        g_tls_maps.get().insert(Pair<usize, usize*>((usize)g_tls.m_thread, (usize*)g_tls.m_tls));
    }
    void thread_close()
    {
        g_tls_maps.get().erase((usize)g_tls.m_thread);
        g_tls_maps.destruct();
        g_main_thread.destruct();
        g_free_func_map.destruct();
        g_free_func_map_mtx = nullptr;
        g_tls_alloc_table_mtx = nullptr;
    }
    LUNA_RUNTIME_API IThread* get_current_thread()
    {
        return g_tls.m_thread;
    }
    LUNA_RUNTIME_API IThread* get_main_thread()
    {
        return &(g_main_thread.get());
    }
    LUNA_RUNTIME_API void sleep(u32 time_milliseconds)
    {
        usleep(time_milliseconds * 1000);
    }
    LUNA_RUNTIME_API void fast_sleep(u32 time_microseconds)
    {
        u64 t = get_ticks();
        f64 tps = get_ticks_per_second();
        
        u64 endTime = t + (u64)(tps * time_microseconds / 1000000);
        if (time_microseconds > 4000) //4ms
        {
            usleep(time_microseconds);
        }
        t = get_ticks();
        while ((u64)(t) < endTime)
        {
            yield_current_thread();
            yield_current_thread();
            yield_current_thread();
            yield_current_thread();
        }
    }
    LUNA_RUNTIME_API void yield_current_thread()
    {
        sched_yield();
    }
    LUNA_RUNTIME_API RP<IFiber> convert_thread_to_fiber()
    {
        IPOSIXThread* t = g_tls.m_thread;
        if (t->get_native_fiber())
        {
            return { P<IFiber>(t->get_native_fiber()), s_already_done };
        }
        P<Fiber> f = memnewref<Fiber>();
        f->m_state = EFiberState::running;    // The default fiber running by default.
        int r = getcontext(&f->m_fiber);
        if(r != 0)
        {
            return e_bad_system_call;
        }
        f->m_is_native_fiber = true;
        t->set_native_fiber(f);
        t->set_fiber(f);
        return { f, s_ok };
    }
    LUNA_RUNTIME_API RV convert_fiber_to_thread()
    {
        IPOSIXThread* t = g_tls.m_thread;
        if (!t->get_native_fiber())
        {
            return e_not_supported;
        }
        if(t->get_native_fiber() != t->get_fiber())
        {
            return e_bad_calling_time;
        }
        t->set_fiber(nullptr);
        t->set_native_fiber(nullptr);
        return s_ok;
    }
    LUNA_RUNTIME_API RP<IFiber> new_fiber(IRunnable* callback, u32 stack_size)
    {
        luassert(callback);
        if (!stack_size)
        {
            stack_size = (u32)1_mb;
        }
        // Allocate stack.
        void* stack = memalloc(stack_size);
        P<Fiber> f = memnewref<Fiber>();
        f->m_stack = stack;
        makecontext(&(f->m_fiber),(void(*)(void))(&posix_uctx_main), 2, (u32)(usize)f.get(), (u32)((usize)(f.get()) >> 32));
        f->m_callback = callback;
        return P<IFiber>(f);
    }
    LUNA_RUNTIME_API IFiber* get_current_fiber()
    {
        IPOSIXThread* t = g_tls.m_thread;
        return t->get_fiber();
    }
    LUNA_RUNTIME_API RV resume_fiber(IFiber* fiber)
    {
        luassert(fiber);
        IPOSIXThread* t = (IPOSIXThread*)g_tls.m_thread;
        Fiber* old_f = t->get_fiber();
        if (!old_f)
        {
            return e_not_supported;
        }
        Fiber* new_f = static_cast<Fiber*>(fiber);
        if ((atom_compare_exchange_u32((u32*)&(new_f->m_state), (u32)EFiberState::running, (u32)EFiberState::ready) == (u32)EFiberState::ready) ||
            (atom_compare_exchange_u32((u32*)&(new_f->m_state), (u32)EFiberState::running, (u32)EFiberState::suspended) == (u32)EFiberState::suspended))
        {
            old_f->m_state = EFiberState::waiting;
            new_f->m_waiting_fiber = old_f;
            t->set_fiber(new_f);
            swapcontext(&(old_f->m_fiber), &(new_f->m_fiber));
            t->set_fiber(old_f);
            new_f->m_waiting_fiber = nullptr;
            if (new_f->m_state != EFiberState::finished)
            {
                new_f->m_state = EFiberState::suspended;
            }
            return s_ok;
        }
        return e_failure;
    }
    LUNA_RUNTIME_API RV yield_current_fiber()
    {
        IPOSIXThread* t = (IPOSIXThread*)g_tls.m_thread;
        Fiber* old_f = t->get_fiber();
        if (!old_f)
        {
            return e_not_supported;
        }
        Fiber* new_f = old_f->m_waiting_fiber;
        if (!new_f)
        {
            // The user calls this function when no fiber is resumed.
            return e_bad_calling_time;
        }
        swapcontext(&(old_f->m_fiber), &(new_f->m_fiber));
        return s_ok;
    }
    LUNA_RUNTIME_API R<handle_t> tls_alloc()
    {
        MutexGuard g(g_tls_alloc_table_mtx);
        for(usize i = 0; i < num_tls_slots; ++i)
        {
            if(!bit_test(g_tls_alloc_table, i))
            {
                bit_set(g_tls_alloc_table, i);
                
                MutexGuard g2(g_free_func_map_mtx.get());
                auto maps = g_free_func_map.get();
                maps.insert(Pair<usize, tls_free_func_t*>((usize)i, nullptr));
                
                //! Reset TLS data for all threads.
                for(auto& it : g_tls_maps.get())
                {
                    usize* tls = it.second;
                    tls[i] = 0;
                }
                
                return (handle_t)i;
            }
        }
        return e_out_of_range;
    }
    LUNA_RUNTIME_API RV tls_free(handle_t handle)
    {
        usize i = (usize)handle;
        MutexGuard g(g_tls_alloc_table_mtx);
        if(bit_test(g_tls_alloc_table, i))
        {
            bit_reset(g_tls_alloc_table, i);
            
            MutexGuard g(g_free_func_map_mtx.get());
            auto maps = g_free_func_map.get();
            maps.erase(i);
            
            return s_ok;
        }
        return e_bad_arguments;
    }
    LUNA_RUNTIME_API RV tls_set(handle_t handle, void* ptr)
    {
        usize i = (usize)handle;
        if(i >= num_tls_slots)
        {
            return e_out_of_range;
        }
        g_tls.m_tls[i] = (usize)ptr;
        return s_ok;
    }
    LUNA_RUNTIME_API RV tls_get(handle_t handle, void** ptr)
    {
        usize i = (usize)handle;
        if(i >= num_tls_slots)
        {
            return e_out_of_range;
        }
        *ptr = (void*)g_tls.m_tls[i];
        return s_ok;
    }
    LUNA_RUNTIME_API RV tls_set_free_func(handle_t handle, tls_free_func_t* free_func)
    {
        MutexGuard g(g_free_func_map_mtx.get());
        auto maps = g_free_func_map.get();
        auto iter = maps.find((usize)handle);
        if (iter == maps.end())
        {
            return e_bad_arguments;
        }
        iter->second = free_func;
        return s_ok;
    }
}

#endif
