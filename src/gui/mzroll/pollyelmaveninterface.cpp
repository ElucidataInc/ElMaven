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
        
        connect(pollyButton, SIGNAL(clicked(bool)), SLOT(goToPolly()));
        //connect(checkBox_advanced_settings,SIGNAL(clicked(bool)),SLOT(showAdvanceSettings()));
        connect(computeButton_upload, SIGNAL(clicked(bool)), SLOT(uploadDataToPolly()));
        connect(cancelButton_upload, SIGNAL(clicked(bool)), SLOT(cancel()));
        connect(new_project_radio_button, SIGNAL(clicked(bool)), SLOT(handle_new_project_radio_button()));
        connect(existing_project_radio_button, SIGNAL(clicked(bool)), SLOT(handle_existing_project_radio_button()));
        connect(logout_upload, SIGNAL(clicked(bool)), SLOT(logout()));
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
        QString status_inside = _pollyintegration->authenticate_login("","");
        emit authentication_result(status_inside);
        if (status_inside=="ok"){
            QVariantMap projectnames_id = _pollyintegration->getUserProjects();
            emit resultReady(projectnames_id);
        }
    }
    else if (state=="upload_files"){
        qDebug()<<"starting thread for uploading files to polly..";
        //re-login to polly may be required because the token expires after 30 minutes..
        QString status_inside = _pollyintegration->authenticate_login("","");
        QStringList patch_ids  = _pollyintegration->exportData(filesToUpload,upload_project_id_thread);
        bool status = tmpDir.removeRecursively();
        emit filesUploaded(patch_ids, upload_project_id_thread, datetimestamp);
    }
}

EPIWorkerThread::~EPIWorkerThread()
{
    if (_pollyintegration) delete (_pollyintegration);
};

void PollyElmavenInterfaceDialog::createIcons()
{   
    QListWidgetItem *firstView = new QListWidgetItem(workflowMenu);
    firstView->setIcon(QIcon(":/images/firstView.png"));
    firstView->setText(tr("FirstView"));
    firstView->setTextAlignment(Qt::AlignHCenter);
    firstView->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *fluxomics = new QListWidgetItem(workflowMenu);
    fluxomics->setIcon(QIcon(":/images/flux.png"));
    fluxomics->setText(tr("PollyPhi Relative LCMS"));
    fluxomics->setTextAlignment(Qt::AlignHCenter);
    fluxomics->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

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

void PollyElmavenInterfaceDialog::handle_new_project_radio_button(){
    lineEdit_new_project_name->setEnabled(true);
    comboBox_existing_projects->setEnabled(false);
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::handle_existing_project_radio_button(){
    lineEdit_new_project_name->setEnabled(false);
    comboBox_existing_projects->setEnabled(true);
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::showAdvanceSettings(){
    use_advanced_settings = "yes";
    _advancedSettings = new AdvancedSettings();
    _advancedSettings->initialSetup();
}

void PollyElmavenInterfaceDialog::initialSetup()
{   
    int node_status = _pollyIntegration->check_node_executable();
    if (node_status==0){
        QMessageBox msgBox(this);
        msgBox.setWindowModality(Qt::NonModal);
        msgBox.setWindowTitle("node is not installed on this system");
        msgBox.exec();
        return;
    }
    int askForLogin = _pollyIntegration->askForLogin();
    if (askForLogin==1){
        try{
            call_login_form();
            } catch(...) {
                QMessageBox msgBox(this);
                msgBox.setWindowModality(Qt::NonModal);
                msgBox.setWindowTitle("couldn't load login form");
                msgBox.exec();
            }
        return;
    }
    else{
        try{
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

void PollyElmavenInterfaceDialog::call_login_form(){
    _loginform =new LoginForm(this);
    _loginform->setModal(true);
    _loginform->show();
}

void PollyElmavenInterfaceDialog::call_initial_EPI_form(){
    computeButton_upload->setEnabled(false);
    comboBox_existing_projects->clear();
    
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

void PollyElmavenInterfaceDialog::handleAuthentication(QString status){
    if (status=="ok"){
        _loadingDialog->statusLabel->setText("Fetching your projects..");
        QCoreApplication::processEvents();
    }
    else {
            _loadingDialog->statusLabel->setText("Authentication failed. Please login again.");
            QCoreApplication::processEvents();
            logout();
    }
}

void PollyElmavenInterfaceDialog::handleResults(QVariantMap projectnames_id_map){
    projectnames_id = projectnames_id_map;
    startup_data_load();
}

void PollyElmavenInterfaceDialog::startup_data_load()
{
    if (stackedWidget->currentIndex() == 0)
        firstView_startup_data_load();
}

QVariantMap PollyElmavenInterfaceDialog::firstView_startup_data_load()
{
    pollyButton->setVisible(false);
    lineEdit_new_project_name->setEnabled(true);
    comboBox_existing_projects->setEnabled(false);
    new_project_radio_button->setChecked(true);
    existing_project_radio_button->setChecked(false);
    comboBox_table_name->clear();
    comboBox_existing_projects->clear();
    QCoreApplication::processEvents();
    
    if (projectnames_id.isEmpty()){
        projectnames_id = _pollyIntegration->getUserProjects();
    }    
    QStringList keys= projectnames_id.keys();

    QIcon project_icon(rsrcPath + "/POLLY.png");
    for (int i=0; i < keys.size(); ++i){
        comboBox_existing_projects->addItem(project_icon,projectnames_id[keys.at(i)].toString());
    }
    QList<QPointer<TableDockWidget> > peaksTableList = mainwindow->getPeakTableList();
    bookmarkedPeaks = mainwindow->getBookmarkedPeaks();
    if(!bookmarkedPeaks->getGroups().isEmpty()){
        bookmarkTableNameMapping[QString("Bookmark Table")]=bookmarkedPeaks;
        comboBox_table_name->addItem("Bookmark Table");
    } 
    int n = peaksTableList.size();
    if (n>0){
        for (int i=0; i < n; ++i){
            QString peak_table_name = QString("Peak Table ")+QString::number(i+1);
            peakTableNameMapping[peak_table_name]=peaksTableList.at(i);
            comboBox_table_name->addItem(peak_table_name);
        }        
    }
    computeButton_upload->setEnabled(true);
    _loadingDialog->close();
    QCoreApplication::processEvents();
    
    return projectnames_id;
}

void PollyElmavenInterfaceDialog::uploadDataToPolly()
{

    computeButton_upload->setEnabled(false);
    int askForLogin = _pollyIntegration->askForLogin();
    if (askForLogin == 1){
        call_login_form();
        emit uploadFinished(false);
        return;
    }
    
    QStringList patch_ids;

    QString new_projectname = lineEdit_new_project_name->text();
    QString projectname = comboBox_existing_projects->currentText();
    QString project_id;

    QString writable_temp_dir =  QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::GenericConfigLocation) + QDir::separator() + "tmp_Elmaven_Polly_files";
    qDebug()<<"writing Polly temp file in this directory -"<<writable_temp_dir;
    QDir qdir(writable_temp_dir);
    if (!qdir.exists()){
        QDir().mkdir(writable_temp_dir);
        QDir qdir(writable_temp_dir);
    }
    upload_status->setStyleSheet("QLabel {color : green; }");
    upload_status->setText("Preparing files..");
    QCoreApplication::processEvents();
    QDateTime current_time;
    QString datetimestamp = current_time.currentDateTime().toString();
    datetimestamp.replace(" ","_");
    datetimestamp.replace(":","-");
    
    QStringList filenames = prepareFilesToUpload(qdir, datetimestamp);
    if (filenames.isEmpty()) {
        upload_status->setText("File preparation failed.");
        _loadingDialog->close();
        QCoreApplication::processEvents();
        emit uploadFinished(false);
        return;
    }
    upload_status->setText("Connecting..");
    QCoreApplication::processEvents();

    upload_status->setText("Sending files to Polly..");
    QCoreApplication::processEvents();
    if (comboBox_existing_projects->isEnabled()){
        QStringList keys= projectnames_id.keys();
        for (int i=0; i < keys.size(); ++i){
            if (projectnames_id[keys.at(i)].toString()==projectname){
                project_id= keys.at(i);
            }
        }
        if (project_id!=""){
            upload_project_id = project_id;
        }
        else{
            QString msg = "No such project on Polly..";
            QMessageBox msgBox(mainwindow);
            msgBox.setWindowTitle("Error");
            msgBox.setText(msg);
            msgBox.exec();
        }
        }
    else if (lineEdit_new_project_name->isEnabled()){
        if (new_projectname==""){
            QString msg = "Invalid Project name";
            QMessageBox msgBox(mainwindow);
            msgBox.setWindowTitle("Error");
            msgBox.setText(msg);
            msgBox.exec();
            upload_status->setText("");
            emit uploadFinished(false);
            return;
        }
        QString new_project_id = _pollyIntegration->createProjectOnPolly(new_projectname);
        upload_project_id = new_project_id;    
    }
    else{
        QString msg = "Please select at least one option";
        QMessageBox msgBox(mainwindow);
        msgBox.setWindowTitle("Error");
        msgBox.setText(msg);
        msgBox.exec();
        upload_status->setText("");
        emit uploadFinished(false);
        return;
    }
    
    EPIWorkerThread *EPIworkerThread = new EPIWorkerThread();
    connect(EPIworkerThread, SIGNAL(filesUploaded(QStringList, QString, QString)), this, SLOT(postUpload(QStringList, QString, QString)));
    connect(EPIworkerThread, &EPIWorkerThread::finished, EPIworkerThread, &QObject::deleteLater);
    EPIworkerThread->state = "upload_files";
    EPIworkerThread->filesToUpload = filenames;
    EPIworkerThread->tmpDir = qdir;
    EPIworkerThread->upload_project_id_thread = upload_project_id;
    EPIworkerThread->datetimestamp = datetimestamp;
    EPIworkerThread->start();
}

void PollyElmavenInterfaceDialog::postUpload(QStringList patch_ids, QString upload_project_id_thread, QString datetimestamp){
    QCoreApplication::processEvents();
    if (!patch_ids.isEmpty()) {
        upload_status->setText("");
        QString redirection_url = QString("https://polly.elucidata.io/main#project=%1&auto-redirect=firstview&elmavenTimestamp=%2").arg(upload_project_id_thread).arg(datetimestamp);
        qDebug() << "redirection_url     - " << redirection_url;
        pollyURL.setUrl(redirection_url);
        pollyButton->setVisible(true);
    }
    else {
        upload_status->setStyleSheet("QLabel {color : red; }");
        upload_status->setText("Error!");
        QString msg = "Unable to send data";
        QMessageBox msgBox(mainwindow);
        msgBox.setWindowTitle("Warning!!");
        msgBox.setText(msg);
        msgBox.exec();
        upload_status->setText("");
    }
    emit uploadFinished(false);   
}

QStringList PollyElmavenInterfaceDialog::prepareFilesToUpload(QDir qdir, QString datetimestamp) {
    
    QString writable_temp_dir =  QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::GenericConfigLocation) + QDir::separator() + "tmp_Elmaven_Polly_files";
    QString peak_table_name = comboBox_table_name->currentText();
    QStringList filenames;
    if (peak_table_name!=""){
        if(peak_table_name=="Bookmark Table"){
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
    _loadingDialog->statusLabel->setStyleSheet("QLabel {color : green; }");
    _loadingDialog->statusLabel->setText("Preparing compound database file..");
    QCoreApplication::processEvents();

    if (use_advanced_settings=="yes"){
        handle_advanced_settings(writable_temp_dir,datetimestamp);
    }
    
    if (use_advanced_settings != "yes" || !_advancedSettings->get_upload_compoundDB()) {
        // Now uploading the Compound DB that was used for peak detection.
        // This is needed for Elmaven->Firstview->PollyPhi relative LCMS workflow.
        // ToDo Kailash, Keep track of compound DB used for each peak table,
        // As of now, uploading what is currently there in the compound section of Elmaven.
        // If advanced settings were used to upload compound DB, we need not do this.
        QString compound_db = mainwindow->ligandWidget->getDatabaseName();
        mainwindow->ligandWidget->saveCompoundList(writable_temp_dir+QDir::separator()+datetimestamp+"_Compound_DB_Elmaven.csv",compound_db);
    }

    qDebug()<< "Now uploading all groups, needed for firstview app..";
    _tableDockWidget->wholePeakSet();
    _tableDockWidget->treeWidget->selectAll();
    _tableDockWidget->prepareDataForPolly(writable_temp_dir,"Groups Summary Matrix Format Comma Delimited (*.csv)",datetimestamp+"_Peak_table_all");
    // Now uploading the json file -
    _loadingDialog->statusLabel->setStyleSheet("QLabel {color : green; }");
    _loadingDialog->statusLabel->setText("Preparing json file..");
    QCoreApplication::processEvents();
    // Uploading the json file here - 
    QString json_filename = writable_temp_dir+QDir::separator()+datetimestamp+"_Peaks_information_json_Elmaven_Polly.json";//  uploading the json file
    _tableDockWidget->exportJsonToPolly(writable_temp_dir,json_filename);
    QByteArray ba = (writable_temp_dir+QDir::separator()+datetimestamp+"_maven_analysis_settings"+".xml").toLatin1();
    const char *save_path = ba.data();
    mainwindow->mavenParameters->saveSettings(save_path);
    qDebug()<<"settings saved now..";
    qdir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList file_list = qdir.entryInfoList();
    
    for (int i = 0; i < file_list.size(); ++i){
        QFileInfo fileInfo = file_list.at(i);
        QString tmp_filename = writable_temp_dir+QDir::separator()+fileInfo.fileName();
        filenames.append(tmp_filename);
    }
    return filenames;
}

void PollyElmavenInterfaceDialog::handle_advanced_settings(QString writable_temp_dir,QString datetimestamp){
        QVariantMap advanced_ui_elements = _advancedSettings->get_ui_elements();

        QString export_option = advanced_ui_elements["export_option"].toString();
        QString export_format = advanced_ui_elements["export_format"].toString();
        QString user_filename = advanced_ui_elements["user_filename"].toString();
        QString compound_db = advanced_ui_elements["compound_db"].toString();
        QString user_compound_DB_name = advanced_ui_elements["user_compound_DB_name"].toString();
        
        if (user_filename==""){
            user_filename = "intensity_file.csv";
        }
        if (user_compound_DB_name==""){
            user_compound_DB_name = "compounds";
        }
        if (_advancedSettings->get_upload_compoundDB()){
            mainwindow->ligandWidget->saveCompoundList(writable_temp_dir+QDir::separator()+datetimestamp+"_Compound_DB_"+user_compound_DB_name+".csv",compound_db);
        }
        
        if (_advancedSettings->get_upload_Peak_Table()){
            if (export_option=="Export Selected"){
                _tableDockWidget->selectedPeakSet();
            }
            else if(export_option=="Export Good"){
                _tableDockWidget->goodPeakSet();
                _tableDockWidget->treeWidget->selectAll();
            }
            else if(export_option=="Export All Groups"){
                _tableDockWidget->wholePeakSet();
                _tableDockWidget->treeWidget->selectAll();
            }
            else if(export_option=="Export Bad"){
                _tableDockWidget->badPeakSet();
                _tableDockWidget->treeWidget->selectAll();
            }
            _loadingDialog->statusLabel->setStyleSheet("QLabel {color : green; }");
            _loadingDialog->statusLabel->setText("Preparing intensity file..");
            QCoreApplication::processEvents();

            QString peak_user_filename  = datetimestamp+"_Peak_table_" +user_filename;
            qDebug()<<"peak_user_filename - "<<peak_user_filename;
            _tableDockWidget->prepareDataForPolly(writable_temp_dir,export_format,peak_user_filename);
        }

}

void PollyElmavenInterfaceDialog::cancel() {
    use_advanced_settings = "no";
    close();   
}

void PollyElmavenInterfaceDialog::logout() {
    use_advanced_settings = "no";
    _pollyIntegration->logout();
    credentials = QStringList();
    projectnames_id = QVariantMap();
    close();   
}

void PollyElmavenInterfaceDialog::performPostUploadTasks(bool uploadSuccessful) {
    computeButton_upload->setEnabled(true);
}