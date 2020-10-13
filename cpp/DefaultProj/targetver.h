#pragma once

#include <WinSDKVer.h>

// Specifies that the minimum required platform is Windows 7.
// Change this to the appropriate value to target other versions of Windows.
#define WIN_VERSION _WIN32_WINNT_WIN7 /*0x0600 */

#ifndef _WIN32_WINNT
#	define _WIN32_WINNT WIN_VERSION
#endif

#ifndef WINVER
#	define WINVER WIN_VERSION
#endif

#ifndef _WIN32_WINDOWS
#	define _WIN32_WINDOWS WIN_VERSION
#endif

#include <SDKDDKVer.h>