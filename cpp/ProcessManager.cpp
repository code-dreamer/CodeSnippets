#include "stdafx.h"
#include "ProcessManager.h"
#include "FileSystemTools.h"
#include "StringTools.h"
#include "MathTools.h"
#include "ScopeExit.h"
#include "WinApiTools.h"

const int s_processWatcherTimeout = 20;
const unsigned long s_invalidId = ((DWORD)0xFFFFFFFF);

namespace
{
void FindChilds(unsigned long rootPid, QList<unsigned long>& linkedProcesses, const QList<PROCESSENTRY32>& processesInfo)
{
	Q_FOREACH (const PROCESSENTRY32& procEntry, processesInfo) {
		if ( rootPid == procEntry.th32ParentProcessID ) {
			unsigned long childPid = procEntry.th32ProcessID;
			if ( !linkedProcesses.contains(childPid) ) {
				linkedProcesses.push_back(childPid);
				FindChilds(childPid, linkedProcesses, processesInfo);
			}
		}
	}
}

QList<PROCESSENTRY32> GetProcessList() 
{
	QList<PROCESSENTRY32> info;

	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE processSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // TH32CS_SNAPALL
	if (processSnap != INVALID_HANDLE_VALUE) {
		procEntry.dwSize = sizeof(PROCESSENTRY32);

		if (::Process32First(processSnap, &procEntry)) {
			do {
				info.push_back(procEntry);
			} while (::Process32Next(processSnap, &procEntry));
		}

		::CloseHandle(processSnap);
	}

	return info;
}

QString GetProcessPath(HANDLE processHandle)
{
	CHECK_PTR(processHandle);

	QString processPath;
	wchar_t buff[MAX_PATH];
	DWORD res = ::GetProcessImageFileName(processHandle, buff, MAX_PATH);
	if (res != 0) {
		QString deviceFormatPah = StringTools::CreateQString(buff);
		std::wstring deviceFormatPahStd = StringTools::CreateStdWString(deviceFormatPah);
		WinApiTools::DeviceFormatPathToDriveLetterPath(deviceFormatPahStd.c_str(), buff);
		processPath = StringTools::CreateQString(buff);
	}

	QSTRING_NOT_EMPTY(processPath);
	return processPath;
}

unsigned long GetProcessExitCode(HANDLE processHandle)
{
	CHECK_PTR(processHandle);

	unsigned long result = s_invalidId;
	unsigned long exitCode;
	if ( ::GetExitCodeProcess(processHandle, &exitCode) == TRUE ) {
		result = exitCode;
	}
	
	return result;
}

QList<unsigned long> GetLinkedProcessesIds(const QList<PROCESS_INFORMATION>& monitoringProcesses)
{
	QList<unsigned long> linkedProcesses;

	const QList<PROCESSENTRY32> processEntries = GetProcessList();

	Q_FOREACH(const PROCESS_INFORMATION& processInfo, monitoringProcesses) {
		FindChilds(processInfo.dwProcessId, linkedProcesses, processEntries);
	}

	return linkedProcesses;
}
} // namespace

ProcessManager::ProcessManager(QObject* parent)
	: QObject(parent)
	, processesWatcher_(nullptr)
{
	processesWatcher_ = new QTimer(this);
	CHECKED_CONNECT(processesWatcher_, SIGNAL(timeout()), this, SLOT(WatchProcesses()));
}

ProcessManager::~ProcessManager()
{
	if (processesWatcher_->isActive()) {
		processesWatcher_->stop();
	}

	for (int i = 0, count = monitoringProcesses_.count(); i < count; ++i) {
		::CloseHandle(monitoringProcesses_.at(i).hProcess);
	}
}

bool ProcessManager::StartProcess(const QString& filePath, const QStringList& arguments)
{
	QSTRING_NOT_EMPTY(filePath);
	G_ASSERT( FileSystemTools::IsExistingPathNative(filePath) );
	G_ASSERT( monitoringProcesses_.isEmpty() );
	G_ASSERT( !processesWatcher_->isActive() );

	monitoringProcesses_.clear();
	processesWatcher_->stop();

	QString workingDir = FileSystemTools::GetFileDir(filePath);
	qint64 pid = -1;
	bool started = QProcess::startDetached(filePath, arguments, workingDir, &pid);
	if (started && pid != -1) {
		Q_EMIT ProcessStarted(filePath);

		StartProcessMonitoring(MathTools::SaturationCast<unsigned long, qint64>(pid));
		processesWatcher_->start(s_processWatcherTimeout);
	}

	return started;
}


void ProcessManager::StartProcessMonitoring(unsigned long pid)
{
	HANDLE processHandle = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
	CHECK_PTR(processHandle);
	if (processHandle != nullptr) {
		PROCESS_INFORMATION processInfo;
		memset(&processInfo, 0, sizeof(processInfo));
		processInfo.dwProcessId = pid;
		processInfo.hProcess = processHandle;

		monitoringProcesses_.push_back(processInfo);
	}
}

void ProcessManager::WatchProcesses()
{
	G_ASSERT( !monitoringProcesses_.isEmpty() );

	for (auto it = monitoringProcesses_.begin(); it != monitoringProcesses_.end(); ++it) {
		HANDLE processHandle = (*it).hProcess;
		CHECK_PTR(processHandle);

		unsigned long exitCode = GetProcessExitCode(processHandle);
		G_ASSERT(exitCode != s_invalidId);
		if (exitCode != s_invalidId && exitCode != STILL_ACTIVE) {
			QString processPath = GetProcessPath(processHandle);
			::CloseHandle(processHandle);

			Q_EMIT ProcessFinished(processPath, exitCode == EXIT_SUCCESS);

			it = monitoringProcesses_.erase(it);
			if (it == monitoringProcesses_.end()) {
				--it;
			}
		}
	}

	if (monitoringProcesses_.isEmpty()) {
		processesWatcher_->stop();
		Q_EMIT AllProcessesFinished();
		return;
	}

	QList<unsigned long> linkedProcesses = GetLinkedProcessesIds(monitoringProcesses_);
	Q_FOREACH(unsigned long pid, linkedProcesses) {
		auto it = std::find_if(monitoringProcesses_.constBegin(), monitoringProcesses_.constEnd(), [&](const PROCESS_INFORMATION& processInfo) {
			return (processInfo.dwProcessId == pid);
		});

		if (it == monitoringProcesses_.constEnd()) {
			StartProcessMonitoring(pid);
		}
	}
}
