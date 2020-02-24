#ifndef BACKGROUND_PEAK_UPDATE_H
#define BACKGROUND_PEAK_UPDATE_H

#include "PeakGroup.h"
#include "stable.h"

class MainWindow;
class Database;
class TableDockWidget;
class EIC;
class Aligner;
class MavenParameters;
class PeakDetector;
class Compound;
class mzSlice;

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
    void qtSlot(const string& progressText, unsigned int progress, int totalSteps);
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
        void setPeakDetector(PeakDetector* pd);

	/**
	 * [get Peak Detector]
	 * @return peakDetector
	 */
        PeakDetector* getPeakDetector() {
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

	/**
	 * [set Main Window]
	 * @param mw [pointer to MainWindow]
	 */
	void setMainWindow(MainWindow* mw) {
		mainwindow = mw;
	}

    PeakDetector* peakDetector;
    MavenParameters* mavenParameters;

    inline void setUntargetedMustHaveMs2(bool pred) {
        _untargetedMustHaveMs2 = pred;
    }

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
	 * @brief signals whether alignment is in effect
	 */
	void samplesAligned(bool);

	/**
	 * @brief signal for alignment cancellation and restore
	 */
	void restoreAlignment();

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
    bool _untargetedMustHaveMs2;
	string runFunction;
	MainWindow *mainwindow;

	/**
	 * [align function]
	 */
	void align();
	void alignUsingDatabase();
	void alignWithObiWarp();

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

	void pullIsotopesIsoWidget(PeakGroup *group);

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
    void processCompounds(vector<Compound*> set,
                          string setName);

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
	 * @brief Find peaks using SRM Slices
	 * @details SRM Slices are calculated on the basis of filterline (srm id).
	 * These mzSlices are then used to find peaks.
	 * @see SRMList
	 */
	bool covertToMzXML(QString filename, QString outfile);



private:
	volatile bool _stopped;

};

#endif
