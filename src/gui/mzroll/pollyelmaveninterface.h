#ifndef POLLYELMAVENINTERFACE_H
#define POLLYELMAVENINTERFACE_H

#include "ui_pollyelmaveninterface.h"
#include "pollywaitdialog.h"
#include "stable.h"
#include "database.h"
#include "mainwindow.h"
#include "loginform.h"
#include "pollyintegration.h"
#include <QDateTime>
#include <QMap>

class PollyIntegration;
class MainWindow;
class LoginForm;
class TableDockWidget;

class PollyWaitDialog;

enum class PollyApp : int
{
    FirstView = 0,
    Fluxomics = 1
};

/**
* @brief This class id responsible for creating the POlly interface and calling pollyCLI library..
*/

class PollyElmavenInterfaceDialog : public QDialog, public Ui_PollyElmavenInterfaceDialog
{
	Q_OBJECT

        public:
                QString uploadProjectId;
                QStringList organisationSpecificCompoundDB;
                /**
                * @brief constructor with mainwindow pointer..
                * @param mw [pointer to mainwindow, used to create GUI for elmaven-polly-interface]
                */
                PollyElmavenInterfaceDialog(MainWindow* mw);
                /**
                 * @brief destructor of this class.. 
                 */
                ~PollyElmavenInterfaceDialog();
                /**
                 * @brief json object which contains the mapping of project names with their IDs
                 */
                QVariantMap projectNamesId;
                
                /**
                 * @brief json object which contains the mapping of project names with thier uploaded files
                 * @details this is a QVariantMap object that will look like this -
                 * project id -[<file1>,<file2>]
                 * this object is used to populate the compound database,setting combo boxes under polly-elmave-interface GUI.
                 */
                QVariantMap userProjectFilesMap;
                /**
                 * @brief project ID as stored on Polly
                */
                QString ProjectId; 
                /**
                 * @brief pointer to pollyintegration class..
                */
                PollyIntegration* _pollyIntegration;
                TableDockWidget* bookmarkedPeaks;
                /**
                 * @brief pointer to loginform class..
                */
                LoginForm* _loginform;

        private:
                TableDockWidget* _activeTable = NULL ;
                QString getRedirectionUrl(PollyApp currentApp, QString datetimestamp, QString uploadProjectIdThread);
                void setUiElementsFlux();
                void setUiElementsFV();
                QMap<QString, TableDockWidget*> tableNameMapping;
                QString getProjectId(QComboBox* projectList, QLineEdit* newProject);
                void showErrorMessage(QString title, QString message);
                QString writableTempDir = QStandardPaths::writableLocation(
                                                QStandardPaths::QStandardPaths::GenericConfigLocation)
                                                + QDir::separator()
                                                + "tmp_Elmaven_Polly_files";
        
        private Q_SLOTS:
                void goToPolly();
                void performPostUploadTasks(PollyApp currentApp, bool uploadSuccessful);
                void changePage(QListWidgetItem*, QListWidgetItem*);
                void setFluxPage();
        
        public Q_SLOTS:
                /**
                 * @brief This function prepares all the files that are to be uploaded..
                 * @details this function performs the following tasks in the given order -
                 * 1.Get peaksTableList from Mainwindow.if no peak tables, then show warning and stop
                 * 2. If peak tables are there, proceed to upload the latest peak table csv file to Polly
                 * 3. call getAllGroups on the latest peak table. if empty, show warning and exit.
                 * 4. call exportGroupsToSpreadsheet_polly to save CSV file to tmp directory..
                 * 5. this tmp directory is nothing but lastdir+"/tmp_files/"
                 * 6. call saveSettings function from mainwindow->mavenParameters to save settings file to the same tmp directory
                 * 7. finally output the list of files in that tmp directory..
                 */

                QStringList prepareFilesToUpload(PollyApp currentApp, QDir qdir, QString datetimestamp);

                /**
                 * @brief This function uploads all the files prepared by prepareFilesToUpload function, to Polly
                 * @details this function performs the following tasks in the given order -
                 * 1.check the values of comboBox_existing_projects and lineEdit_new_project_name
                 * 2. If new project is specified, create it using createProjectOnPolly function from PollyCLI library
                 *  and upload to the newly created project, else upload to existing project that user has selected
                 * 3.call prepareFilesToUpload function 
                 * 4. Check if files are ready to upload. If no, show warning and exit.
                 * 5. If files are prepared, call exportData function from PollyCLI library
                 * 6. If successfully uploaded, show redirection url messagebox, else display error messagebox.
                 */

                void uploadDataToPolly();

                /**
                 * @brief This function loads settings file and compound database files back to Elmaven from a project on polly
                 * @details this function performs the following tasks in the given order -
                 * 1.check the values of comboBox_load_settings and comboBox_load_db
                 * 2. create tmp directory to download files
                 * 3. download the files specified by the above two combo boxes, by calling loadDataFromPolly function from PollyCLI library
                 * 4. if successfully loaded, call loadPollySettings function from mainwindow to load settings..
                 * 5. update the progress bar and display messagebox with load status message
                 */


                // void loadDataFromPolly();

                void logout();
                void handleNewProject();
                void handleSelectProject();
                void initialSetup();
                /**
                 * @brief This function is responsible for loading the form data on polly-elmaven-interface GUI
                 * @details this function performs the following tasks in the given order -
                 * 1. First  of all, tell the user that Elmaven is trying to get data from polly..
                 * 2. change the login status to "connected"
                 * 3. call PollyCLI library to get project names and id mapping object, corresponding to that user..
                 * 4. call PollyCLI library to get project id and files mapping
                 * 5. Populate all combo boxes on this UI, with the data obtained from above steps..
                 */
                void startupDataLoad();

                // void loadFormData();
                // void handleResults(QStringList results);
                 /**
                 * @brief This function calls login form UI to take credentials from user.
                 */
                void call_login_form();
                 /**
                 * @brief This function calls initial login UI to connect to polly and fetch projects.
                 */
                void call_initial_EPI_form();
                /**
                 * @brief This function cancels the polly-elmaven-interface GUI
                 */
		void cancel();

                void setActiveTable(TableDockWidget* table) { _activeTable = table; }
                /**
                 * @brief This function changes the values of compoundDb, settings combo boxes based on values in load_project combo box
                 * @details this function performs the following tasks in the given order -
                 * 1. first of all clear the values of compoundDb, settings combo boxes
                 * 2. Now, check userProjectFilesMap object to see if their are any compound database and settings file 
                 * corresponding to that project on polly..
                 * 3. If yes, then populate compoundDb, settings combo boxes with those file names..
                 */
                // void on_comboBox_load_projects_activated(const QString &arg1);
                // void on_comboBox_existing_projects_activated(const QString &arg1);
        
        signals:
                /**
                 * @brief Signal emitted when uploading process has completed, successfully or otherwise.
                 */
                void uploadFinished(PollyApp currentApp, bool success);

        private:
                /**
                 * @brief pointer to mainwindow
                */
                MainWindow *mainwindow;
                /**
                 * @brief pointer to TableDockWidget class..
                */
                PollyWaitDialog* _loadingDialog;
                QUrl pollyURL;

        public slots:
            void handleResults(QVariantMap projectNamesId);
            void handleAuthentication(QString status);
            void postUpload(QStringList patchId, QString uploadProjectIdThread, QString datetimestamp, PollyApp currentApp);
};

class EPIWorkerThread : public QThread
{
    Q_OBJECT
    public:
        EPIWorkerThread();
        ~EPIWorkerThread();
        void run();
        QString username;
        QString password;
        QString state;
        QString datetimestamp;
        QDir tmpDir;
        QString uploadProjectIdThread;
        QStringList filesToUpload;
        PollyApp currentApp;
        PollyIntegration* _pollyintegration;
    signals:
        void filesUploaded(QStringList patchId, QString uploadProjectIdThread, QString datetimestamp, PollyApp currentApp);
        void resultReady(QVariantMap projectNamesId);
        void authentication_result(QString status);
};

#endif
