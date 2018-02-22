#include "pollyintegration.h"

PollyIntegration::PollyIntegration(TableDockWidget* tableDockWidget)
{

    _tableDockWidget = tableDockWidget;
    qDebug()<<"entering PollyIntegration now....";

    // connect(w, signal, this, slot())

}

PollyIntegration::~PollyIntegration()
{
    qDebug()<<"exiting PollyIntegration now....";
}

QByteArray PollyIntegration::run_qt_process(QString command){
    qDebug() << "command -   " << command;
    QProcess process;
    process.start(command);
    process.waitForFinished();
    QByteArray result = process.readAllStandardOutput();
    QByteArray result2 = process.readAllStandardError();
    qDebug() << "QByteArray std out result  " << result;
    qDebug() << "QByteArray std error result   " << result2;
    return result;
}

QByteArray PollyIntegration::run_system_process(QString command){
    qDebug() << "command inside run_system_process-   " << command;
    QByteArray ba = command.toLatin1();
    const char *c_str2 = ba.data();
    system(c_str2);
    return "";
}


QString PollyIntegration::get_run_id(QByteArray result){
    QList<QByteArray> test_list = result.split('\n');
    int size = test_list.size();
    QByteArray result2 = test_list[size-2];
    QJsonDocument doc(QJsonDocument::fromJson(result2));
    // Get JSON object
    QJsonObject json = doc.object();
    QVariantMap json_map = json.toVariantMap();
    QString run_id =  json_map["id"].toString();
    qDebug() << run_id;
    return run_id;
}

QString PollyIntegration::get_patch_id(QByteArray result){
    QList<QByteArray> test_list = result.split('\n');
    int size = test_list.size();
    QByteArray result2 = test_list[size-2];
    QString patch_id = QString("%1").arg(result2.toInt());
    return patch_id;
}

QStringList PollyIntegration::get_system_urls(QString filename){
    QStringList upload_commands ;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return upload_commands;
    QTextStream in(&file);
    QString line = in.readLine();
    while (!line.isNull()) {
        if (line.contains("file_upload_urls")){
            QByteArray byte_line = line.toLatin1();
            QJsonDocument doc(QJsonDocument::fromJson(byte_line));
            // Get JSON object
            QJsonObject json = doc.object();
            QVariantMap json_map = json.toVariantMap();
            QVariant url_json =  json_map["file_upload_urls"];
            QVariantMap url_map = url_json.toMap();
            QStringList keys= url_map.keys();
            for (int i = 0; i < keys.size(); ++i){
                QString key=keys.at(i).toLocal8Bit().constData();
                qDebug() <<"key - "<<key;
                QString url_map_json =  url_map[key].toList()[0].toString();
                qDebug() << "this is the url "<<url_map_json;
                QString key2= "test.csv";        
                // QString command= "mithoo createPutRequest %1 %2".arg(url_map_json).arg(key);
                QString command= QString("mithoo createPutRequest \"%1\" \"%2\"").arg(url_map_json).arg(key2);
                upload_commands.append(command);
            }
        }
        line = in.readLine();
    }
    return upload_commands;
}


QStringList PollyIntegration::get_project_upload_url_commands(QByteArray result2,QStringList filenames){
    QStringList upload_commands ;
    qDebug()<<"filenames inside get_project_upload_url_commands  "<<filenames;
    qDebug()<<"result2  "<<result2;

    QList<QByteArray> test_list = result2.split('\n');
    int size = test_list.size();
    QByteArray url_jsons = test_list[size-2];
    // qDebug()<<"url_jsons  "<<url_jsons;
    QJsonDocument doc(QJsonDocument::fromJson(url_jsons));
    // Get JSON object
    QJsonObject json = doc.object();
    QVariantMap json_map = json.toVariantMap();
    // QString url_with_wildcard =  json_map["file_upload_urls"].toString();
    for (int i=0; i < filenames.size(); ++i){
        QString filename = filenames.at(i);
        // QString random_filename = filename;
        
        QStringList test_files_list = filename.split('/');
        int size = test_files_list.size();
        QString new_filename = test_files_list[size-1];
        qDebug()<<"new_filename  "<<new_filename;
        QString url_with_wildcard =  json_map["file_upload_urls"].toString();
        qDebug()<<" url_with_wildcard  "<<url_with_wildcard;
        QString url_map_json = url_with_wildcard.replace("*",new_filename) ;
        qDebug()<<"url_map_json  "<<url_map_json;
        QString command= QString("mithoo upload_project_data \"%1\" \"%2\"").arg(url_map_json).arg(filename);
        upload_commands.append(command);
        
    }
    return upload_commands;
}

QString PollyIntegration::get_urls(QByteArray result){    
    QList<QByteArray> test_list = result.split('\n');
    int size = test_list.size();
    QByteArray result2 = test_list[size-2];
    QJsonDocument doc(QJsonDocument::fromJson(result2));
    QJsonObject json = doc.object();
    QVariantMap json_map = json.toVariantMap();
    QString url =  json_map["file_upload_urls"].toString();
    return url;
}



int PollyIntegration::check_already_logged_in(){
    int status;
    QString command = QString("mithoo authenticate");
    QByteArray result2 = run_qt_process(command);
    QList<QByteArray> test_list = result2.split('\n');
    QByteArray status_line = test_list[0];
    if (status_line=="already logged in"){
        status = 1;
    }
    else{
        status=0;
    }
    return status;
}


int PollyIntegration::authenticate_login(QString username,QString password){
    QString command = QString("mithoo authenticate %1 %2").arg(username).arg(password);
    QByteArray result = run_qt_process(command);
    int status_inside = check_already_logged_in();
    return status_inside;
}


void PollyIntegration::transferData(){

    int status = check_already_logged_in();
    if (status==0){
        qDebug()<<"not logged in....";
        login_user();
        // get_project_name();
    }
    else {
        qDebug("user is already logged in... sending data to polly now..");
        get_project_name();
        // exportData();
    }


}



void PollyIntegration::login_user(){
    LoginForm *w=new LoginForm(_tableDockWidget->_mainwindow,this);
    qDebug()<< "calling w.show now........";
    w->show();
    // connect(w,SIGNAL(),&loop,SLOT(quit()));
    
    // QEventLoop loop;
}

void PollyIntegration::get_project_name(){
    qDebug()<<"inside get_project_name now...";
    ProjectForm *popup=new ProjectForm(_tableDockWidget->_mainwindow,this);
    qDebug()<< "calling popup.show now........";
    popup->show();
    // connect(w,SIGNAL(),&loop,SLOT(quit()));
    
    // QEventLoop loop;
}

QVariantMap PollyIntegration::getUserProjectsMap(QByteArray result2){
    QVariantMap user_projects;
    QList<QByteArray> test_list = result2.split('\n');
    // qDebug()<<" test_list "<<test_list;
    int size = test_list.size();
    QByteArray result_jsons = test_list[size-2];
    // qDebug()<<" result_jsons "<<result_jsons;
    QJsonDocument doc(QJsonDocument::fromJson(result_jsons));
    // Get JSON object
    // qDebug()<<"is array - "<<doc.isArray();
    // qDebug()<<"is object - "<<doc.isObject();
    QJsonArray json_array = doc.array();
    for (int i=0; i < json_array.size(); ++i){
        QJsonValue project_json = json_array.at(i);
        QJsonObject project_json_object = project_json.toObject();
        // qDebug()<<"  project_json object"<<project_json.toObject();
        QVariantMap project_json_object_map = project_json_object.toVariantMap();
        user_projects[project_json_object_map["id"].toString()] = project_json_object_map["name"].toString();
    }
    // QJsonObject json = doc.object();
    // QVariantMap json_map = json.toVariantMap();
    // QString url_with_wildcard =  json_map["file_upload_urls"].toString()
    // qDebug()<<" json_map.keys() "<<json_map.keys();
    // QVariantMap user_projects;
    // user_projects["506"] = "new_get_project1";
    // user_projects["507"] = "new_get_project2";
    return user_projects;
}

QVariantMap PollyIntegration::getUserProjects(){
    // qDebug()<<"inside getUserProject() now...";
    QString get_projects_command = QString("mithoo get_Project_names");
    QByteArray result2 = run_qt_process(get_projects_command);
    QVariantMap user_projects = getUserProjectsMap(result2);
    return user_projects;
}

QString PollyIntegration::exportData(QString projectname,QString ProjectId) {
    QList<PeakGroup> allgroups =  _tableDockWidget->getAllGroups();
    
    if (allgroups.size() == 0 ) {
        QString msg = "Peaks Table is Empty, can't export to POLLY";
        QMessageBox::warning(_tableDockWidget, "Error", msg);
        return QString("None");
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
    QSettings* settings = _tableDockWidget->_mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    qDebug() << "going to write polly cli code here....\n\n";
    qDebug() << "valid credentials,sending data to polly now....\n\n";
    _tableDockWidget->_mainwindow->check_polly_login->setText("connected");
    _tableDockWidget->_mainwindow->check_polly_login->setStyleSheet("QLabel { background-color : white; color : green; }");
    QStringList filenames;
    // QString fileName = QFileDialog::getSaveFileName(this, tr("Save EICs to Polly "),dir,tr("*.json"));
    // qDebug()<<"fileName - "<<fileName;
    QDir qdir(dir+QString("/tmp_files/"));
    if (!qdir.exists()){
    qDebug()<<"making the temp dir";
     QDir().mkdir(dir+QString("/tmp_files"));   
     QDir qdir(dir+QString("/tmp_files/"));
    }
    qdir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList file_list = qdir.entryInfoList();
    for (int i = 0; i < file_list.size(); ++i){
        QFileInfo fileInfo = file_list.at(i);
        QString tmp_filename = dir+QString("/tmp_files/")+fileInfo.fileName();
        filenames.append(tmp_filename);

    }
    qDebug()<<"filenames   "<<filenames;
    // QDir().mkdir(dir+QString("/tmp_files"));
    QString jsonfileName = dir+QString("/tmp_files/export_json.json");
    if (jsonfileName.isEmpty()) return QString("None");
    if(!jsonfileName.endsWith(".json",Qt::CaseInsensitive)) jsonfileName = jsonfileName + ".json";
    saveJson * jsonSaveThread = new saveJson();
    jsonSaveThread->setMainwindow(_tableDockWidget->_mainwindow);
    jsonSaveThread->setPeakTable(_tableDockWidget);
    jsonSaveThread->setfileName(jsonfileName.toStdString());
    jsonSaveThread->start();
    filenames.append(jsonfileName);
    // QString command2 = QString("mithoo createRun %1 %2 %3 %4").arg("sampleqc_uploader").arg("22").arg("129").arg("exp_name");
    qDebug()<<"ProjectId    "<<ProjectId;
    QString run_id;
    if (ProjectId==""){
        QString command2 = QString("mithoo createProject %1").arg(projectname);
        QByteArray result1 = run_qt_process(command2);
        run_id = get_run_id(result1);
    }
    else{
        run_id = ProjectId;
    }
    // QString filename = "test_postrun.txt";
    // QString postrun_command = QString("mithoo postRun --file_name '{\"%1\":\"%2\",\"%3\" :\"%4\"}' --sub_component SampleQC --id %5 --\"file_type\" raw_input_files > %6").arg("sample_cohort").arg("sample-cohort.csv").arg("lcms_conv").arg("intensity.csv").arg(run_id).arg(filename);
    QString get_upload_Project_urls = QString("mithoo get_upload_Project_urls --id %1").arg(run_id);
    QByteArray result2 = run_qt_process(get_upload_Project_urls);
    QStringList upload_project_data_commands = get_project_upload_url_commands(result2,filenames);
    // run_system_process(postrun_command);
    // QStringList put_commands = get_system_urls(filename);
    qDebug()<<"upload_project_data_commands  "<<upload_project_data_commands;
    for (int i = 0; i < upload_project_data_commands.size(); ++i){
        QString command = upload_project_data_commands.at(i);    
        QByteArray patch_id_result = run_qt_process(command);
        // QString patch_id = get_patch_id(patch_id_result);
    }
    qDebug()<<"removing the tmp dir now..";
    
    qdir.removeRecursively();
    qDebug()<<"removed the tmp dir ..";

    // QString command5 = QString(" mithoo createPatchRequest %1").arg(run_id);
    // run_qt_process(command5);
    qDebug()<<" All 5 commands completed\n";
    return run_id;

}