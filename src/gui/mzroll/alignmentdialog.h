#ifndef ALIGNDIALOG_H
#define ALIGNDIALOG_H

#include "stable.h"
#include "projectdatabase.h"
#include "ui_alignmentdialog.h"

class BackgroundPeakUpdate;
class Database;
class MainWindow;
class mzSample;

class AlignmentDialog : public QDialog, public Ui_AlignmentDialog {
	Q_OBJECT

	public:
        AlignmentDialog(MainWindow* parent);
		~AlignmentDialog();
		void setMainWindow(MainWindow* mw);
                void setWorkerThread(BackgroundPeakUpdate* alignmentWorkerThread);
		MainWindow* _mw;
        BackgroundPeakUpdate* workerThread;
        void updateUiFromValues(map<string, variant> settings);
        void saveValuesForUi();

    Q_SIGNALS:
        void changeRefSample(mzSample* sample);


    public Q_SLOTS:
        void refSampleChanged();
		void cancel();
		void show();
		void inputInitialValuesAlignmentDialog();
		void intialSetup();
		void algoChanged();
		void setDatabase();
		void setDatabase(QString db);
		void setProgressBar(QString text, int progress, int totalSteps);
		void showInfo(QString text);
		void setInitPenalty(bool checked);
		void restorDefaultValues(bool checked);
		void showAdvanceParameters(bool checked);
		void samplesAligned(bool status);
		void updateRestoreStatus();
};

#endif
