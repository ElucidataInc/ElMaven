#include "pollyelmaveninterface.h"
#include <string>
#include <QVariant>

PollyElmavenInterfaceDialog::PollyElmavenInterfaceDialog(MainWindow* mw) :
        QDialog(mw),
        mainwindow(mw)
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
    QString storeCredFile = QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::GenericConfigLocation) + QDir::separator() + "store_cred_file.txt";
    QStringList credentials = readFromFile(storeCredFile);
    if (credentials.isEmpty()){
        call_login_form();
    }
    else{
        int status = _pollyIntegration->authenticate_login(credentials.at(0),credentials.at(1));
        if (status!=1){
            call_login_form();
        }
        else{
            loadFormData();
        }
    }
}

void PollyElmavenInterfaceDialog::call_login_form(){
    _loginform =new LoginForm(this);
    _loginform->setModal(true);
    _loginform->show();
}

QStringList PollyElmavenInterfaceDialog::readFromFile(QString fileName){
    QStringList credentials;
    QFile file(fileName);
    if(!file.exists()){
        qDebug() << "credentials not stored..asking the user to log in now.  ";
    }
    else{
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream stream(&file);
            while (!stream.atEnd()){
                QString line = stream.readLine();
                qDebug() << "line is- "<<line;
                credentials<<line;
            }
        }
    }
    file.close();
    return credentials;
}

QVariantMap PollyElmavenInterfaceDialog::loadFormData(){
    QMessageBox msgBox(NULL);
    msgBox.setWindowModality(Qt::NonModal);
    msgBox.setWindowTitle("getting data from polly..");
    msgBox.show();
    mainwindow->check_polly_login->setText("connected");
    mainwindow->check_polly_login->setStyleSheet("QLabel { background-color : white; color : green; }");
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
    comboBox_collaborators->addItem("Aman");
    comboBox_collaborators->addItem("Sabu");
    comboBox_collaborators->addItem("Swetabh");
    comboBox_collaborators->addItem("Nikita");
    comboBox_collaborators->addItem("Sahil");
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
        qDebug()<<"filename  "<<filename;
        qDebug()<<"filename.split('.')[filename.split('.').size()-1]   - "<<filename.split('.')[filename.split('.').size()-1];
        if (filename.split('.')[filename.split('.').size()-1]=="xml"){
            comboBox_load_settings->addItem(filename);
        }
        else{
            comboBox_load_db->addItem(filename);
        }
    }
}

QString PollyElmavenInterfaceDialog::uploadDataToPolly()
{   
    QStringList patch_ids;
    QVariantMap projectnames_id = _pollyIntegration->getUserProjects();
    qDebug()<<"inside uploadDataToPolly.....uploading data to polly now.."<<endl;
    QString new_project_id;
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
        }
    else{
        QString new_project_id = _pollyIntegration->createProjectOnPolly(new_projectname);
        patch_ids  = _pollyIntegration->exportData(filenames,new_project_id);       
    }
    qDebug()<<"result of exportdata...."<<patch_ids;
    progressBar_upload->setValue(100);
    
    if (!patch_ids.isEmpty()){
        QString redirection_url = QString("<a href='https://polly.elucidata.io/main#project=%1&auto-redirect=firstview'>Go To Polly</a>").arg(new_project_id);
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
        QMessageBox::warning(_tableDockWidget, "Error", msg);
        return filenames;
    }
    QList<PeakGroup> allgroups =  _tableDockWidget->getAllGroups();

    if (allgroups.size() == 0 ) {
        QString msg = "Peaks Table is Empty, can't export to POLLY";
        QMessageBox::warning(_tableDockWidget, "Error", msg);
        return filenames;
    }
    /**
     * copy all groups from <allgroups> to <vallgroups> which is used by
     * < libmaven/jsonReports.cpp>
    */
    _tableDockWidget->vallgroups.clear();
    for(int i=0;i<allgroups.size();++i){
        _tableDockWidget->vallgroups.push_back(allgroups[i]);
    }

    QString dir = ".";
    QSettings* settings = mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();
    _tableDockWidget->wholePeakSet();
    _tableDockWidget->treeWidget->selectAll();
    _tableDockWidget->exportGroupsToSpreadsheet_polly();
    
    QDir qdir(dir+QString("/tmp_files/"));
    if (!qdir.exists()){
        QDir().mkdir(dir+QString("/tmp_files"));
        QDir qdir(dir+QString("/tmp_files/"));
    }

    QByteArray ba = (dir+QString("/tmp_files/maven_analysis_settings.xml")).toLatin1();
    const char *save_path = ba.data();
    mainwindow->mavenParameters->saveSettings(save_path);
    qdir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList file_list = qdir.entryInfoList();
    
    for (int i = 0; i < file_list.size(); ++i){
        QFileInfo fileInfo = file_list.at(i);
        QString tmp_filename = dir+QString("/tmp_files/")+fileInfo.fileName();
        filenames.append(tmp_filename);
    }
    return filenames;
}

void PollyElmavenInterfaceDialog::loadDataFromPolly()
{
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
    QString dir = ".";
    QSettings* settings = mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();
    qDebug() << "valid credentials,loading data from polly now....\n\n";
    
    QDir qdir(dir+QString("/tmp_files/"));
    if (!qdir.exists()){
        QDir().mkdir(dir+QString("/tmp_files"));
        QDir qdir(dir+QString("/tmp_files/"));
    }
    QStringList full_path_filenames;
    for (int i = 0; i < filenames.size(); ++i){
        QString tmp_filename = dir+QString("/tmp_files/")+filenames.at(i);;
        full_path_filenames.append(tmp_filename);
    }
    QString load_status = _pollyIntegration->loadDataFromPolly(ProjectId,full_path_filenames);
    if (load_status=="project data loaded"){
        mainwindow->loadPollySettings(dir+QString("/tmp_files/")+settings_file);
    }
    progressBar_load_project->setValue(100);
    QMessageBox msgBox(mainwindow);
    msgBox.setText(load_status);
    msgBox.exec();
}

void PollyElmavenInterfaceDialog::cancel() {
    LOGD;
    close();   
}
