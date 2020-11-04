#ifndef BACKGROUND_PEAK_UPDATE_H
#define BACKGROUND_PEAK_UPDATE_H

#include <common/downloadmanager.h>
#include <common/logger.h>

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
class PollyIntegration;

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
    void setPullIsotopesForFormulaArgs(string formula, int charge)
    {
        _isotopeFormula = formula;
        _isotopeCharge = charge;
    }

    void setParentGroup(shared_ptr<PeakGroup> group)
    {
        _parentGroup = make_shared<PeakGroup>(*group);
    }

    shared_ptr<PeakGroup> parentGroup() { return _parentGroup; }

    void completeStop();

    void setMainWindow(MainWindow* mw) { mainwindow = mw; }

    PeakDetector* peakDetector;
    MavenParameters* mavenParameters;

    void performPolyFitAlignment() { _performPolyFitAlignment = true; };

    /**
     * @brief updateGroups Updates the attributes of peakgroups according to new
     * mainwindow parameters.
     * @param groups    Updation of attributes of peakgroups.
     * @param samples   Current visible samples in the state of alignment or not
     */
    static void updateGroups(QList<shared_ptr<PeakGroup>>& groups,
                             vector<mzSample*> samples);
    
    void classifyGroups(vector<PeakGroup>& groups);

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

    /**
     * @brief Some functions of classification runs outside
     * El-MAVEN and cancellation at that point can not be 
     * handled and hence signal to disable/enable cancel button
     * in peak detector dialog.
     */
    void toggleCancel(); 
    
    void newPeakGroup(PeakGroup* group);

    void noInternet();

protected:
    void run(void);

private:
    string runFunction;
    MainWindow* mainwindow;

    void align();
    void alignWithObiWarp();

    void emitGroups();

    void pullIsotopesForFormula(string formula, int charge);
    void pullIsotopesForGroup(PeakGroup* parentGroup);
    void pullIsotopesForBarPlot(PeakGroup* parentGroup);

    void computePeaks();
    void findFeatures();

    // arguments for `pullIsotopesForFormula` method
    string _isotopeFormula;
    int _isotopeCharge;

    // argument for `pullIsotopesForGroup` and `pullIsotopesForBarplot`
    shared_ptr<PeakGroup> _parentGroup;

    // perform PolyFit alignment just after peak detection (if true)
    bool _performPolyFitAlignment;

    /**
	 * [write CSV Report]
	 * @param setName [name of the set]
	 */
	void writeCSVRep(string setName);

    /**
	 * @brief Downloads binary and model from S3 bucket 
	 * if the two files does not exist on user's pc. 
	 * @param filename Tells the file either model or 
	 * binary to be downloaded. 
	 */ 
	bool downloadPeakMlFilesFromAws(QString fileName);

    /**
	 * @brief Downloads binary and model from S3 bucket 
	 * if the two files does not exist on user's pc using 
     * signed URL. 
	 * @param filename Tells the file either model or 
	 * binary to be downloaded. 
	 */ 
	bool downloadPeakMlFilesFromURL(QString fileName);
	
	/**
	 * @brief Changes the mode of the file and gives it executable rights. 
	 * Used to give downloaded moi file the execute rights. 
	 */
	void changeMode(string fileName);

	/**
	 * @brief Remove peakML files- model and CSV files for input and output
	 * from user's system
	 */
	void removeFiles();

    /**
     * @brief Checks for active internet connection before downloading the
     * required files.
     * @return Boolean value. True if active else false
     */ 
    bool _checkInternetConnection();

	DownloadManager *_dlManager;
	PollyIntegration *_pollyIntegration;
};

#endif
