#ifndef POLLYINTEGRATION_H
#define POLLYINTEGRATION_H

#include <QtCore>
#include <QObject>

class Logger;

enum class PollyApp: int
{
    FirstView = 0,
    PollyPhi = 1,
    QuantFit = 2,
    None = 3
};

enum class ErrorStatus: int
{
    Success = 0,
    Failure = 1,
    Error = 2,
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
    QString nodeModulesPath;
    QString createProjectOnPolly(QString projectname);

    /**
     * @brief This creates a workflow request for PollyPhi and returns its ID.
     * @param projectId The ID of the project for which to create workflow.
     * @return Return workflow request ID as a QString.
     */
    QString createWorkflowRequest(QString projectId,
                                  QString workflowName,
                                  QString workflowId);

    /**
     * @brief This creates a run request for QuantFit and returns its ID.
     * @param componentName The name of Polly component to create run for.
     * @param projectId The ID of the project for which to create run.
     * @param extraInfo Extra information that can be used on Polly however it
     * may be required.
     * @return Returns run request ID as a QString.
     */
    QString createRunRequest(QString componentId,
                             QString projectId,
                             QString extraInfo="");

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
    ErrorStatus UploadPeaksToCloud(QString sessionId, QString fileName, QString filePath);
    ErrorStatus UploadToCloud(QString uploadUrl, QString filePath);

    /**
     * @brief Parse and return the project ID for a new Polly project
     * @param result output from running "createproject" command that creates a
     * new Polly project
     * @return project ID generated for the new project
     */
    QString parseId(QByteArray result);

    ErrorStatus sendEmail(QString user_email,
                   QString email_content,
                   QString email_message,
                   QString appName);
    ErrorStatus authenticateLogin(QString username, QString password);
    ErrorStatus checkLoginStatus();
    int checkNodeExecutable();
    int askForLogin();
    QPair<ErrorStatus, QStringList> exportData(QStringList filenames, QString projectId);
    QVariantMap getUserProjects();
    QVariantMap getUserProjectFiles(QStringList ProjectIds);
    QStringList getUserProjectFilesMap(QByteArray result2);
    bool validSampleCohort(QString sampleCohortFile,
                           QStringList loadedSamples = QStringList());
    QString getCredFile();
    QString getCurrentUsername();

    /**
     * @brief check for active internet connection
     * @details ping google.com to check of the user is connected to the
     * internet
     * @return true if user has an active connection
     */
    ErrorStatus activeInternet();

    QByteArray redirectionUiEndpoint();

    /**
     * @brief Given a Polly project's ID, obtain its UI endpoint.
     * @param projectId A string storing a unique project ID.
     * @return A string storing the URL directing to Polly project.
     */
    QString getProjectUrl(QString projectId);

    /**
     * @brief Obtain a redirection URL for a given component and run ID.
     * @param componentId The component ID for which URL will be fetched.
     * @param runId The run ID that will be replaced within the URL.
     * @param datetimestamp The date-time string that will be replaced within
     * the URL.
     * @return A URL redirecting to web application run.
     */
    QString getComponentEndpoint(QString componentId, QString runId, QString datetimestamp);

    /**
     * @brief Obtain a redirection URL for a given workflow and run ID.
     * @param workflowID The workflow ID for which URL will be fetched.
     * @param workflowRequestId The wf-request ID that will be replaced within
     * the URL.
     * @param landingPage The name of the landing page that will be replaced within
     * the URL.
     * @param uploadProjectIdThread The project ID that will be replaced within the URL.
     * @param datetimestamp The date-time string that will be replaced within
     * the URL.
     * @return A URL redirecting to web application run.
     */
    QString getWorkflowEndpoint(QString workflowId,
                                QString workflowRequestId,
                                QString landingPage,
                                QString uploadProjectIdThread,
                                QString datetimestamp);

    /**
     * @brief Extract out a component ID from a.JSON response obtained by
     * requesting to api/component endpoint.
     * @param app The Polly application for which ID is needed.
     * @return The component ID as a QString.
     */
    QString obtainComponentId(PollyApp app);

    QString obtainComponentName(PollyApp app);

    /**
     * @brief Get the status of current user's application licenses on Polly.
     * @return A map of PollyApp and a boolean value indicating whether the user
     * should have access to that application.
     */
    QMap<PollyApp, bool> getAppLicenseStatus();

    /**
     * @brief Extract out a workflow ID from a JSON response obtained by
     * requesting to api/wf-fe-info endpoint.
     * @param workflowName The Polly application for which ID is needed.
     * @return The workflow ID as a QString.
     */
    QString obtainWorkflowId(PollyApp app);

    /**
     * @brief Get the string name of a PollyApp enum identifier.
     * @return A string with name of the app.
     */
    static QString stringForApp(PollyApp app);

public slots:
    void requestSuccess();
    void requestFailed();

Q_SIGNALS:
    void receivedEPIError(QString);

private:
    QString _username;
    QString credFile;
    bool _hasIndexFile;
    DownloadManager* _dlManager;
    QTemporaryFile* _fPtr;
    unsigned int _retries;
    QString indexFileURL;

    QPair<ErrorStatus, QMap<QString, QStringList>> _fetchAppLicense();
    bool _checkForIndexFile();
    bool validCohorts(QStringList cohorts);
    bool _hasError(QList<QByteArray>);
    Logger* _log();
};

#endif // POLLYINTEGRATION_H
