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

		public slots:
				 void findPeaks();
				 void loadModel();
				 void setOutputDir();
				 void setProgressBar(QString text, int progress, int totalSteps);
				 void runBackgroupJob(QString func);
				 void showInfo(QString text);
				 void cancel();
				 void show();
                 void setFeatureDetection(FeatureDetectionType type);

		private:
				QSettings *settings;
				MainWindow *mainwindow;
   				BackgroundPeakUpdate* peakupdater;
				FeatureDetectionType _featureDetectionType;
};

#endif
