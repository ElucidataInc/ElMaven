#ifndef POLLYELMAVENINTERFACE_H
#define POLLYELMAVENINTERFACE_H

#include "ui_pollyelmaveninterface.h"
#include "stable.h"
#include "database.h"
#include "mainwindow.h"
#include "pollyintegration.h"

#include <QMap>

class PollyIntegration;
class MainWindow;
// class BackgroundPeakUpdate;

class PollyElmavenInterfaceSettings;
// extern Database DB;


class PollyElmavenInterfaceDialog : public QDialog, public Ui_PollyElmavenInterfaceDialog
{
		Q_OBJECT

		public:
				//  enum FeatureDetectionType { FullSpectrum=0, CompoundDB, QQQ };
				 PollyElmavenInterfaceDialog(MainWindow* mw);
                 QVariantMap projectnames_id;
                 QVariantMap userProjectFilesMap;
                 QString ProjectId; 
                 QJsonObject project_file_map;
				//  ~PollyElmavenInterfaceDialog();
				 void setSettings(QSettings* settings) { this->settings = settings; }
				 void setMainWindow(MainWindow* w) { this->mainwindow = w; }
                 MainWindow* getMainWindow() {return mainwindow;}
                //  void displayAppropriatePeakDetectionDialog(FeatureDetectionType type); //TODO: Sahil - Kiran, Added while merging mainwindow
				//  void setMavenParameters(QSettings *settings);
                PollyIntegration* _pollyIntegration;

		public Q_SLOTS:
                // void showEvent( QShowEvent* event );
				void uploadDataToPolly();
                void loadDataFromPolly();
                void initialSetup();
                QVariantMap loadFormData();
				void cancel();
                //  void show(); //TODO: Sahil - Kiran, Added while merging mainwindow
				void on_comboBox_load_projects_activated(const QString &arg1);
                // virtual void closeEvent(QCloseEvent* event) override;
				// void dialogRejected();

                Q_SIGNALS:
                    void updateSettings(PollyElmavenInterfaceSettings* pd);
                    // void settingsChanged(string key, string value);
        // public:
                // QString massCutoffType;

        private:
				QSettings *settings;
				MainWindow *mainwindow;
                // BackgroundPeakUpdate* peakupdater;
				// FeatureDetectionType _featureDetectionType;
                PollyElmavenInterfaceSettings* pdSettings;
                // void displayAppropriatePeakDetectionDialog(FeatureDetectionType type); //TODO: Sahil - Kiran, removed while merging mainwindow
                // void inputInitialValuesPeakDetectionDialog();
                // void updateQSettingsWithUserInput(QSettings *settings);


};

// Q_DECLARE_METATYPE(QString*)

class PollyElmavenInterfaceSettings: public QObject
{
    Q_OBJECT
    public:
        PollyElmavenInterfaceSettings(PollyElmavenInterfaceDialog* dialog);
        QMap<QString,QVariant>& getSettings() { return settings; }

    public Q_SLOTS:
        void updatePollySettings(string key, string value);

    private:
        QMap<QString, QVariant> settings;
        PollyElmavenInterfaceDialog* pd;

};

#endif
