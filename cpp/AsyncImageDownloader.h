#pragma once

class KERNEL_API AsyncImageDownloader : public QObject
{
	Q_OBJECT

public:
	AsyncImageDownloader(QObject* parent = nullptr);
	virtual ~AsyncImageDownloader();

Q_SIGNALS:
	void ImageReady(QUrl url, QImage image);

public:
	QImage DownloadImage(const QUrl& url);
	void AsyncDownloadImage(const QUrl& url);

	bool IsAsyncDownloadInProcess(const QUrl& url) const;

private Q_SLOTS:
	void OnReplyFinished(QNetworkReply* reply);

private:
	QNetworkAccessManager* networkManager_;

#pragma warning(push)
#pragma warning(disable : 4251)	
private:
	QList<QNetworkReply*> waitingRequests_;
#pragma warning(pop)
};
