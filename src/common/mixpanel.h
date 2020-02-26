#ifndef MIXPANEL_H
#define MIXPANEL_H

#include <QString>
#include <QNetworkRequest>

class QSettings;

/**
 * @brief The Mixpanel class allows creating usage trackers that report to a
 * Mixpanel project identified using a secret authentication token (supplied
 * during build time).
 */
class Mixpanel
{
public:
    /**
     * @brief Constructor. Sends a "Session Start" event automatically.
     */
    Mixpanel();

    /**
     * @brief Destructor. Sends a "Session End" event automatically.
     */
    ~Mixpanel();

    /**
     * @brief Track individual events that must have a unique event-action
     * string. Optionally other attributes can be provided to specialise or
     * differentiate an event.
     * @param event A string denoting a meaningful action taken by the user.
     * @param properties Optional map of event properties that provide
     * additional context to the given event.
     * @param waitForFinish Makes this a blocking call by not returning until
     * Mixpanel confirms that the event has been registered.
     */
    void trackEvent(const QString& event,
                    QMap<QString, QVariant> properties,
                    bool waitForFinish = false) const;

    /**
     * @brief Update (or insert) the details of a user. Only one attribute is
     * allowed to be updated at a time (limitation imposed by Mixpanel API).
     * @param attribute User attribute to update.
     * @param value Value of user attribute being updated.
     */
    void updateUser(const QString& attribute,
                    const QVariant& value) const;

    /**
     * @brief Allows querying for values of user attributes that might have been
     * saved in an earlier session.
     * @param attribute Name of the attribute to search for.
     */
    QVariant userAttribute(const QString& attribute) const;

private:
    /**
     * @brief A unique user ID that is set during object initialization.
     * Persists for a single machine over multiple sessions.
     */
    QString _clientId;

    /**
     * @brief A unique authentication token used to post events/user updates to
     * Mixpanel account.
     */
    QString _authToken;

    /**
     * @brief A settings object that stores values that may be needed to store
     * various machine-specific attributes over multiple sessions.
     */
    QSettings* _settings;

    /**
     * @brief A flag that stores whether a user session is the first one on a
     * machine or not.
     */
    bool _isFirstSession;

    /**
     * @brief A request object used for all POST requests to the following API:
     * "https://api.mixpanel.com/track".
     */
    QNetworkRequest _eventRequest;

    /**
     * @brief A request object used for all POST requests to the following API:
     * "https://api.mixpanel.com/engage".
     */
    QNetworkRequest _userRequest;

    /**
     * @brief Obtains the number of seconds elapsed since epoch.
     * @return UNIX timestamp in seconds.
     */
    uint _getUnixTime() const;

    /**
     * @brief Send an HTTP POST request with a given value of "data" key on one
     * of the Mixpanels endpoints based on whether the request is to track an
     * event or to update user details.
     * @param data A base64 encoded byte-array, that encodes a JSON body
     * acceptable by Mixpanel's HTTP spec.
     * @param isEventRequest Boolean that denotes whether the request is for
     * event tracking or user update. True by default.
     * @param waitForFinish Stalls current thread to make sure that the method
     * returns only when the server responds that it has received this request.
     */
    void _httpRequest(QByteArray data,
                      bool isEventRequest = true,
                      bool waitForFinish = false) const;
};

#endif // MIXPANEL_H
