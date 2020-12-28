// Copyright 2018-2020 Luna Project. All rights reserved.
/*
* @file Result.hpp
* @author JXMaster
* @date 2020/6/29
 */
#pragma once
#include "MemoryUtils.hpp"
#include "Assert.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	//! `error_code_t` is an aliasing type of `i32`, which represents an error code number.
	//! The error code is 0 if there is no error, any other error codes represents one error.
	using error_code_t = i32;

	//! Represents one error code domain.
	//! Every error code domain shall only have one instance, so we can simply compare
	//! two error domain pointer to check if they are in the same domain.
	struct ErrorDomain
	{
		const c8* name; // The name of the domain. This must be a static string.
	};

	//! Represents one error. One error object encapsulates one error code and one error domain.
	//! The error may have error code 0 and error domain `nullptr`, in which case this represents no error.
	//! The error domain is assumed only valid when the error code is not 0.
	struct Error
	{
		const ErrorDomain* error_domain;
		error_code_t error_code;

		//! The default constructor constructs an empty error object, which represents "no error".
		Error() :
			error_domain(nullptr),
			error_code(0) {}

		Error(const ErrorDomain* _error_domain, error_code_t _error_code) :
			error_domain(_error_domain),
			error_code(_error_code) {}

		Error(const Error&) = default;
		Error& operator=(const Error&) = default;

		//! Compares two error object. The error domain is compared only when the error code is not 0.
		bool operator==(const Error& rhs) const
		{
			if (error_code)
			{
				return (error_code == rhs.error_code) && (error_domain == rhs.error_domain);
			}
			return error_code == rhs.error_code;
		}
		bool operator!=(const Error& rhs) const
		{
			return !(*this == rhs);
		}

		//! Checks if this error is empty, that is, contains no error information.
		bool empty() const
		{
			return (error_code == 0);
		}
	};

	//! R<_Ty> (represents for Result) is the wrapper object for the returned value of the function that may fail.
	//! If the function succeeds, `R<_Ty>` contains the real returned object; if the function fails, `R<_Ty>` contains 
	//! the error code along with the error domain so that it can be identified.
	template <typename _Ty>
	struct [[nodiscard]] R
	{
	private:
		error_code_t m_err_code;
		union
		{
			_Ty m_value;
			const ErrorDomain* m_err_domain;
		};
	public:

		//! Tests if the returned value is valid.
		bool valid() const
		{
			return m_err_code == 0;
		}

		//! Constructs with the returned value means success.
		R(const _Ty& v) :
			m_err_code(0)
		{
			new (&m_value) _Ty(v);
		}
		R(_Ty&& v) :
			m_err_code(0)
		{
			new (&m_value) _Ty(move(v));
		}

		//! Constructs with Error object means error.
		R(const Error& error) :
			m_err_code(error.error_code),
			m_err_domain(error.error_domain)
		{
			luassert(m_err_code);
		}

		R(const R& rhs) :
			m_err_code(rhs.m_err_code)
		{
			if (m_err_code)
			{
				m_err_domain = rhs.m_err_domain;
			}
			else
			{
				new (&m_value) _Ty(rhs.m_value);
			}
		}
		R(R&& rhs) :
			m_err_code(rhs.m_err_code)
		{
			if (m_err_code)
			{
				m_err_domain = rhs.m_err_domain;
			}
			else
			{
				new (&m_value) _Ty(move(rhs.m_value));
			}
		}
		R& operator=(const R& rhs)
		{
			if (m_err_code && !rhs.m_err_code)	// this is failure and rhs is success.
			{
				new (&m_value) _Ty(rhs.m_value);
			}
			else if (!m_err_code && !rhs.m_err_code)	// this is success and rhs is success.
			{
				m_value = rhs.m_value;
			}
			else if (!m_err_code && rhs.m_err_code) // this is success and rhs is failure.
			{
				m_value.~_Ty();
				m_err_domain = rhs.m_err_domain;
			}
			else // this is failure and rhs is failure.
			{
				m_err_domain = rhs.m_err_domain;
			}
			m_err_code = rhs.m_err_code;
			return *this;
		}
		R& operator=(R&& rhs)
		{
			if (m_err_code && !rhs.m_err_code)	// this is failure and rhs is success.
			{
				new (&m_value) _Ty(move(rhs.m_value));
			}
			else if (!m_err_code && !rhs.m_err_code)	// this is success and rhs is success.
			{
				m_value = move(rhs.m_value);
			}
			else if (!m_err_code && rhs.m_err_code) // this is success and rhs is failure.
			{
				m_value.~_Ty();
				m_err_domain = rhs.m_err_domain;
			}
			else // this is failure and rhs is failure.
			{
				m_err_domain = rhs.m_err_domain;
			}
			m_err_code = rhs.m_err_code;
			return *this;
		}

		~R()
		{
			if (!m_err_code)
			{
				m_value.~_Ty();
			}
		}

		//! Gets the returned value.
		//! This can be called if and only if `valid()` is `true`.
		const _Ty& get() const
		{
			luassert(valid());
			return m_value;
		}
		_Ty& get()
		{
			luassert(valid());
			return m_value;
		}

		//! Gets the error object.
		//! If the returned value is valid, this returns an empty error object.
		Error error() const
		{
			return valid() ? Error() : Error(m_err_domain, m_err_code);
		}

		operator Error() const
		{
			return error();
		}

		//! Same as `valid()`.
		operator bool() const
		{
			return valid();
		}
	};

	// Specification for void type.
	template <>
	struct [[nodiscard]] R<void>
	{
	private:
		error_code_t m_err_code;
		const ErrorDomain* m_err_domain;

	public:
		//! Tests if the returned value is valid.
		bool valid() const
		{
			return m_err_code == 0;
		}

		//! Success does not require any returned value.
		R() : m_err_code(0) {}

		//! Returns the error object means error.
		R(const Error& error) :
			m_err_code(error.error_code),
			m_err_domain(error.error_domain)
		{
			luassert(error.error_code);
		}

		R(const R& rhs) :
			m_err_code(rhs.m_err_code)
		{
			if (m_err_code)
			{
				m_err_domain = rhs.m_err_domain;
			}
		}
		R& operator=(const R& rhs)
		{
			m_err_code = rhs.m_err_code;
			if (m_err_code)
			{
				m_err_domain = rhs.m_err_domain;
			}
			return *this;
		}

		//! Gets the error object.
		//! If the returned value is valid, this returns an empty error object.
		Error error() const
		{
			return valid() ? Error() : Error(m_err_domain, m_err_code);
		}

		operator Error() const
		{
			return error();
		}

		//! Same as `valid()`.
		operator bool() const
		{
			return valid();
		}
	};

	using RV = R<void>;

	//! The errors used by the Runtime itself. These errors can also be used by user module.
	namespace BasicError
	{
		//! General failure. 
		//! @remark The user should avoid returning generic failure if it can determine what is going wrong and can report that
		//! since returning generic failure does not provide any information for the caller to handle the error.
		extern LUNA_RUNTIME_API Error failure;

		//! The specified item does not exist.
		extern LUNA_RUNTIME_API Error not_found;

		//! The specified item already exists.
		extern LUNA_RUNTIME_API Error already_exists;

		//! Invalid arguments are specified.
		extern LUNA_RUNTIME_API Error bad_arguments;

		//! The operation is invoked in improper time, like trying to reset the resource when the resource is still using.
		extern LUNA_RUNTIME_API Error bad_calling_time;

		//! Memory allocation failed.
		extern LUNA_RUNTIME_API Error bad_memory_alloc;

		//! This operation is not supported, it is most possibly not implemented on this build/platform.
		extern LUNA_RUNTIME_API Error not_supported;

		//! A call to the underlying operating system is failed and the reason cannot be represented by any other
		//! result code.
		extern LUNA_RUNTIME_API Error bad_system_call;

		//! The access to the file or resource is denied.
		extern LUNA_RUNTIME_API Error access_denied;

		//! The specified path is not a directory, or one component of the path prefix of the specified path is not a directory.
		extern LUNA_RUNTIME_API Error not_directory;

		//! The time limit is reached before the operation finishes.
		extern LUNA_RUNTIME_API Error timeout;

		//! The provided data or string is too long.
		extern LUNA_RUNTIME_API Error data_too_long;

		//! The memory buffer provided by the user is not large enough to contain all returned data.
		extern LUNA_RUNTIME_API Error insufficient_buffer;

		//! The service provider is too busy to finish the request call.
		extern LUNA_RUNTIME_API Error busy;

		//! The number or container overflows, or the index specified by the user is not valid.
		extern LUNA_RUNTIME_API Error out_of_range;

		//! Syntax error detected when parsing the scripts or strings.
		extern LUNA_RUNTIME_API Error syntax_error;

		//! The operation is canceled by user.
		extern LUNA_RUNTIME_API Error user_canceled;
	}
}

// Static-typed zero-overhead exception mechanism.

#define lures _try_res
#define luerr _try_err
#define lutry Error lures;
#define luthrow(_r) lures = (_r); goto luerr;
#define lucatch luerr: if(!lures.empty())
#define lucatchret luerr: if(!lures.empty()) { return lures; }
#define luexp(_exp) lures = (_exp); if(!lures.empty()) { goto luerr; }
#define luset(_v, _exp)  { auto _res = (_exp); if(!_res.valid()) { lures = _res.error(); goto luerr; } (_v) = move(_res.get()); }
#define lulet(_v, _exp) auto _r_##_v = (_exp); if(!(_r_##_v).valid()) { lures = _r_##_v.error(); goto luerr; } auto& _v = _r_##_v.get();

#define lures2 _try_res2
#define luerr2 _try_err2
#define lutry2 Error lures2;
#define luthrow2(_r) lures2 = (_r); goto luerr2;
#define lucatch2 luerr2: if(!lures2.empty())
#define lucatchret2 luerr2: if(!lures2.empty()) { return lures2; }
#define luexp2(_exp) lures2 = (_exp); if(!lures2.empty()) { goto luerr2; }
#define luset2(_v, _exp)  { auto _res = (_exp); if(!_res.valid()) { lures2 = _res.error(); goto luerr2; } (_v) = move(_res.get()); }
#define lulet2(_v, _exp) auto _r_##_v = (_exp); if(!(_r_##_v).valid()) { lures2 = _r_##_v.error(); goto luerr2; } auto& _v = _r_##_v.get();

#define lures3 _try_res3
#define luerr3 _try_err3
#define lutry3 Error lures3;
#define luthrow3(_r) lures3 = (_r); goto luerr3;
#define lucatch3 luerr3: if(!lures3.empty())
#define lucatchret3 luerr3: if(!lures3.empty()) { return lures3; }
#define luexp3(_exp) lures3 = (_exp); if(!lures3.empty()) { goto luerr3; }
#define luset3(_v, _exp)  { auto _res = (_exp); if(!_res.valid()) { lures3 = _res.error(); goto luerr3; } (_v) = move(_res.get()); }
#define lulet3(_v, _exp) auto _r_##_v = (_exp); if(!(_r_##_v).valid()) { lures3 = _r_##_v.error(); goto luerr3; } auto& _v = _r_##_v.get();