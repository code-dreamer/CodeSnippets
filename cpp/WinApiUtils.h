#pragma once

namespace WinApiUtils {
;
CString GetComErrorStr(HRESULT hr);
CString GetErrorStr(DWORD error = GetLastError());

} // namespace WinApiUtils