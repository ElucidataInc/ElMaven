#define _STR(X) #X
#define STR(X) _STR(X)

#include <ctime>

#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "mixpanel.h"
#ifdef __OSX_AVAILABLE
#ifndef DEBUG
#include "sentry.h"
#endif
#endif

Mixpanel::Mixpanel()
{
    QString configLocation = QStandardPaths::writableLocation(
        QStandardPaths::ConfigLocation);
    auto gaSettingsPath = QDir::cleanPath(configLocation
                                          + QDir::separator()
                                          + "El-MAVEN_analytics.ini");
    auto gaSettings = new QSettings(gaSettingsPath, QSettings::IniFormat);
    auto settingsPath = QDir::cleanPath(configLocation
                                        + QDir::separator()
                                        + "ElMaven"
                                        + QDir::separator()
                                        + "mixpanel.ini");
    _settings = new QSettings(settingsPath, QSettings::IniFormat);
    if (!_settings->contains("mixpanel-uuid")) {
        // use an existing Google Analytics ID if available
        if (gaSettings->contains("analytics-cid")) {
            _clientId = gaSettings->value("analytics-cid").toString();
        } else {
            _clientId = QUuid::createUuid().toString();
        }
        _settings->setValue("mixpanel-uuid", _clientId);
        _isFirstSession = true;
        updateUser("Name", "");
        updateUser("Email", "");
    } else {
        _clientId = _settings->value("mixpanel-uuid").toString();
        _isFirstSession = false;
    }

    _authToken = QString(STR(MIXPANEL_TOKEN));
    _eventRequest = QNetworkRequest(QUrl("https://api.mixpanel.com/track"));
    _eventRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                            "application/x-www-form-urlencoded");
    _userRequest = QNetworkRequest(QUrl("https://api.mixpanel.com/engage"));
    _userRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                           "application/x-www-form-urlencoded");

#ifdef __OSX_AVAILABLE
#ifndef DEBUG
    // this ensures our crash reports for this session will be associated with
    // the client ID for a given system
    sentry_set_extra("Client ID",
                     sentry_value_new_string(_clientId.toStdString().c_str()));
#endif
#endif

    QMap<QString, QVariant> properties;
    properties["First session"] = _isFirstSession;
    trackEvent("EM Session Start", properties);
}

Mixpanel::~Mixpanel()
{
    trackEvent("EM Session End", QMap<QString, QVariant>(), true);
}

void Mixpanel::trackEvent(const QString& event,
                          QMap<QString, QVariant> properties,
                          bool waitForFinish) const
{
    properties["token"] = _authToken;
    properties["distinct_id"] = _clientId;
    properties["time"] = _getUnixTime();
    properties["Version"] = qApp->applicationVersion();
    QJsonObject eventData
    {
        {"event", event},
        {"properties", QJsonObject::fromVariantMap(properties)}
    };
    _httpRequest(QJsonDocument(eventData).toJson().toBase64(),
                 true,
                 waitForFinish);
}

void Mixpanel::updateUser(const QString& attribute, const QVariant& value) const
{
    QMap<QString, QVariant> details;
    details[attribute] = value;

    QMap<QString, QVariant> body;
    body["$token"] = _authToken;
    body["$distinct_id"] = _clientId;
    body["$time"] = _getUnixTime();
    body["$set"] = details;
    auto userData = QJsonObject::fromVariantMap(body);
    _httpRequest(QJsonDocument(userData).toJson().toBase64(), false);
    _settings->setValue(attribute, value);
}

QVariant Mixpanel::userAttribute(const QString &attribute) const
{
    return _settings->value(attribute);
}

uint Mixpanel::_getUnixTime() const
{
    return static_cast<uint>(::time(nullptr));
}

void Mixpanel::_httpRequest(QByteArray data,
                            bool isEventRequest,
                            bool waitForFinish) const
{
    QUrlQuery query;
    QByteArray params;
    query.addQueryItem("data", data);
    query.addQueryItem("ip", QString(std::to_string(1).c_str()));
    params.append(query.query());

    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QNetworkReply* reply = nullptr;
    if (isEventRequest) {
        reply = manager->post(_eventRequest, params);
    } else {
        reply = manager->post(_userRequest, params);
    }

    if (reply != nullptr && waitForFinish) {
        while (reply->isRunning())
            QCoreApplication::processEvents();
    }
}
