#pragma once

namespace ComUtils {
;
CString GetComErrorStr(HRESULT hr);
CString GetErrorStr(DWORD error = GetLastError());
CString AnsiToString(const char* str);
CString FormatString(_In_z_ _Printf_format_string_ wchar_t* pszFormat, ...);

} // namespace ComUtils