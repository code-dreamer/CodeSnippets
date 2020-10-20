#pragma once

namespace ProcessTools
{
	KERNEL_API QString GetProcessName(DWORD pid);
	// returns -1 if process not found
	KERNEL_API int GetProcessId(const QString& processName);
	KERNEL_API bool IsProcessExist(const QString& processName);
}