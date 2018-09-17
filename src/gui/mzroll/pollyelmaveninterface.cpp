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
        workflowMenu->setCurrentRow(0);

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

void EPIWorkerThread::run(){
    if (state=="initial_setup"){
        qDebug()<<"starting thread to authenticate and fetch project info from polly";
        QString statusInside = _pollyintegration->authenticateLogin("","");
        emit authentication_result(statusInside);
        if (statusInside=="ok"){
            QVariantMap projectNamesId = _pollyintegration->getUserProjects();
            emit resultReady(projectNamesId);
        }
    }
    else if (state=="upload_files"){
        qDebug()<<"starting thread for uploading files to polly..";
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

void PollyElmavenInterfaceDialog::handleNewProject(){
    if (stackedWidget->currentIndex() == 0) {
        firstViewNewProject->setEnabled(true);
        firstViewProjectList->setEnabled(false);
    }
    else {
        fluxNewProject->setEnabled(true);
        fluxProjectList->setEnabled(false);
    }
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::handleSelectProject(){
    if (stackedWidget->currentIndex() == 0) {
        firstViewNewProject->setEnabled(false);
        firstViewProjectList->setEnabled(true);
    }
    else {
        fluxNewProject->setEnabled(false);
        fluxProjectList->setEnabled(true);
    }
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::showAdvanceSettings() {
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
    }
    else {
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

void PollyElmavenInterfaceDialog::call_login_form() {
    _loginform = new LoginForm(this);
    _loginform->setModal(true);
    _loginform->show();
}

void PollyElmavenInterfaceDialog::call_initial_EPI_form() {
    if (stackedWidget->currentIndex() == 0) {
        firstViewUpload->setEnabled(false);
        firstViewProjectList->clear();
    }
    else {
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
    _loadingDialog->statusLabel->setText("Authenticating..");
    _loadingDialog->label->setVisible(true);
    _loadingDialog->label->setMovie(_loadingDialog->movie);
    _loadingDialog->label->setAlignment(Qt::AlignCenter);
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::handleAuthentication(QString status) {
    if (status == "ok") {
        _loadingDialog->statusLabel->setText("Fetching your projects..");
        QCoreApplication::processEvents();
    }
    else {
            _loadingDialog->statusLabel->setText("Authentication failed. Please login again.");
            QCoreApplication::processEvents();
            logout();
    }
}

void PollyElmavenInterfaceDialog::handleResults(QVariantMap projectnamesIdMap){
    projectNamesId = projectnamesIdMap;
    startupDataLoad();
}

void PollyElmavenInterfaceDialog::setFluxPage()
{
    //index 1 corresponds to Fluxomics
    //TODO: use an enum to make this less error prone and more scalable
    stackedWidget->setCurrentIndex(1);
    workflowMenu->setCurrentRow(1);
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
    for (int i = 0; i < keys.size(); ++i){
        fluxProjectList->addItem(project_icon, projectNamesId[keys.at(i)].toString());
        firstViewProjectList->addItem(project_icon, projectNamesId[keys.at(i)].toString());
    }

    QList<QPointer<TableDockWidget> > peaksTableList = mainwindow->getPeakTableList();
    bookmarkedPeaks = mainwindow->getBookmarkedPeaks();
    if (!bookmarkedPeaks->getGroups().isEmpty()) {
        bookmarkTableNameMapping[QString("Bookmark Table")] = bookmarkedPeaks;
        fluxTableList->addItem("Bookmark Table");
        firstViewTableList->addItem("Bookmark Table");
    } 

    int n = peaksTableList.size();
    if (n > 0) {
        for (int i = 0; i < n; ++i) {
            QString peak_table_name = QString("Peak Table ")+QString::number(i+1);
            peakTableNameMapping[peak_table_name] = peaksTableList.at(i);
            fluxTableList->addItem(peak_table_name);
            firstViewTableList->addItem(peak_table_name);
        }        
    }

    _loadingDialog->close();
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::uploadDataToPolly()
{
    fluxUpload->setEnabled(false);
    firstViewUpload->setEnabled(false);
    int askForLogin = _pollyIntegration->askForLogin();
    if (askForLogin == 1) {
        call_login_form();
        emit uploadFinished(false);
        return;
    }
    
    QStringList patchId;

    QString new_projectname;
    QString projectname;
    if (stackedWidget->currentIndex() == 0) {
        new_projectname = firstViewNewProject->text();
        projectname = firstViewProjectList->currentText();
    }
    else {
        new_projectname = fluxNewProject->text();
        projectname = fluxProjectList->currentText();
    }
    
    QString project_id;

    qDebug() << "writing Polly temp file in this directory -" << writableTempDir;
    QDir qdir(writableTempDir);
    if (!qdir.exists()){
        QDir().mkdir(writableTempDir);
        QDir qdir(writableTempDir);
    }
    
    if (stackedWidget->currentIndex() == 0) {
        firstViewStatus->setStyleSheet("QLabel {color : green; }");
        firstViewStatus->setText("Preparing files..");
    }
    else {
        fluxStatus->setStyleSheet("QLabel {color : green; }");
        fluxStatus->setText("Preparing files..");
    }
    QCoreApplication::processEvents();
    QDateTime current_time;
    QString datetimestamp = current_time.currentDateTime().toString();
    datetimestamp.replace(" ","_");
    datetimestamp.replace(":","-");
    
    QStringList filenames = prepareFilesToUpload(qdir, datetimestamp);
    if (filenames.isEmpty()) {
        if (stackedWidget->currentIndex() == 0)
            firstViewStatus->setText("File preparation failed.");
        else    fluxStatus->setText("File preparation failed.");
        _loadingDialog->close();
        QCoreApplication::processEvents();
        emit uploadFinished(false);
        return;
    }
    if (stackedWidget->currentIndex() == 0)
        firstViewStatus->setText("Connecting..");
    else fluxStatus->setText("Connecting..");
    QCoreApplication::processEvents();

    if (stackedWidget->currentIndex() == 0)
        firstViewStatus->setText("Sending files to Polly..");
    else fluxStatus->setText("Sending files to Polly..");
    
    QCoreApplication::processEvents();
    
    if ((stackedWidget->currentIndex() == 0 && firstViewProjectList->isEnabled()) 
            || (stackedWidget->currentIndex() == 1 && fluxProjectList->isEnabled())) {
        QStringList keys = projectNamesId.keys();
        for (int i = 0; i < keys.size(); ++i) {
            if (projectNamesId[keys.at(i)].toString() == projectname) {
                project_id = keys.at(i);
            }
        }
        if (project_id != "") {
            upload_project_id = project_id;
        }
        else {
            QString msg = "No such project on Polly..";
            QMessageBox msgBox(mainwindow);
            msgBox.setWindowTitle("Error");
            msgBox.setText(msg);
            msgBox.exec();
        }
    }
    else if ((stackedWidget->currentIndex() == 0 && firstViewNewProject->isEnabled()) 
            || (stackedWidget->currentIndex() == 1 && fluxNewProject->isEnabled())) {
        if (new_projectname == "") {
            QString msg = "Invalid Project name";
            QMessageBox msgBox(mainwindow);
            msgBox.setWindowTitle("Error");
            msgBox.setText(msg);
            msgBox.exec();
            firstViewStatus->setText("");
            fluxStatus->setText("");
            emit uploadFinished(false);
            return;
        }
        QString new_project_id = _pollyIntegration->createProjectOnPolly(new_projectname);
        upload_project_id = new_project_id;    
    }
    else {
        QString msg = "Please select at least one option";
        QMessageBox msgBox(mainwindow);
        msgBox.setWindowTitle("Error");
        msgBox.setText(msg);
        msgBox.exec();
        firstViewStatus->setText("");
        fluxStatus->setText("");
        emit uploadFinished(false);
        return;
    }
    
    EPIWorkerThread *EPIworkerThread = new EPIWorkerThread();
    connect(EPIworkerThread, SIGNAL(filesUploaded(QStringList, QString, QString)), this, SLOT(postUpload(QStringList, QString, QString)));
    connect(EPIworkerThread, &EPIWorkerThread::finished, EPIworkerThread, &QObject::deleteLater);
    EPIworkerThread->state = "upload_files";
    EPIworkerThread->filesToUpload = filenames;
    EPIworkerThread->tmpDir = qdir;
    EPIworkerThread->uploadProjectIdThread = upload_project_id;
    EPIworkerThread->datetimestamp = datetimestamp;
    EPIworkerThread->start();
}

void PollyElmavenInterfaceDialog::postUpload(QStringList patchId, QString uploadProjectIdThread, QString datetimestamp){
    QCoreApplication::processEvents();
    
    if (!patchId.isEmpty()) {
        firstViewStatus->setText("");
        QString redirection_url = getRedirectionUrl(datetimestamp, uploadProjectIdThread);
        fluxStatus->setText("");
        qDebug() << "redirection_url     - " << redirection_url;
        pollyURL.setUrl(redirection_url);
        if (stackedWidget->currentIndex() == 0)
            pollyButton->setVisible(true);
        else
            fluxButton->setVisible(true);
    }
    else {
        if (stackedWidget->currentIndex() == 0) {
            firstViewStatus->setStyleSheet("QLabel {color : red; }");
            firstViewStatus->setText("Error!");
        }
        else {
            fluxStatus->setStyleSheet("QLabel {color: red; }");
            fluxStatus->setText("Error!");
        }
        QString msg = "Unable to send data";
        QMessageBox msgBox(mainwindow);
        msgBox.setWindowTitle("Warning!!");
        msgBox.setText(msg);
        msgBox.exec();
        firstViewStatus->setText("");
        fluxStatus->setText("");
    }
    emit uploadFinished(false);
}

QString PollyElmavenInterfaceDialog::getRedirectionUrl(QString datetimestamp, QString uploadProjectIdThread)
{
    if (stackedWidget->currentIndex() == 1) { 
        redirectTo = "relative_lcms_elmaven";
        QString CohortFileName = writableTempDir + QDir::separator() + datetimestamp + "_Cohort_Mapping_Elmaven.csv";
        if (!_pollyIntegration->validSampleCohort(CohortFileName))
            redirectTo = "gsheet_sym_polly_elmaven";
    } else redirectTo = "firstview";    
    
    QString redirection_url = QString("https://polly.elucidata.io/main#project=%1&auto-redirect=%2&elmavenTimestamp=%3").arg(uploadProjectIdThread).arg(redirectTo).arg(datetimestamp);
    return redirection_url;
}

QStringList PollyElmavenInterfaceDialog::prepareFilesToUpload(QDir qdir, QString datetimestamp)
{
    QString peak_table_name;
    if (stackedWidget->currentIndex() == 0)
        peak_table_name = firstViewTableList->currentText();
    else peak_table_name = fluxTableList->currentText();
    
    QStringList filenames;
    if (peak_table_name != "") {
        if (peak_table_name == "Bookmark Table") {
            _tableDockWidget = bookmarkedPeaks;
        }
        else{
            _tableDockWidget = peakTableNameMapping[peak_table_name];
        }

        if (_tableDockWidget->groupCount() == 0){
            QString msg = "Peaks Table is Empty";
            QMessageBox msgBox(mainwindow);
            msgBox.setWindowTitle("Error");
            msgBox.setText(msg);
            msgBox.exec();
            return filenames;
        }
    }
    else{
        QString msg = "No Peak tables";
        QMessageBox msgBox(mainwindow);
        msgBox.setWindowTitle("Error");
        msgBox.setText(msg);
        msgBox.exec();
        return filenames;
    }

    if (stackedWidget->currentIndex() == 0) {
        firstViewStatus->setStyleSheet("QLabel {color : green; }");
        firstViewStatus->setText("Preparing compound database file..");
    }
    else {
        fluxStatus->setStyleSheet("QLabel {color : green; }");
        fluxStatus->setText("Preparing compound database file..");
    }
    
    QCoreApplication::processEvents();

    if (advancedSettingsFlag){
        handle_advanced_settings(datetimestamp);
    }
    
    if (!advancedSettingsFlag || !_advancedSettings->getUploadCompoundDB()) {
        // Now uploading the Compound DB that was used for peak detection.
        // This is needed for Elmaven->Firstview->PollyPhi relative LCMS workflow.
        // ToDo Kailash, Keep track of compound DB used for each peak table,
        // As of now, uploading what is currently there in the compound section of Elmaven.
        // If advanced settings were used to upload compound DB, we need not do this.
        QString compoundDb = mainwindow->ligandWidget->getDatabaseName();
        mainwindow->ligandWidget->saveCompoundList(writableTempDir + QDir::separator() + datetimestamp + "_Compound_DB_Elmaven.csv", compoundDb);
    }

    qDebug()<< "Now uploading all groups, needed for firstview app..";
    _tableDockWidget->wholePeakSet();
    _tableDockWidget->treeWidget->selectAll();
    _tableDockWidget->prepareDataForPolly(writableTempDir,"Groups Summary Matrix Format Comma Delimited (*.csv)",datetimestamp+"_Peak_table_all");
    
    //Preparing the json file -
    if (stackedWidget->currentIndex() == 0) {
        firstViewStatus->setStyleSheet("QLabel {color : green; }");
        firstViewStatus->setText("Preparing json file..");
    }
    else {
        fluxStatus->setStyleSheet("QLabel {color : green; }");
        fluxStatus->setText("Preparing json file..");
    }
    QCoreApplication::processEvents();

    //Preparing the json file 
    QString json_filename = writableTempDir+QDir::separator()+datetimestamp+"_Peaks_information_json_Elmaven_Polly.json";//  uploading the json file
    _tableDockWidget->exportJsonToPolly(writableTempDir,json_filename);
    
    if (stackedWidget->currentIndex() == 1) {
        fluxStatus->setStyleSheet("QLabel {color : green; }");
        fluxStatus->setText("Preparing sample cohort file..");
        QCoreApplication::processEvents();
        //Preparing the sample cohort file
        QString sampleCohortFileName = writableTempDir + QDir::separator() + datetimestamp +
                                        "_Cohort_Mapping_Elmaven.csv";
        mainwindow->projectDockWidget->prepareSampleCohortFile(sampleCohortFileName);
    }
    
    //Saving settings file
    QByteArray ba = (writableTempDir + QDir::separator() + datetimestamp + "_maven_analysis_settings" + ".xml").toLatin1();
    const char *save_path = ba.data();
    mainwindow->mavenParameters->saveSettings(save_path);
    qDebug() << "settings saved now";
    
    qdir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList file_list = qdir.entryInfoList();
    
    for (int i = 0; i < file_list.size(); ++i){
        QFileInfo fileInfo = file_list.at(i);
        QString tmp_filename = writableTempDir+QDir::separator()+fileInfo.fileName();
        filenames.append(tmp_filename);
    }
    return filenames;
}

void PollyElmavenInterfaceDialog::handle_advanced_settings(QString datetimestamp){
        QVariantMap advanced_ui_elements = _advancedSettings->getUIElements();

        QString exportOption = advanced_ui_elements["export_option"].toString();
        QString exportFormat = advanced_ui_elements["export_format"].toString();
        QString userFilename = advanced_ui_elements["user_filename"].toString();
        QString compoundDb = advanced_ui_elements["compound_db"].toString();
        QString userCompoundDBName = advanced_ui_elements["user_compound_DB_name"].toString();
        
        if (userFilename==""){
            userFilename = "intensity_file.csv";
        }
        if (userCompoundDBName==""){
            userCompoundDBName = "compounds";
        }
        if (_advancedSettings->getUploadCompoundDB()){
            mainwindow->ligandWidget->saveCompoundList(writableTempDir + QDir::separator()
                                                        + datetimestamp
                                                        + "_Compound_DB_"
                                                        + userCompoundDBName
                                                        + ".csv", compoundDb );
        }
        
        if (_advancedSettings->getUploadPeakTable()){
            if (exportOption=="Export Selected"){
                _tableDockWidget->selectedPeakSet();
            }
            else if(exportOption=="Export Good"){
                _tableDockWidget->goodPeakSet();
                _tableDockWidget->treeWidget->selectAll();
            }
            else if(exportOption=="Export All Groups"){
                _tableDockWidget->wholePeakSet();
                _tableDockWidget->treeWidget->selectAll();
            }
            else if(exportOption=="Export Bad"){
                _tableDockWidget->badPeakSet();
                _tableDockWidget->treeWidget->selectAll();
            }
            _loadingDialog->statusLabel->setStyleSheet("QLabel {color : green; }");
            _loadingDialog->statusLabel->setText("Preparing intensity file..");
            QCoreApplication::processEvents();

            QString peakUserFilename  = datetimestamp + "_Peak_table_" + userFilename;
            qDebug() << "peakUserFilename - " << peakUserFilename;
            _tableDockWidget->prepareDataForPolly(writableTempDir, exportFormat, peakUserFilename);
        }

}

void PollyElmavenInterfaceDialog::cancel() {
    advancedSettingsFlag = false;
    close();   
}

void PollyElmavenInterfaceDialog::logout() {
    advancedSettingsFlag = false;
    _pollyIntegration->logout();
    credentials = QStringList();
    projectNamesId = QVariantMap();
    close();   
}

void PollyElmavenInterfaceDialog::performPostUploadTasks(bool uploadSuccessful) {
    firstViewUpload->setEnabled(true);
    fluxUpload->setEnabled(true);
}