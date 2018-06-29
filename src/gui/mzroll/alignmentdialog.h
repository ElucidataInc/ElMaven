#ifndef ALIGNDIALOG_H
#define ALIGNDIALOG_H

#include "stable.h"
#include "globals.h"
#include "ui_alignmentdialog.h"
#include "mainwindow.h"
#include "alignment/polyFit.h"
#include "alignment/loessFit.h"
#include "alignment/obiWarpAlign.h"


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
		
		void UndoAlignment();
		void updateParameters();
		void func();
		void processSlices(vector<mzSlice*>&slices, string setName, PeakDetector peakDetector);
		void writeCSVRep(string setName, PeakDetector peakDetector);
		void checkCornerCases();
		void connectStatements();
		void align();


	Q_SIGNALS:
		/**
		 * [update ProgressBar]
		 * @param QString [message]
		 * @param int     [progress value]
		 * @param int     [total value]
		 */
		void updateProgressBar(QString, int, int);
		void alignmentError(QString);

		/**
		 * [new PeakGroup]
		 * @param group [pointer to PeakGroup]
		 */
		void newPeakGroup(PeakGroup* group);
		


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
		void setAlignWrtExpectedRt(bool checked);
		void setInitPenalty(bool checked);
		void restorDefaultValues(bool checked);
		void toggleObiParams(bool show);
		void showAdvanceParameters(bool checked);
};

#endif
