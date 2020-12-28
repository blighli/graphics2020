// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file TypeSystem.cpp
* @author JXMaster
* @date 2020/12/26
*/
#include "TypeSystem.hpp"

namespace Luna
{
	Type* TypeDomainImpl::get_type(const Name& name)
	{
		auto iter = m_types.find(name);
		if (iter == m_types.end())
		{
			return nullptr;
		}
		return iter->second;
	}
	ClassType* TypeDomainImpl::get_class_type(const Guid& guid)
	{
		auto iter = m_class_types.find(guid);
		if (iter == m_class_types.end())
		{
			return nullptr;
		}
		return iter->second;
	}
	InterfaceType* TypeDomainImpl::get_interface_type(const Guid& guid)
	{
		auto iter = m_interface_types.find(guid);
		if (iter == m_interface_types.end())
		{
			return nullptr;
		}
		return iter->second;
	}
	Vector<Function*> TypeDomainImpl::get_functions(const Name& name)
	{
		auto range = m_functions.equal_range(name);
		Vector<Function*> ret;
		for (auto iter = range.first; iter != range.second; ++iter)
		{
			ret.push_back(iter->second);
		}
		return ret;
	}
	usize TypeDomainImpl::count_functions(const Name& name)
	{
		return m_functions.count(name);
	}
	Vector<Name> TypeDomainImpl::get_all_types()
	{
		Vector<Name> r;
		r.reserve(m_types.size());
		for (auto& i : m_types)
		{
			r.push_back(i.first);
		}
		return r;
	}
	Vector<Name> TypeDomainImpl::get_all_functions()
	{
		Vector<Name> r;
		r.reserve(m_functions.size());
		for (auto& i : m_functions)
		{
			if (r.empty() || i.first != r.back())
			{
				r.push_back(i.first);
			}
		}
		return r;
	}
}