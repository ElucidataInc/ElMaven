#ifndef BACKGROUND_PEAK_UPDATE_H
#define BACKGROUND_PEAK_UPDATE_H

#include <qstring.h>
#include <qthread.h>
#include <string>

#include "../libmaven/PeakDetector.h"

/**
 * \class BackgroundPeakUpdate
 *
 * \ingroup mzroll
 *
 * \brief Runs in background thread to update the peaks corresponding to metabolites.
 *
 * Runs in background thread to update the peaks corresponding to metabolites.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class BackgroundPeakUpdate: public QThread {
	Q_OBJECT

public:

	/** Create a BackgroundPeakUpdate instance.
	 * @param pointer to QWidget
	 */
	BackgroundPeakUpdate(QWidget*);
//
//	~BackgroundPeakUpdate();

	/** Sets the run function name.
	 * @param Qstring inpute filename
	 */
	void setRunFunction(QString functionName);

	/** Stop the thread.
	 */
	void stop() {
		_stopped = true;
	}

	/** Returns if the thread is stopped or not.
	 */
	bool stopped() {
		return _stopped;
	}
	void setMainWindow(MainWindow* mw) {
		mainwindow = mw;
	}

	PeakDetector getPeakDetector() {
		return peakDetector;
	}

	void setPeakDetector(PeakDetector* pd) {
		peakDetector = *pd;
	}

	signals:
	/** Update the progress bar at the bottom of Maven.
	 * Shows progress of the file imports.
	 * @param Qstring inpute filename, int, int
	 */
	void updateProgressBar(QString, int, int);
	void newPeakGroup(PeakGroup* group);

protected:
	void run(void);

private:
	MainWindow *mainwindow;
	PeakDetector peakDetector;
	string runFunction;

private:
	volatile bool _stopped;
};

#endif
