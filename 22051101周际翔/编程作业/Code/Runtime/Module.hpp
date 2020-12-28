// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Memory.hpp
* @author JXMaster
* @date 2020/8/26
* @brief Runtime module management APIs.
*/
#pragma once
#include "Base.hpp"
#include "Result.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	//! The function to be called when the module is initialized.
	using module_init_func_t = RV(void);

	//! The function to be called when the module is closed.
	using module_close_func_t = void(void);

	//! Module description structure. This shall be allocated on static memory and being kept valid during the application 
	//! lifetime.
	struct ModuleDesc
	{
		//! Used by the Runtime. The user should not change these memory.
		c8 reserved[32];

		// Filled by the user.
		
		//! The name of the module. The lifetime of the string should be equal to the lifetime of the module.
		const c8* name;
		
		//! The dependent modules of the module. The lifetime of the string array and strings in it
		//! should be equal to the lifetime of the module.
		//! 
		//! The string array shall end with an empty string ("") or with a null pointer (`nullptr`). Both representation
		//! is acceptable and equal.
		//! 
		//! Dependencies to `Runtime` should not be listed here. The Runtime is always initialized before any other
		//! module gets initialized.
		const c8* const* dependencies;

		//! The initialize function of the module.
		module_init_func_t* init_func;

		//! The close function of the module.
		module_close_func_t* close_func;
	};

	//! Adds one Engine module to the Runtime. This function can be called before `Luna::init` is called.
	//! 
	//! The added module will not be initialized immediately. If the module is added before the Runtime is initialized,
	//! they will be initialized when `Luna::init` is called; if the module is added after the Runtime is initialized,
	//! they must be initialized by one explicit call to `init_modules`. When `Luna::init` or `init_modules` are called,
	//! all dependent modules of the registered module must also be registered.
	//! 
	//! All modules will be closed by their dependency order when `Luna::close` is called. There is no way to close and remove 
	//! Engine modules on the fly, since the Engine modules are "unmanaged" and it makes the module implementation very complex to do so.
	//! You should restart the Engine if you make changes to the Engine module.
	//! 
	//! @param[in] module_desc The description structure of the module. This structure should be available during the module lifetime. 
	LUNA_RUNTIME_API void add_engine_module(ModuleDesc* module_desc);

	//! Removes one Engine module from the Runtime. This function must be called before `Luna::init` or after `Luna::close`.
	LUNA_RUNTIME_API void remove_engine_module(ModuleDesc* module_desc);

	//! Initializes all uninitialized modules.
	//! @return Returns `s_ok` if all modules is successfully initialized, returns error code if at least one module is failed to initialize.
	LUNA_RUNTIME_API RV init_modules();
}