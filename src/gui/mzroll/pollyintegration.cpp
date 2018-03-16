#include "pollyintegration.h"
#include "tabledockwidget.h"

#include <QElapsedTimer>
#include <QStandardPaths>
#include <QDir>

PollyIntegration::PollyIntegration(MainWindow* mw,PollyElmavenInterfaceDialog* EPI): nodePath(""), jsPath("")
{
    qDebug()<<"inside PollyIntegration now..";
    _mainwindow = mw;
    _pollyelmaveninterfacedialog = EPI;
    qDebug()<<"initialized mainwindow ";
    _loginform = nullptr;
    _tableDockWidget = nullptr;
    
    qDebug()<<"crashed..";
    credFile = QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::GenericConfigLocation) + QDir::separator() + "cred_file";

    nodePath = QStandardPaths::findExecutable("node");
    jsPath = qApp->applicationDirPath() + QDir::separator() + "index.js";

    #ifdef Q_OS_WIN
      if(!QStandardPaths::findExecutable("node", QStringList() << qApp->applicationDirPath()).isEmpty())
        nodePath = qApp->applicationDirPath() + QDir::separator() + "node.exe";
    #endif

    #ifdef Q_OS_LINUX
      if(!QStandardPaths::findExecutable("node", QStringList() << qApp->applicationDirPath()).isEmpty())
          nodePath = qApp->applicationDirPath() + QDir::separator() + "node";
    #endif

    #ifdef Q_OS_MAC
      QString binDir = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator();
      if(!QStandardPaths::findExecutable("node", QStringList() << binDir + "node_bin" + QDir::separator() ).isEmpty())
        nodePath = binDir + "node_bin" + QDir::separator() + "node";

      jsPath = binDir  + "index.js";
    #endif
    qDebug()<<"PollyIntegration class initialised...";

}

PollyIntegration::~PollyIntegration()
{
    qDebug()<<"exiting PollyIntegration now....";
    if(_loginform!=nullptr){
        delete _loginform;
    }
}

QByteArray PollyIntegration::run_qt_process(QString command, QStringList args){

    // e.g: command = "authenticate", "get_Project_names" etc
    // e.g: args = username, password, projectName  etc
    QProcess process;
    QStringList arg;
    arg << jsPath; // where index.js files is placed
    arg << command; // what command to pass to index.js. eg. authenticate
    arg << args; // required params by that command . eg username, password

    // nodePath = "PATH_OF_MAVEN/bin/node.exe"
    process.setProgram(nodePath);
    process.setArguments(arg);

    qDebug () << "program: " <<  process.program()  << "args: " << process.arguments();

    process.start();
    process.waitForFinished(-1);
    QByteArray result = process.readAllStandardOutput();
    QByteArray result2 = process.readAllStandardError();

    qDebug() << "output: " << result << endl;
    qDebug()<< "error: " << result2 << endl;
    return result;
}

QByteArray PollyIntegration::run_system_process(QString command){
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
                QString url_map_json =  url_map[key].toList()[0].toString();
                QString key2= "test.csv";

                QString command= QString("%1 %2 createPutRequest \"%3\" \"%4\"").arg(nodePath).arg(jsPath).arg(url_map_json).arg(key2);
                upload_commands.append(command);
            }
        }
        line = in.readLine();
    }
    return upload_commands;
}


QStringList PollyIntegration::get_project_upload_url_commands(QByteArray result2,QStringList filenames){
    QStringList patch_ids ;
    QList<QByteArray> test_list = result2.split('\n');
    int size = test_list.size();
    QByteArray url_jsons = test_list[size-2];
    QJsonDocument doc(QJsonDocument::fromJson(url_jsons));
    // Get JSON object
    QJsonObject json = doc.object();
    QVariantMap json_map = json.toVariantMap();
    for (int i=0; i < filenames.size(); ++i){
        QString filename = filenames.at(i);
        QStringList test_files_list = filename.split('/');
        int size = test_files_list.size();
        QString new_filename = test_files_list[size-1];
        QString url_with_wildcard =  json_map["file_upload_urls"].toString();
        QString url_map_json = url_with_wildcard.replace("*",new_filename) ;
        QString upload_command = "upload_project_data";
        QByteArray patch_id_result = run_qt_process(upload_command,QStringList() <<url_map_json <<filename);
        patch_ids.append(patch_id_result);
    }
    return patch_ids;
}
QStringList PollyIntegration::get_projectFiles_download_url_commands(QByteArray result2,QStringList filenames){
    QStringList patch_ids ;
    QList<QByteArray> test_list = result2.split('\n');
    int size = test_list.size();
    QByteArray url_jsons = test_list[size-2];
    QJsonDocument doc(QJsonDocument::fromJson(url_jsons));
    // Get JSON object
    QJsonObject json = doc.object();
    QVariantMap json_map = json.toVariantMap();
    for (int i=0; i < filenames.size(); ++i){
        QString filename = filenames.at(i);
        QStringList test_files_list = filename.split('/');
        int size = test_files_list.size();
        QString new_filename = test_files_list[size-1];
        QString url_with_wildcard =  json_map["file_upload_urls"].toString();
        QString url_map_json = url_with_wildcard.replace("*",new_filename) ;
        QString upload_command = "download_project_data";
        QByteArray patch_id_result = run_qt_process(upload_command,QStringList() <<url_map_json <<filename);
        patch_ids.append(patch_id_result);
        qDebug()<<"going to load settings now..";
        qDebug()<<"new_filename.split('.')   "<<new_filename.split('.');
        if (new_filename.split('.')[new_filename.split('.').size()-1]=="xml"){
            _mainwindow->loadPollySettings(filename);
        }

    }
    return patch_ids;
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
    QString command = QString("authenticate");
    QByteArray result2 = run_qt_process(command, QStringList() << credFile);
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
    QString command = "authenticate";
    QByteArray result = run_qt_process(command, QStringList() << credFile << username << password);
    int status_inside = check_already_logged_in();
    return status_inside;
}


void PollyIntegration::transferData(){
    int status = check_already_logged_in();
    if (status==0){
        qDebug()<<"not logged in....";
        login_user();
    }
    else {
        qDebug("user is already logged in... sending data to polly now..");
        get_project_name();
    }
}



void PollyIntegration::login_user(){
    _loginform =new LoginForm(this);
    _loginform->setModal(true);
    _loginform->show();
}

void PollyIntegration::get_project_name(){
    // QProgressDialog progress("Getting data from Polly...", "Cancle", 0, 100, _mainwindow);
    //     qDebug()<<"QProgressDialog initialized...";
    //     progress.setMinimumDuration(0);
    //     progress.setWindowModality(Qt::WindowModal);
    //     for (int i = 0; i < 1; i++) {
    //         _pollyelmaveninterfacedialog->loadFormData();
    //         qDebug()<<"form data loaded....";
    //         // _pollyIntegration->transferData();
    //         if (progress.wasCanceled())
    //             break;
    //     }
    //     progress.setValue(100);
    _pollyelmaveninterfacedialog->loadFormData();
}

QVariantMap PollyIntegration::getUserProjectsMap(QByteArray result2){
    QVariantMap user_projects;
    QList<QByteArray> test_list = result2.split('\n');
    int size = test_list.size();
    QByteArray result_jsons = test_list[size-2];
    QJsonDocument doc(QJsonDocument::fromJson(result_jsons));
    // Get JSON object
    QJsonArray json_array = doc.array();
    for (int i=0; i < json_array.size(); ++i){
        QJsonValue project_json = json_array.at(i);
        QJsonObject project_json_object = project_json.toObject();
        QVariantMap project_json_object_map = project_json_object.toVariantMap();
        user_projects[project_json_object_map["id"].toString()] = project_json_object_map["name"].toString();
    }
    return user_projects;
}

QVariantMap PollyIntegration::getUserProjects(){
    QString get_projects_command = "get_Project_names";
    QByteArray result2 = run_qt_process(get_projects_command,QStringList() << credFile);
    QVariantMap user_projects = getUserProjectsMap(result2);
    return user_projects;
}

QStringList PollyIntegration::getUserProjectFilesMap(QByteArray result2){
    qDebug()<<"inside getUserProjectFilesMap....";
    QStringList user_projectfiles;
    QList<QByteArray> test_list = result2.split('\n');
    int size = test_list.size();
    QByteArray result_jsons = test_list[size-2];
    QJsonDocument doc(QJsonDocument::fromJson(result_jsons));
    // Get JSON object
    QJsonObject project_json_object = doc.object();
    QVariantMap project_json_object_map = project_json_object.toVariantMap();
    qDebug()<<"project files.."<<project_json_object_map["project_files"].toStringList();
    // user_projectfilesmap[project_json_object_map["id"].toString()] = project_json_object_map["name"].toString();
    user_projectfiles = project_json_object_map["project_files"].toStringList();

    // QJsonArray json_array = doc.array();
    // for (int i=0; i < json_array.size(); ++i){
    //     QJsonValue project_json = json_array.at(i);
    //     QJsonObject project_json_object = project_json.toObject();
    //     QVariantMap project_json_object_map = project_json_object.toVariantMap();
    //     qDebug()<<"project files.."<<project_json_object_map["project_files"].toStringList();
    //     // user_projectfilesmap[project_json_object_map["id"].toString()] = project_json_object_map["name"].toString();
    //     user_projectfiles = project_json_object_map["project_files"].toStringList();
    // }
    return user_projectfiles;
}

QVariantMap PollyIntegration::getUserProjectFiles(QStringList ProjectIds){
    QVariantMap user_projectfilesmap;
    for (int i=0; i < ProjectIds.size(); ++i){
        QString ProjectId = ProjectIds.at(i);
        QString get_projects_command = "get_Project_files";
        QByteArray result2 = run_qt_process(get_projects_command,QStringList() << credFile<<ProjectId);
        QStringList user_projectfiles = getUserProjectFilesMap(result2);
        user_projectfilesmap[ProjectId] = user_projectfiles;
    }
    return user_projectfilesmap;
}

QString PollyIntegration::exportData(QString projectname,QString ProjectId) {
    qDebug()<<"inside exportData now..";
    QList<QPointer<TableDockWidget> > peaksTableList = _mainwindow->getPeakTableList();
    qDebug()<<"peaks table are here..";
    int n = peaksTableList.size();
    qDebug()<<"size  of list "<<n<<endl;
    if (n>0){
            _tableDockWidget = peaksTableList.at(n-1);
    }
    else{
        return QString("nullptr");
    }
    QList<PeakGroup> allgroups =  _tableDockWidget->getAllGroups();

    if (allgroups.size() == 0 ) {
        QString msg = "Peaks Table is Empty, can't export to POLLY";
        QMessageBox::warning(_tableDockWidget, "Error", msg);
        return QString("None");
    }
    qDebug()<<"everything clear... trying to upload to polly now..";
    /**
     * copy all groups from <allgroups> to <vallgroups> which is used by
     * < libmaven/jsonReports.cpp>
    */
    _tableDockWidget->vallgroups.clear();
    for(int i=0;i<allgroups.size();++i){
        _tableDockWidget->vallgroups.push_back(allgroups[i]);
    }

    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    qDebug() << "going to write polly cli code here....\n\n";
    qDebug() << "valid credentials,sending data to polly now....\n\n";
    _mainwindow->check_polly_login->setText("connected");
    _mainwindow->check_polly_login->setStyleSheet("QLabel { background-color : white; color : green; }");
    
    _tableDockWidget->wholePeakSet();
    _tableDockWidget->treeWidget->selectAll();
    _tableDockWidget->exportGroupsToSpreadsheet_polly();
    
    QStringList filenames;
    QDir qdir(dir+QString("/tmp_files/"));
    if (!qdir.exists()){
        QDir().mkdir(dir+QString("/tmp_files"));
        QDir qdir(dir+QString("/tmp_files/"));
    }

    QByteArray ba = (dir+QString("/tmp_files/maven_analysis_settings.xml")).toLatin1();
    const char *save_path = ba.data();
    _mainwindow->mavenParameters->saveSettings(save_path);
    qdir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList file_list = qdir.entryInfoList();
    // qDebug()<<"files in temp dir..."<<file_list<<endl;
    for (int i = 0; i < file_list.size(); ++i){
        QFileInfo fileInfo = file_list.at(i);
        QString tmp_filename = dir+QString("/tmp_files/")+fileInfo.fileName();
        filenames.append(tmp_filename);

    }
    qDebug()<<"filenames   "<<filenames;
    // QString jsonfileName = dir+QString("/tmp_files/export_json.json");
    // if (jsonfileName.isEmpty()) return QString("None");
    // if(!jsonfileName.endsWith(".json",Qt::CaseInsensitive)) jsonfileName = jsonfileName + ".json";
    // saveJson * jsonSaveThread = new saveJson();
    // jsonSaveThread->setMainwindow(_mainwindow);
    // jsonSaveThread->setPeakTable(_tableDockWidget);
    // jsonSaveThread->setfileName(jsonfileName.toStdString());
    // jsonSaveThread->start();
    QElapsedTimer timer;
    timer.start();
    // while(jsonSaveThread->isRunning()){
    //     ;
    // }
    // qDebug() << "time taken in writing json file, by Elmaven is - "<<timer.elapsed();
    // filenames.append(jsonfileName);
    QString run_id;
    if (ProjectId==""){
        QString command2 = "createProject";
        QByteArray result1 = run_qt_process(command2, QStringList() << credFile<< projectname);
        run_id = get_run_id(result1);
    }
    else{
        run_id = ProjectId;
    }
    timer.start();
    QString get_upload_Project_urls = "get_upload_Project_urls";
    QByteArray result2 = run_qt_process(get_upload_Project_urls, QStringList() << credFile << run_id);
    QStringList patch_ids = get_project_upload_url_commands(result2,filenames);
    qDebug() << "time taken in uploading json file, by polly cli is - "<<timer.elapsed();
    // qdir.removeRecursively();
    return run_id;

}


QString PollyIntegration::loadDataFromPolly(QString ProjectId,QStringList filenames) {
    
    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    qDebug() << "going to write polly cli code here....\n\n";
    qDebug() << "valid credentials,loading data from polly now....\n\n";
    
    QDir qdir(dir+QString("/tmp_files/"));
    if (!qdir.exists()){
        QDir().mkdir(dir+QString("/tmp_files"));
        QDir qdir(dir+QString("/tmp_files/"));
    }
    
    QElapsedTimer timer;
    timer.start();
    QStringList full_path_filenames;
    for (int i = 0; i < filenames.size(); ++i){
        QString tmp_filename = dir+QString("/tmp_files/")+filenames.at(i);;
        full_path_filenames.append(tmp_filename);
    }

    QString run_id;
    QString get_upload_Project_urls = "get_upload_Project_urls";
    QByteArray result2 = run_qt_process(get_upload_Project_urls, QStringList() << credFile << ProjectId);
    QStringList patch_ids = get_projectFiles_download_url_commands(result2,full_path_filenames);
    qDebug() << "time taken in loading data by polly cli is - "<<timer.elapsed();
    // qdir.removeRecursively();
    return run_id;

}

