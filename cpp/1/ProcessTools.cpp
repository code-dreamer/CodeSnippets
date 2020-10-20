#include "stdafx.h"
#include "ProcessTools.h"

namespace ProcessTools {
;

boost::optional<PROCESSENTRY32> FindProcess(std::function<bool(const PROCESSENTRY32& processEntry)> pred)
{
	PROCESSENTRY32 processEntry;

	HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
		return boost::none;

	auto guard = wxMakeGuard([&]() {
		::CloseHandle(hSnap);
	});

	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if (::Process32First(hSnap, &processEntry)) {
		do {
			if (pred(processEntry))
				return processEntry;
				
		} while (::Process32Next(hSnap, &processEntry));
	}

	return boost::none;
}

std::vector<PROCESSENTRY32> FindProcesses(std::function<bool(const PROCESSENTRY32& processEntry)> pred)
{
	std::vector<PROCESSENTRY32> result;

	HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
		return result;

	auto guard = wxMakeGuard([&]() {
		::CloseHandle(hSnap);
	});

	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if (::Process32First(hSnap, &processEntry)) {
		do {
			if (pred(processEntry)) {
				result.push_back(processEntry);
			}
		} while (::Process32Next(hSnap, &processEntry));
	}

	return result;
}

size_t GetProcessId(const wxString& processName)
{
	boost::optional<PROCESSENTRY32> process = FindProcess([processName](const PROCESSENTRY32& processEntry) {
		return (_wcsicmp(processEntry.szExeFile, processName.wc_str()) == 0);
	});
	
	return process ? process->th32ProcessID : 0;
}

bool IsProcessExist(const wxString& processName)
{
	return (GetProcessId(processName) != 0);
}

bool IsProcessExist(size_t pid)
{
	/*
	boost::optional<PROCESSENTRY32> process = FindProcess([pid](const PROCESSENTRY32& processEntry) {
		return (processEntry.th32ProcessID == pid);
	});

	return process;*/
	return wxProcess::Exists((int)pid);
}

std::vector<size_t> GetProcessIds(const wxString& processName)
{
	std::vector<PROCESSENTRY32> procs = FindProcesses([processName](const PROCESSENTRY32& processEntry) {
		return (_wcsicmp(processEntry.szExeFile, processName.wc_str()) == 0);
	});

	std::vector<size_t> pids{procs.size()};
	std::transform(procs.begin(), procs.end(), pids.begin(), [](const PROCESSENTRY32& proceEntry) {
		return proceEntry.th32ProcessID;
	});

	return pids;

	/*
	PROCESSENTRY32 processEntry;

	HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
		return pids;

	auto guard = wxMakeGuard([&]() {
		::CloseHandle(hSnap);
	});

	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if (::Process32First(hSnap, &processEntry)) {
		//const std::wstring procNameStd = processName.wc_str();
		do {
			if (_wcsicmp(processEntry.szExeFile, processName.wc_str()) == 0) {
				pids.push_back(processEntry.th32ProcessID);
			}
		} while (::Process32Next(hSnap, &processEntry));
	}

	return pids;*/
}

bool WaitForProcessStart(size_t pid, size_t maxWaitingMs, size_t waitStep)
{
	const size_t waitDeltaMs = waitStep;
	size_t totalWaitMs = 0;
	while (!ProcessTools::IsProcessExist(pid)) {
		wxMilliSleep(waitDeltaMs);
		if ((totalWaitMs += waitDeltaMs) > maxWaitingMs)
			return false;
	}

	return true;
}

bool WaitForProcessEnd(size_t pid, size_t maxWaitingMs, size_t waitStep)
{
	const size_t waitDeltaMs = waitStep;
	size_t totalWaitMs = 0;
	while (ProcessTools::IsProcessExist(pid)) {
		wxMilliSleep(waitDeltaMs);
		if ((totalWaitMs += waitDeltaMs) > maxWaitingMs)
			return false;
	}

	return true;
}

size_t GetCurrentProcessId()
{
	return wxGetProcessId();
}

size_t GetProcessIdWithImagePath(const wxString& imageFullPath)
{
	wxFileName filename{imageFullPath};
	if (!filename.IsAbsolute()) {
		FAIL;
		return 0;
	}

	wxString fullname = filename.GetFullName();
	std::vector<PROCESSENTRY32> procs = FindProcesses([&](const PROCESSENTRY32& processEntry) {
		return (_wcsicmp(processEntry.szExeFile, fullname.wc_str()) == 0);
	});

	DWORD result = 0;
	for (const PROCESSENTRY32& procEntry : procs) {
		DWORD currPid = procEntry.th32ProcessID;
		HANDLE prochHandle = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, currPid);
		if (!prochHandle)
			continue;

		auto guard = wxMakeGuard([&]() {
			::CloseHandle(prochHandle);
		});

		wchar_t buff[MAX_PATH] = {};
		DWORD buffLen = GetModuleFileNameEx(prochHandle, 0, buff, MAX_PATH);
		if (buffLen > 0) {
			wxString currImageFullPath{buff, buffLen};
			if (currImageFullPath.CmpNoCase(imageFullPath) == 0) {
				result = currPid;
				break;
			}
		}
	}

	return result;
}

} // namespace ProcessTools
