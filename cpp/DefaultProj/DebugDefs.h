#pragma once

#ifdef DEBUG
#	define ASSERT_IMPL(expr)		\
		(void) ((!!(expr)) ||		\
		(1 != _CrtDbgReportW(_CRT_ASSERT, _CRT_WIDE(__FILE__), __LINE__, NULL, L"%s", _CRT_WIDE(#expr))) || \
		(_CrtDbgBreak(), 0))

#	define ASSERT_IMPL_MSG(expr, msg)	\
		(void) ((!!(expr)) ||			\
		(1 != _CrtDbgReportW(_CRT_ASSERT, _CRT_WIDE(__FILE__), __LINE__, NULL, L"%s \"%s\"", _CRT_WIDE(#expr), _CRT_WIDE(msg))) || \
		(_CrtDbgBreak(), 0))
#else
#	define ASSERT_IMPL(expr) NOOP
#	define ASSERT_IMPL_MSG(expr) NOOP
#endif

#define DCHECK						ASSERT_IMPL
#define DCHECK_MSG					ASSERT_IMPL_MSG
#define DFAIL						DCHECK(false)

#define DCHECK_HR(hr)				DCHECK(SUCCEEDED(hr))
#define DCHECK_STDSTRING(string)	DCHECK(!string.empty())
#define DCHECK_CSTRING(string)		DCHECK(string && strlen(string) > 0)
#define DCHECK_WCSTRING(string)		DCHECK(string && wcslen(string) > 0)
