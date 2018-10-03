#include "pollyelmaveninterface.h"
#include <string>
#include <QVariant>

PollyElmavenInterfaceDialog::PollyElmavenInterfaceDialog(MainWindow* mw) :
        QDialog(mw),
        mainwindow(mw),
        _loginform(NULL)
{
        setModal(true);
        setupUi(this);
        createIcons();
        workflowMenu->setCurrentRow(int(PollyApp::FirstView));

        _pollyIntegration = new PollyIntegration();
        _loadingDialog = new PollyWaitDialog(this);
        pollyButton->setVisible(false);
        fluxButton->setVisible(false);
        
        connect(pollyButton, SIGNAL(clicked(bool)), SLOT(goToPolly()));
        connect(fluxButton, SIGNAL(clicked(bool)), SLOT(goToPolly()));
        //connect(checkBox_advanced_settings,SIGNAL(clicked(bool)),SLOT(showAdvanceSettings()));
        connect(firstViewUpload, SIGNAL(clicked(bool)), SLOT(uploadDataToPolly()));
        connect(fluxUpload, SIGNAL(clicked(bool)), SLOT(uploadDataToPolly()));
        connect(firstViewCancel, SIGNAL(clicked(bool)), SLOT(cancel()));
        connect(fluxCancel, SIGNAL(clicked(bool)), SLOT(cancel()));
        connect(firstViewCreateProject, SIGNAL(clicked(bool)), SLOT(handleNewProject()));
        connect(fluxCreateProject, SIGNAL(clicked(bool)), SLOT(handleNewProject()));
        connect(firstViewSelectProject, SIGNAL(clicked(bool)), SLOT(handleSelectProject()));
        connect(fluxSelectProject, SIGNAL(clicked(bool)), SLOT(handleSelectProject()));
        connect(firstViewLogout, SIGNAL(clicked(bool)), SLOT(logout()));
        connect(fluxLogout, SIGNAL(clicked(bool)), SLOT(logout()));
        connect(this, SIGNAL(uploadFinished(bool)), SLOT(performPostUploadTasks(bool)));
}
 
PollyElmavenInterfaceDialog::~PollyElmavenInterfaceDialog()
{
    qDebug()<<"exiting PollyElmavenInterfaceDialog now....";
    if (_loginform) delete (_loginform);
}

EPIWorkerThread::EPIWorkerThread()
{
    _pollyintegration = new PollyIntegration();   
    
};

void EPIWorkerThread::run()
{
    if (state == "initial_setup"){
        qDebug() << "Checking for active internet connection..";
        QString status;
        if (!_pollyintegration->activeInternet()) {
            status = "error";
            emit authentication_result(status);
            return;
        }

        qDebug() << "Authenticating..";
        status = _pollyintegration->authenticateLogin("","");
        emit authentication_result(status);
        
        qDebug() << "Fetching projects from Polly..";
        if (status == "ok") {
            QVariantMap projectNamesId = _pollyintegration->getUserProjects();
            emit resultReady(projectNamesId);
        }
    } else if (state == "upload_files") {
        qDebug() << "starting thread for uploading files to polly..";
        //re-login to polly may be required because the token expires after 30 minutes..
        QString statusInside = _pollyintegration->authenticateLogin("","");
        QStringList patchId  = _pollyintegration->exportData(filesToUpload, uploadProjectIdThread);
        emit filesUploaded(patchId, uploadProjectIdThread, datetimestamp);
    }
}

EPIWorkerThread::~EPIWorkerThread()
{
    for (auto fileName : filesToUpload) {
        tmpDir.remove(fileName);
    }
    if (_pollyintegration) delete (_pollyintegration);
};

void PollyElmavenInterfaceDialog::createIcons()
{   
    QListWidgetItem *firstView = new QListWidgetItem(workflowMenu);
    firstView->setIcon(QIcon(":/images/firstView.png"));
    firstView->setText(tr("FirstView"));
    firstView->setTextAlignment(Qt::AlignHCenter);
    firstView->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    firstView->setToolTip("FirstView: Preview El-MAVEN processed data and perform further analysis");

    QListWidgetItem *fluxomics = new QListWidgetItem(workflowMenu);
    fluxomics->setIcon(QIcon(":/images/flux.png"));
    fluxomics->setText(tr("PollyPhi Relative"));
    fluxomics->setTextAlignment(Qt::AlignHCenter);
    fluxomics->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    fluxomics->setToolTip("PollyPhi Relative: Process and derive insights from Relative flux workflow");

    workflowMenu->setSizeAdjustPolicy(QListWidget::AdjustToContents);
    workflowMenu->setViewMode(QListView::IconMode);
    workflowMenu->setFlow(QListView::TopToBottom);
    workflowMenu->setSpacing(18);
    workflowMenu->setIconSize(QSize(140, 140));

    connect(workflowMenu, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
        this, SLOT(changePage(QListWidgetItem*, QListWidgetItem*)));
}

void PollyElmavenInterfaceDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
 {
    if (!current)
        current = previous;

    stackedWidget->setCurrentIndex(workflowMenu->row(current));
 }

void PollyElmavenInterfaceDialog::goToPolly()
{
    QDesktopServices::openUrl(pollyURL);
}

void PollyElmavenInterfaceDialog::handleNewProject()
{
    if (stackedWidget->currentIndex() == int(PollyApp::FirstView)) {
        firstViewNewProject->setEnabled(true);
        firstViewProjectList->setEnabled(false);
    }
    
    if (stackedWidget->currentIndex() == int(PollyApp::Fluxomics)) {
        fluxNewProject->setEnabled(true);
        fluxProjectList->setEnabled(false);
    }
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::handleSelectProject()
{
    if (stackedWidget->currentIndex() == int(PollyApp::FirstView)) {
        firstViewNewProject->setEnabled(false);
        firstViewProjectList->setEnabled(true);
    }
    
    if (stackedWidget->currentIndex() == int(PollyApp::Fluxomics)) {
        fluxNewProject->setEnabled(false);
        fluxProjectList->setEnabled(true);
    }
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::showAdvanceSettings() 
{
    advancedSettingsFlag = true;
    _advancedSettings = new AdvancedSettings();
    _advancedSettings->initialSetup();
}

void PollyElmavenInterfaceDialog::initialSetup()
{   
    int nodeStatus = _pollyIntegration->checkNodeExecutable();
    if (nodeStatus == 0) {
        QMessageBox msgBox(this);
        msgBox.setWindowModality(Qt::NonModal);
        msgBox.setWindowTitle("node is not installed on this system");
        msgBox.exec();
        return;
    }
    int askForLogin = _pollyIntegration->askForLogin();
    if (askForLogin == 1) {
        try {
            call_login_form();
        } catch(...) {
                QMessageBox msgBox(this);
                msgBox.setWindowModality(Qt::NonModal);
                msgBox.setWindowTitle("couldn't load login form");
                msgBox.exec();
        }
        return;
    } else {
        try {
            call_initial_EPI_form();
        } catch(...) {
                QMessageBox msgBox(this);
                msgBox.setWindowModality(Qt::NonModal);
                msgBox.setWindowTitle("couldn't load initial form");
                msgBox.exec();
        }
        return;
    }
}

void PollyElmavenInterfaceDialog::call_login_form()
{
    _loginform = new LoginForm(this);
    _loginform->setModal(true);
    _loginform->show();
}

void PollyElmavenInterfaceDialog::call_initial_EPI_form() 
{
    if (stackedWidget->currentIndex() == int(PollyApp::FirstView)) {
        firstViewUpload->setEnabled(false);
        firstViewProjectList->clear();
    } else {
        fluxUpload->setEnabled(false);
        fluxProjectList->clear();
    }
    
    EPIWorkerThread *EPIworkerThread = new EPIWorkerThread();
    connect(EPIworkerThread, SIGNAL(resultReady(QVariantMap)), this, SLOT(handleResults(QVariantMap)));
    connect(EPIworkerThread, SIGNAL(authentication_result(QString)), this, SLOT(handleAuthentication(QString)));
    connect(EPIworkerThread, &EPIWorkerThread::finished, EPIworkerThread, &QObject::deleteLater);
    EPIworkerThread->state = "initial_setup";
    EPIworkerThread->start();
    show();
    
    _loadingDialog->setModal(true);
    _loadingDialog->show();
    _loadingDialog->statusLabel->setVisible(true);
    _loadingDialog->statusLabel->setStyleSheet("QLabel {color : green;}");
    _loadingDialog->statusLabel->setText("Authenticating..");
    _loadingDialog->label->setVisible(true);
    _loadingDialog->label->setMovie(_loadingDialog->movie);
    _loadingDialog->label->setAlignment(Qt::AlignCenter);
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::handleAuthentication(QString status)
{
    if (status == "ok") {
        _loadingDialog->statusLabel->setStyleSheet("QLabel {color : green;}");
        _loadingDialog->statusLabel->setText("Fetching your projects..");
        QCoreApplication::processEvents();
    } else if (status == "error") {
        _loadingDialog->statusLabel->setStyleSheet("QLabel {color : red;}");
        _loadingDialog->statusLabel->setText("No Internet Access");
        QCoreApplication::processEvents();
        close();
    } else {
        _loadingDialog->statusLabel->setStyleSheet("QLabel {color : red;}");
        _loadingDialog->statusLabel->setText("Authentication failed. Please login again.");
        QCoreApplication::processEvents();
        logout();
    }
}

void PollyElmavenInterfaceDialog::handleResults(QVariantMap projectnamesIdMap)
{
    projectNamesId = projectnamesIdMap;
    startupDataLoad();
}

void PollyElmavenInterfaceDialog::setFluxPage()
{
    stackedWidget->setCurrentIndex(int(PollyApp::Fluxomics));
    workflowMenu->setCurrentRow(int(PollyApp::Fluxomics));
}

void PollyElmavenInterfaceDialog::setUiElementsFlux()
{
    fluxTableList->clear();
    fluxCreateProject->setChecked(true);
    fluxNewProject->setEnabled(true);
    
    fluxSelectProject->setChecked(false);
    fluxProjectList->setEnabled(false);
    fluxProjectList->clear();
    
    fluxButton->setVisible(false);
    fluxUpload->setEnabled(true);
}

void PollyElmavenInterfaceDialog::setUiElementsFV()
{
    firstViewTableList->clear();

    firstViewCreateProject->setChecked(true);
    firstViewNewProject->setEnabled(true);
    
    firstViewSelectProject->setChecked(false);
    firstViewProjectList->setEnabled(false);
    firstViewProjectList->clear();
    
    pollyButton->setVisible(false);
    firstViewUpload->setEnabled(true);
}

void PollyElmavenInterfaceDialog::startupDataLoad()
{
    setUiElementsFlux();
    setUiElementsFV();

    QCoreApplication::processEvents();

    QIcon project_icon(rsrcPath + "/POLLY.png");
    if (projectNamesId.isEmpty()) {
        projectNamesId = _pollyIntegration->getUserProjects();
    }    
    
    QStringList keys= projectNamesId.keys();
    for (auto key : keys) {
        fluxProjectList->addItem(project_icon, projectNamesId[key].toString());
        firstViewProjectList->addItem(project_icon, projectNamesId[key].toString());
    }

    bookmarkedPeaks = mainwindow->getBookmarkedPeaks();
    QString tableName = "Bookmark Table ";
    if (!bookmarkedPeaks->getGroups().isEmpty()) {
        firstViewTableList->insertItem(0, tableName);
        if (bookmarkedPeaks->labeledGroups > 0)
            fluxTableList->insertItem(0, tableName);
        tableNameMapping[tableName] = bookmarkedPeaks;
    } 

    QList<QPointer<TableDockWidget> > peaksTableList = mainwindow->getPeakTableList();
    for (auto peakTable : peaksTableList) {
        if (peakTable->getGroups().isEmpty())
            continue;
        
        QString peakTableName = QString("Peak Table " + QString::number(peakTable->tableId) + " ");
        firstViewTableList->insertItem(0, peakTableName);
        if (peakTable->labeledGroups > 0)
            fluxTableList->insertItem(0, peakTableName);
        tableNameMapping[peakTableName] = peakTable;
    }

    //set latest table by default
    firstViewTableList->setCurrentIndex(0);
    fluxTableList->setCurrentIndex(0);

    //set to active table if available
    if (_activeTable && tableNameMapping.values().contains(_activeTable)) {
        QString tableName = tableNameMapping.key(_activeTable);
        firstViewTableList->setCurrentText(tableName);
        fluxTableList->setCurrentText(tableName);
        //reset active table
        _activeTable = NULL;
    }

    _loadingDialog->close();
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::uploadDataToPolly()
{
    //set currently visible items
    QPushButton* uploadButton = firstViewUpload;
    QLabel* statusUpdate = firstViewStatus;
    QLineEdit* newProject = firstViewNewProject;
    QComboBox* projectList = firstViewProjectList;

    if (stackedWidget->currentIndex() == int(PollyApp::Fluxomics)) {
        uploadButton = fluxUpload;
        statusUpdate = fluxStatus;
        newProject = fluxNewProject;
        projectList = fluxProjectList;
    }
    
    uploadButton->setEnabled(false);

    statusUpdate->setText("Connecting..");
    QCoreApplication::processEvents();

    //check for active internet connection
    if (!_pollyIntegration->activeInternet()) {
        statusUpdate->setStyleSheet("QLabel {color : red;}");
        statusUpdate->setText("No Internet Access");
        uploadButton->setEnabled(true);
        return;
    }

    //redirect to login form if user credentials have not been saved
    int askForLogin = _pollyIntegration->askForLogin();
    if (askForLogin == 1) {
        call_login_form();
        emit uploadFinished(false);
        return;
    }
    
    statusUpdate->setStyleSheet("QLabel {color : green; }");
    statusUpdate->setText("Preparing files..");
    QCoreApplication::processEvents();

    //create a temporary directory to store polly files
    qDebug() << "writing Polly temp file in this directory -" << writableTempDir;
    QDir qdir(writableTempDir);
    if (!qdir.exists()){
        QDir().mkdir(writableTempDir);
        QDir qdir(writableTempDir);
    }
    
    //add datetimestamp to identify files from one upload thread
    QDateTime current_time;
    const QString format = "dd-MM-yyyy_hh_mm_ss";
	QString datetimestamp= current_time.currentDateTime().toString(format);
    datetimestamp.replace(" ","_");
    datetimestamp.replace(":","-");
    
    QStringList filenames = prepareFilesToUpload(qdir, datetimestamp);
    if (filenames.isEmpty()) {
        statusUpdate->setText("File preparation failed.");
        _loadingDialog->close();
        QCoreApplication::processEvents();
        emit uploadFinished(false);
        return;
    }

    statusUpdate->setText("Sending files to Polly..");  
    QCoreApplication::processEvents();

    uploadProjectId = getProjectId(projectList, newProject);
    if (uploadProjectId.isEmpty()) {
        emit uploadFinished(false);
        statusUpdate->setText("");
        return;
    }
    
    //check for active internet again before upload
    if (!_pollyIntegration->activeInternet()) {
        statusUpdate->setStyleSheet("QLabel {color : red;}");
        statusUpdate->setText("Internet connection interrupted");
        uploadButton->setEnabled(true);
        return;
    }

    EPIWorkerThread *EPIworkerThread = new EPIWorkerThread();
    connect(EPIworkerThread, SIGNAL(filesUploaded(QStringList, QString, QString)), this, SLOT(postUpload(QStringList, QString, QString)));
    connect(EPIworkerThread, &EPIWorkerThread::finished, EPIworkerThread, &QObject::deleteLater);
    EPIworkerThread->state = "upload_files";
    EPIworkerThread->filesToUpload = filenames;
    EPIworkerThread->tmpDir = qdir;
    EPIworkerThread->uploadProjectIdThread = uploadProjectId;
    EPIworkerThread->datetimestamp = datetimestamp;
    EPIworkerThread->start();
}

QString PollyElmavenInterfaceDialog::getProjectId(QComboBox* projectList, QLineEdit* newProject)
{
    if (projectList->isEnabled()) {
        QString projectId;
        QStringList keys = projectNamesId.keys();
        projectId = projectNamesId.key(projectList->currentText());

        if (projectId.isEmpty())
            showErrorMessage("Error", "No such project on Polly");
        
        return projectId;
    }
    
    if (newProject->isEnabled()) {
        if (newProject->text().isEmpty()) {
            showErrorMessage("Error", "Invalid Project name");
            return "";
        }

        return _pollyIntegration->createProjectOnPolly(newProject->text());
    }
    
    showErrorMessage("Error", "Please select at least one option");
    return "";
}

void PollyElmavenInterfaceDialog::showErrorMessage(QString title, QString message)
{
    QMessageBox errorBox(mainwindow);
    errorBox.setWindowTitle(title);
    errorBox.setText(message);
    errorBox.exec();
}

void PollyElmavenInterfaceDialog::postUpload(QStringList patchId, QString uploadProjectIdThread, QString datetimestamp)
{
    QCoreApplication::processEvents();
    
    //set UI elements
    QLabel* statusUpdate = firstViewStatus;
    QPushButton* redirectButton = pollyButton;

    if (stackedWidget->currentIndex() == int(PollyApp::Fluxomics)) {
        statusUpdate = fluxStatus;
        redirectButton = fluxButton;
    }
    
    if (!patchId.isEmpty()) {
        statusUpdate->setText("");
        QString redirection_url = getRedirectionUrl(datetimestamp, uploadProjectIdThread);
        qDebug() << "redirection_url     - " << redirection_url;
        pollyURL.setUrl(redirection_url);
        redirectButton->setVisible(true);
    } else {
        statusUpdate->setStyleSheet("QLabel {color : red; }");
        statusUpdate->setText("Error!");
        
        showErrorMessage("Warning!", "Unable to send data");
        statusUpdate->setText("");
    }
    emit uploadFinished(false);
}

QString PollyElmavenInterfaceDialog::getRedirectionUrl(QString datetimestamp, QString uploadProjectIdThread)
{
    QString redirectionUrl;
    //redirect to firstView
    if (stackedWidget->currentIndex() == int(PollyApp::FirstView)) {
        redirectionUrl = QString("https://polly.elucidata.io/main#project=%1&auto-redirect=%2&elmavenTimestamp=%3").arg(uploadProjectIdThread).arg("firstview").arg(datetimestamp);
        return redirectionUrl;
    }
    
    //redirect to fluxomics
    if (stackedWidget->currentIndex() == int(PollyApp::Fluxomics)) { 
        QString landingPage = QString("relative_lcms_elmaven");
        QString workflowRequestId = _pollyIntegration->createWorkflowRequest(uploadProjectIdThread);
        
        //send to google sheets if sample cohort file is not valid
        QString CohortFileName = writableTempDir + QDir::separator() + datetimestamp + "_Cohort_Mapping_Elmaven.csv";
        if (!_pollyIntegration->validSampleCohort(CohortFileName))
            landingPage = QString("gsheet_sym_polly_elmaven");

        redirectionUrl = QString(
                            "https://polly.elucidata.io/workflow-requests/%1/lcms_tstpl_pvd/dashboard#redirect-from=%2#project=%3#timestamp=%4")
                            .arg(workflowRequestId)
                            .arg(landingPage)
                            .arg(uploadProjectIdThread)
                            .arg(datetimestamp);
        
    }  
    return redirectionUrl;  
}

QStringList PollyElmavenInterfaceDialog::prepareFilesToUpload(QDir qdir, QString datetimestamp)
{
    QStringList filenames;
    TableDockWidget* peakTable = NULL;

    //set UI elements
    QComboBox* tableList = firstViewTableList;
    QLabel* statusUpdate = firstViewStatus;

    if (stackedWidget->currentIndex() == int(PollyApp::Fluxomics)) {
        tableList = fluxTableList;
        statusUpdate = fluxStatus;
    }

    //check for no peak tables
    if (tableList->currentIndex() == -1)
    {        
        showErrorMessage("Error", "No Peak tables");
        return filenames;
    }
    
    peakTable = tableNameMapping[tableList->currentText()];

    statusUpdate->setStyleSheet("QLabel {color : green; }");
    statusUpdate->setText("Preparing compound database file..");
    
    QCoreApplication::processEvents();

    if (advancedSettingsFlag)
        handle_advanced_settings(datetimestamp, peakTable);
    
    if (!advancedSettingsFlag || !_advancedSettings->getUploadCompoundDB()) {
        // Now uploading the Compound DB that was used for peak detection.
        // This is needed for Elmaven->Firstview->PollyPhi relative LCMS workflow.
        // ToDo Kailash, Keep track of compound DB used for each peak table,
        // As of now, uploading what is currently there in the compound section of Elmaven.
        // If advanced settings were used to upload compound DB, we need not do this.
        QString compoundDb = mainwindow->ligandWidget->getDatabaseName();
        mainwindow->ligandWidget->saveCompoundList(writableTempDir + QDir::separator() + datetimestamp + "_Compound_DB_Elmaven.csv", compoundDb);
    }

    qDebug() << "Now uploading all groups, needed for firstview app..";
    peakTable->wholePeakSet();
    peakTable->treeWidget->selectAll();
    peakTable->prepareDataForPolly(writableTempDir,
                "Groups Summary Matrix Format Comma Delimited (*.csv)",
                datetimestamp
                + "_Peak_table_all_");
    
    //Preparing the json file
    statusUpdate->setStyleSheet("QLabel {color : green; }");
    statusUpdate->setText("Preparing json file..");
    
    QCoreApplication::processEvents();
 
    QString json_filename = writableTempDir
                            + QDir::separator()
                            + datetimestamp
                            + "_Peaks_information_json_Elmaven_Polly.json";
    peakTable->exportJsonToPolly(writableTempDir, json_filename);
    
    if (stackedWidget->currentIndex() == int(PollyApp::Fluxomics)) {
        fluxStatus->setStyleSheet("QLabel {color : green; }");
        fluxStatus->setText("Preparing sample cohort file..");
        QCoreApplication::processEvents();
        //Preparing the sample cohort file
        QString sampleCohortFileName = writableTempDir + QDir::separator() + datetimestamp +
                                        "_Cohort_Mapping_Elmaven.csv";
        mainwindow->projectDockWidget->prepareSampleCohortFile(sampleCohortFileName);
    }
    
    //Saving settings file
    QByteArray ba = (writableTempDir
                    + QDir::separator()
                    + datetimestamp
                    + "_maven_analysis_settings"
                    + ".xml").toLatin1();
    const char *save_path = ba.data();
    mainwindow->mavenParameters->saveSettings(save_path);
    qDebug() << "settings saved now";
    
    qdir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList file_list = qdir.entryInfoList();
    
    for (auto fileInfo : file_list) {
        QString tmp_filename = writableTempDir + QDir::separator() + fileInfo.fileName();
        filenames.append(tmp_filename);
    }
    return filenames;
}

void PollyElmavenInterfaceDialog::handle_advanced_settings(QString datetimestamp, TableDockWidget* peakTable)
{
        QVariantMap advanced_ui_elements = _advancedSettings->getUIElements();

        QString exportOption = advanced_ui_elements["export_option"].toString();
        QString exportFormat = advanced_ui_elements["export_format"].toString();
        QString userFilename = advanced_ui_elements["user_filename"].toString();
        QString compoundDb = advanced_ui_elements["compound_db"].toString();
        QString userCompoundDBName = advanced_ui_elements["user_compound_DB_name"].toString();
        
        if (userFilename == "") {
            userFilename = "intensity_file.csv";
        }
        if (userCompoundDBName == "") {
            userCompoundDBName = "compounds";
        }
        if (_advancedSettings->getUploadCompoundDB()) {
            mainwindow->ligandWidget->saveCompoundList(writableTempDir + QDir::separator()
                                                        + datetimestamp
                                                        + "_Compound_DB_"
                                                        + userCompoundDBName
                                                        + ".csv", compoundDb );
        }
        
        if (_advancedSettings->getUploadPeakTable()) {
            if (exportOption == "Export Selected") {
                peakTable->selectedPeakSet();
            } else if (exportOption == "Export Good") {
                peakTable->goodPeakSet();
                peakTable->treeWidget->selectAll();
            } else if (exportOption == "Export All Groups") {
                peakTable->wholePeakSet();
                peakTable->treeWidget->selectAll();
            } else if(exportOption == "Export Bad") {
                peakTable->badPeakSet();
                peakTable->treeWidget->selectAll();
            }
            _loadingDialog->statusLabel->setStyleSheet("QLabel {color : green; }");
            _loadingDialog->statusLabel->setText("Preparing intensity file..");
            QCoreApplication::processEvents();

            QString peakUserFilename  = datetimestamp + "_Peak_table_" + userFilename;
            qDebug() << "peakUserFilename - " << peakUserFilename;
            peakTable->prepareDataForPolly(writableTempDir, exportFormat, peakUserFilename);
        }

}

void PollyElmavenInterfaceDialog::cancel()
{
    advancedSettingsFlag = false;
    close();   
}

void PollyElmavenInterfaceDialog::logout()
{
    advancedSettingsFlag = false;
    _pollyIntegration->logout();
    projectNamesId = QVariantMap();
    close();   
}

void PollyElmavenInterfaceDialog::performPostUploadTasks(bool uploadSuccessful)
{
    firstViewUpload->setEnabled(true);
    fluxUpload->setEnabled(true);
}