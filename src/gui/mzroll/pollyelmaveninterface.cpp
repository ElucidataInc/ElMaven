#include "pollyelmaveninterface.h"
#include <string>
#include <QVariant>

PollyElmavenInterfaceDialog::PollyElmavenInterfaceDialog(MainWindow* mw) :
        QDialog(mw),
        mainwindow(mw),
        _loginform(NULL),
        _initialEPIform(NULL)
{
        setupUi(this);
        setModal(true);
        upload_peaks_frame->hide();
        upload_compound_DB_frame->hide();
        label_create_new_project->hide();
        lineEdit_new_project_name->hide();
        label_peaks_table->hide();
        comboBox_table_name->hide();
        checkBox_upload_compond_DB->hide();
        personal_project_frame->hide();
        groupBox_load_status->hide();
    
        _pollyIntegration = new PollyIntegration();
        connect(checkBox_advanced_settings,SIGNAL(clicked(bool)),SLOT(showAdvanceSettings()));
        connect(checkBox_upload_compond_DB,SIGNAL(clicked(bool)),SLOT(showCompoundDBUploadFrame()));
        
        connect(computeButton_upload, SIGNAL(clicked(bool)), SLOT(uploadDataToPolly()));
        connect(cancelButton_upload, SIGNAL(clicked(bool)), SLOT(cancel()));
        connect(pushButton_load, SIGNAL(clicked(bool)), SLOT(loadDataFromPolly()));
        connect(load_form_data_button, SIGNAL(clicked(bool)), SLOT(loadFormData()));
        connect(cancelButton_load, SIGNAL(clicked(bool)), SLOT(cancel()));
        connect(logout_upload, SIGNAL(clicked(bool)), SLOT(logout()));
        connect(logout_load, SIGNAL(clicked(bool)), SLOT(logout()));
        connect(add_collab_button, SIGNAL(clicked(bool)), SLOT(AddCollaborator()));
               
}
 
PollyElmavenInterfaceDialog::~PollyElmavenInterfaceDialog()
{
    qDebug()<<"exiting PollyElmavenInterfaceDialog now....";
    if (_loginform) delete (_loginform);
    if (_initialEPIform) delete (_initialEPIform);
}

EPIWorkerThread::EPIWorkerThread()
{
    // _pollyintegration = new PollyIntegration();   
    
};

void EPIWorkerThread::run(){
    // QString status_inside = _pollyintegration->authenticate_login(username,password);
    // emit resultReady(QStringList()<<status_inside<<username<<password);
}

EPIWorkerThread::~EPIWorkerThread()
{
    // if (_pollyintegration) delete (_pollyintegration);
};


void PollyElmavenInterfaceDialog::on_comboBox_existing_projects_activated(const QString &arg1){
    QString projectname = comboBox_existing_projects->currentText();
    if (projectname=="Add new"){
        label_create_new_project->show();
        lineEdit_new_project_name->show();
        label_peaks_table->show();
        comboBox_table_name->show();
        QCoreApplication::processEvents();
    
    }
    else if (projectname=="Select project"){
        label_create_new_project->hide();
        lineEdit_new_project_name->hide();
        label_peaks_table->hide();
        comboBox_table_name->hide();
        QCoreApplication::processEvents();
    
    }
    else {
        label_create_new_project->hide();
        lineEdit_new_project_name->hide();
        label_peaks_table->show();
        comboBox_table_name->show();
        QCoreApplication::processEvents();
    
    }
}

void PollyElmavenInterfaceDialog::showAdvanceSettings(){
    if (upload_peaks_frame->isVisible()){
        upload_peaks_frame->hide();
        upload_compound_DB_frame->hide();
        checkBox_upload_compond_DB->hide();
    }
    else{
        upload_peaks_frame->show();
        upload_compound_DB_frame->show();
        checkBox_upload_compond_DB->show();
    }  
}

void PollyElmavenInterfaceDialog::showCompoundDBUploadFrame(){
    if (upload_compound_DB_frame->isEnabled()){
        upload_compound_DB_frame->setEnabled(false); 
    }
    else{
       upload_compound_DB_frame->setEnabled(true); 
    }  
}

void PollyElmavenInterfaceDialog::initialSetup()
{   
    int node_status = _pollyIntegration->check_node_executable();
    if (node_status==0){
        QMessageBox msgBox(NULL);
        msgBox.setWindowModality(Qt::NonModal);
        msgBox.setWindowTitle("node is not installed on this system");
        msgBox.exec();
        return;
    }
    if (credentials.isEmpty()){
        call_login_form();
        return;
    }
    else{
        call_initial_EPI_form();
        return;
    }
}

void PollyElmavenInterfaceDialog::call_login_form(){
    _loginform =new LoginForm(this);
    _loginform->setModal(true);
    _loginform->show();
}

void PollyElmavenInterfaceDialog::call_initial_EPI_form(){
    // _initialEPIform =new InitialEPIForm(this);
    // _initialEPIform->setModal(true);
    // _initialEPIform->show();
}

void PollyElmavenInterfaceDialog::populate_comboBox_compound_db() {
    comboBox_compound_db->clear();
	QSet<QString>set;
	for(int i=0; i< DB.compoundsDB.size(); i++) {
        if (! set.contains( DB.compoundsDB[i]->db.c_str() ) )
            set.insert( DB.compoundsDB[i]->db.c_str() );
	}
    QIcon icon(rsrcPath + "/dbsearch.png");
    QSetIterator<QString> i(set);
    int pos=0;
	while (i.hasNext()) { 
        comboBox_compound_db->addItem(icon,i.next());
	}
}

QVariantMap PollyElmavenInterfaceDialog::startup_data_load(){
    label_welcome_load->setStyleSheet("QLabel {color : green; }");
    label_welcome_load->setText("Welcome back "+credentials.at(0));
    comboBox_collaborators->clear();
    comboBox_table_name->clear();
    comboBox_export_table->clear();
    comboBox_export_format->clear();
    comboBox_load_projects->clear();
    comboBox_load_settings->clear();
    comboBox_load_db->clear();
    comboBox_compound_db->clear();
    comboBox_existing_projects->clear();
    progressBar_upload->setValue(0);
    progressBar_load_project->setValue(0);
    projectnames_id = _pollyIntegration->getUserProjects();    
    QStringList keys= projectnames_id.keys();

    comboBox_export_table->addItems(QStringList()<<"Export Selected"<<"Export All Groups"<<"Export Good"<<"Export Bad");

    QString groupsSTAB = "Groups Summary Matrix Format With Set Names (*.tab)";
    QString groupsTAB = "Groups Summary Matrix Format (*.tab)";    
    QString peaksTAB =  "Peaks Detailed Format (*.tab)";
    QString groupsSCSV = "Groups Summary Matrix Format Comma Delimited With Set Names (*.csv)";
    QString groupsCSV = "Groups Summary Matrix Format Comma Delimited (*.csv)";
    QString peaksCSV =  "Peaks Detailed Format Comma Delimited (*.csv)";    
    QString peaksListQE= "Inclusion List QE (*.csv)";
    QString mascotMGF=   "Mascot Format MS2 Scans (*.mgf)";

    comboBox_export_format->addItems(QStringList()<<groupsSTAB<<groupsTAB<<peaksTAB<<groupsSCSV<<groupsCSV<<peaksCSV<<peaksListQE<<mascotMGF);
    QIcon project_icon(rsrcPath + "/POLLY.png");
    comboBox_existing_projects->addItem("Select project");
    comboBox_existing_projects->addItem("Add new");
    for (int i=0; i < keys.size(); ++i){
        comboBox_existing_projects->addItem(project_icon,projectnames_id[keys.at(i)].toString());
    }
    QList<QPointer<TableDockWidget> > peaksTableList = mainwindow->getPeakTableList();
    bookmarkedPeaks = mainwindow->getBookmarkedPeaks();
    if(!bookmarkedPeaks->getGroups().isEmpty()){
        bookmarkTableNameMapping[QString("Bookmark Table")]=bookmarkedPeaks;
        comboBox_table_name->addItem("Bookmark Table");
    } 
    qDebug()<<"peaks table are here..";
    int n = peaksTableList.size();
    qDebug()<<"size  of list "<<n<<endl;
    if (n>0){
        for (int i=0; i < n; ++i){
            QString peak_table_name = QString("Peak Table ")+QString::number(i+1);
            peakTableNameMapping[peak_table_name]=peaksTableList.at(i);
            comboBox_table_name->addItem(peak_table_name);
        }        
    }
    qDebug()<<"DB.compoundsDB.size()  - "<<DB.compoundsDB.size();
    populate_comboBox_compound_db();
    this->show();
    return projectnames_id;
}

void PollyElmavenInterfaceDialog::loadFormData(){
    if (credentials.isEmpty()){
        call_login_form();
        return;
    }
    load_form_data_button->setEnabled(false);
    label_load_status->setStyleSheet("QLabel {color : green; }");
    label_load_status->setText("Connecting to polly..");
    QCoreApplication::processEvents();
    QString status_inside = _pollyIntegration->authenticate_login(credentials.at(0),credentials.at(1));
    // QMessageBox msgBox(NULL);
    // msgBox.setWindowModality(Qt::NonModal);
    // msgBox.setWindowTitle("getting data from polly..");
    // msgBox.show();
    label_load_status->setText("getting data from polly..");
    QCoreApplication::processEvents();
    progressBar_load_project->setValue(0);
    QStringList keys= projectnames_id.keys();

    QIcon project_icon(rsrcPath + "/POLLY.png");
    
    userProjectFilesMap = _pollyIntegration->getUserProjectFiles(keys);
    qDebug()<<"userProjectFilesMap - "<<userProjectFilesMap;
    for (int i=0; i < keys.size(); ++i){
        comboBox_load_projects->addItem(project_icon,projectnames_id[keys.at(i)].toString());
    }

    QString organization = credentials.at(0).split('@')[1];
    QStringList organizationSpecificCompoundDB = _pollyIntegration->getOrganizationalDBs(organization);
    organizationSpecificCompoundDB.append("Lung_Cancer_Experiment_compound_Database.csv");
    organizationSpecificCompoundDB.append("SRM3_hyperinsulinaemic_compound_Database.csv");
    organizationSpecificCompoundDB.append("Tongue_Cancer_Experiment_Elucidata.csv");
    organizationSpecificCompoundDB.append("Phi_analyis_diabetes_experiment.csv");
    for (int i=0; i < organizationSpecificCompoundDB.size(); ++i){
        comboBox_organization_DBs->addItem(project_icon,organizationSpecificCompoundDB.at(i));
    }
    label_load_status->setText("data fetched.");
    personal_project_frame->show();
    groupBox_load_status->show();
    load_form_data_button->setEnabled(true);
    QCoreApplication::processEvents();
    // comboBox_organization_DBs->addItem(project_icon,"sampledata");
    // cancelButton_load->setEnabled(true);
    // pushButton_load->setEnabled(true);
}

void PollyElmavenInterfaceDialog::on_comboBox_load_projects_activated(const QString &arg1)
{
    comboBox_load_db->clear();
    comboBox_load_settings->clear();
    QStringList keys= projectnames_id.keys();
    QString projectname = comboBox_load_projects->currentText();
    QString ProjectId;
    for (int i=0; i < keys.size(); ++i){
        if (projectnames_id[keys.at(i)].toString()==projectname){
            ProjectId= keys.at(i);
        }
    }
    QStringList userProjectFiles= userProjectFilesMap[ProjectId].toStringList();
    
    for (int i=0; i < userProjectFiles.size(); ++i){
        QString filename = userProjectFiles.at(i);
        if ((filename.split('.')[filename.split('.').size()-1]=="xml") && filename.contains("_maven_analysis_settings", Qt::CaseInsensitive)){
            comboBox_load_settings->addItem(filename);
        }
        else if (((filename.split('.')[filename.split('.').size()-1]=="csv") || (filename.split('.')[filename.split('.').size()-1]=="tab")) && filename.contains("Compound_DB_", Qt::CaseInsensitive)){
            comboBox_load_db->addItem(filename);
        }
    }
}

void PollyElmavenInterfaceDialog::AddCollaborator(){
    QString collaborator_name = comboBox_collaborators->currentText();
    QString access_level = comboBox_collaborators_access_level->currentText();
    collaborators_map[collaborator_name]=access_level;
}

QString PollyElmavenInterfaceDialog::uploadDataToPolly()
{   

    if (credentials.isEmpty()){
        call_login_form();
        return "";
    }
    QStringList patch_ids;
    QString upload_project_id;

    QString new_projectname = lineEdit_new_project_name->text();
    QString projectname = comboBox_existing_projects->currentText();
    QString project_id;

    QString writable_temp_dir =  QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::GenericConfigLocation) + QDir::separator() + "tmp_Elmaven_Polly_files";
    qDebug()<<"writable_temp_dir -"<<writable_temp_dir;
    QDir qdir(writable_temp_dir);
    if (!qdir.exists()){
        QDir().mkdir(writable_temp_dir);
        QDir qdir(writable_temp_dir);
    }
    label_upload_status->setStyleSheet("QLabel {color : green; }");
    label_upload_status->setText("Preparig files to upload..");
    QCoreApplication::processEvents();
    QStringList filenames = prepareFilesToUpload(qdir);
    if (filenames.isEmpty()){
        label_upload_status->setText("Unable to prepare files to upload..");
        QCoreApplication::processEvents();
        return "";
    }
    label_upload_status->setText("Connecting to Polly now..");
    QCoreApplication::processEvents();
    //re-login to polly may be required because the token expires after 30 minutes..
    QString status_inside = _pollyIntegration->authenticate_login(credentials.at(0),credentials.at(1));
    label_upload_status->setText("Uploading to Polly now..Please wait..");
    QCoreApplication::processEvents();
    if (new_projectname==""){
        QStringList keys= projectnames_id.keys();
        for (int i=0; i < keys.size(); ++i){
            if (projectnames_id[keys.at(i)].toString()==projectname){
                project_id= keys.at(i);
            }
        }
            patch_ids = _pollyIntegration->exportData(filenames,project_id);
            upload_project_id = project_id;
        }
    else{
        QString new_project_id = _pollyIntegration->createProjectOnPolly(new_projectname);
        patch_ids  = _pollyIntegration->exportData(filenames,new_project_id);   
        upload_project_id = new_project_id;    
    }
    progressBar_upload->setValue(100);
    bool status = qdir.removeRecursively();
    QCoreApplication::processEvents();
    if (!patch_ids.isEmpty()){
        QString redirection_url = QString("<a href='https://polly.elucidata.io/main#project=%1&auto-redirect=firstview'>Go To Polly</a>").arg(upload_project_id);
        qDebug()<<"redirection_url     - "<<redirection_url;
        label_upload_status->setText(redirection_url);
        label_upload_status->setTextFormat(Qt::RichText);
        label_upload_status->setTextInteractionFlags(Qt::TextBrowserInteraction);
        label_upload_status->setOpenExternalLinks(true);
        QCoreApplication::processEvents();
        return "";
    }
    else{
        label_upload_status->setText("Unable to upload data.");
        return "";
    }
}

QStringList PollyElmavenInterfaceDialog::prepareFilesToUpload(QDir qdir){

    QDateTime current_time;
    QString datetimestamp= current_time.currentDateTime().toString();
    datetimestamp.replace(" ","_");
    datetimestamp.replace(":","-");
    
    QString writable_temp_dir =  QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::GenericConfigLocation) + QDir::separator() + "tmp_Elmaven_Polly_files";
    QString peak_table_name = comboBox_table_name->currentText();
    QString export_option = comboBox_export_table->currentText();
    QString export_format = comboBox_export_format->currentText();
    QString user_filename = lineEdit_filename->text();
    QString compound_db = comboBox_compound_db->currentText();
    QString user_compound_DB_name = lineEdit_compound_DB_name->text();
    
    if (user_filename==""){
        user_filename = "intensity_file.csv";
    }
    if (user_compound_DB_name==""){
        user_compound_DB_name = "compounds.csv";
    }
    QStringList filenames;
    if (peak_table_name!=""){
        if(peak_table_name=="Bookmark Table"){
            _tableDockWidget = bookmarkedPeaks;
        }
        else{
            _tableDockWidget = peakTableNameMapping[peak_table_name];
        }
    }
    else{
        QString msg = "No Peak tables";
        QMessageBox msgBox(mainwindow);
        msgBox.setWindowTitle("Warning!!");
        msgBox.setText(msg);
        msgBox.exec();
        return filenames;
    }

    if (comboBox_compound_db->isEnabled()){
        mainwindow->ligandWidget->saveCompoundListToPolly(writable_temp_dir+QDir::separator()+datetimestamp+"_Compound_DB_"+user_compound_DB_name,compound_db);
    }
    // Now uploading the Compound DB that was used for peakdetection..this is needed for Elmaven->Firstview->PollyPhi relative LCMS workflow..
    mainwindow->ligandWidget->saveCompoundListToPolly(writable_temp_dir+QDir::separator()+datetimestamp+"_Compound_DB_Elmaven_"+user_compound_DB_name,compound_db);
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

    QString peak_user_filename  = datetimestamp+"_Peak_table_" +user_filename;
    qDebug()<<"peak_user_filename - "<<peak_user_filename;
    _tableDockWidget->prepareDataForPolly(writable_temp_dir,export_format,peak_user_filename);
    qDebug()<< "Now uploading all groups, needed for firstview app..";
    _tableDockWidget->wholePeakSet();
    _tableDockWidget->treeWidget->selectAll();
    qDebug()<<"running _tableDockWidget->prepareDataForPolly now...";
    _tableDockWidget->prepareDataForPolly(writable_temp_dir,"Groups Summary Matrix Format Comma Delimited (*.csv)",datetimestamp+"_Peak_table_all_" +user_filename);
    qDebug()<<"done..";
    QByteArray ba = (writable_temp_dir+QDir::separator()+datetimestamp+"_"+user_filename.split('.')[user_filename.split('.').size()-1]+"_maven_analysis_settings"+".xml").toLatin1();
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

void PollyElmavenInterfaceDialog::loadDataFromPolly()
{
    if (credentials.isEmpty()){
        call_login_form();
        return;
    }
    QString status_inside = _pollyIntegration->authenticate_login(credentials.at(0),credentials.at(1));
    QStringList filenames;
    QString db_file = comboBox_load_db->currentText();
    QString org_db_file = comboBox_organization_DBs->currentText();
    QString settings_file = comboBox_load_settings->currentText();
    filenames.append(org_db_file);
    filenames.append(settings_file);
    QStringList keys= projectnames_id.keys();
    QString projectname = comboBox_load_projects->currentText();
    QString ProjectId;
    for (int i=0; i < keys.size(); ++i){
        if (projectnames_id[keys.at(i)].toString()==projectname){
            ProjectId= keys.at(i);
        }
    }
    qDebug() << "valid credentials,loading data from polly now....\n\n";
    
    QString writable_temp_dir =  QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::GenericConfigLocation) + QDir::separator() + "tmp_Elmaven_Polly_files";
    QDir qdir(writable_temp_dir);
    if (!qdir.exists()){
        QDir().mkdir(writable_temp_dir);
        QDir qdir(writable_temp_dir);
    }
    
    QStringList full_path_filenames;
    for (int i = 0; i < filenames.size(); ++i){
        QString tmp_filename = writable_temp_dir+QDir::separator()+filenames.at(i);;
        full_path_filenames.append(tmp_filename);
    }
    label_load_status->setText("Loading data in Elmaven..Please wait.");
    QCoreApplication::processEvents();
    QString load_status = _pollyIntegration->loadDataFromPolly(ProjectId,full_path_filenames);
    if (load_status=="project data loaded"){
        mainwindow->loadPollySettings(writable_temp_dir+QDir::separator()+settings_file);
        qDebug()<<"trying to load compound DB from Polly inside Elmaven now..";
        mainwindow->loadCompoundsFile(writable_temp_dir+QDir::separator()+org_db_file);
    }
    progressBar_load_project->setRange(0, 100);
    progressBar_load_project->setValue(100);
    bool status = qdir.removeRecursively();
    label_load_status->setText(load_status);
    QCoreApplication::processEvents();
    // QMessageBox msgBox(mainwindow);
    // msgBox.setText(load_status);
    // msgBox.exec();
}

void PollyElmavenInterfaceDialog::cancel() {
    close();   
}

void PollyElmavenInterfaceDialog::logout() {
    _pollyIntegration->logout();
    credentials = QStringList();
    close();   
}