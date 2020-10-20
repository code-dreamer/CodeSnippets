#pragma once

#pragma warning(push)
#pragma warning(disable: 4191 4266 4365 4574 4625 4626 4668 4917 4986 4987)

#ifndef VC_EXTRALEAN
#	define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

// conflicts with std::numeric_limits<>::max
#define NOMINMAX

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#ifndef _INC_WINDOWS
	#include <Windows.h>
#endif

#pragma warning(pop)
