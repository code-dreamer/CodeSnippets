#include "stdafx.h"
#include "SystemInfoTools.h"
#include "StringTools.h"
#include "FileSystemTools.h"

namespace
{

enum SystemDirType
{
	  ProgramFilesx86 = 0
	, ProgramFilesx64
	, SystemDirTypeCount = 3
};

QString GetDirPathOldImpl(SystemDirType type)
{
	QString path;

	int folderType = -1;
	switch (type) {
	case ProgramFilesx86:
	case ProgramFilesx64:
		folderType = CSIDL_PROGRAM_FILES;
		break;

	case SystemDirTypeCount:
	default:
		G_ASSERT(false);
	}

	if (folderType != -1) {
		wchar_t rawPath[MAX_PATH] = L"\0";
		HRESULT result = SHGetFolderPath(nullptr, folderType, nullptr, SHGFP_TYPE_CURRENT, rawPath);
		if (result == S_OK) {
			path = StringTools::CreateQString(rawPath);
		}
	}

	return path;
}

QString GetDirPathNewImpl(SystemDirType type)
{
	QString path;

	const KNOWNFOLDERID* folderType = nullptr;
	switch (type) {
	case ProgramFilesx86:
	case ProgramFilesx64:
		folderType = &FOLDERID_ProgramFiles;
		break;

	case SystemDirTypeCount:
	default:
		G_ASSERT(false);
	}

	if (folderType != nullptr) {
		typedef BOOL (WINAPI* SHGetKnownFolderPathType) (REFKNOWNFOLDERID, DWORD, HANDLE, PWSTR*);
#pragma warning(push)
#pragma warning(disable: 4191)
		SHGetKnownFolderPathType SHGetKnownFolderPathPtr = reinterpret_cast<SHGetKnownFolderPathType>(::GetProcAddress(::GetModuleHandle(L"Shell32"), "SHGetKnownFolderPath"));
#pragma warning(pop)
		if (SHGetKnownFolderPathPtr != nullptr) {
			PWSTR* rawPath = static_cast<PWSTR*>(::CoTaskMemAlloc(MAX_PATH));
			HRESULT result = SHGetKnownFolderPathPtr(*folderType, 0x00001000/*KF_FLAG_NO_ALIAS*/, nullptr, rawPath);
			if (result == S_OK) {
				path = StringTools::CreateQString(*rawPath);
			}
			::CoTaskMemFree(rawPath);
		}
		else {
			path = GetDirPathOldImpl(type);
		}
	}

	return path;
}

QString GetDirPathImpl(SystemDirType type)
{
	return (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA) ? GetDirPathNewImpl(type) : GetDirPathOldImpl(type);
}

} // namespace

QString SystemInfoTools::GetProgramFilesPath(bool x64)
{
	QString path;

	if (x64 && GetSystemBitness() == x86BitType) {
		return path;
	}

	path = GetDirPathImpl(x64 ? ProgramFilesx64 : ProgramFilesx86);

	if (x64) {
		path.remove(STR(" (x86)"));
		if ( !FileSystemTools::IsPathExist(path) ) {
			path = EMPTY_STR;
		}
	}

	return path;
}

QString SystemInfoTools::GetWindowsGamesPath()
{
	const QString programFilesDir = GetProgramFilesPath( Is64BitOS() );
	// TODO: what if GetProgramFilesDir failed and returns empty string?
	return FileSystemTools::MergePath( programFilesDir, STR("\\Microsoft Games") );
}

SystemInfoTools::SystemBitType SystemInfoTools::GetSystemBitness()
{
	SystemBitType type = UnknownBitness;

	SYSTEM_INFO sysInfo;
	::GetNativeSystemInfo(&sysInfo);

	std::string bitness;
	switch (sysInfo.wProcessorArchitecture) {
	case PROCESSOR_ARCHITECTURE_AMD64:
	case PROCESSOR_ARCHITECTURE_IA64:
		type = x64BitType;
		break;

	case PROCESSOR_ARCHITECTURE_INTEL:
		type = x86BitType;
		break;

	case PROCESSOR_ARCHITECTURE_UNKNOWN:
	default:
		G_ASSERT(false);
	}

	return type;
}

bool SystemInfoTools::Is64BitOS()
{
	return GetSystemBitness() == x64BitType;
}

bool SystemInfoTools::Is32BitOS()
{
	return GetSystemBitness() == x86BitType;
}

QString SystemInfoTools::GetWindowsDirectory()
{
	QVarLengthArray<wchar_t, MAX_PATH> fullPath;

	UINT retLen = ::GetWindowsDirectory(fullPath.data(), MAX_PATH);
	if (retLen > MAX_PATH) {
		fullPath.resize(retLen);
		retLen = ::GetWindowsDirectory(fullPath.data(), retLen);
	}

	return retLen == 0 ? STR("C:\\Windows") : StringTools::CreateQString(fullPath.constData(), (int)retLen);
}

QString SystemInfoTools::GetSystemDirectory()
{
	QVarLengthArray<wchar_t, MAX_PATH> fullPath;

	UINT retLen = ::GetSystemDirectory(fullPath.data(), MAX_PATH);
	if (retLen > MAX_PATH) {
		fullPath.resize(retLen);
		retLen = ::GetSystemDirectory(fullPath.data(), retLen);
	}

	return retLen == 0 ? STR("C:\\Windows\\System32") : StringTools::CreateQString(fullPath.constData(), (int)retLen);
}
