#ifndef  BODE_INCLUDED
#define  BODE_INCLUDED

#include "stable.h"
#include "globals.h"
#include <ctime>
#include <sstream>
#include "mavenparameters.h"
#include "SRMList.h"
#include "scatterplot.h"
#include "eicwidget.h"
#include "settingsform.h"
#include "pathwaywidget.h"
#include "spectrawidget.h"
#include "alignmentvizallgroupswidget.h"
#include "qcustomplot.h"
#include "masscalcgui.h"
#include "adductwidget.h"
#include "gettingstarted.h"
#include "ligandwidget.h"
#include "isotopeswidget.h"
#include "treedockwidget.h"
#include "tabledockwidget.h"
#include "samplertwidget.h"
#include "isotopeplotdockwidget.h"
#include "isotopeplot.h"
#include "peakdetectiondialog.h"
#include "pollyelmaveninterface.h"
#include "alignmentdialog.h"
#include "awsbucketcredentialsdialog.h"
//#include "rconsoledialog.h"
#include "background_peaks_update.h"
#include "heatmap.h"
#include "treemap.h"
#include "note.h"
#include "analytics.h"
#include "history.h"
#include "suggest.h"
#include "animationcontrol.h"
#include "noteswidget.h"
#include "gallerywidget.h"
#include "projectdockwidget.h"
#include "spectramatching.h"
#include "mzfileio.h"
#include "logwidget.h"
#include "qdownloader.h"
#include "spectralhit.h"
// #include "rconsolewidget.h"
#include "spectralhitstable.h"
#include "peptidefragmentation.h"
//Added when merged with Maven776 - Kiran
#include "remotespectrahandler.h"
#include "messageBoxResize.h"
#include <csignal>
#include <QList>
#include <QRegExp>

class SettingsForm;
class EicWidget;
class PlotDockWidget;
class BackgroundPeakUpdate;
class PeakDetectionDialog;
class PollyElmavenInterfaceDialog;
class AwsBucketCredentialsDialog;
class AlignmentDialog;
//class RConsoleDialog;
class SpectraWidget;
class GroupRtWidget;
class AlignmentVizAllGroupsWidget;
class QCustomPlot;
class IsotopicPlots;
class AdductWidget;
class GettingStarted;
class LigandWidget;
class PathwayWidget;
class IsotopeWidget;
class MassCalcWidget;
class TreeDockWidget;
class BookmarkTableDockWidget;
class PeakTableDockWidget;
class SampleRtWidget;
class IsotopePlotDockWidget;
class IsotopePlot;
class Classifier;
class ClassifierNeuralNet;
class ClassifierNaiveBayes;
class HeatMap;
class ScatterPlot;
class TreeMap;
class History;
class QSqlDatabase;
class SuggestPopup;
class NotesWidget;
class GalleryWidget;
class mzFileIO;
class ProjectDockWidget;
class SpectraMatching;
class LogWidget;
// class RconsoleWidget;
class SpectralHit;
class SpectralHitsDockWidget;
class PeptideFragmentationWidget;

extern Database DB;
//Added when merged with Maven776 - Kiran
class RemoteSpectraHandler;

class AutoSave : public QThread
{
    Q_OBJECT

public:
    AutoSave(MainWindow*);
    void saveProjectWorker(bool tablesOnly=false);
    MainWindow* _mainwindow;

private:
    bool saveTablesOnly;
    void run();
};

class MainWindow: public QMainWindow {
Q_OBJECT

public:
	int value() const { return m_value; }
	MainWindow(QWidget *parent = 0);
    ~MainWindow();
	QSettings* getSettings() {
		return settings;
	}
	vector<mzSample*> samples;		//list of loaded samples
	static mzSample* loadSample(QString filename);
	int peaksMarked = 0;
    int lastPeakTableId = 0;
	int totalCharge = 0;
	bool allPeaksMarked = false;
	bool samplesAlignedFlag = false;
	map<pair<string,string>, double> deltaRt;

	Analytics* getAnalytics(){
		return analytics;
	}

	AutoSave* autosave;
    QSet<QString> pendingMzRollSaves;
	MavenParameters* mavenParameters;
	QSqlDatabase localDB;					//local database
	QDoubleSpinBox *massCutoffWindowBox;
	QComboBox *massCutoffComboBox;
	QLineEdit *searchText;
	QLabel *ionizationModeLabel;
	QSpinBox *ionChargeBox;
	QComboBox *quantType;
	QLabel *statusText;
	QStringList pathlist;
	QString currentIntensityName;

	SRMList *srmList;
    map<string, Compound*> annotation;
	
	PathwayWidget *pathwayWidget;
	SpectraWidget *spectraWidget;
    GroupRtWidget* groupRtWidget;
    SampleRtWidget *sampleRtWidget;
	AlignmentVizAllGroupsWidget * alignmentVizAllGroupsWidget;
	IsotopePlotDockWidget *isotopePlotDockWidget;
	IsotopePlot *isotopePlot;
	QCustomPlot *customPlot;
    QCustomPlot *groupRtVizPlot;
    QCustomPlot *sampleRtVizPlot;
	QCustomPlot *alignmentVizAllGroupsPlot;
	MassCalcWidget *massCalcWidget;
	AdductWidget *adductWidget;
	GettingStarted *gettingstarted;
	LigandWidget *ligandWidget;
	IsotopeWidget *isotopeWidget;
	TreeDockWidget *covariantsPanel;
	TreeDockWidget *fragPanel;
	TreeDockWidget *pathwayPanel;
	TreeDockWidget *srmDockWidget;
	//TreeDockWidget   *peaksPanel;
	QDockWidget *spectraDockWidget;
    QDockWidget *groupRtDockWidget;
	QDockWidget *alignmentVizAllGroupsDockWidget;
	QDockWidget *pathwayDockWidget;
	QDockWidget *heatMapDockWidget;
	QDockWidget *scatterDockWidget;
	QDockWidget *treeMapDockWidget;
	QDockWidget *galleryDockWidget;
	LogWidget *logWidget;
	NotesWidget *notesDockWidget;
	ProjectDockWidget *projectDockWidget;
	SpectraMatching *spectraMatchingForm;
	PeptideFragmentationWidget *peptideFragmentation;

	BookmarkTableDockWidget *bookmarkedPeaks;
	SuggestPopup *suggestPopup;
	HeatMap *heatmap;
	GalleryWidget *galleryWidget;
	ScatterPlot *scatterplot;
	TreeMap *treemap;
	SpectralHitsDockWidget *spectralHitsDockWidget;

	SettingsForm *settingsForm;
	PeakDetectionDialog *peakDetectionDialog;
	PollyElmavenInterfaceDialog *pollyElmavenInterfaceDialog;
	AwsBucketCredentialsDialog *awsBucketCredentialsDialog;
	AlignmentDialog* alignmentDialog;
	// RconsoleWidget* rconsoleDockWidget;
	mzFileIO*             fileLoader; //TODO: Sahil, Added while merging projectdockwidget
    //Added when merged with Maven776 - Kiran
    Pillow::HttpServer*	  embededhttpserver;
	QProgressBar *progressBar;

	Compound * threadCompound;


	int sampleCount() {
		return samples.size();
	}
	EicWidget* getEicWidget() {
		return eicWidget;
	}
	SpectraWidget* getSpectraWidget() {
		return spectraWidget;
	}
	PathwayWidget* getPathwayWidget() {
		return pathwayWidget;
	}
	ProjectDockWidget* getProjectWidget() {
		return projectDockWidget;
	}
	// RconsoleWidget* getRconsoleWidget() {
	// 	return rconsoleDockWidget;
	// }
	BookmarkTableDockWidget* getBookmarkedPeaks() {
		return bookmarkedPeaks;
	}

    QList< QPointer<TableDockWidget> > getPeakTableList() { return groupTables; } //TODO: Sahil - Kiran, Added while merging mainwindow

	ClassifierNeuralNet* getClassifier() {
		return clsf;
	}

	MatrixXf getIsotopicMatrix(PeakGroup* group);
	MatrixXf getIsotopicMatrixIsoWidget(PeakGroup* group);
	void isotopeC13Correct(MatrixXf& MM, int numberofCarbons, map<unsigned int, string> carbonIsotopeSpecies);
	void autoSaveSignal();
	void normalizeIsotopicMatrix(MatrixXf &MM);

    void savePeakTableAsMzRoll(TableDockWidget* peaksTable,
                               QString fileName,
                               QString tableName);

    mzSample* getSampleByName(QString sampleName); //TODO: Sahil, Added this while merging mzfile
	void setIsotopicPlotStyling();
	//TODO: Sahil - Kiran, Removed while merging mainwindow
	// mzSample* getSample(int i) {
	// 	assert(i < samples.size());
	// 	return (samples[i]);
	// }
	inline vector<mzSample*> getSamples() {
		return samples;
	}
	inline void reBootApp() {
		Q_EMIT(reBoot());
	}
	vector<mzSample*> getVisibleSamples();

	PeakGroup::QType getUserQuantType();

        /**
         * @brief Converts `EL_MAVEN_VERSION" macro value to a usable QString
         * and returns it.
         * @return Application version as a QString.
         */
        static QString appVersion();

	void saveSettingsToLog();

    bool updateSamplePathinMzroll(QStringList filelist);
	void setValue(int value);
	//TODO: Sahil - Kiran, removed while merging mainwindow
	// bool isSampleFileType(QString filename);
        // bool isProjectFileType(QString filename);

    /**
     * @brief Save a session as an emDB project.
     * @param explicitSave This boolean value tells the method whether save
     * command was issued explicitly by the user. False by default.
     */
    void saveProject(bool explicitSave=false);

    /**
     * @brief Call appropriate save methods for the filename set in
     * `_currentProjectName` variable.
     * @param tablesOnly Save only peak tables in the project file.
     */
    void saveProjectForFilename(bool tablesOnly=false);

    /**
     * @brief Stores whether a timestamp file is being used to save in the
     * background.
     */
    bool timestampFileExists;

	void loadPollySettings(QString fileName);
Q_SIGNALS:
	void valueChanged(int newValue);
	void saveSignal();
	void undoAlignment(QList<PeakGroup>);
	void reBoot();
    void metaCsvFileLoaded();
    void loadedSettings();

protected:
	void closeEvent(QCloseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

public Q_SLOTS:
    void togglePerGroupAlignmentWidget();
    void toggleAllGroupAlignmentWidget();
    void toggleSampleRtWidget();
	void showAlignmentErrorDialog(QString errorMessage);
	void setMassCutoffType(QString massCutoffType);
	void printvalue();
    void autosaveProject();
	QDockWidget* createDockWidget(QString title, QWidget* w);
	void showPeakInfo(Peak*);
	void setProgressBar(QString, int step, int totalSteps);
	void setStatusText(QString text = QString::null);
	void setMzValue();
	void setMzValue(float mz1, float mz2 = 0.0);
	void loadModel();
	void refreshIntensities();
    void loadCompoundsFile();
    void loadCompoundsFile(QString filename, bool threaded=true);
    void loadMetaInformation();
    bool loadMetaInformation(QString filename);
    int loadMetaCsvFile(string filename);
	void loadPathwaysFolder(QString& pathwaysFolder);
	void showAlignmentWidget();
	void showspectraMatchingForm();
	void showsettingsForm();
	void sendAnalytics();
	void openAWSDialog();
	void analyticsBoxPlot();
	void analyticsAverageSpectra();
	void plotAlignmentVizAllGroupGraph(QList<PeakGroup> allgroups);
	void createPeakTable(QString);

	void setIonizationModeLabel();
	void setFilterLine();
	void setInjectionOrderFromTimeStamp();

    void mzrollLoadDB(QString dbname);

	bool addSample(mzSample* sample);
        // void compoundDatabaseSearch();
        void showPeakdetectionDialog();  // TODO: Sahil - Kiran, Added while
                                         // merging mainwindow
		void showPollyElmavenInterfaceDialog();
	void setUrl(QString url, QString link = QString::null);
	void setUrl(Compound*);
	void setUrl(Reaction*);
	void setFormulaFocus(QString formula);
	void Align();
	void UndoAlignment();
	void spectaFocused(Peak* _peak);
	bool checkCompoundExistance(Compound* c);
	void setCompoundFocus(Compound* c);
	void setPathwayFocus(Pathway* p);
	void showFragmentationScans(float pmz);
	QString groupTextExport(PeakGroup* group);
	// void bookmarkPeakGroup(PeakGroup* group); //TODO: Sahil Changed the structure of function
	PeakGroup* bookmarkPeakGroup(PeakGroup* group);
	void setClipboardToGroup(PeakGroup* group);
	// void bookmarkPeakGroup();
    PeakGroup* bookmarkPeakGroup();
	void reorderSamples(PeakGroup* group);
	void findCovariants(Peak* _peak);
	void reportBugs();
	void updateEicSmoothingWindow(int value);
    bool setPeptideSequence(QString peptideSeq);
	void open();
	void print();
	void exportPDF();
	void exportSVG();
	void setPeakGroup(PeakGroup* group);
	void showDockWidgets();
	void hideDockWidgets();
	//void terminateTheads();
	void doSearch(QString needle);
	//void setupSampleColors();
        // void showMassSlices();
        void showSRMList();
        void addToHistory(const mzSlice& slice);
        void historyNext();
	void historyLast();
	void getLinks(Peak* peak);
	void markGroup(PeakGroup* group, char label);
    //Added when merged with Maven776 - Kiran
    void startEmbededHttpServer();
	int getIonizationMode() {
		return mavenParameters->ionizationMode;
	}
	void setTotalCharge();

	void setUserMassCutoff(double x);
	MassCutoff * getUserMassCutoff() {
		return _massCutoffWindow;
	}
	//Added when merging with Maven776 - Kiran
    SettingsForm* getSettingsForm() { return settingsForm; }
	TableDockWidget* addPeaksTable(QString title);
	//SpectralHitsDockWidget* addSpectralHitsTable(QString title); //TODO: Sahil - Kiran, Added while merging mainwindow

	//Added when merging with Maven776 - Kiran
    void removePeaksTable(TableDockWidget*);
    void removeAllPeakTables();
	BackgroundPeakUpdate* newWorkerThread(QString funcName);
	QWidget* eicWidgetController();
	QWidget* pathwayWidgetController();
    void saveSettings();
    void loadSettings();
    void showNotification(TableDockWidget* table);

    /**
     * @brief Save method that can be called when user explicitly asks for a
     * project save.
     */
    void explicitSave();

    /**
     * @brief Save a session in a filename using a background thread.
     * @param filename String name of a  project file to save to.
     */
    void threadSave(QString filename);

    /**
     * @brief Reset the status of autosave for current El-MAVEN session. Should
     * be called whenever a new project is loaded into the session.
     */
    void resetAutosave();

    /**
     * @brief Get the latest project that was loaded/saved by the user.
     * @return A QString storing the name of the project.
     */
    QString getLatestUserProject();

private Q_SLOTS:
	void createMenus();
	void openURL(int choice);
	void createToolBars();
	void readSettings();
	void writeSettings();
	void toggleIsotopicBarPlot(bool show);
	inline void slotReboot() {
 		qDebug() << "Performing application reboot...";
		QString rep = QDir::cleanPath(QCoreApplication::applicationFilePath());
   		QStringList arguments;
        Q_FOREACH( QString newFileName, this->pendingMzRollSaves) {
			arguments << newFileName;
		}
   		QProcess *myProcess = new QProcess();
    	myProcess->start(rep, arguments);
		settings->setValue("closeEvent", 1);
		writeSettings();
		QCoreApplication::quit();
	};

	inline void deleteCrashFileTables() {
		unsigned int size = settings->beginReadArray("crashTables");
		for (unsigned int i = 0; i < size; ++i) {
			settings->setArrayIndex(i);
			QFile file (settings->value("crashTable").toString());
    		file.remove();
		}
		settings->endArray();
		settings->beginWriteArray("crashTables");
		settings->endArray();
		settings->sync();
    };

    void _setStatusString(QString);
    void _showEMDBProgressBar(QString projectFilename);
    void _updateEMDBProgressBar(int progress, int finish);
    void _postCompoundsDBLoadActions(QString filename, int compoundCount);

private:
	int m_value;
	Analytics* analytics;
	QSettings* settings;
	ClassifierNeuralNet* clsf;
	QList<QPointer<TableDockWidget> > groupTables;
	//Added when merging with Maven776 - Kiran
    QMap< QPointer<TableDockWidget>, QToolButton*> groupTablesButtons;
	EicWidget *eicWidget; //plot of extractred EIC
	History history;

	MassCutoff *_massCutoffWindow;

	QToolBar* sideBar;

    vector<string> unloadableFiles;

    QProgressDialog* _loadProgressDialog;

        QToolButton* addDockWidgetButton(QToolBar*, QDockWidget*, QIcon, QString);

    /**
     * @brief Name of the project to which all threaded saving will be done.
     */
    QString _currentProjectName;

    /**
     * @brief Name of the project that was last loaded or saved and will be used
     * when saving from explicit user command or final save when exiting app.
     */
    QString _latestUserProjectName;

    QString newFileName;

    QString _newAutosaveFile();
    void _setProjectFilenameIfEmpty();
    QString _getProjectFilenameFromProjectDockWidget();
    void _saveMzRollList(QString projectFileName);
    void _saveAllTablesAsMzRoll();
    void checkCorruptedSampleInjectionOrder();
    void warningForInjectionOrders(QMap<int, QList<mzSample*>>, QList<mzSample*>);
    void _notifyIfBadCompoundsDB(QString filename, bool failedToLoadCompletely);
};

struct FileLoader {
	FileLoader() {
	}
	;
	typedef mzSample* result_type;

	mzSample* operator()(const QString filename) {
		mzSample* sample = MainWindow::loadSample(filename);
		return sample;
	}
};

class MainWindowWidgetAction : public QWidgetAction
{
    public:
        MainWindow* mw;
        MainWindowWidgetAction(QObject *parent, MainWindow* mainwindow, QString btnType) : QWidgetAction(parent) {
            btnName = btnType;
            mw = mainwindow;
        }
        virtual ~MainWindowWidgetAction() {}

    protected:
        /**
        * [This is a virtual function of class QWidgetAction. This function gets called when we create
            instance of class QWidgetAcion or class inherting QWidgetAction. This widget creates custom Widgets]
        * @param parent [parent of the instance]
        */
        virtual QWidget *createWidget(QWidget *parent);

    private:
        QString btnName;
};

#endif
