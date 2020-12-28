// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Error.cpp
* @author JXMaster
* @date 2020/11/24
*/
#include <Runtime/PlatformDefines.hpp>

#define LUNA_RUNTIME_API LUNA_EXPORT
#include <Runtime/Result.hpp>

namespace Luna
{
	ErrorDomain g_basic_domain = ErrorDomain{ "Basic" };

	constexpr error_code_t e_failure = 1;
	constexpr error_code_t e_not_found = 2;
	constexpr error_code_t e_already_exists = 3;
	constexpr error_code_t e_bad_arguments = 4;
	constexpr error_code_t e_bad_calling_time = 5;
	constexpr error_code_t e_bad_memory_alloc = 6;
	constexpr error_code_t e_not_supported = 7;
	constexpr error_code_t e_bad_system_call = 8;
	constexpr error_code_t e_access_denied = 9;
	constexpr error_code_t e_not_directory = 10;
	constexpr error_code_t e_timeout = 11;
	constexpr error_code_t e_data_too_long = 12;
	constexpr error_code_t e_insufficient_buffer = 13;
	constexpr error_code_t e_busy = 14;
	constexpr error_code_t e_out_of_range = 15;
	constexpr error_code_t e_syntax_error = 16;
	constexpr error_code_t e_user_canceled = 17;

	namespace BasicError
	{
		LUNA_RUNTIME_API Error failure = Error(&g_basic_domain, e_failure);
		LUNA_RUNTIME_API Error not_found = Error(&g_basic_domain, e_not_found);
		LUNA_RUNTIME_API Error already_exists = Error(&g_basic_domain, e_already_exists);
		LUNA_RUNTIME_API Error bad_arguments = Error(&g_basic_domain, e_bad_arguments);
		LUNA_RUNTIME_API Error bad_calling_time = Error(&g_basic_domain, e_bad_calling_time);
		LUNA_RUNTIME_API Error bad_memory_alloc = Error(&g_basic_domain, e_bad_memory_alloc);
		LUNA_RUNTIME_API Error not_supported = Error(&g_basic_domain, e_not_supported);
		LUNA_RUNTIME_API Error bad_system_call = Error(&g_basic_domain, e_bad_system_call);
		LUNA_RUNTIME_API Error access_denied = Error(&g_basic_domain, e_access_denied);
		LUNA_RUNTIME_API Error not_directory = Error(&g_basic_domain, e_not_directory);
		LUNA_RUNTIME_API Error timeout = Error(&g_basic_domain, e_timeout);
		LUNA_RUNTIME_API Error data_too_long = Error(&g_basic_domain, e_data_too_long);
		LUNA_RUNTIME_API Error insufficient_buffer = Error(&g_basic_domain, e_insufficient_buffer);
		LUNA_RUNTIME_API Error busy = Error(&g_basic_domain, e_busy);
		LUNA_RUNTIME_API Error out_of_range = Error(&g_basic_domain, e_out_of_range);
		LUNA_RUNTIME_API Error syntax_error = Error(&g_basic_domain, e_syntax_error);
		LUNA_RUNTIME_API Error user_canceled = Error(&g_basic_domain, e_user_canceled);
	}
}
