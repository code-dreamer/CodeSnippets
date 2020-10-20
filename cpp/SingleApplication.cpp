#include "stdafx.h"

#include "SingleApplication.h"
#include "SingleApplicationExceptions.h"

using namespace KernelExceptions;
using namespace CoreExceptionOperators;

const QString SingleApplication::anotherInstanceWasRunnedCommand = STR("anotherInstanceWasRunned");
const int g_socketWaitTimeout = 1000;

SingleApplication::SingleApplication(int& argc, char* argv[], const QString& appUniqueKey)
	: QApplication(argc, argv)
	, uniqueKey_(appUniqueKey)
{
	sharedMemory_.setKey(uniqueKey_);
	if (sharedMemory_.attach()) {
		isRunning_ = true;
	}
	else {
		isRunning_ = false;
		// create shared memory.
		if ( !sharedMemory_.create(QSharedMemory::ReadWrite) ) {
			G_ASSERT(false);
			SingleApplicationSharedMemoryException e;
			throw e
				<< "shared memory error string = " 
				<< sharedMemory_.errorString()
				<< endl;
		}
		// create local server and listen to incoming messages from other instances.
		localServer_ = new QLocalServer(this);
		CHECKED_CONNECT(localServer_, SIGNAL(newConnection()), this, SLOT(ReceiveMessage()));
		localServer_->listen(uniqueKey_);
	}
}

void SingleApplication::ReceiveMessage()
{
	QLocalSocket* localSocket = localServer_->nextPendingConnection();
	if ( !localSocket->waitForReadyRead(g_socketWaitTimeout) ) {
		G_ASSERT(false);
		SingleApplicationSocketException e;
		throw e	
			<< "socket error string = "
			<< localSocket->errorString()
			<< endl;
	}
	QByteArray byteArray = localSocket->readAll();
	QString message = QString::fromUtf8(byteArray.constData());
	Q_EMIT MessageAvailable(message);
	localSocket->disconnectFromServer();
}

bool SingleApplication::IsRunning()
{
	return isRunning_;
}

bool SingleApplication::SendMessage(const QString& message)
{
	if (!isRunning_) {
		return false;
	}
	QLocalSocket localSocket(this);
	localSocket.connectToServer(uniqueKey_, QIODevice::WriteOnly);
	if (!localSocket.waitForConnected(g_socketWaitTimeout)) {
		G_ASSERT(false);
		SingleApplicationSocketException e;
		throw e	
			<< "socket error string = "
			<< localSocket.errorString()
			<< endl;
	}
	localSocket.write(message.toUtf8());
	if (!localSocket.waitForBytesWritten(g_socketWaitTimeout)) {
		SingleApplicationSocketException e;
		throw e	
			<< "socket error string = "
			<< localSocket.errorString()
			<< endl;
	}
	localSocket.disconnectFromServer();
	return true;
}