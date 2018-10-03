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
		QString createProjectOnPolly(QString projectname);
		QString createWorkflowRequest(QString projectId);
		QString shareProjectOnPolly(QString project_id,QVariantMap collaborators_map);
		QString getShareStatus(QByteArray result);
		
		/**
 		 * @brief Execute terminal commands from c++
 		 * @details This function uses Qprocess from Qt library to execute terminal commands from C++
 		 * @param command Terminal commands to be run
 		 * @param args List of arguments for the command
 		 * @return QByteArray of output and errors
		*/
        QList<QByteArray> runQtProcess(QString command, QStringList args = QStringList());

		/**
		 * @brief Upload given files to Polly
		 * @param url_with_wildcard URL for uploading the files with a * that has to be replaced
		 * with the filename
		 * @param filenames Names of the files to be uploaded to the project
		 * @return patchId output and error for every file upload process
		*/
	    QStringList get_project_upload_url_commands(QString url_with_wildcard, QStringList filenames);
		QString getFileUploadURLs(QByteArray result2);
		QStringList get_projectFiles_download_url_commands(QByteArray result2, QStringList filenames);
	    
		/**
 		 * @brief Parse and return the project ID for a new Polly project
 		 * @param result output from running "createproject" command that creates a new Polly project
 		 * @return project ID generated for the new project
		*/
		QString parseId(QByteArray result);
		bool sendEmail(QString user_email, QString email_content, QString email_message);
	    QString authenticateLogin(QString username,QString password);
	    int checkLoginStatus();
		int checkNodeExecutable();
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
		/**
		 * @brief check for active internet connection
		 * @details ping google.com to check of the user is connected to the internet
		 * @return true if user has an active connection
		*/
		bool activeInternet();
	private:
		QString credFile;
		bool validCohorts(QStringList cohorts);
};

#endif // POLLYINTEGRATION_H
