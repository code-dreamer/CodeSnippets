#pragma once

class KERNEL_API ProcessManager : public QObject
{
	Q_OBJECT

public:
	ProcessManager(QObject* parent = nullptr);
	virtual ~ProcessManager();

public:
	bool StartProcess(const QString& filePath, const QStringList& arguments = QStringList());

Q_SIGNALS:
	void ProcessStarted(QString processPath);
	void ProcessFinished(QString processPath, bool normalExit);
	void AllProcessesFinished();

private Q_SLOTS:
	void WatchProcesses();

private:
	void StartProcessMonitoring(unsigned long pid);

private:
	QTimer* processesWatcher_;

#pragma warning(push)
#pragma warning(disable : 4251)
private:
	QList<PROCESS_INFORMATION> monitoringProcesses_;
#pragma warning(pop)
};
