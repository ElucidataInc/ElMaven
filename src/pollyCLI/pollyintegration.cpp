#include "pollyintegration.h"

PollyIntegration::PollyIntegration(): nodePath(""), jsPath(""),username(""),password("")
{
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
}

PollyIntegration::~PollyIntegration()
{
    qDebug()<<"exiting PollyIntegration now....";
}

 // name OF FUNCTION: run_qt_process
// PURPOSE:
//    This function uses Qprocess from Qt library to execute terminal commands from C++ and
// Returns its output as QByteArray
// CALLS TO: none
//
// CALLED FROM: multiple functions in this script

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
    QFile file (credFile);
    file.remove();
    QString command = "authenticate";
    QByteArray result = run_qt_process(command, QStringList() << credFile << username << password);
    int status_inside = check_already_logged_in();
    return status_inside;
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

QString PollyIntegration::createProjectOnPolly(QString projectname){
    QString command2 = "createProject";
    QByteArray result1 = run_qt_process(command2, QStringList() << credFile<< projectname);
    QString run_id = get_run_id(result1);
    return run_id;
}

QStringList PollyIntegration::exportData(QStringList filenames,QString projectId) {
    qDebug()<<"files to be uploaded   "<<filenames;
    QElapsedTimer timer;
    timer.start();
    QString get_upload_Project_urls = "get_upload_Project_urls";
    QByteArray result2 = run_qt_process(get_upload_Project_urls, QStringList() << credFile << projectId);
    QStringList patch_ids = get_project_upload_url_commands(result2,filenames);
    qDebug() << "time taken in uploading json file, by polly cli is - "<<timer.elapsed();
    return patch_ids;
}

QString PollyIntegration::loadDataFromPolly(QString ProjectId,QStringList filenames) {
    QString run_id;
    QString get_upload_Project_urls = "get_upload_Project_urls";
    QByteArray result2 = run_qt_process(get_upload_Project_urls, QStringList() << credFile << ProjectId);
    QStringList patch_ids = get_projectFiles_download_url_commands(result2,filenames);
    if (0<filenames.size()&&!patch_ids.isEmpty()){
        return "data_loaded";
    }
    else if(0<filenames.size()&&patch_ids.isEmpty()){
        return "error";
    }
    else{
        return "empty_filenames_list";
    }
}

