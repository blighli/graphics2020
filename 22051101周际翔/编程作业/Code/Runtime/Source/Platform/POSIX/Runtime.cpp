// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Runtime.cpp
* @author JXMaster
* @date 2020/9/29
 */
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX

#define LUNA_RUNTIME_API LUNA_EXPORT
#include <Runtime/Runtime.hpp>
#include "../../Name.hpp"
#include "Memory.hpp"
#include "Thread.hpp"
#include "File.hpp"
#include "../../Vfs.hpp"
#include "../../Random.hpp"
#include "../../UpdateLoop.hpp"
#include "../../Log.hpp"
#include "../../Dispatch.hpp"
#include "../../Module.hpp"

#include <sys/types.h>
#include <sys/sysctl.h>

namespace Luna
{
    void time_init();

    bool g_engine_initialized = false;

    LUNA_RUNTIME_API RV init()
    {
        memory_init();
        name_init();
        time_init();
        thread_init();
        file_init();
        vfs_init();
        random_init();
        update_loop_init();
        log_init();
        dispatch_init();
        auto r = module_init();
        if (failed(r))
        {
            dispatch_deinit();
            log_deinit();
            random_shutdown();
            vfs_deinit();
            thread_close();
            name_close();
            return r;
        }
        g_engine_initialized = true;
        return s_ok;
    }

    LUNA_RUNTIME_API void close()
    {
        g_engine_initialized = false;
        module_close();
        dispatch_deinit();
        log_deinit();
        random_shutdown();
        vfs_deinit();
        thread_close();
        name_close();
        memory_deinit();
    }

    LUNA_RUNTIME_API RV get_system_info(ESystemInfo system_info, void* data)
    {
        switch (system_info)
        {
        case ESystemInfo::processors_count:
        {
#ifdef LUNA_PLATFORM_MACOS
            {
                usize size;
                int name[2];
                size = 4;
                name[0] = CTL_HW;
                name[1] = HW_NCPU;
                int processor_count;
                if (sysctl(name, 2, &processor_count, &size, nullptr, 0) != 0)
                {
                    processor_count = 1;
                }
                *((u32*)data) = processor_count;
            }
#elif LUNA_PLATFORM_LINUX
            {
                int processor_count = max(sysconf(_SC_NPROCESSORS_ONLN), 1);
                *((u32*)data) = processor_count;
            }
#else
#error "get processor count not implemented."
#endif
        }
        default:
            break;
        }
        return e_bad_arguments;
    }
}

#endif
