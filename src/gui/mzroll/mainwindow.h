#ifndef  BODE_INCLUDED
#define  BODE_INCLUDED

#include <csignal>
#include <ctime>
#include <sstream>

#include <QWidgetAction>

#include "database.h"
#include "history.h"
#include "PeakGroup.h"
#include "stable.h"

class QCustomPlot;
class QDoubleSpinBox;
class QSpinBox;

class Adduct;
class SRMList;
class Controller;
class SettingsForm;
class EicWidget;
class PlotDockWidget;
class BackgroundOpsThread;
class PeakDetectionDialog;
class PollyElmavenInterfaceDialog;
class AlignmentDialog;
class SpectraWidget;
class GroupRtWidget;
class AlignmentVizAllGroupsWidget;
class IsotopicPlots;
class AdductWidget;
class LigandWidget;
class IsotopeWidget;
class MassCalcWidget;
class TreeDockWidget;
class TableDockWidget;
class BookmarkTableDockWidget;
class PeakTableDockWidget;
class SampleRtWidget;
class IsotopeDialog;
class IsotopePlotDockWidget;
class IsotopePlot;
class Classifier;
class ClassifierNeuralNet;
class ClassifierNaiveBayes;
class groupClassifier;
class svmPredictor;
class ScatterPlot;
class SuggestPopup;
class PeakEditor;
class mzFileIO;
class ProjectDockWidget;
class SpectralHit;
class Analytics;
class AutoSave;
class MavenParameters;
class LibraryManager;
class Mixpanel;
class InfoDialog;
class ProjectSaveWorker;
class TempProjectSaveWorker;
class CorrelationTable;

enum ThemeType : int {
    ElMavenLight = 0,
    ElMavenDark = 1,
};

QPalette namedColorSchemePalette(ThemeType x);

extern Database DB;

class MainWindow: public QMainWindow {
Q_OBJECT

public:
	int value() const { return m_value; }
    MainWindow(Controller* controller,QWidget *parent = 0);
    ~MainWindow();
	QSettings* getSettings() {
		return settings;
	}
	vector<mzSample*> samples;		//list of loaded samples
	static mzSample* loadSample(QString filename);
	int totalCharge = 0;
	bool samplesAlignedFlag = false;
	map<pair<string,string>, double> deltaRt;

    Controller* getController() {
        return _controller;
    }

	Analytics* getAnalytics(){
		return analytics;
	}

    ProjectSaveWorker* saveWorker;
    TempProjectSaveWorker* autosaveWorker;
	MavenParameters* mavenParameters;
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

	SpectraWidget *spectraWidget;
	SpectraWidget* fragSpectraWidget;
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
	LigandWidget *ligandWidget;
	IsotopeWidget *isotopeWidget;
	TreeDockWidget *covariantsPanel;
	TreeDockWidget *fragPanel;
	TreeDockWidget *srmDockWidget;
	QDockWidget *spectraDockWidget;
	QDockWidget* fragSpectraDockWidget;
    QDockWidget *groupRtDockWidget;
	QDockWidget *alignmentVizAllGroupsDockWidget;
    ScatterPlot *scatterDockWidget;
	QDockWidget *treeMapDockWidget;
	ProjectDockWidget *projectDockWidget;

	BookmarkTableDockWidget *bookmarkedPeaks;
	SuggestPopup *suggestPopup;
	ScatterPlot *scatterplot;

	IsotopeDialog *isotopeDialog;
	SettingsForm *settingsForm;
	PeakDetectionDialog *peakDetectionDialog;
	PollyElmavenInterfaceDialog *pollyElmavenInterfaceDialog;
	AlignmentDialog* alignmentDialog;
    mzFileIO* fileLoader;
	QProgressBar *progressBar;

	int sampleCount() {
		return samples.size();
	}
	EicWidget* getEicWidget() {
		return eicWidget;
	}
	SpectraWidget* getSpectraWidget() {
		return spectraWidget;
	}
	ProjectDockWidget* getProjectWidget() {
		return projectDockWidget;
	}

	BookmarkTableDockWidget* getBookmarkedPeaks() {
		return bookmarkedPeaks;
	}

    QList< QPointer<TableDockWidget> > getPeakTableList() { return groupTables; } //TODO: Sahil - Kiran, Added while merging mainwindow

	ClassifierNeuralNet* getClassifier() {
		return clsf;
	}

        LibraryManager* getLibraryManager() { return _libraryManager; }

    MatrixXf getIsotopicMatrix(PeakGroup* group, bool barplot = false);
	MatrixXf getIsotopicMatrixIsoWidget(PeakGroup* group);
	void isotopeC13Correct(MatrixXf& MM, int numberofCarbons, map<unsigned int, string> carbonIsotopeSpecies);
    void autoSaveSignal(QList<shared_ptr<PeakGroup>> groups = {});
    void normalizeIsotopicMatrix(MatrixXf &MM);

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
    void setUserQuantType(QString type);

        /**
         * @brief Converts `EL_MAVEN_VERSION" macro value to a usable QString
         * and returns it.
         * @return Application version as a QString.
         */
        static QString appVersion();

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
     * @param groupsToBeSaved If supplied, save only the given list of
     * `PeakGroup` objects in the project file. This assumes that the rest of
     * the project already contains the other necessary information for session
     * restoration.
     */
    void saveProjectForFilename(QList<shared_ptr<PeakGroup> > groupsToBeSaved);

	void loadPollySettings(QString fileName);

    /**
     * @brief Obtain the currently "active" table, being browsed, in the main
     * window. By default, the "Bookmark Table" is the active (set upon its
     * creation) table until a new one is created.
     * @return Pointer to the `TableDockWidget` recognized as the active table.
     */
    TableDockWidget* activeTable();

    /**
     * @brief Get the unique table for the given table ID.
     * @param tableId The table ID of the desired table.
     * @return A pointer to the desired table if found, `nullptr` otherwise.
     */
    TableDockWidget* tableForTableId(int tableId);

    CorrelationTable* getCorrelationTable() const {
        return _correlationTable;
    }

Q_SIGNALS:
	void valueChanged(int newValue);
    void saveSignal();
    void saveSignal(QList<shared_ptr<PeakGroup>> groups);
	void undoAlignment(QList<PeakGroup>);
	void reBoot();
    void metaCsvFileLoaded();
    void loadedSettings();
    void updateAllowed();

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
    void autosaveGroups(QList<shared_ptr<PeakGroup>> groups = {});
    void autosaveProject();
	QDockWidget* createDockWidget(QString title, QWidget* w);
	void showPeakInfo(Peak*);
    void setProgressBar(QString text,
                        int progress,
                        int totalSteps,
                        bool highPriority = false);
    void setStatusText(QString text = "");
	void setMzValue();
	void setMzValue(float mz1, float mz2 = 0.0);
	void loadModel();
	void refreshIntensities();
    void loadCompoundsFile();
    void loadCompoundsFile(QString filename, bool threaded=true);
    void loadMetaInformation();
    bool loadMetaInformation(QString filename);
    int loadMetaCsvFile(string filename);
    void showAlignmentWidget();
	void showsettingsForm();
	void sendAnalytics(bool checked = false);
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
	void setFormulaFocus(QString formula);
	void Align();
	void UndoAlignment();
	void spectaFocused(Peak* _peak);
    bool checkCompoundExistance(Compound* c);
    void setCompoundFocus(Compound* compound,
                          Isotope isotope = Isotope(),
                          Adduct* adduct = nullptr);
	void showFragmentationScans(float pmz);
	QString groupTextExport(PeakGroup* group);
	// void bookmarkPeakGroup(PeakGroup* group); //TODO: Sahil Changed the structure of function
    shared_ptr<PeakGroup> bookmarkPeakGroup(shared_ptr<PeakGroup> group);
	void setClipboardToGroup(PeakGroup* group);
	// void bookmarkPeakGroup();
	void reorderSamples(PeakGroup* group);
	void findCovariants(Peak* _peak);
	void reportBugs();
	void updateEicSmoothingWindow(int value);
	void open();
	void print();
	void exportPDF();
    void exportSVG();
    void setPeakGroup(shared_ptr<PeakGroup> group);
	void showDockWidgets();
	void hideDockWidgets();
    void searchForQuery();
    void showSRMList();
    void addToHistory(const mzSlice& slice);
    void historyNext();
	void historyLast();
    void getLinks(Peak* peak);
    void markGroup(shared_ptr<PeakGroup> group, char label);
    int getIonizationMode();
	void setTotalCharge();
    void showWarning(QString message);

    void setUserMassCutoff(double cutoff);
	MassCutoff * getUserMassCutoff() {
		return _massCutoffWindow;
	}
	//Added when merging with Maven776 - Kiran
    SettingsForm* getSettingsForm() { return settingsForm; }
    TableDockWidget* addPeaksTable(const QString& tableTitle="", 
	                               bool hasClassifiedGroups = false);
	//SpectralHitsDockWidget* addSpectralHitsTable(QString title); //TODO: Sahil - Kiran, Added while merging mainwindow

	//Added when merging with Maven776 - Kiran
    void removePeaksTable(TableDockWidget*);
    void removeAllPeakTables();
	BackgroundOpsThread* newWorkerThread(QString funcName);
	QWidget* eicWidgetController();
    void saveSettings();
    void loadSettings();

    /**
     * @brief Save method that can be called when user explicitly asks for a
     * project save.
     */
    void explicitSave();

    /**
     * @brief Save a session in a filename using a background thread.
     * @param filename String name of a  project file to save to.
     * @param saveRawData Whether the emDB should be saved with raw EIC and
     * spectra for peaks. `false` by default.
     * @param saveChromatogram If emDB is saved with raw EIC data, saveChromatogram
     * depicts whether the EIC(s) are to be sliced or whole chromatogram must be saved. 
     */
    void threadSave(const QString filename, const bool saveRawData = false, const bool saveChromatogram = false);

    /**
     * @brief Get the latest project that was loaded/saved by the user.
     * @return A QString storing the name of the project.
     */
    QString getLatestUserProject();

    /**
     * @brief Save the filename of a compound database that should be loaded the
     * next time El-MAVEN is started.
     * @details NIST library files having size greater than 2Mb, are not saved
     * for automatic load when starting the next session.
     * @param filename Save the name of the file.
     */
    void setLastLoadedDatabase(QString filename);

    /**
     * @brief Obtain the global Mixpanel usage tracker.
     * @return A const pointer to a `Mixpanel` object.
     */
    const Mixpanel* getUsageTracker() { return _usageTracker; }

    /**
     * @brief Informs the user that a new version of the application is
     * available and prompts an update operation.
     * @param version The new version of application that has been made
     * available.
     */
    void promptUpdate(QString version);

    /**
     * @brief updateTablePostAlignment Update the attributes of peaks for every
     * group present in all current peak-tables. This method should be called
     * after operations such as alignment, where the scan's RT changes from its
     * original value.
     */
    void updateTablePostAlignment();

	/**
	 * @brief Creates a message box that notifies the user about sample 
	 * uploaded is not supported in El-MAVEN.
	 */ 
	void unsupportedFormat(QStringList unsupportedFileList);

    /**
     * @brief The peak-editor is a widget that will allow the user to edit the
     * RT bounds of individual peaks in a peak-group.
     * @return A pointer to the global peak-editor widget, owned by main window.
     */
    PeakEditor* peakEditor() const { return _peakEditor; }

    /**
     * @brief Tells main window to recognize the given table as the "active"
     * one.
     * @param table Pointer to a `TableDockWidget` object.
     */
    void setActiveTable(TableDockWidget* table);

    /**
     * @brief Instruct the main-window to obey or reject requests for auto-save.
     * @param enabled If `true` enables auto-save. If `false` disables it.
     */
    void setAutosaveEnabled(bool enabled) { _autosaveEnabled = enabled; }

private Q_SLOTS:
	void createMenus();
	void createToolBars();
	void readSettings();
	void writeSettings();
	void toggleIsotopicBarPlot(bool show);
	void sendPeaksGA();
	inline void slotReboot() {
 		qDebug() << "Performing application reboot...";
		QString rep = QDir::cleanPath(QCoreApplication::applicationFilePath());
   		QStringList arguments;
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

    /**
     * @brief Warn the user if the polarity of loaded samples and polarity of
     * NIST library compounds do not match.
     * @details This method should be called whenever loading either a set of
     * samples or loading a NIST metabolite library has successfully completed.
     * The checks will be done with the entire sample set and the currently
     * selected compound library. A notification message will only appear if any
     * of the loaded samples and the selected library are PRM compatible.
     */
    void _warnIfNISTPolarityMismatch();

    void _postProjectLoadActions();
    void _handleUnrecognizedProjectVersion(QString projectFilename);
    void _confirmMsMsType();

private:
    Controller* _controller;
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
    int _statusPriority;
    bool _autosaveEnabled;

    /**
     * @brief A small tool that allows the user to manage their compound
     * databases. It can be used to import, load and remember the location of
     * their libraries over different sessions.
     */
    LibraryManager* _libraryManager;

    PeakEditor* _peakEditor;

        QToolButton* addDockWidgetButton(QToolBar*, QDockWidget*, QIcon, QString);

    /**
     * @brief Name of the project that was last loaded or saved and will be used
     * when saving from explicit user command or final save when exiting app.
     */
    QString _latestUserProjectName;

    Mixpanel* _usageTracker;
    InfoDialog* _infoDialog;
    CorrelationTable* _correlationTable;

    /**
     * @brief The table currently being browsed by the user. Defaults to the
     * "Bookmark Table".
     */
    TableDockWidget* _activeTable;

    QString _getNewProjectFilename();
    QString _getProjectFilenameFromProjectDockWidget();
    void checkCorruptedSampleInjectionOrder();
    void warningForInjectionOrders(QMap<int, QList<mzSample*>>, QList<mzSample*>);

    /**
     * @brief Notfiy the user about bad entries in compound database.
     * @param filename The filename of the partially database loaded.
     * @param failedToLoadCompletely Whether the loading of compounds from DB
     * failed completely, i.e., no compounds were loaded.
     */
    void _notifyIfBadCompoundsDB(QString filename, bool failedToLoadCompletely);

    /**
     * @brief This method allows checking whether a version of the application
     * has been run before on a user's system.
     * @details If the current running version does not exist in the record, a
     * `Logger` is used to add the version to the record, which is a file
     * reserved for this purpose. The record will be used in future checks to
     * deduce which versions have already been run before.
     * @return `true` if the current version has been run on the system before,
     * false otherwise.
     */
    bool _versionRecordExists();
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
