// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file PrimitiveType.hpp
* @author JXMaster
* @date 2020/12/26
*/
#pragma once
#include "../../TypeSystem.hpp"

namespace Luna
{
	struct VoidType : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("void"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 0; }
		virtual usize get_alignment() override { return 0; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_void; }
	};
	struct BoolType : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("bool"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 1; }
		virtual usize get_alignment() override { return 1; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_bool; }
	};
	struct U8Type : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("u8"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 1; }
		virtual usize get_alignment() override { return 1; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_u8; }
	};
	struct I8Type : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("i8"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 1; }
		virtual usize get_alignment() override { return 1; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_i8; }
	};
	struct U16Type : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("u16"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 2; }
		virtual usize get_alignment() override { return 2; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_u16; }
	};
	struct I16Type : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("i16"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 2; }
		virtual usize get_alignment() override { return 2; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_i16; }
	};
	struct U32Type : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("u32"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 4; }
		virtual usize get_alignment() override { return 4; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_u32; }
	};
	struct I32Type : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("I32"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 4; }
		virtual usize get_alignment() override { return 4; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_i32; }
	};
	struct U64Type : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("u64"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 8; }
		virtual usize get_alignment() override { return 8; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_u64; }
	};
	struct I64Type : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("i64"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 8; }
		virtual usize get_alignment() override { return 8; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_i64; }
	};
	struct F32Type : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("f32"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 4; }
		virtual usize get_alignment() override { return 4; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_f32; }
	};
	struct F64Type : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("f64"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 8; }
		virtual usize get_alignment() override { return 8; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_f64; }
	};
	struct USizeType : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("usize"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return sizeof(usize); }
		virtual usize get_alignment() override { return alignof(usize); }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_usize; }
	};
	struct ISizeType : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("isize"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return sizeof(isize); }
		virtual usize get_alignment() override { return alignof(isize); }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_isize; }
	};
	struct C8Type : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("c8"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 1; }
		virtual usize get_alignment() override { return 1; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_c8; }
	};
	struct C16Type : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("c16"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 2; }
		virtual usize get_alignment() override { return 2; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_c16; }
	};
	struct C32Type : public PrimitiveType
	{
		TypeDomain* m_domain;
		virtual Name get_name() override { return Name("c32"); }
		virtual TypeDomain* get_type_domain() override { return m_domain; }
		virtual TypeKind get_type_kind() override { return TypeKind::primitive; }
		virtual usize get_size() override { return 4; }
		virtual usize get_alignment() override { return 4; }
		virtual PrimitiveTypeKind get_primitive_type_kind() override { return PrimitiveTypeKind::kind_c32; }
	};
}