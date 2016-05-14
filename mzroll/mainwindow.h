#ifndef  BODE_INCLUDED
#define  BODE_INCLUDED

#include "stable.h"
#include "globals.h"

#include "scatterplot.h"
#include "eicwidget.h"
#include "settingsform.h"
#include "pathwaywidget.h"
#include "spectrawidget.h"
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

class SettingsForm;
class EicWidget;
class PlotDockWidget;
class BackgroundPeakUpdate;
class PeakDetectionDialog;
class AlignmentDialog;
//class RConsoleDialog;
class SpectraWidget;
class AdductWidget;
class LigandWidget;
class PathwayWidget;
class IsotopeWidget;
class massCalcGui;
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

extern Database DB;


/**
 * \class MainWindow
 *
 * \ingroup mzroll
 *
 * \brief Class for defining widgets and components of the main Qt window.
 *
 * This class is used for defining widgets and components of the main Qt window.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    QSettings* getSettings() {
        return settings;
    }
    vector <mzSample*> samples;		//list of loadded samples
    static mzSample* loadSample(QString filename);



    QSqlDatabase localDB;					//local database
    QDoubleSpinBox 	  *ppmWindowBox;
    QLineEdit         *searchText;
    QComboBox		  *quantType;
    QLabel			  *statusText;

    PathwayWidget     *pathwayWidget;
    SpectraWidget     *spectraWidget;
    massCalcGui       *massCalcWidget;
    AdductWidget     *adductWidget;
    LigandWidget     *ligandWidget;
    IsotopeWidget    *isotopeWidget;
    TreeDockWidget 	*covariantsPanel;
    TreeDockWidget	 *fragPanel;
    TreeDockWidget	 *pathwayPanel;
    TreeDockWidget	 *srmDockWidget;
    //TreeDockWidget   *peaksPanel;
    QDockWidget         *spectraDockWidget;
    QDockWidget         *pathwayDockWidget;
    QDockWidget		 *heatMapDockWidget;
    QDockWidget		 *scatterDockWidget;
    QDockWidget		 *treeMapDockWidget;
    QDockWidget	 	 *galleryDockWidget;
    NotesWidget		 *notesDockWidget;
    ProjectDockWidget    *projectDockWidget;
    SpectraMatching      *spectraMatchingForm;


    TableDockWidget      *bookmarkedPeaks;
    SuggestPopup	 *suggestPopup;
    HeatMap		 *heatmap;
    GalleryWidget	 *galleryWidget;
    ScatterPlot		 *scatterplot;
    TreeMap		 *treemap;


    SettingsForm   *settingsForm;
    PeakDetectionDialog *peakDetectionDialog;
    AlignmentDialog*	  alignmentDialog;
    //RConsoleDialog*	  	  rconsoleDialog;

    QProgressBar  *progressBar;

    int sampleCount() {
        return samples.size();
    }
    EicWidget* getEicWidget() {
        return eicWidget;
    }
    SpectraWidget*  getSpectraWidget() {
        return spectraWidget;
    }
    PathwayWidget* getPathwayWidget() {
        return pathwayWidget;
    }
    ProjectDockWidget* getProjectWidget() {
        return projectDockWidget;
    }

    Classifier* getClassifier() {
        return clsf;
    }

    MatrixXf getIsotopicMatrix(PeakGroup* group);
    void isotopeC13Correct(MatrixXf& MM, int numberofCarbons);

    mzSample* getSample(int i) {
        assert(i < samples.size());
        return(samples[i]);
    }
    inline vector<mzSample*> getSamples() {
        return samples;
    }
    vector<mzSample*> getVisibleSamples();

    PeakGroup::QType getUserQuantType();

    QSqlDatabase* getLocalDB() {
        return &localDB;
    }
    int versionCheck();
    bool isSampleFileType(QString filename);
    bool isProjectFileType(QString filename);

protected:
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

public slots:
    QDockWidget* createDockWidget(QString title, QWidget* w);
    void showPeakInfo(Peak*);
    void setProgressBar(QString, int step, int totalSteps);
    void setStatusText(QString text = QString::null);
    void setMzValue();
    void setMzValue(float mz);
    void loadModel();
    void loadCompoundsFile();
    void loadCompoundsFile(QString filename);
    bool addSample(mzSample* sample);
    void computeKnowsPeaks();
    void setUrl(QString url,QString link=QString::null);
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
    void bookmarkPeakGroup(PeakGroup* group);
    void setClipboardToGroup(PeakGroup* group);
    void bookmarkPeakGroup();
    void reorderSamples(PeakGroup* group);
    void findCovariants(Peak* _peak);
    void reportBugs();
    void updateEicSmoothingWindow(int value);
    vector<mzSlice*> getSrmSlices();

    void open();
    void print();
    void exportPDF();
    void exportSVG();
    void setPeakGroup(PeakGroup* group);
    void showDockWidgets();
    void hideDockWidgets();
    //void terminateTheads();
    void doSearch(QString needle);
    void setupSampleColors();
    void showMassSlices();
    void showSRMList();
    void addToHistory(const mzSlice& slice);
    void historyNext();
    void historyLast();
    void getLinks(Peak* peak);
    void markGroup(PeakGroup* group,char label);

    void setIonizationMode( int x );
    int  getIonizationMode() {
        return _ionizationMode;
    }

    void setUserPPM( double x);
    double getUserPPM() {
        return _ppmWindow;
    }


    TableDockWidget* addPeaksTable(QString title);
    BackgroundPeakUpdate* newWorkerThread(QString funcName);
    QWidget* eicWidgetController();
    QWidget* pathwayWidgetController();

private slots:
    void createMenus();
    void createToolBars();
    void readSettings();
    void writeSettings();

private:
    QSettings* settings;
    Classifier* clsf;
    QList< QPointer<TableDockWidget> > groupTables;
    EicWidget *eicWidget; //plot of extractred EIC
    History history;

    int _ionizationMode;
    double _ppmWindow;

    QToolBar* sideBar;
    QToolButton* addDockWidgetButton( QToolBar*, QDockWidget*, QIcon, QString);

};

struct FileLoader {
    FileLoader() {};
    typedef mzSample* result_type;

    mzSample* operator()(const QString filename) {
        mzSample* sample = MainWindow::loadSample(filename);
        return sample;
    }
};

struct EicLoader {
    enum   PeakDetectionFlag { NoPeakDetection=0, PeakDetection=1 };

    EicLoader(mzSlice* islice,
              PeakDetectionFlag iflag=NoPeakDetection,
              int smoothingWindow=5,
              int smoothingAlgorithm=0,
              float amuQ1=0.1,
              float amuQ2=0.5) {

        slice=islice;
        pdetect=iflag;
        eic_smoothingWindow=smoothingWindow;
        eic_smoothingAlgorithm=smoothingAlgorithm;
        eic_amuQ1=amuQ1;
        eic_amuQ2=amuQ2;
    }

    typedef EIC* result_type;

    EIC* operator()(mzSample* sample) {
        EIC* e = NULL;
        Compound* c = slice->compound;

        if ( ! slice->srmId.empty() ) {
            //cout << "computeEIC srm:" << slice->srmId << endl;
            e = sample->getEIC(slice->srmId);
        } else if ( c && c->precursorMz >0 && c->productMz >0 ) {
            //cout << "computeEIC qqq: " << c->precursorMz << "->" << c->productMz << endl;
            e = sample->getEIC(c->precursorMz, c->collisionEnergy, c->productMz,eic_amuQ1, eic_amuQ2);
        } else {
            //cout << "computeEIC mzrange" << setprecision(7) << slice->mzmin  << " " << slice->mzmax << slice->rtmin  << " " << slice->rtmax << endl;
            e = sample->getEIC(slice->mzmin, slice->mzmax,slice->rtmin,slice->rtmax,1);
        }

        if (e) {
            e->setSmootherType((EIC::SmootherType) (eic_smoothingAlgorithm));
        }

        if(pdetect == PeakDetection && e) e->getPeakPositions(eic_smoothingWindow);
        return e;
    }

    mzSlice* slice;
    PeakDetectionFlag pdetect;
    int eic_smoothingWindow;
    int eic_smoothingAlgorithm;
    float eic_amuQ1;
    float eic_amuQ2;
};

#endif
