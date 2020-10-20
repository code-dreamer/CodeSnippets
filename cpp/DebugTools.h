#pragma once

#include <KernelLibSwitch.h>

namespace DebugTools
{

KERNEL_API int ShowDebugMessageBox(const wchar_t* expression, const wchar_t* functionName, 
	const wchar_t* filename, unsigned lineNumber, const wchar_t* errorMessage = nullptr);

KERNEL_API void AbortApplication();

}
