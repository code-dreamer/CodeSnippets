#pragma once

namespace ProcessTools {
;
size_t GetCurrentProcessId();
size_t GetProcessId(const wxString& processName);
size_t GetProcessIdWithImagePath(const wxString& imageFullPath);
std::vector<size_t> GetProcessIds(const wxString& processName);
bool IsProcessExist(const wxString& processName);
bool IsProcessExist(size_t pid);

const size_t kWaitStep = 200;

bool WaitForProcessStart(size_t pid, size_t maxWaitingMs, size_t waitStep = kWaitStep);
bool WaitForProcessEnd(size_t pid, size_t maxWaitingMs, size_t waitStep = kWaitStep);
} // namespace ProcessTools
