#include "pollyintegration.h"

PollyIntegration::PollyIntegration(): nodePath(""), jsPath("")
{
    credFile = QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::GenericConfigLocation) + QDir::separator() + "cred_file";

    // nodePath = QStandardPaths::findExecutable("node");
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
QList<QByteArray> PollyIntegration::run_qt_process(QString command, QStringList args){

    // e.g: command = "authenticate", "get_Project_names" etc
    // e.g: args = username, password, projectName  etc
    QProcess process;
    QStringList arg;
    arg << jsPath; // where index.js files is placed
    arg << command; // what command to pass to index.js. eg. authenticate
    arg << args; // required params by that command . eg username, password
    // qDebug()<<"nodePath"<<nodePath;
    // qDebug()<<"args -"<<arg;
    // nodePath = "PATH_OF_MAVEN/bin/node.exe"
    process.setProgram(nodePath);
    process.setArguments(arg);

    process.start();
    //TODO kailash, use threading for this, it should not just run indefinitely 
    process.waitForFinished(-1);
    QByteArray result = process.readAllStandardOutput();
    QByteArray result2 = process.readAllStandardError();
    qDebug()<<"StandardOutput  - "<<result;
    qDebug()<<"StandardError, if any  - "<<result2;
    return QList<QByteArray>()<<result<<result2;
}

// name OF FUNCTION: get_run_id
// PURPOSE:
//    This function parses the output of "createproject" command returned by run_qt_process
// this command is used to create a new project on polly
// this function Returns project id for the new project..
// CALLS TO: none
//
// CALLED FROM: createProjectOnPolly


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

// name OF FUNCTION: get_project_upload_url_commands
// PURPOSE:
//    This function is responsible for uploading given files to polly.
// It will take the upload urls as an input.
// For every file in filenames, it will first modify the url a bit..
// and then it will call "upload_project_data" command for that url,
// This command simply does a put request for that url with the help of index.js and node..

// CALLS TO: run_qt_process
//
// CALLED FROM: exportData


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
        QStringList test_files_list = filename.split(QDir::separator());
        int size = test_files_list.size();
        QString new_filename = test_files_list[size-1];
        QString url_with_wildcard =  json_map["file_upload_urls"].toString();
        QString url_map_json = url_with_wildcard.replace("*",new_filename) ;
        QString upload_command = "upload_project_data";
        QList<QByteArray> patch_id_result_and_error = run_qt_process(upload_command,QStringList() <<url_map_json <<filename);
        patch_ids.append(patch_id_result_and_error.at(0));
    }
    return patch_ids;
}

// name OF FUNCTION: get_projectFiles_download_url_commands
// PURPOSE:
//    This function parses the output of "createproject" command run from Qtprocess..
// Return project id for the new project..
// CALLS TO: run_qt_process
//
// CALLED FROM: loadDataFromPolly


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
        QStringList test_files_list = filename.split(QDir::separator());
        int size = test_files_list.size();
        QString new_filename = test_files_list[size-1];
        QString url_with_wildcard =  json_map["file_upload_urls"].toString();
        QString url_map_json = url_with_wildcard.replace("*",new_filename) ;
        QString upload_command = "download_project_data";
        QList<QByteArray> result_and_error = run_qt_process(upload_command,QStringList() <<url_map_json <<filename);
        patch_ids.append(result_and_error.at(0));
    }
    return patch_ids;
}

// name OF FUNCTION: check_already_logged_in
// PURPOSE:
//    This function checks if the user is already logged in or not.
// Returns log in status of current user

// CALLS TO: run_qt_process
//
// CALLED FROM: authenticate_login

int PollyIntegration::check_already_logged_in(){
    int status;
    QString command = QString("authenticate");
    QList<QByteArray> result_and_error = run_qt_process(command, QStringList() << credFile);
    QList<QByteArray> test_list = result_and_error.at(0).split('\n');
    QByteArray status_line = test_list[0];
    if (status_line=="already logged in"){
        status = 1;
    }
    else{
        status=0;
    }
    return status;
}

// name OF FUNCTION: authenticate_login
// PURPOSE:
//    This function is responsible for authenticating the user.
// This is the entry point for Elmaven-Polly-Integration
//It will first run the command "authenticate" and then check its ouptut to see if log in was succesfull
// username and password are inputed from external clients
// Returns status of log in commands, if successfully logged in, it will return 1, else 0
// CALLS TO: run_qt_process,check_already_logged_in
//
// CALLED FROM: external clients


QString PollyIntegration::authenticate_login(QString username,QString password){
    QString command = "authenticate";
    QString status;
    QList<QByteArray> result_and_error = run_qt_process(command, QStringList() << credFile << username << password);
    int status_inside = check_already_logged_in();
    if (status_inside==1){
        status="ok";
    }
    else if (result_and_error.at(1)!=""){
        status="error";
    }
    else {
        status = "incorrect credentials";
    }
    return status;
}

// This function checks if node executable path has been defined for the library or not..
int PollyIntegration::check_node_executable(){
    if (nodePath==""){
        return 0;
    }
    return 1;
    
}
// This function deletes the token and logs out the user..
void PollyIntegration::logout(){
    QFile file (credFile);
    file.remove();
}
// name OF FUNCTION: getUserProjectsMap
// PURPOSE:
//    This function parses the output of "get_Project_names" command and store it in a json format to be used later..
// When the user selects a project on Elmaven GUI, this json will be used to get ID for that project
// This ID will then further be used for uploading and all..

// Returns user projects in json format with id as keys and name as values..
// CALLS TO: 
//
// CALLED FROM: getUserProjects

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

// name OF FUNCTION: getUserProjects
// PURPOSE:
//    This function runs "get_Project_names" command from Qtprocess..
// This command will return data corresponding to all the projects on polly for the logged in user..
// This function will then call getUserProjectsMap to parse the output and store it in a json format to be used later..
// When the user selects a project on Elmaven GUI, this json will be used to get ID for that project
// This ID will then further be used for uploading and all..

// Returns user projects in json format with id as keys and name as values..
// CALLS TO: getUserProjects
//
// CALLED FROM: 


QVariantMap PollyIntegration::getUserProjects(){
    QString get_projects_command = "get_Project_names";
    QList<QByteArray> result_and_error = run_qt_process(get_projects_command,QStringList() << credFile);
    QVariantMap user_projects = getUserProjectsMap(result_and_error.at(0));
    return user_projects;
}

// name OF FUNCTION: getUserProjectFilesMap
// PURPOSE:
//    This function parses the output of "get_Project_files" command run from Qtprocess..

// Return a map of projectid and files uploaded to that project on polly..
// CALLS TO: getUserProjects
//
// CALLED FROM: getUserProjectFiles


QStringList PollyIntegration::getUserProjectFilesMap(QByteArray result2){
    QStringList user_projectfiles;
    QList<QByteArray> test_list = result2.split('\n');
    int size = test_list.size();
    QByteArray result_jsons = test_list[size-2];
    QJsonDocument doc(QJsonDocument::fromJson(result_jsons));
    // Get JSON object
    QJsonObject project_json_object = doc.object();
    QVariantMap project_json_object_map = project_json_object.toVariantMap();
    user_projectfiles = project_json_object_map["project_files"].toStringList();
    return user_projectfiles;
}

// name OF FUNCTION: getUserProjectFiles
// PURPOSE:
//    This function downloads the file information for all the projects corresponding to the logged in user.
// First of all it will run "get_Project_files" command which will connect to polly through index.js
//and then this function will call  getUserProjectFilesMap to parse the output of that command

// Returns a map of projectid and files uploaded to that project on polly..
// CALLS TO: getUserProjectFilesMap
//
// CALLED FROM: external clients

QVariantMap PollyIntegration::getUserProjectFiles(QStringList ProjectIds){
    QVariantMap user_projectfilesmap;
    for (int i=0; i < ProjectIds.size(); ++i){
        QString ProjectId = ProjectIds.at(i);
        QString get_projects_command = "get_Project_files";
        QList<QByteArray> result_and_error = run_qt_process(get_projects_command,QStringList() << credFile<<ProjectId);
        QStringList user_projectfiles = getUserProjectFilesMap(result_and_error.at(0));
        user_projectfilesmap[ProjectId] = user_projectfiles;
    }
    return user_projectfilesmap;
}

// name OF FUNCTION: createProjectOnPolly
// PURPOSE:
//    This function parses the output of "createproject" command run from Qtprocess..
// Return project id for the new project..
// CALLS TO: run_qt_process,get_run_id
//
// CALLED FROM: external clients


QString PollyIntegration::createProjectOnPolly(QString projectname){
    QString command2 = "createProject";
    QList<QByteArray> result_and_error = run_qt_process(command2, QStringList() << credFile<< projectname);
    QString run_id = get_run_id(result_and_error.at(0));
    return run_id;
}


// name OF FUNCTION: exportData
// PURPOSE:
//    This function uploads specified files to the given project
//First this function will run "get_upload_Project_urls" command which provide the upload url..
//then this function will call get_project_upload_url_commands function to upload all the given files to that url.. 
// Return patch ids for all the uploads..
// CALLS TO: run_qt_process,get_project_upload_url_commands
//
// CALLED FROM: external clients


QStringList PollyIntegration::exportData(QStringList filenames,QString projectId) {
    qDebug()<<"files to be uploaded   "<<filenames;
    QElapsedTimer timer;
    timer.start();
    QString get_upload_Project_urls = "get_upload_Project_urls";
    QList<QByteArray> result_and_error = run_qt_process(get_upload_Project_urls, QStringList() << credFile << projectId);
    QStringList patch_ids = get_project_upload_url_commands(result_and_error.at(0),filenames);
    qDebug() << "time taken in uploading json file, by polly cli is - "<<timer.elapsed();
    return patch_ids;
}

// name OF FUNCTION: loadDataFromPolly
// PURPOSE:
//    This function downloads the specified files for a given projects..
// External clients can then use those files to load data back to Elmaven..

// CALLS TO: run_qt_process,get_projectFiles_download_url_commands
//
// CALLED FROM: external clients



QString PollyIntegration::loadDataFromPolly(QString ProjectId,QStringList filenames) {
    QString get_upload_Project_urls = "get_upload_Project_urls";
    QList<QByteArray> result_and_error = run_qt_process(get_upload_Project_urls, QStringList() << credFile << ProjectId);
    QStringList patch_ids = get_projectFiles_download_url_commands(result_and_error.at(0),filenames);
    if (0<filenames.size()&&!patch_ids.isEmpty()){
        return "project data loaded";
    }
    else if(0<filenames.size()&&patch_ids.isEmpty()){
        return "error while loading project";
    }
    else{
        return "no file selected to laod";
    }
}

