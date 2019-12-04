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
    _settings = new QSettings(settingsPath, QSettings::IniFormat);
    if (!_settings->contains("mixpanel-uuid")) {
        _clientId = QUuid::createUuid().toString();
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

    QMap<QString, QVariant> properties;
    properties["First session"] = _isFirstSession;
    trackEvent("Session Start", properties);
}

Mixpanel::~Mixpanel()
{
    trackEvent("Session End", QMap<QString, QVariant>());
}

void Mixpanel::trackEvent(const QString& event,
                          QMap<QString, QVariant> properties) const
{
    properties["token"] = _authToken;
    properties["distinct_id"] = _clientId;
    properties["time"] = _getUnixTime();
    QJsonObject eventData
    {
        {"event", event},
        {"properties", QJsonObject::fromVariantMap(properties)}
    };
    _httpRequest(QJsonDocument(eventData).toJson().toBase64());
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

void Mixpanel::_httpRequest(QByteArray data, bool isEventRequest) const
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
