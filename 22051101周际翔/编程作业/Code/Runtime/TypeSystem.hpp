// Copyright 2018-2020 Luna Project. All rights reserved.
/*
* @file TypeSystem.hpp
* @author JXMaster
* @date 2020/12/13
* @brief The type system APIs for Luna Runtime.
 */
#pragma once
#include "Result.hpp"
#include "Name.hpp"
#include "Vector.hpp"
namespace Luna
{
	enum class TypeKind : u32
	{
		//! The following types are named types. Their type objects will have only one instance 
		//! for each type in each context.
		primitive = 0,
		concrete_aliasing = 1,
		concrete_class = 2,
		concrete_struct = 3,
		concrete_interface = 4,
		concrete_enum =5,
		//! The following types are named generic types. The generic types cannot be used directly, they serve as the
		//! templates for the real type and should be fully instantiated before using at runtime.
		generic_aliasing = 6,
		generic_class = 7,
		generic_struct = 8,
		generic_interface = 9,
		generic_enum = 10,
		//! The following types are unnamed types. Their type objects will be created on needed and
		//! are not unique.
		array = 11,
		tuple = 12,
		closure = 13,
		pointer = 14,
		strong_reference = 15,		// Strong reference of class or interface type
		weak_reference = 16,		// Weak reference of class or interface type.
		lvalue_reference = 17,		// Lvalue Reference of value type.
		rvalue_reference = 18,		// Rvalue Reference of value type.
	};

	struct TypeDomain;

	//! One type represents one kind of memory block that can contains data.
	struct Type
	{
		//! Gets one name that describes this type.
		//! The name is used for display only.
		virtual Name get_name() = 0;

		//! Gets the type domain that owns this type.
		virtual TypeDomain* get_type_domain() = 0;

		//! Gets the type kind.
		virtual TypeKind get_type_kind() = 0;

		//! Gets the size of this type.
		virtual usize get_size() = 0;

		//! Gets the alignment of this type.
		virtual usize get_alignment() = 0;
	};

	enum class PropertyFlag : u32
	{
		none = 0,
		//! This property is declared with `mut` keyword.
		mutating = 1,
	};

	//! Used to represent one property of structures, classes, function parameters and function return values.
	struct Property
	{
		//! The type of the property.
		Type* type;
		//! For struct, class, closure or tuple, this is the offset of this property from the beginning of the struct, 
		//! class, closure or tuple.
		//! For function parameters and return values, this is the offset of this property from the beginning of the frame stack.
		usize offset;
		//! The property flags.
		PropertyFlag flags;
	};

	enum class FunctionFlag : u32
	{
		none = 0,
		//! This method is declared as mutating.
		mutating = 1,
	};

	//! One function represents one callable procedure. It does not contain 
	//! any data, while the closure contains data.
	struct Function
	{
		//! Gets the parameter types.
		virtual Vector<Property> get_parameter_types() = 0;

		//! Gets the return value type.
		virtual Property get_return_value_type() = 0;

		//! Gets the native pointer of this function. This is used for native call only (C++ to C++).
		virtual handle_t get_native_pointer() = 0;

		//! Gets the vm handle of this function. This is used for VM call.
		virtual handle_t get_handle() = 0;

		//! Gets the function flags.
		virtual FunctionFlag get_flags() = 0;
	};

	enum class PrimitiveTypeKind : u32
	{
		kind_void = 0,
		kind_bool = 1,
		kind_u8 = 2,
		kind_i8 = 3,
		kind_u16 = 4,
		kind_i16 = 5,
		kind_u32 = 6,
		kind_i32 = 7,
		kind_u64 = 8,
		kind_i64 = 9,
		kind_f32 = 10,
		kind_f64 = 11,
		kind_usize = 12,
		kind_isize = 13,
		kind_c8 = 14,
		kind_c16 = 15,
		kind_c32 = 16
	};

	struct PrimitiveType : public Type
	{
		//! Gets the primitive kind of this type.
		virtual PrimitiveTypeKind get_primitive_type_kind() = 0;
	};

	struct Interface;
	struct InterfaceType;

	class Object;

	struct ClassType : public Type
	{
		//! Gets the guid of this class type.
		virtual Guid get_guid() = 0;

		//! Gets the parent class of this class.
		//! @return Returns the parent class of this class, or `nullptr` if this class does not
		//! have parent class.
		virtual ClassType* get_parent() = 0;

		//! Gets a list of all interfaces implemented by this class.
		virtual Vector<InterfaceType*> get_interfaces() = 0;

		//! Checks if this class can be converted to the specified class or interface.
		virtual bool convertible_to(const Guid& guid) = 0;

		//! Queries the specified interface from this class type.
		//! This call does not add reference count.
		//! @param[in] object The instance to query the interface from.
		//! @param[in] interface_type The interface type you want to query.
		//! @param[out] interface_instance The interface variable to hold the queried interface.
		//! @return Returns success if the interface is successfully queried, or BasicError::not_supported if the
		//! interface is not implemented by this class.
		virtual RV query_interface(Object* instance, const Guid& interface_guid, Interface* interface_instance) = 0;

		//! Destroys the instance of this type immediately.
		virtual void delete_instance(Object* instance) = 0;
	};

	struct StructType : public Type
	{
		//! Gets a list of all properties ordered by their offset in the structure.
		virtual Vector<Name> get_properties() = 0;

		//! Gets the specified property.
		virtual Property get_property(const Name& property_name) = 0;

		//! Gets a list of functions.
		virtual Vector<Name> get_functions() = 0;
	};

	struct InterfaceType : public Type
	{
		//! Gets the guid of this interface type.
		virtual Guid get_guid() = 0;

		//! Gets the parent interfaces.
		virtual Vector<InterfaceType*> get_parents() = 0;
	};

	struct EnumType : public Type
	{

	};

	struct ArrayType : public Type
	{

	};

	struct TupleType : public Type
	{

	};

	struct ClosureType : public Type
	{

	};

	struct PointerType : public Type
	{

	};

	struct GenericType : public Type
	{
		//! Gets the generic parameters.
		virtual Vector<Name> get_generic_parameters() = 0;
	};

	struct Assembly
	{
		//! Gets the name of this assembly.
		virtual Name get_name() = 0;

		//! Gets a list of all types in this assembly.
		virtual Vector<Name> get_all_types() = 0;

		//! Gets a list of all global functions in this assembly.
		virtual Vector<Name> get_all_functions() = 0;
	};

	//! Deletes one assembly object.
	LUNA_RUNTIME_API void delete_assembly(Assembly* assembly);

	struct VMContext;

	//! Represents one type domain.
	struct TypeDomain
	{
		//! Creates one copy of this type domain.
		virtual TypeDomain* clone() = 0;

		//! Gets the context that owns this type domain.
		//! Returns `nullptr` if this type domain is not bound to any context.
		virtual VMContext* get_vmcontext() = 0;

		//! Loads one assembly to this type domain.
		//! When one assembly is loaded to the domain, the type domain verifies all 
		//! information in the assembly and decides whether the assembly is suitable
		//! for loading. If the assembly is suitable for loading, the type domain extracts
		//! all metadata informations from the assembly and pours them into the context.
		//! The type domain does not refer he assembly object, so the assembly is safe for 
		//! deleting or being loaded to another domain after this operation.
		//! 
		//! The assembly loading is a unit operation, all information in one assembly will
		//! all be loaded, or no information is loaded if some error occurs.
		//! @param[in] assembly The assembly you want to load.
		virtual RV load_assembly(Assembly* assembly) = 0;

		//! Gets the specified type by its name.
		//! @return Returns `nullptr` if the type does not exist.
		virtual Type* get_type(const Name& name) = 0;

		//! Gets the specified class type by its guid.
		//! @return Returns `nullptr` if the type does not exist.
		virtual ClassType* get_class_type(const Guid& guid) = 0;

		//! Gets the specified interface type by its guid.
		//! @return Returns `nullptr` if the type does not exist.
		virtual InterfaceType* get_interface_type(const Guid& guid) = 0;

		//! Gets all overloaded functions with the specified name. This is only for global functions, for class/struct 
		//! methods, use `get_method` of the class/struct interface.
		virtual Vector<Function*> get_functions(const Name& name) = 0;

		//! Gets the number of functions with the specified name.
		virtual usize count_functions(const Name& name) = 0;

		//! Gets a list of all types in this type domain.
		virtual Vector<Name> get_all_types() = 0;

		//! Gets a list of all global functions in this type domain.
		//! Every function name will appear only once in the returned array, no matter how many functions 
		//! with the same name are registered.
		virtual Vector<Name> get_all_functions() = 0;
	};

	//! Creates one new type domain.
	//! The new type domain only contains the primitive type definition.
	LUNA_RUNTIME_API TypeDomain* new_type_domain();

	//! Deletes one type domain.
	//! The can only be called for the type domains created by `new_type_domain` or
	//! `clone_type_domain`, you cannot delete the main type domain.
	LUNA_RUNTIME_API void delete_type_domain(TypeDomain* type_domain);

	//! Gets the main type domain.
	//! The main type domain is created when the Luna Runtime is initialized and cannot
	//! be deleted by the user.
	LUNA_RUNTIME_API TypeDomain* get_main_type_domain();


}