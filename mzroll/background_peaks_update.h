#ifndef BACKGROUND_PEAK_UPDATE_H
#define BACKGROUND_PEAK_UPDATE_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"
#include "database.h"
#include "csvreports.h"
#include <iostream>


class MainWindow;
class Database;
class QInputDialog;
class TableDockWidget;
class EIC;

extern Database DB;

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

class BackgroundPeakUpdate : public QThread
{
    Q_OBJECT

public:

    /** Create a BackgroundPeakUpdate instance.
     * @param pointer to QWidget
     */
    BackgroundPeakUpdate(QWidget*);

    /** Create a BackgroundPeakUpdate instance.
     */
    ~BackgroundPeakUpdate();

    /** Sets the run function name.
     * @param Qstring inpute filename
     */
    void setRunFunction(QString functionName);
    void setCompounds(vector<Compound*>set) {
        compounds = set;
    }
    void setSlices(vector<mzSlice*>set)     {
        _slices = set;
    }
    void setPeakGroup(PeakGroup* p)         {
        _group = p;
    }
    void setSamples(vector<mzSample*>&set);
    void setMainWindow(MainWindow* mw)  {
        mainwindow = mw;
    }
    void setOutputDir(QString outdir)   {
        outputdir = outdir.toStdString() + string(DIR_SEPARATOR_STR);
    }
    void setMaxGroupCount(int x)      {
        limitGroupCount = x;
    }

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

    bool writePdfReportFlag;
    bool writeCSVFlag;
    bool alignSamplesFlag;
    bool keepFoundGroups;
    bool processKeggFlag;
    bool processMassSlicesFlag;
    bool pullIsotopesFlag;
    bool showProgressFlag;
    bool matchRtFlag;
    bool checkConvergance;

    /** default ionization mode used by mass spec
     */
    int ionizationMode;

    /** mass slicing parameters
     */
    float mzBinStep;
    float rtStepSize;
    float avgScanTime;
    float ppmMerge;
    float qcut;

    //peak detection

    /** smoothing window
     */
    float eic_smoothingWindow;
    int eic_smoothingAlgorithm;
    float eic_ppmWindow;
    int baseline_smoothing;
    int baseline_quantile;

    /** merging of ajacent peaks within eic
     */
    bool eic_mergeOverlapingPeaks;
    float eic_mergePPMCutoff;
    float eic_mergeMaxHeightDrop;

    //peak filtering
    int minGoodPeakCount;
    float minSignalBlankRatio;
    float minNoNoiseObs;
    float minSignalBaseLineRatio;
    float minGroupIntensity;

    //eic window around compound
    float compoundPPMWindow;
    float compoundRTWindow;
    int eicMaxGroups;

    //grouping of peaks across samples

    /** do no group peaks that are greater than differ more than X in retention time
     */
    float grouping_maxRtWindow;
    float grouping_maxMzWindow;

    /** stop looking for groups if group count is greater than X
     */
    int limitGroupCount;

//CLASS FUNCTIONS
    static vector<EIC*> pullEICs(mzSlice* slice, std::vector<mzSample*>&samples, int peakDetect, int smoothingWindow, int smoothingAlgorithm, float amuQ1, float amuQ3);

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
    string runFunction;
    string outputdir;

    MassCalculator mcalc;
    MainWindow *mainwindow;
    Classifier* clsf;
    PeakGroup* _group;
    vector<mzSample*>samples;
    vector<Compound*>compounds;
    vector<mzSlice*> _slices;

    void pullIsotopes(PeakGroup *group);
    void processSlices(void);
    void processSlices(vector<mzSlice*>&slices, string setName);
    void processSlice(mzSlice& slice);
    void processCompounds(vector<Compound*> set, string setName);
    void computePeaks();

    /** Processes Mass Slices using two algorithms A and B.
     */
    void processMassSlices();
    void findPeaksOrbi(void);
    void findPeaksQQQ(void);
    void computeKnowsPeaks(void);
    bool addPeakGroup(PeakGroup& group);
    vector<PeakGroup>allgroups;
    void cleanup();
    void printSettings();
    bool covertToMzXML(QString filename, QString outfile);

private:
    volatile bool _stopped;

};

#endif
