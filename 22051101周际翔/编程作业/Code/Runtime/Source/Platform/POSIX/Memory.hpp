// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Memory.hpp
* @author JXMaster
* @date 2020/9/22
 */
#pragma once
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX

#include <Runtime/IHeap.hpp>
#include <Runtime/Interface.hpp>

namespace Luna
{
    void memory_init();
    void memory_deinit();
}

#endif
