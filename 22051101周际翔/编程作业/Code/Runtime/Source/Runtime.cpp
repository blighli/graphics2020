// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Runtime.cpp
* @author JXMaster
* @date 2020/12/10
*/
#pragma once
#include "../PlatformDefines.hpp"
#define LUNA_RUNTIME_API LUNA_EXPORT
#include "../Runtime.hpp"
#include "Name.hpp"
#include "Module.hpp"
#include "OS.hpp"
namespace Luna
{
	void time_init();

	LUNA_RUNTIME_API RV init()
	{
		OS::init();
		name_init();
		time_init();
		auto r = module_init();
		if (!r.valid())
		{
			name_close();
			return r;
		}
		return RV();
	}
	LUNA_RUNTIME_API void close()
	{
		module_close();
		name_close();
		OS::close();
	}
}