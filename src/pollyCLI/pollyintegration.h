#ifndef POLLYINTEGRATION_H
#define POLLYINTEGRATION_H

#include <QtCore>
#include <QObject>

enum class PollyApp: int
{
    FirstView = 0,
    PollyPhi = 1,
    QuantFit = 2
};

class QTemporaryFile;
class DownloadManager;
class PollyIntegration : public QObject
{
    Q_OBJECT
public:
    ~PollyIntegration();
    PollyIntegration(DownloadManager* dlManager);
    void logout();
    QString password;
    QString jsPath;
    QString nodePath;
    QString createProjectOnPolly(QString projectname);

    /**
     * @brief This creates a workflow request for PollyPhi and returns its ID.
     * @param projectId The ID of the project for which to create workflow.
     * @return Return workflow request ID as a QString.
     */
    QString createWorkflowRequest(QString projectId);

    /**
     * @brief This creates a run request for QuantFit and returns its ID.
     * @param componentName The name of Polly component to create run for.
     * @param projectId The ID of the project for which to create run.
     * @return Returns run request ID as a QString.
     */
    QString createRunRequest(QString componentId, QString projectId);

    /**
     * @brief Execute terminal commands from c++
     * @details This function uses Qprocess from Qt library to execute terminal
     * commands from C++
     * @param command Terminal commands to be run
     * @param args List of arguments for the command
     * @return QByteArray of output and errors
     */
    QList<QByteArray> runQtProcess(QString command,
                                   QStringList args = QStringList());

    /**
     * @brief Upload given files to Polly
     * @param url_with_wildcard URL for uploading the files with a * that has to
     * be replaced with the filename
     * @param filenames Names of the files to be uploaded to the project
     * @return patchId output and error for every file upload process
     */
    QStringList get_project_upload_url_commands(QString url_with_wildcard,
                                                QStringList filenames);
    QString getFileUploadURLs(QByteArray result2);
    QStringList get_projectFiles_download_url_commands(QByteArray result2,
                                                       QStringList filenames);
    QString UploadPeaksToCloud(QString sessionId, QString fileName, QString filePath);
    QString UploadToCloud(QString uploadUrl, QString filePath);

    /**
     * @brief Parse and return the project ID for a new Polly project
     * @param result output from running "createproject" command that creates a
     * new Polly project
     * @return project ID generated for the new project
     */
    QString parseId(QByteArray result);

    bool sendEmail(QString user_email,
                   QString email_content,
                   QString email_message,
                   QString appName);
    QString authenticateLogin(QString username, QString password);
    int checkLoginStatus();
    int checkNodeExecutable();
    int askForLogin();
    QStringList exportData(QStringList filenames, QString projectId);
    QString loadDataFromPolly(QString ProjectId, QStringList filenames);
    QVariantMap getUserProjects();
    QVariantMap getUserProjectFiles(QStringList ProjectIds);
    QVariantMap getUserProjectsMap(QByteArray result2);
    QStringList getUserProjectFilesMap(QByteArray result2);
    QStringList getOrganizationalDBs(QString organisation);
    bool validSampleCohort(QString sampleCohortFile,
                           QStringList loadedSamples = QStringList());
    QStringList parseResultOrganizationalDBs(QString result);
    QString getCredFile();
    QString getCurrentUsername();
    //void checkLicense();

    /**
     * @brief check for active internet connection
     * @details ping google.com to check of the user is connected to the
     * internet
     * @return true if user has an active connection
     */
    bool activeInternet();

    /**
     * @brief Obtain a redirection URL for a given component and run ID.
     * @param componentId The component ID for which URL will be fetched.
     * @param runId The run ID that will be replaced within the URL.
     * @param datetimestamp The date-time string that will be replaced wwithin
     * the URL.
     * @return A URL redirecting to web application run.
     */
    QString redirectionUiEndpoint(QString componentId,
                                  QString runId,
                                  QString datetimestamp);

    /**
     * @brief Extract out a component ID from a.JSON response obtained by
     * requesting to api/component endpoint.
     * @param app The Polly application for which ID is needed.
     * @return The component ID as a QString.
     */
    QString obtainComponentId(PollyApp app);

    /**
     * @brief Get the status of current user's application licenses on Polly.
     * @return A map of PollyApp and a boolean value indicating whether the user
     * should have access to that application.
     */
    QMap<PollyApp, bool> getAppLicenseStatus();

private:
    void checkForIndexFile();

public slots:
    void requestSuccess();
    void requestFailed();

private:
    QString _username;
    QString credFile;
    bool validCohorts(QStringList cohorts);
    DownloadManager* _dlManager;
    QTemporaryFile* _fPtr;
    unsigned int _retries;
    
    QMap<QString, QStringList> _fetchAppLicense();
    QString _stringForApp(PollyApp app);
};

#endif // POLLYINTEGRATION_H
