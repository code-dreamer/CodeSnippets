#include "stdafx.h"
#include <windows.h>
#include <AccCtrl.h>
#include <Aclapi.h>
#include "ProcessRunner.h"
#include "StringTools.h"
#include "FileSystemTools.h"
#include "GameStateInterface.h"
#include "WinApiTools.h"

namespace
{
const int s_timerInterval = 50;

wchar_t* QStrToWStrSpecial(const QString& source)
{
	int lenght = source.length();
	wchar_t* result = new wchar_t [ lenght + 1 ];
	StringTools::CopyQStringToWArray(source, result);
	result[lenght] = '\0';

	return result;
}

void GetMsiPath(wchar_t* fullPath)
{
	wchar_t sysDir[1000];
	int result = GetSystemDirectory(sysDir, 1000);
	G_ASSERT(result > 0);
	wchar_t msiExeName[] = L"\\msiexec.exe";

	wcscpy(fullPath, sysDir);
	wcscat(fullPath, msiExeName);
}

void GetMsiCmdLine(const QString& exePath, wchar_t* CmdLine)
{
	wchar_t* fullExeName = QStrToWStrSpecial(exePath);
	wcscpy(CmdLine, L" /i ");
	wcscat(CmdLine, fullExeName);
	delete[] fullExeName;
}

bool IsMsiInstaller(const QString& exeName)
{
	QString extantion = STR(".msi");

	if (exeName[exeName.length() - 1] == extantion[3]
		&& exeName[exeName.length() - 2] == extantion[2]
		&& exeName[exeName.length() - 3] == extantion[1]
		&& exeName[exeName.length() - 4] == extantion[0]
		) {
		return true;
	}

	return false;
}
}

ProcessRunner::ProcessRunner(QObject* const parent, QString path)
	: QObject(parent)
	, timerId_(0)
	, timerIdGameMonitor_(0)
	, pathToGame_(StringTools::CreateStdWString(path))
{
	gameProcessHandle_ = NULL;
	memset(&startUpInfo_, 0, sizeof(startUpInfo_));
	startUpInfo_.cb = sizeof(startUpInfo_);
	memset(&processInfo_, 0, sizeof(processInfo_));

	timerIdGameMonitor_ = startTimer(s_timerInterval);
}

ProcessRunner::~ProcessRunner()
{
	if ( timerId_ != 0 ) {
		killTimer(timerId_);
		timerId_ = 0;
	}

	if ( timerIdGameMonitor_ != 0 ) {
		killTimer(timerIdGameMonitor_);
		timerIdGameMonitor_ = 0;
	}	

	if ( processInfo_.hProcess != NULL ) {
		CloseHandle( processInfo_.hProcess );
		memset(&processInfo_, NULL, sizeof(processInfo_));
	}

	if ( gameProcessHandle_ != NULL ) {
		CloseHandle( gameProcessHandle_ );
		gameProcessHandle_ = NULL;
	}
}

void ProcessRunner::RunInstaller(const QString& exePath)
{
	QString corectExePath = QDir::toNativeSeparators(exePath);

	if (!IsMsiInstaller(corectExePath)) {
		RunInstallerExe(corectExePath);
	}
	else {
		RunInstallerMsi(corectExePath);
	}
}

void ProcessRunner::RunInstallerExe(const QString& exeName)
{
	QSTRING_NOT_EMPTY(exeName);
	G_ASSERT( FileSystemTools::IsPathExist(exeName) );
	G_ASSERT( processInfo_.hProcess == 0 );

	wchar_t* fullExeName = QStrToWStrSpecial(exeName);
	wchar_t* workDir = QStrToWStrSpecial( FileSystemTools::GetFileDir(exeName) );

	//------------
	PSID adminSid = NULL;
	PACL accesControlList = NULL;
	SECURITY_DESCRIPTOR securDescriptor;
	BOOL result;

	SID_IDENTIFIER_AUTHORITY sidIdentifierAuthority = SECURITY_NT_AUTHORITY;
	result = AllocateAndInitializeSid(&sidIdentifierAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0,0,0,0,0,0, &adminSid);
	G_ASSERT( result );
	if( !result ) {
		if (adminSid) FreeSid(adminSid);
		adminSid = NULL;
	}

	EXPLICIT_ACCESS explicitAccces;
	memset(&explicitAccces, 0, sizeof(EXPLICIT_ACCESS));

	explicitAccces.grfAccessPermissions = KEY_ALL_ACCESS;
	explicitAccces.grfAccessMode = SET_ACCESS;
	explicitAccces.grfInheritance = NO_INHERITANCE;
	explicitAccces.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	explicitAccces.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	explicitAccces.Trustee.ptstrName = (LPWSTR)adminSid;

	DWORD rez;
	rez = SetEntriesInAcl(1, &explicitAccces, NULL, &accesControlList);
	G_ASSERT( rez == ERROR_SUCCESS );
	if( rez != ERROR_SUCCESS ) {
		if (accesControlList) LocalFree(accesControlList);
		accesControlList = NULL;
	}

	result = InitializeSecurityDescriptor(&securDescriptor, SECURITY_DESCRIPTOR_REVISION);
	G_ASSERT( result );
	if (result) {
		SetSecurityDescriptorDacl(&securDescriptor, true, accesControlList, false);
	}

	SECURITY_ATTRIBUTES processAtribute;
	processAtribute.nLength  = sizeof(SECURITY_ATTRIBUTES);
	processAtribute.bInheritHandle = false;
	processAtribute.lpSecurityDescriptor = &securDescriptor;
	//------------

	result = CreateProcess( fullExeName, NULL, &processAtribute, NULL, false, 0, NULL, workDir, &startUpInfo_, &processInfo_);
	G_ASSERT( result );

	if(result){
		if ( timerId_ != 0 ) {
			killTimer(timerId_);
			timerId_ = 0;
		}
		gameProcessHandle_ = processInfo_.hProcess;
		timerId_ = startTimer(s_timerInterval);
		Q_EMIT ProcessStarted();
	}
	else {
		Q_EMIT ProcessAndChildsFinished( ProcessRunner::UndefinedExit );
	}

	delete[] fullExeName;
	delete[] workDir;

	if (adminSid) FreeSid(adminSid);
	if (accesControlList) LocalFree(accesControlList);
}

void ProcessRunner::RunInstallerMsi(const QString& exeName)
{
	QSTRING_NOT_EMPTY(exeName);
	G_ASSERT( FileSystemTools::IsPathExist(exeName) );
	G_ASSERT( processInfo_.hProcess == 0 );

	wchar_t msiPath[1000];
	GetMsiPath(msiPath);
	wchar_t msiCmdLine[10000];
	GetMsiCmdLine(exeName, msiCmdLine);

	BOOL result = CreateProcess( msiPath, msiCmdLine, NULL, NULL, false, 0, NULL, NULL, &startUpInfo_, &processInfo_);
	G_ASSERT( result );

	if ( timerId_ != 0 ) {
		killTimer(timerId_);
		timerId_ = 0;
	}
	gameProcessHandle_ = processInfo_.hProcess;
	timerId_ = startTimer(s_timerInterval);
	Q_EMIT ProcessStarted();
}

void ProcessRunner::RunGame(const QString& exePath)
{
	QSTRING_NOT_EMPTY(exePath);
	G_ASSERT( FileSystemTools::IsPathExist(exePath) );
	G_ASSERT( processInfo_.hProcess == 0 );	

	wchar_t* fullExeName = QStrToWStrSpecial(exePath);
	wchar_t* workDir = QStrToWStrSpecial( FileSystemTools::GetFileDir(exePath) );

	BOOL result = CreateProcess( fullExeName, NULL, NULL, NULL, false, 0, NULL, workDir, &startUpInfo_, &processInfo_);
	G_ASSERT( result );

	if ( timerId_ != 0 ) {
		killTimer(timerId_);
		timerId_ = 0;
	}

	gameProcessHandle_ = processInfo_.hProcess;
	timerId_ = startTimer(s_timerInterval);
	Q_EMIT ProcessStarted();

	delete[] fullExeName;
	delete[] workDir;
}

void ProcessRunner::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == timerId_) {

		if (gameProcessHandle_) {
			DWORD exitCode = STILL_ACTIVE;
			GetExitCodeProcess(gameProcessHandle_, &exitCode);

			if (exitCode != STILL_ACTIVE) {
				memset(&startUpInfo_, 0, sizeof(startUpInfo_));
				startUpInfo_.cb = sizeof(startUpInfo_);

				CloseHandle(gameProcessHandle_);
				memset(&processInfo_, 0, sizeof(processInfo_));
				gameProcessHandle_ = NULL;
				if (exitCode == 0) {
					Q_EMIT ProcessAndChildsFinished(ProcessRunner::NormalExit);
				}
				else {
					Q_EMIT ProcessAndChildsFinished(ProcessRunner::UndefinedExit);
				}
			}
		}
	}
	else {
		if (event->timerId() == timerIdGameMonitor_) {
			if (gameProcessHandle_) {
				return;
			}

			std::vector < std::pair < std::wstring, DWORD >> allProcesses;
			WinApiTools::GetListFullFilePathProcess(&allProcesses);

			int count = allProcesses.size();
			for (int i = 0; i < count; ++i) {
				if (allProcesses[i].first == pathToGame_) {
					HANDLE currentThreadToken = nullptr;
					if (!WinApiTools::OpenCurrentThreadToken(currentThreadToken)) {
						currentThreadToken = nullptr;
					}
					WinApiTools::ActivateDebugPrivilege(currentThreadToken, true);
					gameProcessHandle_ = OpenProcess(PROCESS_ALL_ACCESS, 0, allProcesses[i].second);
					WinApiTools::ActivateDebugPrivilege(currentThreadToken, false);

					if (gameProcessHandle_) {
						Q_EMIT GameProcessDetected();
						if (timerId_ != 0) {
							killTimer(timerId_);
							timerId_ = 0;
						}
						timerId_ = startTimer(s_timerInterval);
					}
					break;
				}
			}
		}
		else {
			QObject::timerEvent(event);
		}
	}
}
