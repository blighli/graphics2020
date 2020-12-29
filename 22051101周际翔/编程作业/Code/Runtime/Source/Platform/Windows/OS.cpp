// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file OS.cpp
* @author JXMaster
* @date 2020/12/9
 */
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_WINDOWS
#include "../../OS.hpp"
#include "MiniWin.hpp"

namespace Luna
{
	namespace OS
	{
		void time_init();
		void file_init();

		void init()
		{
			time_init();
			file_init();
		}

		void close() {}

		u32 get_num_processors()
		{
			SYSTEM_INFO si;
			memzero(&si, sizeof(SYSTEM_INFO));
			::GetSystemInfo(&si);
			return si.dwNumberOfProcessors;
		}
	}
}

#endif