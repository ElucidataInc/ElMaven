#include "mainwindow.h"
#include <QStandardPaths>
#include "notificator.h"
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

//TODO: remove this fucntion. use another function to start
// crashReporter
void MainWindow::printvalue() {

    // TODO: hide all this ugliness somewhere. OS dependent paths are used in a lot of places and hence
    // this should be abstracted out somewhere

    QString crashReporterPath;
    #if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
        crashReporterPath = QCoreApplication::applicationDirPath() + QDir::separator() + "CrashReporter";
    #endif

    #if defined(Q_OS_MAC)
        QString binFolder = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + ".." \
            + QDir::separator() + ".." + QDir::separator();
        crashReporterPath = binFolder + "CrashReporter.app" + QDir::separator() + "Contents" + QDir::separator() + "MacOS" + QDir::separator() + "CrashReporter";
    #endif


    if (samples.size() > 0 ) {
        settings->setValue("closeEvent", 1);
        QFileInfo fi(QString::fromStdString(samples[0]->fileName));
        QString samplePath = fi.absolutePath() + QDir::separator();
        const QDateTime nowTime = QDateTime::currentDateTime();
        const QString timestamp = nowTime.toString(QLatin1String("yyyyMMdd-hhmmsszzz"));
        QString AutosavePath = samplePath + timestamp + ".mzroll";
        this->fileName = AutosavePath;
        this->doAutosave = true;
        this->saveMzRoll();
        unsigned int countCrashState = 0;
        settings->beginWriteArray("crashTables");
        Q_FOREACH( QString newFileName, this->SaveMzrollListvar) {
            settings->setArrayIndex(countCrashState++);
            settings->setValue("crashTable", newFileName);
        }
        settings->endArray();
        settings->sync();
    }

    saveSettingsToLog();

    //IMP: start the process in detached mode otherwise
    // crashreporter gets killed as soon as parent dies
    QProcess *pr = new QProcess;
    pr->setProgram(crashReporterPath);
    pr->setProcessChannelMode(QProcess::SeparateChannels);

		#if defined(Q_OS_WIN)
			pr->start();
		#else
			pr->startDetached(crashReporterPath);
		#endif

}

using namespace mzUtils;

 MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent) {
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

	
	QList<QString> dirs;
	dirs << dataDir << QApplication::applicationDirPath()
		 << QApplication::applicationDirPath() + "/../Resources/";

	//find location of DATA
	Q_FOREACH (QString d, dirs){
		QFile test(d+"/ADDUCTS.csv");
		if (test.exists()) {dataDir=d; settings->setValue("dataDir", dataDir); break;}
	}

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

	QString commonAdducts = dataDir + "/" + "ADDUCTS.csv";
	if (QFile::exists(commonAdducts))
		DB.loadFragments(commonAdducts.toStdString());


	clsf = new ClassifierNeuralNet();    //clsf = new ClassifierNaiveBayes();
		mavenParameters = new MavenParameters(QString(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QDir::separator() + "lastRun.xml").toStdString());
	_massCutoffWindow = new MassCutoff();



   /* double massCutoff=settings->value("compoundMassCutoffWindow").toDouble();
      string massCutoffType=settings->value("massCutoffType").toString().toStdString();
      _massCutoffWindow->setMassCutoffAndType(massCutoff,massCutoffType);
    */


    // always load model file present in the bin directory.
    QString clsfModelFilename;

    #if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
      clsfModelFilename = QDir::cleanPath(QApplication::applicationDirPath() + QDir::separator() + "default.model");
    #endif

    #if defined(Q_OS_MAC)
      clsfModelFilename = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator() + ".." \
              + QDir::separator() + "default.model";
    #endif



	if (QFile::exists(clsfModelFilename)) {
		settings->setValue("clsfModelFilename", clsfModelFilename);
		clsf->loadModel( clsfModelFilename.toStdString());
		mavenParameters->clsf = getClassifier();
	} else {
		settings->setValue("clsfModelFilename", QString(""));
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
	alignmentVizWidget = new AlignmentVizWidget(this);
	alignmentVizAllGroupsWidget = new AlignmentVizAllGroupsWidget(this);
	customPlot = new QCustomPlot(this);
	alignmentVizPlot = new QCustomPlot(this);
    sampleRtVizPlot = new QCustomPlot(this);
	alignmentVizAllGroupsPlot = new QCustomPlot(this);	
	pathwayWidget = new PathwayWidget(this);
	adductWidget = new AdductWidget(this);
	gettingstarted = new GettingStarted(this);
	isotopeWidget = new IsotopeWidget(this);
	isotopePlot = new IsotopePlot(this);


	massCalcWidget = new MassCalcWidget(this);
	covariantsPanel = new TreeDockWidget(this, "Covariants", 3);
	fragPanel = new TreeDockWidget(this, "Fragmentation", 5);
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
	alignmentVizDockWidget = createDockWidget("AlignmentVisualization", alignmentVizPlot);
	alignmentVizAllGroupsDockWidget = createDockWidget("AlignmentVisualizationForAllGroups", alignmentVizAllGroupsPlot);
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
	alignmentVizPlot->xAxis->setTicks(false);
	alignmentVizPlot->xAxis->setBasePen(QPen(Qt::white));
	alignmentVizPlot->xAxis->grid()->setVisible(false);	
	// prepare y axis:
	alignmentVizPlot->yAxis->setTicks(false);
	alignmentVizPlot->yAxis->setBasePen(QPen(Qt::white));
	alignmentVizPlot->yAxis->grid()->setVisible(true);

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
	adductWidget->setVisible(false);

	isotopeWidget->setVisible(false);
	massCalcWidget->setVisible(false);
	fragPanel->setVisible(false);
	bookmarkedPeaks->setVisible(false);
	pathwayDockWidget->setVisible(false);
	spectraDockWidget->setVisible(false);
	alignmentVizDockWidget->setVisible(false);
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
	
	//treemap->setVisible(false);
	//peaksPanel->setVisible(false);
	//treeMapDockWidget =  createDockWidget("TreeMap",treemap);

    //added while merging with Maven776 - Kiran
    //create toolbar for SRM dock widget
    srmDockWidget->setQQQToolBar();

	//
	//DIALOGS
	//
	peakDetectionDialog = new PeakDetectionDialog(this);
	peakDetectionDialog->setMainWindow(this);
	peakDetectionDialog->setSettings(settings);
	
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

	addDockWidget(Qt::LeftDockWidgetArea, ligandWidget, Qt::Vertical);
	addDockWidget(Qt::LeftDockWidgetArea, pathwayPanel, Qt::Vertical);
	addDockWidget(Qt::LeftDockWidgetArea, projectDockWidget, Qt::Vertical);

	ligandWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
	pathwayPanel->setAllowedAreas(Qt::LeftDockWidgetArea);
	projectDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);

	addDockWidget(Qt::BottomDockWidgetArea, spectraDockWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, alignmentVizDockWidget, Qt::Horizontal);
    addDockWidget(Qt::BottomDockWidgetArea, sampleRtWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, alignmentVizAllGroupsDockWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, isotopePlotDockWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, pathwayDockWidget, Qt::Horizontal);
	addDockWidget(Qt::BottomDockWidgetArea, adductWidget, Qt::Horizontal);
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
	tabifyDockWidget(spectraDockWidget, alignmentVizDockWidget);
    tabifyDockWidget(spectraDockWidget, sampleRtWidget);
	tabifyDockWidget(spectraDockWidget, alignmentVizAllGroupsDockWidget);
	tabifyDockWidget(spectraDockWidget, isotopePlotDockWidget);
	tabifyDockWidget(spectraDockWidget, pathwayDockWidget);
	tabifyDockWidget(spectraDockWidget, fragPanel);
	tabifyDockWidget(spectraDockWidget, covariantsPanel);
	tabifyDockWidget(spectraDockWidget, notesDockWidget);
	tabifyDockWidget(spectraDockWidget, galleryDockWidget);
	tabifyDockWidget(spectraDockWidget, logWidget);
	// tabifyDockWidget(rconsoleDockWidget, logWidget);
    tabifyDockWidget(peptideFragmentation,logWidget);

	connect(this, SIGNAL(saveSignal()), this, SLOT(autosaveMzRoll()));

    //added while merging with Maven776 - Kiran
    connect(fileLoader,SIGNAL(updateProgressBar(QString,int,int)), SLOT(setProgressBar(QString, int,int)));
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
	alignmentVizDockWidget->raise();
    sampleRtWidget->raise();
	alignmentVizAllGroupsDockWidget->raise();	

	createToolBars();
	// setIonsizationMode(0);
	currentIntensityName = "Max "+quantType->currentText();

	setIonizationModeLabel();
	setTotalCharge();
	connect(ionChargeBox, SIGNAL(valueChanged(int)), this, SLOT(setTotalCharge()));

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
				loadCompoundsFile(lfile);
		}
		QString databaseSet;
		if (settings->contains("lastCompoundDatabase")) {
			ligandWidget->setDatabase(
					settings->value("lastCompoundDatabase").toString());
			databaseSet = settings->value("lastCompoundDatabase").toString();
		} else {
			ligandWidget->setDatabase("KNOWNS");
			databaseSet = "KNOWNS";
		}

	}

	setAcceptDrops(true);

	showNormal();	//return from full screen on startup

	//remove close button from dockwidget
	QList<QDockWidget *> dockWidgets = this->findChildren<QDockWidget *>();
	for (int i = 0; i < dockWidgets.size(); i++) {
		dockWidgets[i]->setFeatures(
				dockWidgets[i]->features() ^ QDockWidget::DockWidgetClosable);
	}

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

}

MainWindow::~MainWindow()
{
	analytics->sessionEnd();
    delete mavenParameters;
}

void MainWindow::saveSettingsToLog() {

    QString buffer;
    QTextStream summary( &buffer, QIODevice::ReadWrite);	

    summary << "\n\n-------------------Maven Parameters-------------------"<< "\n"<< "\n";
//     summary << "runFunction =" << runFunction<< "\n";
    summary << "alignSamplesFlag="  <<  mavenParameters->alignSamplesFlag<< "\n";
    summary << "alignMaxItterations="  <<  mavenParameters->alignMaxItterations << "\n";
    summary << "alignPolynomialDegree="  <<  mavenParameters->alignPolynomialDegree << "\n";

    summary << "--------------------------------MASS SLICING"<< "\n";
    summary << "rtStepSize=" << mavenParameters->rtStepSize<< "\n";
    summary << "massCutoffMerge=" << mavenParameters->massCutoffMerge->getMassCutoff()<< "\n";
    summary << "limitGroupCount=" << mavenParameters->limitGroupCount<< "\n";

    summary << "minMz=" << mavenParameters->minMz << "\n";
    summary << "maxMz=" << mavenParameters->maxMz << "\n";

    summary << "minRt=" << mavenParameters->minRt << "\n";
    summary << "maxRt=" << mavenParameters->maxRt << "\n";

    summary << "minIntensity=" << mavenParameters->minIntensity << "\n";
    summary << "maxIntensity=" << mavenParameters->maxIntensity << "\n";

    summary << "minCharge=" << mavenParameters->minCharge << "\n";
    summary << "maxCharge=" << mavenParameters->maxCharge << "\n";
    summary << "------------------------------COMPOUND MATCHING"
            << "\n";

    summary << "ionizationMode=" << mavenParameters->ionizationMode << "\n";
    summary << "matchRtFlag=" << mavenParameters->matchRtFlag << "\n";
    summary << "compoundMassCutoffWindow=" << mavenParameters->compoundMassCutoffWindow->getMassCutoff()
            << "\n";
    summary << "compoundRTWindow=" << mavenParameters->compoundRTWindow << "\n";
    summary << "matchFragmentation=" << mavenParameters->matchFragmentation
            << "\n";
    summary << "fragmentMatchMassCutoffTolr=" << mavenParameters->fragmentMatchMassCutoffTolr->getMassCutoff()
            << "\n";

    summary << "------------------------------EIC CONSTRUCTION"
            << "\n";
    summary << "eic_smoothingWindow=" << mavenParameters->eic_smoothingWindow
            << "\n";
    summary << "eic_smoothingAlgorithm="
            << mavenParameters->eic_smoothingAlgorithm << "\n";
    summary << "baseline_smoothingWindow="
            << mavenParameters->baseline_smoothingWindow << "\n";
    summary << "baseline_dropTopX=" << mavenParameters->baseline_dropTopX
            << "\n";
    summary << "------------------------------PEAK GROUPING"
            << "\n";

    summary << "minSignalBaselineDifference=" << mavenParameters->minSignalBaselineDifference
            << "\n";
    summary << "------------------------------PEAK FILTERING"
            << "\n";

    summary << "grouping_maxRtWindow=" << mavenParameters->grouping_maxRtWindow
            << "\n";
    summary << "eicMaxGroups=" << mavenParameters->eicMaxGroups << "\n";
    summary << "------------------------------GROUP FILTERING"
            << "\n";

    summary << "minGoodPeakCount=" << mavenParameters->minGoodGroupCount
            << "\n";
    summary << "minSignalBlankRatio=" << mavenParameters->minSignalBlankRatio
            << "\n";
    summary << "minSignalBlankRatio=" << mavenParameters->minSignalBlankRatio
            << "\n";
    summary << "minNoNoiseObs=" << mavenParameters->minNoNoiseObs << "\n";
    summary << "minSignalBaseLineRatio="
            << mavenParameters->minSignalBaseLineRatio << "\n";
    summary << "minGroupIntensity=" << mavenParameters->minGroupIntensity
            << "\n";

    summary << "-----------------------------------OUTPUT"
            << "\n";
    summary << "outputdir="
            << QString::fromStdString(mavenParameters->outputdir) << "\n";
    summary << "writeCSVFlag=" << mavenParameters->writeCSVFlag << "\n";
    summary << "keepFoundGroups=" << mavenParameters->keepFoundGroups << "\n";
    summary << "showProgressFlag=" << mavenParameters->showProgressFlag << "\n";

    QString mavenParametersSummary = summary.readAll();


	QString qsettingsSummary;

	qsettingsSummary += "\n\n----------------------------------SETTINGS-----------------------------\n";


	Q_FOREACH(QString key, settings->allKeys()) {
		if (key == "bucket_name" || key == "access_key" || key == "secret_key") {
			continue;
		} else {
			qsettingsSummary += "\n" + key + "=" + settings->value(key).toString();
		}
	}

}

void MainWindow::showNotification(TableDockWidget* table) {
	QIcon icon = QIcon(":/images/notification.png");
	QString title("");
	QString message("View your fluxomics workflow on Polly!");
	
	if (table->groupCount() == 0 || table->labeledGroups == 0)
		return;
	
	Notificator* fluxomicsPrompt = Notificator::showMessage(icon, title, message, table);
	connect(fluxomicsPrompt, SIGNAL(promptClicked()), SLOT(showPollyElmavenInterfaceDialog()));
	connect(fluxomicsPrompt, SIGNAL(promptClicked()), pollyElmavenInterfaceDialog, SLOT(setFluxPage()));
	connect(fluxomicsPrompt, SIGNAL(promptClicked(TableDockWidget*)), pollyElmavenInterfaceDialog, SLOT(setActiveTable(TableDockWidget*)));
}

void MainWindow::createPeakTable(QString filenameNew) {	
	TableDockWidget * peaksTable = this->addPeaksTable("title");
	peaksTable->loadPeakTable(filenameNew);
	peaksTable->showAllGroups();
}

bool MainWindow::askAutosave() {

	bool doAutosave = false;
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "Autosave", "Do you want to enable autosave?",
								QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	if (reply == QMessageBox::Yes) {
		doAutosave = true;
	} else {
		doAutosave = false;
	}
	return doAutosave;
}

AutoSave::AutoSave(MainWindow* mw){

	_mainwindow = mw;
	_mainwindow->doAutosave = false;
	_mainwindow->askAutosaveMain = 0;

}

void MainWindow::saveMzRollList(QString MzrollFileName){
	 SaveMzrollListvar.insert(MzrollFileName);
 }

void AutoSave::saveMzRollWorker() {
	this->start();
}
void AutoSave::run() {
	_mainwindow->saveMzRoll();
}
void MainWindow::showAlignmetErrorDialog(QString errorMessage){
	QErrorMessage alignmentErrorDialog(this);
	alignmentErrorDialog.setWindowTitle("Alignment Error");
	alignmentErrorDialog.showMessage(errorMessage);
	alignmentErrorDialog.exec();
}
void MainWindow::autosaveMzRoll() {
	if (this->peaksMarked == 1 && this->askAutosaveMain == 0) {
		this->askAutosaveMain++;
		this->doAutosave = this->askAutosave();
		if (this->doAutosave) {
			QString dir = ".";
			if ( settings->contains("lastDir") ) {
				QString ldir = settings->value("lastDir").value<QString>();
				QDir test(ldir);
				if (test.exists()) dir = ldir;
			}

			this->fileName = QFileDialog::getSaveFileName( this,
					"Save Project (.mzroll)", dir, "mzRoll Project(*.mzroll)");
			if (this->fileName.isEmpty()) {
				this->doAutosave = false;
				return;
			}
			if(!this->fileName.endsWith(".mzroll",Qt::CaseInsensitive)) this->fileName = this->fileName + ".mzroll";
			autosave->saveMzRollWorker();
		}
	} else if (this->doAutosave) {
		autosave->saveMzRollWorker();
	}
}

void MainWindow::openAWSDialog()
{

	awsBucketCredentialsDialog = new AwsBucketCredentialsDialog(this);
	awsBucketCredentialsDialog->show();
	awsBucketCredentialsDialog->setMainWindow(this);
	awsBucketCredentialsDialog->setSettings(settings);

}



void MainWindow::saveMzRoll() {

    QSettings* settings = this->getSettings();
	if (this->peaksMarked > 5) {
			this->saveMzRollAllTables();
	} else if (this->allPeaksMarked) {
			this->saveMzRollAllTables();
	} else if (settings->value("closeEvent").toInt() == 1) {
		this->saveMzRollAllTables();
	} else if(this->doAutosave) {
		this->saveMzRollAllTables();
	}
}

void MainWindow::saveMzRollAllTables() {

    QSettings* settings = this->getSettings();

	if (fileName.isEmpty()) {
		fileName = this->projectDockWidget->lastSavedProject;
	}
	QList<QPointer<TableDockWidget> > peaksTableList =
		this->getPeakTableList();
	peaksTableList.append(0);

	TableDockWidget* peaksTable;

	int j = 1;
	SaveMzrollListvar.clear();
	Q_FOREACH(peaksTable, peaksTableList) {

		if ( !newFileName.isEmpty() && this->projectDockWidget->lastSavedProject == newFileName ) {
			savePeaksTable(peaksTable, fileName, QString::number(j));
		} else {
			savePeaksTable(peaksTable, fileName, QString::number(j));
		}
		j++;
	}

}

void MainWindow::savePeaksTable(TableDockWidget* peaksTable, QString fileName, QString tableName) {
	if(fileName.endsWith(".mzroll",Qt::CaseInsensitive)) {
		QRegExp rxr("_table-[0-9]+");
		fileName = fileName.replace(rxr, "");
		QRegExp rxr1("_bookmarkedPeaks");
		fileName = fileName.replace(rxr1, "");
		QFileInfo fi(fileName);

		if (peaksTable) {
			newFileName = fi.absolutePath() + QDir::separator() + fi.completeBaseName() + "_table-" + tableName + ".mzroll";
			saveMzRollList(newFileName);
			this->projectDockWidget->saveProject(newFileName, peaksTable);
		} else if (!this->bookmarkedPeaks->getGroups().isEmpty()) {
			newFileName = fi.absolutePath() + QDir::separator() + fi.completeBaseName() + "_bookmarkedPeaks" + ".mzroll";
			saveMzRollList(newFileName);
			this->projectDockWidget->saveProject(newFileName);
		} else {
			newFileName = fi.absolutePath() + QDir::separator() + fi.completeBaseName() + ".mzroll";
			saveMzRollList(newFileName);
			this->projectDockWidget->saveProject(newFileName);
		}
	}
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

void MainWindow::autoSaveSignal() {
	Q_EMIT(saveSignal());
}

void MainWindow::setUrl(Compound* c) {
	if (c == NULL)
		return;
	QString biocycURL = "http://biocyc.org/ECOLI/NEW-IMAGE?type=NIL&object";
	QString keggURL = "http://www.genome.jp/dbget-bin/www_bget?";
	QString pubChemURL =
			"http://www.ncbi.nlm.nih.gov/sites/entrez?db=pccompound&term=";

	QString url;
	if (c->db == "MetaCyc") {
		url = biocycURL + tr("=%1").arg(c->id.c_str());
	} else if (c->db == "KEGG") {
		url = keggURL + tr("%1").arg(c->id.c_str());
		//} else if ( c->id.c_str() != "") {
		//  url = keggURL+tr("%1").arg(c->id.c_str());
	} else {
		url = pubChemURL + tr("%1").arg(c->name.c_str());
	}
	QString link(c->name.c_str());
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

TableDockWidget* MainWindow::addPeaksTable(QString title) {
	TableDockWidget* panel = new PeakTableDockWidget(this);

	addDockWidget(Qt::BottomDockWidgetArea, panel, Qt::Horizontal);
	QToolButton* btnTable = addDockWidgetButton(sideBar, panel, QIcon(rsrcPath + "/featuredetect.png"), title);

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

// SpectralHitsDockWidget* MainWindow::addSpectralHitsTable(QString title) {
// 	QPointer<SpectralHitsDockWidget> panel = new SpectralHitsDockWidget(this,
// 			"Spectral Hits Table");
// 	addDockWidget(Qt::BottomDockWidgetArea, panel, Qt::Horizontal);
// 	//groupTables.push_back(panel);

// 	if (sideBar) {
// 		QToolButton *btnTable = new QToolButton(sideBar);
// 		btnTable->setIcon(QIcon(rsrcPath + "/spreadsheet.png"));
// 		btnTable->setChecked(panel->isVisible());
// 		btnTable->setCheckable(true);
// 		btnTable->setToolTip(title);
// 		connect(btnTable, SIGNAL(clicked(bool)), panel, SLOT(setVisible(bool)));
// 		connect(panel, SIGNAL(visibilityChanged(bool)), btnTable,
// 				SLOT(setChecked(bool)));
// 		sideBar->addWidget(btnTable);
// 	}
// 	return panel;
// }

void MainWindow::setUserMassCutoff(double x) {
	double cutoff=x;
	string type=massCutoffComboBox->currentText().toStdString();
	_massCutoffWindow->setMassCutoffAndType(cutoff,type);
	massCalcWidget->setMassCutoff(_massCutoffWindow);
	eicWidget->setMassCutoff(_massCutoffWindow);

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

	massCalcWidget->setCharge(mode);
	isotopeWidget->setCharge(mode);
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

void MainWindow::setTotalCharge() {

	mavenParameters->charge = ionChargeBox->value();
	totalCharge = mavenParameters->ionizationMode * ionChargeBox->value();

	ligandWidget->updateTable();

} 

vector<mzSample*> MainWindow::getVisibleSamples() {

	vector<mzSample*> vsamples;
	for (int i = 0; i < samples.size(); i++) {
		if (samples[i] && samples[i]->isSelected) {
			vsamples.push_back(samples[i]);
		}
	}
	return vsamples;
}
//TODOL - Sahil Removed this older function to add new while merging eicwidget.cpp
// void MainWindow::bookmarkPeakGroup() {
// 	//qDebug() << "MainWindow::bookmarkPeakGroup()";
// 	std::cerr << "REACHED bookmarkPeakGroup!!!!!!!!!!!!!!!!" << std::endl;
// 	if (eicWidget)
// 		bookmarkPeakGroup(eicWidget->getParameters()->getSelectedGroup());
// }

PeakGroup* MainWindow::bookmarkPeakGroup() {
    //qDebug() << "MainWindow::bookmarkPeakGroup()";
    if ( eicWidget && (eicWidget->getParameters()->getSelectedGroup() != NULL) ) 
	{
       return bookmarkPeakGroup(eicWidget->getParameters()->getSelectedGroup() );
    }
}

//TODOL - Sahil Removed this older function to add new while merging eicwidget.cpp
// void MainWindow::bookmarkPeakGroup(PeakGroup* group) {

// 	if (bookmarkedPeaks == NULL)
// 		return;

// 	if (bookmarkedPeaks->isVisible() == false) {
// 		bookmarkedPeaks->setVisible(true);
// 	}

// 	if (bookmarkedPeaks->hasPeakGroup(group) == false) {
// 		bookmarkedPeaks->addPeakGroup(group);
// 		bookmarkedPeaks->showAllGroups();
// 	}
// 	bookmarkedPeaks->updateTable();
// }


PeakGroup* MainWindow::bookmarkPeakGroup(PeakGroup* group) {

	if ( bookmarkedPeaks == NULL ) return NULL;
	//TODO: User feedback when group is rejected
	if (group->peakCount() == 0) return NULL;

    if ( bookmarkedPeaks->isVisible() == false ) {
        bookmarkedPeaks->setVisible(true);
    }

    PeakGroup* bookmarkedGroup=NULL;
    if ( bookmarkedPeaks->hasPeakGroup(group) == false) {


		float rtDiff = -1;

		if (group->compound != NULL && group->compound->expectedRt > 0)
		{
			rtDiff = abs(group->compound->expectedRt - (group->meanRt));
			group->expectedRtDiff = rtDiff;
		}

		double A = (double) mavenParameters->qualityWeight/10;
        double B = (double) mavenParameters->intensityWeight/10;
        double C = (double) mavenParameters->deltaRTWeight/10;

        if (mavenParameters->deltaRtCheckFlag && group->compound != NULL && group->compound->expectedRt > 0)
        {
            group->groupRank = pow(rtDiff, 2*C) * pow((1.1 - group->maxQuality), A)
                                                  * (1 /( pow(log(group->maxIntensity + 1), B)));
        }
        else
        {
            group->groupRank = pow((1.1 - group->maxQuality), A)
                                                  * (1 /(pow(log(group->maxIntensity + 1), B)));
        }

		bookmarkedGroup = bookmarkedPeaks->addPeakGroup(group);
        bookmarkedPeaks->showAllGroups();
		bookmarkedPeaks->updateTable();
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
	qDebug() << "setCompoundFocus:" << c->name.c_str() << " " << charge << " "
			<< c->expectedRt;

	float mz = c->mass;
	if (!c->formula.empty())
		mz = c->adjustedMass(charge);

	//if (pathwayWidget != NULL && pathwayWidget->isVisible() ) {
	//  pathwayWidget->clear();
	//    pathwayWidget->setCompound(c);
	//}
	
	if (massCalcWidget && massCalcWidget->isVisible()) {
		massCalcWidget->setMass(mz);
	}

	if (eicWidget->isVisible() && samples.size() > 0) {
		PeakGroup *selectedGroup;
		selectedGroup = eicWidget->setCompound(c);
		//mavenParameters->_group is closest group to expected Rt
		if (isotopeWidget && isotopeWidget->isVisible())
		{
			isotopeWidget->setCompound(c);
			isotopeWidget->setPeakGroupAndMore(selectedGroup);
		}
    }

	//TODO: Sahil-Kiran, Added while merging mainwindow
    if ( spectraDockWidget->isVisible()) {
			spectraWidget->overlayCompoundFragmentation(c);
	}

    if(fragPanel->isVisible()   )
        showFragmentationScans(mz);

    QString compoundName(c->name.c_str());
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
    analytics->hitEvent("BoxPlot","Clicked");
}

void MainWindow::analyticsAverageSpectra(){
    analytics->hitEvent("Average Spectra","Clicked");
}


void MainWindow::open() {


	QString dir = ".";

	if (settings->contains("lastDir")) {
		QString ldir = settings->value("lastDir").value<QString>();
		QDir test(ldir);
		if (test.exists())
			dir = ldir;
	}

	QStringList filelist =
			QFileDialog::getOpenFileNames(this,
					"Select projects, peaks, samples to open:", dir,
					tr(
							"All Known Formats(*.mzroll *.mzPeaks *.mzXML *.mzxml *.mzdata *.mzData *.mzData.xml *.cdf *.nc *.mzML);;")
							+ tr("mzXML Format(*.mzXML *.mzxml);;")
							+ tr("mzData Format(*.mzdata *.mzData *.mzData.xml);;")
							+ tr("mzML Format(*.mzml *.mzML);;")
							+ tr("NetCDF Format(*.cdf *.nc);;")
							+ tr("Thermo (*.raw);;") //TODO: Sahil-Kiran, Added while merging mainwindow
							+ tr("Maven Project File (*.mzroll);;")
							+ tr("Maven Peaks File (*.mzPeaks);;")
							+ tr("Peptide XML(*.pep.xml *.pepXML);;")
							+ tr("Peptide idpDB(*.idpDB);;")
							+ tr("All Files(*.*)"));
							
	if (filelist.size() == 0)
		return;

	analytics->hitEvent("ProjectDockWidget", "open", filelist.size());

	//Saving the file location into the Qsettings class so that it can be
	//used the next time the user opens
	QString absoluteFilePath(filelist[0]);
	QFileInfo fileInfo(absoluteFilePath);
	QDir tmp = fileInfo.absoluteDir();
	if (tmp.exists())
		settings->setValue("lastDir", tmp.absolutePath());

  //Changing the title of the main window after selecting the samples
	setWindowTitle(
			programName + "_" + STR(EL_MAVEN_VERSION) + " "
					+ fileInfo.fileName());
    //updated while merging with Maven776 - Kiran
    Q_FOREACH (QString filename, filelist)  fileLoader->addFileToQueue(filename);

	bool cancelUploading = false;
	cancelUploading = updateSamplePathinMzroll(filelist);
	if (!cancelUploading) {
		fileLoader->start();
	}
	else {
		fileLoader->removeAllFilefromQueue();
	}
}

bool MainWindow::updateSamplePathinMzroll(QStringList filelist) {

    QStringList projects;
	bool cancelUploading = false;

    Q_FOREACH(QString filename, filelist ) {
        QFileInfo fileInfo(filename);
        if (!fileInfo.exists()) continue;
        if (fileLoader->isProjectFileType(filename)) {
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

bool MainWindow::loadCompoundsFile(QString filename) {

	string dbfilename = filename.toStdString();
	string dbname = mzUtils::cleanFilename(dbfilename);
	int compoundCount = 0;
	bool reloading = false;

    //added while merging with Maven776 - Kiran
    if ( filename.endsWith("pepXML",Qt::CaseInsensitive)) {
       // compoundCount=fileLoader->loadPepXML(filename);
    } else if ( filename.endsWith("msp",Qt::CaseInsensitive) || filename.endsWith("sptxt",Qt::CaseInsensitive)) {
        compoundCount=fileLoader->loadNISTLibrary(filename);
    } else if ( filename.endsWith("massbank",Qt::CaseInsensitive)) { 
        compoundCount=fileLoader->loadMassBankLibrary(filename);
    } else {
        compoundCount = DB.loadCompoundCSVFile(dbfilename);
    }
	deque<Compound*> compoundsDB = DB.getCompoundsDB();

	for (int i=0; i < compoundsDB.size(); i++) {
        Compound* currentCompound = compoundsDB[i];
		if (currentCompound->db == dbname) {
			reloading = true;
			break;
		}
	}

	//check status in case the same file had been uploaded earlier
	//without modifications
	if ((compoundCount > 0 || reloading) && ligandWidget) {
		ligandWidget->setDatabaseNames();
		if (ligandWidget->isVisible())
			ligandWidget->setDatabase(QString(dbname.c_str()));

		settings->setValue("lastDatabaseFile", filename);
		setStatusText(tr("loadCompounds: done after loading %1 compounds").arg(QString::number(compoundCount)));
		return true;
	} else {
		setStatusText(tr("loadCompounds: not able to load %1 database").arg(filename));
		return false;
	}
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

void MainWindow::loadCompoundsFile() {
	QStringList filelist =
			QFileDialog::getOpenFileNames(this, "Select Compounds File To Load",
					".",
					"All Known Formats(*.csv *.tab *.tab.txt *.msp *.sptxt *.pepXML *.massbank);;Tab Delimited(*.tab);;Tab Delimited Text(*.tab.txt);;CSV File(*.csv);;NIST Library(*.msp);;SpectraST(*.sptxt);;pepXML(*.pepXML);;MassBank(*.massbank");

    if ( filelist.size() == 0 || filelist[0].isEmpty() ) return;
	if(!loadCompoundsFile(filelist[0])) {
		string dbfilename = filelist[0].toStdString();
		string dbname = mzUtils::cleanFilename(dbfilename);
		string notFoundColumns = "Following are the unknown column name(s) found: ";

        QMessageBox msgBox;
		msgBox.setText(tr("Trouble in loading compound database %1").arg(QString::fromStdString(dbname)));
        msgBox.setIcon(QMessageBox::Warning);
		if (DB.notFoundColumns.size() > 0) {
			for(std::vector<string>::iterator it = DB.notFoundColumns.begin(); it != DB.notFoundColumns.end(); ++it) {
    			notFoundColumns += "\n" + *it;
			}
			msgBox.setDetailedText(QString::fromStdString(notFoundColumns));
			msgBox.setWindowFlags(msgBox.windowFlags() & ~Qt::WindowCloseButtonHint);
		}

		int ret = msgBox.exec();
	} else {
		if (DB.notFoundColumns.size() > 0) {
			string notFoundColumns = "Following are the unknown column name(s) found: ";
            QMessageBox msgBox;
			msgBox.setText(tr("Found some unknown column name(s)"));
			for(std::vector<string>::iterator it = DB.notFoundColumns.begin(); it != DB.notFoundColumns.end(); ++it) {
    			notFoundColumns += "\n" + *it;
			}
			msgBox.setDetailedText(QString::fromStdString(notFoundColumns));
			msgBox.setWindowFlags(msgBox.windowFlags() & ~Qt::WindowCloseButtonHint);
            msgBox.setIcon(QMessageBox::Information);
			int ret = msgBox.exec();
		}
		if (DB.invalidRows.size() > 0) {
			string invalidRowsString = "The following compounds had insufficient information for peak detection, and were not loaded:";
            QMessageBox msgBox;
			msgBox.setText(tr("Invalid compounds found"));
			for(auto compoundID: DB.invalidRows) {
    			invalidRowsString += "\n - " + compoundID;
            }
            msgBox.setDetailedText(QString::fromStdString(invalidRowsString));
			msgBox.setWindowFlags(Qt::CustomizeWindowHint);
            msgBox.setIcon(QMessageBox::Information);
			int ret = msgBox.exec();
		}
	}
}

// open function for set csv
void MainWindow::loadMetaInformation() {
	QStringList filelist =
			QFileDialog::getOpenFileNames(this, "Select Set Information File To Load",
					".",
					"All Known Formats(*.csv *.tab *.tab.txt);;Tab Delimited(*.tab);;Tab Delimited Text(*.tab.txt);;CSV File(*.csv)");

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
    static const string allHeadersarr[] = {"sample", "set", "injection order"};
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
        if ( header.count("set")&& header["set"]<N)	set = fields[ header["set"] ];
		else{
			set = "";
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
	workerThread->setRunFunction(funcName);
	//threads.push_back(workerThread);
	return workerThread;
}

/*
 void MainWindow::terminateTheads() {

 for(int i=0; i < threads.size(); i++ ) {
 if (threads[i] != NULL ) {
 if (  threads[i]->isRunning())  {
 QMessageBox::StandardButton reply;
 reply = QMessageBox::critical(this, tr(  "QMessageBox::critical()"), "Do you wish to stop currently running backround job?", QMessageBox::Yes | QMessageBox::No);
 if (reply == QMessageBox::Yes) threads[i]->terminate();
 }
 if (! threads[i]->isRunning()) { delete(threads[i]); threads[i]=NULL; }
 }
 }
 }


 */

void MainWindow::exportPDF() {
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

void MainWindow::exportSVG() {

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
			
    if (!settings->contains("clsfModelFilename") || settings->value("clsfModelFilename").toString().length() <=0) {
        #if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
          settings->setValue("clsfModelFilename",  QApplication::applicationDirPath() + "/" + "default.model");
        #endif
        #if defined(Q_OS_MAC)
          QString binPath = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator() + ".." \
                  + QDir::separator() + "default.model";
          settings->setValue("clsfModelFilename", binPath);
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


	if (!settings->contains("AbthresholdBarplot"))
		settings->setValue("AbthresholdBarplot", 1);


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
	settings->setValue("size", size()*0.7);
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

void MainWindow::closeEvent(QCloseEvent *event) {
	settings->setValue("closeEvent", 1);
	this->saveMzRoll();
	writeSettings();
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

	QAction* openAct = new QAction(QIcon(":/images/open.png"),
			tr("&Load Samples|Projects|Peaks"), this);
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

	QAction* saveProjectFile = new QAction(tr("Save Project As"), this);
	saveProjectFile->setShortcut(tr("Ctrl+S"));
	connect(saveProjectFile, SIGNAL(triggered()), projectDockWidget,
			SLOT(saveProject()));
	fileMenu->addAction(saveProjectFile);

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
    connect(aj,SIGNAL(toggled(bool)),fragPanel,SLOT(setVisible(bool)));

    QAction* al = widgetsMenu->addAction("Peptide Fragmentation");
    al->setCheckable(true);  al->setChecked(false);
    connect(al,SIGNAL(toggled(bool)),peptideFragmentation,SLOT(setVisible(bool)));

	QSignalMapper* signalMapper = new QSignalMapper (this) ;
	QAction* doc = helpMenu->addAction("Documentation");
	connect(doc,SIGNAL(triggered(bool)), signalMapper, SLOT(map()));

	QAction* tutorial = helpMenu->addAction("Video Tutorials");
	connect(tutorial,SIGNAL(triggered()), signalMapper, SLOT(map()));

	QAction* faq = helpMenu->addAction("FAQs");
	connect(faq, SIGNAL(triggered()), signalMapper, SLOT(map()));

	QAction* start = helpMenu->addAction("Getting Started");
	connect(start,SIGNAL(triggered(bool)), gettingstarted, SLOT(show()));

	signalMapper->setMapping(doc, 1);
	signalMapper->setMapping(tutorial, 2);
	signalMapper->setMapping(faq, 3);

	connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(openURL(int)));

	menuBar()->show();
}

void MainWindow::openURL(int choice)
{
	map<int,QUrl> URL{
		{1, QUrl("https://github.com/ElucidataInc/ElMaven/wiki")},
		{2, QUrl("https://www.youtube.com/channel/UCZYVM0I1zqRgkGTdIlQZ9Yw/videos")},
		{3, QUrl("https://elucidatainc.github.io/ElMaven/faq/")}
	};
	QDesktopServices::openUrl(URL[choice]);
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
	connect(btn, SIGNAL(clicked(bool)), this, SLOT(sendAnalytics()));
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

void MainWindow::sendAnalytics() {

	QString btnName = QObject::sender()->objectName();
	analytics->hitScreenView(btnName);

}

void MainWindow::createToolBars() {

	QToolBar *toolBar = new QToolBar(this);
	toolBar->setObjectName("mainToolBar");
	toolBar->setMovable(false);

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

	QToolButton *btnSpectraMatching = new QToolButton(toolBar);
	btnSpectraMatching->setText("Match");
	btnSpectraMatching->setIcon(QIcon(rsrcPath + "/spectra_search.png"));
	btnSpectraMatching->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnSpectraMatching->setStyleSheet("QToolTip {color: #000000; background-color: #fbfbd5; border: 1px solid black; padding: 1px;}");
	btnSpectraMatching->setToolTip(
			tr("Matching Spectra for Fragmentation Patterns"));

	QToolButton *btnSettings = new QToolButton(toolBar);
	btnSettings->setText("Options");
	btnSettings->setIcon(QIcon(rsrcPath + "/settings.png"));
	btnSettings->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnSettings->setStyleSheet("QToolTip {color: #000000; background-color: #fbfbd5; border: 1px solid black; padding: 1px;}");
	btnSettings->setToolTip(tr("1. Instrumentation: Ionization settings\n2. File Import: Scan filter settings\n3. Peak Detection: EIC smoothing and baseline settings\n4. Peak Filtering: Parent and Isotopic peak filtering settings\n5. Isotope Detection: Isotopic label filters\n6. EIC (XIC): EIC type selection\n7. Peak Grouping: Peak Grouping Score calculation\n8. Group Rank: Group Rank calculation - Group Rank decides which groups are selected for a given m/z"));

	connect(btnOpen, SIGNAL(clicked()), SLOT(open()));
	connect(btnAlign, SIGNAL(clicked()), alignmentDialog, SLOT(show()));
	// connect(btnAlign, SIGNAL(clicked()), alignmentDialog, SLOT(intialSetup()));
	//connect(btnDbSearch, SIGNAL(clicked()), SLOT(showPeakdetectionDialog())); //TODO: Sahil-Kiran, Removed while merging mainwindow
	connect(btnFeatureDetect, SIGNAL(clicked()), SLOT(showPeakdetectionDialog()));
	connect(btnPollyBridge, SIGNAL(clicked()), SLOT(showPollyElmavenInterfaceDialog()));
	connect(btnSettings, SIGNAL(clicked()), SLOT(showsettingsForm()));
	connect(btnSpectraMatching, SIGNAL(clicked()), SLOT(showspectraMatchingForm()));

	toolBar->addWidget(btnOpen);
	toolBar->addWidget(btnAlign);
	//toolBar->addWidget(btnDbSearch); //TODO: Sahil-Kiran, Removed while merging mainwindow
	toolBar->addWidget(btnFeatureDetect);
	toolBar->addWidget(btnSpectraMatching);
	toolBar->addWidget(btnSettings);
	toolBar->addWidget(btnPollyBridge);

	QWidget *hBox = new QWidget(toolBar);
	(void) toolBar->addWidget(hBox);

	QHBoxLayout *layout = new QHBoxLayout(hBox);
	layout->setSpacing(0);
	layout->addWidget(new QWidget(hBox), 15); // spacer

	//ppmValue
	massCutoffWindowBox = new QDoubleSpinBox(hBox);
	massCutoffWindowBox->setRange(0.00, 100000.0);
	massCutoffWindowBox->setDecimals(6);
	massCutoffWindowBox->setValue(settings->value("massCutoffWindowBox").toDouble());
	massCutoffWindowBox->setToolTip("Mass Cut-off Unit");
	// connect(massCutoffWindowBox, SIGNAL(valueChanged(double)), this,
	// 		SLOT(setUserPPM(double)));
	// connect(massCutoffWindowBox, SIGNAL(valueChanged(double)), eicWidget,
	// 		SLOT(setPPM(double)));
	connect(massCutoffWindowBox, SIGNAL(valueChanged(double)), this,
	SLOT(setUserMassCutoff(double)));

	massCutoffComboBox=  new QComboBox(hBox);
	massCutoffComboBox->addItem("ppm");
	massCutoffComboBox->addItem("mDa");
	massCutoffWindowBox->setSingleStep(0.5);
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
	ionChargeBox->setValue(settings->value("ionChargeBox").toInt());

	quantType = new QComboBox(hBox);
	quantType->addItem("AreaTop");
	quantType->addItem("Area");
    quantType->addItem("Height");
    quantType->addItem("AreaNotCorrected"); //TODO: Sahil-Kiran, Added while merging mainwindow
    quantType->addItem("AreaTopNotCorrected");
    quantType->addItem("Retention Time");
	quantType->addItem("Quality");
	quantType->setToolTip("Peak Quntitation Type");
	connect(quantType, SIGNAL(activated(int)), eicWidget, SLOT(replot()));
	connect(quantType, SIGNAL(currentIndexChanged(int)), SLOT(refreshIntensities()));

	settings->beginGroup("searchHistory");
	QStringList keys = settings->childKeys();
	Q_FOREACH(QString key, keys)suggestPopup->addToHistory(key, settings->value(key).toInt());
	settings->endGroup();

	layout->addWidget(ionizationModeLabel, 0);
	layout->addWidget(new QLabel("Charge", hBox), 0);
	layout->addWidget(ionChargeBox, 0);
	layout->addWidget(quantType, 0);
	layout->addWidget(new QLabel("[m/z]", hBox), 0);
	layout->addWidget(searchText, 0);
	layout->addWidget(new QLabel("+/-", 0, 0));
	layout->addWidget(massCutoffWindowBox, 0);
	layout->addWidget(massCutoffComboBox,0);
	
	sideBar = new QToolBar(this);
	sideBar->setObjectName("sideBar");


    QToolButton* btnSamples = addDockWidgetButton(sideBar,projectDockWidget,QIcon(rsrcPath + "/samples.png"), "Show Samples Widget (F2)");
    QToolButton* btnLigands = addDockWidgetButton(sideBar,ligandWidget,QIcon(rsrcPath + "/molecule.png"), "Show Compound Widget (F3)");
    QToolButton* btnSpectra = addDockWidgetButton(sideBar,spectraDockWidget,QIcon(rsrcPath + "/spectra.png"), "Show Spectra Widget (F4)");

    QToolButton* btnAlignment = new QToolButton(sideBar);
    btnAlignment->setIcon(QIcon(rsrcPath + "/alignmentButton.png"));
    btnAlignment->setText("Alignment Visualizations");
    QMenu* alignmentMenu = new QMenu("Alignment Visualizations Menu");

    btnAlignment->setMenu(alignmentMenu);
    btnAlignment->setPopupMode(QToolButton::InstantPopup);


    QAction* perGroupAlignment = alignmentMenu->addAction(QIcon(rsrcPath + "/alignmentViz.png"), "Per group Alignment Visualization");
    QAction* allGroupAlignment = alignmentMenu->addAction(QIcon(rsrcPath + "/alignmentVizAllGroups.png"), "Alignment Visualization of all groups");
    QAction* sampleRtDeviation = alignmentMenu->addAction(QIcon(rsrcPath + "/sampleRtViz.png"), "Sample Retention time Deviation Visualization");


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
    if(alignmentVizDockWidget->isVisible()) {
        alignmentVizDockWidget->hide();
        return;
    }

    alignmentVizDockWidget->show();
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
}

void MainWindow::setMassCutoffType(QString massCutoffType){
	double cutoff=massCutoffWindowBox->value();
	string type=massCutoffType.toStdString();
	_massCutoffWindow->setMassCutoffAndType(cutoff,type);
	massCalcWidget->setMassCutoff(_massCutoffWindow);
	eicWidget->setMassCutoff(_massCutoffWindow);
}

void MainWindow::refreshIntensities() {
	QList<QPointer<TableDockWidget> > peaksTableList = getPeakTableList();
	for(int i=0; i<peaksTableList.size(); i++) {
		peaksTableList[i]->showAllGroups();
	}
	bookmarkedPeaks->showAllGroups();
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

	if (isotopeWidget && isotopeWidget->isVisible() && group->compound != NULL) {
		isotopeWidget->setPeakGroupAndMore(group);
	}

	//TODO: Sahil-Kiran, Added while merging mainwindow
    if ( group->compound != NULL) {
        QString compoundName(group->compound->name.c_str());
        if (! setPeptideSequence(compoundName)) {
            setUrl(group->compound);
            if ( spectraDockWidget->isVisible()  ) {
                //spectraWidget->showConsensusSpectra(group);
                //spectraWidget->overlayCompoundFragmentation(group->compound);
            }
        }
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

void MainWindow::Align() {
	if (sampleCount() < 2)
		return;

	aligned = true;

	BackgroundPeakUpdate* workerThread;

	if(alignmentDialog->alignAlgo->currentIndex() == 2){
		workerThread = newWorkerThread("alignWithObiWarp");
		workerThread->setMavenParameters(mavenParameters);
		workerThread->start();
		connect(workerThread, SIGNAL(finished()), eicWidget, SLOT(replotForced()));
		connect(workerThread, SIGNAL(finished()), alignmentDialog, SLOT(close()));
		return;
	}
	
	if (alignmentDialog->peakDetectionAlgo->currentText() == "Compound Database Search") {
		workerThread = newWorkerThread("alignUsingDatabase");
		mavenParameters->setCompounds(DB.getCopoundsSubset(alignmentDialog->selectDatabaseComboBox->currentText().toStdString()));

	} else {
		workerThread = newWorkerThread("processMassSlices");
	}

	connect(workerThread, SIGNAL(finished()), eicWidget, SLOT(replotForced()));
	connect(workerThread, SIGNAL(finished()), alignmentDialog, SLOT(close()));

	mavenParameters->minGoodGroupCount =
			alignmentDialog->minGoodPeakCount->value();
	mavenParameters->limitGroupCount =
			alignmentDialog->limitGroupCount->value();
	mavenParameters->minGroupIntensity =
			alignmentDialog->minGroupIntensity->value();
	mavenParameters->minIntensity =
			alignmentDialog->minIntensity->value();
	mavenParameters->maxIntensity =
			alignmentDialog->maxIntensity->value();

	//TODO: Sahil Re-verify this two parameters. Values are same
	// mavenParameters->eic_smoothingWindow =
	// 		alignmentDialog->groupingWindow->value(); //TODO: Sahil-Kiran, Added while merging mainwindow
	mavenParameters->rtStepSize =
			alignmentDialog->groupingWindow->value(); //TODO: Sahil-Kiran, Added while merging mainwindow

	mavenParameters->minSignalBaseLineRatio = alignmentDialog->minSN->value();
	mavenParameters->minNoNoiseObs = alignmentDialog->minPeakWidth->value();


    mavenParameters->minSignalBlankRatio = 0; //TODO: Sahil-Kiran, Added while merging mainwindow
    mavenParameters->alignMaxItterations = alignmentDialog->maxItterations->value(); //TODO: Sahil-Kiran, Added while merging mainwindow
    mavenParameters->alignPolynomialDegree = alignmentDialog->polynomialDegree->value(); //TODO: Sahil-Kiran, Added while merging mainwindow

    mavenParameters->checkConvergance=false; //TODO: Sahil-Kiran, Added while merging mainwindow
	mavenParameters->alignSamplesFlag = true;
	mavenParameters->keepFoundGroups = true;
    mavenParameters->eicMaxGroups = peakDetectionDialog->eicMaxGroups->value();

	mavenParameters->samples = getSamples();
	mavenParameters->stop = false;
	workerThread->setMavenParameters(mavenParameters);
	workerThread->setPeakDetector(new PeakDetector(mavenParameters));
	alignmentDialog->setWorkerThread(workerThread);

    //connect new connections
    connect(workerThread, SIGNAL(newPeakGroup(PeakGroup*)), bookmarkedPeaks, SLOT(addPeakGroup(PeakGroup*))); //TODO: Sahil-Kiran, Added while merging mainwindow
    connect(workerThread, SIGNAL(finished()), bookmarkedPeaks, SLOT(showAllGroups())); //TODO: Sahil-Kiran, Added while merging mainwindow
    // connect(workerThread, SIGNAL(terminated()), bookmarkedPeaks, SLOT(showAllGroups())); //TODO: Sahil-Kiran, Added while merging mainwindow

	workerThread->start();
}

void MainWindow::plotAlignmentVizAllGroupGraph(QList<PeakGroup> allgroups) {
	alignmentVizAllGroupsWidget->plotGraph(allgroups);
}


void MainWindow::showAlignmentWidget() {

	alignmentVizAllGroupsDockWidget->setVisible(true);
	alignmentVizAllGroupsDockWidget->raise();

}

void MainWindow::UndoAlignment() {
	if(alignmentDialog->alignAlgo->currentIndex() == 2){
		for (int i = 0; i < samples.size(); i++) {
			for(int j = 0; j < samples[i]->scans.size(); ++j)
				if(samples[i]->scans[j]->originalRt >= 0)
					samples[i]->scans[j]->rt = samples[i]->scans[j]->originalRt;
		}

		eicWidget->replotForced();
		alignmentDialog->close();
		return;
	}

	aligned = false;
	for (int i = 0; i < samples.size(); i++) {
		if (samples[i])
			samples[i]->restoreOriginalRetentionTimes();
	}
	getEicWidget()->replotForced();

	mavenParameters->alignButton = 0;

	QList<PeakGroup> listGroups;
	for (unsigned int i = 0; i<mavenParameters->undoAlignmentGroups.size(); i++) {
			listGroups.append(mavenParameters->undoAlignmentGroups.at(i));
	}

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

	if (massCalcWidget->isVisible()) {
		massCalcWidget->setMass(_peak->peakMz);
		massCalcWidget->setCharge(ionizationMode);
	}

	if (isotopeWidget->isVisible()) {
		isotopeWidget->setIonizationMode(ionizationMode);
		isotopeWidget->setCharge(ionizationMode);
	}

	if (fragPanel->isVisible()) {
		showFragmentationScans(_peak->peakMz);
	}
	//TODO: Sahil-Kiran, Added while merging mainwindow
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
		compoundName = "\"" + QString(group->compound->name.c_str()) + "\"";
		expectedRt = group->compound->expectedRt;
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

void MainWindow::showFragmentationScans(float pmz) {

	if (!fragPanel || fragPanel->isVisible() == false)
		return;
	MassCutoff *massCutoff = getUserMassCutoff();

	if (samples.size() <= 0)
		return;
	fragPanel->clearTree();
	for (unsigned int i = 0; i < samples.size(); i++) {
		for (unsigned int j = 0; j < samples[i]->scans.size(); j++) {
			if (samples[i]->scans[j]->mslevel > 1
					&& massCutoffDist(samples[i]->scans[j]->precursorMz, pmz,massCutoff) < massCutoff->getMassCutoff()) {
				fragPanel->addScanItem(samples[i]->scans[j]);
			}
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

	toolBar->addSeparator();
	
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
			Q_FOREACH( Compound*c, compunds){ links[i].note += " |" + c->name; break;}
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
	if (adductWidget->isVisible())
		adductWidget->setPeak(peak);
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

int MainWindow::versionCheck() {

//define a MACRO for converting DEFINES to strings.. jfc!
#define xstr(s) str(s)
#define str(s) #s

	QString hostname = "http://genomics-pubs.princeton.edu";
	QString path = "/mzroll/vercheck.php?";
	QString os = "os=" + QString(xstr(PLATFORM));
	//QString ver = "ver=" + QString::number(EL_MAVEN_VERSION);
	QString ver = "ver=620";
	QString query = os + "&" + ver;

	logWidget->append("Latest version check: " + hostname + path + query);

	QDownloader* downloader = new QDownloader(this);
	downloader->getPage(hostname + path + query);
	connect(downloader, SIGNAL(downloadResult(QString)), logWidget,
			SLOT(append(QString)));

	return 0;
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
	if (group->compound && !group->compound->formula.empty()) {
		map<string, int> composition = MassCalculator::getComposition(
				group->compound->formula);
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
	if (group->compound && !group->compound->formula.empty()) {
		map<string, int> composition = MassCalculator::getComposition(
				group->compound->formula);
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
        if (samples[i]->sampleName == name.toStdString()) {
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

//TODO: sahil-Kiran, removed while merging mainwindow
// bool MainWindow::isSampleFileType(QString filename) {
// 	if (filename.endsWith("mzXML", Qt::CaseInsensitive)) {
// 		return 1;
// 	} else if (filename.endsWith("cdf", Qt::CaseInsensitive)
// 			|| filename.endsWith("nc", Qt::CaseInsensitive)) {
// 		return 1;
// 	} else if (filename.endsWith("mzCSV", Qt::CaseInsensitive)) {
// 		return 1;
// 	} else if (filename.contains("mzData", Qt::CaseInsensitive)) {
// 		return 1;
// 	} else if (filename.contains("mzML", Qt::CaseInsensitive)) {
// 		return 1;
// 	}
// 	return 0;
// }

//TODO: sahil-Kiran, removed while merging mainwindow
// bool MainWindow::isProjectFileType(QString filename) {
// 	if (filename.endsWith("mzroll", Qt::CaseInsensitive)) {
// 		return 1;
// 	}
// 	return 0;
// }
