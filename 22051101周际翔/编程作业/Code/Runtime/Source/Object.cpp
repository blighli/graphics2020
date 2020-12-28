// Copyright 2018-2020 Luna Project. All rights reserved.
/*
* @file Object.cpp
* @author JXMaster
* @date 2020/12/11
 */
#include <Runtime/PlatformDefines.hpp>
#define LUNA_RUNTIME_API LUNA_EXPORT
#include "../Object.hpp"
#include <Runtime/HashMap.hpp>
#include "OS.hpp"

namespace Luna
{
	//! Path to object map.
	HashMap<Path, Object*> g_path_to_object;

	//! Object to Path map.
	HashMap<Object*, Path> g_object_to_path;

	handle_t g_registry_mtx;

	void object_init()
	{
		g_registry_mtx = OS::new_mutex();
	}

	void object_close()
	{
		OS::delete_mutex(g_registry_mtx);
		g_registry_mtx = nullptr;
	}
}