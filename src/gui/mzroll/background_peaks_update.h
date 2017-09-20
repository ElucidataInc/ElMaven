#ifndef BACKGROUND_PEAK_UPDATE_H
#define BACKGROUND_PEAK_UPDATE_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"
#include "database.h"
#include "csvreports.h"
#include <iostream>
#include <QProcess>
#include <QJsonObject>
#include "PeakDetector.h"
#include "mavenparameters.h"

class MainWindow;
class Database;
class TableDockWidget;
class EIC;
class Aligner;


extern Database DB;

/**
 * @class BackgroundPeakUpdate
 * @ingroup mzroll
 * @brief Class which runs a background thread. Used for peak detection.
 * @author Elucidata
 */
class BackgroundPeakUpdate: public QThread {
Q_OBJECT

public:


	BackgroundPeakUpdate(QWidget*);
	~BackgroundPeakUpdate();
	void qtSlot(const string& progressText, unsigned int completed_slices, int total_slices);
	void qtSignalSlot(const string& progressText, unsigned int completed_samples, int total_samples);
	/**
	 * [get Maven Parameters]
	 * @return [params]
	 */
	MavenParameters* getMavenParameters() {
		return mavenParameters;
	}

	/**
	 * [set Maven Parameters]
	 * @param mp [params]
	 */
	void setMavenParameters(MavenParameters* mp) {
		mavenParameters = mp;
	}
	/**
	 * [set Peak Detector]
	 * @param pd [pointer to peakDetector]
	 */
	void setPeakDetector(PeakDetector* pd) {
		peakDetector = *pd;
	}

	/**
	 * [get Peak Detector]
	 * @return peakDetector
	 */
	PeakDetector getPeakDetector() {
		return peakDetector;
	}

	/**
	 * [set RunFunction]
	 * @param functionName [QString RunFunction]
	 */
	void setRunFunction(QString functionName);

	/**
	 * [stop thread]
	 */
	void stop() {
		_stopped = true;
	}

	void started() {
		_stopped = false;
	}

	/**
	 * [check for stopped thread]
	 * @return [true if stopped, else false]
	 */
	bool stopped() {
		return _stopped;
	}

	void completeStop();
	void saveSettings(QString filename); //TODO: Sahil - Kiran, Added while merging mainwindow
  	void loadSettings(QString filename); //TODO: Sahil - Kiran, Added while merging mainwindow
	QString printSettings(); //TODO: Sahil - Kiran, Added while merging mainwindow

	/**
	 * [set Main Window]
	 * @param mw [pointer to MainWindow]
	 */
	void setMainWindow(MainWindow* mw) {
		mainwindow = mw;
	}

    void readDataFromPython(QByteArray& data);
    void runPythonProg(Aligner* aligner);
    void sendDataToPython(QJsonObject& grpJson, QJsonObject& rtsJson);

    PeakDetector peakDetector;
	MavenParameters* mavenParameters;

Q_SIGNALS:

	/**
	 * [update ProgressBar]
	 * @param QString [message]
	 * @param int     [progress value]
	 * @param int     [total value]
	 */
	void updateProgressBar(QString, int, int);

	/**
	 * [new PeakGroup]
	 * @param group [pointer to PeakGroup]
	 */
	void newPeakGroup(PeakGroup* group);

	void alignmentComplete(QList<PeakGroup>);



protected:
	/**
	 * [run method for a thread]
	 */
	void run(void);

private:
	string runFunction;
	MainWindow *mainwindow;
    QProcess* pythonProg;

	/**
	 * [align function]
	 */
	void align();
	void alignUsingDatabase();

	/**
	 * [get PullIsotope Settings]
	 */
	void getPullIsotopeSettings();

	/**
	 * [write CSV Report]
	 * @param setName [name of the set]
	 */
	void writeCSVRep(string setName);

	/**
	 * [get ProcessSlices Settings]
	 */
	void getProcessSlicesSettings();

	/**
	 * [pullIsotopes]
	 * @param group [pointer to group]
	 */
	void pullIsotopes(PeakGroup *group);

	void pullIsotopesBarPlot(PeakGroup *group);

	/**
	 * [processSlices]
	 */
	void processSlices(void);

	/**
	 * [process one Slice]
	 * @method processSlice
	 * @param  slice        [pointer to mzSlice]
	 */
	void processSlices(vector<mzSlice*>&slices, string setName);

	/**
	 * [process one Slice]
	 * @method processSlice
	 * @param  slice        [pointer to mzSlice]
	 */
	void processSlice(mzSlice& slice);

	/**
	 * [process Compounds]
	 * @method processSlices
	 * @param  set        [vector of pointer to Compound]
	 * @param  setName       [name of set]
	 */
	void processCompounds(vector<Compound*> set, string setName);

	/**
	 * [compute Peaks]
	 */
	void computePeaks();

	/**
	 * [process Mass Slices]
	 * @method processMassSlices
	 */
	void processMassSlices();

	/**
	 * [find Peaks for Orbitrap machine mode]
	 */
	void findPeaksOrbi(void);

	/**
	 * [find Peaks for QQQ mode]
	 */
	void findPeaksQQQ(void);

	bool covertToMzXML(QString filename, QString outfile);



private:
	volatile bool _stopped;

};

#endif
