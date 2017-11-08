#pragma once

#include <QObject>

class HttpService : public QObject
{
public:
	HttpService();
	~HttpService();

	void MakeRequest(/*QNetworkReply* reply*/);

private slots:
	// void replyFinished(class QNetworkReply* reply);

private:
	class QNetworkAccessManager* manager;
	class QNetworkReply* reply;

};

