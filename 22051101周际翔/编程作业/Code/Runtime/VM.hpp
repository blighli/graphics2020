// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file VM.cpp
* @author JXMaster
* @date 2020/12/10
 */
#pragma once
#include "TypeSystem.hpp"
#include "Object.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif
namespace Luna
{

	//! Represents one context.
	struct VMContext
	{
		//! Fetches the type domain attached to this context.
		virtual TypeDomain* get_type_domain() = 0;

		//! Creates a new object in this context.
		template <typename _Ty, typename... _Args>
		_Ty* new_object(_Args&&... args)
		{
			// Allocate memory.
			_Ty* o = reinterpret_cast<_Ty*>(memalloc(sizeof(_Ty), alignof(_Ty)));
			if (o)
			{
				// Initialize the header.
				Object* obj = (Object*)o;
				obj->__object_rc = 1;
				obj->__object_type = get_type_domain()->get_class_type(_Ty::__guid);
				// Call ctor.
				new (o) _Ty(forward<_Args>(args)...);
				return o;
			}
			return nullptr;
		}

		//! Gets the root object of this context.


		//! Calls the specified function.
		//! @param[in] func The vm handle of the function.
		//! @param[in] args The stack frame to place the return value and arguments.
		//! @return Returns nothing if the function does not throw, returns error code if the function throws.
		virtual RV call(handle_t func, void* args) = 0;

		//! Calls the specified function with no exception.
		//! @param[in] func The vm handle of the function.
		//! @param[in] args The stack frame to place the return value and arguments.
		virtual void call_nothrow(handle_t func, void* args) = 0;
	};

	//! Creates a new context.
	//! One context owns one type domain, which can be fetched from `VMContext::get_type_domain()`.
	LUNA_RUNTIME_API VMContext* new_vmcontext();

	//! Deletes one context. 
	//! The context must be created by `new_vmcontext`, you cannot delete the main context.
	//! The user should delete all objects it this context before deleting the context.
	LUNA_RUNTIME_API void delete_vmcontext(VMContext* context);

	//! Fetches the main context.
	LUNA_RUNTIME_API VMContext* get_main_vmcontext();
}