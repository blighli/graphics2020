// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Fiber.hpp
* @author JXMaster
* @date 2020/9/28
*/
#pragma once
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX

#define _XOPEN_SOURCE 600

#include <Runtime/IFiber.hpp>
#include <Runtime/IRunnable.hpp>
#include <Runtime/Interface.hpp>

#include <ucontext.h>

namespace Luna
{
    class Fiber final : public IFiber
    {
    public:
        lucid("{5190ad04-0809-48be-a8bb-a285a89243d4}");
        luiimpl(Fiber, IFiber, IObject);
            
        ucontext_t m_fiber;
        void* m_stack;
        P<IRunnable> m_callback;
        P<Fiber> m_waiting_fiber;    // The fiber to switch to when yield is called.
        EFiberState m_state;
        bool m_is_native_fiber;    // true if this fiber is created using `convert_thread_to_fiber`


        Fiber() :
            m_is_native_fiber(false),
            m_stack(nullptr),
            m_state(EFiberState::ready) {}

        ~Fiber()
        {
            if(m_stack)
            {
                memfree(m_stack);
                m_stack = nullptr;
            }
        }

        virtual EFiberState state() override
        {
            return m_state;
        }
    };
}

#endif
