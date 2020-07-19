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
 * @brief The BackgroundOpsThread class runs a background thread which can be
 * used for running detection and alignment operations concurrently.
 */
class BackgroundOpsThread : public QThread
{
    Q_OBJECT

public:
    BackgroundOpsThread(QWidget*);
    ~BackgroundOpsThread();
    void qtSlot(const string& progressText,
                unsigned int progress,
                int totalSteps);
    void qtSignalSlot(const string& progressText,
                      unsigned int completed_samples,
                      int total_samples);

    MavenParameters* getMavenParameters() { return mavenParameters; }
    void setMavenParameters(MavenParameters* mp) { mavenParameters = mp; }

    void setPeakDetector(PeakDetector* pd);
    PeakDetector* getPeakDetector() { return peakDetector; }

    void setRunFunction(QString functionName);

    void setStopped(bool stopped) { _stopped = stopped; }
    bool stopped() { return _stopped; }
    void completeStop();

    void setMainWindow(MainWindow* mw) { mainwindow = mw; }

    PeakDetector* peakDetector;
    MavenParameters* mavenParameters;

    /**
     * @brief updateGroups Updates the attributes of peakgroups according to new
     * mainwindow parameters.
     * @param groups    Updation of attributes of peakgroups.
     * @param samples   Current visible samples in the state of alignment or not
     * @param mavenParameters current mainwindow mavenparameters.
     */
    static void updateGroups(QList<shared_ptr<PeakGroup>> groups,
                             vector<mzSample*> samples,
                             MavenParameters* mavenParameters);

signals:
    void updateProgressBar(QString, int, int);

    /**
     * @brief Emitted when there was an error during alignment.
     */
    void alignmentError(QString);

    /**
     * @brief Signals whether alignment is in effect.
     */
    void samplesAligned(bool);

    /**
     * @brief Signals the completion of an alignment process.
     */
    void alignmentComplete(QList<PeakGroup>);

    /**
     * @brief Signal for alignment cancellation and restore.
     */
    void restoreAlignment();

    void newPeakGroup(PeakGroup* group);

protected:
    void run(void);

private:
    string runFunction;
    MainWindow* mainwindow;

    void align();
    void alignWithObiWarp();

    void emitGroups();

    void pullIsotopes(PeakGroup* group);
    void pullIsotopesBarPlot(PeakGroup* group);

    void postProcessGroups();
    void computePeaks();
    void findFeatures();

    volatile bool _stopped;
};

#endif
