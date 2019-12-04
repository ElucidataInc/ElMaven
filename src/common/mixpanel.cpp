#define _STR(X) #X
#define STR(X) _STR(X)

#include <ctime>

#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "mixpanel.h"

Mixpanel::Mixpanel()
{
    QString settingsPath = QStandardPaths::writableLocation(
        QStandardPaths::ConfigLocation);
    settingsPath = QDir::cleanPath(settingsPath
                                   + QDir::separator()
                                   + "ElMaven"
                                   + QDir::separator()
                                   + "mixpanel.ini");
    QSettings settings(settingsPath, QSettings::IniFormat);
    if (!settings.contains("mixpanel-uuid")) {
        _clientId = QUuid::createUuid().toString();
        settings.setValue("mixpanel-uuid", _clientId);
        _isFirstSession = true;
    } else {
        _clientId = settings.value("mixpanel-uuid").toString();
        _isFirstSession = false;
    }

    _authToken = QString(STR(MIXPANEL_TOKEN));
    _eventRequest = QNetworkRequest(QUrl("https://api.mixpanel.com/track"));
    _eventRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                            "application/x-www-form-urlencoded");
    _userRequest = QNetworkRequest(QUrl("https://api.mixpanel.com/engage"));
    _userRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                           "application/x-www-form-urlencoded");

    QMap<QString, QVariant> properties;
    properties["First session"] = QVariant(_isFirstSession);
    trackEvent("Session Start", properties);
}

Mixpanel::~Mixpanel()
{
    trackEvent("Session End", QMap<QString, QVariant>());
}

void Mixpanel::trackEvent(const QString& event,
                          QMap<QString, QVariant> properties)
{
    properties["token"] = QVariant(_authToken);
    properties["distinct_id"] = QVariant(_clientId);
    properties["time"] = QVariant(_getUnixTime());
    QJsonObject eventData
    {
        {"event", event},
        {"properties", QJsonObject::fromVariantMap(properties)}
    };
    _httpRequest(QJsonDocument(eventData).toJson().toBase64());
}

void Mixpanel::updateUser(const QString& attribute, const QVariant& value)
{
    QMap<QString, QVariant> details;
    details["$token"] = QVariant(_authToken);
    details["$distinct_id"] = QVariant(_clientId);
    details["$time"] = QVariant(_getUnixTime());
    details[attribute] = value;
    auto userData = QJsonObject::fromVariantMap(details);
    _httpRequest(QJsonDocument(userData).toJson().toBase64(), false);
}

uint Mixpanel::_getUnixTime()
{
    return static_cast<uint>(::time(nullptr));
}

void Mixpanel::_httpRequest(QByteArray data, bool isEventRequest)
{
    QUrlQuery query;
    QByteArray params;
    query.addQueryItem("data", data);
    query.addQueryItem("ip", QString(std::to_string(1).c_str()));
    params.append(query.query());

    QNetworkAccessManager* manager = new QNetworkAccessManager();
    if (isEventRequest) {
        manager->post(_eventRequest, params);
    } else {
        manager->post(_userRequest, params);
    }
}
