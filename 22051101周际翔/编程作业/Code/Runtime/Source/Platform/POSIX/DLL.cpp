// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file DLL.cpp
* @author JXMaster
* @date 2020/9/22
*/
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX
#define LUNA_RUNTIME_API LUNA_EXPORT
#include <Runtime/DLL.hpp>
#include <Runtime/Error.hpp>
#include <dlfcn.h>

namespace Luna
{
	LUNA_RUNTIME_API R<handle_t> load_library(const c8* filename)
	{
        lucheck(filename);
        void* handle = dlopen((const char*)filename, 0);
        if(handle == nullptr)
        {
            const char* err = dlerror();
            get_error().set(e_bad_system_call, "dlopen failed: %s", err);
            return e_user_failure;
        }
        return handle;
	}
	LUNA_RUNTIME_API RV free_library(handle_t handle)
	{
        lucheck(handle);
        int r = dlclose(handle);
        if(r != 0)
        {
            const char* err = dlerror();
            get_error().set(e_bad_system_call, "dlclose failed: %s", err);
            return e_user_failure;
        }
        return s_ok;
	}
	LUNA_RUNTIME_API R<void*> get_proc_address(handle_t handle, const c8* symbol_name)
	{
        lucheck(handle && symbol_name);
        void* r = dlsym(handle, symbol_name);
        if(r == nullptr)
        {
            const char* err = dlerror();
            get_error().set(e_bad_system_call, "dlsym failed: %s", err);
            return e_user_failure;
        }
        return r;
	}
}

#endif
