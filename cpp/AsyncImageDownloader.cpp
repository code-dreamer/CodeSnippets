#include "stdafx.h"
#include "AsyncImageDownloader.h"
#include "QtTools.h"

AsyncImageDownloader::AsyncImageDownloader(QObject* const parent)
	: QObject(parent)
{
	networkManager_ = new QNetworkAccessManager();
	CHECKED_CONNECT(networkManager_, SIGNAL(finished(QNetworkReply*)), this, SLOT(OnReplyFinished(QNetworkReply*)));
}

AsyncImageDownloader::~AsyncImageDownloader()
{
	networkManager_->deleteLater();
}

void AsyncImageDownloader::AsyncDownloadImage(const QUrl& url)
{
	G_ASSERT(url.isValid());

	QNetworkRequest request;
	request.setUrl(url);
	QNetworkReply* const reply = networkManager_->get(request);
	waitingRequests_.push_back(reply);
}

void AsyncImageDownloader::OnReplyFinished(QNetworkReply* const reply)
{
	CHECK_PTR(reply);

	std::unique_ptr<QNetworkReply, QtTools::QueuedDeleter> replyDeleter(reply);

	if (waitingRequests_.contains(reply)) {
		QVariant redirectedUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
		QUrl redirectedTo = redirectedUrl.toUrl();
		if (redirectedTo.isValid())	{
			// guard from infinite redirect loop
			if (redirectedTo != reply->request().url())	{
				AsyncDownloadImage(redirectedTo);
			}
			else {
				qWarning() << STR("[UrlDownloader] Infinite redirect loop at ") + redirectedTo.toString();
			}
		}
		else
		{
			if (reply->error() == QNetworkReply::NoError) {
				QImage image;
				image.loadFromData(reply->readAll());
				Q_EMIT ImageReady(reply->url(), image);
			}
			else {
				qWarning() << QString(STR("[UrlDownloader] Reply error: %1")).arg(reply->errorString());
			}
		}
		waitingRequests_.removeOne(reply);
	}
}

bool AsyncImageDownloader::IsAsyncDownloadInProcess(const QUrl& url) const
{
	bool processed = false;

	Q_FOREACH(const QNetworkReply* const reply, waitingRequests_) {
		if (reply->url() == url) {
			processed = true;
			break;
		}
	}

	return processed;
}
