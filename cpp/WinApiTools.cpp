#include "stdafx.h"

#include <Strsafe.h>
#include "WinApiTools.h"
#include "StringTools.h"
#include "BitTools.h"
#include "FileSystemTools.h"

namespace
{

std::map<std::wstring, std::wstring> InfellDrivesMap();

static const std::map<std::wstring, std::wstring> s_drivesMap = InfellDrivesMap();

std::map<std::wstring, std::wstring> InfellDrivesMap()
{
	std::map<std::wstring, std::wstring> drivesMap;

	const int allDrivesLength = 1000;
	wchar_t allDrives[allDrivesLength] = L"\0";
	if ( !GetLogicalDriveStrings(allDrivesLength, allDrives) ){
		return drivesMap;
	}

	wchar_t dosDrive[MAX_PATH] = L"\0";
	int currentPos = 0;
	const int driveLetterLength = 3;
	wchar_t driveLetter[driveLetterLength];

	do {
		driveLetter[0] = '\0';
		wcsncpy(driveLetter, &allDrives[currentPos], driveLetterLength);
		if (wcslen(driveLetter) == 0) {
			break;
		}
		driveLetter[driveLetterLength - 1] = '\0';

		if (QueryDosDevice(driveLetter, dosDrive, MAX_PATH)) {
			drivesMap[dosDrive] = driveLetter;
		}

		currentPos += driveLetterLength + 1;
	} while (currentPos < allDrivesLength);

	return drivesMap;
}

bool ReadRegKeyValue(const wchar_t* regkey, const wchar_t* param, wchar_t* value)
{
	HKEY hCurentKey;
	const int rootKeysCount = 2;
	HKEY hRootKeys[ rootKeysCount ] = { HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE };
	int valueSize;
	DWORD keyType = REG_NONE;
	value[0] = '\0';

	for(int i=0; i<rootKeysCount; ++i) {
		if( RegOpenKeyEx(hRootKeys[i], regkey, 0, KEY_QUERY_VALUE, &hCurentKey) == ERROR_SUCCESS) {
			RegQueryValueEx(hCurentKey, param, NULL, &keyType, NULL, (LPDWORD)&valueSize);
			if(keyType == REG_SZ) {
				RegQueryValueEx(hCurentKey, param, NULL, NULL, (LPBYTE)value, (LPDWORD)&valueSize);
				if(wcslen(value) > 2) {
					RegCloseKey(hCurentKey);
					return true;
				}
			}
			RegCloseKey(hCurentKey);
		}
	}
	return false;
}

#pragma warning(push)
#pragma warning(disable : 4995)	
bool SeparateRegKey(const QString fullRegKey, wchar_t* regKey, wchar_t* regParam)
{
	QSTRING_NOT_EMPTY(fullRegKey);
	std::wstring tempstr = StringTools::CreateStdWString(fullRegKey);

	const wchar_t* temp = wcsrchr(tempstr.c_str(), '\\');
	if(temp == nullptr) {
		return false;
	}
	int lentParam = wcslen(temp);
	wcscpy(regParam, ++temp);

	G_ASSERT(--temp != wcschr(tempstr.c_str(), '\\') );

	temp = wcschr(tempstr.c_str(), '\\');
	int lentRegkey = wcslen(temp);
	wcscpy(regKey, ++temp);
	regKey[lentRegkey-lentParam] = '\0';

	return true;
}
#pragma warning(pop)

} // namespace

static DWORD QButtonsToWinApiButtons(QMessageBox::StandardButtons buttons);
static QMessageBox::StandardButton WinApiButtonToQButton(int winButton);
static DWORD QIconToWinApiIcon(const QMessageBox::Icon icon);

int WinApiTools::ShowWinMessageBox(const QWidget* parent, const QString& caption, const QString& text, DWORD flags)
{
	const std::wstring stdTitle = StringTools::CreateStdWString(caption);
	const std::wstring stdText = StringTools::CreateStdWString(text);
	HWND parentHandle = nullptr;
	if (parent != nullptr) {
		parentHandle = reinterpret_cast<HWND>( parent->winId() );
	}

	return ::MessageBox(parentHandle, stdText.c_str(), stdTitle.c_str(), flags);
}

QMessageBox::StandardButton WinApiTools::ShowMessageBox(const QWidget* parent, const QString& caption, const QString& text, 
	const QMessageBox::Icon icon, const QMessageBox::StandardButtons& buttons)
{
	using namespace BitTools;

	DWORD winIconFlag = QIconToWinApiIcon(icon);
	DWORD winButtonFlag = QButtonsToWinApiButtons(buttons);

	const std::wstring stdTitle = StringTools::CreateStdWString(caption);
	const std::wstring stdText = StringTools::CreateStdWString(text);
	HWND parentHandle = nullptr;
	if (parent != nullptr) {
		parentHandle = reinterpret_cast<HWND>( parent->winId() );
	}
	
	const int result = ::MessageBox(parentHandle, stdText.c_str(), stdTitle.c_str(), winIconFlag | winButtonFlag);
	return WinApiButtonToQButton(result);
}

static DWORD QButtonsToWinApiButtons(QMessageBox::StandardButtons buttons)
{
	using namespace BitTools;

	DWORD winButtonFlag = 0;
	if ( IsBitExist(buttons, QMessageBox::Ok) ) {
		AddBit(winButtonFlag, MB_OK);
	} 
	if ( IsBitExist(buttons, QMessageBox::Yes) && IsBitExist(buttons, QMessageBox::No) ) {
		AddBit(winButtonFlag, MB_YESNO);
	}
	if ( IsBitExist(buttons, QMessageBox::Yes) && IsBitExist(buttons, QMessageBox::No) 
		&& IsBitExist(buttons, QMessageBox::Cancel) ) {
			AddBit(winButtonFlag, MB_YESNOCANCEL);
	}
	if ( IsBitExist(buttons, QMessageBox::Ok) && IsBitExist(buttons, QMessageBox::Cancel) ) {
		AddBit(winButtonFlag, MB_OKCANCEL);
	}
	if ( IsBitExist(buttons, QMessageBox::Abort) && IsBitExist(buttons, QMessageBox::Retry) 
		&& IsBitExist(buttons, QMessageBox::Ignore)) {
			AddBit(winButtonFlag, MB_ABORTRETRYIGNORE);
	}
	if ( IsBitExist(buttons, QMessageBox::Retry) && IsBitExist(buttons, QMessageBox::Cancel) ) {
		AddBit(winButtonFlag, MB_RETRYCANCEL);
	}

	return winButtonFlag;
}

static QMessageBox::StandardButton WinApiButtonToQButton(int winButton)
{
	using namespace BitTools;

	QMessageBox::StandardButton button = QMessageBox::Ok;

	if ( IsBitExist(winButton, IDABORT) ) {
		button = QMessageBox::Abort;
	}
	else if ( IsBitExist(winButton, IDCANCEL) ) {
		button = QMessageBox::Cancel;
	}
	else if ( IsBitExist(winButton, IDIGNORE) ) {
		button = QMessageBox::Ignore;
	}
	else if ( IsBitExist(winButton, IDNO) ) {
		button = QMessageBox::No;
	}
	else if ( IsBitExist(winButton, IDOK) ) {
		button = QMessageBox::Ok;
	}
	else if ( IsBitExist(winButton, IDRETRY) || IsBitExist(winButton, IDTRYAGAIN) ) {
		button = QMessageBox::Retry;
	}
	else if ( IsBitExist(winButton, IDYES) ) {
		button = QMessageBox::Yes;
	}

	return button;
}

static DWORD QIconToWinApiIcon(const QMessageBox::Icon icon)
{
	using namespace BitTools;

	DWORD winIconFlag = 0;
	if ( icon == QMessageBox::Information ) {
		winIconFlag = MB_ICONINFORMATION;
	}
	else if ( icon == QMessageBox::Warning ) {
		winIconFlag = MB_ICONWARNING;
	}
	else if ( icon == QMessageBox::Critical ) {
		winIconFlag = MB_ICONERROR;
	}
	else if ( icon == QMessageBox::Question ) {
		winIconFlag = MB_ICONQUESTION;
	}
	else {
		G_ASSERT(false);
	}
	G_ASSERT(winIconFlag != 0);

	return winIconFlag;
}

QString WinApiTools::ShowFolderDialog(const QString& title, const QWidget* const owner)
{
	QString selectedFolder;

	BROWSEINFO browseInfo;
	memset(&browseInfo, 0, sizeof(browseInfo));

	browseInfo.ulFlags   = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_NONEWFOLDERBUTTON;
	if (QSysInfo::windowsVersion() >= QSysInfo::WV_VISTA) {
		browseInfo.ulFlags |= BIF_NEWDIALOGSTYLE;
	}
	browseInfo.hwndOwner = (owner == nullptr ? nullptr : owner->winId());
	std::wstring titleStd = StringTools::CreateStdWString(title);
	browseInfo.lpszTitle = titleStd.c_str();

	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	const LPITEMIDLIST pIDL = SHBrowseForFolder(&browseInfo);

	if (pIDL != nullptr) {
		// Create a buffer to store the path, then get the path.
		wchar_t buffer[_MAX_PATH] = {'\0'};
		if(SHGetPathFromIDList(pIDL, buffer) != 0) {
			selectedFolder = StringTools::CreateQString(buffer);
		}		

		// free the item id list
		CoTaskMemFree(pIDL);
	}

	CoUninitialize();

	return selectedFolder;
}

QString WinApiTools::FormatWinError(DWORD errorCode) {
	QString errorDescription;

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = errorCode; 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
		(lstrlen((LPCTSTR)lpMsgBuf) + 40) * sizeof(TCHAR)); 
	StringCchPrintf((LPTSTR)lpDisplayBuf, 
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s"), 
		lpMsgBuf); 

	errorDescription = StringTools::CreateQString((LPTSTR)lpMsgBuf);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);

	return errorDescription;
}

QString WinApiTools::GetRegKeyValue(const QString fullRegKey)
{
	QString value;
	if( fullRegKey.isEmpty() ) {
		return value;
	}
	wchar_t* regkey = new wchar_t[ fullRegKey.length() ];
	wchar_t* param = new wchar_t[ fullRegKey.length() ];
	wchar_t tempValue[10000];

	if( SeparateRegKey(fullRegKey, regkey, param) && ReadRegKeyValue(regkey, param, tempValue) ) {
		wchar_t* rezValue = StringTools::DeleteEnclosingQuote(tempValue);
		value = StringTools::CreateQString(rezValue);
	}
	delete[] regkey;
	delete[] param;
	
	return value;
}

QString WinApiTools::GetProgramFilesDir()
{
	QString resultDir;

	wchar_t rawPath[MAX_PATH] = L"\0";
	HRESULT result = ::SHGetFolderPath(nullptr, CSIDL_PROGRAM_FILES, nullptr, SHGFP_TYPE_CURRENT, rawPath);
	if ( result == S_OK ) {
		resultDir = StringTools::CreateQString(rawPath);
	}
	else {
		resultDir = STR(qgetenv("PROGRAMFILES").constData());
	}

	resultDir.remove(STR(" (x86)"));

	return FileSystemTools::IsPathExist(resultDir) ? resultDir : EMPTY_STR;
}

QString WinApiTools::GetStandartWindowsGamesDir()
{
	// TODO: what if GetProgramFilesDir failed and returns empty string? 
	return FileSystemTools::MergePath( GetProgramFilesDir(), STR("\\Microsoft Games") );
}

void WinApiTools::GetListFullFilePathProcess(std::vector<std::pair<std::wstring, DWORD>>* processes)
{
	CHECK_PTR(processes);
	G_ASSERT(processes->size() == 0);

	HANDLE currentThreadToken = nullptr;
	if (!WinApiTools::OpenCurrentThreadToken(currentThreadToken)) {
		currentThreadToken = nullptr;
	}

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 curentProcess;
	curentProcess.dwSize = sizeof(PROCESSENTRY32);
	BOOL exist = Process32First(hSnapshot, &curentProcess);

	WinApiTools::ActivateDebugPrivilege(currentThreadToken, true);

	const int arrayLength = 10000;
	wchar_t fullPath[arrayLength] = L"\0";
	wchar_t temp[arrayLength] = L"\0";
	std::wstring tempCurentProcesFilePath;

	while (exist) {
		HANDLE hCurentProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, curentProcess.th32ProcessID);
		if (hCurentProcess != NULL && GetProcessImageFileName(hCurentProcess, fullPath, arrayLength)) {
			tempCurentProcesFilePath = DeviceFormatPathToDriveLetterPath(fullPath, temp);
			processes->push_back(std::make_pair(tempCurentProcesFilePath, curentProcess.th32ProcessID));
		}
		CloseHandle(hCurentProcess);
		exist = Process32Next(hSnapshot, &curentProcess);
	}

	CloseHandle(hSnapshot);
	WinApiTools::ActivateDebugPrivilege(currentThreadToken, false);
	CloseHandle(currentThreadToken);
}

bool WinApiTools::OpenCurrentThreadToken(HANDLE& token)
{
	if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &token)) {
		if (GetLastError() == ERROR_NO_TOKEN) {
			if (!ImpersonateSelf(SecurityImpersonation)) {
				return false;
			}

			if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &token)) {
				return false;
			}
		}
		else {
			return false;
		}
	}
	return true;
}

bool WinApiTools::ActivateDebugPrivilege(HANDLE threadToken, bool enablePrivilege)
{
	TOKEN_PRIVILEGES tokenPrivilege;
	LUID luid;

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		return false;
	}

	tokenPrivilege.PrivilegeCount = 1;
	tokenPrivilege.Privileges[0].Luid = luid;
	tokenPrivilege.Privileges[0].Attributes = 0;

	TOKEN_PRIVILEGES tokenPrivilegePrevious;
	DWORD oldLength = sizeof(TOKEN_PRIVILEGES);
	AdjustTokenPrivileges(threadToken, FALSE, &tokenPrivilege, sizeof(TOKEN_PRIVILEGES), &tokenPrivilegePrevious, &oldLength);

	if (GetLastError() != ERROR_SUCCESS) {
		return false;
	}

	tokenPrivilegePrevious.PrivilegeCount = 1;
	tokenPrivilegePrevious.Privileges[0].Luid = luid;

	if (enablePrivilege) {
		tokenPrivilegePrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
	}
	else {
		tokenPrivilegePrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED & tokenPrivilegePrevious.Privileges[0].Attributes);
	}

	AdjustTokenPrivileges(threadToken, FALSE, &tokenPrivilegePrevious, oldLength, NULL, NULL);
	if (GetLastError() != ERROR_SUCCESS){
		return false;
	}

	return true;
}

wchar_t* WinApiTools::DeviceFormatToDriveLetter(const wchar_t* driveInDeviceFormat, wchar_t* driveAsDriveLater)
{
	CHECK_PTR(driveInDeviceFormat);
	CHECK_PTR(driveAsDriveLater);
	G_ASSERT(wcslen(driveInDeviceFormat) != 0);

	driveAsDriveLater[0] = '\0';

	try {
#pragma warning(push)
#pragma warning(disable : 4995)
		wcscpy(driveAsDriveLater, s_drivesMap.at(driveInDeviceFormat).c_str());
#pragma warning(pop)
	}
	catch (std::out_of_range exept) {
		driveAsDriveLater[0] = '\0';
	}

	return driveAsDriveLater;
}

wchar_t* WinApiTools::DeviceFormatPathToDriveLetterPath(const wchar_t* deviceFormatPath, wchar_t* driveLetterPath)
{
	CHECK_PTR(deviceFormatPath);
	CHECK_PTR(driveLetterPath);
	G_ASSERT(wcslen(deviceFormatPath) != 0);

	driveLetterPath[0] = '\0';
	wchar_t deviceFormatDrive[50] = L"\0";
	wchar_t driveAsDriveLetter[10] = L"\0";
	const int lengthDeviceFormat = 23;

	G_ASSERT(wcslen(deviceFormatPath) >= lengthDeviceFormat);

	wcsncpy(deviceFormatDrive, deviceFormatPath, lengthDeviceFormat);
	if (wcslen(DeviceFormatToDriveLetter(deviceFormatDrive, driveAsDriveLetter)) == 0) {
		return driveLetterPath;
	}

#pragma warning(push)
#pragma warning(disable : 4995)
	wcscpy(driveLetterPath, driveAsDriveLetter);
	wcscat(driveLetterPath, &deviceFormatPath[lengthDeviceFormat]);
#pragma warning(pop)

	return driveLetterPath;
}
