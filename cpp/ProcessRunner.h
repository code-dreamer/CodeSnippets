#pragma once

class GameStateInterface;

class KERNEL_API ProcessRunner : public QObject
{
	Q_OBJECT

public:
	enum ExitResult
	{
		UndefinedExit,
		NormalExit,
		FailedExit
	};

public:
	ProcessRunner(QObject* parent = nullptr, QString path = QString());
	virtual ~ProcessRunner();

Q_SIGNALS:
	void ProcessStarted();
	void ProcessAndChildsFinished(ProcessRunner::ExitResult);
	void GameProcessDetected();

public:
	void RunInstaller(const QString& exePath);
	void RunGame(const QString& exePath);

private:
	void timerEvent(QTimerEvent*);
	void RunInstallerExe(const QString& exeName);
	void RunInstallerMsi(const QString& exeName);

private:
	STARTUPINFO startUpInfo_;
	PROCESS_INFORMATION processInfo_;
	int timerId_;
	int timerIdGameMonitor_;
	HANDLE gameProcessHandle_;
#pragma warning(push)
#pragma warning(disable : 4251)
	std::wstring pathToGame_;
#pragma warning(pop)
};

