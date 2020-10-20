#pragma once

namespace SystemInfoTools
{

KERNEL_API QString GetProgramFilesPath(bool x64 = false);
KERNEL_API QString GetWindowsGamesPath();

KERNEL_API enum SystemBitType
{
	  UnknownBitness = 0
	, x86BitType
	, x64BitType
	, SystemBitTypeCount = 3
};
KERNEL_API SystemBitType GetSystemBitness();
KERNEL_API bool Is64BitOS();
KERNEL_API bool Is32BitOS();

KERNEL_API QString GetWindowsDirectory();
KERNEL_API QString GetSystemDirectory();

}