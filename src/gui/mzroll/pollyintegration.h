#ifndef POLLYINTEGRATION_H
#define POLLYINTEGRATION_H

#include "loginform.h"
#include "mainwindow.h"
#include "saveJson.h"
#include "pollyelmaveninterface.h"

class LoginForm;
class MainWindow;
class TableDockWidget;
class PollyElmavenInterfaceDialog;
class PollyIntegration
{

	public:
	    ~PollyIntegration();
	    PollyIntegration(MainWindow* mw = nullptr,PollyElmavenInterfaceDialog* EPI=nullptr);
	    QString username;
	    QString password;
        void exportGroupsToSpreadsheet_polly();
		QByteArray run_qt_process(QString command, QStringList args = QStringList());
	    QByteArray run_system_process(QString command);
	    QString get_urls(QByteArray result);
	    QStringList get_system_urls(QString filename);
	    QStringList get_project_upload_url_commands(QByteArray result2,QStringList filenames);
		QStringList get_projectFiles_download_url_commands(QByteArray result2,QStringList filenames);
	    QString get_run_id(QByteArray result);
	    QString get_patch_id(QByteArray result);
	    int authenticate_login(QString username,QString password);
	    int check_already_logged_in();
	    void transferData();
	    QString exportData(QString projectname,QString ProjectId);
		QString loadDataFromPolly(QString ProjectId,QStringList filenames);
	    void get_project_name();
		void login_user();
		QVariantMap getUserProjects();
		QVariantMap getUserProjectFiles(QStringList ProjectIds);
		QVariantMap getUserProjectsMap(QByteArray result2);
		QStringList getUserProjectFilesMap(QByteArray result2);
		LoginForm* _loginform;
	private:
		MainWindow* _mainwindow;
		TableDockWidget* _tableDockWidget;
		PollyElmavenInterfaceDialog* _pollyelmaveninterfacedialog;
		QString nodePath;
		QString jsPath;
		QString credFile;
};

#endif // POLLYINTEGRATION_H
