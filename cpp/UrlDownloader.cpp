#include "stdafx.h"
#include "UrlDownloader.h"
#include "NetworkReplyException.h"
#include "QtTools.h"

UrlDownloader::UrlDownloader(QObject* const parent)
	: QObject(parent)
{
	networkManager_ = new QNetworkAccessManager();
	loop_ = new QEventLoop();
	CHECKED_CONNECT(networkManager_, SIGNAL(finished(QNetworkReply*)), loop_, SLOT(quit()));
}

UrlDownloader::~UrlDownloader()
{
	networkManager_->deleteLater();
	loop_->deleteLater();
}

QImage UrlDownloader::DownloadImage(const QUrl& url)
{
	G_ASSERT(url.isValid());

	QImage result;

	QNetworkReply* const replyPtr = DownloadUrl(url);
	std::unique_ptr<QNetworkReply, QtTools::QueuedDeleter> reply(replyPtr);

	if (reply->error() != QNetworkReply::NoError) {
		throw KernelExceptions::NetworkReplyException(reply.get());
	}

	if ( !result.loadFromData(reply->readAll()) ) {
		throw KernelExceptions::KernelException(tr("Can't parse image from url %1").arg(url.toString()));
	}

	return result;
}

QByteArray UrlDownloader::DownloadFile(const QUrl& url)
{
	G_ASSERT(url.isValid());

	QByteArray result;

	QNetworkReply* const replyPtr = DownloadUrl(url);
	std::unique_ptr<QNetworkReply, QtTools::QueuedDeleter> reply(replyPtr);

	result = reply->readAll();

	G_ASSERT( !result.isEmpty() );
	return result;
}

QNetworkReply* UrlDownloader::DownloadUrl(const QUrl& url)
{	
	G_ASSERT(url.isValid());

	QNetworkRequest request(url);
	QNetworkReply* const reply = networkManager_->get(request);

	loop_->exec();

	const QVariant redirectedUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	const QUrl redirectedTo = redirectedUrl.toUrl();
	if (redirectedTo.isValid()) {
		// guard from infinite redirect loop
		if (redirectedTo != reply->request().url()) {
			return DownloadUrl(redirectedTo);
		}
		else {
			qWarning() << STR("[UrlDownloader] Infinite redirect loop at " )+ redirectedTo.toString();
			G_ASSERT(false);
		}
	}

	CHECK_PTR(reply);
	return reply;
}