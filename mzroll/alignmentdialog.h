#ifndef ALIGNDIALOG_H
#define ALIGNDIALOG_H

#include "stable.h"
#include "globals.h"
#include "ui_alignmentdialog.h"
#include "mainwindow.h"

class BackgroundPeakUpdate;
class Database;
class MainWindow;

extern Database DB; 

class AlignmentDialog : public QDialog, public Ui_AlignmentDialog {
	Q_OBJECT

	public:
		AlignmentDialog(QWidget *parent);
		~AlignmentDialog();
		void setMainWindow(MainWindow* mw);
		void setWorkerThread(BackgroundPeakUpdate* alignmentWorkerThread) {workerThread = alignmentWorkerThread;}
		MainWindow* _mw;
		BackgroundPeakUpdate* workerThread;

	public Q_SLOTS:
		void cancel();
		void show();
		void inputInitialValuesAlignmentDialog();
		void intialSetup();
		void algoChanged();
		void setDatabase();
		void setDatabase(QString db);
		void setProgressBar(QString text, int progress, int totalSteps);
		void showInfo(QString text);
};

#endif
