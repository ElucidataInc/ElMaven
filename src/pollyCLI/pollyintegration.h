#ifndef POLLYINTEGRATION_H
#define POLLYINTEGRATION_H

#include <QtCore>

class PollyIntegration
{

	public:
	    ~PollyIntegration();
	    PollyIntegration();
		void logout();
	    QString username;
	    QString password;
		QString jsPath;
		QString nodePath;
		QString user_login_required();
		QString createProjectOnPolly(QString projectname);
        QByteArrayList run_qt_process(QString command, QStringList args = QStringList());
	    QByteArray run_system_process(QString command);
	    QString get_urls(QByteArray result);
	    QStringList get_system_urls(QString filename);
	    QStringList get_project_upload_url_commands(QByteArray result2,QStringList filenames);
		QStringList get_projectFiles_download_url_commands(QByteArray result2,QStringList filenames);
	    QString get_run_id(QByteArray result);
	    QString authenticate_login(QString username,QString password);
	    int check_already_logged_in();
		int check_node_executable();
	    QStringList exportData(QStringList filenames,QString projectId);
		QString loadDataFromPolly(QString ProjectId,QStringList filenames);
		QVariantMap getUserProjects();
		QVariantMap getUserProjectFiles(QStringList ProjectIds);
		QVariantMap getUserProjectsMap(QByteArray result2);
		QStringList getUserProjectFilesMap(QByteArray result2);

	private:
		QString credFile;
};

#endif // POLLYINTEGRATION_H
