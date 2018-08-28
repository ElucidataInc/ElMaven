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
		QString shareProjectOnPolly(QString project_id,QVariantMap collaborators_map);
		QString get_share_status(QByteArray result);
		
		/**
 		 * @brief Execute terminal commands from c++
 		 * @details This function uses Qprocess from Qt library to execute terminal commands from C++
 		 * @param command Terminal commands to be run
 		 * @param args List of arguments for the command
 		 * @return QByteArray of output and errors
		*/
        QList<QByteArray> run_qt_process(QString command, QStringList args = QStringList());
	    QByteArray run_system_process(QString command);
	    QString get_urls(QByteArray result);
	    QStringList get_system_urls(QString filename);

		/**
		 * @brief Upload given files to Polly
		 * @param url_with_wildcard URL for uploading the files with a * that has to be replaced
		 * with the filename
		 * @param filenames Names of the files to be uploaded to the project
		 * @return patch_ids output and error for every file upload process
		*/
	    QStringList get_project_upload_url_commands(QString url_with_wildcard, QStringList filenames);
		QString getFileUploadURLs(QByteArray result2);
		QStringList get_projectFiles_download_url_commands(QByteArray result2, QStringList filenames);
	    
		/**
 		 * @brief Parse and return the project ID for a new Polly project
 		 * @param result output from running "createproject" command that creates a new Polly project
 		 * @return project ID generated for the new project
		*/
		QString get_run_id(QByteArray result);
		bool send_email(QString user_email, QString email_content, QString email_message);
	    QString authenticate_login(QString username,QString password);
	    int check_already_logged_in();
		int check_node_executable();
		int askForLogin();
	    QStringList exportData(QStringList filenames,QString projectId);
		QString loadDataFromPolly(QString ProjectId,QStringList filenames);
		QVariantMap getUserProjects();
		QVariantMap getUserProjectFiles(QStringList ProjectIds);
		QVariantMap getUserProjectsMap(QByteArray result2);
		QStringList getUserProjectFilesMap(QByteArray result2);
		QStringList getOrganizationalDBs(QString organisation);
		bool validSampleCohort(QString sampleCohortFile, QStringList loadedSamples = QStringList());
		QStringList parseResultOrganizationalDBs(QString result);
		QString getCredFile();
	private:
		QString credFile;
		bool validCohorts(QStringList cohorts);
};

#endif // POLLYINTEGRATION_H
