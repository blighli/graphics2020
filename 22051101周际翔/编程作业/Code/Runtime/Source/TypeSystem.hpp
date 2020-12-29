// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file TypeSystem.hpp
* @author JXMaster
* @date 2020/12/26
*/
#pragma once
#include "../TypeSystem.hpp"
#include "../HashMap.hpp"
#include "../HashMultiMap.hpp"
namespace Luna
{
	class TypeDomainImpl : public TypeDomain
	{
		//! The context that owns this type domain.
		VMContext* m_context;
		// All types are stored in this map.
		HashMap<Name, Type*> m_types;
		// All global functions are stored in this map.
		// Multiple functions may have the same name but with different signature.
		HashMultiMap<Name, Function*> m_functions;
		// Class types are stored in this additional map.
		HashMap<Guid, ClassType*> m_class_types;
		// Interface types are stored in this additional map.
		HashMap<Guid, InterfaceType*> m_interface_types;

		virtual TypeDomain* clone() override;
		virtual VMContext* get_vmcontext() override
		{
			return m_context;
		}
		virtual RV load_assembly(Assembly* assembly) override;
		virtual Type* get_type(const Name& name) override;
		virtual ClassType* get_class_type(const Guid& guid) override;
		virtual InterfaceType* get_interface_type(const Guid& guid) override;
		virtual Vector<Function*> get_functions(const Name& name) override;
		virtual usize count_functions(const Name& name) override;
		virtual Vector<Name> get_all_types() override;
		virtual Vector<Name> get_all_functions() override;
	};
}