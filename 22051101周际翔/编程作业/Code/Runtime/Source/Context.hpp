// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Context.hpp
* @author JXMaster
* @date 2020/12/26
*/
#pragma once
#include "../VM.hpp"
#include "TypeSystem.hpp"

namespace Luna
{
	class VMContextImpl : public VMContext
	{
		TypeDomainImpl* m_type_domain;

		virtual TypeDomain* get_type_domain() override
		{
			return m_type_domain;
		}

		virtual RV call(handle_t func, void* args) override;
		virtual void call_nothrow(handle_t func, void* args) override;
	};
}