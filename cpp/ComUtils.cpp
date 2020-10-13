#include "stdafx.h"
#include "ComUtils.h"

namespace ComUtils {
;
CString GetComErrorStr(HRESULT hr)
{
	LPWSTR buff;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		(DWORD)hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&buff,
		0,
		NULL);

	if (buff) 
	{
		int nLen = lstrlen(buff);
		if (nLen > 1 && buff[nLen - 1] == '\n') {
			buff[nLen - 1] = 0;
			if (buff[nLen - 2] == '\r') {
				buff[nLen - 2] = 0;
			}
		}

		return buff;
	}

	return L"";
}

} // namespace ComUtils
