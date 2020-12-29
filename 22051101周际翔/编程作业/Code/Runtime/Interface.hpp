// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Interface.hpp
* @author JXMaster
* @date 2020/7/26
 */
#pragma once
#include "Base.hpp"
#include "Object.hpp"

namespace Luna
{
	//! The base classes for all interfaces.
	//! Every interface stores two pointers, the first pointer points to the object itself,
	//! and the second pointer points to the interface vtable, which is used to call the interface.
	//! methods.
	struct Interface
	{
		//! Pointers to the object that implements the interface.
		Object* self;

	};
}