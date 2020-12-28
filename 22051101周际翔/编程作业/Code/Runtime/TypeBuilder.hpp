// Copyright 2018-2020 Luna Project. All rights reserved.
/*
* @file TypeBuilder.hpp
* @author JXMaster
* @date 2020/12/16
 */
#pragma once
#include "TypeSystem.hpp"

namespace Luna
{
	using class_delete_func_t = void(ClassType* type, Object* instance);

	struct ClassBuilder
	{
		//! Sets the guid of the class.
		virtual void set_guid(const Guid& guid) = 0;

		//! Sets the parent class of this class.
		//! @param[in] parent The name of the parent class.
		virtual void set_parent(const Name& parent) = 0;

		//! Sets the 

		//! Sets the deletion function.
		virtual void set_delete_func(class_delete_func_t* func) = 0;
	};

	struct InterfaceBuilder
	{
		//! Sets the guid of the interface.
		virtual void set_guid(const Guid& guid) = 0;

		//! Sets the parent interfaces of this interface.
		virtual void set_parents(const Vector<Name>& parents) = 0;
	};

	struct AssemblyBuilder
	{
		//! Sets the name of the assembly.
		virtual void set_name(const Name& name) = 0;

		//! Creates a new class in this assembly.
		//! @param[in] name The name of the class.
		virtual ClassBuilder* new_class(const Name& name) = 0;

		//! Creates a new interface in this assembly.
		//! @param[in] name The name of the interface.
		virtual InterfaceBuilder* new_interface(const Name& name) = 0;

		//! Builds the assembly. The user should delete the built assembly after it is no longer needed.
		virtual R<Assembly*> build() = 0;
	};

	//! Creates a new assembly builder.
	//! @param[in] name The name of the builder.
	LUNA_RUNTIME_API AssemblyBuilder* new_assembly_builder();

	//! Deletes the assembly builder.
	LUNA_RUNTIME_API void delete_assembly_builder(AssemblyBuilder* builder);
}