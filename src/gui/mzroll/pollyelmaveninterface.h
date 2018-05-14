#ifndef POLLYELMAVENINTERFACE_H
#define POLLYELMAVENINTERFACE_H

#include "ui_pollyelmaveninterface.h"
#include "stable.h"
#include "database.h"
#include "mainwindow.h"
#include "loginform.h"
#include "initialEPIform.h"
#include "pollyintegration.h"
#include <QDateTime>
#include <QMap>

class PollyIntegration;
class MainWindow;
class LoginForm;
class InitialEPIForm;
class TableDockWidget;

extern Database DB;
/**
* @brief This class id responsible for creating the POlly interface and calling pollyCLI library..
*/

class PollyElmavenInterfaceDialog : public QDialog, public Ui_PollyElmavenInterfaceDialog
{
	Q_OBJECT

        public:
                /**
                * @brief credentials required to connect to polly..
                */
                QStringList credentials;
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
                QVariantMap projectnames_id;
                QVariantMap collaborators_map;
                /**
                 * @brief json object which contains the mapping of project names with thier uploaded files
                 * @details this is a QVariantMap object that will look like this -
                 * project id -[<file1>,<file2>]
                 * this object is used to populate the compound database,setting combo boxes under polly-elmave-interface GUI.
                 */
                QVariantMap userProjectFilesMap;
                QMap< QString,QPointer<TableDockWidget> > peakTableNameMapping;
                QMap< QString,QPointer<TableDockWidget> > bookmarkTableNameMapping;
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
                /**
                 * @brief pointer to loginform class..
                */
                InitialEPIForm* _initialEPIform;

        public Q_SLOTS:
                /**
                 * @brief This function prepares all the files that are to be uploaded..
                 * @details this function performs the following tasks in the given order -
                 * 1.Get peaksTableList from Mainwindow.if no peak tables, then show warning and stop
                 * 2. If peak tables are there, proceed to upload the latest peak table csv file to Polly
                 * 3. call getAllGroups on the latest peak table. if empty, show warning and exit.
                 * 4. call exportGroupsToSpreadsheet_polly to save CSV file to tmp directory..
                 * 5.this tmp directory is nothing but lastdir+"/tmp_files/"
                 * 6. call saveSettings function from mainwindow->mavenParameters to save settings file to the same tmp directory
                 * 7. finally output the list of files in that tmp directory..
                 */

                QStringList prepareFilesToUpload(QDir qdir);

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

                QString uploadDataToPolly();

                /**
                 * @brief This function loads settings file and compound database files back to Elmaven from a project on polly
                 * @details this function performs the following tasks in the given order -
                 * 1.check the values of comboBox_load_settings and comboBox_load_db
                 * 2. create tmp directory to download files
                 * 3. download the files specified by the above two combo boxes, by calling loadDataFromPolly function from PollyCLI library
                 * 4. if successfully loaded, call loadPollySettings function from mainwindow to load settings..
                 * 5. update the progress bar and display messagebox with load status message
                 */


                void loadDataFromPolly();

                /**
                 * @brief This function is responsible for initial steps..
                 * @details this function performs the following tasks in the given order -
                 * 1. checks if the user credentials file exists, if not then call login form..
                 * if the file is there, try to login to polly using the credentials mentioned in that file
                 * if successfull, call loadformdata, else call login form.. 
                 */
                void logout();
                void showCompoundDBUploadFrame();
                void showAdvanceSettings();
                void populate_comboBox_compound_db();
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
                QVariantMap startup_data_load();
                void loadFormData();
                void AddCollaborator();
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
                /**
                 * @brief This function changes the values of compound_db, settings combo boxes based on values in load_project combo box
                 * @details this function performs the following tasks in the given order -
                 * 1. first of all clear the values of compound_db, settings combo boxes
                 * 2. Now, check userProjectFilesMap object to see if their are any compound database and settings file 
                 * corresponding to that project on polly..
                 * 3. If yes, then populate compound_db, settings combo boxes with those file names..
                 */
                void on_comboBox_load_projects_activated(const QString &arg1);
                void on_comboBox_existing_projects_activated(const QString &arg1);

        private:
                /**
                 * @brief pointer to mainwindow
                */
                MainWindow *mainwindow;
                /**
                 * @brief pointer to TableDockWidget class..
                */
                TableDockWidget* _tableDockWidget;
};

class EPIWorkerThread : public QThread
{
    Q_OBJECT
    public:
        EPIWorkerThread();
        ~EPIWorkerThread();
        void run();
    signals:
        void resultReady(QStringList results);
};

#endif

