#include "stdafx.h"
#include "Utils.h"

namespace Utils {
;
CString AnsiToString(const char* str)
{
	CString result;

	if (!str)
		return result;

	// Compute the needed size with the NULL terminator
	int cwch = (int)MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	if (cwch == 0)
		return result;

	LPWSTR wsOut = (LPWSTR)CoTaskMemAlloc(cwch * sizeof(WCHAR));

	if (MultiByteToWideChar(CP_ACP, 0, str, -1, wsOut, cwch) != 0)
		result.SetString(wsOut, cwch - 1);

	CoTaskMemFree(wsOut);

	return result;
}

CString FormatString(_In_z_ _Printf_format_string_ wchar_t* pszFormat, ...)
{
	DCHECK_WCSTRING(pszFormat);

#pragma warning(push)
#pragma warning(disable: 4840)

	va_list argList;
	va_start(argList, pszFormat);
	CString str;
	str.FormatV(pszFormat, argList);
	va_end(argList);

#pragma warning(pop)

	return str;
}

} // namespace Utils
