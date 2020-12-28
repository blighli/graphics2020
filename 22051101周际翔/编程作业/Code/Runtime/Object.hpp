// Copyright 2018-2020 Luna Project. All rights reserved.
/*
* @file Object.hpp
* @author JXMaster
* @date 2020/11/25
 */
#pragma once
#include "Atomic.hpp"
#include "Path.hpp"
#include "Source/OS.hpp"
#include "TypeSystem.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	class Object;
	class SideTable
	{
		Object* m_object;
		i32 m_ref;
		i32 m_weak_ref;
		mutable volatile u32 m_lock;	// non-recursive spin lock.
		void lock() const
		{
			while (atom_exchange_u32(&m_lock, 1))
			{
				OS::yield_current_thread();
			}
		}
		void unlock() const
		{
			atom_exchange_u32(&m_lock, 0);
		}
	public:
		SideTable(Object* obj, i32 ref) :
			m_object(obj),
			m_ref(ref),
			m_weak_ref(1),
			m_lock(0) {}
		i32 get_ref() const
		{
			return m_ref;
		}
		i32 add_ref()
		{
			lock();
			i32 r = atom_inc_i32(&m_ref);
			unlock();
			return r;
		}
		i32 release()
		{
			bool should_delete = false;
			lock();
			auto ref = atom_dec_i32(&m_ref);
			if (!ref)
			{
				m_object = nullptr;
				if (!m_weak_ref)
				{
					should_delete = true;
				}
			}
			unlock();
			if (should_delete)
			{
				memdelete(this);
			}
			return ref;
		}
		i32 get_weak_ref() const
		{
			lock();
			auto ref = m_weak_ref;
			unlock();
			return ref;
		}
		i32 add_weak_ref()
		{
			lock();
			auto ref = atom_inc_i32(&m_weak_ref);
			unlock();
		}
		i32 release_weak()
		{
			bool should_delete = false;
			lock();
			auto ref = atom_dec_i32(&m_weak_ref);
			if (!ref && !m_ref)
			{
				should_delete = true;
			}
			unlock();
			if (should_delete)
			{
				memdelete(this);
			}
			return ref;
		}
		bool is_expired() const
		{
			return m_object ? false : true;
		}
		Object* lock_obj()
		{
			lock();
			if (!m_object)
			{
				unlock();
				return nullptr;
			}
			atom_inc_i32(&m_ref);
			unlock();
			return m_object;
		}
	};

	// The object root class.
	// All managed objects should inherit from this class.
	class Object
	{
	public:
		//! A pointer to the type object of this object.
		//! The type object stores the type metadata of this object.
		ClassType* __object_type;
		//! The reference counter.
		//! If the highest bit is 0, it stores the reference count number,
		//! if the highest bit is 1, it stores one pointer to the side table.
		usize __object_rc;

		bool __is_pointer_to_side_table() const
		{
#ifdef LUNA_PLATFORM_64BIT
			return __object_rc & 0x8000000000000000LL ? true : false;
#else
			return __object_rc & 0x80000000 ? true : false;
#endif
		}

		SideTable* __get_side_table_internal() const
		{
			return (SideTable*)(__object_rc << 2);
		}

		void __set_side_table(SideTable* side_table)
		{
#ifdef LUNA_PLATFORM_64BIT
			__object_rc = (((usize)side_table) >> 2 | 0x8000000000000000LL);
#else
			__object_rc = (((usize)side_table) >> 2 | 0x80000000);
#endif
		}

		//! Fetches the type object of this object.
		ClassType* __get_type() const
		{
			return __object_type;
		}
		//! Fetches the reference count number of this object.
		i32 __get_ref() const
		{
			return __is_pointer_to_side_table() ?
				__get_side_table_internal()->get_ref() : (i32)__object_rc;
		}
		//! Atomically increases the reference counter by one.
		//! @return Returns the reference counter value after this operation.
		i32 __add_ref()
		{
			return __is_pointer_to_side_table() ?
				__get_side_table_internal()->add_ref() :
				(i32)atom_inc_usize(&__object_rc);
		}
		//! Atomically decreases the reference counter by one.
		//! @return Returns the reference counter value after this operation.
		i32 __release()
		{
			i32 ref = __is_pointer_to_side_table() ?
				__get_side_table_internal()->release() :
				(i32)atom_dec_usize(&__object_rc);
			if (!ref)
			{
				__get_type()->delete_instance(this);
			}
			return ref;
		}
		i32 __get_weak_ref() const
		{
			return __is_pointer_to_side_table() ?
				__get_side_table_internal()->get_weak_ref() : 0;
		}
		i32 __add_weak_ref()
		{
			if (!__is_pointer_to_side_table())
			{
				while (true)
				{
					usize old_rc = __object_rc;
					usize new_rc = old_rc;
					SideTable* table = memnew<SideTable>(this, (i32)old_rc);
#ifdef LUNA_PLATFORM_64BIT
					new_rc = (((usize)table) >> 2 | 0x8000000000000000LL);
#else
					new_rc = (((usize)table) >> 2 | 0x80000000);
#endif
					usize replaced = atom_compare_exchange_usize(&__object_rc, new_rc, old_rc);
					if (replaced == old_rc)
					{
						break;
					}
					memdelete(table);
				}
				return __get_side_table_internal()->get_weak_ref();
			}
			else
			{
				return __get_side_table_internal()->add_weak_ref();
			}
		}
		SideTable* __get_side_table() const
		{
			return __is_pointer_to_side_table() ? __get_side_table_internal() : nullptr;	\
		}
	};
}