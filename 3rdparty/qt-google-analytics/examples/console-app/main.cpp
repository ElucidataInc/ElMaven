#include <QCoreApplication>
#include <QDebug>

#include "ganalytics.h"

class Watcher : public QObject
{
    Q_OBJECT

public slots:
    void onIsSendingChanged(bool sending)
    {
        if (sending)
            return;

        QCoreApplication::instance()->quit();
    }
};

int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationName("HSAnet");
    QCoreApplication::setApplicationName("Console-App");
    QCoreApplication::setApplicationVersion("0.1");

    QCoreApplication app(argc, argv);

    // Create the tracker
    GAnalytics tracker("UA-53395376-1");

    Watcher watcher;
    QObject::connect(&tracker, SIGNAL(isSendingChanged(bool)), &watcher, SLOT(onIsSendingChanged(bool)));

    // Shorten the interval
    tracker.setSendInterval(5 * 1000);

    // Send some dummy events
    tracker.sendEvent("lifecycle", "application_started");
    tracker.sendEvent("lifecycle", "application_stopped");

    qDebug() << "Waiting until the evens were sent ...";

    int ret = app.exec();

    qDebug() << "Shutting down.";

    return ret;
}

#include "main.moc"
