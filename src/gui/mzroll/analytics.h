#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <string>
#include <QString>
#include <QNetworkAccessManager>

class Analytics {

    private:
        QString trackerID;
        QString clientID;
        QString hostname;
        QString language;
        QNetworkRequest req;
        bool _isFirstSession;

        void httpPost(QUrlQuery query);
        QUrlQuery intialSetup();
        QString getClientID();
        QString getUserAgent();
        QString osName();

    public:
        Analytics();
        void hitScreenView(QString screenName);
        void hitEvent(QString category,
                      QString action,
                      QString label="",
                      int value = 0);
        void sessionStart();
        void sessionEnd();

};

#endif //ANALYTICS_H
