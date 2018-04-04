#ifndef POLLYELMAVENINTERFACE_H
#define POLLYELMAVENINTERFACE_H

#include "ui_pollyelmaveninterface.h"
#include "stable.h"
#include "database.h"
#include "mainwindow.h"
#include "loginform.h"
#include "pollyintegration.h"

#include <QMap>

class PollyIntegration;
class MainWindow;
class LoginForm;
class TableDockWidget;

class PollyElmavenInterfaceDialog : public QDialog, public Ui_PollyElmavenInterfaceDialog
{
	Q_OBJECT

        public:
                PollyElmavenInterfaceDialog(MainWindow* mw);
                ~PollyElmavenInterfaceDialog();
                QVariantMap projectnames_id;
                QVariantMap userProjectFilesMap;
                QString ProjectId; 
                QJsonObject project_file_map;
                void setSettings(QSettings* settings) { this->settings = settings; }
                void setMainWindow(MainWindow* w) { this->mainwindow = w; }
                MainWindow* getMainWindow() {return mainwindow;}
                PollyIntegration* _pollyIntegration;
                LoginForm* _loginform;

        public Q_SLOTS:
                QStringList prepareFilesToUpload();
                QString uploadDataToPolly();
                void loadDataFromPolly();
                void initialSetup();
                QVariantMap loadFormData();
                void call_login_form();
		void cancel();
                QStringList readFromFile(QString fileName);
                void on_comboBox_load_projects_activated(const QString &arg1);

        private:
                QSettings *settings;
                MainWindow *mainwindow;
                TableDockWidget* _tableDockWidget;
};

#endif
