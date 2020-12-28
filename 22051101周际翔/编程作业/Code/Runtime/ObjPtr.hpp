// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file ObjPtr.hpp
* @author JXMaster
* @date 2020/12/15
* @brief This file defines smart pointers: O<T>, I<T>, ORef<T>, IRef<T>, OWeak<T>, IWeak<T>
 */
#pragma once
#include "Object.hpp"
#include "Interface.hpp"

namespace Luna
{
	template <typename _Ty> class I;

	//! Object Ownership smart pointer.
	template <typename _Ty>
	class O
	{
		static_assert(is_base_of_v<Object, _Ty>, "The template argument of O wrapper must inherit from `Object` class.");
		_Ty* m_ptr;
		void internal_addref() const
		{
			if (m_ptr)
			{
				m_ptr->__add_ref();
			}
		}
		void internal_clear()
		{
			if (m_ptr)
			{
				m_ptr->__release();
				m_ptr = nullptr;
			}
		}
		template <typename _Rty> friend class O;
	public:
		//! clear the object reference and make it points to `nullptr`.
		void clear()
		{
			internal_clear();
		}
		//! Test if this reference is empty.
		//! A reference is valid if it is not `nullptr`.
		bool empty() const
		{
			return m_ptr == nullptr;
		}
		//! Get the underlying raw pointer without adding reference count.
		_Ty* get() const
		{
			return m_ptr;
		}
		//! Releases the underlying pointer and attaches the specified pointer without increasing ref count.
		void attach(_Ty* ptr)
		{
			internal_clear();
			m_ptr = ptr;
		}
		//! Sets the underlying pointer to `nullptr` and returns the pointer.
		_Ty* detach()
		{
			_Ty* r = m_ptr;
			m_ptr = nullptr;
			return r;
		}
		//! Checks if the object this reference refers to can be converted to the specified class or interface.
		bool convertible_to(const Guid& id) const
		{
			if (m_ptr)
			{
				return m_ptr->__get_type()->convertible_to(id);
			}
			return false;
		}
		template <typename _Rty>
		bool convertible_to() const
		{
			return convertible_to(_Rty::__guid);
		}
		O() : m_ptr(nullptr) {}
		~O()
		{
			internal_clear();
		}
		// Ctor from another reference of the same type.
		O(const O& rhs) = delete;
		O(O&& rhs) : m_ptr(rhs.detach()) {}
		O& operator=(const O& rhs) = delete;
		O& operator=(O&& rhs)
		{
			internal_clear();
			m_ptr = rhs.detach();
			return *this;
		}
		// Ctor from another reference of a different type.
		template <typename _Rty>
		O(O<_Rty>&& rhs)
		{
			m_ptr = nullptr;
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs.detach();
			}
		}
		template <typename _Rty>
		O& operator=(O<_Rty>&& rhs)
		{
			internal_clear();
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs.detach();
			}
			return *this;
		}
		// Ctor from I<_Rty>.
		template <typename _Rty> O(I<_Rty>&& rhs);
		template <typename _Rty> O& operator=(I<_Rty>&& rhs);
		bool operator== (const O<_Ty>& rhs) const
		{
			return m_ptr == rhs.m_ptr;
		}
		bool operator!= (const O<_Ty>& rhs) const
		{
			return m_ptr != rhs.m_ptr;
		}
		bool operator< (const O<_Ty>& rhs) const
		{
			return (usize)m_ptr < (usize)(rhs.m_ptr);
		}
		_Ty* operator->() const
		{
			return m_ptr;
		}
		//! Implicitly convert to raw pointer. Same as get() method.
		//! This will not add ref count.
		operator _Ty* () const
		{
			return m_ptr;
		}
	};

	//! Interface Ownership smart pointer.
	template <typename _Ty>
	class I
	{
		static_assert(is_base_of_v<Interface, _Ty>, "The template argument of O wrapper must inherit from `Object` class.");
		_Ty m_i;
		void internal_addref() const
		{
			if (m_i.self)
			{
				m_i.self->__add_ref();
			}
		}
		void internal_clear()
		{
			if (m_i.self)
			{
				m_i.self->__release();
				m_i.self = nullptr;
				m_i.vtable = nullptr;
			}
		}
		template <typename _Rty> friend class I;
	public:
		//! Clears the reference and make it points to `nullptr`.
		void clear()
		{
			internal_clear();
		}
		//! Tests if this interface reference is empty.
		//! A interface reference is valid if its `self` property is not `nullptr`.
		bool empty() const
		{
			return m_i.self == nullptr;
		}
		//! Gets the underlying interface object.
		_Ty get() const
		{
			return m_i;
		}
		//! Releases the underlying interface reference and attaches the specified interface without increasing reference counter.
		void attach(const _Ty& i)
		{
			internal_clear();
			m_i = i;
		}
		//! Clears the underlying interface reference and returns the interface.
		_Ty detach()
		{
			_Ty r = m_i;
			m_i.self = nullptr;
			m_i.vtable = nullptr;
			return r;
		}
		//! Checks if this interface supports the specified type.
		bool convertible_to(const Guid& iid) const
		{
			if (m_i.self)
			{
				return m_i.self->__get_type()->convertible_to(iid);
			}
			return false;
		}
		template <typename _Rty>
		bool convertible_to() const
		{
			return convertible_to(_Rty::__guid);
		}
		I()
		{
			m_i.self = nullptr;
			m_i.vtable = nullptr;
		}
		~I()
		{
			internal_clear();
		}
		//! Ctor from another interface reference of the same type.
		I(const I& rhs) = delete;
		I(I&& rhs) :
			m_i(rhs.detach()) {}
		I& operator=(const I& rhs) = delete;
		I& operator=(I&& rhs)
		{
			internal_clear();
			m_i = rhs.detach();
			return *this;
		}
		// Ctor from another interface reference of a different type.
		template <typename _Rty>
		I(I<_Rty>&& rhs)
		{
			m_i.self = nullptr;
			m_i.vtable = nullptr;
			if (!rhs.empty())
			{
				auto r = rhs.m_i.self->__get_type()->query_interface(rhs.m_i.self, _Ty::__guid, &m_i);
				if (!empty())
				{
					rhs.detach();
				}
			}
		}
		template <typename _Rty>
		I& operator=(I<_Rty>&& rhs)
		{
			internal_clear();
			if (!rhs.empty())
			{
				auto r = rhs.m_i.self->__get_type()->query_interface(rhs.m_i.self, _Ty::__guid, &m_i);
				if (!empty())
				{
					rhs.detach();
				}
			}
			return *this;
		}
		// Ctor from O<_Rty>.
		template <typename _Rty>
		I(O<_Rty>&& rhs)
		{
			m_i.self = nullptr;
			m_i.vtable = nullptr;
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.m_i.self, _Ty::__guid, &m_i);
				if (!empty())
				{
					rhs.detach();
				}
			}
		}
		template <typename _Rty>
		I& operator=(O<_Rty>&& rhs)
		{
			internal_clear();
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.m_i.self, _Ty::__guid, &m_i);
				if (!empty())
				{
					rhs.detach();
				}
			}
			return *this;
		}
		bool operator== (const I<_Ty>& rhs) const
		{
			return m_i.self == rhs.m_i.self;
		}
		bool operator!= (const I<_Ty>& rhs) const
		{
			return m_i.self != rhs.m_i.self;
		}
		bool operator< (const I<_Ty>& rhs) const
		{
			return (usize)m_i.self < (usize)(rhs.m_i.self);
		}
		_Ty* operator->() const
		{
			return &m_i;
		}
		//! Implicitly convert to raw interface. Same as get() method.
		//! This will not add ref count.
		operator _Ty() const
		{
			return m_i;
		}
	};
	template <typename _Ty>
	template <typename _Rty>
	inline O<_Ty>::O(I<_Rty>&& rhs) :
		m_ptr(nullptr)
	{
		Object* obj = rhs.get().self;
		if (obj && obj->__get_type()->convertible_to(_Ty::__guid))
		{
			m_ptr = (_Ty*)obj;
			rhs.detach();
		}
	}
	template <typename _Ty>
	template <typename _Rty>
	inline O<_Ty>& O<_Ty>::operator=(I<_Rty>&& rhs)
	{
		internal_clear();
		Object* obj = rhs.get().self;
		if (obj && obj->__get_type()->convertible_to(_Ty::__guid))
		{
			m_ptr = (_Ty*)obj;
			rhs.detach();
		}
		return *this;
	}
	//! Object Strong Reference smart pointer.
	template <typename _Ty>
	class ORef
	{
		static_assert(is_base_of_v<Object, _Ty>, "The template argument of ORef wrapper must inherit from `Object` class.");
		_Ty* m_ptr;
		void internal_addref() const
		{
			if (m_ptr)
			{
				m_ptr->__add_ref();
			}
		}
		void internal_clear()
		{
			if (m_ptr)
			{
				m_ptr->__release();
				m_ptr = nullptr;
			}
		}
		template <typename _Rty> friend class ORef;
	public:
		//! clear the object reference and make it points to `nullptr`.
		void clear()
		{
			internal_clear();
		}
		//! Test if this reference is empty.
		//! A reference is valid if it is not `nullptr`.
		bool empty() const
		{
			return m_ptr == nullptr;
		}
		//! Get the underlying raw pointer without adding reference count.
		_Ty* get() const
		{
			return m_ptr;
		}
		//! Releases the underlying pointer and attaches the specified pointer without increasing ref count.
		void attach(_Ty* ptr)
		{
			internal_clear();
			m_ptr = ptr;
		}
		//! Sets the underlying pointer to `nullptr` and returns the pointer.
		_Ty* detach()
		{
			_Ty* r = m_ptr;
			m_ptr = nullptr;
			return r;
		}
		//! Checks if the object this reference refers to can be converted to the specified class or interface.
		bool convertible_to(const Guid& id) const
		{
			if (m_ptr)
			{
				return m_ptr->__get_type()->convertible_to(id);
			}
			return false;
		}
		template <typename _Rty>
		bool convertible_to() const
		{
			return convertible_to(_Rty::__guid);
		}
		ORef() : m_ptr(nullptr) {}
		~ORef()
		{
			internal_clear();
		}
		// Ctor from another reference of the same type.
		ORef(const ORef& rhs) : m_ptr(rhs.m_ptr)
		{
			internal_addref();
		}
		ORef(ORef&& rhs) : m_ptr(rhs.detach()) {}
		ORef& operator=(const ORef& rhs)
		{
			internal_clear();
			m_ptr = rhs.m_ptr;
			internal_addref();
			return *this;
		}
		ORef& operator=(ORef&& rhs)
		{
			internal_clear();
			m_ptr = rhs.detach();
			return *this;
		}
		// Ctor from another reference of a different type.
		template <typename _Rty>
		ORef(const ORef<_Rty>& rhs)
		{
			m_ptr = nullptr;
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs.m_ptr;
				internal_addref();
			}
		}
		template <typename _Rty>
		ORef(ORef<_Rty>&& rhs)
		{
			m_ptr = nullptr;
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs.detach();
			}
		}
		template <typename _Rty>
		ORef& operator=(const ORef<_Rty>& rhs)
		{
			internal_clear();
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs.m_ptr;
				internal_addref();
			}
			return *this;
		}
		template <typename _Rty>
		ORef& operator=(ORef<_Rty>&& rhs)
		{
			internal_clear();
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs.detach();
			}
			return *this;
		}
		// Ctor from raw pointer of the same type.
		ORef(const _Ty*& rhs) : m_ptr(rhs)
		{
			internal_addref();
		}
		ORef& operator=(const _Ty*& rhs)
		{
			internal_clear();
			m_ptr = rhs;
			internal_addref();
			return *this;
		}
		ORef(_Ty*&& rhs) : m_ptr(rhs)
		{
			internal_addref();
		}
		ORef& operator=(_Ty*&& rhs)
		{
			internal_clear();
			m_ptr = rhs;
			internal_addref();
			return *this;
		}
		// Ctor for O<_Ty>.
		ORef(const O<_Ty>& rhs) : m_ptr(rhs.get())
		{
			internal_addref();
		}
		ORef(O<_Ty>&& rhs) : m_ptr(rhs.detach()) {}
		ORef& operator=(const O<_Ty>& rhs)
		{
			internal_clear();
			m_ptr = rhs.get();
			internal_addref();
			return *this;
		}
		ORef& operator=(O<_Ty>&& rhs)
		{
			internal_clear();
			m_ptr = rhs.detach();
			return *this;
		}
		// Ctor for O<_Rty>.
		template <typename _Rty>
		ORef(const O<_Rty>& rhs)
		{
			m_ptr = nullptr;
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs.get();
				internal_addref();
			}
		}
		template <typename _Rty>
		ORef(O<_Rty>&& rhs)
		{
			m_ptr = nullptr;
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs.detach();
			}
		}
		template <typename _Rty>
		ORef& operator=(const O<_Rty>& rhs)
		{
			internal_clear();
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs.get();
				internal_addref();
			}
			return *this;
		}
		template <typename _Rty>
		ORef& operator=(O<_Rty>&& rhs)
		{
			internal_clear();
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs.detach();
			}
			return *this;
		}
		// Ctor from I<_Rty>
		template <typename _Rty>
		ORef(const I<_Rty>& rhs)
		{
			m_ptr = nullptr;
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs->self;
				internal_addref();
			}
		}
		template <typename _Rty>
		ORef(I<_Rty>&& rhs)
		{
			m_ptr = nullptr;
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs->self;
				rhs.detach();
			}
		}
		template <typename _Rty>
		ORef& operator=(const I<_Rty>& rhs)
		{
			internal_clear();
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs->self;
				internal_addref();
			}
			return *this;
		}
		template <typename _Rty>
		ORef& operator=(I<_Rty>&& rhs)
		{
			internal_clear();
			if (rhs.convertible_to<_Ty>())
			{
				m_ptr = (_Ty*)rhs->self;
				rhs.detach();
			}
			return *this;
		}
		bool operator== (const ORef<_Ty>& rhs) const
		{
			return m_ptr == rhs.m_ptr;
		}
		bool operator!= (const ORef<_Ty>& rhs) const
		{
			return m_ptr != rhs.m_ptr;
		}
		bool operator< (const ORef<_Ty>& rhs) const
		{
			return (usize)m_ptr < (usize)(rhs.m_ptr);
		}
		_Ty* operator->() const
		{
			return m_ptr;
		}
		//! Implicitly convert to raw pointer. Same as get() method.
		//! This will not add ref count.
		operator _Ty* () const
		{
			return m_ptr;
		}
	};

	//! Interface Strong Reference smart pointer.
	template <typename _Ty>
	class IRef
	{
		static_assert(is_base_of_v<Interface, _Ty>, "The template argument of O wrapper must inherit from `Object` class.");
		_Ty m_i;
		void internal_addref() const
		{
			if (m_i.self)
			{
				m_i.self->__add_ref();
			}
		}
		void internal_clear()
		{
			if (m_i.self)
			{
				m_i.self->__release();
				m_i.self = nullptr;
				m_i.vtable = nullptr;
			}
		}
		template <typename _Rty> friend class IRef;
	public:
		//! Clears the reference and make it points to `nullptr`.
		void clear()
		{
			internal_clear();
		}
		//! Tests if this interface reference is empty.
		//! A interface reference is valid if its `self` property is not `nullptr`.
		bool empty() const
		{
			return m_i.self == nullptr;
		}
		//! Gets the underlying interface object.
		_Ty get() const
		{
			return m_i;
		}
		//! Releases the underlying interface reference and attaches the specified interface without increasing reference counter.
		void attach(const _Ty& i)
		{
			internal_clear();
			m_i = i;
		}
		//! Clears the underlying interface reference and returns the interface.
		_Ty detach()
		{
			_Ty r = m_i;
			m_i.self = nullptr;
			m_i.vtable = nullptr;
			return r;
		}
		//! Checks if this interface supports the specified type.
		bool convertible_to(const Guid& iid) const
		{
			if (m_i.self)
			{
				return m_i.self->__get_type()->convertible_to(iid);
			}
			return false;
		}
		template <typename _Rty>
		bool convertible_to() const
		{
			return convertible_to(_Rty::__guid);
		}
		IRef()
		{
			m_i.self = nullptr;
			m_i.vtable = nullptr;
		}
		~IRef()
		{
			internal_clear();
		}
		//! Ctor from another interface reference of the same type.
		IRef(const IRef& rhs) :
			m_i(rhs.m_i)
		{
			internal_addref();
		}
		IRef(IRef&& rhs) :
			m_i(rhs.detach()) {}
		IRef& operator=(const IRef& rhs)
		{
			internal_clear();
			m_i = rhs.m_i;
			internal_addref();
			return *this;
		}
		IRef& operator=(IRef&& rhs)
		{
			internal_clear();
			m_i = rhs.detach();
			return *this;
		}
		// Ctor from another interface reference of a different type.
		template <typename _Rty>
		IRef(const IRef<_Rty>& rhs)
		{
			m_i.self = nullptr;
			m_i.vtable = nullptr;
			if (!rhs.empty())
			{
				auto r = rhs.m_i.self->__get_type()->query_interface(rhs.m_i.self, _Ty::__guid, &m_i);
				internal_addref();
			}
		}
		template <typename _Rty>
		IRef(IRef<_Rty>&& rhs)
		{
			m_i.self = nullptr;
			m_i.vtable = nullptr;
			if (!rhs.empty())
			{
				auto r = rhs.m_i.self->__get_type()->query_interface(rhs.m_i.self, _Ty::__guid, &m_i);
				if (!empty())
				{
					rhs.m_i.self = nullptr;
					rhs.m_i.vtable = nullptr;
				}
			}
		}
		template <typename _Rty>
		IRef& operator=(const IRef<_Rty>& rhs)
		{
			internal_clear();
			if (!rhs.empty())
			{
				auto r = rhs.m_i.self->__get_type()->query_interface(rhs.m_i.self, _Ty::__guid, &m_i);
				internal_addref();
			}
			return *this;
		}
		template <typename _Rty>
		IRef& operator=(IRef<_Rty>&& rhs)
		{
			internal_clear();
			if (!rhs.empty())
			{
				auto r = rhs.m_i.self->__get_type()->query_interface(rhs.m_i.self, _Ty::__guid, &m_i);
				if (!empty())
				{
					rhs.m_i.self = nullptr;
					rhs.m_i.vtable = nullptr;
				}
			}
			return *this;
		}
		// Ctor from raw interface of the same type.
		IRef(const _Ty& rhs) : m_i(rhs)
		{
			internal_addref();
		}
		IRef& operator=(const _Ty& rhs)
		{
			internal_clear();
			m_i = rhs;
			internal_addref();
			return *this;
		}
		IRef(_Ty&& rhs) : m_i(rhs)
		{
			internal_addref();
		}
		IRef& operator=(_Ty&& rhs)
		{
			internal_clear();
			m_i = rhs;
			internal_addref();
			return *this;
		}
		// Ctor from O<_Rty>
		template <typename _Rty>
		IRef(const O<_Rty>& rhs)
		{
			m_i.self = nullptr;
			m_i.vtable = nullptr;
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.get(), _Ty::__guid, &m_i);
				internal_addref();
			}
		}
		template <typename _Rty>
		IRef(O<_Rty>&& rhs)
		{
			m_i.self = nullptr;
			m_i.vtable = nullptr;
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.get(), _Ty::__guid, &m_i);
				if (!empty())
				{
					rhs.detach();
				}
			}
		}
		template <typename _Rty>
		IRef& operator=(const O<_Rty>& rhs)
		{
			internal_clear();
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.get(), _Ty::__guid, &m_i);
				internal_addref();
			}
			return *this;
		}
		template <typename _Rty>
		IRef& operator=(O<_Rty>&& rhs)
		{
			internal_clear();
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.get(), _Ty::__guid, &m_i);
				if (!empty())
				{
					rhs.detach();
				}
			}
			return *this;
		}
		// Ctor from I<_Ty>
		IRef(const I<_Ty>& rhs) : 
			m_i(rhs.get())
		{
			internal_addref();
		}
		IRef(I<_Ty>&& rhs) : 
			m_i(rhs.detach) {}
		IRef& operator=(const I<_Ty>& rhs)
		{
			internal_clear();
			m_i = rhs.get();
			internal_addref();
			return *this;
		}
		IRef& operator=(I<_Ty>&& rhs)
		{
			internal_clear();
			m_i = rhs.detach();
			return *this;
		}
		// Ctor from I<_Rty>
		template <typename _Rty>
		IRef(const I<_Rty>& rhs)
		{
			m_i.self = nullptr;
			m_i.vtable = nullptr;
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.get(), _Ty::__guid, &m_i);
				internal_addref();
			}
		}
		template <typename _Rty>
		IRef(I<_Rty>&& rhs)
		{
			m_i.self = nullptr;
			m_i.vtable = nullptr;
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.get(), _Ty::__guid, &m_i);
				if (!empty())
				{
					rhs.detach();
				}
			}
		}
		template <typename _Rty>
		IRef& operator=(const I<_Rty>& rhs)
		{
			internal_clear();
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.get(), _Ty::__guid, &m_i);
				internal_addref();
			}
			return *this;
		}
		template <typename _Rty>
		IRef& operator=(I<_Rty>&& rhs)
		{
			internal_clear();
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.get(), _Ty::__guid, &m_i);
				if (!empty())
				{
					rhs.detach();
				}
			}
			return *this;
		}
		// Ctor from ORef<_Rty>
		template <typename _Rty>
		IRef(const ORef<_Rty>& rhs)
		{
			m_i.self = nullptr;
			m_i.vtable = nullptr;
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.get(), _Ty::__guid, &m_i);
				internal_addref();
			}
		}
		template <typename _Rty>
		IRef(ORef<_Rty>&& rhs)
		{
			m_i.self = nullptr;
			m_i.vtable = nullptr;
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.get(), _Ty::__guid, &m_i);
				if (!empty())
				{
					rhs.detach();
				}
			}
		}
		template <typename _Rty>
		IRef& operator=(const ORef<_Rty>& rhs)
		{
			internal_clear();
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.get(), _Ty::__guid, &m_i);
				internal_addref();
			}
			return *this;
		}
		template <typename _Rty>
		IRef& operator=(ORef<_Rty>&& rhs)
		{
			internal_clear();
			if (!rhs.empty())
			{
				auto r = rhs.get()->__get_type()->query_interface(rhs.get(), _Ty::__guid, &m_i);
				if (!empty())
				{
					rhs.detach();
				}
			}
			return *this;
		}
		// Converted to ORef
		template <typename _Rty>
		operator ORef<_Rty>() const
		{
			if (convertible_to<_Rty>())
			{
				return ORef<_Rty>((_Rty*)m_i.self);
			}
			return ORef<_Rty>();
		}
		bool operator== (const IRef<_Ty>& rhs) const
		{
			return m_i.self == rhs.m_i.self;
		}
		bool operator!= (const IRef<_Ty>& rhs) const
		{
			return m_i.self != rhs.m_i.self;
		}
		bool operator< (const IRef<_Ty>& rhs) const
		{
			return (usize)m_i.self < (usize)(rhs.m_i.self);
		}
		_Ty* operator->() const
		{
			return &m_i;
		}
		//! Implicitly convert to raw interface. Same as get() method.
		//! This will not add ref count.
		operator _Ty() const
		{
			return m_i;
		}
	};

	//! Object Weak Reference smart pointer.
	template <typename _Ty>
	class OWeak
	{
		static_assert(is_base_of_v<Object, _Ty>, "The template argument of OWeak wrapper must inherit from `Object` class.");
		mutable SideTable* m_st;

		void internal_addref() const
		{
			if (m_st)
			{
				m_st->add_weak_ref();
			}
		}

		void internal_clear() const
		{
			if (m_st)
			{
				m_st->release_weak();
				m_st = nullptr;
			}
		}

		SideTable* get_handle() const
		{
			if (m_st && m_st->is_expired())
			{
				internal_clear();
			}
			return m_st;
		}

	public:
		//! clear the smart pointer and make it points to `nullptr`.
		void clear()
		{
			internal_clear();
		}
		//! Test if this pointer is empty.
		//! A pointer is valid if it is not `nullptr` and the object is not expired.
		bool empty() const
		{
			return get_handle() == nullptr;
		}
		//! Set the underlying pointer to `nullptr` and return the pointer.
		SideTable* detach()
		{
			SideTable* r = get_handle();
			m_st = nullptr;
			return r;
		}
		OWeak() : 
			m_st(nullptr) {}
		~OWeak()
		{
			internal_clear();
		}
		OWeak(const OWeak& rhs) : 
			m_st(rhs.m_st)
		{
			internal_addref();
		}
		OWeak(OWeak&& rhs) :
			m_st(rhs.detach()) {}
		OWeak& operator=(const OWeak& rhs)
		{
			internal_clear();
			m_st = rhs.m_st;
			internal_addref();
			return *this;
		}
		OWeak& operator=(OWeak&& rhs)
		{
			internal_clear();
			m_st = rhs.detach();
			return *this;
		}
		// Ctor from strong reference of the same type.
		OWeak(const ORef<_Ty>& rhs) :
			m_st(nullptr)
		{
			if (rhs)
			{
				rhs.get()->__add_weak_ref();
				m_st = rhs.get()->__get_side_table();
			}
		}
		OWeak& operator=(const ORef<_Ty>& rhs)
		{
			internal_clear();
			if (rhs)
			{
				rhs.get()->__add_weak_ref();
				m_st = rhs.get()->__get_side_table();
			}
			return *this;
		}
		// Ctor from ownership of the same type.
		OWeak(const O<_Ty>& rhs) :
			m_st(nullptr)
		{
			if (rhs)
			{
				rhs.get()->__add_weak_ref();
				m_st = rhs.get()->__get_side_table();
			}
		}
		OWeak& operator=(const O<_Ty>& rhs)
		{
			internal_clear();
			if (rhs)
			{
				rhs.get()->__add_weak_ref();
				m_st = rhs.get()->__get_side_table();
			}
			return *this;
		}
		// Ctor from raw pointer of the same type.
		OWeak(const _Ty*& rhs) :
			m_st(nullptr)
		{
			if (rhs)
			{
				rhs->__add_weak_ref();
				m_st = rhs->__get_side_table();
			}
		}
		OWeak& operator=(const _Ty*& rhs)
		{
			internal_clear();
			if (rhs)
			{
				rhs->__add_weak_ref();
				m_st = rhs->__get_side_table();
			}
			return *this;
		}
		// Convert to the strong reference of the same type.
		ORef<_Ty> lock()
		{
			auto handle = get_handle();
			if (handle)
			{
				_Ty* ptr = (_Ty*)m_st->lock_obj();
				if (!ptr)
				{
					return nullptr;
				}
				ORef<_Ty> p;
				p.attach(ptr);
				return p;
			}
			else
			{
				return nullptr;
			}
		}
		operator bool() const
		{
			return get_handle() != nullptr;
		}
		bool operator== (const WP<_Ty>& rhs) const
		{
			return get_handle() == rhs.get_handle();
		}
		bool operator!= (const WP<_Ty>& rhs) const
		{
			return get_handle() != rhs.get_handle();
		}
	};

	//! Interface Weak Reference smart pointer.
	template <typename _Ty>
	class IWeak
	{
		static_assert(is_base_of_v<Interface, _Ty>, "The template argument of IWeak wrapper must inherit from `Interface` class.");
		mutable SideTable* m_st;

		void internal_addref() const
		{
			if (m_st)
			{
				m_st->add_weak_ref();
			}
		}

		void internal_clear() const
		{
			if (m_st)
			{
				m_st->release_weak();
				m_st = nullptr;
			}
		}

		SideTable* get_handle() const
		{
			if (m_st && m_st->is_expired())
			{
				internal_clear();
			}
			return m_st;
		}

	public:
		//! clear the smart pointer and make it points to `nullptr`.
		void clear()
		{
			internal_clear();
		}
		//! Test if this pointer is empty.
		//! A pointer is valid if it is not `nullptr` and the object is not expired.
		bool empty() const
		{
			return get_handle() == nullptr;
		}
		//! Set the underlying pointer to `nullptr` and return the pointer.
		SideTable* detach()
		{
			SideTable* r = get_handle();
			m_st = nullptr;
			return r;
		}
		IWeak() :
			m_st(nullptr) {}
		~IWeak()
		{
			internal_clear();
		}
		IWeak(const IWeak& rhs) :
			m_st(rhs.m_st)
		{
			internal_addref();
		}
		IWeak(IWeak&& rhs) :
			m_st(rhs.detach()) {}
		IWeak& operator=(const IWeak& rhs)
		{
			internal_clear();
			m_st = rhs.m_st;
			internal_addref();
			return *this;
		}
		IWeak& operator=(IWeak&& rhs)
		{
			internal_clear();
			m_st = rhs.detach();
			return *this;
		}
		// Ctor from strong reference of the same type.
		IWeak(const IRef<_Ty>& rhs) :
			m_st(nullptr)
		{
			if (rhs)
			{
				_Ty i = rhs->get();
				i.self->__add_weak_ref();
				m_st = i.self->__get_side_table();
			}
		}
		IWeak& operator=(const IRef<_Ty>& rhs)
		{
			internal_clear();
			if (rhs)
			{
				_Ty i = rhs->get();
				i.self->__add_weak_ref();
				m_st = i.self->__get_side_table();
			}
			return *this;
		}
		// Ctor from ownership of the same type.
		IWeak(const I<_Ty>& rhs) :
			m_st(nullptr)
		{
			if (rhs)
			{
				_Ty i = rhs->get();
				i.self->__add_weak_ref();
				m_st = i.self->__get_side_table();
			}
		}
		IWeak& operator=(const I<_Ty>& rhs)
		{
			internal_clear();
			if (rhs)
			{
				_Ty i = rhs->get();
				i.self->__add_weak_ref();
				m_st = i.self->__get_side_table();
			}
			return *this;
		}
		// Ctor from raw pointer of the same type.
		IWeak(const _Ty& rhs) :
			m_st(nullptr)
		{
			if (rhs)
			{
				rhs.self->__add_weak_ref();
				m_st = rhs.self->__get_side_table();
			}
		}
		IWeak& operator=(const _Ty& rhs)
		{
			internal_clear();
			if (rhs)
			{
				rhs.self->__add_weak_ref();
				m_st = rhs.self->__get_side_table();
			}
			return *this;
		}
		// Convert to the strong reference of the same type.
		IRef<_Ty> lock()
		{
			auto handle = get_handle();
			if (handle)
			{
				Object* ptr = m_st->lock_obj();
				if (!ptr)
				{
					return IRef<_Ty>();
				}
				_Ty i;
				auto _ = ptr->__get_type()->query_interface(ptr, _Ty::__guid, &i);// This will never fail.
				IRef<_Ty> p;
				p.attach(i);
				return p;
			}
			else
			{
				return IRef<_Ty>();
			}
		}
		operator bool() const
		{
			return get_handle() != nullptr;
		}
		bool operator== (const WP<_Ty>& rhs) const
		{
			return get_handle() == rhs.get_handle();
		}
		bool operator!= (const WP<_Ty>& rhs) const
		{
			return get_handle() != rhs.get_handle();
		}
	};
}