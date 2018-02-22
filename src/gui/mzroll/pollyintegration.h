#ifndef POLLYINTEGRATION_H
#define POLLYINTEGRATION_H

#include <loginform.h>
#include <projectform.h>
#include <tabledockwidget.h>
#include "saveJson.h"
// class TableDockWidget;

class PollyIntegration
{

	public:
	    ~PollyIntegration();
	    PollyIntegration(TableDockWidget* tableDockWidget);
	    QString username;
	    QString password;
        void exportGroupsToSpreadsheet_polly();
		QByteArray run_qt_process(QString command);
	    QByteArray run_system_process(QString command);
	    QString get_urls(QByteArray result);
	    QStringList get_system_urls(QString filename);
	    QStringList get_project_upload_url_commands(QByteArray result2,QStringList filenames);
	    QString get_run_id(QByteArray result);
	    QString get_patch_id(QByteArray result);    
	    int authenticate_login(QString username,QString password);
	    int check_already_logged_in();
	    void transferData();
	    QString exportData(QString projectname,QString ProjectId);
	    void get_project_name();
		void login_user();
		QVariantMap getUserProjects();
		QVariantMap getUserProjectsMap(QByteArray result2);
	private:
		TableDockWidget* _tableDockWidget;
};

#endif // POLLYINTEGRATION_H
