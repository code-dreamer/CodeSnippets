#pragma once

#ifdef CRASHHANDLER_EXPORTS
#define CRASHHANDLER_API __declspec(dllexport)
#else
#define CRASHHANDLER_API __declspec(dllimport)
#endif

extern "C"
{
	CRASHHANDLER_API HRESULT Init(const wchar_t* appName, const wchar_t* appVersion, const wchar_t* dumpSaveDir,
		const wchar_t* restartAppFilepath, const wchar_t* restartAppCmdLine, const wchar_t* crashZipExt, DWORD dumpType);
	CRASHHANDLER_API HRESULT Shutdown();

	CRASHHANDLER_API void RaiseCrash();
}

