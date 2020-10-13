#include "stdafx.h"
#include "CrashHandlerCore.h"

#if 0
static int CALLBACK CrashCallback(CR_CRASH_CALLBACK_INFO* /*pInfo*/)
{
	// The application has crashed.

	// Close the log file here and add log to error report

	// Return CR_CB_DODEFAULT to generate error report
	return CR_CB_DODEFAULT;
}
#endif

static CComMultiThreadModel::AutoCriticalSection gCs;

static HRESULT InitImpl(CString appName, CString appVersion, CString dumpSaveDir, CString restartAppFilepath, 
						CString restartAppCmdLine, CString crashZipExt, MINIDUMP_TYPE dumpType)
{
	CComCritSecLock<CComMultiThreadModel::AutoCriticalSection> lock(gCs);
	if (crIsInstalledForCurrentProcess())
		return S_OK;

	CR_INSTALL_INFO info;
	memset(&info, 0, sizeof(CR_INSTALL_INFO));
	info.cb = sizeof(CR_INSTALL_INFO);    
	info.pszAppName = appName;
	info.pszAppVersion = appVersion;
	info.uMiniDumpType = dumpType;
	info.pszErrorReportSaveDir = dumpSaveDir;

	info.pszZipExt = crashZipExt;

	info.dwFlags |= 
		// Install all available exception handlers.
		CR_INST_ALL_POSSIBLE_HANDLERS
		// Restart the application on crash.
		| (restartAppFilepath.IsEmpty() ? 0 : CR_INST_APP_RESTART)
		// Don't send error report immediately, just save it locally.
		| CR_INST_DONT_SEND_REPORT 
		// CrashRpt should store both uncompressed error report files and ZIP archives.
		| CR_INST_STORE_ZIP_ARCHIVES 
		// Specifying this flag results in automatic installation of all available exception handlers to
		// all threads that will be created in the future. This flag only works if CrashRpt is compiled as a DLL, it does not work if you 
		// compile CrashRpt as static library.
		| CR_INST_AUTO_THREAD_HANDLERS
		// Do not show GUI
		| CR_INST_NO_GUI
		;
	
	info.pszRestartCmdLine = restartAppCmdLine;
	info.pszRestartAppFilePath = restartAppCmdLine;
	info.pszRestartAppFilePath = restartAppFilepath;
	info.nRestartTimeout = 1;
	info.bEnableLog = FALSE;

	WCHAR currDllPath[MAX_PATH] = {0};
	DWORD len = GetModuleFileName((HINSTANCE)&__ImageBase, currDllPath, MAX_PATH);
	if (len != 0)
	{
		if (PathRemoveFileSpec(currDllPath))
			info.pszDebugHelpDLL = currDllPath;
	}

	int result = crInstallW(&info);
	return result == 0 ? S_OK : E_FAIL;
	
	// Set crash callback function
	//crSetCrashCallback(CrashCallback, NULL);
	// Add our log file to the error report
	//crAddFile2(_T("log.txt"), NULL, _T("Log File"), CR_AF_MAKE_FILE_COPY); 
	// We want the screenshot of the entire desktop is to be added on crash
	//crAddScreenshot2(CR_AS_MAIN_WINDOW, 0);
	// Add a named property that means what graphics adapter is
	// installed on end user's machine
	//crAddProperty(_T("VideoCard"), _T("nVidia GeForce 8600 GTS"));
}

HRESULT Init(const wchar_t* appName, const wchar_t* appVersion, const wchar_t* dumpSaveDir,	const wchar_t* restartAppFilepath, 
			const wchar_t* restartAppCmdLine, const wchar_t* crashZipExt, DWORD dumpType)
{
	return InitImpl(appName, appVersion, dumpSaveDir, restartAppFilepath, restartAppCmdLine, crashZipExt, (MINIDUMP_TYPE)dumpType);
}

HRESULT Shutdown()
{
	if (!crIsInstalledForCurrentProcess())  
		crUninstall();

	return S_OK;
}

void InstallToCurrentThread()
{
	// If 0 is specified as dwFlags, assume all available exception handlers should be
	// installed  
	crInstallToCurrentThread2(0);
}

void UninstallFromCurrentThread()
{
	crUninstallFromCurrentThread();
}

void RaiseCrash()
{	
	//crEmulateCrash(CR_SEH_EXCEPTION);

#pragma warning(push)
#pragma warning(disable: 4774)
	printf(NULL);
#pragma warning(pop)
}
