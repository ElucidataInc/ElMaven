#ifndef AUTOUPDATE_H
#define AUTOUPDATE_H

#include <QThread>

class AutoUpdater : public QThread
{
    Q_OBJECT
public:
    /**
     * @brief Runs a check for updates in a new thread. This is the primary use
     * of this class. The signal `updateAvailable` is emitted when a new version
     * of the application is found to be present at the update repository.
     */
    void run() override { _checkForUpdate(); }

public slots:
    /**
     * @brief Assumes an expected path for the installed build's maintenance
     * tool and starts it in the "updater" mode. The tool is run as a subprocess
     * and will not block even when called from the main event loop.
     */
    void startMaintenanceTool();

signals:
    /**
     * @brief This signal is emitted when there is an update available. Its
     * argument is the version string for the latest available update.
     */
    void updateAvailable(QString);

    /**
     * @brief This signal is emitted when there was a failure in the process of
     * checking for an update, such as those caused by network or parse failure.
     */
    void updateCheckFailed();

    /**
     * @brief A signal notifying that the maintenance tool has been started.
     */
    void maintenanceToolStarted();

    /**
     * @brief A signal notifying that the maintenance tool has been closed.
     */
    void maintenanceToolClosed();

private:
    /**
     * @brief Checks for the availability of a new version and emits relevant
     * signals. Is blocking in itself, but since this is called from the `run`
     * method, it should be running in separate thread.
     */
    void _checkForUpdate();

    /**
     * @brief Parses raw data from the "Updates.xml" file that should be
     * available at the online repository's root path.
     * @param downloadedData A `QByteArray` that contains downloaded data for
     * the XML file.
     * @return A pair of a boolean, that denotes whether the byte array was
     * parsed successfully as an XML file, and a `QString`, that stores the new
     * version, if available.
     */
    pair<bool, QString> _parseUpdatesXml(QByteArray& downloadedData);
};

#endif
