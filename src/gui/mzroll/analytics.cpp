#include "analytics.h"

Analytics::Analytics() {

    // tracker for google analytics
    trackerId = "UA-118159593-1";
    // Get hostname so we can set it as a parameter.
    hostname = QHostInfo::localHostName() + "." + QHostInfo::localDomainName();
    // Generate a unique ID to use as the Client ID.
    uuid = QUuid::createUuid().toString();
    // Get language
    language = QLocale::system().name().toLower().replace("_", "-");
    // Create an http network request.
    req = QNetworkRequest(QUrl("http://www.google-analytics.com/collect"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::UserAgentHeader, getUserAgent());
}

QUrlQuery Analytics::intialSetup()
{
    // Build up the query parameters.
    QUrlQuery query;
    query.addQueryItem("v", "1"); // Version
    query.addQueryItem("tid", trackerId); // Tracking ID
    query.addQueryItem("cid", uuid); // Client ID
    query.addQueryItem("ul", language); // Language
    query.addQueryItem("an", qApp->applicationName()); // Application Name
    query.addQueryItem("av", qApp->applicationVersion()); // Application Version
    return query;
}

void Analytics::hitScreenView(QString screenName)
{
    QUrlQuery query = intialSetup(); // Get query

    query.addQueryItem("t", "screenview"); // Screen view hit type
    query.addQueryItem("cd", screenName); // Screen Name

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

void Analytics::sessionStart() {

    QUrlQuery query = intialSetup(); // Get query
    query.addQueryItem("sc", "end"); // Session start
    httpPost(query); // POST
}

void Analytics::sessionEnd() {

    QUrlQuery query = intialSetup(); // Get query
    query.addQueryItem("sc", "end"); // Session end
    httpPost(query); // POST
}

void Analytics::httpPost(QUrlQuery query) {

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QByteArray data;
    data.append(query.query());
    qDebug() << data; // Output for debug purposes.
    QNetworkReply *reply = manager->post(req, data);

}

QString Analytics::getUserAgent()
{
    QString locale = QLocale::system().name();
    QString operatingSystem = osName();

    return QString("%1/%2 (%3; %4) GAnalytics/1.0 (Qt/%5)").arg(qApp->applicationName()).arg(qApp->applicationVersion()).arg(operatingSystem).arg(locale).arg(QT_VERSION_STR);
}


QString Analytics::osName()
{
    #if defined(Q_OS_MACOS)
    return QLatin1String("macos");
    #elif defined(Q_OS_WIN)
    return QLatin1String("windows");
    #elif defined(Q_OS_LINUX)
    return QLatin1String("linux");
    #elif defined(Q_OS_UNIX)
    return QLatin1String("unix");
    #else
    return QLatin1String("unknown");
    #endif
}
