#include "stdafx.h"
#include "ProcessTools.h"
#include "StringTools.h"

QString ProcessTools::GetProcessName(const DWORD pid) {
	std::wstring procName;

	HANDLE procHandle = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (procHandle != nullptr) {
		wchar_t buff[MAX_PATH];

		::GetProcessImageFileName(procHandle, buff, MAX_PATH);

		procName = buff;
		size_t index = procName.find_last_of(L"\\");
		if (index != std::string::npos) {
			procName.erase(0, ++index);
		}

		::CloseHandle(procHandle);
	}

	return StringTools::CreateQString(procName);
}

int ProcessTools::GetProcessId(const QString& processName) {
	int pid = -1;

	PROCESSENTRY32 processEntry;

	HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (hSnap != INVALID_HANDLE_VALUE) {
		processEntry.dwSize = sizeof(PROCESSENTRY32);

		if (::Process32First(hSnap, &processEntry)) {
			const std::wstring procNameStd = StringTools::CreateStdWString(processName);
			do {
				if (_wcsicmp(processEntry.szExeFile, procNameStd.c_str()) == 0) {
					pid = processEntry.th32ProcessID;
					break;
				}
			} while (::Process32Next(hSnap, &processEntry));
		}
	}

	return pid;
}

bool ProcessTools::IsProcessExist(const QString& procName) {
	return (GetProcessId(procName) != -1);
}
