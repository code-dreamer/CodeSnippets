#include "stdafx.h"
#include "WinApiUtils.h"

namespace WinApiUtils {
;
CString GetErrorStr(DWORD error /*= GetLastError()*/)
{
	CString result;

	LPVOID lpMsgBuf;
	DWORD bufLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&lpMsgBuf,
		0, NULL);

	if (bufLen) {
		LPWSTR lpMsgStr = (LPWSTR)lpMsgBuf;
		result.Append(lpMsgStr, (int)bufLen);
		LocalFree(lpMsgBuf);
	}

	return result;
}

} // namespace WinApiUtils
