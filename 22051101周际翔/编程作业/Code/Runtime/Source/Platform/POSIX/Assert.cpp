// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Assert.cpp
* @author JXMaster
* @date 2020/9/22
 */
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX

#define LUNA_RUNTIME_API LUNA_EXPORT
#include <Runtime/Assert.hpp>
#include <Runtime/Unicode.hpp>
#include <assert.h>

namespace Luna
{
    LUNA_RUNTIME_API void assert_fail(const c8* msg, const c8* file, u32 line)
    {
        printf ("%s:%d: failed assertion `%s'\n", file, line, msg);
        abort();
    }
}

#endif
