#ifndef PEAKDETECTIONDIALOG_H
#define PEAKDETECTIONDIALOG_H

#include "ui_peakdetectiondialog.h"
#include "superSlider.h"
#include "stable.h"

class MainWindow;
class TableDockWidget;
class BackgroundOpsThread;
class PeakDetectionSettings;
class RangeSlider;

class PeakDetectionDialog : public QDialog, public Ui_PeakDetectionDialog
{
		Q_OBJECT

		public:
				 enum FeatureDetectionType { FullSpectrum=0, CompoundDB, QQQ };
                                 PeakDetectionDialog(MainWindow* parent);
				 ~PeakDetectionDialog();
				 void setSettings(QSettings* settings) { this->settings = settings; }
				 void setMainWindow(MainWindow* w) { this->mainwindow = w; }
                 MainWindow* getMainWindow() {return mainwindow;}
                 void displayAppropriatePeakDetectionDialog(FeatureDetectionType type); //TODO: Sahil - Kiran, Added while merging mainwindow
				 void setMavenParameters(QSettings *settings);
                 bool databaseSearchEnabled();

		public Q_SLOTS:
				 void findPeaks();
				 void setProgressBar(QString text, int progress, int totalSteps);
				 void runBackgroupJob(QString func);
                 void dbSearchClicked();
                 void featureOptionsClicked();
				 void showInfo(QString text);
				 void cancel();
                 void initPeakDetectionDialogWindow(FeatureDetectionType type);
                 void getLoginForPeakMl();
                 void loginSuccessful();
                 void unsuccessfulLogin();
                 void updateCurationParameter(int lowerRange, int upperRange);

                 /**
                  * @brief Disables or enables certain UI elements, based on
                  * whether peak detection is currently in progress.
                  * @param detectionMode A boolean to signify whether peak
                  * detection is taking place or not.
                  */
                 void setDetectionMode(bool detectionModeOn);

                 /**
                  * @brief Repopulate the relevant databases in dropdowns.
                  */
                 void refreshCompoundDatabases();

                 void toggleFragmentation();
				 void show(); //TODO: Sahil - Kiran, Added while merging mainwindow
				 void showIntensityQuantileStatus(int);
				 void showQualityQuantileStatus(int);
				 void showBaselineQuantileStatus(int);
				 void showBlankQuantileStatus(int);
                 void showPeakWidthQuantileStatus(int value);
                 void setMassCutoffType(QString type);
                 void setQuantType(QString type);
                 void triggerSettingsUpdate();
                void onReset();

                Q_SIGNALS:
                    void updateSettings(PeakDetectionSettings* pd);
                    void settingsUpdated(QString key, QVariant value);
                    void settingsChanged(string key, string value);
                    void resetSettings(QList<QString> keys);
                    void findPeaksClicked();
        public:
                QString massCutoffType;
                bool peakMlSet;
                RangeSlider* slider; 

        protected:
                virtual void closeEvent(QCloseEvent* event) override;
                virtual void keyPressEvent(QKeyEvent* event) override;

        private:
				QSettings *settings;
				MainWindow *mainwindow;
                BackgroundOpsThread* peakupdater;
				FeatureDetectionType _featureDetectionType;
                PeakDetectionSettings* peakSettings;
                bool _inDetectionMode;

                // void displayAppropriatePeakDetectionDialog(FeatureDetectionType type); //TODO: Sahil - Kiran, removed while merging mainwindow
                void inputInitialValuesPeakDetectionDialog();
                void updateQSettingsWithUserInput(QSettings *settings);
};

Q_DECLARE_METATYPE(QString*)

class PeakDetectionSettings: public QObject
{
    Q_OBJECT
    public:
        PeakDetectionSettings(PeakDetectionDialog* dialog);
        QMap<QString,QVariant>& getSettings() { return settings; }

    public Q_SLOTS:
        void updatePeakSettings(string key, string value);

    private:
        QMap<QString, QVariant> settings;
        PeakDetectionDialog* pd;

};

#endif
