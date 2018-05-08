#include "pollyelmaveninterface.h"
#include <string>
#include <QVariant>

PollyElmavenInterfaceDialog::PollyElmavenInterfaceDialog(MainWindow* mw) :
        QDialog(mw),
        mainwindow(mw),
        _loginform(NULL)
{
        setupUi(this);
        setModal(true);
        
        _pollyIntegration = new PollyIntegration();
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
        QString status_inside = _pollyIntegration->authenticate_login(credentials.at(0),credentials.at(1));
        QMessageBox msgBox(NULL);
        msgBox.setWindowModality(Qt::NonModal);
        msgBox.setWindowTitle("fetching data from polly..");
        msgBox.show();
        startup_data_load();
    }
}

void PollyElmavenInterfaceDialog::call_login_form(){
    _loginform =new LoginForm(this);
    _loginform->setModal(true);
    _loginform->show();
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
    QString status_inside = _pollyIntegration->authenticate_login(credentials.at(0),credentials.at(1));
    QMessageBox msgBox(NULL);
    msgBox.setWindowModality(Qt::NonModal);
    msgBox.setWindowTitle("getting data from polly..");
    msgBox.show();
    progressBar_load_project->setValue(0);
    QStringList keys= projectnames_id.keys();

    QIcon project_icon(rsrcPath + "/POLLY.png");
    
    userProjectFilesMap = _pollyIntegration->getUserProjectFiles(keys);
    for (int i=0; i < keys.size(); ++i){
        comboBox_load_projects->addItem(project_icon,projectnames_id[keys.at(i)].toString());
    }

    QString organization = credentials.at(0).split('@')[1];
    QStringList organizationSpecificCompoundDB = _pollyIntegration->getOrganizationalDBs(organization);
    for (int i=0; i < organizationSpecificCompoundDB.size(); ++i){
        comboBox_organization_DBs->addItem(project_icon,organizationSpecificCompoundDB.at(i));
    }
    comboBox_organization_DBs->addItem(project_icon,"sampledata");
    cancelButton_load->setEnabled(true);
    pushButton_load->setEnabled(true);
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

    QString writable_temp_dir =  QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::GenericConfigLocation) + QDir::separator() + "tmp_files";
    qDebug()<<"writable_temp_dir -"<<writable_temp_dir;
    QDir qdir(writable_temp_dir);
    if (!qdir.exists()){
        QDir().mkdir(writable_temp_dir);
        QDir qdir(writable_temp_dir);
    }

    QStringList filenames = prepareFilesToUpload(qdir);
    if (filenames.isEmpty()){
        QMessageBox msgBox(mainwindow);
        msgBox.setText("Unable to prepare files to upload..");
        msgBox.exec();
        return "";
    }
    //re-login to polly may be required because the token expires after 30 minutes..
    QString status_inside = _pollyIntegration->authenticate_login(credentials.at(0),credentials.at(1));
    
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

    if (!patch_ids.isEmpty()){
        QString redirection_url = QString("<a href='https://polly.elucidata.io/main#project=%1&auto-redirect=firstview'>Go To Polly</a>").arg(upload_project_id);
        qDebug()<<"redirection_url     - "<<redirection_url;
        QMessageBox msgBox(mainwindow);
        msgBox.setWindowTitle("Redirecting to polly..");
        msgBox.setTextFormat(Qt::RichText);   //this is what makes the links clickable
        msgBox.setText(redirection_url);
        msgBox.exec();
        return "";
    }
    else{
        QMessageBox msgBox(mainwindow);
        msgBox.setText("Unable to upload data.");
        msgBox.exec();
        return "";
    }
}

QStringList PollyElmavenInterfaceDialog::prepareFilesToUpload(QDir qdir){

    QDateTime current_time;
    QString datetimestamp= current_time.currentDateTime().toString();
    datetimestamp.replace(" ","_");
    qDebug()<<"datetime -"<< datetimestamp;

    QString writable_temp_dir =  QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::GenericConfigLocation) + QDir::separator() + "tmp_files";
    QString peak_table_name = comboBox_table_name->currentText();
    QString export_option = comboBox_export_table->currentText();
    QString export_format = comboBox_export_format->currentText();
    QString user_filename = lineEdit_filename->text();
    QString compound_db = comboBox_compound_db->currentText();
    QString user_compound_DB_name = lineEdit_compound_DB_name->text();
    
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

    
    mainwindow->ligandWidget->saveCompoundListToPolly(writable_temp_dir+QDir::separator()+datetimestamp+"_Compound_DB_"+user_compound_DB_name,compound_db);    


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
    //Now uploading all groups, needed for firstview app..
    _tableDockWidget->wholePeakSet();
    _tableDockWidget->treeWidget->selectAll();
    _tableDockWidget->prepareDataForPolly(writable_temp_dir,"Groups Summary Matrix Format Comma Delimited (*.csv)",datetimestamp+"_Peak_table_all_" +user_filename);
    QByteArray ba = (writable_temp_dir+QDir::separator()+datetimestamp+"_"+user_filename.split('.')[user_filename.split('.').size()-1]+"_maven_analysis_settings"+".xml").toLatin1();
    const char *save_path = ba.data();
    mainwindow->mavenParameters->saveSettings(save_path);
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
    QString settings_file = comboBox_load_settings->currentText();
    filenames.append(db_file);
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
    
    QString writable_temp_dir =  QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::GenericConfigLocation) + QDir::separator() + "tmp_files";
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
    QString load_status = _pollyIntegration->loadDataFromPolly(ProjectId,full_path_filenames);
    if (load_status=="project data loaded"){
        mainwindow->loadPollySettings(writable_temp_dir+QDir::separator()+settings_file);
    }
    progressBar_load_project->setRange(0, 100);
    progressBar_load_project->setValue(100);
    QMessageBox msgBox(mainwindow);
    msgBox.setText(load_status);
    msgBox.exec();
}

void PollyElmavenInterfaceDialog::cancel() {
    close();   
}

void PollyElmavenInterfaceDialog::logout() {
    _pollyIntegration->logout();
    credentials = QStringList();
    close();   
}