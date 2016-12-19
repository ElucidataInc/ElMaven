#include "mainwindow.h"
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

using namespace mzUtils;

MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent) {

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

#ifdef Q_WS_MAC
	QDir dir(QApplication::applicationDirPath());
	dir.cdUp();
	dir.cd("plugins");
	QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
	QStringList list = QApplication::libraryPaths();
	qDebug() << "Library Path=" << list;
#endif

	readSettings();

	QString dataDir = ".";

	
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
	if (!QFile::exists(methodsFolder))
		methodsFolder = dataDir + "/" + "methods";

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
	mavenParameters = new MavenParameters();
	QString clsfModelFilename = settings->value("clsfModelFilename").value<QString>();

	if (QFile::exists(clsfModelFilename)) {
		settings->setValue("clsfModelFilename", clsfModelFilename);
		clsf->loadModel( clsfModelFilename.toStdString());
		mavenParameters->clsf = getClassifier();
	} else {
		settings->setValue("clsfModelFilename", QString(""));
		clsf->loadModel("");
		mavenParameters->clsf = getClassifier();
	}



	//QString storageLocation =   QDesktopServices::storageLocation(QDesktopServices::DataLocation);

    //added while merging with Maven776 - Kiran
	//fileLoader

	autosave = new AutoSave(this);
	autosave->setMainWindow(this);

    fileLoader = new mzFileIO(this);
    fileLoader->setMainWindow(this);

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
	btnBugs->setToolTip(tr("Bug!"));
	connect(btnBugs, SIGNAL(clicked()), SLOT(reportBugs()));
	statusBar()->addPermanentWidget(btnBugs, 0);

	setWindowIcon(QIcon(":/images/icon.png"));

	//dock widgets
	setDockOptions(
			QMainWindow::AllowNestedDocks | QMainWindow::VerticalTabs
					| QMainWindow::AnimatedDocks);

	//set main dock widget
	eicWidget = new EicWidget(this);
	setCentralWidget(eicWidgetController());
	spectraWidget = new SpectraWidget(this);
	customPlot = new QCustomPlot(this);
	pathwayWidget = new PathwayWidget(this);
	adductWidget = new AdductWidget(this);
	isotopeWidget = new IsotopeWidget(this);


	massCalcWidget = new MassCalcWidget(this);
	covariantsPanel = new TreeDockWidget(this, "Covariants", 3);
	fragPanel = new TreeDockWidget(this, "Fragmentation", 5);
	pathwayPanel = new TreeDockWidget(this, "Pathways", 1);
	srmDockWidget = new TreeDockWidget(this, "SRM List", 1);
	ligandWidget = new LigandWidget(this);
	 heatmap = new HeatMap(this);
	galleryWidget = new GalleryWidget(this);
	bookmarkedPeaks = new TableDockWidget(this, "Bookmarked Groups", 0);
	bookmarkedPeaks->bookmarkPeaksTAble = true;


	//treemap	 = 	  new TreeMap(this);
	//peaksPanel	= new TreeDockWidget(this,"Group Information", 1);
	spectraDockWidget = createDockWidget("Spectra", spectraWidget);
	isotopePlotsDockWidget = createDockWidget("IsotopePlots", customPlot);
	pathwayDockWidget = createDockWidget("PathwayViewer", pathwayWidget);
	heatMapDockWidget = createDockWidget("HeatMap", heatmap);
	galleryDockWidget = createDockWidget("Gallery", galleryWidget);
	scatterDockWidget = new ScatterPlot(this);
	notesDockWidget = new NotesWidget(this);
	projectDockWidget = new ProjectDockWidget(this);
	logWidget = new LogWidget(this, std::cout);
	rconsoleDockWidget = new RconsoleWidget(this);
	spectralHitsDockWidget = new SpectralHitsDockWidget(this, "Spectral Hits");
    peptideFragmentation = new PeptideFragmentationWidget(this);

	setIsotopicPlotStyling();

	ligandWidget->setVisible(false);
	pathwayPanel->setVisible(false);
	covariantsPanel->setVisible(false);
	adductWidget->setVisible(false);
	isotopeWidget->setVisible(false);
	massCalcWidget->setVisible(false);
	fragPanel->setVisible(false);
	bookmarkedPeaks->setVisible(false);
	pathwayDockWidget->setVisible(false);
	spectraDockWidget->setVisible(false);
	isotopePlotsDockWidget->show();
	scatterDockWidget->setVisible(false);
	notesDockWidget->setVisible(false);
	heatMapDockWidget->setVisible(false);
	galleryDockWidget->setVisible(false);
	projectDockWidget->setVisible(false);
	logWidget->setVisible(false);
	rconsoleDockWidget->setVisible(false);
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

	//alignment dialog
	alignmentDialog = new AlignmentDialog(this);
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
	addDockWidget(Qt::BottomDockWidgetArea, isotopePlotsDockWidget, Qt::Horizontal);
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
	addDockWidget(Qt::BottomDockWidgetArea, rconsoleDockWidget, Qt::Horizontal);
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
	tabifyDockWidget(spectraDockWidget, isotopePlotsDockWidget);
	tabifyDockWidget(spectraDockWidget, pathwayDockWidget);
	tabifyDockWidget(spectraDockWidget, fragPanel);
	tabifyDockWidget(spectraDockWidget, covariantsPanel);
	tabifyDockWidget(spectraDockWidget, notesDockWidget);
	tabifyDockWidget(spectraDockWidget, galleryDockWidget);
	tabifyDockWidget(spectraDockWidget, logWidget);
	tabifyDockWidget(rconsoleDockWidget, logWidget);
    tabifyDockWidget(peptideFragmentation,logWidget);

	connect(this, SIGNAL(saveSignal()), autosave, SLOT(saveMzRoll()));

    //added while merging with Maven776 - Kiran
    connect(fileLoader,SIGNAL(updateProgressBar(QString,int,int)), SLOT(setProgressBar(QString, int,int)));
    connect(fileLoader,SIGNAL(sampleLoaded()),projectDockWidget, SLOT(updateSampleList()));

    connect(fileLoader,SIGNAL(spectraLoaded()),spectralHitsDockWidget, SLOT(showAllHits()));
    connect(fileLoader,SIGNAL(spectraLoaded()),spectralHitsDockWidget, SLOT(show()));
    connect(fileLoader,SIGNAL(spectraLoaded()),spectralHitsDockWidget, SLOT(raise()));


    connect(fileLoader,SIGNAL(projectLoaded()),projectDockWidget, SLOT(updateSampleList()));
    connect(fileLoader,SIGNAL(projectLoaded()),bookmarkedPeaks, SLOT(showAllGroups()));
    connect(fileLoader,SIGNAL(projectLoaded()), SLOT(showSRMList()));

    connect(spectralHitsDockWidget,SIGNAL(updateProgressBar(QString,int,int)), SLOT(setProgressBar(QString, int,int)));
    connect(eicWidget,SIGNAL(scanChanged(Scan*)),spectraWidget,SLOT(setScan(Scan*)));


    setContextMenuPolicy(Qt::NoContextMenu);
    pathwayPanel->setInfo(DB.pathwayDB);

	if (settings->contains("windowState")) {
		restoreState(settings->value("windowState").toByteArray());
	}

	if (settings->contains("geometry")) {
		restoreGeometry(settings->value("geometry").toByteArray());
	}


    scatterDockWidget->hide();
    spectralHitsDockWidget->hide();
    peptideFragmentation->hide();
    fragPanel->hide();
    projectDockWidget->raise();
    spectraDockWidget->raise();

	setIonizationMode(0);
	if (settings->contains("ionizationMode")) {
		setIonizationMode(settings->value("ionizationMode").toInt());
	}

  // This been set here why is this here; beacuse of this
  // in the show function of peak detector its been made to set to this
  // value
	setUserPPM(5);
	if (settings->contains("ppmWindowBox")) {
		setUserPPM(settings->value("ppmWindowBox").toDouble());
	}

	QRectF view = settings->value("mzslice").value<QRectF>();
	if (view.width() > 0 && view.height() > 0) {
		eicWidget->setMzSlice(
				mzSlice(view.x(), view.y(), view.width(), view.height()));
	} else {
		eicWidget->setMzSlice(mzSlice(0, 0, 0, 100));
	}

	createMenus();
	createToolBars();
	if (ligandWidget)
		loadMethodsFolder(methodsFolder);
	if (pathwayWidget)
		loadPathwaysFolder(pathwaysFolder);

	if (ligandWidget) {
		if (settings->contains("lastDatabaseFile")) {
			QString lfile =
					settings->value("lastDatabaseFile").value<QString>();
			QFile testf(lfile);
			qDebug() << "Loading last database" << lfile;
			if (testf.exists())
				loadCompoundsFile(lfile);
		}

		if (settings->contains("lastCompoundDatabase")) {
			ligandWidget->setDatabase(
					settings->value("lastCompoundDatabase").toString());
		} else {
			ligandWidget->setDatabase("KNOWNS");
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

		setUrl("http://genomics-pubs.princeton.edu/mzroll/index.php?show=bugs",
				"Woops.. did the program crash last time? Would you like to report a bug?");
	}

	logWidget->append("Initiaalization complete..\n");
	//versionCheck(); //TODO: Sahil-Kiran, Removed while merging mainwindow

	settings->setValue("closeEvent", 0);

	peakDetectionDialog->setMavenParameters(settings);
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


AutoSave::AutoSave(QWidget*){
}


void AutoSave::setMainWindow(MainWindow* mw) {
    _mainwindow=mw;
}


void AutoSave::saveMzRoll(){

    QSettings* settings = _mainwindow->getSettings();

	if (_mainwindow->peaksMarked == 1){
		doAutosave = _mainwindow->askAutosave();
		if (doAutosave) saveMzRollAllTables();
	}

	if (_mainwindow->peaksMarked % 10 == 0 && doAutosave){
			saveMzRollAllTables();
	}

	if (_mainwindow->allPeaksMarked && doAutosave) {
			saveMzRollAllTables();		
	}

	if (settings->value("closeEvent").toInt() == 1 && doAutosave) {
		saveMzRollAllTables();
	}
}

void AutoSave::saveMzRollAllTables() {

    QSettings* settings = _mainwindow->getSettings();

    QString dir = ".";
    if ( settings->contains("lastDir") ) {
        QString ldir = settings->value("lastDir").value<QString>();
        QDir test(ldir);
        if (test.exists()) dir = ldir;
    }

	if (fileName.isEmpty()) {
		fileName = _mainwindow->projectDockWidget->lastSavedProject;
	}
	QList<QPointer<TableDockWidget> > peaksTableList =
		_mainwindow->getPeakTableList();
	peaksTableList.append(0);

	TableDockWidget* peaksTable;

	int j = 1;
	Q_FOREACH(peaksTable, peaksTableList) {

		if ( !newFileName.isEmpty() && _mainwindow->projectDockWidget->lastSavedProject == newFileName ) {
			savePeaksTable(peaksTable, fileName, QString::number(j));
		} else {
			fileName = QFileDialog::getSaveFileName( _mainwindow,
					"Save Project (.mzroll)", dir, "mzRoll Project(*.mzroll)");
			if (fileName.isEmpty()) {
				doAutosave = false;
				break;
			}
			if(!fileName.endsWith(".mzroll",Qt::CaseInsensitive)) fileName = fileName + ".mzroll";

			savePeaksTable(peaksTable, fileName, QString::number(j));
		}
		j++;
	}

}

void AutoSave::savePeaksTable(TableDockWidget* peaksTable, QString fileName, QString tableName) {
	
	if (peaksTable) {
		if(fileName.endsWith(".mzroll",Qt::CaseInsensitive)) {
			QFileInfo fi(fileName);
			newFileName = fi.absolutePath() + QDir::separator() + fi.completeBaseName() + "_" + tableName + ".mzroll";
		}
		_mainwindow->projectDockWidget->saveProject(newFileName, peaksTable);
	} else if (!_mainwindow->bookmarkedPeaks->getGroups().isEmpty()) {
		if(fileName.endsWith(".mzroll",Qt::CaseInsensitive)) {
			QFileInfo fi(fileName);
			newFileName = fi.absolutePath() + QDir::separator() + fi.completeBaseName() + "_bookmarkedPeaks" + ".mzroll";
		}
		_mainwindow->projectDockWidget->saveProject(newFileName);
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

QDockWidget* MainWindow::createDockWidgetIsotopes(QString title, QWidget* w) {
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
	// prepare y axis:
	customPlot->yAxis->grid()->setVisible(false);
	customPlot->yAxis->setTickLabels( false );
	customPlot->yAxis->setTicks( false );
	customPlot->yAxis->setBasePen(QPen(Qt::white));
	customPlot->yAxis->setRange(0, 1);
	
}

void MainWindow::reportBugs() {
	QUrl link("https://github.com/ElucidataInc/ElMaven/issues");
	QDesktopServices::openUrl(link);

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
	//TableDockWidget* panel	 = new TableDockWidget(this,"Bookmarked Groups",0);
	QPointer<TableDockWidget> panel = new TableDockWidget(this,
			"Bookmarked Groups", 0);
	addDockWidget(Qt::BottomDockWidgetArea, panel, Qt::Horizontal);
    groupTables.push_back(panel);
    panel->setObjectName(tr("PeakTable: %1").arg(groupTables.size())); //TODO: Sahil-Kiran, Added while merging mainwindow

	if (sideBar) {
        QAction *btnTable = new QAction(sideBar);
        groupTablesButtons[panel]=btnTable;
		btnTable->setIcon(QIcon(rsrcPath + "/featuredetect.png"));
		btnTable->setChecked(panel->isVisible());
		btnTable->setCheckable(true);
		btnTable->setToolTip(title);
		connect(btnTable, SIGNAL(toggled(bool)), panel, SLOT(setVisible(bool)));
		connect(panel, SIGNAL(visibilityChanged(bool)), btnTable,
				SLOT(setChecked(bool)));
		sideBar->addAction(btnTable);
	}

	return panel;
}

void MainWindow::removePeaksTable(TableDockWidget* panel) {
	//Merged with Maven776 - Kiran
    if (groupTablesButtons.contains(panel))
        sideBar->removeAction(groupTablesButtons[panel]);
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

void MainWindow::setUserPPM(double x) {
	_ppmWindow = x;
}

void MainWindow::setIonizationMode(int x) {
	_ionizationMode = x;
	massCalcWidget->setCharge(_ionizationMode);
	isotopeWidget->setCharge(_ionizationMode);

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
    if ( eicWidget ) {
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

    if ( bookmarkedPeaks->isVisible() == false ) {
        bookmarkedPeaks->setVisible(true);
    }

    PeakGroup* bookmarkedGroup=NULL;
    if ( bookmarkedPeaks->hasPeakGroup(group) == false) {
        bookmarkedGroup = bookmarkedPeaks->addPeakGroup(group);
        bookmarkedPeaks->showAllGroups();
		bookmarkedPeaks->updateTable();
    }
    return bookmarkedGroup;
}

void MainWindow::setFormulaFocus(QString formula) {
	int charge = 0;
	if (getIonizationMode())
		charge = getIonizationMode(); //user specified ionization mode

	// MassCalculator mcalc;
	double parentMass = MassCalculator::computeMass(formula.toStdString(), charge);
	if (eicWidget->isVisible())
		eicWidget->setMzSlice(parentMass);
	isotopeWidget->setFormula(formula);
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

	int charge = 0;
	if (samples.size() > 0 && samples[0]->getPolarity() > 0)
		charge = 1;
	if (getIonizationMode())
		charge = getIonizationMode(); //user specified ionization mode
	qDebug() << "setCompoundFocus:" << c->name.c_str() << " " << charge << " "
			<< c->expectedRt;

	float mz = c->mass;
	if (!c->formula.empty() && charge)
		mz = c->ajustedMass(charge);

	//if (pathwayWidget != NULL && pathwayWidget->isVisible() ) {
	//  pathwayWidget->clear();
	//    pathwayWidget->setCompound(c);
	//}
	
	if (isotopeWidget && isotopeWidget->isVisible())
		isotopeWidget->setCompound(c);
	if (massCalcWidget && massCalcWidget->isVisible()) {
		massCalcWidget->setMass(mz);
	}

	if (eicWidget->isVisible() && samples.size() > 0) {
		eicWidget->setCompound(c);
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
	QString value = searchText->text();
	float mz = value.toDouble(&isDouble);
	if (isDouble) {
		if (eicWidget->isVisible())
			eicWidget->setMzSlice(mz);
		if (massCalcWidget->isVisible())
			massCalcWidget->setMass(mz);
		if (fragPanel->isVisible())
			showFragmentationScans(mz);
	}
	suggestPopup->addToHistory(QString::number(mz, 'f', 5));
}

void MainWindow::setMzValue(float mz) {
	searchText->setText(QString::number(mz, 'f', 8));
	if (eicWidget->isVisible())
		eicWidget->setMzSlice(mz);
	if (massCalcWidget->isVisible())
		massCalcWidget->setMass(mz);
	if (fragPanel->isVisible())
		showFragmentationScans(mz);
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
							+ tr(
									"mzData Format(*.mzdata *.mzData *.mzData.xml);;")
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

  //Saving the file location into the Qsettings class so that it can be
  //used yhe next time the user opens
	QString absoluteFilePath(filelist[0]);
	QFileInfo fileInfo(absoluteFilePath);
	QDir tmp = fileInfo.absoluteDir();
	if (tmp.exists())
		settings->setValue("lastDir", tmp.absolutePath());

  //Changing the title of the main window aftyer selecting the samples
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

void MainWindow::loadCompoundsFile(QString filename) {

	string dbfilename = filename.toStdString();
	string dbname = mzUtils::cleanFilename(dbfilename);
	int compoundCount = 0;

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

	if (compoundCount > 0 && ligandWidget) {
		ligandWidget->setDatabaseNames();
		if (ligandWidget->isVisible())
			ligandWidget->setDatabase(QString(dbname.c_str()));

		settings->setValue("lastDatabaseFile", filename);
		setStatusText(
			tr("loadCompounds: done after loading %1 compounds").arg(
					QString::number(compoundCount)));
	} else {
		setStatusText(tr("loadCompounds: not able to load %1 database").arg(filename));
	}
}

void MainWindow::loadCompoundsFile() {
	QStringList filelist =
			QFileDialog::getOpenFileNames(this, "Select Compounds File To Load",
					".",
					"All Known Formats(*.csv *.tab *.tab.txt *.msp *.sptxt *.pepXML *.massbank);;Tab Delimited(*.tab);;Tab Delimited Text(*.tab.txt);;CSV File(*.csv);;NIST Library(*.msp);;SpectraST(*.sptxt);;pepXML(*.pepXML);;MassBank(*.massbank");

    if ( filelist.size() == 0 || filelist[0].isEmpty() ) return;
    loadCompoundsFile(filelist[0]);
}

void MainWindow::loadMethodsFolder(QString& methodsFolder) {
	cerr << "LOADING METHODS FROM:" << methodsFolder.toStdString() << endl;
	QDir dir(methodsFolder);
	if (dir.exists()) {
		dir.setFilter(QDir::Files);
		QFileInfoList list = dir.entryInfoList();
		for (int i = 0; i < list.size(); ++i) {
			QFileInfo fileInfo = list.at(i);
			//std::cerr << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10).arg(fileInfo.fileName())) << endl;
			loadCompoundsFile(fileInfo.absoluteFilePath());
		}
	}
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
			SLOT(setProgressBar(QString, int,int)));
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

	QPoint pos = settings->value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings->value("size", QSize(400, 400)).toSize();

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
			
	if (!settings->contains("clsfModelFilename") || settings->value("clsfModelFilename").toString().length() <=0)
    	settings->setValue("clsfModelFilename",  QApplication::applicationDirPath() + "/" + "default.model");

    // EIC Processing: Baseline and calculation
    if (!settings->contains("eic_smoothingAlgorithm"))
        settings->setValue("eic_smoothingAlgorithm", 0);

    if (!settings->contains("eic_smoothingWindow"))
		settings->setValue("eic_smoothingWindow", 5);

    if (!settings->contains("grouping_maxRtWindow"))
        settings->setValue("grouping_maxRtWindow", 0.5);

    // BaseLine Calculation
    if (!settings->contains("baseline_smoothingWindow"))
        settings->setValue("baseline_smoothingWindow", 5);

    if (!settings->contains("baseline_dropTopX"))
        settings->setValue("baseline_dropTopX", 80);

    // Peak Scoring and Fitering
    if (!settings->contains("minGoodGroupCount"))
        settings->setValue("minGoodGroupCount", 1);

    if (!settings->contains("minNoNoiseObs"))
        settings->setValue("minNoNoiseObs", 3);

    if (!settings->contains("minSignalBaseLineRatio"))
        settings->setValue("minSignalBaseLineRatio", 2);

    if (!settings->contains("minSignalBlankRatio"))
        settings->setValue("minSignalBlankRatio", 2);

    if (!settings->contains("minGroupIntensity"))
        settings->setValue("minGroupIntensity", 5000);

    // Compound DB Search
    if (!settings->contains("matchRtFlag"))
        settings->setValue("matchRtFlag", 0);

    if (!settings->contains("compoundPPMWindow"))
        settings->setValue("compoundPPMWindow", 20);

    if (!settings->contains("compoundRTWindow"))
        settings->setValue("compoundRTWindow", 2);

    if (!settings->contains("eicMaxGroups"))
        settings->setValue("eicMaxGroups", 10);

    //Setting to enable or disable compund or Auto Detection 
    if (!settings->contains("dbOptions"))
        settings->setValue("dbOptions", 2);
    
    if (!settings->contains("featureOptions"))
        settings->setValue("featureOptions", 0);

    // Automated Peak Detection
    if (!settings->contains("ppmMerge"))
        settings->setValue("ppmMerge", 20);

    if (!settings->contains("rtStepSize"))
        settings->setValue("rtStepSize", 10);

    if(!settings->contains("minRT"))
        settings->setValue("minRT", 0);

    if(!settings->contains("maxRT"))
        settings->setValue("maxRT", 0);

    if (!settings->contains("minMz")) settings->setValue("minMz", 0);

    if (!settings->contains("maxMz")) settings->setValue("maxMz", 0);

    if (!settings->contains("minIntensity"))
        settings->setValue("minIntensity", 0);

    if (!settings->contains("maxIntensity"))
        settings->setValue("maxIntensity", 0);

    if (!settings->contains("minCharge")) settings->setValue("minCharge", 0);

    if(!settings->contains("maxCharge")) settings->setValue("maxCharge", 0);

    //Isotope Detection in peakDetection Dialogue
    if (!settings->contains("pullIsotopesFlag"))
        settings->setValue("pullIsotopesFlag", 0);
        
    // if (!settings->contains("checkBox"))
    //     settings->setValue("checkBox", 0);

    // if (!settings->contains("checkBox_2"))
    //     settings->setValue("checkBox_2", 0);

    // if (!settings->contains("checkBox_3"))
    //     settings->setValue("checkBox_3", 0);

    // if (!settings->contains("checkBox_4"))
    //     settings->setValue("checkBox_4", 0);

    //Pull Isotopes in options
    if (!settings->contains("isotopeC13Correction"))
        settings->setValue("isotopeC13Correction", 2);

	if (!settings->contains("maxNaturalAbundanceErr"))
		settings->setValue("maxNaturalAbundanceErr", 100);

	if (!settings->contains("maxIsotopeScanDiff"))
		settings->setValue("maxIsotopeScanDiff", 10);

	if (!settings->contains("minIsotopicCorrelation"))
		settings->setValue("minIsotopicCorrelation", 0.1);

	if (!settings->contains("C13Labeled_Barplot"))
		settings->setValue("C13Labeled_Barplot", 2);
	
	if (!settings->contains("N15Labeled_Barplot"))
		settings->setValue("N15Labeled_Barplot", 2);
    
	if (!settings->contains("S34Labeled_Barplot"))
		settings->setValue("S34Labeled_Barplot", 2);
    
	if (!settings->contains("D2Labeled_Barplot"))
		settings->setValue("D2Labeled_Barplot", 2);

	if (!settings->contains("C13Labeled_BPE"))
		settings->setValue("C13Labeled_BPE", 2);

	if (!settings->contains("N15Labeled_BPE"))
		settings->setValue("N15Labeled_BPE", 2);

	if (!settings->contains("S34Labeled_BPE"))
		settings->setValue("S34Labeled_BPE", 2);

	if (!settings->contains("D2Labeled_BPE"))
		settings->setValue("D2Labeled_BPE", 2);

	if (!settings->contains("C13Labeled_IsoWidget"))
		settings->setValue("C13Labeled_IsoWidget", 2);
	
	if (!settings->contains("N15Labeled_IsoWidget"))
		settings->setValue("N15Labeled_IsoWidget", 2);

	if (!settings->contains("S34Labeled_IsoWidget"))
		settings->setValue("S34Labeled_IsoWidget", 2);

	if (!settings->contains("D2Labeled_IsoWidget"))
		settings->setValue("D2Labeled_IsoWidget", 2);

    //Main window right hand top
    if (!settings->contains("ppmWindowBox"))
        settings->setValue("ppmWindowBox", 5);

    if (!settings->contains("mzslice"))
        settings->setValue("mzslice", QRectF(100.0, 100.01, 0, 30));

    //Options tab 
    if (!settings->contains("ionizationMode"))
        settings->setValue("ionizationMode", -1);


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

	resize(size);
	move(pos);
}

void MainWindow::writeSettings() {
	settings->setValue("pos", pos());
	settings->setValue("size", size());
	settings->setValue("ppmWindowBox", ppmWindowBox->value());
	settings->setValue("geometry", saveGeometry());
	settings->setValue("windowState", saveState());
	settings->setValue("ionizationMode", getIonizationMode());

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
	autosave->saveMzRoll();
	writeSettings();
	event->accept();

}

/**
 * MainWindow::createMenus This functin creates the menu that is on top of
 * the window. All the functionalities that are here are there in other
 * places on the window
 */
void MainWindow::createMenus() {
	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	QMenu* widgetsMenu = menuBar()->addMenu(tr("&Widgets"));

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

	QAction* saveProjectFile = new QAction(tr("Save Project"), this);
	saveProjectFile->setShortcut(tr("Ctrl+S"));
	connect(saveProjectFile, SIGNAL(triggered()), projectDockWidget,
			SLOT(saveProject()));
	fileMenu->addAction(saveProjectFile);

	QAction* settingsAct = new QAction(tr("Options"), this);
	settingsAct->setToolTip(tr("Set program options"));
	connect(settingsAct, SIGNAL(triggered()), settingsForm, SLOT(show()));
	fileMenu->addAction(settingsAct);

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

    QAction* al = widgetsMenu->addAction("Peptide Fragmenation");
    al->setCheckable(true);  al->setChecked(false);
    connect(al,SIGNAL(toggled(bool)),peptideFragmentation,SLOT(setVisible(bool)));

	menuBar()->show();
}

QToolButton* MainWindow::addDockWidgetButton(QToolBar* bar,
		QDockWidget* dockwidget, QIcon icon, QString description) {
	QToolButton *btn = new QToolButton(bar);
	btn->setCheckable(true);
	btn->setIcon(icon);
	btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
	btn->setToolTip(description);
	connect(btn, SIGNAL(clicked(bool)), dockwidget, SLOT(setVisible(bool)));
	connect(btn, SIGNAL(clicked(bool)), dockwidget, SLOT(raise()));
	btn->setChecked(dockwidget->isVisible());
	connect(dockwidget, SIGNAL(visibilityChanged(bool)), btn,
			SLOT(setChecked(bool)));
	dockwidget->setWindowIcon(icon);
	return btn;
}

void MainWindow::createToolBars() {

	QToolBar *toolBar = new QToolBar(this);
	toolBar->setObjectName("mainToolBar");
	toolBar->setMovable(false);

	QToolButton *btnOpen = new QToolButton(toolBar);
	btnOpen->setText("Open");
	btnOpen->setIcon(QIcon(rsrcPath + "/fileopen.png"));
	btnOpen->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnOpen->setToolTip(tr("Read in Mass Spec. Files"));

	QToolButton *btnAlign = new QToolButton(toolBar);
	btnAlign->setText("Align");
	btnAlign->setIcon(QIcon(rsrcPath + "/textcenter.png"));
	btnAlign->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnAlign->setToolTip(tr("Align Samples"));

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
	btnFeatureDetect->setToolTip(tr("Feature Detection"));

	QToolButton *btnSpectraMatching = new QToolButton(toolBar);
	btnSpectraMatching->setText("Match");
	btnSpectraMatching->setIcon(QIcon(rsrcPath + "/spectra_search.png"));
	btnSpectraMatching->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnSpectraMatching->setToolTip(
			tr("Seach Spectra for Fragmentation Patterns"));

	QToolButton *btnSettings = new QToolButton(toolBar);
	btnSettings->setText("Options");
	btnSettings->setIcon(QIcon(rsrcPath + "/settings.png"));
	btnSettings->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnSettings->setToolTip(tr("Change Global Options"));

	connect(btnOpen, SIGNAL(clicked()), SLOT(open()));
	connect(btnAlign, SIGNAL(clicked()), alignmentDialog, SLOT(show()));
	//connect(btnDbSearch, SIGNAL(clicked()), SLOT(showPeakdetectionDialog())); //TODO: Sahil-Kiran, Removed while merging mainwindow
	connect(btnFeatureDetect, SIGNAL(clicked()), SLOT(showPeakdetectionDialog()));
	connect(btnSettings, SIGNAL(clicked()), settingsForm, SLOT(show()));
	connect(btnSpectraMatching, SIGNAL(clicked()), spectraMatchingForm,
			SLOT(show()));

	toolBar->addWidget(btnOpen);
	toolBar->addWidget(btnAlign);
	//toolBar->addWidget(btnDbSearch); //TODO: Sahil-Kiran, Removed while merging mainwindow
	toolBar->addWidget(btnFeatureDetect);
	toolBar->addWidget(btnSpectraMatching);
	toolBar->addWidget(btnSettings);

	QWidget *hBox = new QWidget(toolBar);
	(void) toolBar->addWidget(hBox);

	QHBoxLayout *layout = new QHBoxLayout(hBox);
	layout->setSpacing(0);
	layout->addWidget(new QWidget(hBox), 15); // spacer

	//ppmValue
	ppmWindowBox = new QDoubleSpinBox(hBox);
	ppmWindowBox->setRange(0.00, 100000.0);
	ppmWindowBox->setValue(settings->value("ppmWindowBox").toDouble());
	ppmWindowBox->setSingleStep(0.5);	//ppm step
	ppmWindowBox->setToolTip("PPM (parts per million) Window");
	connect(ppmWindowBox, SIGNAL(valueChanged(double)), this,
			SLOT(setUserPPM(double)));
	connect(ppmWindowBox, SIGNAL(valueChanged(double)), eicWidget,
			SLOT(setPPM(double)));

    searchText = new QLineEdit(hBox);
    searchText->setMinimumWidth(200);
    searchText->setPlaceholderText("MW / Compound");   
    searchText->setToolTip("<b>Text Search</b> <br> Compound Names: <b>ATP</b>,<br> Patterns: <b>[45]-phosphate</b> <br>Formulas: <b> C6H10* </b>");
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

	quantType = new QComboBox(hBox);
	quantType->addItem("AreaTop");
	quantType->addItem("Area");
    quantType->addItem("Height");
    quantType->addItem("AreaNotCorrected"); //TODO: Sahil-Kiran, Added while merging mainwindow
	quantType->addItem("Retention Time");
	quantType->addItem("Quality");
	quantType->setToolTip("Peak Quntitation Type");
	connect(quantType, SIGNAL(activated(int)), eicWidget, SLOT(replot()));

	settings->beginGroup("searchHistory");
	QStringList keys = settings->childKeys();
	Q_FOREACH(QString key, keys)suggestPopup->addToHistory(key, settings->value(key).toInt());
	settings->endGroup();

	layout->addWidget(quantType, 0);
	layout->addWidget(new QLabel("[m/z]", hBox), 0);
	layout->addWidget(searchText, 0);
	layout->addWidget(new QLabel("+/-", 0, 0));
	layout->addWidget(ppmWindowBox, 0);

	sideBar = new QToolBar(this);
	sideBar->setObjectName("sideBar");


    QToolButton* btnSamples = addDockWidgetButton(sideBar,projectDockWidget,QIcon(rsrcPath + "/samples.png"), "Show Samples Widget (F2)");
    QToolButton* btnLigands = addDockWidgetButton(sideBar,ligandWidget,QIcon(rsrcPath + "/molecule.png"), "Show Compound Widget (F3)");
    QToolButton* btnSpectra = addDockWidgetButton(sideBar,spectraDockWidget,QIcon(rsrcPath + "/spectra.png"), "Show Spectra Widget (F4)");
    QToolButton* btnIsotopes = addDockWidgetButton(sideBar,isotopeWidget,QIcon(rsrcPath + "/isotope.png"), "Show Isotopes Widget (F5)");
    QToolButton* btnFindCompound = addDockWidgetButton(sideBar,massCalcWidget,QIcon(rsrcPath + "/findcompound.png"), "Show Match Compound Widget (F6)");
    QToolButton* btnCovariants = addDockWidgetButton(sideBar,covariantsPanel,QIcon(rsrcPath + "/covariants.png"), "Find Covariants Widget (F7)");
    QToolButton* btnPathways = addDockWidgetButton(sideBar,pathwayDockWidget,QIcon(rsrcPath + "/pathway.png"), "Show Pathway Widget (F8)");
    QToolButton* btnNotes = addDockWidgetButton(sideBar,notesDockWidget,QIcon(rsrcPath + "/note.png"), "Show Notes Widget (F9)");
    QToolButton* btnBookmarks = addDockWidgetButton(sideBar,bookmarkedPeaks,QIcon(rsrcPath + "/showbookmarks.png"), "Show Bookmarks (F10)");
    QToolButton* btnGallery = addDockWidgetButton(sideBar,galleryDockWidget,QIcon(rsrcPath + "/gallery.png"), "Show Gallery Widget");
    QToolButton* btnScatter = addDockWidgetButton(sideBar,scatterDockWidget,QIcon(rsrcPath + "/scatterplot.png"), "Show Scatter Plot Widget");
    QToolButton* btnSRM = addDockWidgetButton(sideBar,srmDockWidget,QIcon(rsrcPath + "/qqq.png"), "Show SRM List (F12)");
    QToolButton* btnRconsole = addDockWidgetButton(sideBar,rconsoleDockWidget,QIcon(rsrcPath + "/R.png"), "Show R Console");



	//btnSamples->setShortcut(Qt::Key_F2);
	btnLigands->setShortcut(Qt::Key_F3);
	btnSpectra->setShortcut(Qt::Key_F4);
	btnIsotopes->setShortcut(Qt::Key_F5);
	btnFindCompound->setShortcut(Qt::Key_F6);
	btnCovariants->setShortcut(Qt::Key_F7);
	btnPathways->setShortcut(Qt::Key_F8);
	btnNotes->setShortcut(Qt::Key_F9);
	btnBookmarks->setShortcut(Qt::Key_F10);
	btnSRM->setShortcut(Qt::Key_F12);

	connect(pathwayDockWidget, SIGNAL(visibilityChanged(bool)), pathwayPanel,
			SLOT(setVisible(bool)));
	connect(btnSRM, SIGNAL(clicked(bool)), SLOT(showSRMList()));

	sideBar->setOrientation(Qt::Vertical);
	sideBar->setMovable(false);

	sideBar->addWidget(btnSamples);
	sideBar->addWidget(btnLigands);
	sideBar->addWidget(btnSpectra);
	sideBar->addWidget(btnIsotopes);
	sideBar->addWidget(btnFindCompound);
	sideBar->addWidget(btnCovariants);
	sideBar->addWidget(btnPathways);
	sideBar->addWidget(btnNotes);
	sideBar->addWidget(btnSRM);
	sideBar->addWidget(btnGallery);
	sideBar->addWidget(btnScatter);
	sideBar->addWidget(btnRconsole);
	sideBar->addSeparator();
	sideBar->addWidget(btnBookmarks);
	// sideBar->addWidget(btnHeatmap);

	addToolBar(Qt::TopToolBarArea, toolBar);
	addToolBar(Qt::RightToolBarArea, sideBar);
}

void MainWindow::historyLast() {
	if (history.size() == 0)
		return;
	eicWidget->setMzSlice(history.last());
}

void MainWindow::historyNext() {
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
		if (sample->getPolarity())
			setIonizationMode(sample->getPolarity());
		return true;
	} else {
		delete (sample);
		return false;
	}
}

/*
@author: Sahil-Kiran\
*/
//TODO: Sahil-Kiran, Added while merging mainwindow
void MainWindow::showPeakdetectionDialog() {
    peakDetectionDialog->show();   
   
}

// void MainWindow::showMassSlices() {
//    peakDetectionDialog->initPeakDetectionDialogWindow(
//    PeakDetectionDialog::FullSpectrum );
//    peakDetectionDialog->show(); //TODO: Sahil-Kiran, Added while merging
//    mainwindow
//}

// void MainWindow::compoundDatabaseSearch() {
//    peakDetectionDialog->initPeakDetectionDialogWindow(PeakDetectionDialog::CompoundDB);
//    peakDetectionDialog->show(); //TODO: Sahil-Kiran, Added while merging
//    mainwindow
//}

void MainWindow::showSRMList() {

     //added while merging with Maven776 - Kiran
     if (srmDockWidget->isVisible()) {
        double amuQ1 = getSettings()->value("amuQ1").toDouble();
        double amuQ3 = getSettings()->value("amuQ3").toDouble();
        bool associateCompoundNames=true;
        vector<mzSlice*>slices = getSrmSlices(amuQ1,amuQ3,associateCompoundNames);
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

	if (isotopeWidget && group->compound != NULL) {
		isotopeWidget->setCompound(group->compound);
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
        showPeakInfo(&(group->peaks[0]));
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

	BackgroundPeakUpdate* workerThread = newWorkerThread("alignUsingDatabase");
	connect(workerThread, SIGNAL(finished()), eicWidget, SLOT(replotForced()));
	connect(workerThread, SIGNAL(started()), alignmentDialog, SLOT(close()));

	if (settings != NULL) {
		mavenParameters->eic_smoothingAlgorithm = settings->value(
				"eic_smoothingWindow").toInt();
	}

	//mavenParameters->eic_ppmWindow = getUserPPM(); //TODO: Sahil-Kiran, Added while merging mainwindow

	mavenParameters->minGoodGroupCount =
			alignmentDialog->minGoodPeakCount->value();
	mavenParameters->limitGroupCount =
			alignmentDialog->limitGroupCount->value();
	mavenParameters->minGroupIntensity =
			alignmentDialog->minGroupIntensity->value();

	//TODO: Sahil Re-verify this two parameters. Values are same
	mavenParameters->eic_smoothingWindow =
			alignmentDialog->groupingWindow->value(); //TODO: Sahil-Kiran, Added while merging mainwindow
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
	mavenParameters->eicMaxGroups = 5;

	mavenParameters->samples = getSamples();
	mavenParameters->stop = false;
	mavenParameters->setCompounds(DB.getCopoundsSubset(ligandWidget->getDatabaseName().toStdString()));
	workerThread->setMavenParameters(mavenParameters);
	workerThread->setPeakDetector(new PeakDetector(mavenParameters));

    //connect new connections
    connect(workerThread, SIGNAL(newPeakGroup(PeakGroup*)), bookmarkedPeaks, SLOT(addPeakGroup(PeakGroup*))); //TODO: Sahil-Kiran, Added while merging mainwindow
    connect(workerThread, SIGNAL(finished()), bookmarkedPeaks, SLOT(showAllGroups())); //TODO: Sahil-Kiran, Added while merging mainwindow
    connect(workerThread, SIGNAL(terminated()), bookmarkedPeaks, SLOT(showAllGroups())); //TODO: Sahil-Kiran, Added while merging mainwindow


	workerThread->start();
}

void MainWindow::UndoAlignment() {
	for (int i = 0; i < samples.size(); i++) {
		if (samples[i])
			samples[i]->restoreOriginalRetentionTimes();
	}
	getEicWidget()->replotForced();
}

vector<mzSlice*> MainWindow::getSrmSlices(double amuQ1, double amuQ3, bool associateCompoundNames) {
	//Merged with Maven776 - Kiran
    qDebug() << "getSrmSlices() Q1=" << amuQ1 << " Q3=" << amuQ3;
    QSet<QString>seenMRMS;
    //+118.001@cid34.00 [57.500-58.500]
    //+ c ESI SRM ms2 102.000@cid19.00 [57.500-58.500]
    //-87.000 [42.500-43.500]
    //- c ESI SRM ms2 159.000 [113.500-114.500]

    QRegExp rx1a("[+/-](\\d+\\.\\d+)");
    QRegExp rx1b("ms2\\s*(\\d+\\.\\d+)");
    QRegExp rx2("(\\d+\\.\\d+)-(\\d+\\.\\d+)");
    int countMatches=0;

    vector<mzSlice*>slices;
    for(int i=0; i < samples.size(); i++ ) {
    	mzSample* sample = samples[i];
        for( int j=0; j < sample->scans.size(); j++ ) {
            Scan* scan = sample->getScan(j);
            if (!scan) continue;

            QString filterLine(scan->filterLine.c_str());            

            if (filterLine.isEmpty() or seenMRMS.contains(filterLine)) continue;
            seenMRMS.insert(filterLine);


            mzSlice* s = new mzSlice(0,0,0,0);
            s->srmId = scan->filterLine.c_str();
            slices.push_back(s);

            if (associateCompoundNames) {
                //match compounds
                Compound* compound = NULL;
                float precursorMz = scan->precursorMz;
                float productMz   = scan->productMz;
                int   polarity= scan->getPolarity();
                if (polarity==0) filterLine[0] == '+' ? polarity=1 : polarity =-1;
                if (getIonizationMode()) polarity=getIonizationMode();  //user specified ionization mode

                if ( precursorMz == 0 ) {
                    if( rx1a.indexIn(filterLine) != -1 ) {
                        precursorMz = rx1a.capturedTexts()[1].toDouble();
                    } else if ( rx1b.indexIn(filterLine) != -1 ) {
                        precursorMz = rx1b.capturedTexts()[1].toDouble();
                    }
                }

                if (productMz == 0) {
                    if ( rx2.indexIn(filterLine) != -1 ) {
                        float lb = rx2.capturedTexts()[1].toDouble();
                        float ub = rx2.capturedTexts()[2].toDouble();
                        productMz = lb+(ub-lb)/2;
                    }
                }

                if (precursorMz != 0 && productMz != 0 ) {
                    compound = DB.findSpeciesByPrecursor(precursorMz,productMz,polarity,amuQ1,amuQ3);
                }

                if (compound) {
                    compound->srmId = filterLine.toStdString();
                    s->compound = compound;
                    s->rt = compound->expectedRt;
                    countMatches++;
                }
            }
        }
        qDebug() << "SRM mapping: " << countMatches << " compounds mapped out of " << seenMRMS.size();
    }
    return slices;
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
	if (getIonizationMode())
		ionizationMode = getIonizationMode(); //user specified ionization mode

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
		isotopeWidget->setPeak(_peak);
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
	if (getIonizationMode())
		ionizationMode = getIonizationMode(); //user specified ionization mode

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
			<< "sample\tgroupId\tcompoundName\texpectedRt\tpeakMz\tmedianMz\trt\trtmin\trtmax\tquality\tpeakIntensity\tpeakArea\tpeakAreaTop\tpeakAreaCorrected\tnoNoiseObs\tsignalBaseLineRatio\tfromBlankSample";
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
	float ppm = getUserPPM();

	if (samples.size() <= 0)
		return;
	fragPanel->clearTree();
	for (unsigned int i = 0; i < samples.size(); i++) {
		for (unsigned int j = 0; j < samples[i]->scans.size(); j++) {
			if (samples[i]->scans[j]->mslevel > 1
					&& ppmDist(samples[i]->scans[j]->precursorMz, pmz) < ppm) {
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
	if (samples.size() > 0 && samples[0]->getPolarity() > 0)
		charge = 1;
	if (getIonizationMode())
		charge = getIonizationMode(); //user specified ionization mode

	float mz = c->ajustedMass(charge);
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

	QToolButton *btnZoom = new QToolButton(toolBar);
	btnZoom->setIcon(QIcon(rsrcPath + "/resetzoom.png"));
	btnZoom->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnZoom->setToolTip(tr("Zoom out (0)"));

	QToolButton *btnPrint = new QToolButton(toolBar);
	btnPrint->setIcon(QIcon(rsrcPath + "/fileprint.png"));
	btnPrint->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnPrint->setToolTip(tr("Print EIC (Ctr+P)"));
	btnPrint->setShortcut(tr("Ctrl+P"));

	QToolButton *btnPDF = new QToolButton(toolBar);
	btnPDF->setIcon(QIcon(rsrcPath + "/exportpdf.png"));
	btnPDF->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnPDF->setToolTip(tr("Save EIC Image to PDF file"));

	QToolButton *btnPNG = new QToolButton(toolBar);
	btnPNG->setIcon(QIcon(rsrcPath + "/copyPNG.png"));
	btnPNG->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnPNG->setToolTip(tr("Copy EIC Image to Clipboard"));

	QToolButton *btnLast = new QToolButton(toolBar);
	btnLast->setIcon(QIcon(rsrcPath + "/last.png"));
	btnLast->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnLast->setToolTip(tr("History Back (Ctrl+Left)"));
	btnLast->setShortcut(tr("Ctrl+Left"));

	QToolButton *btnNext = new QToolButton(toolBar);
	btnNext->setIcon(QIcon(rsrcPath + "/next.png"));
	btnNext->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnNext->setToolTip(tr("History Forward (Ctrl+Right)"));
	btnNext->setShortcut(tr("Ctrl+Right"));

	QToolButton *btnAutoZoom = new QToolButton(toolBar);
	btnAutoZoom->setCheckable(true);
	btnAutoZoom->setChecked(true);
	btnAutoZoom->setIcon(QIcon(rsrcPath + "/autofocus.png"));
	btnAutoZoom->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnAutoZoom->setToolTip(
			tr(
					"Auto Zoom. Always center chromatogram on expected retention time!"));

	QToolButton *btnGallary = new QToolButton(toolBar);
	btnGallary->setIcon(QIcon(rsrcPath + "/gallery.png"));
	btnGallary->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnGallary->setToolTip(tr("Show In Gallary"));

	QToolButton *btnShowTic = new QToolButton(toolBar);
	btnShowTic->setCheckable(true);
	btnShowTic->setChecked(false);
	btnShowTic->setIcon(QIcon(rsrcPath + "/tic.png"));
	btnShowTic->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnShowTic->setToolTip(tr("Show TICs"));

	QToolButton *btnMarkGood = new QToolButton(toolBar);
	btnMarkGood->setIcon(QIcon(rsrcPath + "/markgood.png"));
	btnMarkGood->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnMarkGood->setToolTip(tr("Bookmark as Good Group (G)"));

	QToolButton *btnMarkBad = new QToolButton(toolBar);
	btnMarkBad->setIcon(QIcon(rsrcPath + "/markbad.png"));
	btnMarkBad->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnMarkBad->setToolTip(tr("Bookmark as Bad Group (B)"));

	QToolButton *btnCopyCSV = new QToolButton(toolBar);
	btnCopyCSV->setIcon(QIcon(rsrcPath + "/copyCSV.png"));
	btnCopyCSV->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnCopyCSV->setToolTip(tr("Copy Group Information to Clipboard (Ctrl+C)"));
	btnCopyCSV->setShortcut(tr("Ctrl+C"));

	QToolButton *btnBookmark = new QToolButton(toolBar);
	btnBookmark->setIcon(QIcon(rsrcPath + "/bookmark.png"));
	btnBookmark->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnBookmark->setToolTip(tr("Bookmark Group (Ctrl+D)"));
	btnBookmark->setShortcut(tr("Ctrl+D"));

	QToolButton *btnIntegrateArea = new QToolButton(toolBar);
	btnIntegrateArea->setIcon(QIcon(rsrcPath + "/integrateArea.png"));
	btnIntegrateArea->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnIntegrateArea->setToolTip(tr("Manual Integration (Shift+MouseDrag)"));

	QToolButton *btnAverageSpectra = new QToolButton(toolBar);
	btnAverageSpectra->setIcon(QIcon(rsrcPath + "/averageSpectra.png"));
	btnAverageSpectra->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnAverageSpectra->setToolTip(tr("Average Specta (Ctrl+MouseDrag)"));


	//TODO: Sahil-Kiran, Added while merging mainwindow
    QToolButton *btnShowBarplot = new QToolButton(toolBar);
    btnShowBarplot->setIcon(QIcon(rsrcPath + "/barplot.png"));
    btnShowBarplot->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btnShowBarplot->setToolTip(tr("Show Barplot"));
    btnShowBarplot->setCheckable(true);
    btnShowBarplot->setChecked(true);
    
	//TODO: Sahil-Kiran, Added while merging mainwindow
    QToolButton *btnShowBoxplot = new QToolButton(toolBar);
    btnShowBoxplot->setIcon(QIcon(rsrcPath + "/boxplot.png"));
    btnShowBoxplot->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btnShowBoxplot->setToolTip(tr("Show Boxplot"));
    btnShowBoxplot->setCheckable(true);
    btnShowBoxplot->setChecked(false);
    
	//TODO: Sahil-Kiran, Added while merging mainwindow
    QToolButton *btnShowIsotopeplot = new QToolButton(toolBar);
    btnShowIsotopeplot->setIcon(QIcon(rsrcPath + "/isotopeplot.png"));
    btnShowIsotopeplot->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btnShowIsotopeplot->setToolTip(tr("Show Isotope Plot"));
    btnShowIsotopeplot->setCheckable(true);
    btnShowIsotopeplot->setChecked(true);

	//TODO: Sahil-Kiran, Added while merging mainwindow
    QToolButton *btnShowSplines = new QToolButton(toolBar);
    btnShowSplines->setIcon(QIcon(rsrcPath + "/splines.png"));
    btnShowSplines->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btnShowSplines->setToolTip(tr("Show Splines"));
    btnShowSplines->setCheckable(true);
    btnShowSplines->setChecked(false);
	/*
	 QSpinBox* smoothingWindowBox = new QSpinBox(toolBar);
	 smoothingWindowBox->setRange(1, 2000);
	 smoothingWindowBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	 smoothingWindowBox->setValue(settings->value("eic_smoothingWindow").toInt());
	 smoothingWindowBox->setToolTip("EIC Smoothing Window");
	 connect(smoothingWindowBox, SIGNAL(valueChanged(int)), SLOT(updateEicSmoothingWindow(int)));
	 */

	toolBar->addWidget(btnZoom);
	toolBar->addWidget(btnBookmark);
	toolBar->addWidget(btnCopyCSV);
	toolBar->addWidget(btnMarkGood);
	toolBar->addWidget(btnMarkBad);
	toolBar->addWidget(btnGallary);
	toolBar->addSeparator();
	toolBar->addWidget(btnIntegrateArea);
	toolBar->addWidget(btnAverageSpectra);

	toolBar->addWidget(btnLast);
	toolBar->addWidget(btnNext);
	toolBar->addSeparator();

	toolBar->addWidget(btnPDF);
	toolBar->addWidget(btnPNG);
	toolBar->addWidget(btnPrint);
	toolBar->addSeparator();

	toolBar->addWidget(btnAutoZoom);
	toolBar->addWidget(btnShowTic);
    toolBar->addSeparator(); //TODO: Sahil-Kiran, Added while merging mainwindow

    toolBar->addWidget(btnShowBarplot); //TODO: Sahil-Kiran, Added while merging mainwindow
    toolBar->addWidget(btnShowIsotopeplot); //TODO: Sahil-Kiran, Added while merging mainwindow
    toolBar->addWidget(btnShowBoxplot); //TODO: Sahil-Kiran, Added while merging mainwindow
    toolBar->addWidget(btnShowSplines); //TODO: Sahil-Kiran, Added while merging mainwindow
//    toolBar->addWidget(smoothingWindowBox);

	connect(btnLast, SIGNAL(clicked()), SLOT(historyLast()));
	connect(btnNext, SIGNAL(clicked()), SLOT(historyNext()));
	connect(btnPrint, SIGNAL(clicked()), SLOT(print()));
	connect(btnZoom, SIGNAL(clicked()), eicWidget, SLOT(resetZoom()));
	connect(btnPDF, SIGNAL(clicked()), SLOT(exportPDF()));
	connect(btnPNG, SIGNAL(clicked()), SLOT(exportSVG()));
	connect(btnAutoZoom, SIGNAL(toggled(bool)), eicWidget,
			SLOT(autoZoom(bool)));
	connect(btnBookmark, SIGNAL(clicked()), this, SLOT(bookmarkPeakGroup()));
	connect(btnCopyCSV, SIGNAL(clicked()), eicWidget, SLOT(copyToClipboard()));
	connect(btnMarkGood, SIGNAL(clicked()), eicWidget, SLOT(markGroupGood()));
	connect(btnMarkBad, SIGNAL(clicked()), eicWidget, SLOT(markGroupBad()));
	connect(btnGallary, SIGNAL(clicked()), eicWidget, SLOT(setGallaryToEics()));
	connect(btnIntegrateArea, SIGNAL(clicked()), eicWidget,
			SLOT(startAreaIntegration()));
	connect(btnAverageSpectra, SIGNAL(clicked()), eicWidget,
			SLOT(startSpectralAveraging()));

	
    connect(btnShowBarplot,SIGNAL(toggled(bool)),  eicWidget, SLOT(showBarPlot(bool))); //TODO: Sahil-Kiran, Added while merging mainwindow
    connect(btnShowBarplot,SIGNAL(toggled(bool)), eicWidget, SLOT(replot())); //TODO: Sahil-Kiran, Added while merging mainwindow
    
    connect(btnShowBoxplot,SIGNAL(toggled(bool)),  eicWidget, SLOT(showBoxPlot(bool))); //TODO: Sahil-Kiran, Added while merging mainwindow
    connect(btnShowBoxplot,SIGNAL(toggled(bool)), eicWidget, SLOT(replot())); //TODO: Sahil-Kiran, Added while merging mainwindow
   
    connect(btnShowIsotopeplot,SIGNAL(toggled(bool)),  eicWidget, SLOT(showIsotopePlot(bool))); //TODO: Sahil-Kiran, Added while merging mainwindow
    connect(btnShowIsotopeplot,SIGNAL(toggled(bool)),  eicWidget, SLOT(showIsotopicBarPlot())); //TODO: Sahil-Kiran, Added while merging mainwindow
    connect(btnShowIsotopeplot,SIGNAL(toggled(bool)), eicWidget, SLOT(updateIsotopicBarplot())); //TODO: Sahil-Kiran, Added while merging mainwindow
    
    connect(btnShowSplines,SIGNAL(toggled(bool)),  eicWidget, SLOT(showSpline(bool))); //TODO: Sahil-Kiran, Added while merging mainwindow
    connect(btnShowSplines,SIGNAL(toggled(bool)), eicWidget, SLOT(replot())); //TODO: Sahil-Kiran, Added while merging mainwindow
	connect(btnShowTic, SIGNAL(toggled(bool)), eicWidget,
			SLOT(showTicLine(bool)));
	connect(btnShowTic, SIGNAL(toggled(bool)), eicWidget, SLOT(replot()));

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

	float ppm = getUserPPM();

	vector<mzLink> links = peak->findCovariants();
	vector<mzLink> linksX = SpectraWidget::findLinks(peak->peakMz, scan, ppm,
			ionizationMode);
	for (int i = 0; i < linksX.size(); i++)
		links.push_back(linksX[i]);

	//correlations
	float rtmin = peak->rtmin - 1;
	float rtmax = peak->rtmax + 1;
	for (int i = 0; i < links.size(); i++) {
		links[i].correlation = sample->correlation(links[i].mz1, links[i].mz2,
				5, rtmin, rtmax);
	}

	//matching compounds
	for (int i = 0; i < links.size(); i++) {
		QSet<Compound*> compunds = massCalcWidget->findMathchingCompounds(
				links[i].mz2, ppm, ionizationMode);
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

MatrixXf MainWindow::getIsotopicMatrix(PeakGroup* group) {

	PeakGroup::QType qtype = getUserQuantType();
	//get visiable samples
	vector<mzSample*> vsamples = getVisibleSamples();
	sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);

	//get isotopic groups
	vector<PeakGroup*> isotopes;
	for (int i = 0; i < group->childCountBarPlot(); i++) {
		if (group->childrenBarPlot[i].isIsotope()) {
			PeakGroup* isotope = &(group->childrenBarPlot[i]);
			isotopes.push_back(isotope);
		}
	}
	std::sort(isotopes.begin(), isotopes.end(), PeakGroup::compC13);

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

	isotopeC13Correct(MM, numberofCarbons);
	return MM;
}

MatrixXf MainWindow::getIsotopicMatrixIsoWidget(PeakGroup* group) {

	PeakGroup::QType qtype = getUserQuantType();
	//get visiable samples
	vector<mzSample*> vsamples = getVisibleSamples();
	sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);

	//get isotopic groups
	vector<PeakGroup*> isotopes;
	for (int i = 0; i < group->childCount(); i++) {
		if (group->children[i].isIsotope()) {
			PeakGroup* isotope = &(group->children[i]);
			isotopes.push_back(isotope);
		}
	}
	std::sort(isotopes.begin(), isotopes.end(), PeakGroup::compC13);

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

	isotopeC13Correct(MM, numberofCarbons);
	return MM;
}

void MainWindow::isotopeC13Correct(MatrixXf& MM, int numberofCarbons) {
	if (numberofCarbons == 0)
		return;

	qDebug() << "IsotopePlot::isotopeC13Correct() " << MM.rows() << " "
			<< MM.cols() << " nCarbons=" << numberofCarbons << endl;
	if (settings && settings->value("isotopeC13Correction").toBool() == false)
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
			vector<double> cmv = mzUtils::naturalAbundanceCorrection(
					numberofCarbons, mv);

			for (int j = 0; j < mv.size(); j++) {
				MM(i, j) = cmv[j];

				//cerr << mv[j] << " " << cmv[j] << endl;
			}
		}
	}
	cerr << "IsotopePlot::IsotopePlot() done.." << endl;

}

void MainWindow::updateEicSmoothingWindow(int value) {
	settings->setValue("eic_smoothingWindow", value);
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


/*
@author: Sahil
*/
//TODO: Sahil, Added this while mergin mzfileio
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
