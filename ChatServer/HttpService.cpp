#include "HttpService.h"

#include <QNetworkReply>
#include <QNetworkAccessManager>

HttpService::HttpService()
{
}


HttpService::~HttpService()
{
}

void HttpService::MakeRequest(/*QNetworkReply * reply*/)
{
	manager = new QNetworkAccessManager();

	reply = manager->get(QNetworkRequest(QUrl("http://preeve.com/")));
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

}
/*
void HttpService::replyFinished(QNetworkReply* reply)
{
	int hej = 5;
}
*/