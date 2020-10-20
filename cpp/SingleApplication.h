#pragma once

class KERNEL_API SingleApplication : public QApplication
{
	Q_OBJECT

public:
	static const QString anotherInstanceWasRunnedCommand;

public:
	SingleApplication(int& argc, char* argv[], const QString& appUniqueKey);

public:
	bool IsRunning();
	bool SendMessage(const QString& message);

Q_SIGNALS:
	void MessageAvailable(QString message);

private Q_SLOTS:
	void ReceiveMessage();

private:
	bool isRunning_;
	QString uniqueKey_;
	QSharedMemory sharedMemory_;
	QLocalServer* localServer_;
};