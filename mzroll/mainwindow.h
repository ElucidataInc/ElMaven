#ifndef  BODE_INCLUDED
#define  BODE_INCLUDED

#include "stable.h"
#include "globals.h"
#include <ctime>
#include <sstream>
#include "../libmaven/mavenparameters.h"
#include "scatterplot.h"
#include "eicwidget.h"
#include "settingsform.h"
#include "pathwaywidget.h"
#include "spectrawidget.h"
#include "alignmentvizwidget.h"
#include "alignmentvizallgroupswidget.h"
#include "qcustomplot.h"
#include "masscalcgui.h"
#include "adductwidget.h"
#include "ligandwidget.h"
#include "isotopeswidget.h"
#include "treedockwidget.h"
#include "tabledockwidget.h"
#include "peakdetectiondialog.h"
#include "alignmentdialog.h"
//#include "rconsoledialog.h"
#include "background_peaks_update.h"
#include "heatmap.h"
#include "treemap.h"
#include "note.h"
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


class SettingsForm;
class EicWidget;
class PlotDockWidget;
class BackgroundPeakUpdate;
class PeakDetectionDialog;
class AlignmentDialog;
//class RConsoleDialog;
class SpectraWidget;
class AlignmentVizWidget;
class AlignmentVizAllGroupsWidget;
class QCustomPlot;
class IsotopicPlots;
class AdductWidget;
class LigandWidget;
class PathwayWidget;
class IsotopeWidget;
class MassCalcWidget;
class TreeDockWidget;
class TableDockWidget;
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

class AutoSave: public QThread {
Q_OBJECT

public Q_SLOTS:
	void saveMzRoll();
	void saveMzRollAllTables();
	void savePeaksTable(TableDockWidget* peaksTable, QString fileName, QString tableName);

public:
	AutoSave(QWidget*);
	void setMainWindow(MainWindow*);
	bool doAutosave;
	int askAutosave;
	QString fileName;
	QString newFileName;
	MainWindow* _mainwindow;
};

class MainWindow: public QMainWindow {
Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	QSettings* getSettings() {
		return settings;
	}
	vector<mzSample*> samples;		//list of loadded samples
	static mzSample* loadSample(QString filename);
	int peaksMarked = 0;
	int noOfPeakTables = 0;
	bool allPeaksMarked = false;
	bool aligned = false;
	map<pair<string,string>, double> deltaRt;

	AutoSave* autosave;
	MavenParameters* mavenParameters;
	QSqlDatabase localDB;					//local database
	QDoubleSpinBox *ppmWindowBox;
	QLineEdit *searchText;
	QComboBox *quantType;
	QLabel *statusText;
	QStringList pathlist;

	PathwayWidget *pathwayWidget;
	SpectraWidget *spectraWidget;
	AlignmentVizWidget *alignmentVizWidget;
	AlignmentVizAllGroupsWidget * alignmentVizAllGroupsWidget;
	QCustomPlot *customPlot;
	QCustomPlot *alignmentVizPlot;
	QCustomPlot *alignmentVizAllGroupsPlot;
	MassCalcWidget *massCalcWidget;
	AdductWidget *adductWidget;
	LigandWidget *ligandWidget;
	IsotopeWidget *isotopeWidget;
	TreeDockWidget *covariantsPanel;
	TreeDockWidget *fragPanel;
	TreeDockWidget *pathwayPanel;
	TreeDockWidget *srmDockWidget;
	//TreeDockWidget   *peaksPanel;
	QDockWidget *spectraDockWidget;
	QDockWidget *alignmentVizDockWidget;
	QDockWidget *alignmentVizAllGroupsDockWidget;
	QDockWidget *isotopePlotsDockWidget;
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

	TableDockWidget *bookmarkedPeaks;
	SuggestPopup *suggestPopup;
	HeatMap *heatmap;
	GalleryWidget *galleryWidget;
	ScatterPlot *scatterplot;
	TreeMap *treemap;
	SpectralHitsDockWidget *spectralHitsDockWidget;

	SettingsForm *settingsForm;
	PeakDetectionDialog *peakDetectionDialog;
	AlignmentDialog* alignmentDialog;
	// RconsoleWidget* rconsoleDockWidget;
	mzFileIO*             fileLoader; //TODO: Sahil, Added while merging projectdockwidget
    //Added when merged with Maven776 - Kiran
    Pillow::HttpServer*	  embededhttpserver;
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
	PathwayWidget* getPathwayWidget() {
		return pathwayWidget;
	}
	ProjectDockWidget* getProjectWidget() {
		return projectDockWidget;
	}
	// RconsoleWidget* getRconsoleWidget() {
	// 	return rconsoleDockWidget;
	// }
	TableDockWidget* getBookmarkedPeaks() {
		return bookmarkedPeaks;
	}

    QList< QPointer<TableDockWidget> > getPeakTableList() { return groupTables; } //TODO: Sahil - Kiran, Added while merging mainwindow
	
	Classifier* getClassifier() {
		return clsf;
	}

	MatrixXf getIsotopicMatrix(PeakGroup* group);
	MatrixXf getIsotopicMatrixIsoWidget(PeakGroup* group);
	void isotopeC13Correct(MatrixXf& MM, int numberofCarbons, map<unsigned int, string> carbonIsotopeSpecies);
	void autoSaveSignal();

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
	inline void reBootApp(QString fileName = NULL) {
		Q_EMIT(reBoot(fileName));
	}
	vector<mzSample*> getVisibleSamples();

	PeakGroup::QType getUserQuantType();

	//TODO: Sahil - Kiran, removed while merging mainwindow
	// QSqlDatabase* getLocalDB() {
	// 	return &localDB;
	// }
	int versionCheck();
	bool updateSamplePathinMzroll(QStringList filelist);

	//TODO: Sahil - Kiran, removed while merging mainwindow
	// bool isSampleFileType(QString filename);
	// bool isProjectFileType(QString filename);
	bool askAutosave();
Q_SIGNALS:
	void saveSignal();
	void undoAlignment(QList<PeakGroup>);
	void reBoot(QString);

protected:
	void closeEvent(QCloseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

public Q_SLOTS:
	QDockWidget* createDockWidget(QString title, QWidget* w);
	QDockWidget* createDockWidgetIsotopes(QString title, QWidget* w);
	void showPeakInfo(Peak*);
	void setProgressBar(QString, int step, int totalSteps);
	void setStatusText(QString text = QString::null);
	void setMzValue();
	void setMzValue(float mz);
	void loadModel();
	void loadCompoundsFile();
	bool loadCompoundsFile(QString filename);
	void loadMethodsFolder(QString& methodsFolder);
	void loadPathwaysFolder(QString& pathwaysFolder);
	void showAlignmentWidget();
	void plotAlignmentVizAllGroupGraph(QList<PeakGroup> allgroups);

    void mzrollLoadDB(QString dbname);
	
	bool addSample(mzSample* sample);
        // void compoundDatabaseSearch();
        void showPeakdetectionDialog();  // TODO: Sahil - Kiran, Added while
                                         // merging mainwindow
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
    bool setPeptideSequence(QString peptideSeq); //TODO: Sahil, Added while merging point
	//Added when merging with Maven776 - Kiran
	vector<mzSlice*> getSrmSlices(double q1tol, double q3tol, bool associateCompoundNames);

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

	void setIonizationMode(int x);
	int getIonizationMode() {
		return _ionizationMode;
	}

	void setUserPPM(double x);
	double getUserPPM() {
		return _ppmWindow;
	}
	//Added when merging with Maven776 - Kiran
    SettingsForm* getSettingsForm() { return settingsForm; }
	TableDockWidget* addPeaksTable(QString title);
	//SpectralHitsDockWidget* addSpectralHitsTable(QString title); //TODO: Sahil - Kiran, Added while merging mainwindow
	
	//Added when merging with Maven776 - Kiran
    void removePeaksTable(TableDockWidget*);
	BackgroundPeakUpdate* newWorkerThread(QString funcName);
	QWidget* eicWidgetController();
	QWidget* pathwayWidgetController();
	void updateQType(QString);

private Q_SLOTS:
	void createMenus();
	void createToolBars();
	void readSettings();
	void writeSettings();
	void checkSRMList();
	inline void slotReboot(QString mzrollPath = NULL) {
 		qDebug() << "Performing application reboot...";
		QString rep = QDir::cleanPath(QCoreApplication::applicationFilePath());
   		QStringList arguments;
		if (mzrollPath != NULL) arguments << mzrollPath;
   		QProcess *myProcess = new QProcess(this);
    	myProcess->start(rep, arguments);
		settings->setValue("closeEvent", 1);
		autosave->saveMzRoll();
		writeSettings();
		QCoreApplication::quit();
	}

private:
	QSettings* settings;
	Classifier* clsf;
	QList<QPointer<TableDockWidget> > groupTables;
	//Added when merging with Maven776 - Kiran
    QMap< QPointer<TableDockWidget>, QAction*> groupTablesButtons;
	EicWidget *eicWidget; //plot of extractred EIC
	History history;

	int _ionizationMode;
	double _ppmWindow;

	QToolBar* sideBar;

	vector<string> unloadableFiles;

	QToolButton* addDockWidgetButton(QToolBar*, QDockWidget*, QIcon, QString);

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

#endif
