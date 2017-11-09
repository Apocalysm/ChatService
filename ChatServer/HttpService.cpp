#include "HttpService.h"

#include <iostream>

#include <QByteArray>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QAuthenticator>
#include <QSslSocket>

HttpService::HttpService() :
	manager(new QNetworkAccessManager(this))
{
	connect(manager, &QNetworkAccessManager::finished, this, &HttpService::replyFinished);
	//connect(manager, &QNetworkAccessManager::sslErrors, this, &HttpService::ignoreSslError);
	//connect(manager, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(ignoreSslError(const QList<QSslError>&)));
}


HttpService::~HttpService()
{
}

void HttpService::MakeRequest(/*QNetworkReply * reply*/)
{
	manager->get(QNetworkRequest(QUrl("http://preev.com/")));
}

void HttpService::ignoreSslError(const QList<QSslError>& errors)
{
	int hej = 5;
}


void HttpService::replyFinished(QNetworkReply* res)
{
	std::cout << "It finished!" << std::endl;
	QByteArray data = res->readAll();
	for (int i = 0; i < data.length(); i++)
		std::cout << data[i] << std::endl;
}
