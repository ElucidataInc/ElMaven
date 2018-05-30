#include "analytics.h"

Analytics::Analytics() {

    // tracker for google analytics
    trackerID = "UA-118159593-1";
    // Get hostname so we can set it as a parameter.
    hostname = QHostInfo::localHostName() + "." + QHostInfo::localDomainName();
    // Generate a unique ID to use as the Client ID.
    clientID = getClientID();
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
    query.addQueryItem("tid", trackerID); // Tracking ID
    query.addQueryItem("cid", clientID); // Client ID
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
    query.addQueryItem("t", "event"); // Hit event
    query.addQueryItem("sc", "start"); // Session start
    query.addQueryItem("ec", "session"); // Event category
    query.addQueryItem("ea", "start"); // Event action
    httpPost(query); // POST
}

void Analytics::sessionEnd() {

    QUrlQuery query = intialSetup(); // Get query
    query.addQueryItem("t", "event"); // Hit event
    query.addQueryItem("sc", "end"); // Session end
    query.addQueryItem("ec", "session"); // Event category
    query.addQueryItem("ea", "end"); // Event action
    httpPost(query); // POST
}

void Analytics::httpPost(QUrlQuery query) {

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QByteArray data;
    data.append(query.query());
    qDebug() << data; // Output for debug purposes.
    QNetworkReply *reply = manager->post(req, data);

}

QString Analytics::getClientID()
{
    QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    settingsPath = QDir::cleanPath(settingsPath + QDir::separator() + "El-MAVEN_analytics.ini");

    QSettings settings(settingsPath, QSettings::IniFormat);
    QString clientID;
    if (!settings.contains("analytics-cid"))
    {
        clientID = QUuid::createUuid().toString();
        settings.setValue("analytics-cid", clientID);
    }
    else
    {
        clientID = settings.value("analytics-cid").toString();
    }

    return clientID;
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
    return QString("Macintosh;" + QSysInfo::productVersion());
    #elif defined(Q_OS_WIN)
    return QString("Windows;" + QSysInfo::productVersion());
    #elif defined(Q_OS_LINUX)
    return QString("Linux;" + QSysInfo::productVersion());
    #elif defined(Q_OS_UNIX)
    return QString("Unix;" + QSysInfo::productVersion());
    #else
    return QString("Unknown; ");
    #endif
}
