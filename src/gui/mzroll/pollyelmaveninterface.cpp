#include "pollyelmaveninterface.h"
#include <string>
#include <QVariant>

PollyElmavenInterfaceDialog::PollyElmavenInterfaceDialog(MainWindow* mw) :
        QDialog(mw),
        mainwindow(mw),
        _loginform(nullptr)
{
        setupUi(this);
        setModal(true);
        _pollyIntegration = new PollyIntegration();
        connect(computeButton_upload, SIGNAL(clicked(bool)), SLOT(uploadDataToPolly()));
        connect(cancelButton_upload, SIGNAL(clicked(bool)), SLOT(cancel()));
        connect(pushButton_load, SIGNAL(clicked(bool)), SLOT(loadDataFromPolly()));
        connect(cancelButton_load, SIGNAL(clicked(bool)), SLOT(cancel()));
}

PollyElmavenInterfaceDialog::~PollyElmavenInterfaceDialog()
{
    qDebug()<<"exiting PollyElmavenInterfaceDialog now....";
    if(_loginform!=nullptr){
        delete _loginform;
    }
}

void PollyElmavenInterfaceDialog::initialSetup()
{
    int node_status = _pollyIntegration->check_node_executable();
    if (node_status==0){
        QMessageBox msgBox(NULL);
        msgBox.setWindowModality(Qt::NonModal);
        msgBox.setWindowTitle("node is not installed on this system");
        msgBox.show();
        return;
    }
    if (credentials.isEmpty()){
        call_login_form();
    }
    else{
        int status = _pollyIntegration->authenticate_login(credentials.at(0),credentials.at(1));
        loadFormData();
    }
}

void PollyElmavenInterfaceDialog::call_login_form(){
    _loginform =new LoginForm(this);
    _loginform->setModal(true);
    _loginform->show();
}

QVariantMap PollyElmavenInterfaceDialog::loadFormData(){
    QMessageBox msgBox(NULL);
    msgBox.setWindowModality(Qt::NonModal);
    msgBox.setWindowTitle("getting data from polly..");
    msgBox.show();
    comboBox_collaborators->clear();
    comboBox_load_projects->clear();
    comboBox_existing_projects->clear();
    progressBar_upload->setValue(0);
    progressBar_load_project->setValue(0);
    projectnames_id = _pollyIntegration->getUserProjects();    
    QStringList keys= projectnames_id.keys();

    userProjectFilesMap = _pollyIntegration->getUserProjectFiles(keys);
    for (int i=0; i < keys.size(); ++i){
        comboBox_existing_projects->addItem(projectnames_id[keys.at(i)].toString());
        comboBox_load_projects->addItem(projectnames_id[keys.at(i)].toString());
    }
    return projectnames_id;
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
        if (filename.split('.')[filename.split('.').size()-1]=="xml"){
            comboBox_load_settings->addItem(filename);
        }
    }
}

QString PollyElmavenInterfaceDialog::uploadDataToPolly()
{   if (credentials.isEmpty()){
        call_login_form();
        return "";
    }
    int status = _pollyIntegration->authenticate_login(credentials.at(0),credentials.at(1));
    QStringList patch_ids;
    QString upload_project_id;
    QString new_projectname = lineEdit_new_project_name->text();
    QString projectname = comboBox_existing_projects->currentText();
    QString project_id;
    QStringList filenames = prepareFilesToUpload();
    if (filenames.isEmpty()){
        QMessageBox msgBox(mainwindow);
        msgBox.setText("Unable to prepare files to upload..");
        msgBox.exec();
        return "";
    }
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

QStringList PollyElmavenInterfaceDialog::prepareFilesToUpload(){
    QStringList filenames;
    QList<QPointer<TableDockWidget> > peaksTableList = mainwindow->getPeakTableList();
    qDebug()<<"peaks table are here..";
    int n = peaksTableList.size();
    qDebug()<<"size  of list "<<n<<endl;
    if (n>0){
            _tableDockWidget = peaksTableList.at(n-1);
    }
    else{
        QString msg = "No Peak tables";
        QMessageBox msgBox(mainwindow);
        msgBox.setWindowTitle("Warning!!");
        msgBox.setText(msg);
        msgBox.exec();
        return filenames;
    }
    _tableDockWidget->wholePeakSet();
    _tableDockWidget->treeWidget->selectAll();
    QString writable_temp_dir =  QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::GenericConfigLocation) + QDir::separator() + "tmp_files";
    QDir qdir(writable_temp_dir);
    if (!qdir.exists()){
        QDir().mkdir(writable_temp_dir);
        QDir qdir(writable_temp_dir);
    }
    _tableDockWidget->exportGroupsToSpreadsheet_polly(writable_temp_dir+QDir::separator()+"export_all_groups.csv");
    QByteArray ba = (writable_temp_dir+QDir::separator()+"maven_analysis_settings.xml").toLatin1();
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
    int status = _pollyIntegration->authenticate_login(credentials.at(0),credentials.at(1));
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
    progressBar_load_project->setValue(100);
    QMessageBox msgBox(mainwindow);
    msgBox.setText(load_status);
    msgBox.exec();
}

void PollyElmavenInterfaceDialog::cancel() {
    close();   
}
