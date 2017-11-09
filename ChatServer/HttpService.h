#pragma once

#include <QObject>
#include <QList>
#include <QSslError>

class HttpService : public QObject
{
public:
	HttpService();
	~HttpService();

	void MakeRequest(/*QNetworkReply* reply*/);

private slots:
	void replyFinished(class QNetworkReply*);
	void ignoreSslError(const QList<QSslError>&);

private:
	class QNetworkAccessManager* manager;

};

