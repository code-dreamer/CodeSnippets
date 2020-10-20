#pragma once

namespace WinApiTools
{

KERNEL_API int ShowWinMessageBox(const QWidget* parent, const QString& caption, const QString& text, DWORD flags);
KERNEL_API QMessageBox::StandardButton ShowMessageBox(const QWidget* parent, const QString& caption, const QString& text, QMessageBox::Icon icon, const QMessageBox::StandardButtons& buttons = QMessageBox::Ok);

KERNEL_API QString ShowFolderDialog(const QString& title, const QWidget* owner = nullptr);
KERNEL_API QString FormatWinError(DWORD errorCode = ::GetLastError());
KERNEL_API QString GetRegKeyValue(const QString fullRegKey);

KERNEL_API QString GetProgramFilesDir();
KERNEL_API QString GetStandartWindowsGamesDir();
KERNEL_API void GetListFullFilePathProcess(std::vector < std::pair < std::wstring, DWORD >> *processes);
KERNEL_API bool OpenCurrentThreadToken(HANDLE& token);
KERNEL_API bool ActivateDebugPrivilege(HANDLE threadToken, bool enablePrivilege);

KERNEL_API wchar_t* DeviceFormatToDriveLetter(const wchar_t* driveInDeviceFormat, wchar_t* driveAsDriveLater);
KERNEL_API wchar_t* DeviceFormatPathToDriveLetterPath(const wchar_t* deviceFormatPath, wchar_t* driveLetterPath);

}
