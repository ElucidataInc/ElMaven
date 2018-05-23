#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <string>
#include <QString>
#include <QNetworkAccessManager>

class Analytics {

    private:
        QString trackerId;
        QString hostname;
        QString uuid;
        QString language;
        QNetworkRequest req;
        void httpPost(QUrlQuery query);
        QUrlQuery intialSetup();
        QString getUserAgent();
        QString osName();

    public:
        Analytics();
        void hitScreenView(QString screenName);
        void hitEvent(QString category, QString action, int value = 0);

};

#endif //ANALYTICS_H
