#include "analytics.h"

Analytics::Analytics() {

    // tracker for google analytics
    trackerId = "UA-118159593-1";
    // Get hostname so we can set it as a parameter.
    hostname = QHostInfo::localHostName() + "." + QHostInfo::localDomainName();
    // Generate a unique ID to use as the Client ID.
    uuid = QUuid::createUuid().toString();
    // Create an http network request.
    req = QNetworkRequest(QUrl("http://www.google-analytics.com/collect"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
}

QUrlQuery Analytics::intialSetup()
{
    // Build up the query parameters.
    QUrlQuery query;
    query.addQueryItem("v", "1"); // Version
    query.addQueryItem("tid", trackerId); // Tracking ID
    query.addQueryItem("cid", uuid); // Client ID
    return query;
}

void Analytics::hitPageView(QString page, QString title)
{
    QUrlQuery query = intialSetup(); // Get query

    query.addQueryItem("t", "pageview"); // Page view hit type
    query.addQueryItem("dh", hostname); // Document hostname
    query.addQueryItem("dp", page); // Page
    query.addQueryItem("dt", title); // Title

    httpPost(query); // POST
}

void Analytics::hitEvent(QString category, QString action, int value)
{
    QUrlQuery query = intialSetup(); // Get query

    query.addQueryItem("t", "event"); // Event hit type
    query.addQueryItem("ec", category); // Event category
    query.addQueryItem("ea", action); // Event action
    if(value>0) query.addQueryItem("ev", QString::number(value)); // Event value

    httpPost(query); // POST
}

void Analytics::httpPost(QUrlQuery query) {

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QByteArray data;
    data.append(query.query());
    qDebug() << data; // Output for debug purposes.
    QNetworkReply *reply = manager->post(req, data);

}