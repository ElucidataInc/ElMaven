#ifndef POLLYELMAVENINTERFACE_H
#define POLLYELMAVENINTERFACE_H

#include <QDateTime>
#include <QMap>

#include "pollyintegration.h"
#include "stable.h"
#include "ui_pollyelmaveninterface.h"

Q_DECLARE_METATYPE(PollyApp)

class PollyIntegration;
class MainWindow;
class LoginForm;
class TableDockWidget;
class PollyWaitDialog;

class EPIWorkerThread : public QThread
{
    Q_OBJECT

public:

    enum class RunMethod
    {
        AuthenticateAndFetchData,
        UploadFiles,
        SendEmail
    };

    EPIWorkerThread(PollyIntegration *pi) : _pollyIntegration(pi) {}

    void run();

    void setMethodToRun(RunMethod method)
    {
        _currentMethod = method;
    }

    void setUploadArgs(QDir dir,
                       QStringList filesToUpload,
                       QString datetimestamp,
                       QString pollyProjectId)
    {
        _uploadArgs.dir = dir;
        _uploadArgs.filesToUpload = filesToUpload;
        _uploadArgs.datetimestamp = datetimestamp;
        _uploadArgs.pollyProjectId = pollyProjectId;
    }

    void setEmailArgs(QString username,
                      QString subject,
                      QString content,
                      QString appname)
    {
        _emailArgs.username = username;
        _emailArgs.subject = subject;
        _emailArgs.content = content;
        _emailArgs.appname = appname;
    }

Q_SIGNALS:
    void filesUploaded(QStringList patchId,
                       QString uploadProjectIdThread,
                       QString datetimestamp);
    void licensesReady(QMap<PollyApp, bool> licenseMap);
    void projectsReady(QVariantMap projectNamesId);
    void authenticationFinished(QString username, QString status);

private:

    struct _UploadStruct
    {
        QDir dir;
        QStringList filesToUpload;
        QString datetimestamp;
        QString pollyProjectId;
    };

    struct _EmailStruct
    {
        QString username;
        QString subject;
        QString content;
        QString appname;
    };

    RunMethod _currentMethod;

    PollyIntegration* _pollyIntegration;

    _UploadStruct _uploadArgs;

    _EmailStruct _emailArgs;

    void _authenticateUserAndFetchData();

    void _uploadFiles();

    void _sendEmail();

    void _removeFilesFromDir(QDir dir, QStringList files);
};

/**
* @brief This class is responsible for creating the Polly interface and calling
* PollyCLI library.
*/
class PollyElmavenInterfaceDialog : public QDialog,
                                    public Ui_PollyElmavenInterfaceDialog
{
    Q_OBJECT

public:

    enum class SendMode {
        PollyApp,
        PollyProject
    };

    /**
     * @brief Constructor for the dialog.
     * @param mw A pointer to the main window of the application, used to create
     * a dialog with this window as its owner.
     */
    PollyElmavenInterfaceDialog(MainWindow* mw);

    /**
     * @brief Destructor of this class.
     */
    ~PollyElmavenInterfaceDialog();

    /**
     * @brief Checks for various conditions before allowing interaction with the
     * dialog.
     * @details Some conditions are necessary for proper interaction and upload
     * through EPI dialog, which at the moment are:
     * 1. availability of Node executable (for network communication),
     * 2. logged-in status for a user,
     * 3. setting of default state on the EPI dialog.
     */
    void initialSetup();

    /**
     * @brief This function is responsible for loading the form data on
     * polly-elmaven-interface GUI.
     * @details this function performs the following tasks in the given order -
     * 1. First  of all, tell the user that Elmaven is trying to get data from
     * Polly.
     * 2. change the login status to "connected"
     * 3. call PollyCLI library to get project names and id mapping object,
     * corresponding to that user.
     * 4. call PollyCLI library to get project id and files mapping
     * 5. Populate all combo boxes on this UI, with the data obtained from above
     * steps.
     */
    void startupDataLoad();

    /**
     * @brief Get a pointer to the main window owning this dialog.
     * @return Pointer to a MainWindow object.
     */
    MainWindow* getMainWindow();

public Q_SLOTS:

    /**
     * @brief Switch to a given Polly app.
     * @param app A PollyApp enum that signifies which application should be
     * selected.
     */
    void switchToApp(PollyApp app);

    /**
     * @brief Select the peak table in the table combo box, if the given table
     * name exists.
     * @param tableName The peak table to be selected automatically. If the name
     * of this peak table does not exist in the list of table names, nothing
     * will happen.
     */
    void setSelectedTable(TableDockWidget* table);

    void showEPIError(QString errorMessage);

Q_SIGNALS:

    /**
     * @brief Signal emitted when uploading process has completed, successfully
     * or otherwise.
     */
    void uploadFinished(bool success);

private:

    /**
     * @brief A pointer to the main application window.
     */
    MainWindow* _mainwindow;

    /**
     * @brief The current selected Polly application.
     */
    PollyApp _selectedApp;

    /**
     * @brief A pointer to the currently selected peak table.
     */
    TableDockWidget* _activeTable;

    /**
     * @brief A pointer to the table storing bookmarked peaks.
     */
    TableDockWidget* _bookmarkTable;

    /**
     * @brief A data structure of table names mapping to the peak tables that
     * they represent.
     */
    QMap<QString, TableDockWidget*> _tableNameMapping;

    /**
     * @brief A pointer to the loading dialog that logs user in and fetches
     * their data.
     */
    PollyWaitDialog* _loadingDialog;

    /**
     * @brief A pointer to loginform class.
     */
    LoginForm* _loginform;

    /**
     * @brief A pointer to PollyIntegration object.
     */
    PollyIntegration* _pollyIntegration;

    /**
     * @brief A URL that will lead the user to their analysis using sent data on
     * the selected Polly web app.
     */
    QMap<PollyApp, QUrl> _redirectionUrlMap;

    /**
     * @brief A URL that will open the Polly project to which the last session
     * data upload was made.
     */
    QUrl _projectRedirectionUrl;

    /**
     * @brief Project ID to which all uploads will be happening.
     */
    QString _pollyProjectId;

    /**
     * @brief A JSON object which contains the mapping of project names with
     * their IDs.
     */
    QVariantMap _projectNameIdMap;

    /**
     * @brief A map which contains the mapping of licenses names with their
     * active state.
     */
    QMap<PollyApp, bool> _licenseMap;

    /**
     * @brief Name of the temporary directory where files are contained before
     * being sent to Polly.
     */
    QString _writeableTempDir;

    /**
     * @brief An indicator of whether an upload thread is running in the
     * background.
     */
    bool _uploadInProgress;

    /**
     * @brief A boolean that denotes whether the last sample cohort file
     * prepared for upload was valid or not.
     */
    bool _lastCohortFileWasValid;

    /**
     * @brief Stores which mode the dialog is currently set to be in.
     */
    SendMode _selectedMode;

    /**
     * @brief A worker thread that allows separating blocking operations from
     * the main event loop.
     */
    EPIWorkerThread *_worker;

    /**
     * @brief This function calls login form UI to take credentials from user.
     */
    void _callLoginForm();

    /**
     * @brief This function calls initial login UI to connect to Polly and fetch
     * projects.
     */
    void _callInitialEPIForm();

    /**
     * @brief This function prepares all the files that are to be uploaded.
     * @details this function performs the following tasks in the given order -
     * 1.Get peaksTableList from Mainwindow.if no peak tables, then show warning
     * and stop
     * 2. If peak tables are there, proceed to upload the latest peak table csv
     * file to Polly
     * 3. call getAllGroups on the latest peak table. if empty, show warning and
     * exit.
     * 4. call exportGroupsToSpreadsheet_polly to save CSV file to tmp
     * directory..
     * 5. this tmp directory is nothing but lastdir+"/tmp_files/"
     * 6. call saveSettings function from mainwindow->mavenParameters to save
     * settings file to the same tmp directory
     * 7. finally output the list of files in that tmp directory.
     */
    QStringList _prepareFilesToUpload(QDir qdir, QString datetimestamp);

    /**
     * @brief Creates an emDB session file and JSON files for all peak tables,
     * to be uploaded to a Polly project.
     * @param datetimestamp A string which will be prepended to file-names as a
     * way of time-stamping them.
     * @return A list of file-names that were created.
     */
    QStringList _prepareSessionFiles(QString datetimestamp);

    /**
     * @brief Creates a redirection URL based on the current state of the
     * dialog.
     * @param datetimestamp A timestamp string that will be used to generate a
     * unique URL for a project.
     * @param uploadProjectIdThread Project ID of the user project to send files
     * to.
     * @return Redirection URL as a QString.
     */
    QString _getAppRedirectionUrl(QString datetimestamp,
                                  QString uploadProjectIdThread);

    /**
     * @brief Creates a redirection URL for the given Polly project ID.
     * @param projectId The unique ID to which a URL is needed.
     * @return A URL as a QString.
     */
    QString _getProjectRedirectionUrl(QString projectId);

    /**
     * @brief Get project ID for the currently selected or newly entered project
     * name by the user.
     * @return Project ID as a QString.
     */
    QString _getProjectId();

    /**
     * @brief Pop up a message for an error that occurs anytime while using the
     * EPI dialog.
     * @param title Title of the message box to be shown.
     * @param message Message to be shown.
     */
    void _showErrorMessage(QString title,
                           QString message,
                           QMessageBox::Icon icon);

    /**
     * @brief Reset and clear UI elements of the dialog to their fresh state.
     */
    void _resetUiElements();

    /**
     * @brief Adds the table to the table combo box, if the table contains
     * relevant data for the current workflow.
     * @param table A pointer to the peak table to be added.
     * @param tableName Name of the peak table to be added.
     */
    void _addTableIfPossible(TableDockWidget *table, QString tableName);

    void _showPollyButtonIfUrlExists();

    void _populateProjects();

    void _populateTables();

private Q_SLOTS:

    /**
     * @brief Enables new project creation UI and disables existing project UI.
     */
    void _enableNewProjectUi();

    /**
     * @brief Enables existing project selection UI and disables new project UI.
     */
    void _enableExistingProjectUi();

    /**
     * @brief Handle information fetched from Polly about current user's
     * projects.
     * @param projectNamesId A map of user project names to their IDs on Polly.
     */
    void _handleProjects(QVariantMap projectNameIdMap);

    /**
     * @brief Handle information fetched from Polly about current user's
     * licenses.
     * @param licenseMap A map of application names to their license status on
     * Polly.
     */
    void _handleLicenses(QMap<PollyApp, bool> licenseMap);

    /**
     * @brief Handle the result of authentication status post-process.
     * @param username The username for which the authentication was performed.
     * @param status A QString storing status value of the authentication
     * process after its finished.
     */
    void _handleAuthentication(QString username, QString status);

    /**
     * @brief Perform actions and checks that need to be done after files have
     * been uploaded to Polly.
     * @param patchId This value is not being used at the moment, but can be
     * used to check if the upload process finished successfully.
     * @param uploadProjectIdThread The project ID for which uploading has
     * finished.
     * @param datetimestamp Timestamp for the data upload.
     */
    void _performPostFilesUploadTasks(QStringList patchId,
                                      QString uploadProjectIdThread,
                                      QString datetimestamp);

    /**
     * @brief Perform any actions that might be needed after upload process was
     * finished or prematurely terminated.
     * @param uploadSuccessful Whether upload to Polly was successful or not.
     */
    void _performPostUploadTasks(bool uploadSuccessful);

    /**
     * @brief This function uploads all the files prepared by
     * prepareFilesToUpload function, to Polly.
     * @details this function performs the following tasks in the given order -
     * 1.check the values of comboBox_existing_projects and
     * lineEdit_new_project_name
     * 2. If new project is specified, create it using createProjectOnPolly
     * function from PollyCLI library and upload to the newly created project,
     * else upload to existing project that user has selected 3.call
     * prepareFilesToUpload function
     * 4. Check if files are ready to upload. If no, show warning and exit.
     * 5. If files are prepared, call exportData function from PollyCLI library
     * 6. If successfully uploaded, show redirection url messagebox, else
     * display error messagebox.
     */
    void _uploadDataToPolly();

    /**
     * @brief Open a URL in desktop environment's browser that takes the user to
     * a Polly application's interface where they can proceed with the analysis.
     */
    void _goToPollyApp();

    /**
     * @brief Open a URL in desktop environment's browser that takes the user to
     * the Polly project where they can proceed with their analysis.
     */
    void _goToPollyProject();

    /**
     * @brief Log-out of Polly for the currently logged in user.
     */
    void _logout();

    /**
     * @brief Change the currently selected Polly application.
     */
    void _changePage();

    /**
     * @brief Hide the form and switch to advert if the user is not licensed for
     * the currently selected application.
     */
    void _hideFormIfNotLicensed();

    /**
     * @brief Enable or disables group subset options, based on the table name
     * passed.
     * @param tableName Name of the table for which the group options have to be
     * revised.
     */
    void _reviseGroupOptions(QString tableName);

    /**
     * @brief Switch for the currently selected mode of data upload. This will
     * either be sending files for a specific Polly application or only to a
     * Polly project.
     */
    void _changeMode();
};

#endif
