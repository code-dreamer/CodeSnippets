#pragma once

class KERNEL_API UrlDownloader : public QObject
{
	Q_OBJECT

public:
	UrlDownloader(QObject* parent = nullptr);
	virtual ~UrlDownloader();

public:
	QImage DownloadImage(const QUrl& url);
	QByteArray DownloadFile(const QUrl& url);

private:
	QNetworkReply* DownloadUrl(const QUrl& url);

private:
	QNetworkAccessManager* networkManager_;
	QEventLoop* loop_;
};
