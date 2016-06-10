#ifndef BACKGROUND_PEAK_UPDATE_H
#define BACKGROUND_PEAK_UPDATE_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"
#include "database.h"
#include "csvreports.h"
#include <iostream>
#include "../libmaven/PeakDetector.h"

class MainWindow;
class Database;
class TableDockWidget;
class EIC;

extern Database DB;

class BackgroundPeakUpdate: public QThread {
Q_OBJECT

public:

	BackgroundPeakUpdate(QWidget*);
	~BackgroundPeakUpdate();

	void setPeakDetector(PeakDetector* pd) {
		peakDetector = *pd;
	}
	void setRunFunction(QString functionName);

	void stop() {
		_stopped = true;
	}
	bool stopped() {
		return _stopped;
	}
	void setMainWindow(MainWindow* mw) {
		mainwindow = mw;
	}

	PeakDetector peakDetector;

signals:
	void updateProgressBar(QString, int, int);
	void newPeakGroup(PeakGroup* group);

protected:
	void run(void);

private:
	string runFunction;
	MainWindow *mainwindow;

	void align();
	void getPullIsotopeSettings();
	void writeCSVRep(string setName);
	void getProcessSlicesSettings();
	void pullIsotopes(PeakGroup *group);
	void processSlices(void);
	void processSlices(vector<mzSlice*>&slices, string setName);
	void processSlice(mzSlice& slice);
	void processCompounds(vector<Compound*> set, string setName);
	void computePeaks();
	void processMassSlices();
	void findPeaksOrbi(void);
	void findPeaksQQQ(void);
	bool addPeakGroup(PeakGroup& group);

private:
	volatile bool _stopped;

};

#endif

