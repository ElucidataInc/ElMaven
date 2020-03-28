#include <QStandardPaths>
#include <QList>
#include <QRegExp>
#include <qcustomplot.h>

#include "SRMList.h"
#include "adductwidget.h"
#include "alignmentdialog.h"
#include "alignmentvizallgroupswidget.h"
#include "common/analytics.h"
#include "common/mixpanel.h"
#include "animationcontrol.h"
#include "awsbucketcredentialsdialog.h"
#include "background_peaks_update.h"
#include "Compound.h"
#include "controller.h"
#include "classifierNeuralNet.h"
#include "datastructures/adduct.h"
#include "eiclogic.h"
#include "eicwidget.h"
#include "gallerywidget.h"
#include "globals.h"
#include "groupClassifier.h"
#include "grouprtwidget.h"
#include "heatmap.h"
#include "infodialog.h"
#include "isotopedialog.h"
#include "isotopeplot.h"
#include "isotopeplotdockwidget.h"
#include "isotopeswidget.h"
#include "librarymanager.h"
#include "ligandwidget.h"
#include "common/logger.h"
#include "logwidget.h"
#include "mainwindow.h"
#include "masscalcgui.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "messageBoxResize.h"
#include "mzfileio.h"
#include "mzSample.h"
#include "note.h"
#include "noteswidget.h"
#include "notificator.h"
#include "pathwaywidget.h"
#include "peakdetectiondialog.h"
#include "PeakDetector.h"
#include "Peptide.hpp"
#include "peptidefragmentation.h"
#include "pollyelmaveninterface.h"
#include "projectdockwidget.h"
#include "qdownloader.h"
#include "remotespectrahandler.h"
#include "samplertwidget.h"
#include "Scan.h"
#include "scatterplot.h"
#include "settingsform.h"
#include "spectralhit.h"
#include "spectralhitstable.h"
#include "spectramatching.h"
#include "spectrawidget.h"
#include "suggest.h"
#include "svmPredictor.h"
#include "tabledockwidget.h"
#include "treedockwidget.h"
#include "treemap.h"
#include "updatedialog.h"
#include "videoplayer.h"
#include "eiclogic.h"

#ifdef WIN32
#include <windows.h>
#endif
#define _STR(X) #X
#define STR(X) _STR(X)

QDataStream &operator<<(QDataStream &out, const mzSample*) {
	return out;
}
QDataStream &operator>>(QDataStream &in, mzSample*) {
	return in;
}
QDataStream &operator<<(QDataStream &out, const Compound*) {
	return out;
}
QDataStream &operator>>(QDataStream &in, Compound*) {
	return in;
}
QDataStream &operator<<(QDataStream &out, const PeakGroup*) {
	return out;
}
QDataStream &operator>>(QDataStream &in, PeakGroup*) {
	return in;
}
QDataStream &operator<<(QDataStream &out, const Scan*) {
	return out;
}
QDataStream &operator>>(QDataStream &in, Scan*) {
	return in;
}
QDataStream &operator<<(QDataStream &out, const Pathway*) {
	return out;
}
QDataStream &operator>>(QDataStream &in, Pathway*) {
	return in;
}
QDataStream &operator<<(QDataStream &out, const mzSlice*) {
	return out;
}
QDataStream &operator>>(QDataStream &in, mzSlice*) {
	return in;
}
QDataStream &operator<<(QDataStream &out, const mzSlice&) {
	return out;
}
QDataStream &operator>>(QDataStream &in, mzSlice&) {
	return in;
}
QDataStream &operator<<(QDataStream &out, const SpectralHit*) {
	return out;
}
QDataStream &operator>>(QDataStream &in, SpectralHit*) {
	return in;
}


void MainWindow::setValue(int value)
{
    if (value != m_value) {
        m_value = value;
        Q_EMIT valueChanged(value);
    }
}

using namespace mzUtils;

 MainWindow::MainWindow(Controller* controller, QWidget *parent) :
    _controller(controller),
    QMainWindow(parent)
{
    connect( this, SIGNAL (reBoot()), this, SLOT (slotReboot()));
    m_value=0;


    qRegisterMetaType<mzSample*>("mzSample*");
	qRegisterMetaTypeStreamOperators<mzSample*>("mzSample*");

	qRegisterMetaType<Compound*>("Compound*");
	qRegisterMetaTypeStreamOperators<Compound*>("Compound*");

	qRegisterMetaType<Scan*>("Scan*");
	qRegisterMetaTypeStreamOperators<Scan*>("Scan*");

	qRegisterMetaType<PeakGroup*>("PeakGroup*");
	qRegisterMetaTypeStreamOperators<PeakGroup*>("PeakGroup*");

	qRegisterMetaType<Pathway*>("Pathway*");
	qRegisterMetaTypeStreamOperators<Pathway*>("Pathway*");

	qRegisterMetaType<mzSlice*>("mzSlice*");
	qRegisterMetaTypeStreamOperators<mzSlice*>("mzSlice*");

	qRegisterMetaType<mzSlice>("mzSlice");
	qRegisterMetaTypeStreamOperators<mzSlice>("mzSlice");

	qRegisterMetaType<SpectralHit*>("SpectralHit*");
	qRegisterMetaTypeStreamOperators<SpectralHit*>("SpectralHit*");

	qRegisterMetaType<UserNote*>("UserNote*");
	//qRegisterMetaTypeStreamOperators<UserNote*>("UserNote*");

	qRegisterMetaType<QTextCursor>("QTextCursor");



#ifdef Q_OS_MAC
	QDir dir(QApplication::applicationDirPath());
	dir.cdUp();
	dir.cd("plugins");
	QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
	QStringList list = QApplication::libraryPaths();
	qDebug() << "Library Path=" << list;
#endif

	threadCompound = NULL;

    readSettings();
	QString dataDir = ".";
	unloadableFiles.reserve(50);

	setWindowTitle(programName + " " + STR(EL_MAVEN_VERSION));

	//locations of common files and directories
	QString methodsFolder = settings->value("methodsFolder").value<QString>();
	if (!QFile::exists(methodsFolder)) {
		methodsFolder = dataDir + "/" + "methods";
		#ifdef Q_OS_MAC
			methodsFolder = QCoreApplication::applicationDirPath() + "/../../../methods/";
		#endif
	}

	QString pathwaysFolder = settings->value("pathwaysFolder").value<QString>();
	if (!QFile::exists(pathwaysFolder))
		pathwaysFolder = dataDir + "/" + "pathways";

	QString ligandDbFilename = pathwaysFolder + "/"
			+ settings->value("ligandDbFilename").value<QString>();
	if (QFile::exists(ligandDbFilename)) {
		DB.connect(ligandDbFilename.toStdString());
		DB.loadAll();
	}

	QString commonFragments = dataDir + "/" + "FRAGMENTS.csv";
	if (QFile::exists(commonFragments))
		DB.loadFragments(commonFragments.toStdString());

	clsf = new ClassifierNeuralNet();    //clsf = new ClassifierNaiveBayes();
		mavenParameters = new MavenParameters(QString(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QDir::separator() + "lastRun.xml").toStdString());
	_massCutoffWindow = new MassCutoff();


    QString clsfModelFilename;
    QString weightsFile;
    QString modelFile;
    QString appDir;

    #if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
        appDir =  QDir::cleanPath(QApplication::applicationDirPath()) + QDir::separator();
    #endif

    #if defined(Q_OS_MAC)
        appDir =  qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator() + ".." \
              + QDir::separator();
    #endif

    clsfModelFilename = appDir + "default.model";
    weightsFile = appDir + "group.weights";
    modelFile = appDir + "svm.model";

	groupClsf = new groupClassifier();
        groupClsf->loadModel(weightsFile.toStdString());
 
  	groupPred = new svmPredictor();
        groupPred->loadModel(modelFile.toStdString());


   /* double massCutoff=settings->value("compoundMassCutoffWindow").toDouble();
      string massCutoffType=settings->value("massCutoffType").toString().toStdString();
      _massCutoffWindow->setMassCutoffAndType(massCutoff,massCutoffType);
    */

    if (QFile::exists(clsfModelFilename)) {
        settings->setValue("peakClassifierFile", clsfModelFilename);
        clsf->loadModel( clsfModelFilename.toStdString());
        mavenParameters->clsf = getClassifier();
    } else {
        settings->setValue("peakClassifierFile", QString(""));
        clsf->loadModel("");
        mavenParameters->clsf = getClassifier();
    }


	analytics = new Analytics();
	analytics->hitScreenView("MainWindow");
	analytics->sessionStart();

	//QString storageLocation =   QDesktopServices::storageLocation(QDesktopServices::DataLocation);

    //added while merging with Maven776 - Kiran
	//fileLoader
	autosave = new AutoSave(this);

    fileLoader = new mzFileIO(this);
    fileLoader->setMainWindow(this);
	connect(fileLoader, SIGNAL(createPeakTableSignal(QString)), this,  SLOT(createPeakTable(QString)));
    connect(fileLoader, &mzFileIO::addNewSample, this, &MainWindow::addSample);
	//settings dialog
	settingsForm = new SettingsForm(settings, this);
	//progress Bar on the bottom of the page
	statusText = new QLabel(this);
	statusText->setOpenExternalLinks(true);
	statusBar()->addPermanentWidget(statusText, 1);

	progressBar = new QProgressBar(this);
	progressBar->hide();
    statusBar()->addPermanentWidget(progressBar);

    _loadProgressDialog = nullptr;

	QToolButton *btnBugs = new QToolButton(this);
	btnBugs->setIcon(QIcon(rsrcPath + "/bug.png"));
	btnBugs->setToolTip(tr("Feedback!"));
	connect(btnBugs, SIGNAL(clicked()), SLOT(reportBugs()));
	statusBar()->addPermanentWidget(btnBugs, 0);

	// hide this button until it does something meaningful.
	btnBugs->setHidden(true);

	setWindowIcon(QIcon(":/images/icon.png"));

	//dock widgets
	setDockOptions(
			QMainWindow::AllowNestedDocks | QMainWindow::VerticalTabs
					| QMainWindow::AnimatedDocks);

	//set main dock widget
	eicWidget = new EicWidget(this);
	spectraWidget = new SpectraWidget(this);
    fragSpectraWidget = new SpectraWidget(this, true);
	customPlot = new QCustomPlot(this);
    groupRtVizPlot = new QCustomPlot(this);
    sampleRtVizPlot = new QCustomPlot(this);
	alignmentVizAllGroupsPlot = new QCustomPlot(this);	
	pathwayWidget = new PathwayWidget(this);
	adductWidget = new AdductWidget(this);
	isotopeWidget = new IsotopeWidget(this);
	isotopePlot = new IsotopePlot(this);

    _libraryManager = new LibraryManager(this);
    massCalcWidget = new MassCalcWidget(this);
	covariantsPanel = new TreeDockWidget(this, "Covariants", 3);
	fragPanel = new TreeDockWidget(this, "Fragmentation Events", 5);
	fragPanel->setupScanListHeader();
	pathwayPanel = new TreeDockWidget(this, "Pathways", 1);
	srmDockWidget = new TreeDockWidget(this, "SRM List", 1);
	ligandWidget = new LigandWidget(this);
	heatmap = new HeatMap(this);
	galleryWidget = new GalleryWidget(this);
	bookmarkedPeaks = new BookmarkTableDockWidget(this);

    sampleRtWidget = new SampleRtWidget(this);
    sampleRtWidget->setWidget(sampleRtVizPlot);

	isotopePlotDockWidget = new IsotopePlotDockWidget(this);
	isotopePlotDockWidget->setWidget(customPlot);

	//treemap	 = 	  new TreeMap(this);
	//peaksPanel	= new TreeDockWidget(this,"Group Information", 1);
	spectraDockWidget = createDockWidget("Spectra", spectraWidget);
	fragSpectraDockWidget = createDockWidget("Fragmentation Spectra", fragSpectraWidget);

    groupRtDockWidget = createDockWidget("Per Group Deviation", groupRtVizPlot);
    groupRtWidget = new GroupRtWidget(this,groupRtDockWidget);

    alignmentVizAllGroupsDockWidget = createDockWidget("All Groups Deviation", alignmentVizAllGroupsPlot);
    alignmentVizAllGroupsWidget = new AlignmentVizAllGroupsWidget(this, alignmentVizAllGroupsDockWidget);


    pathwayDockWidget = createDockWidget("PathwayViewer", pathwayWidget);
	heatMapDockWidget = createDockWidget("HeatMap", heatmap);
	galleryDockWidget = createDockWidget("Gallery", galleryWidget);
	scatterDockWidget = new ScatterPlot(this);
	notesDockWidget = new NotesWidget(this);
	projectDockWidget = new ProjectDockWidget(this);
	logWidget = new LogWidget(this, std::cout);
	// rconsoleDockWidget = new RconsoleWidget(this);
	spectralHitsDockWidget = new SpectralHitsDockWidget(this, "Spectral Hits");
	peptideFragmentation = new PeptideFragmentationWidget(this);
	
	setIsotopicPlotStyling();

	// prepare x axis:
    groupRtVizPlot->xAxis->setTicks(false);
    groupRtVizPlot->xAxis->setBasePen(QPen(Qt::white));
    groupRtVizPlot->xAxis->grid()->setVisible(false);
	// prepare y axis:
    groupRtVizPlot->yAxis->setTicks(false);
    groupRtVizPlot->yAxis->setBasePen(QPen(Qt::white));
    groupRtVizPlot->yAxis->grid()->setVisible(true);

 	// prepare x axis:
    sampleRtVizPlot->xAxis->setTicks(false);
    sampleRtVizPlot->xAxis->setBasePen(QPen(Qt::white));
    sampleRtVizPlot->xAxis->grid()->setVisible(false);
	// prepare y axis:
    sampleRtVizPlot->yAxis->setTicks(false);
    sampleRtVizPlot->yAxis->setBasePen(QPen(Qt::white));
    sampleRtVizPlot->yAxis->grid()->setVisible(true);


	// prepare x axis:
	alignmentVizAllGroupsPlot->xAxis->setTicks(false);
	alignmentVizAllGroupsPlot->xAxis->setBasePen(QPen(Qt::white));
	alignmentVizAllGroupsPlot->xAxis->grid()->setVisible(false);	
	// prepare y axis:
	alignmentVizAllGroupsPlot->yAxis->setTicks(false);
	alignmentVizAllGroupsPlot->yAxis->setBasePen(QPen(Qt::white));
	alignmentVizAllGroupsPlot->yAxis->grid()->setVisible(true);

	ligandWidget->setVisible(true);
	pathwayPanel->setVisible(false);
	covariantsPanel->setVisible(false);

	isotopeWidget->setVisible(false);
	massCalcWidget->setVisible(false);
	fragPanel->setVisible(false);
	bookmarkedPeaks->setVisible(false);
	pathwayDockWidget->setVisible(false);
	spectraDockWidget->setVisible(false);
	fragSpectraDockWidget->setVisible(false);
    groupRtDockWidget->setVisible(false);
    sampleRtWidget->setVisible(false);
	alignmentVizAllGroupsDockWidget->setVisible(false);
	scatterDockWidget->setVisible(false);
	notesDockWidget->setVisible(false);
	heatMapDockWidget->setVisible(false);
	galleryDockWidget->setVisible(false);
	projectDockWidget->setVisible(true);
	logWidget->setVisible(false);
	// rconsoleDockWidget->setVisible(false);
	spectralHitsDockWidget->setVisible(false);
    peptideFragmentation->setVisible(false);
	srmDockWidget->setVisible(false);
	isotopePlotDockWidget->setVisible(false);
	
	//treemap->setVisible(false);
	//peaksPanel->setVisible(false);
	//treeMapDockWidget =  createDockWidget("TreeMap",treemap);

    //added while merging with Maven776 - Kiran
    //create toolbar for SRM dock widget
    srmDockWidget->setQQQToolBar();

	//
	//DIALOGS
	//

    isotopeDialog = new IsotopeDialog(this);

	peakDetectionDialog = new PeakDetectionDialog(this);
	peakDetectionDialog->setSettings(settings);
    connect(peakDetectionDialog, SIGNAL(findPeaksClicked()),
            this, SLOT(sendPeaksGA()));
	
	// pollyelmavengui dialog
	pollyElmavenInterfaceDialog = new PollyElmavenInterfaceDialog(this);

	//alignment dialog
	alignmentDialog = new AlignmentDialog(this);
	alignmentDialog->setMainWindow(this);
	connect(alignmentDialog->alignButton, SIGNAL(clicked()), SLOT(Align()));
	connect(alignmentDialog->UndoAlignment, SIGNAL(clicked()),
			SLOT(UndoAlignment()));

	//rconsole dialog
	//rconsoleDialog	 =  new RConsoleDialog(this);


	spectraMatchingForm = new SpectraMatching(this);

	connect(scatterDockWidget, SIGNAL(groupSelected(PeakGroup*)),
			SLOT(setPeakGroup(PeakGroup*)));
	pathwayWidgetController();


    vidPlayer = new VideoPlayer(settings, this, nullptr);

	addDockWidget(Qt::LeftDockWidgetArea, ligandWidget, Qt::Vertical);
	addDockWidget(Qt::LeftDockWidgetArea, pathwayPanel, Qt::Vertical);
	addDockWidget(Qt::LeftDockWidgetArea, projectDockWidget, Qt::Vertical);

	ligandWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
	pathwayPanel->setAllowedAreas(Qt::LeftDockWidgetArea);
	projectDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);

	addDockWidget(Qt::BottomDockWidgetArea, spectraDockWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, fragSpectraDockWidget, Qt::Horizontal);
    addDockWidget(Qt::BottomDockWidgetArea, groupRtDockWidget, Qt::Horizontal);
    addDockWidget(Qt::BottomDockWidgetArea, sampleRtWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, alignmentVizAllGroupsDockWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, isotopePlotDockWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, pathwayDockWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, covariantsPanel, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, fragPanel, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, scatterDockWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, bookmarkedPeaks, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, notesDockWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, galleryDockWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, srmDockWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, logWidget, Qt::Horizontal);
	// addDockWidget(Qt::BottomDockWidgetArea, rconsoleDockWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, spectralHitsDockWidget,
			Qt::Horizontal);
    addDockWidget(Qt::BottomDockWidgetArea,peptideFragmentation,Qt::Horizontal);

	//addDockWidget(Qt::BottomDockWidgetArea,peaksPanel,Qt::Horizontal);
	//addDockWidget(Qt::BottomDockWidgetArea,treeMapDockWidget,Qt::Horizontal);
	//addDockWidget(Qt::BottomDockWidgetArea,heatMapDockWidget,Qt::Horizontal);

	tabifyDockWidget(ligandWidget, pathwayPanel);
	tabifyDockWidget(ligandWidget, projectDockWidget);

	tabifyDockWidget(spectraDockWidget, massCalcWidget);
	tabifyDockWidget(spectraDockWidget, isotopeWidget);
	tabifyDockWidget(spectraDockWidget, massCalcWidget);
    tabifyDockWidget(spectraDockWidget, groupRtDockWidget);
    tabifyDockWidget(spectraDockWidget, sampleRtWidget);
	tabifyDockWidget(spectraDockWidget, alignmentVizAllGroupsDockWidget);
	tabifyDockWidget(spectraDockWidget, isotopePlotDockWidget);
	tabifyDockWidget(spectraDockWidget, pathwayDockWidget);
	tabifyDockWidget(spectraDockWidget, fragPanel);
	tabifyDockWidget(spectraDockWidget, covariantsPanel);
	tabifyDockWidget(spectraDockWidget, notesDockWidget);
	tabifyDockWidget(spectraDockWidget, galleryDockWidget);
	tabifyDockWidget(spectraDockWidget, logWidget);
	tabifyDockWidget(spectraDockWidget, fragSpectraDockWidget);
	// tabifyDockWidget(rconsoleDockWidget, logWidget);
    tabifyDockWidget(peptideFragmentation,logWidget);

    connect(this, SIGNAL(saveSignal()), this, SLOT(autosaveProject()));
    connect(this,
            SIGNAL(saveSignal(QList<PeakGroup*>)),
            this,
            SLOT(autosaveGroup(QList<PeakGroup*>)));

    connect(fileLoader,
            SIGNAL(updateStatusString(QString)),
            SLOT(_setStatusString(QString)));

    connect(fileLoader,SIGNAL(updateProgressBar(QString,int,int)), SLOT(setProgressBar(QString, int,int)));
    connect(fileLoader,
            SIGNAL(sampleLoaded()),
            this,
            SLOT(_warnIfNISTPolarityMismatch()));

	connect(fileLoader,SIGNAL(sampleLoaded()), this, SLOT(setInjectionOrderFromTimeStamp()));
    connect(fileLoader,SIGNAL(sampleLoaded()),projectDockWidget, SLOT(updateSampleList()));
	connect(fileLoader,SIGNAL(sampleLoaded()), SLOT(showSRMList()));
	connect(fileLoader,SIGNAL(sampleLoaded()), this, SLOT(setIonizationModeLabel()));
	connect(fileLoader,SIGNAL(sampleLoaded()), this, SLOT(setFilterLine()));

    connect(fileLoader,SIGNAL(spectraLoaded()),spectralHitsDockWidget, SLOT(showAllHits()));
    connect(fileLoader,SIGNAL(spectraLoaded()),spectralHitsDockWidget, SLOT(show()));
    connect(fileLoader,SIGNAL(spectraLoaded()),spectralHitsDockWidget, SLOT(raise()));
	connect(fileLoader,SIGNAL(spectraLoaded()), this,SLOT(setIonizationModeLabel()));
	connect(fileLoader,SIGNAL(spectraLoaded()), this, SLOT(setInjectionOrderFromTimeStamp()));

    connect(fileLoader,SIGNAL(projectLoaded()),projectDockWidget, SLOT(updateSampleList()));
    connect(fileLoader,SIGNAL(projectLoaded()),bookmarkedPeaks, SLOT(showAllGroups()));
    connect(fileLoader,SIGNAL(projectLoaded()), SLOT(showSRMList()));
	connect(fileLoader,SIGNAL(projectLoaded()), this,SLOT(setIonizationModeLabel()));
	connect(fileLoader,SIGNAL(projectLoaded()), this,SLOT(deleteCrashFileTables()));
    connect(fileLoader,SIGNAL(projectLoaded()), this, SLOT(setInjectionOrderFromTimeStamp()));
    connect(projectDockWidget, &ProjectDockWidget::samplesDeleted, spectraWidget, &SpectraWidget::clearScans);
    connect(fileLoader,
            SIGNAL(compoundsLoaded(QString, int)),
            this,
            SLOT(_postCompoundsDBLoadActions(QString,int)));

    // EMDB singals and slots
    connect(fileLoader,
            SIGNAL(sqliteDBLoadStarted(QString)),
            SLOT(_showEMDBProgressBar(QString)));
    connect(fileLoader,
            &mzFileIO::settingsLoaded,
            this,
            [=] { _updateEMDBProgressBar(1, 5); });
    connect(fileLoader,
            &mzFileIO::sqliteDBSamplesLoaded,
            this,
            [=] { _updateEMDBProgressBar(2, 5); });
    connect(fileLoader,
            &mzFileIO::sqliteDBPeakTablesCreated,
            this,
            [=] { _updateEMDBProgressBar(3, 5); });
    connect(fileLoader,
            &mzFileIO::sqliteDBAlignmentDone,
            this,
            [=] { _updateEMDBProgressBar(4, 5); });
    connect(fileLoader,
            SIGNAL(sqliteDBPeakTablesPopulated()),
            SLOT(_postProjectLoadActions()));
    connect(fileLoader,
            SIGNAL(sqliteDBUnrecognizedVersion(QString)),
            SLOT(_handleUnrecognizedProjectVersion(QString)));
    connect(fileLoader,
            &mzFileIO::sampleLoadFailed,
            this,
            [=](QList<QString> sampleFiles) {
                QMessageBox msgBox;
                msgBox.setWindowTitle("Sample load failure");
                QString fileHtmlList = "";
                for (auto& filepath : sampleFiles) {
                    QFileInfo fi(filepath);
                    fileHtmlList += "<li>" + fi.fileName();
                }
                auto htmlText = QString("<p>The following sample files "
                                        "failed to load:</p>"
                                        "<ul>%1</ul>").arg(fileHtmlList);
                msgBox.setText(htmlText);
                msgBox.setInformativeText("Please make sure your file import "
                                          "settings do not conflict with the "
                                          "type of samples being loaded.");
                msgBox.setStyleSheet("QMessageBox { font-weight: normal; }");
                msgBox.addButton(QMessageBox::Ok);
                msgBox.exec();
            });

    connect(spectralHitsDockWidget,SIGNAL(updateProgressBar(QString,int,int)), SLOT(setProgressBar(QString, int,int)));
    connect(eicWidget,SIGNAL(scanChanged(Scan*)),spectraWidget,SLOT(setScan(Scan*)));

	qRegisterMetaType<QList<PeakGroup> >("QList<PeakGroup>");
	connect(this, SIGNAL(undoAlignment(QList<PeakGroup> )), this, SLOT(plotAlignmentVizAllGroupGraph(QList<PeakGroup>)));


    setContextMenuPolicy(Qt::NoContextMenu);
    pathwayPanel->setInfo(DB.pathwayDB);

    scatterDockWidget->hide();
    spectralHitsDockWidget->hide();
    peptideFragmentation->hide();
    fragPanel->hide();
    projectDockWidget->raise();
    spectraDockWidget->raise();
	fragSpectraDockWidget->raise();
    groupRtDockWidget->raise();
    sampleRtWidget->raise();
	alignmentVizAllGroupsDockWidget->raise();	

	createToolBars();
	// setIonsizationMode(0);
	currentIntensityName = "Max "+quantType->currentText();

	setIonizationModeLabel();
	setTotalCharge();

  // This been set here why is this here; beacuse of this
  // in the show function of peak detector its been made to set to this
  // value
    setUserMassCutoff(5);
	if (settings->contains("massCutoffWindowBox")) {
		setUserMassCutoff(settings->value("massCutoffWindowBox").toDouble());
	}

	QRectF view = settings->value("mzslice").value<QRectF>();
	if (view.width() > 0 && view.height() > 0) {
		eicWidget->setMzSlice(
				mzSlice(view.x(), view.y(), view.width(), view.height()));
	} else {
		eicWidget->setMzSlice(mzSlice(0, 0, 0, 100));
	}

	createMenus();
	if (ligandWidget) {

		/**
		 * loadsMethodsFolder changes the value of lastDatabaseFile in .conf
		 * inorder to revert the value back to the original, its value was saved and set back to original
		 * Note: the default value present in .conf file is KNOWNS
		*/
	
		QString lastDatabaseFile = settings->value("lastDatabaseFile").value<QString>();
		settings->setValue("lastDatabaseFile",lastDatabaseFile);
	}
	if (pathwayWidget) {
		loadPathwaysFolder(pathwaysFolder);
	}

        setCentralWidget(eicWidgetController());

        if (ligandWidget) {
            if (settings->contains("lastDatabaseFile")) {
                QString lfile =
                    settings->value("lastDatabaseFile").value<QString>();
                QFile testf(lfile);
                qDebug() << "Loading last database" << lfile;
                if (testf.exists())
                    loadCompoundsFile(lfile, false);
            }
            QString databaseSet;
            if (settings->contains("lastCompoundDatabase")) {
                ligandWidget->setDatabase(
                    settings->value("lastCompoundDatabase").toString());
                databaseSet =
                    settings->value("lastCompoundDatabase").toString();
            } else {
                ligandWidget->setDatabase("KNOWNS");
                databaseSet = "KNOWNS";
            }
        }

        setAcceptDrops(true);

	showNormal();	//return from full screen on startup


	//Starting server to fetch remote data - Kiran
	//Added when merged with Maven776
    if ( settings->value("embeded_http_server_autostart").value<bool>() == true) {
         startEmbededHttpServer();
    }
	//check if program exited correctly last time
	if (settings->contains("closeEvent")
			and settings->value("closeEvent").toInt() == 0) {

		setUrl("https://github.com/ElucidataInc/ElMaven/issues",
				"Woops.. did the program crash last time? Would you like to report a bug?");
	}

	logWidget->append("Initiaalization complete..\n");
	//versionCheck(); //TODO: Sahil-Kiran, Removed while merging mainwindow

	settings->setValue("closeEvent", 0);
	peakDetectionDialog->setMavenParameters(settings);


	readSettings();

	QStringList CrashFileList;
	unsigned int size = settings->beginReadArray("crashTables");
	for (unsigned int i = 0; i < size; ++i) {
		settings->setArrayIndex(i);
		//TODO:If the file does not exist it has to be removed from the entry of the Qsettings
		//The above part of the code has to be implemented
		CrashFileList << settings->value("crashTable").toString();
		fileLoader->addFileToQueue(settings->value("crashTable").toString());
	}
	settings->endArray();

	if (CrashFileList.size() > 0) {
		QMessageBox* msgBox = new QMessageBox( this );
		msgBox->setAttribute( Qt::WA_DeleteOnClose );
		QPushButton *restore = msgBox->addButton(tr("Restore"), QMessageBox::ActionRole);
		QPushButton *restoreNO = msgBox->addButton(tr("Don't Restore"), QMessageBox::ActionRole);
		msgBox->setIcon(QMessageBox::Information);
		msgBox->setText(tr("Unfortunately, ElMaven crashed when you were using it earlier.\nHowever, we can restore the application without losing information."));
		msgBox->setModal( true );
		msgBox->open();
		msgBox->exec();
		if (msgBox->clickedButton() == restore) {
			fileLoader->start();
        } else {
			fileLoader->removeAllFilefromQueue();
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
		}
	}

    _usageTracker = new Mixpanel;
    _infoDialog = new InfoDialog(this);
}

MainWindow::~MainWindow()
{
	analytics->sessionEnd();
    delete mavenParameters;
    delete _usageTracker;
}

void MainWindow::promptUpdate(QString version)
{
    qDebug() << "New release"
             << QString("v%1").arg(version)
             << "available.";

    // prompt for update after 15 seconds
    QTimer::singleShot(15000, [this] {
        UpdateDialog prompt(this);
        prompt.exec();
        if (prompt.updateAllowed())
            emit updateAllowed();
    });
}

void MainWindow::sendPeaksGA()
{
    if (isotopeDialog->isotopeDetectionEnabled() && peakDetectionDialog->databaseSearchEnabled()) {
        analytics->hitEvent("Peak Detection", "Find Peaks With Isotopes");
    } else {
        analytics->hitEvent("Peak Detection", "Find Peaks");
    }
}

void MainWindow::showNotification(TableDockWidget* table) {
    QIcon icon = QIcon(":/images/notification.png");
    QString title("");
    QString message("Make your analyses more insightful with Machine learning."
                    "\nView your fluxomics workflow in PollyPhi.");

    if (table->groupCount() == 0 || table->getLabeledGroupCount() == 0)
        return;

    Notificator* fluxomicsPrompt = Notificator::showMessage(icon,
                                                            title,
                                                            message,
                                                            table);
    connect(fluxomicsPrompt,
            SIGNAL(promptClicked()),
            SLOT(showPollyElmavenInterfaceDialog()));
    connect(fluxomicsPrompt,
            &Notificator::promptClicked,
            this,
            [=] {
                analytics->hitEvent("Prompt", "Clicked", "PollyPhi");
                pollyElmavenInterfaceDialog->switchToApp(PollyApp::PollyPhi);
            });
    connect(fluxomicsPrompt,
            SIGNAL(promptClicked(TableDockWidget*)),
            pollyElmavenInterfaceDialog,
            SLOT(setSelectedTable(TableDockWidget*)));
}

void MainWindow::createPeakTable(QString filenameNew) {
    projectDockWidget->setLastOpenedProject(filenameNew);
    TableDockWidget * peaksTable = this->addPeaksTable();
    auto groups = fileLoader->readGroupsXML(filenameNew);
    for (auto group : groups) {
        peaksTable->addPeakGroup(group);
    }
    peaksTable->showAllGroups();
}

QString MainWindow::_newAutosaveFile()
{
    auto now = QDateTime::currentDateTime();
    auto tempFilename = now.toString("dd_MM_yyyy_hh_mm_ss") + ".emDB";
    auto firstSampleFile = getSamples()[0]->fileName;
    auto sampleFileInfo = QFileInfo(QString::fromStdString(firstSampleFile));
    auto samplePath = sampleFileInfo.absolutePath();
    return samplePath + QDir::separator() + tempFilename;
}

AutoSave::AutoSave(MainWindow* mw)
{
    _mainwindow = mw;
    _mainwindow->timestampFileExists = false;
}

void AutoSave::saveProjectWorker(QList<PeakGroup*> groupsToBeSaved)
{
    this->groupsToBeSaved = groupsToBeSaved;
    this->start();
}

void AutoSave::run()
{
    _mainwindow->saveProjectForFilename(groupsToBeSaved);
}

void MainWindow::_setProjectFilenameIfEmpty()
{
    if (this->_currentProjectName.isEmpty()) {
        QString dir = ".";
        if (settings->contains("lastDir")) {
            QString ldir = settings->value("lastDir").value<QString>();
            QDir test(ldir);
            if (test.exists())
                dir = ldir;
        }

        // we prefer "emDB" format as the default
        auto filename =
            QFileDialog::getSaveFileName(this,
                                         "Save Project (.emDB)",
                                         dir,
                                         "El-MAVEN Database Format(*.emDB)");
        if (!filename.isEmpty()) {
            if (!filename.endsWith(".emDB", Qt::CaseInsensitive)) {
                this->_currentProjectName = filename + ".emDB";
            } else {
                this->_currentProjectName = filename;
            }
        }
    }
}

QString MainWindow::_getProjectFilenameFromProjectDockWidget()
{
    auto lastSave = projectDockWidget->getLastSavedTime();
    auto lastLoad = projectDockWidget->getLastOpenedTime();
    if (!projectDockWidget->getLastSavedProject().isEmpty()
            && lastSave > lastLoad)
        return projectDockWidget->getLastSavedProject();
    if (!projectDockWidget->getLastOpenedProject().isEmpty()
            && lastLoad > lastSave)
        return projectDockWidget->getLastOpenedProject();
    return "";
}

QString MainWindow::getLatestUserProject()
{
    if (_latestUserProjectName.isEmpty())
        return _getProjectFilenameFromProjectDockWidget();
    return _latestUserProjectName;
}

void MainWindow::resetAutosave()
{
    if (this->timestampFileExists) {
        while (autosave->isRunning());
        fileLoader->closeSQLiteProject();
        QFile::remove(_currentProjectName);
    }
    this->timestampFileExists = false;
    this->peaksMarked = 0;
    _currentProjectName = "";
}

void MainWindow::autosaveGroup(QList<PeakGroup*> groups)
{
    if (groups.empty() || !this->timestampFileExists) {
        autosaveProject();
        return;
    }

    autosave->saveProjectWorker(groups);
}

void MainWindow::autosaveProject()
{
    if (this->peaksMarked == 1 || this->timestampFileExists) {
        if (!this->timestampFileExists) {
            this->_currentProjectName = this->_newAutosaveFile();
            this->timestampFileExists = true;
        }
        autosave->saveProjectWorker();
    }
}

void MainWindow::explicitSave()
{
    // the user is ordering an explicit save, reset the current project name
    if (timestampFileExists) {
        resetAutosave();
    }

    saveProject(true);
}

void MainWindow::threadSave(QString filename)
{
    // a non-temporary file is being used to save session data,
    // all autosave states should be discarded
    if (timestampFileExists) {
        resetAutosave();
    }

    _currentProjectName = filename;
    _latestUserProjectName = filename;

    QFileInfo fileInfo(filename);
    setWindowTitle(programName
                   + " "
                   + STR(EL_MAVEN_VERSION)
                   + " "
                   + fileInfo.fileName());

    autosave->saveProjectWorker();
}

void MainWindow::saveProject(bool explicitSave)
{
    QSettings* settings = this->getSettings();
    if (settings->value("closeEvent").toInt() == 1) {
        // if there are no samples, its unlikely that there has been any work
        // done by the user
        if (getSamples().size() == 0)
            return;

        if (_currentProjectName.isEmpty())
            _currentProjectName = _getProjectFilenameFromProjectDockWidget();

        // If the currently loaded project is an mzroll Project, then we should
        // not consider it for our session closure
        if (fileLoader->isMzRollProject(_currentProjectName))
            _currentProjectName = "";

        // if no projects were saved or opened
        if (_latestUserProjectName.isEmpty()) {
            QMessageBox confirmation;
            confirmation.setWindowTitle("Save as project");
            confirmation.setText("Would you like to save your data for this "
                                 "session as a project, before you quit?");
            QPushButton* cancelButton = confirmation.addButton(tr("Cancel"),
                                                         QMessageBox::ActionRole);
            QPushButton* noButton = confirmation.addButton(tr("No"),
                                                     QMessageBox::RejectRole);
            QPushButton* yesButton = confirmation.addButton(tr("Yes"),
                                                      QMessageBox::AcceptRole);
            confirmation.setDefaultButton(yesButton);
            confirmation.setEscapeButton(cancelButton);
            confirmation.exec();

            if (confirmation.clickedButton() == cancelButton) {
                settings->setValue("closeEvent", 0);
                return;
            }

            // remove timestamp autosave file in any case
            fileLoader->closeSQLiteProject();
            QFile::remove(_currentProjectName);
            if (confirmation.clickedButton() == noButton)
                return;

            _currentProjectName = "";
            _setProjectFilenameIfEmpty();

            if (_currentProjectName.isEmpty()) {
                settings->setValue("closeEvent", 0);
                return;
            }

            analytics->hitEvent("Project Save", "emDB");
        } else {
            QMessageBox confirmation;
            confirmation.setText("Would you like to save changes made to this "
                                 "session before you quit?");
            confirmation.setWindowTitle("Save as project");
            QPushButton* cancelButton = confirmation.addButton(tr("Cancel"),
                                                         QMessageBox::ActionRole);
            QPushButton* noButton = confirmation.addButton(tr("No"),
                                                     QMessageBox::RejectRole);
            QPushButton* yesButton = confirmation.addButton(tr("Yes"),
                                                      QMessageBox::AcceptRole);
            confirmation.setDefaultButton(yesButton);
            confirmation.setEscapeButton(cancelButton);
            confirmation.exec();
            if (confirmation.clickedButton() == cancelButton) {
                settings->setValue("closeEvent", 0);
                return;
            } else if (confirmation.clickedButton() == noButton) {
                return;
            }

            QMessageBox msgBox;
            msgBox.setText("Please choose the project file to save your "
                           "progress for this session.");
            QPushButton* saveButton = msgBox.addButton(tr("Save in current"),
                                                       QMessageBox::AcceptRole);
            QPushButton* newButton = msgBox.addButton(tr("Save in new"),
                                                      QMessageBox::AcceptRole);
            msgBox.setDefaultButton(saveButton);
            msgBox.exec();

            // remove current project file only if it was created by autosave
            if (this->timestampFileExists) {
                fileLoader->closeSQLiteProject();
                QFile::remove(_currentProjectName);
            }

            if (msgBox.clickedButton() == newButton) {
                _currentProjectName = "";
                _setProjectFilenameIfEmpty();
                analytics->hitEvent("Project Save", "emDB In New File");
            } else if (msgBox.clickedButton() == saveButton) {
                _currentProjectName = _latestUserProjectName;
                analytics->hitEvent("Project Save", "emDB In Current File");
            } else {
                return;
            }

            if (_currentProjectName.isEmpty()) {
                settings->setValue("closeEvent", 0);
                return;
            }
        }

        // creating a persistent message box, which will be cleared when
        // the application exits anyway
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setText("Please wait. Your project is being saved…");
        msgBox->setStandardButtons(QMessageBox::NoButton);
        msgBox->open();

        this->autosave->saveProjectWorker();
    } else if (explicitSave) {
        _currentProjectName = _getProjectFilenameFromProjectDockWidget();
        if (_latestUserProjectName.isEmpty()) {
            auto reply = QMessageBox::question(this,
                                               "No project open",
                                               "You do not have a project for "
                                               "this session. Would you like "
                                               "to create one?",
                                               QMessageBox::No|QMessageBox::Yes,
                                               QMessageBox::Yes);
            if (reply == QMessageBox::Yes)
                _setProjectFilenameIfEmpty();

            // still empty?!
            if (_currentProjectName.isEmpty())
                return;

            _latestUserProjectName = _currentProjectName;
            QFileInfo fileInfo(_latestUserProjectName);
            setWindowTitle(programName
                           + " "
                           + STR(EL_MAVEN_VERSION)
                           + " "
                           + fileInfo.fileName());
        } else {
            _currentProjectName = _latestUserProjectName;
        }
        this->autosave->saveProjectWorker();
    } else if (this->timestampFileExists) {
        this->autosave->saveProjectWorker();
    } else if (this->peaksMarked > 5 || this->allPeaksMarked) {
        this->autosave->saveProjectWorker();
    }
}

void MainWindow::saveProjectForFilename(QList<PeakGroup*> groupsToBeSaved)
{
    if (fileLoader->isEmdbProject(_currentProjectName)) {
        if (!groupsToBeSaved.empty()) {
            for (auto queuedGroup : groupsToBeSaved) {
                projectDockWidget->savePeakGroupInSQLite(queuedGroup,
                                                         _currentProjectName);
            }
        } else {
            projectDockWidget->saveSQLiteProject(_currentProjectName);
        }
    }
}

void MainWindow::showAlignmentErrorDialog(QString errorMessage)
{
    QMessageBox alignmentError;
    alignmentError.setText(errorMessage);
    alignmentError.open();
}

void MainWindow::openAWSDialog()
{

	awsBucketCredentialsDialog = new AwsBucketCredentialsDialog(this);
	awsBucketCredentialsDialog->show();
	awsBucketCredentialsDialog->setMainWindow(this);
	awsBucketCredentialsDialog->setSettings(settings);

}

QDockWidget* MainWindow::createDockWidget(QString title, QWidget* w) {
	QDockWidget* dock = new QDockWidget(title, this, Qt::Widget);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setFloating(false);
	dock->setVisible(false);
	dock->setObjectName(title);
	dock->setWidget(w);
	return dock;

}

void MainWindow::setIsotopicPlotStyling() {
	//prepare x axis
	customPlot->xAxis->setTickLabels( false );
	customPlot->xAxis->setTicks( false );
	customPlot->xAxis->setBasePen(QPen(Qt::white));
	customPlot->xAxis->grid()->setVisible(false);
	customPlot->xAxis->setRange(0, 1);	
	// prepare y axis:
	customPlot->yAxis->grid()->setVisible(false);
	customPlot->yAxis->setTickLabels( false );
	customPlot->yAxis->setTicks( false );
	customPlot->yAxis->setBasePen(QPen(Qt::white));	
}

void MainWindow::mzrollLoadDB(QString dbname) {
	ligandWidget->setDatabaseNames();
    ligandWidget->setDatabase(dbname);
}

void MainWindow::reportBugs() {
	QString crashReporterPath = QCoreApplication::applicationDirPath() + QDir::separator() + "CrashReporter";
	QProcess *myProcess = new QProcess();
	QStringList arguments;
	arguments << QDir::cleanPath(QCoreApplication::applicationFilePath());
	arguments << settings->value("bucket_name").toString();
	arguments << settings->value("access_key").toString();
	arguments << settings->value("secret_key").toString();
//	arguments <<  myAppender.getMessageQList();
	arguments << "0";
	myProcess->start(crashReporterPath, arguments);

}

void MainWindow::setUrl(QString url, QString link) {

	if (url.isEmpty())
		return;
	if (link.isEmpty())
		link = "Link";
	setStatusText(tr("<a href=\"%1\">%2</a>").arg(url, link));
}

void MainWindow::autoSaveSignal(QList<PeakGroup*> groups) {
    if (groups.empty()) {
        Q_EMIT(saveSignal());
    } else {
        Q_EMIT(saveSignal(groups));
    }
}

void MainWindow::setUrl(Compound* c) {
	if (c == NULL)
		return;
	QString biocycURL = "http://biocyc.org/ECOLI/NEW-IMAGE?type=NIL&object";
	QString keggURL = "http://www.genome.jp/dbget-bin/www_bget?";
	QString pubChemURL =
			"http://www.ncbi.nlm.nih.gov/sites/entrez?db=pccompound&term=";

	QString url;
        if (c->db() == "MetaCyc") {
                url = biocycURL + tr("=%1").arg(c->id().c_str());
        } else if (c->db() == "KEGG") {
                url = keggURL + tr("%1").arg(c->id().c_str());
		//} else if ( c->id.c_str() != "") {
		//  url = keggURL+tr("%1").arg(c->id.c_str());
	} else {
                url = pubChemURL + tr("%1").arg(c->name().c_str());
	}
        QString link(c->name().c_str());
	setUrl(url, link);
}

void MainWindow::setUrl(Reaction* r) {
	if (r == NULL)
		return;
	//QString url = wikiUrl+tr("n=Reaction.%1").arg(r->id.c_str());
	QString biocycURL = "http://biocyc.org/ECOLI/NEW-IMAGE?type=NIL&object";
	QString keggURL = "http://www.genome.jp/dbget-bin/www_bget?";
	QString url;
	if (r->db == "MetaCyc") {
		url = biocycURL + tr("=%1").arg(r->id.c_str());
	} else if (r->db == "KEGG") {
		url = keggURL + tr("%1").arg(r->id.c_str());
	} else {
		url = keggURL + tr("%1").arg(r->id.c_str());
	}
	QString link(r->name.c_str());
	setUrl(url, link);
}

TableDockWidget* MainWindow::addPeaksTable(const QString& tableTitle) {
    TableDockWidget* panel = new PeakTableDockWidget(this, tableTitle);
    analytics->hitEvent("New Table", "Peak Table");

    addDockWidget(Qt::BottomDockWidgetArea, panel, Qt::Horizontal);

    QToolButton* btnTable = addDockWidgetButton(sideBar,
                                                panel,
                                                QIcon(rsrcPath + "/Peak Table.png"),
                                                "");

    groupTables.push_back(panel);
	groupTablesButtons[panel]=btnTable;
	sideBar->addWidget(btnTable);

	return panel;
}

void MainWindow::removePeaksTable(TableDockWidget* panel) {
	//Merged with Maven776 - Kiran
    if (groupTablesButtons.contains(panel)) {
		QAction* tableAction = sideBar->addWidget(groupTablesButtons[panel]) ;
		sideBar->removeAction(tableAction);
	}
	if (groupTables.contains(panel))
		groupTables.removeAll(panel);
}

void MainWindow::removeAllPeakTables()
{
    for (auto table : getPeakTableList()) {
        auto peakTable = static_cast<PeakTableDockWidget*>(table.data());
        peakTable->destroy();
    }
}

void MainWindow::setUserMassCutoff(double x)
{
    double cutoff = x;
    string type = massCutoffComboBox->currentText().toStdString();
    _massCutoffWindow->setMassCutoffAndType(cutoff, type);
    massCalcWidget->setMassCutoff(_massCutoffWindow);
    eicWidget->setMassCutoff(_massCutoffWindow);
    fileLoader->insertSettingForSave("mainWindowMassResolution",
                                     variant(cutoff));
}

void MainWindow::setIonizationModeLabel() {

	QString ionMode = settingsForm->ionizationMode->currentText();

	MavenParameters::Polarity polarity;
	if(ionMode.contains("Positive")) polarity=MavenParameters::Positive;
	else if (ionMode.contains("Negative")) polarity=MavenParameters::Negative;
	else if(ionMode.contains("Neutral")) polarity=MavenParameters::Neutral;
	else polarity=MavenParameters::AutoDetect;

	mavenParameters->setIonizationMode(polarity);

	int mode=getIonizationMode();
	if(polarity==MavenParameters::AutoDetect ){
		QString polarityLabel=QString::number(mode);
		if(mode==1) polarityLabel="+1";
		ionMode=ionMode+"("+polarityLabel+")";
	}
	
	ionizationModeLabel->setText(ionMode);

    isotopeWidget->setCharge(mode);
    ligandWidget->getAdductWidget()->selectAdductsForCurrentPolarity();
	setTotalCharge();
}


void MainWindow::setInjectionOrderFromTimeStamp() {

    vector<mzSample*> samples = getSamples();
    std::sort(samples.begin(), samples.end(), mzSample::compInjectionTime);

    for (unsigned int i = 1; i <= samples.size(); i++)
    {
        mzSample *sample = samples[i-1];
		if (sample->injectionTime > 0) sample->setInjectionOrder(i);
    }
}

void MainWindow::setFilterLine() {

	QStringList filterlines;

    for(int i=0; i < samples.size(); i++ ) {
        mzSample* sample = samples[i];
        for( int j=0; j < sample->scans.size(); j++ ) {
            Scan* scan = sample->getScan(j);
            if (!scan) continue;

            QString filterLine(scan->filterLine.c_str());

            if (filterLine.isEmpty())   continue;

            if (filterlines.contains(filterLine)){
            	continue;
            }

            filterlines << filterLine;
        }
    }

	settingsForm->filterlineComboBox->insertItems(1, filterlines);

}

int MainWindow::getIonizationMode()
{
    return mavenParameters->ionizationMode;
}

void MainWindow::setTotalCharge()
{
    int charge = ionChargeBox->value();
    mavenParameters->charge = charge;
    fileLoader->insertSettingForSave("mainWindowCharge", variant(charge));
    totalCharge = mavenParameters->ionizationMode * charge;
    ligandWidget->updateTable();
}

vector<mzSample*> MainWindow::getVisibleSamples() {

	vector<mzSample*> vsamples;
	for (int i = 0; i < samples.size(); i++) {
		if (samples[i] && samples[i]->isSelected) {
			vsamples.push_back(samples[i]);
		}
    }
    sort(begin(vsamples), end(vsamples), mzSample::compSampleOrder);
	return vsamples;
}

PeakGroup* MainWindow::bookmarkPeakGroup()
{
    if(eicWidget != nullptr && eicWidget->getParameters() != nullptr){
        if (eicWidget->getParameters()->displayedGroup() != nullptr){
            return bookmarkPeakGroup(eicWidget->getParameters()->displayedGroup());
        }
        else
        {
            PeakGroup* peakgroup;
            return peakgroup;
        }
    }
}

PeakGroup* MainWindow::bookmarkPeakGroup(PeakGroup* group)
{
    if ( bookmarkedPeaks == NULL ) return NULL;
	//TODO: User feedback when group is rejected
	if (group->peakCount() == 0) return NULL;

    bookmarkedPeaks->setVisible(true);

    PeakGroup* bookmarkedGroup = NULL;
	if (bookmarkedPeaks->allgroups.size() == 0)
		analytics->hitEvent("New Table", "Bookmark Table");

    if (bookmarkedPeaks->hasPeakGroup(group) == false) {

        float rtDiff = group->expectedRtDiff();
		double A = (double) mavenParameters->qualityWeight/10;
        double B = (double) mavenParameters->intensityWeight/10;
        double C = (double) mavenParameters->deltaRTWeight/10;

        if (mavenParameters->deltaRtCheckFlag && group->getCompound() != NULL && 
            group->getCompound()->expectedRt() > 0) {
            group->groupRank = pow(rtDiff, 2*C)
                               * pow((1.1 - group->maxQuality), A)
                               * (1 /( pow(log(group->maxIntensity + 1), B)));
        } else {
            group->groupRank = pow((1.1 - group->maxQuality), A)
                               * (1 /(pow(log(group->maxIntensity + 1), B)));
        }

		bookmarkedGroup = bookmarkedPeaks->addPeakGroup(group);
        bookmarkedPeaks->showAllGroups();
		bookmarkedPeaks->updateTable();
        bookmarkedPeaks->selectPeakGroup(bookmarkedGroup);
    }
    return bookmarkedGroup;
}

void MainWindow::setFormulaFocus(QString formula) {
	int charge = 0;
	mavenParameters->formulaFlag = true;
	charge = mavenParameters->getCharge();
	mavenParameters->formulaFlag = false;

	if (eicWidget->isVisible()) {
		string compoundName = "Compound-" + formula.toStdString();
		string compoundId = compoundName;
		Compound* c = new Compound(compoundId, compoundName, formula.toStdString(), charge);
		setCompoundFocus(c);
	}		
}

void MainWindow::setPathwayFocus(Pathway* p) {
	if (p && pathwayWidget) {
		pathwayWidget->setVisible(true);
		pathwayWidget->setPathway(p->id.c_str());
	}
}

void MainWindow::setCompoundFocus(Compound*c) {
	if (c == NULL)
		return;
		
	if (!(isotopeWidget->workerThread->stopped() && isotopeWidget->workerThreadBarplot->stopped())) {
		threadCompound = c;
		return;
	}

		
 	if (!(isotopeWidget->workerThread->stopped() && isotopeWidget->workerThreadBarplot->stopped())) {
 		threadCompound = c;
 		return;
 	}


	int charge = 0;
	// if (samples.size() > 0 && samples[0]->getPolarity() > 0)
	// 	charge = 1;
	// if (getIonizationMode())
	// 	charge = getIonizationMode(); //user specified ionization mode
	charge = mavenParameters->getCharge(c);
        qDebug() << "setCompoundFocus:" << c->name().c_str() << " " << charge << " "
                        << c->expectedRt();

        float mz = c->mz();
        if (!c->formula().empty() || c->neutralMass() != 0.0f)
		mz = c->adjustedMass(charge);
    searchText->setText(QString::number(mz, 'f', 8));

	//if (pathwayWidget != NULL && pathwayWidget->isVisible() ) {
	//  pathwayWidget->clear();
	//    pathwayWidget->setCompound(c);
	//}
	
	if (massCalcWidget && massCalcWidget->isVisible()) {
		massCalcWidget->setMass(mz);
	}

	if (eicWidget->isVisible() && samples.size() > 0) {
		eicWidget->setCompound(c);
		PeakGroup *selectedGroup = eicWidget->getSelectedGroup();
		if (isotopeWidget && isotopeWidget->isVisible()) {
			isotopeWidget->setCompound(c);
			isotopeWidget->setPeakGroupAndMore(selectedGroup);
        } else if (isotopeWidget
                   && isotopePlot
                   && isotopePlot->isVisible()
                   && selectedGroup
                   && selectedGroup->getCompound() != NULL) {
            isotopeWidget->updateIsotopicBarplot(selectedGroup);
        }

		if (fragSpectraWidget->isVisible())
			fragSpectraWidget->overlayPeakGroup(selectedGroup);
    }

    if (fragPanel->isVisible())
        showFragmentationScans(mz);

    QString compoundName(c->name().c_str());
    setPeptideSequence(compoundName);

	/*
	 if( peaksPanel->isVisible() && c->hasGroup() ) {
	 peaksPanel->setInfo(c->getPeakGroup());
	 }
	 */

	if (c)
		setUrl(c);
}

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging point
bool MainWindow::setPeptideSequence(QString peptideSeq) {
    //return false;
    peptideSeq = peptideSeq.simplified();
    QRegExp peptideRegExp("\\/\\d$",Qt::CaseSensitive,QRegExp::RegExp);

    if (peptideSeq.contains(peptideRegExp)) {
        Peptide pept(peptideSeq.toStdString(),0,"");
        if (pept.isGood()) {
            float peptideMz = pept.monoisotopicMZ();
            qDebug() << "setPeptideSequence: " << peptideSeq << " " << peptideMz;

            eicWidget->showMS2Events(true);
            setMzValue(peptideMz);
            peptideFragmentation->show();

            if(isotopeWidget->isVisible()) {
                MassCalculator mwcalc;
                string formula=mwcalc.peptideFormula(peptideSeq.toStdString());
                isotopeWidget->setFormula(QString(formula.c_str()));
                isotopeWidget->setCharge(pept.charge);
            }

            if ( peptideFragmentation->isVisible()) {
                peptideFragmentation->setPeptideSequence(peptideSeq);
                peptideFragmentation->setCharge(pept.charge);
            }
            return true;
         }
    }

    return false;
}

void MainWindow::hideDockWidgets() {
	// setWindowState(windowState() ^ Qt::WindowFullScreen);
	QList<QDockWidget *> dockWidgets = this->findChildren<QDockWidget *>();
	for (int i = 0; i < dockWidgets.size(); i++) {
		dockWidgets[i]->hide();
	}
	QWidget* menu = QMainWindow::menuWidget();
}

void MainWindow::showDockWidgets() {
	//setWindowState(windowState() ^ Qt::WindowFullScreen);
	QList<QDockWidget *> dockWidgets = this->findChildren<QDockWidget *>();
	for (int i = 0; i < dockWidgets.size(); i++) {
		if (!dockWidgets[i]->isVisible())
			dockWidgets[i]->show();
	}

	QWidget* menu = QMainWindow::menuWidget();
	if (menu)
		menu->show();
}

void MainWindow::doSearch(QString needle) {
	QRegExp words("[a-z][A-Z]", Qt::CaseInsensitive, QRegExp::RegExp);
	QRegExp formula("C[1-9].*(H[1-9]+|O[1-9]+|N[1-9]+)", Qt::CaseInsensitive,
			QRegExp::RegExp);

    if (setPeptideSequence(needle) ) {
        return;
    }

	if (needle.contains(words) || needle.isEmpty()) {
		//ligandWidget->setFilterString(needle);  //TODO: Sahil-Kiran, Removed while merging mainwindow
		pathwayPanel->filterTree(needle);
	}

	if (needle.contains(formula)) {
		setFormulaFocus(needle);
	}
}

void MainWindow::setMzValue() {
	bool isDouble = false;
	vector<string> values;
	mzUtils::split(searchText->text().toStdString(), '-', values);
	QString value = QString::fromStdString(values[0]);
	float mz1 = value.toDouble(&isDouble);
	bool isDouble2 = false;
	float mz2 = 0.0;
	if (values.size() > 1) {
		QString value2 = QString::fromStdString(values[1]);
		mz2 = value2.toDouble(&isDouble2);
	}

	if (isDouble) {
		if (eicWidget->isVisible())
			eicWidget->setMzSlice(mz1, mz2);
		if (massCalcWidget->isVisible())
			massCalcWidget->setMass(mz1);
		if (fragPanel->isVisible())
			showFragmentationScans(mz1);
	}
	suggestPopup->addToHistory(QString::number(mz1, 'f', 5));
	connect(searchText, SIGNAL(returnPressed()), getEicWidget(), SLOT(resetZoom()));	
}

void MainWindow::setMzValue(float mz1, float mz2) {
	searchText->setText(QString::number(mz1, 'f', 8));
	if (eicWidget->isVisible())
		eicWidget->setMzSlice(mz1, mz2);
	if (massCalcWidget->isVisible())
		massCalcWidget->setMass(mz1);
	if (fragPanel->isVisible())
		showFragmentationScans(mz1);
}

void MainWindow::print() {
        analytics->hitEvent("Exports", "PDF", "From EIC");
	QPrinter printer;
	QPrintDialog dialog(&printer);

	if (dialog.exec()) {
		printer.setOrientation(QPrinter::Landscape);
		printer.setCreator("MAVEN Metabolics Analyzer");
		QPainter painter;
		if (!painter.begin(&printer)) { // failed to open file
			qWarning("failed to open file, is it writable?");
			return;
		}
		getEicWidget()->render(&painter);
		painter.end();
	}
}


void MainWindow::analyticsBoxPlot(){
    analytics->hitEvent("Box Plot", "Clicked");
}

void MainWindow::analyticsAverageSpectra(){
    analytics->hitEvent("Average Spectra", "Clicked");
}

void MainWindow::open()
{
    // TODO: temporarily added for informing user, remove after a few releases
    if (!_versionRecordExists()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("El-MAVEN");
        msgBox.setText("El-MAVEN is now capable of reading files containing "
                       "zlib compressed data. Please feel free to load such "
                       "files, if you have any.");
        msgBox.setIcon(QMessageBox::Information);
        QPushButton* b = msgBox.addButton("Continue", QMessageBox::AcceptRole);
        msgBox.exec();
    }

    QString dir = ".";

    if (settings->contains("lastDir")) {
        QString ldir = settings->value("lastDir").value<QString>();
        QDir test(ldir);
        if (test.exists())
            dir = ldir;
    }

    QStringList filelist = QFileDialog::getOpenFileNames(
        this,
        "Select projects, peaks, samples to open:",
        dir,
        tr("All Known Formats(*.mzroll *.emDB *.mzrollDB *.mzPeaks *.mzXML "
           "*.mzxml *.mzdata *.mzData *.mzData.xml *.cdf *.nc *.mzML);;")
            + tr("mzXML Format(*.mzXML *.mzxml);;")
            + tr("mzData Format(*.mzdata *.mzData *.mzData.xml);;")
            + tr("mzML Format(*.mzml *.mzML);;")
            + tr("NetCDF Format(*.cdf *.nc);;")
            + tr("Thermo (*.raw);;")  // TODO: Sahil-Kiran, Added while merging
                                      // mainwindow
            + tr("Maven Project File (*.mzroll *.emDB);;")
            + tr("Maven Peaks File (*.mzPeaks);;")
            + tr("Peptide XML(*.pep.xml *.pepXML);;")
            + tr("Peptide idpDB(*.idpDB);;") + tr("All Files(*.*)"));

    if (filelist.size() == 0)
        return;

    analytics->hitEvent("Project Dock Widget", "Open");

    // Saving the file location into the QSettings class so that it can be
    // used the next time the user opens
    QString absoluteFilePath(filelist[0]);
    QFileInfo fileInfo(absoluteFilePath);
    QDir tmp = fileInfo.absoluteDir();
    if (tmp.exists())
        settings->setValue("lastDir", tmp.absolutePath());

    // Changing the title of the main window after selecting the samples
    setWindowTitle(programName
                   + " "
                   + STR(EL_MAVEN_VERSION)
                   + " "
                   + fileInfo.fileName());

    QString emdbProjectBeingLoaded = "";
    Q_FOREACH (QString filename, filelist) {
        if (fileLoader->isEmdbProject(filename)) {
            emdbProjectBeingLoaded = filename;
            analytics->hitEvent("Project Load", "emDB");
        } else if (fileLoader->isMzrollDbProject(filename)) {
            emdbProjectBeingLoaded = fileLoader->swapFilenameExtension(filename,
                                                                       "emDB");
            analytics->hitEvent("Project Load", "mzrollDB");
            quantType->setCurrentText("AreaTopNotCorrected");
        } else if (fileLoader->isMzRollProject(filename)) {
            analytics->hitEvent("Project Load", "mzroll");
        }

        fileLoader->addFileToQueue(filename);
    }

    if (!emdbProjectBeingLoaded.isEmpty()) {
        projectDockWidget->saveAndCloseCurrentSQLiteProject();
        _latestUserProjectName = emdbProjectBeingLoaded;

        // reset filename in the title to overwrite any saves while closing last
        // SQLite project
        QFileInfo fileInfo(_latestUserProjectName);
        setWindowTitle(programName
                       + " "
                       + STR(EL_MAVEN_VERSION)
                       + " "
                       + fileInfo.fileName());
    }

    bool cancelUploading = false;
    cancelUploading = updateSamplePathinMzroll(filelist);
    if (!cancelUploading) {
        fileLoader->start();
    } else {
        fileLoader->removeAllFilefromQueue();
    }
}

bool MainWindow::updateSamplePathinMzroll(QStringList filelist) {

    QStringList projects;
	bool cancelUploading = false;

    Q_FOREACH(QString filename, filelist ) {
        QFileInfo fileInfo(filename);
        if (!fileInfo.exists()) continue;
        if (fileLoader->isMzRollProject(filename)) {
            projects << filename;
        }
    }

    Q_FOREACH(QString filename, projects ) {

		QFileInfo fileinfo(filename);
		QFile data(filename);
		QString projectPath = fileinfo.path();
		pathlist << projectPath << ".";

		if ( !data.open(QFile::ReadOnly) ) {
			QErrorMessage errDialog(this);
			errDialog.showMessage("File open: " + filename + " failed");
			return true;
		}


		QXmlStreamReader xml(&data);

		bool gotDir = false;
		while(!gotDir){
			if (xml.isStartElement()) {
				if (xml.name() == "sample") {

					QString fname = xml.attributes().value("filename").toString();
					QFileInfo sampleFile(fname);

					if (!sampleFile.exists()) {
						Q_FOREACH(QString path, pathlist) {
							fname= path + QDir::separator() + sampleFile.fileName();
							sampleFile.setFile(fname);
							if (sampleFile.exists()) break;
						}
					}

					QString path;
					while (!sampleFile.exists()) {
						QString message = "Select the directory having samples corresponding to "
											+ filename;

						QMessageBox::StandardButton reply;
						reply = QMessageBox::question(this, "Samples not found", message,
											QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Ok);

						if (reply == QMessageBox::Cancel) {
							return true;
						} 

						path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                             "/home",
                                             QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);
						fname= path + QDir::separator() + sampleFile.fileName();
						sampleFile.setFile(fname);
					}
					pathlist << path;
					gotDir = true;
				}
			}
			xml.readNext();
		}
		data.close();
	}
	return false;
}

void MainWindow::loadModel() {
	QStringList filelist = QFileDialog::getOpenFileNames(this,
			"Select Model To Load", ".", "All Files(*.model)");
	if (filelist.size() > 0)
		clsf->loadModel(filelist[0].toStdString());
}

void MainWindow::loadCompoundsFile(QString filename, bool threaded)
{
    // Saving the file location into QSettings class so that it can be
    // used the next time user wants to load a compounds DB
    QFileInfo fileInfo(filename);
    QDir tmp = fileInfo.absoluteDir();
    if (tmp.exists())
        settings->setValue("lastCompoundsDir", tmp.absolutePath());

    if (threaded) {
        fileLoader->addFileToQueue(filename);
        fileLoader->start();
        return;
    }

    int compoundCount = fileLoader->loadCompoundsFromFile(filename);
    _postCompoundsDBLoadActions(filename, compoundCount);
}

void MainWindow::_postCompoundsDBLoadActions(QString filename,
                                             int compoundCount)
{
    string dbName = mzUtils::cleanFilename(filename.toStdString());

    bool reloading = false;
    deque<Compound*> compoundsDB = DB.getCompoundsDB();
    for (int i = 0; i < compoundsDB.size(); i++) {
        Compound* currentCompound = compoundsDB[i];
        if (currentCompound->db() == dbName) {
            reloading = true;
            break;
        }
    }

    // check status in case the same file had been uploaded earlier
    // without modifications
    if ((compoundCount > 0 || reloading) && ligandWidget) {
        massCalcWidget->database->addItem(QString::fromStdString(dbName));
        ligandWidget->setDatabaseNames();
        if (ligandWidget->isVisible())
            ligandWidget->setDatabase(QString(dbName.c_str()));

        vector<Compound*> loadedCompounds = DB.getCompoundsSubset(dbName);
        if (!loadedCompounds.empty()) {
            QString eventLabel = "MS1";
            if (loadedCompounds[0]->precursorMz() > 0
                && (loadedCompounds[0]->productMz() > 0
                    || loadedCompounds[0]->fragmentMzValues().size() > 0)) {
                eventLabel = DB.isSpectralLibrary(dbName) ? "MS2 (PRM)"
                                                      : "MS2 (MRM)";
            }
            analytics->hitEvent("Load Compound DB",
                                "Successful Load",
                                eventLabel);
            if (eventLabel == "MS2 (PRM)") {
                analytics->hitEvent("PRM", "LoadedSpectralLibrary");
            }
        }

        setLastLoadedDatabase(filename);

        bool isMSPFile = filename.endsWith("msp", Qt::CaseInsensitive);
        bool isSPTXTFile = filename.endsWith("sptxt", Qt::CaseInsensitive);
        bool isMascotFile = filename.endsWith("mgf", Qt::CaseInsensitive);
        bool isSpectralFile = isMSPFile || isSPTXTFile || isMascotFile;
        if (isSpectralFile)
            _warnIfNISTPolarityMismatch();

        setStatusText(tr("Loaded %1 compounds successfully")
                        .arg(QString::number(compoundCount)));
        _notifyIfBadCompoundsDB(filename, false);
        _libraryManager->addDatabase(filename);
    } else {
        setStatusText(tr("Failed to load compound database")
                        .arg(filename));
        _notifyIfBadCompoundsDB(filename, true);
    }
}

void MainWindow::_notifyIfBadCompoundsDB(QString filename,
                                         bool failedToLoadCompletely)
{
    if (failedToLoadCompletely) {
        analytics->hitEvent("Load Compound DB",
                            "Column Error",
                            "Complete Failure");

        string dbfilename = filename.toStdString();
        string dbname = mzUtils::cleanFilename(dbfilename);

        QMessageBox msgBox;
        msgBox.setText(tr("Failed to load compound database %1")
                         .arg(QString::fromStdString(dbname)));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowFlags(msgBox.windowFlags()
                              & ~Qt::WindowCloseButtonHint);

        string msgString = "Following are the unknown column name(s) found:";
        if (DB.notFoundColumns.size() > 0) {
            for (const auto& column : DB.notFoundColumns) {
                 msgString += "\n - " + column;
            }
            msgBox.setDetailedText(QString::fromStdString(msgString));
        }
        msgBox.open();
    } else {
        if (DB.notFoundColumns.size() > 0) {
            analytics->hitEvent("Load Compound DB",
                                "Column Error",
                                "Partial Failure");

            QMessageBox msgBox;
            msgBox.setText(tr("Found some unknown column name(s)"));
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowFlags(msgBox.windowFlags()
                                  & ~Qt::WindowCloseButtonHint);

            string msgString = "Following are the unknown column name(s) "
                               "found:";
            for (const auto& column : DB.notFoundColumns) {
                 msgString += "\n - " + column;
            }
            msgBox.setDetailedText(QString::fromStdString(msgString));
            msgBox.open();
        }
        if (DB.invalidRows.size() > 0) {
            analytics->hitEvent("Load Compound DB", "Row Error");

            QMessageBox msgBox;
            msgBox.setText(tr("Invalid compounds found"));
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowFlags(Qt::CustomizeWindowHint);

            string msgString = "The following compounds had insufficient "
                               "information for peak detection, and were not "
                               "loaded:";
            for (auto compoundID : DB.invalidRows) {
                msgString += "\n - " + compoundID;
            }
            msgBox.setDetailedText(QString::fromStdString(msgString));
            msgBox.open();
        }
    }
}

void MainWindow::_warnIfNISTPolarityMismatch()
{
    if (samples.size() == 0)
        return;

    // make sure the samples contain PRM data (TODO: check needs revision)
    mzSample* sample = samples[0];
    if (sample->ms1ScanCount() == 0 || sample->ms2ScanCount() == 0)
        return;

    // make sure the database is a spectral library
    string dbName = ligandWidget->getDatabaseName().toStdString();
    if (!DB.isSpectralLibrary(dbName))
        return;

    int samplePolarity = sample->getPolarity();
    Compound::IonizationMode polarity = DB.getCompoundsSubset(dbName)[0]->ionizationMode;
    int dbPolarity = 0;
    if (polarity == Compound::IonizationMode::Positive)
        dbPolarity = 1;
    else if (polarity == Compound::IonizationMode::Negative)
        dbPolarity = -1;
    else
        dbPolarity = 0;

    if (samplePolarity != dbPolarity) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Polarity Mismatch"));
        QString msg = "The polarity of loaded samples and spectral library do "
                      "not match. Please upload a spectral library of correct "
					  "polarity %1";
        QString polarity = (samplePolarity > 0) ? "(positive)" : "(negative)";
        msg = msg.arg(polarity);
        msgBox.setText(msg);
        QPushButton* upload = msgBox.addButton(tr("Upload Spectral Library"),
                                               QMessageBox::ActionRole);
        msgBox.addButton(QMessageBox::Ignore);
        msgBox.setDefaultButton(upload);
        msgBox.exec();
        analytics->hitEvent("PRM", "Polarity Mismatch");

        if (msgBox.clickedButton() == upload) {
            analytics->hitEvent("PRM", "Loaded Spectral Library", "Polarity Mismatch prompt");
            loadCompoundsFile();
        }
        else {
            analytics->hitEvent("PRM", "Closed", "Polarity Mismatch prompt");
        }
    }
}

void MainWindow::setLastLoadedDatabase(QString filename)
{
    QFileInfo fileInfo(filename);
    bool isMSPFile = filename.endsWith("msp", Qt::CaseInsensitive);
    bool isSPTXTFile = filename.endsWith("sptxt", Qt::CaseInsensitive);
    bool isMascotFile = filename.endsWith("mgf", Qt::CaseInsensitive);
    bool nistFile = isMSPFile || isSPTXTFile || isMascotFile;
    bool smallerThan2Mb = fileInfo.size() < 2000000;
    if (!nistFile || smallerThan2Mb)
        settings->setValue("lastDatabaseFile", filename);
}

bool MainWindow::_versionRecordExists()
{
    QString currentVersion = STR(EL_MAVEN_VERSION);
    QString versionLogPath = QString(QStandardPaths::writableLocation(
                                         QStandardPaths::GenericConfigLocation)
                                     + QDir::separator()
                                     + "ElMaven"
                                     + QDir::separator()
                                     + "version.log");

    // lambda that can be used to log an entry for the current app version
    auto logCurrentVersion = [&]() {
        Logger versionLogger(versionLogPath.toStdString(), false, false);
        versionLogger.info() << currentVersion.toStdString() << endl;
    };

    // lambda that checks whether two regex matches represent the same hits
    auto sameVersionMatch = [](QRegularExpressionMatch& first,
                               QRegularExpressionMatch& second) {
        // for two successful matches of version strings, one of which might be
        // "v0.8.0-beta.2-142-g1d3468ae", checks whether the groups (0.8.0),
        // (beta) and (2) match or not; build numbers and tags are disregarded
        return (first.captured(2) == second.captured(2)
                && first.captured(4) == second.captured(4)
                && first.captured(6) == second.captured(6));
    };

    QFile data(versionLogPath);
    if (!data.open(QFile::ReadOnly) ) {
        qDebug() << "Cannot open file: " << versionLogPath;
        logCurrentVersion();
        return false;
    }

    QRegularExpression
        ver("v((\\d+\\.\\d+\\.\\d+)(-(beta|alpha))?(\\.(\\d)*)?(-(\\d+))?)");
    auto currentVersionMatch = ver.match(currentVersion);

    QTextStream stream(&data);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        auto match = ver.match(line);
        if (match.hasMatch()
            && sameVersionMatch(currentVersionMatch, match)) {
            return true;
        }
    }

    logCurrentVersion();
    return false;
}

void MainWindow::checkCorruptedSampleInjectionOrder()
{

    vector<mzSample*> samples = getSamples();

    // check for samples with same injection order
    // store samples with same injection order in a map with injection order as their key
    QMap<int,QList<mzSample*>> sameInjectionOrder;
    auto it = samples.begin();
    while(it != samples.end()) {

        if((*it)->getInjectionOrder() != 0 && ((*it)->getInjectionOrder() != -1)){
            if(sameInjectionOrder.find((*it)->getInjectionOrder()) == sameInjectionOrder.end())
                sameInjectionOrder.insert((*it)->getInjectionOrder(),QList<mzSample*>() << (*it));

            else {
                QMap<int, QList<mzSample*>>::iterator  i = sameInjectionOrder.find((*it)->getInjectionOrder());
                i.value().push_back((*it));
            }
        }

        it++;
    }

    //check for sample with corrupted(negative/alphabet/string) injection order
    // any injection Order that is corrupted has been assigned -1 previously
    QList<mzSample*> negativeInjectionOrder;
    it = samples.begin();
    while(it != samples.end()) {

        if((*it)->getInjectionOrder() < 0 )
            negativeInjectionOrder.push_back((*it));

        it++;
    }

    // present a warning to user only if we found some samples with corrupted injection order
    if(sameInjectionOrder.size() > 0 || negativeInjectionOrder.size() > 0)
        warningForInjectionOrders(sameInjectionOrder, negativeInjectionOrder);


}

void MainWindow::warningForInjectionOrders(QMap<int, QList<mzSample*>> sameOrders, QList<mzSample*> negativeOrders)
{
    QString corruptedSamples;
    QString warningMsg;

    {
        // iterate on sample with same orders and form a warning message
        auto it = sameOrders.begin();
        while(it != sameOrders.end()) {

            if(it.value().size() > 1) {

                auto it2 = it.value().begin();
                QString smpl;
                while(it2 != it.value().end()) {
                    smpl += QString((*it2)->getSampleName().c_str());
                    smpl += "   ";
                    it2++;
                }
                if(!smpl.isEmpty()) {
                    smpl = "* " + smpl;
                    smpl += "\n\n";
                }
                corruptedSamples += smpl;
                smpl.clear();
            }

            it++;
        }
    }

    if(!corruptedSamples.isEmpty()) {
        warningMsg = "Samples with same injection order";
        warningMsg += "\n";
        warningMsg += corruptedSamples;
        warningMsg += "\n\n";
    }

    corruptedSamples.clear();

    {
        // iterarte on negative orders and add corrupted samples to warning message
        auto it = negativeOrders.begin();
        while(it != negativeOrders.end() ) {
            QString smpl;
            smpl += "* ";
            smpl += QString((*it)->getSampleName().c_str());
            smpl += "\n";
            corruptedSamples += smpl;
            it++;
        }
    }

    if(!corruptedSamples.isEmpty()) {
        warningMsg += "Samples with corrupted injection order ";
        warningMsg += "\n";
        warningMsg += corruptedSamples;
        warningMsg += "\n\n";
    }
    cerr << "*********WARNING MSG******" << endl << warningMsg.toStdString() << endl;

    if(!warningMsg.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("WARNING");
        msgBox.setText(warningMsg);
        msgBox.exec();
    }
}

//load meta information
bool MainWindow::loadMetaInformation(QString filename) {
	string dbfilename = filename.toStdString();
	string dbname = mzUtils::cleanFilename(dbfilename);
	int sampleCount = 0;

    sampleCount = loadMetaCsvFile(dbfilename);
    
	if (sampleCount > 0) {
        setStatusText(tr("loadMetaInfo: done after loading %1 meta information").arg(QString::number(sampleCount)));
        Q_EMIT(metaCsvFileLoaded());

        // user should be presented a warning if injection order of two samples is same or is negative
        checkCorruptedSampleInjectionOrder();
		return true;
	} else {
        setStatusText(tr("loadMetaInfo: not able to load %1 meta information").arg(filename));
		return false;
	}
}

void MainWindow::loadCompoundsFile()
{
    QString dir = ".";
    if (settings->contains("lastCompoundsDir")) {
        QString ldir = settings->value("lastCompoundsDir").value<QString>();
        QDir test(ldir);
        if (test.exists())
            dir = ldir;
    }

    QString filename = QFileDialog::getOpenFileName(
        this,
        "Select Compounds File To Load",
        dir,
        "All Known Formats(*.csv *.tab *.tab.txt *.msp *.sptxt *.mgf *.pepXML "
        "*.massbank);;Tab Delimited(*.tab);;Tab Delimited Text(*.tab.txt);;CSV "
        "File(*.csv);;NIST "
        "Library(*.msp);;SpectraST(*.sptxt);;Mascot Library File(*.mgf);;"
        "pepXML(*.pepXML);;MassBank(*.massbank");

    if (filename.isEmpty())
        return;

    loadCompoundsFile(filename);
}

// open function for set csv
void MainWindow::loadMetaInformation() {
    auto loadedSamples = getSamples();
    if (loadedSamples.empty())
        return;
    auto lastSample = loadedSamples.back();
    auto sampleDir = QFileInfo(lastSample->fileName.c_str()).dir();

    QStringList filelist =
        QFileDialog::getOpenFileNames(this,
                                      "Select Set Information File To Load",
                                      sampleDir.path(),
                                      "All Known Formats(*.csv *.tab *.tab.txt);;"
                                      "Tab Delimited(*.tab);;"
                                      "Tab Delimited Text(*.tab.txt);;"
                                      "CSV File(*.csv)");

    if ( filelist.size() == 0 || filelist[0].isEmpty() ) return;
    if(!loadMetaInformation(filelist[0])) {
		string dbfilename = filelist[0].toStdString();
		string dbname = mzUtils::cleanFilename(dbfilename);

		QMessageBoxResize msgBox;
		msgBox.setText(tr("Trouble in loading set information %1").arg(QString::fromStdString(dbname)));
		msgBox.setIcon(QMessageBoxResize::Warning);

		int ret = msgBox.exec();
	}
}

int MainWindow::loadMetaCsvFile(string filename){

    ifstream myfile(filename.c_str());
    if (! myfile.is_open()) return 0;

    string line;
    string dbname = mzUtils::cleanFilename(filename);
    int loadCount=0;
    int lineCount=0;
    map<string, int>header;
    vector<string> headers;
    static const string allHeadersarr[] = {"sample", "set", "cohort", "scaling", "injection order"};
    vector<string> allHeaders (allHeadersarr, allHeadersarr + sizeof(allHeadersarr) / sizeof(allHeadersarr[0]) );

    //assume that files are tab delimited, unless matched ".csv", then comma delimited
    string sep="\t";
    if(filename.find(".csv") != -1 || filename.find(".CSV") != -1) sep=",";
    //cerr << filename << " sep=" << sep << endl;
    while ( getline(myfile,line) ) {
        if (!line.empty() && line[0] == '#') continue;
        //trim spaces on the left
        line.erase(line.find_last_not_of(" \n\r\t")+1);
        lineCount++;

        vector<string>fields;
        mzUtils::splitNew(line, sep, fields);

        for(unsigned int i=0; i < fields.size(); i++ ) {
            int n = fields[i].length();
            if (n>2 && fields[i][0] == '"' && fields[i][n-1] == '"') {
                fields[i]= fields[i].substr(1,n-2);
            }
            if (n>2 && fields[i][0] == '\'' && fields[i][n-1] == '\'') {
                fields[i]= fields[i].substr(1,n-2);
            }
        }

        if (lineCount==1) {
            headers = fields;
            for(unsigned int i=0; i < fields.size(); i++ ) {
                fields[i] = makeLowerCase(fields[i]);
                header[fields[i]] = i;
            }
            continue;
        }

        string set;
		QString sampleName;
        int injectionOrder = 0;
        int N=fields.size();

        if ( header.count("sample")&& header["sample"]<N) 	 sampleName = QString::fromUtf8(fields[ header["sample"] ].c_str());
        if (header.count("set") && header["set"] < N) {
            set = fields[header["set"]];
        } else if (header.count("cohort") && header["cohort"] < N) {
            set = fields[header["cohort"]];
        } else {
            set = "";
        }

        float scalingFactor = 1.0f;
        if (header.count("scaling") && header["scaling"] < N) {
            auto scaleString = fields[ header["scaling"] ];
            istringstream iss(scaleString);
            float tempScalingFactor = 1.0f;
            iss >> noskipws >> tempScalingFactor;
            if (iss.eof() && !iss.fail())
                scalingFactor = tempScalingFactor;
        }

        if (header.count("injection order") && header["injection order"]<N){

            string io = fields[header["injection order"]];
            if(!io.empty()){
                try{
                    // injection order with a value between 1-9 is considered valid otherwise it's given a value of -1

                    if(std::all_of(io.begin(), io.end(), ::isdigit)){
                        injectionOrder = std::stoi(io);
                        // 0 is not a valid injection order
                        if(injectionOrder == 0)
                            injectionOrder = -1;
                    }
                    // if any of the character is anything other than a  digit that means injection order is invalid
                    else
                        injectionOrder = -1;
                }
                catch(std::exception&) {
                    // we could not convert injection order string to int since it was something invalid
                    injectionOrder = -1;
                }
            }

        }

        if (sampleName.isEmpty()) continue;
		if (set.empty()) set = "";

        if (fileLoader->isSampleFileType(sampleName)){
			sampleName = sampleName.section('.', 0, -2);
		}
		mzSample* sample=getSampleByName(sampleName);
		if(!sample) continue; 
        sample->_setName = set;
        sample->setInjectionOrder(injectionOrder);
        sample->setNormalizationConstant(scalingFactor);
        loadCount++;
    }
    myfile.close();
    return loadCount;
}

void MainWindow::loadPathwaysFolder(QString& pathwaysFolder) {
	cerr << "LOADING PATHWAYS FROM:" << pathwaysFolder.toStdString() << endl;
	QDir dir(pathwaysFolder);
	if (dir.exists()) {
		dir.setFilter(QDir::Files);
		QFileInfoList list = dir.entryInfoList();
		for (int i = 0; i < list.size(); ++i) {
			QFileInfo fileInfo = list.at(i);
			if (fileInfo.fileName().endsWith("xml", Qt::CaseInsensitive)) {
				//std::cerr << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10).arg(fileInfo.fileName())) << endl;
				pathwayWidget->loadModelFile(fileInfo.absoluteFilePath());
			}
		}
	}
}

BackgroundPeakUpdate* MainWindow::newWorkerThread(QString funcName) {
	BackgroundPeakUpdate* workerThread = new BackgroundPeakUpdate(this);
	workerThread->setMainWindow(this);

	connect(workerThread, SIGNAL(updateProgressBar(QString,int,int)),
			alignmentDialog, SLOT(setProgressBar(QString, int,int)));
	connect(workerThread, SIGNAL(samplesAligned(bool)), alignmentDialog, SLOT(samplesAligned(bool)));
	workerThread->setRunFunction(funcName);
	//threads.push_back(workerThread);
	return workerThread;
}

void MainWindow::exportPDF()
{
        analytics->hitEvent("Exports", "PDF", "From Dropdown");
	const QString fileName = QFileDialog::getSaveFileName(this,
			"Export File Name", QString(), "PDF Documents (*.pdf)");

	if (!fileName.isEmpty()) {
		QPrinter printer;
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOrientation(QPrinter::Landscape);
		printer.setOutputFileName(fileName);

		QPainter painter;
		if (!painter.begin(&printer)) { // failed to open file
			qWarning("failed to open file, is it writable?");
			return;
		}

		getEicWidget()->render(&painter);
		painter.end();
	}
}

void MainWindow::exportSVG()
{
        analytics->hitEvent("Exports", "Clipboard", "From Dropdown");

	QPixmap image(eicWidget->width() * 2, eicWidget->height() * 2);
	image.fill(Qt::white);
	//eicWidget->print(&image);
	QPainter painter;
	painter.begin(&image);
	getEicWidget()->render(&painter);
	painter.end();

	QApplication::clipboard()->setPixmap(image);
	statusBar()->showMessage("EIC Image copied to Clipboard");
}

void MainWindow::setStatusText(QString text) {
	statusText->setText(text);
	//statusBar()->showMessage(text,500);
}

void MainWindow::setProgressBar(QString text, int progress, int totalSteps) {
	setStatusText(text);
	if (progressBar->isVisible() == false && progress != totalSteps) {
		progressBar->show();
	}
	progressBar->setRange(0, totalSteps);
	progressBar->setValue(progress);
	if (progress == totalSteps) {
		progressBar->hide();
	}
}

void MainWindow::_setStatusString(QString text)
{
    setStatusText(text);
    if (progressBar->isVisible())
        progressBar->hide();
}

void MainWindow::readSettings() {
	settings = new QSettings("mzRoll", "Application Settings");

	QPoint pos = settings->value("pos", QPoint(0, 0)).toPoint();
	QSize size = settings->value("size", QSize(1000, 1400)).toSize();

	if (settings->contains("windowState")) {
		restoreState(settings->value("windowState").toByteArray());
	}

	if (settings->contains("geometry")) {
		restoreGeometry(settings->value("geometry").toByteArray());
	}

	if (!settings->contains("scriptsFolder"))
		settings->setValue("scriptsFolder", "scripts");

	if (!settings->contains("workDir"))
		settings->setValue("workDir", QString("."));

	if (!settings->contains("methodsFolder"))
		settings->setValue("methodsFolder", "methods");

	if (!settings->contains("pathwaysFolder"))
		settings->setValue("pathwaysFolder", "pathways");

	if (!settings->contains("ligandDbFilename"))
		settings->setValue("ligandDbFilename", QString("ligand.db"));
			
    if (!settings->contains("peakClassifierFile") || settings->value("peakClassifierFile").toString().length() <=0) {
        #if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
          settings->setValue("peakClassifierFile",  QApplication::applicationDirPath() + "/" + "default.model");
        #endif
        #if defined(Q_OS_MAC)
          QString binPath = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator() + ".." \
                  + QDir::separator() + "default.model";
          settings->setValue("peakClassifierFile", binPath);
        #endif
    }



        
    // if (!settings->contains("checkBox"))
    //     settings->setValue("checkBox", 0);

    // if (!settings->contains("checkBox_2"))
    //     settings->setValue("checkBox_2", 0);

    // if (!settings->contains("checkBox_3"))
    //     settings->setValue("checkBox_3", 0);

    // if (!settings->contains("checkBox_4"))
    //     settings->setValue("checkBox_4", 0);

    //Pull Isotopes in options


        if (!settings->contains("abundanceThreshold"))
                settings->setValue("abundanceThreshold", 1);


    //Main window right hand top
    if (!settings->contains("massCutoffWindowBox"))
		settings->setValue("massCutoffWindowBox", 5);



    if (!settings->contains("mzslice"))
        settings->setValue("mzslice", QRectF(100.0, 100.01, 0, 30));

    //Options tab 

	if (!settings->contains("ionChargeBox"))
        settings->setValue("ionChargeBox", 1);


    if (settings->contains("lastOpenedProject"))
		settings->setValue("lastOpenedProject", "");


    //Added when merged with Maven776 - Kiran
    if (!settings->contains("embeded_http_server_autostart")) {
        settings->setValue("embeded_http_server_autostart", true);
    }

    if (!settings->contains("embeded_http_server_port")) {
         settings->setValue("embeded_http_server_port", 45678);
    }

    if (!settings->contains("embeded_http_server_address")) {
        settings->setValue("embeded_http_server_address", "127.0.0.1");
    }


	settings->setValue("uploadMultiprocessing", 2);

	resize(size);
	move(pos);
}

void MainWindow::writeSettings() {
	settings->setValue("pos", pos());
    settings->setValue("size", size());
	settings->setValue("massCutoffWindowBox", massCutoffWindowBox->value());
	settings->setValue("ionChargeBox", ionChargeBox->value());
	settings->setValue("geometry", saveGeometry());
	settings->setValue("windowState", saveState());

	mzSlice slice = eicWidget->getParameters()->getMzSlice();
	settings->setValue("mzslice",
			QRectF(slice.mzmin, slice.mzmax, slice.rtmin, slice.rtmax));

	if (suggestPopup) {
		QMap<QString, int> history = suggestPopup->getHistory();
		Q_FOREACH(QString key, history.keys()){
		if ( history[key] > 1 ) {
			settings->setValue("searchHistory/"+ key, history[key]);
		}
	}
}

	qDebug() << "Settings saved to " << settings->fileName();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    settings->setValue("closeEvent", 1);
    this->saveProject();

    writeSettings();

    // wait until autosave has finished
    while(autosave->isRunning())
        QApplication::processEvents();

    if (settings->value("closeEvent").toInt() == 0) {
        event->ignore();
        return;
    }

    event->accept();
}

/**
 * MainWindow::createMenus This function creates the menu that is on top of
 * the window. All the functionalities that are here are there in other
 * places in the window
 */
void MainWindow::createMenus() {
	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	QMenu* widgetsMenu = menuBar()->addMenu(tr("&Widgets"));
	QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction* openAct = new QAction(tr("&Load Samples|Projects|Peaks"), this);
	openAct->setShortcut(tr("Ctrl+O"));
	openAct->setToolTip(tr("Open an existing file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
	fileMenu->addAction(openAct);

	QAction* loadModel = new QAction(tr("Load Classification Model"), this);
	connect(loadModel, SIGNAL(triggered()), SLOT(loadModel()));
	fileMenu->addAction(loadModel);

	QAction* loadCompoundsFile = new QAction(tr("Load Compound List"), this);
	connect(loadCompoundsFile, SIGNAL(triggered()), SLOT(loadCompoundsFile()));
	fileMenu->addAction(loadCompoundsFile);

    // add option to save the current project, if any
    QAction* saveProject = new QAction(tr("Save project"),
                                       this);
    saveProject->setShortcut(tr("Ctrl+S"));
    connect(saveProject,
            SIGNAL(triggered()),
            this,
            SLOT(explicitSave()));
    fileMenu->addAction(saveProject);

    // add option to save as a database
    QAction* saveProjectAsSQLite = new QAction(tr("Save project as…"),
                                               this);
    saveProjectAsSQLite->setShortcut(tr("Ctrl+Shift+S"));
    connect(saveProjectAsSQLite,
            SIGNAL(triggered()),
            projectDockWidget,
            SLOT(saveProjectAsSQLite()));
    connect(saveProjectAsSQLite, &QAction::triggered, [this]()
    {
        this->analytics->hitEvent("Project Save", "emDB");
    });
    fileMenu->addAction(saveProjectAsSQLite);

    QAction* saveSettings = new QAction("Save Settings", this);
    connect(saveSettings, &QAction::triggered, this ,&MainWindow::saveSettings);
    fileMenu->addAction(saveSettings);

    QAction* loadSettings = new QAction("Load Settings", this);
    connect(loadSettings, &QAction::triggered, this ,&MainWindow::loadSettings);
    fileMenu->addAction(loadSettings);

	QAction* settingsAct = new QAction(tr("Options"), this);
	settingsAct->setToolTip(tr("Set program options"));
	connect(settingsAct, SIGNAL(triggered()), settingsForm, SLOT(show()));
	fileMenu->addAction(settingsAct);

	QAction* awsDialog = new QAction(tr("Add S3 credentials"), this);
	connect(awsDialog, SIGNAL(triggered()), SLOT(openAWSDialog()));
	fileMenu->addAction(awsDialog);

	QAction* reportBug = new QAction(tr("Report Bugs!"), this);
	connect(reportBug, SIGNAL(triggered()), SLOT(reportBugs()));
	fileMenu->addAction(reportBug);

	QAction* exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setToolTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
	fileMenu->addAction(exitAct);

	QAction* hideWidgets = new QAction(tr("Hide Widgets"), this);
	hideWidgets->setShortcut(tr("F11"));
	connect(hideWidgets, SIGNAL(triggered()), SLOT(hideDockWidgets()));
	widgetsMenu->addAction(hideWidgets);

	QAction* logWidgetAction = new QAction(tr("Log Widget"), this);
	logWidgetAction->setShortcut(tr("Ctrl+L"));
	logWidgetAction->setCheckable(true);
	logWidgetAction->setChecked(false);
	connect(logWidgetAction, SIGNAL(toggled(bool)), logWidget,
			SLOT(setVisible(bool)));
	widgetsMenu->addAction(logWidgetAction);

	QAction* ak = widgetsMenu->addAction("Spectral Hits Widget");
	ak->setCheckable(true);
	ak->setChecked(false);
	connect(ak, SIGNAL(toggled(bool)), spectralHitsDockWidget,
			SLOT(setVisible(bool)));

    QAction* aj = widgetsMenu->addAction("MS2 Events");
    aj->setCheckable(true); 
	aj->setChecked(false);
    connect(aj, SIGNAL(toggled(bool)), fragPanel, SLOT(setVisible(bool)));
    connect(aj, &QAction::toggled, [this](const bool checked)
    {
        if (checked) {
            this->analytics->hitEvent("PRM", "OpenedFragmentationEvents");
        }
    });

    QAction* al = widgetsMenu->addAction("Peptide Fragmentation");
    al->setCheckable(true);  al->setChecked(false);
    connect(al,SIGNAL(toggled(bool)),peptideFragmentation,SLOT(setVisible(bool)));

	QSignalMapper* signalMapper = new QSignalMapper (this) ;
	QAction* doc = helpMenu->addAction("Documentation");
	connect(doc,SIGNAL(triggered(bool)), signalMapper, SLOT(map()));

	QAction* tutorial = helpMenu->addAction("Video Tutorials");
    connect(tutorial,SIGNAL(triggered()), signalMapper, SLOT(map()));

    QAction* mlModelVideo = helpMenu->addAction("How ML model works");
    connect(mlModelVideo, &QAction::triggered, vidPlayer, &VideoPlayer::show);

	QAction* faq = helpMenu->addAction("FAQs");
	connect(faq, SIGNAL(triggered()), signalMapper, SLOT(map()));

	signalMapper->setMapping(doc, 1);
	signalMapper->setMapping(tutorial, 2);
	signalMapper->setMapping(faq, 3);

    connect(signalMapper,
            QOverload<int>::of(&QSignalMapper::mapped),
            [this] (int choice) {
                map<int,QUrl> URL{
                    {1, QUrl("https://github.com/ElucidataInc/ElMaven/wiki")},
                    {2, QUrl("https://www.youtube.com/channel/UCZYVM0I1zqRgkGTdIlQZ9Yw/videos")},
                    {3, QUrl("https://elucidatainc.github.io/ElMaven/faq/")}
                };
                QDesktopServices::openUrl(URL[choice]);
            });

	menuBar()->show();
}

QToolButton* MainWindow::addDockWidgetButton(QToolBar* bar,
		QDockWidget* dockwidget, QIcon icon, QString description) {
	QToolButton *btn = new QToolButton(bar);
	btn->setCheckable(true);
	btn->setIcon(icon);
	btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
	btn->setToolTip(description);
	btn->setObjectName(dockwidget->objectName());
	connect(btn, SIGNAL(clicked(bool)), dockwidget, SLOT(setVisible(bool)));
	connect(btn, SIGNAL(clicked(bool)), dockwidget, SLOT(raise()));
	connect(btn, SIGNAL(clicked(bool)), this, SLOT(sendAnalytics(bool)));
	btn->setChecked(dockwidget->isVisible());
	connect(dockwidget, SIGNAL(visibilityChanged(bool)), btn,
			SLOT(setChecked(bool)));
	dockwidget->setWindowIcon(icon);
	return btn;
}

void MainWindow::saveSettings()
{
    QString fileName = QFileDialog::getSaveFileName(Q_NULLPTR,"Save Settings",QString());

    if(!mavenParameters->saveSettings(fileName.toStdString().c_str())) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setText("Failed to save the file");
        msgBox.exec();
    }

}

void MainWindow::_showEMDBProgressBar(QString projectFilename)
{
    if (_loadProgressDialog != nullptr) {
        delete _loadProgressDialog;
    }

    _loadProgressDialog = new QProgressDialog(this);
    QLabel* label = new QLabel(QString("<b>Loading Project \"%1\"</b>")
                               .arg(projectFilename));
    label->setStyleSheet("QLabel { margin-top: 6px; }");
    label->setAlignment(Qt::AlignCenter);
    _loadProgressDialog->setLabel(label);
    _loadProgressDialog->setCancelButton(nullptr);

    // ensure label gets fully accomodated, adjustSize itself did not help
    _loadProgressDialog->setMinimumWidth(label->width() + 48);

    _loadProgressDialog->setModal(true);
    _loadProgressDialog->open();
}

void MainWindow::_updateEMDBProgressBar(int progress, int finish)
{
    QApplication::processEvents();
    if (_loadProgressDialog != nullptr and _loadProgressDialog->isVisible()) {
        _loadProgressDialog->setMinimum(0);
        _loadProgressDialog->setMaximum(finish);
        _loadProgressDialog->setValue(progress);
    }
    if (progress == finish) {
        for (const auto sample : getSamples()) {
            for (const auto scan : sample->scans) {
                if (scan->originalRt != scan->rt) {
                    samplesAlignedFlag = true;
                    break;
                }
            }
        }
        _setStatusString("Project loaded");
    }
}

void MainWindow::loadPollySettings(QString fileName)
{
    bool fileLoaded = false;
    QFile file(fileName);

    if(file.open(QIODevice::ReadOnly)) {

        QByteArray bArr = file.readAll();
        file.close();

        if(mavenParameters->loadSettings(bArr.data()))
            fileLoaded = true;

    }

    if(fileLoaded)
        emit loadedSettings();

    else {
        // display an error message
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setText("Loading the file failed");
        msgBox.exec();
    }
}

void MainWindow::loadSettings()
{
    bool fileLoaded = false;
    QString fileName = QFileDialog::getOpenFileName(Q_NULLPTR, "Load Settings", QString());

    QFile file(fileName);

    if(file.open(QIODevice::ReadOnly)) {

        QByteArray bArr = file.readAll();
        file.close();

        if(mavenParameters->loadSettings(bArr.data()))
            fileLoaded = true;

    }

    if(fileLoaded)
        emit loadedSettings();

    else {
        // display an error message
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setText("Loading the file failed");
        msgBox.exec();
    }
}

void MainWindow::sendAnalytics(bool checked) {

    QString btnName = QObject::sender()->objectName();
    analytics->hitScreenView(btnName);
    if (checked && btnName == "Fragmentation Spectra") {
        analytics->hitEvent("PRM", "OpenedFragmentationSpectra");
    }

    if (checked && btnName == "Fragmentation Events") {
        analytics->hitEvent("PRM", "OpenedFragmentationEvents");
    }
}

void MainWindow::createToolBars() {

	QToolBar *toolBar = new QToolBar(this);
	toolBar->setObjectName("mainToolBar");
	toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24));

	QToolButton *btnOpen = new QToolButton(toolBar);
	btnOpen->setText("Open");
	btnOpen->setIcon(QIcon(rsrcPath + "/fileopen.png"));
	btnOpen->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnOpen->setStyleSheet("QToolTip {color: #000000; background-color: #fbfbd5; border: 1px solid black; padding: 1px;}");
	btnOpen->setToolTip(tr("Sample uploads"));

	QToolButton *btnAlign = new QToolButton(toolBar);
	btnAlign->setText("Align");
	btnAlign->setIcon(QIcon(rsrcPath + "/textcenter.png"));
	btnAlign->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnAlign->setStyleSheet("QToolTip {color: #000000; background-color: #fbfbd5; border: 1px solid black; padding: 1px;}");
	btnAlign->setToolTip(tr("Peak Alignment settings"));

    QToolButton *btnIsotope = new QToolButton(toolBar);
    btnIsotope->setText("Isotopes");
    btnIsotope->setIcon(QIcon(rsrcPath + "/isotopeIcon.png"));
    btnIsotope->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btnIsotope->setStyleSheet("QToolTip {color: #000000; background-color: #fbfbd5; border: 1px solic black; padding: 1px;}");
    btnIsotope->setToolTip(tr("Isotope settings"));

	//TODO: Sahil-Kiran, Removed while merging mainwindow
	//QToolButton *btnDbSearch = new QToolButton(toolBar);
	//btnDbSearch->setText("Databases");
	//btnDbSearch->setIcon(QIcon(rsrcPath + "/dbsearch.png"));
	//btnDbSearch->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	//btnDbSearch->setToolTip(tr("Database Search"));

	QToolButton *btnFeatureDetect = new QToolButton(toolBar);
	btnFeatureDetect->setText("Peaks");
	btnFeatureDetect->setIcon(QIcon(rsrcPath + "/featuredetect.png"));
	btnFeatureDetect->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnFeatureDetect->setStyleSheet("QToolTip {color: #000000; background-color: #fbfbd5; border: 1px solid black; padding: 1px;}");
	btnFeatureDetect->setToolTip(tr("Peak Detection and Group Filtering Settings"));

	QToolButton *btnPollyBridge = new QToolButton(toolBar);
	btnPollyBridge->setText("Polly");
	btnPollyBridge->setIcon(QIcon(rsrcPath + "/POLLY.png"));
	btnPollyBridge->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnPollyBridge->setStyleSheet("QToolTip {color: #000000; background-color: #fbfbd5; border: 1px solid black; padding: 1px;}");
	btnPollyBridge->setToolTip(tr("Send Peaks to Polly to store, collaborate, analyse and visualise your data"));

	// QToolButton *btnSpectraMatching = new QToolButton(toolBar);
	// btnSpectraMatching->setText("Match");
	// btnSpectraMatching->setIcon(QIcon(rsrcPath + "/spectra_search.png"));
	// btnSpectraMatching->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	// btnSpectraMatching->setStyleSheet("QToolTip {color: #000000; background-color: #fbfbd5; border: 1px solid black; padding: 1px;}");
	// btnSpectraMatching->setToolTip(
	// 		tr("Matching Spectra for Fragmentation Patterns"));

	QToolButton *btnSettings = new QToolButton(toolBar);
	btnSettings->setText("Options");
	btnSettings->setIcon(QIcon(rsrcPath + "/settings.png"));
	btnSettings->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnSettings->setStyleSheet("QToolTip {color: #000000; background-color: #fbfbd5; border: 1px solid black; padding: 1px;}");
	btnSettings->setToolTip(tr("1. Instrumentation: Ionization settings\n2. File Import: Scan filter settings\n3. Peak Detection: EIC smoothing and baseline settings\n4. Peak Filtering: Parent and Isotopic peak filtering settings\n5. Isotope Detection: Isotopic label filters\n6. EIC (XIC): EIC type selection\n7. Peak Grouping: Peak Grouping Score calculation\n8. Group Rank: Group Rank calculation - Group Rank decides which groups are selected for a given m/z"));

    QToolButton *btnInfo = new QToolButton(toolBar);
    btnInfo->setText("Support");
    btnInfo->setIcon(QIcon(rsrcPath + "/support.png"));
    btnInfo->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btnInfo->setStyleSheet("QToolTip {"
                           "color: #000000;"
                           "background-color: #fbfbd5;"
                           "border: 1px solid black;"
                           "padding: 1px;"
                           "}");
    btnInfo->setToolTip(tr("Documentaion, information and technical "
                           "support for El-MAVEN."));

	connect(btnOpen, SIGNAL(clicked()), SLOT(open()));
	connect(btnAlign, SIGNAL(clicked()), alignmentDialog, SLOT(show()));
    connect(btnIsotope, &QToolButton::clicked, isotopeDialog, &IsotopeDialog::show);
	//connect(btnDbSearch, SIGNAL(clicked()), SLOT(showPeakdetectionDialog())); //TODO: Sahil-Kiran, Removed while merging mainwindow
	connect(btnFeatureDetect, SIGNAL(clicked()), SLOT(showPeakdetectionDialog()));
	connect(btnPollyBridge, SIGNAL(clicked()), SLOT(showPollyElmavenInterfaceDialog()));
	connect(btnSettings, SIGNAL(clicked()), SLOT(showsettingsForm()));
	//connect(btnSpectraMatching, SIGNAL(clicked()), SLOT(showspectraMatchingForm()));
    connect(btnInfo, &QToolButton::clicked, [this] {
        if (_infoDialog != nullptr)
            _infoDialog->exec();
    });

	toolBar->addWidget(btnOpen);
	toolBar->addWidget(btnAlign);
    toolBar->addWidget(btnIsotope);
	toolBar->addWidget(btnFeatureDetect);
	//toolBar->addWidget(btnSpectraMatching);
	toolBar->addWidget(btnSettings);
	toolBar->addWidget(btnPollyBridge);
    toolBar->addWidget(btnInfo);

	QWidget *hBox = new QWidget(toolBar);
	(void) toolBar->addWidget(hBox);

	QHBoxLayout *layout = new QHBoxLayout(hBox);
	layout->setSpacing(0);
	layout->addWidget(new QWidget(hBox), 15); // spacer

	massCutoffComboBox=  new QComboBox(hBox);
	massCutoffComboBox->addItem("ppm");
	massCutoffComboBox->addItem("mDa");
    /*if(settings->value("massCutoffType")=="mDa"){
        massCutoffComboBox->setCurrentText("mDa");
    }
    else{
        massCutoffComboBox->setCurrentText("ppm");
    }*/
	massCutoffComboBox->setToolTip("mass cutoff unit");
	connect(massCutoffComboBox, SIGNAL(currentIndexChanged(QString)),this,SLOT(setMassCutoffType(QString)));

    /* note: on changing mass cut off type from mainwindow, it's important that it's also changed in peaks dialog */
    connect(massCutoffComboBox, &QComboBox::currentTextChanged, peakDetectionDialog, &PeakDetectionDialog::setMassCutoffType);

    //ppmValue
    massCutoffWindowBox = new QDoubleSpinBox(hBox);
    massCutoffWindowBox->setRange(0.00, 100000.0);
    massCutoffWindowBox->setDecimals(6);
    massCutoffWindowBox->setSingleStep(0.5);
    massCutoffWindowBox->setToolTip("Mass Cut-off Unit");
    // connect(massCutoffWindowBox, SIGNAL(valueChanged(double)), this,
    // 		SLOT(setUserPPM(double)));
    // connect(massCutoffWindowBox, SIGNAL(valueChanged(double)), eicWidget,
    // 		SLOT(setPPM(double)));
    connect(massCutoffWindowBox,
            SIGNAL(valueChanged(double)),
            this,
            SLOT(setUserMassCutoff(double)));
    void (QDoubleSpinBox::* doubleChanged)(double) = &QDoubleSpinBox::valueChanged;
    connect(massCutoffWindowBox,
            doubleChanged,
            [=] (double value) {
                if (!peakDetectionDialog->isVisible())
                    peakDetectionDialog->compoundPPMWindow->setValue(value);
            });
    massCutoffWindowBox->setValue(settings->value("massCutoffWindowBox").toDouble());

    searchText = new QLineEdit(hBox);
    searchText->setMinimumWidth(100);
    searchText->setPlaceholderText("MW / Compound");   
    searchText->setToolTip("<b>Text Search</b> <br> Compound Names: <b>ATP</b> <br> MRM: <b>precursorMz-productMz</b> <br> Patterns: <b>[45]-phosphate</b> <br>Formulas: <b> C6H10* </b>");
    searchText->setObjectName(QString::fromUtf8("searchText"));
    searchText->setShortcutEnabled(true);
    connect(searchText,SIGNAL(textEdited(QString)),this,SLOT(doSearch(QString))); 
	connect(searchText,SIGNAL(returnPressed()), SLOT(setMzValue()));	

	QShortcut* ctrlK = new QShortcut(QKeySequence(tr("Ctrl+K", "Do Search")),
			this);
	QShortcut* ctrlF = new QShortcut(QKeySequence(tr("Ctrl+F", "Do Search")),
			this);

	connect(ctrlK, SIGNAL(activated()), searchText, SLOT(selectAll()));
	connect(ctrlK, SIGNAL(activated()), searchText, SLOT(setFocus()));

	connect(ctrlF, SIGNAL(activated()), searchText, SLOT(selectAll()));
	connect(ctrlF, SIGNAL(activated()), searchText, SLOT(setFocus()));

	suggestPopup = new SuggestPopup(searchText);
	connect(suggestPopup, SIGNAL(compoundSelected(Compound*)), this,
			SLOT(setCompoundFocus(Compound*)));
	connect(suggestPopup, SIGNAL(compoundSelected(Compound*)), ligandWidget,
			SLOT(setCompoundFocus(Compound*)));
	connect(suggestPopup, SIGNAL(pathwaySelected(Pathway*)), pathwayPanel,
			SLOT(show()));
	connect(suggestPopup, SIGNAL(pathwaySelected(Pathway*)), pathwayDockWidget,
			SLOT(show()));
	connect(suggestPopup, SIGNAL(pathwaySelected(Pathway*)), this,
			SLOT(setPathwayFocus(Pathway*)));
	connect(ligandWidget, SIGNAL(databaseChanged(QString)), suggestPopup,
			SLOT(setDatabase(QString)));
	layout->addSpacing(10);

	ionizationModeLabel = new QLabel(hBox);
	ionizationModeLabel->setToolTip("Ionization Mode");
	ionizationModeLabel->setFrameShape(QFrame::Panel);
	ionizationModeLabel->setFrameShadow(QFrame::Raised);

    ionChargeBox = new QSpinBox(hBox);
    connect(ionChargeBox, SIGNAL(valueChanged(int)), this, SLOT(setTotalCharge()));
    ionChargeBox->setValue(settings->value("ionChargeBox").toInt());

    quantType = new QComboBox(hBox);
    quantType->addItem("AreaTop");
    quantType->addItem("Area");
    quantType->addItem("Height");
    quantType->addItem("AreaNotCorrected");
    quantType->addItem("AreaTopNotCorrected");
    quantType->addItem("Retention Time");
    quantType->addItem("Quality");
    quantType->setToolTip("Peak Quantitation Type");
    connect(quantType, SIGNAL(activated(int)), eicWidget, SLOT(replot()));
    connect(quantType,
            SIGNAL(currentIndexChanged(int)),
            SLOT(refreshIntensities()));
    connect(quantType,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            isotopeWidget,
            &IsotopeWidget::refreshForCurrentPeak);
    fileLoader->insertSettingForSave("mainWindowPeakQuantitation",
                                     variant(0));

    settings->beginGroup("searchHistory");
    QStringList keys = settings->childKeys();
    Q_FOREACH (QString key, keys)
        suggestPopup->addToHistory(key, settings->value(key).toInt());
    settings->endGroup();

    layout->addWidget(ionizationModeLabel, 0);
    layout->addWidget(new QLabel("Charge", hBox), 0);
    layout->addWidget(ionChargeBox, 0);
    layout->addWidget(quantType, 0);
    layout->addWidget(new QLabel("[m/z]", hBox), 0);
    layout->addWidget(searchText, 0);
    layout->addWidget(new QLabel("+/-", 0, 0));
    layout->addWidget(massCutoffWindowBox, 0);
    layout->addWidget(massCutoffComboBox, 0);

    sideBar = new QToolBar(this);
    sideBar->setObjectName("sideBar");
    sideBar->setIconSize(QSize(24, 24));

    QToolButton* btnSamples = addDockWidgetButton(sideBar,
												  projectDockWidget,
												  QIcon(rsrcPath + "/samples.png"),
												  "Show Samples Widget (F2)");
    QToolButton* btnLigands = addDockWidgetButton(sideBar,
												  ligandWidget,
												  QIcon(rsrcPath + "/molecule.png"),
												  "Show Compound Widget (F3)");
    QToolButton* btnSpectra = addDockWidgetButton(sideBar,
												  spectraDockWidget,
												  QIcon(rsrcPath + "/spectra.png"),
												  "Show Spectra Widget (F4)");
	QToolButton* btnFragSpectra = addDockWidgetButton(sideBar,
													  fragSpectraDockWidget,
													  QIcon(rsrcPath + "/fragSpectra.png"),
													  "Show Fragmentation Spectra");
	QToolButton* btnFragEvents = addDockWidgetButton(sideBar,
													 fragPanel,
													 QIcon(rsrcPath + "/fragmentationEvents.png"),
													 "Show MS2 Events List");

    QToolButton* btnAlignment = new QToolButton(sideBar);
    btnAlignment->setIcon(QIcon(rsrcPath + "/alignmentButton.png"));
    btnAlignment->setText("Alignment Visualizations");
    QMenu* alignmentMenu = new QMenu("Alignment Visualizations Menu");

    btnAlignment->setMenu(alignmentMenu);
    btnAlignment->setPopupMode(QToolButton::InstantPopup);


    QAction* perGroupAlignment = alignmentMenu->addAction(QIcon(rsrcPath + "/groupRtViz.png"), "Per Group Deviation");
    QAction* allGroupAlignment = alignmentMenu->addAction(QIcon(rsrcPath + "/alignmentVizAllGroups.png"), "All Groups Deviation");
    QAction* sampleRtDeviation = alignmentMenu->addAction(QIcon(rsrcPath + "/sampleRtViz.png"), "Sample Deviation");


    connect(perGroupAlignment, &QAction::triggered, this, &MainWindow::togglePerGroupAlignmentWidget);
    connect(allGroupAlignment, &QAction::triggered, this, &MainWindow::toggleAllGroupAlignmentWidget);
    connect(sampleRtDeviation, &QAction::triggered, this, &MainWindow::toggleSampleRtWidget);

    QToolButton* btnIsotopes = addDockWidgetButton(sideBar,isotopeWidget,QIcon(rsrcPath + "/isotope.png"), "Show Isotopes Widget (F5)");
    QToolButton* btnFindCompound = addDockWidgetButton(sideBar,massCalcWidget,QIcon(rsrcPath + "/findcompound.png"), "Show Match Compound Widget (F6)");
    QToolButton* btnCovariants = addDockWidgetButton(sideBar,covariantsPanel,QIcon(rsrcPath + "/covariants.png"), "Find Covariants Widget (F7)");
    //QToolButton* btnPathways = addDockWidgetButton(sideBar,pathwayDockWidget,QIcon(rsrcPath + "/pathway.png"), "Show Pathway Widget (F8)");
    QToolButton* btnNotes = addDockWidgetButton(sideBar,notesDockWidget,QIcon(rsrcPath + "/note.png"), "Show Notes Widget (F9)");
    QToolButton* btnBookmarks = addDockWidgetButton(sideBar,bookmarkedPeaks,QIcon(rsrcPath + "/showbookmarks.png"), "Show Bookmarks (F10)");
    QToolButton* btnGallery = addDockWidgetButton(sideBar,galleryDockWidget,QIcon(rsrcPath + "/gallery.png"), "Show Gallery Widget");
    QToolButton* btnSRM = addDockWidgetButton(sideBar,srmDockWidget,QIcon(rsrcPath + "/qqq.png"), "Show SRM List (F12)");
    // QToolButton* btnRconsole = addDockWidgetButton(sideBar,rconsoleDockWidget,QIcon(rsrcPath + "/R.png"), "Show R Console");


	//btnSamples->setShortcut(Qt::Key_F2);
	btnLigands->setShortcut(Qt::Key_F3);
	btnSpectra->setShortcut(Qt::Key_F4);
	btnIsotopes->setShortcut(Qt::Key_F5);
	btnFindCompound->setShortcut(Qt::Key_F6);
	btnCovariants->setShortcut(Qt::Key_F7);
	//btnPathways->setShortcut(Qt::Key_F8);
	btnNotes->setShortcut(Qt::Key_F8);
	btnBookmarks->setShortcut(Qt::Key_F9);
	btnSRM->setShortcut(Qt::Key_F10);

    connect(btnGallery, SIGNAL(clicked()), getEicWidget(), SLOT(setGalleryToEics()));

	connect(pathwayDockWidget, SIGNAL(visibilityChanged(bool)), pathwayPanel,
			SLOT(setVisible(bool)));
	connect(btnSRM, SIGNAL(clicked(bool)), SLOT(showSRMList()));

	sideBar->setOrientation(Qt::Vertical);
	sideBar->setMovable(false);

	sideBar->addWidget(btnSamples);
	sideBar->addWidget(btnLigands);
    sideBar->addWidget(btnSpectra);
	sideBar->addWidget(btnFragSpectra);
	sideBar->addWidget(btnFragEvents);
    sideBar->addWidget(btnAlignment);
	sideBar->addWidget(btnIsotopes);
	sideBar->addWidget(btnFindCompound);
	sideBar->addWidget(btnCovariants);
	//sideBar->addWidget(btnPathways);
	sideBar->addWidget(btnNotes);
	sideBar->addWidget(btnSRM);
	sideBar->addWidget(btnGallery);
	// sideBar->addWidget(btnRconsole);
	sideBar->addSeparator();
	sideBar->addWidget(btnBookmarks);
	// sideBar->addWidget(btnHeatmap);

	addToolBar(Qt::TopToolBarArea, toolBar);
	addToolBar(Qt::RightToolBarArea, sideBar);
}

void MainWindow::togglePerGroupAlignmentWidget()
{
    if(groupRtDockWidget->isVisible()) {
        groupRtDockWidget->hide();
        return;
    }

    groupRtDockWidget->show();
}

void MainWindow::toggleAllGroupAlignmentWidget()
{

    if(alignmentVizAllGroupsDockWidget->isVisible()) {
        alignmentVizAllGroupsDockWidget->hide();
        return;
    }

    alignmentVizAllGroupsDockWidget->show();

}

void MainWindow::toggleSampleRtWidget()
{
    if(sampleRtWidget->isVisible()) {
        sampleRtWidget->hide();
        return;
    }

    sampleRtWidget->show();
	sampleRtWidget->plotGraph();
}

void MainWindow::setMassCutoffType(QString massCutoffType){
	double cutoff=massCutoffWindowBox->value();
	string type=massCutoffType.toStdString();
	_massCutoffWindow->setMassCutoffAndType(cutoff,type);
	massCalcWidget->setMassCutoff(_massCutoffWindow);
	eicWidget->setMassCutoff(_massCutoffWindow);
}

void MainWindow::refreshIntensities()
{
    QList<QPointer<TableDockWidget> > peaksTableList = getPeakTableList();
    for(const auto& groupTable : peaksTableList) {
        groupTable->showAllGroups();
    }
    bookmarkedPeaks->showAllGroups();

    fileLoader->insertSettingForSave("mainWindowPeakQuantitation",
                                     variant(quantType->currentIndex()));
}

void MainWindow::_postProjectLoadActions()
{
    emit loadedSettings();

    refreshIntensities();
    if (bookmarkedPeaks->allgroups.size() > 0)
        bookmarkedPeaks->setVisible(true);

    _updateEMDBProgressBar(5, 5);
}

void MainWindow::_handleUnrecognizedProjectVersion(QString projectFilename)
{
    QString message("The project \"%1\" seems to have been created in a later "
                    "version of El-MAVEN. It cannot be opened in El-MAVEN %2.");
    message = message.arg(projectFilename).arg(appVersion());
    QMessageBox::warning(this,
                         tr("Unrecognized Project Version"),
                         message,
                         QMessageBox::Ok,
                         QMessageBox::Ok);
}

void MainWindow::showspectraMatchingForm() {

	spectraMatchingForm->exec();
}

// TODO remove this redundant function
void MainWindow::showsettingsForm() {

	analytics->hitScreenView("OptionsDialog");
	settingsForm->setInitialGroupRank();
	settingsForm->exec();
}

void MainWindow::historyLast() {
    analytics->hitEvent("History", "History Back");
	if (history.size() == 0)
		return;
	eicWidget->setMzSlice(history.last());
}

void MainWindow::historyNext() {
    analytics->hitEvent("History", "History Forward");
	if (history.size() == 0)
		return;
	eicWidget->setMzSlice(history.next());
}

void MainWindow::addToHistory(const mzSlice& slice) {
	history.addToHistory(slice);
}

bool MainWindow::addSample(mzSample* sample) {
	if (sample && sample->scans.size() > 0) {
		samples.push_back(sample);
		mavenParameters->samples.push_back(sample);	
		settingsForm->setSettingsIonizationMode("Auto Detect");		
		return true;
	} else {
		delete (sample);
		return false;
	}
}

void MainWindow::showPeakdetectionDialog() {

    peakDetectionDialog->show();      
}

void MainWindow::showPollyElmavenInterfaceDialog() {
	pollyElmavenInterfaceDialog->initialSetup();
	analytics->hitScreenView("PollyDialog");
}

void MainWindow::showSRMList() {


	if (srmDockWidget->isVisible()) {

        int userPolarity = 0;
		if (getIonizationMode()) userPolarity = getIonizationMode();

		bool associateCompoundNames = true;

        deque<Compound*> compoundsDB = DB.getCompoundsDB();

		double amuQ1 = getSettings()->value("amuQ1").toDouble();
        double amuQ3 = getSettings()->value("amuQ3").toDouble();

		srmList = new SRMList(samples, compoundsDB);
		srmList->setAnnotation(annotation);
		vector<mzSlice*>slices = srmList->getSrmSlices(amuQ1, amuQ3, userPolarity, associateCompoundNames);

		if (slices.size() ==  0 ) return;
        srmDockWidget->setInfo(slices);
        delete_all(slices);
     }
}

void MainWindow::setPeakGroup(PeakGroup* group) {
    qDebug() << "setPeakgroup(group)" << endl;
	if (group == NULL)
		return;

	searchText->setText(QString::number(group->meanMz, 'f', 8));

	if (eicWidget && eicWidget->isVisible()) {
		eicWidget->setPeakGroup(group);
	}

	if (isotopeWidget && isotopeWidget->isVisible() && group->getCompound() != NULL) {
		isotopeWidget->setPeakGroupAndMore(group);
    } else if (isotopeWidget
               && isotopePlot
               && isotopePlot->isVisible()
               && group
               && group->getCompound() != NULL) {
        isotopeWidget->updateIsotopicBarplot(group);
    }

    if ( group->getCompound() != NULL) {
		if (group->ms2EventCount) fragSpectraDockWidget->setVisible(true);
		if (fragSpectraDockWidget->isVisible()) {
			fragSpectraWidget->overlayPeakGroup(group);
		}
        QString compoundName(group->getCompound()->name().c_str());
        if (! setPeptideSequence(compoundName)) {
            setUrl(group->getCompound());
        }
        if (massCalcWidget)
            massCalcWidget->setPeakGroup(group);
    }

	if (scatterDockWidget->isVisible()) {
		((ScatterPlot*) scatterDockWidget)->showSimilar(group);
	}

    if (group->peaks.size() > 0) {
        vector<Scan*>scanset = group->getRepresentativeFullScans(); //TODO: Sahil-Kiran, Added while merging mainwindow
        spectraWidget->setScanSet(scanset); //TODO: Sahil-Kiran, Added while merging mainwindow
        spectraWidget->replot(); //TODO: Sahil-Kiran, Added while merging mainwindow
    }

	//TODO: Sahil-Kiran, Added while merging mainwindow
    if (spectralHitsDockWidget->isVisible()) {
        spectralHitsDockWidget->limitPrecursorMz(group->meanMz);
    }
}

void MainWindow::Align()
{
    if (sampleCount() < 2)
        return;

    BackgroundPeakUpdate* workerThread;

    if (alignmentDialog->alignAlgo->currentIndex() == 0) {
        analytics->hitEvent("Alignment", "Obi-Warp");
        workerThread = newWorkerThread("alignWithObiWarp");
        workerThread->setMavenParameters(mavenParameters);
        alignmentDialog->setWorkerThread(workerThread);
        connect(workerThread,
                SIGNAL(finished()),
                eicWidget,
                SLOT(replotForced()));
        connect(workerThread,
                SIGNAL(finished()),
                alignmentDialog,
                SLOT(close()));
        connect(workerThread,
                SIGNAL(restoreAlignment()),
                alignmentDialog,
                SLOT(updateRestoreStatus()));
        workerThread->start();
        return;
    }

    if (alignmentDialog->peakDetectionAlgo->currentText()
        == "Compound Database Search") {
        workerThread = newWorkerThread("alignUsingDatabase");
        mavenParameters->setCompounds(DB.getCompoundsSubset(
            alignmentDialog->selectDatabaseComboBox->currentText()
                .toStdString()));
        alignmentDialog->setWorkerThread(workerThread);
    } else {
        workerThread = newWorkerThread("processMassSlices");
        alignmentDialog->setWorkerThread(workerThread);
    }

    connect(workerThread, SIGNAL(finished()), eicWidget, SLOT(replotForced()));
    connect(workerThread, SIGNAL(finished()), alignmentDialog, SLOT(close()));

    mavenParameters->minGoodGroupCount =
        alignmentDialog->minGoodPeakCount->value();
    mavenParameters->limitGroupCount =
        alignmentDialog->limitGroupCount->value();
    mavenParameters->minGroupIntensity =
        alignmentDialog->minGroupIntensity->value();
    mavenParameters->minIntensity = alignmentDialog->minIntensity->value();
    mavenParameters->maxIntensity = alignmentDialog->maxIntensity->value();

    // TODO: Sahil Re-verify this two parameters. Values are same
    // mavenParameters->eic_smoothingWindow =
    //     alignmentDialog->groupingWindow->value();
    // while merging mainwindow
    mavenParameters->rtStepSize = alignmentDialog->groupingWindow->value();

    mavenParameters->minSignalBaseLineRatio = alignmentDialog->minSN->value();
    mavenParameters->minNoNoiseObs = alignmentDialog->minPeakWidth->value();

    mavenParameters->minSignalBlankRatio = 0;
    mavenParameters->alignMaxIterations =
        alignmentDialog->maxIterations->value();
    mavenParameters->alignPolynomialDegree =
        alignmentDialog->polynomialDegree->value();

    mavenParameters->alignSamplesFlag = true;
    mavenParameters->keepFoundGroups = true;
    mavenParameters->eicMaxGroups = peakDetectionDialog->eicMaxGroups->value();

    mavenParameters->samples = getSamples();
    mavenParameters->stop = false;
    workerThread->setMavenParameters(mavenParameters);
    workerThread->setPeakDetector(new PeakDetector(mavenParameters));

    // connect new connections
    connect(
        workerThread,
        SIGNAL(newPeakGroup(PeakGroup*)),
        bookmarkedPeaks,
        SLOT(addPeakGroup(PeakGroup*)));
    connect(workerThread,
            SIGNAL(finished()),
            bookmarkedPeaks,
            SLOT(showAllGroups()));

    workerThread->start();
}

void MainWindow::plotAlignmentVizAllGroupGraph(QList<PeakGroup> allgroups) {
	alignmentVizAllGroupsWidget->plotGraph(allgroups);
}


void MainWindow::showAlignmentWidget() {

	alignmentVizAllGroupsDockWidget->setVisible(true);
	alignmentVizAllGroupsDockWidget->raise();

}

void MainWindow::UndoAlignment()
{
	alignmentDialog->samplesAligned(false);
	
	for (auto sample : samples) {
		for(auto scan : sample->scans)
			if(scan->originalRt >= 0)
				scan->rt = scan->originalRt;
	}

	getEicWidget()->replotForced();

	mavenParameters->alignButton = 0;

	QList<PeakGroup> listGroups;
	for (auto group : mavenParameters->undoAlignmentGroups) {
		listGroups.append(group);
	}
	
	alignmentDialog->close();
	Q_EMIT(undoAlignment(listGroups));
}


void MainWindow::showPeakInfo(Peak* _peak) {
	if (_peak == NULL)
		return;

	mzSample* sample = _peak->getSample();
	if (sample == NULL)
		return;

	Scan* scan = sample->getScan(_peak->scan);
	if (scan == NULL)
		return;

	int ionizationMode = scan->getPolarity();
	//if (getIonizationMode())
		ionizationMode = mavenParameters->ionizationMode; //user specified ionization mode

	if (spectraDockWidget->isVisible() && scan) {
		spectraWidget->setScan(_peak);
	}

	if (isotopeWidget->isVisible()) {
		isotopeWidget->setIonizationMode(ionizationMode);
		isotopeWidget->setCharge(ionizationMode);
	}

	if (fragPanel->isVisible()) {
		showFragmentationScans(_peak->peakMz);
	}

    if (spectralHitsDockWidget->isVisible() && scan) {
        spectralHitsDockWidget->limitPrecursorMz(_peak->peakMz);
    }
}




void MainWindow::spectaFocused(Peak* _peak) {
	if (_peak == NULL)
		return;

	mzSample* sample = _peak->getSample();
	if (sample == NULL)
		return;

	Scan* scan = sample->getScan(_peak->scan);
	if (scan == NULL)
		return;

	int ionizationMode = scan->getPolarity();
	//if (getIonizationMode())
		ionizationMode = mavenParameters->ionizationMode; //user specified ionization mode

	if (spectraDockWidget->isVisible() && scan) {
		spectraWidget->setScan(_peak);
	}

	//TODO: Sahil-Kiran, Added while merging mainwindow
    if (spectralHitsDockWidget->isVisible() && scan) {
        spectralHitsDockWidget->limitPrecursorMz(_peak->peakMz);
    }
	massCalcWidget->setMass(_peak->peakMz);

}

// void MainWindow::setupSampleColors() {

// 	float N = samples.size();

// 	if (!sample) continue;

// 	if (sample->color[0] + sample->color[1] + sample->color[2]
// 			> 0)
// 		continue;
// 	//set blank to non transparent red
// 	if (sample->isBlank) {
// 		sample->color[0] = 0.9;
// 		sample->color[1] = 0.0;
// 		sample->color[2] = 0.0;
// 		sample->color[3] = 1.0;
// 		continue;
// 	}

// 	float hue = 1 - 0.6 * ((float) (i + 1) / N);
// 	QColor c = QColor::fromHsvF(hue, 1.0, 1.0, 1.0);
// 	//qDebug() << "SAMPLE COLOR=" << c;

// 	sample->color[0] = c.redF();
// 	sample->color[1] = c.greenF();
// 	sample->color[2] = c.blueF();
// 	sample->color[3] = c.alphaF();
// }

//TODO: Check whether AreaTopNotCorrected should be added to this,
//and whether the extra column would break anything
QString MainWindow::groupTextExport(PeakGroup* group) {

	if (group == NULL || group->isEmpty())
		return QString();

	QStringList groupInfo;
	QString compoundName;
	float expectedRt = -1;

	if (group->hasCompoundLink()) {
                compoundName = "\"" + QString(group->getCompound()->name().c_str()) + "\"";
                expectedRt = group->getCompound()->expectedRt();
	}

	if (compoundName.isEmpty() && group->srmId.length()) {
		compoundName = "\"" + QString(group->srmId.c_str()) + "\"";
	}

	//sort peaks
	sort(group->peaks.begin(), group->peaks.end(), Peak::compSampleOrder);

	groupInfo
			<< "sample\tgroupId\tcompoundName\texpectedRt\tpeakMz\tmedianMz\trt\trtmin\trtmax\tquality\tpeakIntensity\tpeakArea\tpeakSplineArea\tpeakAreaTop\tpeakAreaCorrected\tnoNoiseObs\tsignalBaseLineRatio\tfromBlankSample";
	for (int j = 0; j < group->peaks.size(); j++) {
		QStringList peakinfo;
		Peak& peak = group->peaks[j];
		mzSample* s = peak.getSample();
		string sampleName;
		if (s != NULL)
			sampleName = s->sampleName;

		peakinfo << QString(sampleName.c_str())
				<< QString::number(group->groupId) << compoundName
				<< QString::number(expectedRt, 'f', 4)
				<< QString::number(peak.peakMz, 'f', 4)
				<< QString::number(peak.medianMz, 'f', 4)
				<< QString::number(peak.rt, 'f', 4)
				<< QString::number(peak.rtmin, 'f', 4)
				<< QString::number(peak.rtmax, 'f', 4)
				<< QString::number(peak.quality, 'f', 4)
				<< QString::number(peak.peakIntensity, 'f', 4)
				<< QString::number(peak.peakArea, 'f', 4)
				<< QString::number(peak.peakSplineArea, 'f', 4)
				<< QString::number(peak.peakAreaTop, 'f', 4)
				<< QString::number(peak.peakAreaCorrected, 'f', 4)
				<< QString::number(peak.noNoiseObs, 'f', 4)
				<< QString::number(peak.signalBaselineRatio, 'f', 4)
				<< QString::number(peak.fromBlankSample, 'f', 4);
		groupInfo << peakinfo.join("\t");
	}
	return groupInfo.join("\n");
}

void MainWindow::findCovariants(Peak* _peak) {
	if (covariantsPanel->isVisible()) {
		vector<mzLink> links = _peak->findCovariants();
		covariantsPanel->setInfo(links);
	}
}

void MainWindow::setClipboardToGroup(PeakGroup* group) {
	if (group == NULL || group->isEmpty())
		return;
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(groupTextExport(group));
}

void MainWindow::showFragmentationScans(float pmz)
{
    if (!fragPanel || fragPanel->isVisible() == false)
        return;
	
    MassCutoff *massCutoff = getUserMassCutoff();

    if (samples.size() <= 0)
        return;
    fragPanel->clearTree();
    for (auto sample : samples) {
        if (sample->ms1ScanCount() == 0)
            continue;

        for (auto scan : sample->scans) {
	        if (scan->mslevel != 2) continue;
	        if (massCutoffDist(scan->precursorMz,
                               pmz,
                               massCutoff) > massCutoff->getMassCutoff()) {
                continue;
            }
            fragPanel->addScanItem(scan);
        }
    }
}

void MainWindow::reorderSamples(PeakGroup* group) {
	if (group)
		group->reorderSamples();
	std::sort(samples.begin(), samples.end(), mzSample::compSampleOrder);
	if (projectDockWidget)
		projectDockWidget->updateSampleList();
	if (eicWidget)
		eicWidget->update();
	if (pathwayWidget)
		pathwayWidget->updateCompoundConcentrations();
}

bool MainWindow::checkCompoundExistance(Compound* c) {
	int charge = -1;
	// if (samples.size() > 0 && samples[0]->getPolarity() > 0)
	// 	charge = 1;
	// if (getIonizationMode())
	// 	charge = getIonizationMode(); //user specified ionization mode
	charge = mavenParameters->getCharge(c);
	float mz = c->adjustedMass(charge);
	float mzmin = mz - mz / 1e6 * 3;
	float mzmax = mz + mz / 1e6 * 3;

	for (unsigned int i = 0; i < samples.size(); i++) {
		int consectveMatches = 0;
		for (unsigned int j = 0; j < samples[i]->scans.size(); j++) {
			vector<int> matches = samples[i]->scans[j]->findMatchingMzs(mzmin,
					mzmax);
			if (matches.size() > 0) {
				consectveMatches++;
			} else
				consectveMatches = 0;
			if (consectveMatches > 3)
				return true;
		}
	}
	return false;
}

QWidget* MainWindow::eicWidgetController() {

	QToolBar *toolBar = new QToolBar(this);
	toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24));

	QWidgetAction *btnZoom = new MainWindowWidgetAction(toolBar, this,  "btnZoom");
	QWidgetAction *btnBookmark = new MainWindowWidgetAction(toolBar, this,  "btnBookmark");
	QWidgetAction *btnCopyCSV = new MainWindowWidgetAction(toolBar, this,  "btnCopyCSV");
	QWidgetAction *btnIntegrateArea = new MainWindowWidgetAction(toolBar, this,  "btnIntegrateArea");
	QWidgetAction *btnAverageSpectra = new MainWindowWidgetAction(toolBar, this,  "btnAverageSpectra");
	QWidgetAction *btnLast = new MainWindowWidgetAction(toolBar, this,  "btnLast");
	QWidgetAction *btnNext = new MainWindowWidgetAction(toolBar, this,  "btnNext");
	QWidgetAction *btnExport = new MainWindowWidgetAction(toolBar, this,  "btnExport");
	QWidgetAction *btnAutoZoom = new MainWindowWidgetAction(toolBar, this,  "btnAutoZoom");
	QWidgetAction *btnShowTic = new MainWindowWidgetAction(toolBar, this,  "btnShowTic");
	QWidgetAction *btnShowBarplot = new MainWindowWidgetAction(toolBar, this,  "btnShowBarplot");
	QWidgetAction *btnShowIsotopeplot = new MainWindowWidgetAction(toolBar, this,  "btnShowIsotopeplot");
	QWidgetAction *btnShowBoxplot = new MainWindowWidgetAction(toolBar, this,  "btnShowBoxplot");
    QWidgetAction *spacer = new MainWindowWidgetAction(toolBar,
                                                       this,
                                                       "spacer");
    QWidgetAction *toleranceSyncSwitch = new MainWindowWidgetAction(toolBar,
                                                                    this,
                                                                    "toleranceSyncSwitch");

	toolBar->addAction(btnZoom);
	toolBar->addAction(btnBookmark);
	toolBar->addAction(btnCopyCSV);

	toolBar->addSeparator();
	toolBar->addAction(btnIntegrateArea);
	toolBar->addAction(btnAverageSpectra);
	toolBar->addAction(btnLast);
	toolBar->addAction(btnNext);

	toolBar->addSeparator();
	toolBar->addAction(btnExport);

	toolBar->addSeparator();
	toolBar->addAction(btnAutoZoom);
	toolBar->addAction(btnShowTic);

    toolBar->addSeparator();
    toolBar->addAction(btnShowBarplot);
    toolBar->addAction(btnShowIsotopeplot);
    toolBar->addAction(btnShowBoxplot);

    toolBar->addAction(spacer);
    toolBar->addAction(toleranceSyncSwitch);

    QWidget *window = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setContentsMargins(QMargins(0, 0, 0, 0));
	layout->addWidget(toolBar);
	layout->addWidget(eicWidget);

	window->setLayout(layout);
	return window;
}

QWidget* MainWindowWidgetAction::createWidget(QWidget *parent) {


	if (btnName == "btnZoom") {

		QToolButton *btnZoom = new QToolButton(parent);
		btnZoom->setIcon(QIcon(rsrcPath + "/resetzoom.png"));
		btnZoom->setToolTip(tr("Zoom out (0)"));
		connect(btnZoom, SIGNAL(clicked()), mw->getEicWidget(), SLOT(resetZoom()));
		return btnZoom;

	}
	else if (btnName == "btnBookmark") {

		QToolButton *btnBookmark = new QToolButton(parent);
		btnBookmark->setIcon(QIcon(rsrcPath + "/bookmark.png"));
		btnBookmark->setToolTip(tr("Bookmark Group (Ctrl+D)"));
		btnBookmark->setShortcut(tr("Ctrl+D"));
		connect(btnBookmark, SIGNAL(clicked()), mw, SLOT(bookmarkPeakGroup()));
		return btnBookmark;

	}
	else if (btnName == "btnCopyCSV") {

		QToolButton *btnCopyCSV = new QToolButton(parent);
		btnCopyCSV->setIcon(QIcon(rsrcPath + "/copyCSV.png"));
		btnCopyCSV->setToolTip(tr("Copy Group Information to Clipboard (Ctrl+C)"));
		btnCopyCSV->setShortcut(tr("Ctrl+C"));
		connect(btnCopyCSV, SIGNAL(clicked()), mw->getEicWidget(), SLOT(copyToClipboard()));
		return btnCopyCSV;

	}
	else if (btnName == "btnIntegrateArea") {

		QToolButton *btnIntegrateArea = new QToolButton(parent);
		btnIntegrateArea->setIcon(QIcon(rsrcPath + "/integrateArea.png"));
		btnIntegrateArea->setToolTip(tr("Manual Integration (Shift+MouseDrag)"));
                connect(btnIntegrateArea, SIGNAL(clicked()), mw->getEicWidget(),
                                SLOT(startAreaIntegration()));
                connect(btnIntegrateArea, &QToolButton::clicked, [this]()
                {
                    mw->getAnalytics()->hitEvent("EIC Widget Button",
                                                 "Manual Integration");
                });
		return btnIntegrateArea;

	}
	else if (btnName == "btnAverageSpectra") {

		QToolButton *btnAverageSpectra = new QToolButton(parent);
		btnAverageSpectra->setIcon(QIcon(rsrcPath + "/averageSpectra.png"));
		btnAverageSpectra->setToolTip(tr("Average Specta (Ctrl+MouseDrag)"));
		connect(btnAverageSpectra, SIGNAL(clicked()), mw->getEicWidget(),
				SLOT(startSpectralAveraging()));
        connect(btnAverageSpectra,SIGNAL(clicked()),mw,SLOT(analyticsAverageSpectra()));
		return btnAverageSpectra;

	}
	else if (btnName == "btnLast") {

		QToolButton *btnLast = new QToolButton(parent);
		btnLast->setIcon(QIcon(rsrcPath + "/last.png"));
		btnLast->setToolTip(tr("History Back (Ctrl+Left)"));
		btnLast->setShortcut(tr("Ctrl+Left"));
		connect(btnLast, SIGNAL(clicked()), mw, SLOT(historyLast()));
		return btnLast;

	}
	else if (btnName == "btnNext") {

		QToolButton *btnNext = new QToolButton(parent);
		btnNext->setIcon(QIcon(rsrcPath + "/next.png"));
		btnNext->setToolTip(tr("History Forward (Ctrl+Right)"));
		btnNext->setShortcut(tr("Ctrl+Right"));
		connect(btnNext, SIGNAL(clicked()), mw, SLOT(historyNext()));		
		return btnNext;

	}
	else if (btnName == "btnExport") {
		
		QToolButton* btnExport = new QToolButton(parent);
		btnExport->setIcon(QIcon(rsrcPath + "/exportOptions.png"));
		btnExport->setToolTip("Export as…");
		QMenu* btnExportMenu = new QMenu("Export Image as…");
		btnExport->setMenu(btnExportMenu);
		btnExport->setPopupMode(QToolButton::InstantPopup);
		QAction* pdfAction = btnExportMenu->addAction(QIcon(rsrcPath + "/exportpdf.png"),
													  "Export EIC as PDF file");
		connect(pdfAction, &QAction::triggered, mw, &MainWindow::exportPDF);
		QAction* pngAction = btnExportMenu->addAction(QIcon(rsrcPath + "/copyPNG.png"),
													  "Copy EIC widget to the clipboard");
		connect(pngAction, &QAction::triggered, mw, &MainWindow::exportSVG);
		QAction* printAction = btnExportMenu->addAction(QIcon(rsrcPath + "/fileprint.png"),
														"Print EIC (Ctrl+P)");
		printAction->setShortcut(QKeySequence(tr("Ctrl+P")));
		connect(printAction, &QAction::triggered, mw, &MainWindow::print);
		return btnExport;

	}
	else if (btnName == "btnAutoZoom") {

		QToolButton *btnAutoZoom = new QToolButton(parent);
		btnAutoZoom->setCheckable(true);
		btnAutoZoom->setChecked(true);
		btnAutoZoom->setIcon(QIcon(rsrcPath + "/autofocus.png"));
		btnAutoZoom->setToolTip(
			tr("Auto Zoom. Always center chromatogram on expected retention time!"));

		connect(btnAutoZoom, SIGNAL(toggled(bool)), mw->getEicWidget(),
				SLOT(autoZoom(bool)));

		return btnAutoZoom;
	}
	else if (btnName == "btnShowTic") {

		QToolButton *btnShowTic = new QToolButton(parent);
		btnShowTic->setCheckable(true);
		btnShowTic->setChecked(false);
		btnShowTic->setIcon(QIcon(rsrcPath + "/tic.png"));
		btnShowTic->setToolTip(tr("Show TICs"));
		connect(btnShowTic, SIGNAL(toggled(bool)), mw->getEicWidget(),
				SLOT(showTicLine(bool)));
		connect(btnShowTic, SIGNAL(toggled(bool)), mw->getEicWidget(), SLOT(replot()));

		return btnShowTic;

	}
	else if (btnName == "btnShowBarplot") {

		QToolButton *btnShowBarplot = new QToolButton(parent);
		btnShowBarplot->setIcon(QIcon(rsrcPath + "/barplot.png"));
		btnShowBarplot->setToolTip(tr("Show Barplot"));
		btnShowBarplot->setCheckable(true);
		btnShowBarplot->setChecked(true);
		connect(btnShowBarplot,SIGNAL(toggled(bool)),  mw->getEicWidget(), SLOT(showBarPlot(bool)));
		connect(btnShowBarplot,SIGNAL(toggled(bool)), mw->getEicWidget(), SLOT(replot()));
		return btnShowBarplot;
	}
	else if (btnName == "btnShowIsotopeplot") {

		QToolButton *btnShowIsotopeplot = new QToolButton(parent);
		btnShowIsotopeplot->setIcon(QIcon(rsrcPath + "/isotopeplot.png"));
		btnShowIsotopeplot->setToolTip(tr("Show Isotope Plot"));
		btnShowIsotopeplot->setCheckable(true);
		connect(btnShowIsotopeplot,SIGNAL(clicked(bool)), mw, SLOT(toggleIsotopicBarPlot(bool)));
		connect(btnShowIsotopeplot,SIGNAL(clicked(bool)), mw->isotopeWidget, SLOT(updateIsotopicBarplot()));
		btnShowIsotopeplot->setChecked(mw->isotopePlotDockWidget->isVisible());
		connect(mw->isotopePlotDockWidget, SIGNAL(visibilityChanged(bool)), btnShowIsotopeplot,
				SLOT(setChecked(bool)));
		return btnShowIsotopeplot;
	}
	else if (btnName == "btnShowBoxplot") {
		
		QToolButton *btnShowBoxplot = new QToolButton(parent);
		btnShowBoxplot->setIcon(QIcon(rsrcPath + "/boxplot.png"));
		btnShowBoxplot->setToolTip(tr("Show Boxplot"));
		btnShowBoxplot->setCheckable(true);
		btnShowBoxplot->setChecked(false);
		connect(btnShowBoxplot,SIGNAL(toggled(bool)),  mw->getEicWidget(),SLOT(showBoxPlot(bool)));
		connect(btnShowBoxplot,SIGNAL(toggled(bool)),mw,SLOT(analyticsBoxPlot()));
		connect(btnShowBoxplot,SIGNAL(toggled(bool)), mw->getEicWidget(), SLOT(replot()));
		return btnShowBoxplot;
	}
    else if (btnName == "spacer") {
        QWidget* spacer = new QWidget(parent);
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        return spacer;
    }
    else if (btnName == "toleranceSyncSwitch") {
        QToolButton *toleranceSyncSwitch = new QToolButton(parent);
        toleranceSyncSwitch->setIcon(QIcon(rsrcPath
                                           + "/toleranceSyncUnlock.png"));
        toleranceSyncSwitch->setToolTip(tr("Sync EIC with current global mass "
                                           "tolerance"));
        toleranceSyncSwitch->setCheckable(true);
        toleranceSyncSwitch->setChecked(false);
        if (mw->getEicWidget()->getParameters()->selectedGroup() == nullptr){
            toleranceSyncSwitch->setEnabled(false);
        }

        connect(toleranceSyncSwitch, &QToolButton::toggled, this, [=](bool on) {
            if (on) {
                QIcon locked(rsrcPath + "/toleranceSyncLock.png");
                toleranceSyncSwitch->setIcon(locked);
            } else {
                QIcon unlocked(rsrcPath + "/toleranceSyncUnlock.png");
                toleranceSyncSwitch->setIcon(unlocked);
            }
            mw->getEicWidget()->setSensitiveToTolerance(!on);
        });
        connect(mw->getEicWidget(),
                &EicWidget::groupSet,
                toleranceSyncSwitch,
                [=](PeakGroup* selectedGroup) {
                    if (selectedGroup == nullptr)
                        return;

                    if (selectedGroup->sliceIsZero()) {
                        toleranceSyncSwitch->setChecked(false);
                        toleranceSyncSwitch->setDisabled(true);
                    } else {
                        toleranceSyncSwitch->setEnabled(true);
                    }
                });
        connect(mw->getEicWidget(),
                &EicWidget::compoundSet,
                toleranceSyncSwitch,
                [=](Compound* selectedCompound) {
                    toleranceSyncSwitch->setChecked(false);
                    toleranceSyncSwitch->setDisabled(true);
                });
        return toleranceSyncSwitch;
    }
	else {
		return NULL;
	}
}

QWidget* MainWindow::pathwayWidgetController() {

	QToolBar *toolBar = new QToolBar(this);
	toolBar->setFloatable(false);
	toolBar->setMovable(false);

	QToolButton *btnResetZoom = new QToolButton(toolBar);
	btnResetZoom->setIcon(QIcon(rsrcPath + "/resetzoom.png"));
	btnResetZoom->setToolTip(tr("ResetZoom"));
	connect(btnResetZoom, SIGNAL(clicked()), pathwayWidget, SLOT(resetZoom()));

	QToolButton *btnZoomIn = new QToolButton(toolBar);
	btnZoomIn->setIcon(QIcon(rsrcPath + "/zoomin.png"));
	btnZoomIn->setToolTip(tr("Zoom In"));
	connect(btnZoomIn, SIGNAL(clicked()), pathwayWidget, SLOT(zoomIn()));

	QToolButton *btnZoomOut = new QToolButton(toolBar);
	btnZoomOut->setIcon(QIcon(rsrcPath + "/zoomout.png"));
	btnZoomOut->setToolTip(tr("Zoom Out"));
	connect(btnZoomOut, SIGNAL(clicked()), pathwayWidget, SLOT(zoomOut()));

	QToolButton *btnTextZoomIn = new QToolButton(toolBar);
	btnTextZoomIn->setIcon(QIcon(rsrcPath + "/zoomInText.png"));
	btnTextZoomIn->setToolTip(tr("Increase Font Size"));
	connect(btnTextZoomIn, SIGNAL(clicked()), pathwayWidget,
			SLOT(increaseLabelSize()));

	QToolButton *btnTextZoomOut = new QToolButton(toolBar);
	btnTextZoomOut->setIcon(QIcon(rsrcPath + "/zoomOutText.png"));
	btnTextZoomOut->setToolTip(tr("Decrease Font Size"));
	connect(btnTextZoomOut, SIGNAL(clicked()), pathwayWidget,
			SLOT(decreaseLabelSize()));

	QToolButton *btnNodeZoomIn = new QToolButton(toolBar);
	btnNodeZoomIn->setIcon(QIcon(rsrcPath + "/zoomInNode.png"));
	btnNodeZoomIn->setToolTip(tr("Increase Node Size"));
	connect(btnNodeZoomIn, SIGNAL(clicked()), pathwayWidget,
			SLOT(increaseNodeSize()));

	QToolButton *btnNodeZoomOut = new QToolButton(toolBar);
	btnNodeZoomOut->setIcon(QIcon(rsrcPath + "/zoomOutNode.png"));
	btnNodeZoomOut->setToolTip(tr("Decrease Node Size"));
	connect(btnNodeZoomOut, SIGNAL(clicked()), pathwayWidget,
			SLOT(decreaseNodeSize()));

	QToolButton *btnEdgeZoomIn = new QToolButton(toolBar);
	btnEdgeZoomIn->setIcon(QIcon(rsrcPath + "/zoomInEdge.png"));
	btnEdgeZoomIn->setToolTip(tr("Increase Edge Size"));
	connect(btnEdgeZoomIn, SIGNAL(clicked()), pathwayWidget,
			SLOT(increaseEdgeSize()));

	QToolButton *btnEdgeZoomOut = new QToolButton(toolBar);
	btnEdgeZoomOut->setIcon(QIcon(rsrcPath + "/zoomOutEdge.png"));
	btnEdgeZoomOut->setToolTip(tr("Decrease Edge Size"));
	connect(btnEdgeZoomOut, SIGNAL(clicked()), pathwayWidget,
			SLOT(decreaseEdgeSize()));

	QToolButton *btnLoad = new QToolButton(toolBar);
	btnLoad->setIcon(QIcon(rsrcPath + "/fileopen.png"));
	btnLoad->setToolTip(tr("Load Pathway"));
	connect(btnLoad, SIGNAL(clicked()), pathwayWidget, SLOT(loadModelFile()));

	QToolButton *btnSave = new QToolButton(toolBar);
	btnSave->setIcon(QIcon(rsrcPath + "/filesave.png"));
	btnSave->setToolTip(tr("Save Layout"));
	connect(btnSave, SIGNAL(clicked()), pathwayWidget, SLOT(saveLayout()));

	QToolButton *btnReculculte = new QToolButton(toolBar);
	btnReculculte->setIcon(QIcon(rsrcPath + "/refresh.png"));
	btnReculculte->setToolTip(tr("Update Compound Concentrations"));
	connect(btnReculculte, SIGNAL(clicked()), pathwayWidget,
			SLOT(recalculateConcentrations()));

	AnimationControl* animationControl = new AnimationControl(this);
	connect(animationControl->slider, SIGNAL(valueChanged(int)), pathwayWidget,
			SLOT(showSample(int)));
	connect(pathwayWidget, SIGNAL(titleChanged(QString)),
			animationControl->titleLabel, SLOT(setText(QString)));
	animationControl->adjustSize();
	pathwayWidget->setAnimationControl(animationControl);

	toolBar->addWidget(btnResetZoom);
	toolBar->addWidget(btnZoomIn);
	toolBar->addWidget(btnZoomOut);
	toolBar->addWidget(btnNodeZoomIn);
	toolBar->addWidget(btnNodeZoomOut);
	toolBar->addWidget(btnEdgeZoomIn);
	toolBar->addWidget(btnEdgeZoomOut);
	toolBar->addWidget(btnTextZoomIn);
	toolBar->addWidget(btnTextZoomOut);
	toolBar->addWidget(btnReculculte);
	toolBar->addWidget(btnSave);
	toolBar->addWidget(btnLoad);
	toolBar->addWidget(animationControl);
	pathwayDockWidget->setTitleBarWidget(toolBar);

	return toolBar;
}
void MainWindow::getLinks(Peak* peak) {
	if (!peak)
		return;

	Scan* scan = peak->getScan();
	if (!scan)
		return;

	mzSample* sample = scan->getSample();
	if (!sample)
		return;

	int ionizationMode = scan->getPolarity();
	if (getIonizationMode())
		ionizationMode = getIonizationMode(); //user specified ionization mode

	MassCutoff *massCutoff = getUserMassCutoff();

	vector<mzLink> links = peak->findCovariants();
	vector<mzLink> linksX = SpectraWidget::findLinks(peak->peakMz, scan, massCutoff,
			ionizationMode);
	for (int i = 0; i < linksX.size(); i++)
		links.push_back(linksX[i]);

	//correlations
	float rtmin = peak->rtmin - 1;
	float rtmax = peak->rtmax + 1;
	for (int i = 0; i < links.size(); i++) {
		links[i].correlation = sample->correlation(links[i].mz1, links[i].mz2,
			massCutoff, rtmin, rtmax, mavenParameters->eicType, mavenParameters->filterline);
	}

	//matching compounds
	for (int i = 0; i < links.size(); i++) {
		QSet<Compound*> compunds = massCalcWidget->findMathchingCompounds(
				links[i].mz2, massCutoff, mavenParameters->getCharge());
		if (compunds.size() > 0)
                        Q_FOREACH( Compound*c, compunds){ links[i].note += " |" + c->name(); break;}
	}

	vector<mzLink> subset;
	for (int i = 0; i < links.size(); i++) {
		if (links[i].correlation > 0.5)
			subset.push_back(links[i]);
	}
	if (subset.size())
		covariantsPanel->setInfo(subset);
	if (subset.size() && galleryDockWidget->isVisible())
		galleryWidget->addEicPlots(subset);
}


PeakGroup::QType MainWindow::getUserQuantType() {
	if (quantType) {
		QString type = quantType->currentText();
		if (type == "AreaTop")
			return PeakGroup::AreaTop;
		else if (type == "Area")
			return PeakGroup::Area;
		else if (type == "Height")
			return PeakGroup::Height;
		else if (type == "Retention Time")
			return PeakGroup::RetentionTime;
		//TODO: Sahil-Kiran, Added while merging mainwindow
		else if (type  == "AreaNotCorrected") 
			return PeakGroup::AreaNotCorrected;
        else if (type  == "AreaTopNotCorrected")
            return PeakGroup::AreaTopNotCorrected;
		else if (type == "Quality")
			return PeakGroup::Quality;
		else if (type == "S/N Ratio")
			return PeakGroup::SNRatio;
	}
	return PeakGroup::AreaTop;
}

void MainWindow::markGroup(PeakGroup* group, char label) {
	if (!group)
		return;

	group->setLabel(label);
	bookmarkPeakGroup(group);
	//if (getClassifier()) { getClassifier()->refineModel(group); }
	//getPlotWidget()->scene()->update();
}

QString MainWindow::appVersion() {
        auto version = STR(EL_MAVEN_VERSION);
        return QString(version);
}

void MainWindow::toggleIsotopicBarPlot(bool show)
{
	if (show) {
		isotopePlotDockWidget->show();
		isotopePlotDockWidget->raise();
	}
	else {
		isotopePlotDockWidget->hide();
	}
}

void MainWindow::normalizeIsotopicMatrix(MatrixXf &MM) {
	for(int i = 0; i < MM.rows(); i++) {
		float sum = 0;
		for(int j = 0; j < MM.cols(); j++) sum += MM(i,j);
		if(sum<=0) continue;
		for(int j = 0; j < MM.cols(); j++) MM(i,j) /= sum;
	}
}

MatrixXf MainWindow::getIsotopicMatrix(PeakGroup* group) {

	PeakGroup::QType qtype = getUserQuantType();
	//get visiable samples
	vector<mzSample*> vsamples = getVisibleSamples();
	sort(vsamples.begin(), vsamples.end(), mzSample::compRevSampleOrder);
	map<unsigned int, string> carbonIsotopeSpecies;

	//get isotopic groups
	vector<PeakGroup*> isotopes;
	string delimIsotopic = "C13-label-";
	string delimParent = "C12 PARENT";
	for (int i = 0; i < group->childCountBarPlot(); i++) {
		if (group->childrenBarPlot[i].isIsotope()) {
			PeakGroup* isotope = &(group->childrenBarPlot[i]);
			isotopes.push_back(isotope);
			//Getting the labels of carbon
			if(isotope->tagString.find(delimIsotopic) != string::npos || isotope->tagString.find(delimParent) != string::npos) {
				if (isotope->tagString.find(delimParent) != string::npos) {
					carbonIsotopeSpecies.insert(pair<unsigned int, string>(0, isotope->tagString));
				} else if (isotope->tagString.find(delimIsotopic) != string::npos) {
					unsigned int carbonLabel = atoi(isotope->tagString.substr(delimIsotopic.size() - (isotope->tagString.size() - delimIsotopic.size() - 1)).c_str());
					carbonIsotopeSpecies.insert(pair<unsigned int, string>(carbonLabel, isotope->tagString));
				}
			}
		}
	}

	MatrixXf MM((int) vsamples.size(), (int) isotopes.size()); //rows=samples, cols=isotopes
	MM.setZero();

	for (int i = 0; i < isotopes.size(); i++) {
		if (!isotopes[i])
			continue;
		vector<float> values = isotopes[i]->getOrderedIntensityVector(vsamples,
				qtype); //sort isotopes by sample
		for (int j = 0; j < values.size(); j++)
			MM(j, i) = values[j];  //rows=samples, columns=isotopes
	}

	int numberofCarbons = 0;
        if (group->getCompound() && !group->getCompound()->formula().empty()) {
		map<string, int> composition = MassCalculator::getComposition(
                                group->getCompound()->formula());
		numberofCarbons = composition["C"];
	}
	isotopeC13Correct(MM, numberofCarbons, carbonIsotopeSpecies);
	normalizeIsotopicMatrix(MM);
	return MM;
}

MatrixXf MainWindow::getIsotopicMatrixIsoWidget(PeakGroup* group) {

	PeakGroup::QType qtype = getUserQuantType();
	//get visiable samples
	vector<mzSample*> vsamples = getVisibleSamples();
	sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
	map<unsigned int, string> carbonIsotopeSpecies;

	//get isotopic groups
	vector<PeakGroup*> isotopes;
	string delimIsotopic = "C13-label-";
	string delimParent = "C12 PARENT";
	for (int i = 0; i < group->childCount(); i++) {
		if (group->children[i].isIsotope()) {
			PeakGroup* isotope = &(group->children[i]);
			isotopes.push_back(isotope);
			if(isotope->tagString.find(delimIsotopic) != string::npos || isotope->tagString.find(delimParent) != string::npos) {
				if (isotope->tagString.find(delimParent) != string::npos) {
					carbonIsotopeSpecies.insert(pair<unsigned int, string>(0, isotope->tagString));
				} else if (isotope->tagString.find(delimIsotopic) != string::npos) {
					unsigned int carbonLabel = atoi(isotope->tagString.substr(delimIsotopic.size() - (isotope->tagString.size() - delimIsotopic.size() - 1)).c_str());
					carbonIsotopeSpecies.insert(pair<unsigned int, string>(carbonLabel, isotope->tagString));
				}
			}
		}
	}
	//std::sort(isotopes.begin(), isotopes.end(), PeakGroup::compC13);

	MatrixXf MM((int) vsamples.size(), (int) 2*isotopes.size() ); //rows=samples, cols=isotopes
	MatrixXf MMabundance((int) vsamples.size(), (int) isotopes.size());
	MMabundance.setZero();
	MM.setZero();

	for (int i = 0; i < isotopes.size(); i++) {
		if (!isotopes[i])
			continue;
		vector<float> values = isotopes[i]->getOrderedIntensityVector(vsamples,
				qtype); //sort isotopes by sample
		for (int j = 0; j < values.size(); j++) {
			MM(j, i) = values[j];  //rows=samples, columns=isotopes
			MMabundance(j, i) = values[j];
		}
	}

	int numberofCarbons = 0;
        if (group->getCompound() && !group->getCompound()->formula().empty()) {
		map<string, int> composition = MassCalculator::getComposition(
                                group->getCompound()->formula());
		numberofCarbons = composition["C"];
	}

	isotopeC13Correct(MMabundance, numberofCarbons, carbonIsotopeSpecies);
    if (mavenParameters && mavenParameters->isotopeC13Correction == true) {
		for (int i = 0, k = isotopes.size(); i < isotopes.size(); i++, k++) {
			if (!isotopes[i])
				continue;
			for (int j = 0; j < vsamples.size(); j++) {
				MM(j, k) = MMabundance(j,i);  //rows=samples, columns=isotopes
			}
		}
	}
	return MM;
}

void MainWindow::isotopeC13Correct(MatrixXf& MM, int numberofCarbons, map<unsigned int, string> carbonIsotopeSpecies) {
	if (numberofCarbons == 0)
		return;

	qDebug() << "IsotopePlot::isotopeC13Correct() " << MM.rows() << " "
			<< MM.cols() << " nCarbons=" << numberofCarbons << endl;
    if (mavenParameters && mavenParameters->isotopeC13Correction == false)
		return;

	for (int i = 0; i < MM.rows(); i++) {		//samples
		float sum = 0;
		vector<double> mv(MM.cols());
		//qDebug() << "Correction for " << i;

		//make a copy
		for (int j = 0; j < MM.cols(); j++) {
			mv[j] = MM(i, j);
			sum += MM(i, j);
		}

		//normalize to sum=1 and correct
		if (sum > 0) {
			for (int j = 0; j < mv.size(); j++) {
				mv[j] /= sum;
			} //normalize

			vector<double> cmv = mzUtils::naturalAbundanceCorrection(numberofCarbons, mv, carbonIsotopeSpecies);

			for (int j = 0; j < mv.size(); j++) {
				if (j < cmv.size()) {
					MM(i, j) = cmv[j];
				} else {
					MM(i, j) = mv[j];
				}
				//cerr << " Hello   " << mv[j] << "    " << cmv[j] << "   " << MM(i,j) << endl;
			}
		}
	}
	cerr << "IsotopePlot::IsotopePlot() done.." << endl;

}

void MainWindow::updateEicSmoothingWindow(int value) {
	getEicWidget()->recompute();
	getEicWidget()->replot();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
	//Merged with Maven776 - Kiran
    if(fileLoader->isRunning()) return;
    Q_FOREACH (QUrl url, event->mimeData()->urls() ) {
        if (fileLoader->isKnownFileType(url.toString())) {
            event->acceptProposedAction();
            return;
        }
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
		//Merged with Maven776 - Kiran

    if(fileLoader->isRunning()) return;
     Q_FOREACH (QUrl url, event->mimeData()->urls() ) {
        QString filename = url.toString();
        filename.replace("file:///","");
        filename.replace("file://","");
        fileLoader->addFileToQueue(filename);
    }
     qDebug() << "MainWindow::dropEvent() fileLoader->start() ";
    fileLoader->start();
 }

mzSample* MainWindow::getSampleByName(QString name) {

    //perfect matching
    for(int i=0; i < samples.size(); i++) {
        if (samples[i] == NULL) continue;
        if (samples[i]->sampleName == name.toStdString()) {
            return samples[i];
        }
    }

    //non perfect matching
    for(int i=0; i < samples.size(); i++) {
        if (samples[i] == NULL) continue;
        if (samples[i]->sampleName == mzUtils::cleanFilename(name.toStdString())) {
            return samples[i];
        }
    }
    return NULL;
}



void MainWindow::startEmbededHttpServer() {
	//Merged with Maven 776 - Kiran
#ifdef EMBEDHTTPSERVER
    QString address = settings->value("embeded_http_server_address").value<QString>();
    int port = settings->value("embeded_http_server_port").value<int>();
    embededhttpserver = new Pillow::HttpServer(QHostAddress(address), port);

    RemoteSpectraHandler* handler = new RemoteSpectraHandler(embededhttpserver);

    handler->setMainWindow(this);
    connect(embededhttpserver, SIGNAL(requestReady(Pillow::HttpConnection*)), handler,
                    SLOT(handleRequest(Pillow::HttpConnection*)));

#endif
}
