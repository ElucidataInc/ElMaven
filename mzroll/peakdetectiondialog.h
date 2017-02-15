#ifndef PEAKDETECTIONDIALOG_H
#define PEAKDETECTIONDIALOG_H

#include "ui_peakdetectiondialog.h"
#include "stable.h"
#include "database.h"
#include "mainwindow.h"

class MainWindow;
class TableDockWidget;
class BackgroundPeakUpdate;
extern Database DB;


class PeakDetectionDialog : public QDialog, public Ui_PeakDetectionDialog
{
		Q_OBJECT

		public:
				 enum FeatureDetectionType { FullSpectrum=0, CompoundDB, QQQ };
				 PeakDetectionDialog(QWidget *parent);
				 ~PeakDetectionDialog();
				 void setSettings(QSettings* settings) { this->settings = settings; }
				 void setMainWindow(MainWindow* w) { this->mainwindow = w; }
                 void displayAppropriatePeakDetectionDialog(FeatureDetectionType type); //TODO: Sahil - Kiran, Added while merging mainwindow
				 void setMavenParameters(QSettings *settings);

		public Q_SLOTS:
				 void findPeaks();
				 void loadModel();
				 void setOutputDir();
				 void setProgressBar(QString text, int progress, int totalSteps);
				 void runBackgroupJob(QString func);
                 void dbOptionsClicked();
                 void featureOptionsClicked();
				 void showInfo(QString text);
				 void cancel();
                 void initPeakDetectionDialogWindow(FeatureDetectionType type);
				 void showMethodSummary(); //TODO: Sahil - Kiran, Added while merging mainwindow
                 void saveMethod(); //TODO: Sahil - Kiran, Added while merging mainwindow
                 void loadMethod(); //TODO: Sahil - Kiran, Added while merging mainwindow
                 void updatePeakTableList(); //TODO: Sahil - Kiran, Added while merging mainwindow
				 void show(); //TODO: Sahil - Kiran, Added while merging mainwindow
				 void updatePeakQType(QString);
				 void showQualityWeightStatus(int value);
				 void showIntensityWeightStatus(int value);
				 void showDeltaRTWeightStatus(int value);
				 void setGroupRank();
				 void setInitialGroupRank();
				 void setDeltaRTWeightVisible(bool value);

		private:
				QSettings *settings;
				MainWindow *mainwindow;
                BackgroundPeakUpdate* peakupdater;
				FeatureDetectionType _featureDetectionType;

                // void displayAppropriatePeakDetectionDialog(FeatureDetectionType type); //TODO: Sahil - Kiran, removed while merging mainwindow
                void inputInitialValuesPeakDetectionDialog();
                void updateQSettingsWithUserInput(QSettings *settings);
};

#endif
