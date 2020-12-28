// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Runtime.hpp
* @author JXMaster
* @date 2020/7/22
* @brief The root interface of Engine Runtime. All APIs listed here except `Luna::init` is available only after
* `Luna::init` and before `Luna::close`.
*/
#pragma once
#include "Result.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	//! Initializes and starts Luna Runtime.
	//! @return 
	//! * `s_ok` on success.
	//! * Platform-specific error code otherwise.
	LUNA_RUNTIME_API RV init();

	//! Closes Luna Runtime.
	LUNA_RUNTIME_API void close();
}