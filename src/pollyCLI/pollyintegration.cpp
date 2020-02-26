#include "pollyintegration.h"
#include <common/downloadmanager.h>
#include <common/logger.h>
#include <QTemporaryFile>

PollyIntegration::PollyIntegration(DownloadManager* dlManager):
    _dlManager(dlManager),
    nodePath(""),
    jsPath(""),
    _fPtr(nullptr)
{
    setObjectName("PollyIntegration");

    credFile = QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::GenericConfigLocation) + QDir::separator() + "cred_file";

    // It's important to look for node in the system first, as it might not always be present in the bin dir.
     nodePath = QStandardPaths::findExecutable("node");

    #ifdef Q_OS_WIN
      if(!QStandardPaths::findExecutable("node", QStringList() << qApp->applicationDirPath()).isEmpty())
        nodePath = qApp->applicationDirPath() + QDir::separator() + "node.exe";
      nodeModulesPath = qApp->applicationDirPath() + QDir::separator() + "node_modules" + QDir::separator();
    #endif

    #ifdef Q_OS_LINUX
      if(!QStandardPaths::findExecutable("node", QStringList() << qApp->applicationDirPath()).isEmpty())
          nodePath = qApp->applicationDirPath() + QDir::separator() + "node";
      nodeModulesPath = qApp->applicationDirPath() + QDir::separator() + "node_modules" + QDir::separator();
    #endif

    #ifdef Q_OS_MAC
      QString binDir = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator();
      if(!QStandardPaths::findExecutable("node", QStringList() << binDir + "node_bin" + QDir::separator() ).isEmpty())
        nodePath = binDir + "node_bin" + QDir::separator() + "node";
      nodeModulesPath = binDir + "node_modules" + QDir::separator();
    #endif

    indexFileURL = "https://raw.githubusercontent.com/ElucidataInc/polly-cli/master/prod/index.js";
    _dlManager->setRequest(indexFileURL, this);
}

Logger* PollyIntegration::_log()
{
    QString parentFolder = "ElMaven";
    QString logFile = QString::fromStdString(Logger::constant_time()
                                             + "_polly_integration.log");
    QString fpath = QStandardPaths::writableLocation(
                        QStandardPaths::GenericConfigLocation)
                    + QDir::separator()
                    + parentFolder
                    + QDir::separator()
                    + logFile;
    static Logger* log = new Logger(fpath.toStdString());
    return log;
}

void PollyIntegration::requestSuccess()
{
    _fPtr = new QTemporaryFile();
    _fPtr->setFileTemplate(QDir::tempPath() + QDir::separator() + "index.js");
    if(_fPtr->open()) {
        _fPtr->write(_dlManager->getData());
        _log()->debug() << "Data written to file: "
                      << _fPtr->fileName().toStdString()
                      << std::flush;
        jsPath = _fPtr->fileName();
    } else {
        jsPath = "";
        _log()->error() << "Unable to open temp file: "
                      << _fPtr->errorString().toStdString()
                      << std::flush;
    }
    _fPtr->close();
}

void PollyIntegration::requestFailed()
{
    jsPath = "";
    _log()->error() << "Failed to download file" << std::flush;
}

PollyIntegration::~PollyIntegration()
{
    if(_fPtr != nullptr) {
        if(_fPtr->remove())
            _log()->debug() << "Removed file" << std::flush;
        else {
            _log()->error() << "Error: "
                          << _fPtr->error()
                          << "\n"
                          << _fPtr->errorString().toStdString()
                          << std::flush;
        }
        delete _fPtr;
    }
    delete _log();
}

QString PollyIntegration::getCredFile(){
    return credFile;
}

bool PollyIntegration::_checkForIndexFile()
{
    if(!_fPtr || !_fPtr->exists()) {
        _log()->debug() << "Index file not found, trying to download index file…"
                      << std::flush;
        // synchronous request;
        _dlManager->setRequest(indexFileURL, this, false);
        if(!_dlManager->err) {
            requestSuccess();
            return true;
        }
        requestFailed();
        return false;
    }
    return true;
}

QList<QByteArray> PollyIntegration::runQtProcess(QString command, QStringList args){

    // e.g: command = "authenticate", "get_Project_names" etc
    // e.g: args = username, password, projectName  etc

    if (!_checkForIndexFile()) {
        QList<QByteArray> resultAndError = {
            QByteArray(),
            QString("Unable to fetch file required for integration with Polly. "
                    "This could be due to internet connectivity issues.").toLatin1()
        };
        return resultAndError;
    }

    if(jsPath == "")
        return QList<QByteArray>();

    QProcess process;
    QStringList arg;
    _log()->debug() << "JS file being used: "
                  << jsPath.toStdString()
                  << std::flush;
    arg << jsPath; // where index.js files is placed
    arg << command; // what command to pass to index.js. eg. authenticate
    arg << args; // required params by that command . eg username, password

    // nodePath = "PATH_OF_MAVEN/bin/node.exe"
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("NODE_PATH", nodeModulesPath);

    process.setProcessEnvironment(env);
    process.setProgram(nodePath);
    process.setArguments(arg);

    process.start();
    //TODO kailash, use threading for this, it should not just run indefinitely 
    process.waitForFinished(-1);
    QByteArray stdOutput = process.readAllStandardOutput();
    QByteArray stdError = process.readAllStandardError();
    _log()->debug() << "StandardOutput - "
                  << stdOutput.toStdString()
                  << std::flush;
    _log()->debug() << "StandardError - "
                  << stdError.toStdString()
                  << std::flush;
    return QList<QByteArray>() << stdOutput << stdError;
}

QString PollyIntegration::parseId(QByteArray result){
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

QPair<ErrorStatus, QMap<QString, QStringList>> PollyIntegration::_fetchAppLicense() {
    QString command = "fetchAppLicense";
    QList<QByteArray> resultAndError = runQtProcess(command,
                                                    QStringList() << credFile);

    QMap <QString, QStringList> appLicenses;
    if (_hasError(resultAndError)) {
        return qMakePair(ErrorStatus::Error, appLicenses);
    }

    QByteArray result = resultAndError.at(0);
    QStringList resultList = QString::fromStdString(result.toStdString()).split('\n');
    for (int i = 0; i < resultList.length(); i++) {
        auto temp = resultList[i];
        if (temp.contains("components")) {
            temp = resultList[++i];
            appLicenses["components"] = temp.remove(QRegExp("[\\[\\] ]")).split(',');
        }
        if (temp.contains("workflows")) {
            temp = resultList[++i];
            appLicenses["workflows"] = temp.remove(QRegExp("[\\[\\] ]")).split(',');
        }
        if (temp.contains("license")) {
            temp = resultList[++i];
            appLicenses["licenseActive"] = temp.remove(QRegExp("[\\[\\] ]")).split(',');
        }
    }

    if (appLicenses.isEmpty())
        return qMakePair(ErrorStatus::Failure, appLicenses);
    else
        return qMakePair(ErrorStatus::Success, appLicenses);
}

QMap<PollyApp, bool> PollyIntegration::getAppLicenseStatus()
{
    QMap<PollyApp, QString> appsWithTypes = {
        {PollyApp::FirstView, "components"},
        {PollyApp::PollyPhi, "workflows"},
        {PollyApp::QuantFit, "components"}
    };

    auto errorAndResponse = _fetchAppLicense();
    ErrorStatus error = errorAndResponse.first;
    auto appLicenses = errorAndResponse.second;
    QMap<PollyApp, bool> appLicenseStatus;

    if (error == ErrorStatus::Error ||
        error == ErrorStatus::Failure) {
        appLicenseStatus = {
            {PollyApp::FirstView, false},
            {PollyApp::PollyPhi, false},
            {PollyApp::QuantFit, false}
        };
        return appLicenseStatus;
    }

    QMapIterator<PollyApp, QString> it(appsWithTypes);
    while (it.hasNext()) {
        it.next();
        PollyApp app = it.key();
        QString type = it.value();
        QString componentId;

        if (type == "components")
            componentId = obtainComponentId(app);
        else if (type == "workflows")
            componentId = obtainWorkflowId(app);

        if (appLicenses.value(type).contains(componentId)
            && appLicenses.value("licenseActive")[0] != "0") {
            appLicenseStatus[app] = true;
        } else {
            appLicenseStatus[app] = false;
        }
    }
    return appLicenseStatus;
}

QString PollyIntegration::stringForApp(PollyApp app)
{
    if (app == PollyApp::FirstView) {
        return "FirstView";
    } else if (app == PollyApp::PollyPhi) {
        return "PollyPhi";
    } else if (app == PollyApp::QuantFit) {
        return "QuantFit";
    }
    return "";
}

QString PollyIntegration::obtainComponentName(PollyApp app)
{
    QString command = "getComponentName";
    QString componentName = "";
    QList<QByteArray> resultAndError = runQtProcess(command,
                                                    QStringList() << stringForApp(app)
                                                                  << credFile);
   if (_hasError(resultAndError))
        return componentName;

    QByteArray result = resultAndError.at(0);
    QList<QByteArray> resultList = result.split('\n');
    componentName = QString(resultList.at(0).split(' ').at(1));
    return componentName;
}

QString PollyIntegration::obtainComponentId(PollyApp app)
{
    QString command = "getComponentId";
    QList<QByteArray> resultAndError = runQtProcess(command,
                                                    QStringList() << credFile);
    if (_hasError(resultAndError))
        return QString::number(-1);

    QByteArray result = resultAndError.at(0);

    // replace doubly escaped quotes
    result = result.replace("\\\"", "\"");

    // split based on newlines, JSON is second last string after split
    QList<QByteArray> resultList = result.split('\n');
    result = resultList[resultList.size() - 2];

    // remove extra quotation around JSON array
    result = result.right(result.size() - 1);
    result = result.left(result.size() - 1);

    QString componentName = obtainComponentName(app);
    QJsonDocument doc(QJsonDocument::fromJson(result));
    auto array = doc.array();
    for (auto elem : array) {
        auto jsonObject = elem.toObject();
        auto name = jsonObject.value("component").toString();
        if (name == componentName)
            return QString::number(jsonObject.value("id").toInt());
    }
    return QString::number(-1);
}

QString PollyIntegration::obtainWorkflowId(PollyApp app)
{
    QString command = "getWorkflowId";
    QList<QByteArray> resultAndError = runQtProcess(command,
                                                    QStringList() << credFile);
    if (_hasError(resultAndError))
        return QString::number(-1);

    QByteArray result = resultAndError.at(0);

    // split based on newlines, JSON is second last string after split
    QList<QByteArray> resultList = result.split('\n');
    result = resultList[resultList.size() - 2];

    QString workflowName = obtainComponentName(app);
    QJsonDocument doc(QJsonDocument::fromJson(result));
    auto array = doc.array();
    for (auto elem : array) {
        auto jsonObject = elem.toObject();
        auto name = jsonObject.value("workflow").toString();
        if (name == workflowName)
            return QString::number(jsonObject.value("id").toInt());
    }
    return QString::number(-1);
}

QStringList PollyIntegration::get_project_upload_url_commands(QString url_with_wildcard, 
                                QStringList filenames) {
    
    QStringList patchId;
    for (auto& filename : filenames) {
        QStringList test_files_list = filename.split(QDir::separator());
        int size = test_files_list.size();
        QString new_filename = test_files_list[size-1];
        QString copy_url_with_wildcard = url_with_wildcard;
        QString url_map_json = copy_url_with_wildcard.replace("*", new_filename) ;
        QString upload_command = "upload_project_data";
        QList<QByteArray> resultAndError = runQtProcess(upload_command, QStringList() << url_map_json << filename);
        
        if (_hasError(resultAndError))
            return patchId;

        patchId.append(resultAndError.at(0));
    }
    return patchId;
}

bool PollyIntegration::_hasError(QList<QByteArray> resultAndError)
{
    QString supportMessage = "Please contact tech support at "
                             "elmaven@elucidata.io if the problem persists.";
    if (resultAndError.size() > 1) {
        //if there is standard error look for error message
        QByteArray errorResponse = resultAndError.at(1);

        QJsonDocument doc(QJsonDocument::fromJson(errorResponse));
        QJsonObject jsonObj = doc.object();
        QString message;
        QString type;
        for (auto key : jsonObj.keys()) {
            if (key == "message") {
                message = jsonObj[key].toString();
            }
            if (key == "type") {
                type = jsonObj[key].toString();
            }
        }

        if (!message.isEmpty() || !type.isEmpty()) {
            QString errorMessage = message + "\n" +
                                   type + "\n\n" +
                                   supportMessage;
            emit receivedEPIError(errorMessage);
            return true;
        }

        //if proper error message is not available
        //send full error response from js process
        QString errorString = QString::fromStdString(errorResponse.toStdString());
        errorString.replace("\n", "");
        if (!errorString.isEmpty()) {
            QString errorMessage = "Error: " + errorString + "\n\n" +
                                   supportMessage;
            emit receivedEPIError(errorMessage);
            return true;
        }
    } else if (resultAndError.size() == 0) {
        // no response or error
        if (_fPtr && !_fPtr->exists()) {
            emit receivedEPIError("Error: Failed to detect file required for "
                                  "integration with Polly.");
            return true;
        }

        // the most likely other reason could be…
        emit receivedEPIError("There was an unexpected error. Please make sure "
                              "you are connected to the internet and try again."
                              "\n\n"
                              + supportMessage);
        return true;
    }

    return false;
}

ErrorStatus PollyIntegration::activeInternet()
{
    QString command = QString("check_internet_connection");
    QList<QByteArray> resultAndError = runQtProcess(command, QStringList());
    if (_hasError(resultAndError))
        return ErrorStatus::Error;

    QList<QByteArray> results = resultAndError.at(0).split('\n');
    // the order of status string is not fixed, so we check both the elements
    if (results[0] == "Connected" || results[1] == "Connected")
        return ErrorStatus::Success;

    return ErrorStatus::Failure; 
}

// name OF FUNCTION: checkLoginStatus
// PURPOSE:
//    This function checks if the user is already logged in or not.
// Returns log in status of current user

// CALLS TO: runQtProcess
//
// CALLED FROM: authenticateLogin

ErrorStatus PollyIntegration::checkLoginStatus() {
    QString command = QString("authenticate");
    QList<QByteArray> resultAndError = runQtProcess(command, QStringList() << credFile);

    if (_hasError(resultAndError))
        return ErrorStatus::Error;

    QList<QByteArray> testList = resultAndError.at(0).split('\n');
    QByteArray statusLine = testList[0];
    if (statusLine == "already logged in") {

        // logged in successfully, there must be a user email
        QByteArray userline = testList[1];
        QList<QByteArray> split = userline.split(' ');
        _username = QString(split.back());
        return ErrorStatus::Success;
    }
    else {
        return ErrorStatus::Failure;
    }
}

// name OF FUNCTION: authenticateLogin
// PURPOSE:
//    This function is responsible for authenticating the user.
// This is the entry point for Elmaven-Polly-Integration
//It will first run the command "authenticate" and then check its ouptut to see if log in was succesfull
// username and password are inputed from external clients
// Returns status of log in commands, if successfully logged in, it will return 1, else 0
// CALLS TO: runQtProcess,checkLoginStatus
//
// CALLED FROM: external clients


ErrorStatus PollyIntegration::authenticateLogin(QString username, QString password) {
    QString command = "authenticate";
    
    QList<QByteArray> resultAndError = runQtProcess(command, QStringList() << credFile << username << password);
    if (_hasError(resultAndError))
        return ErrorStatus::Error;

    return checkLoginStatus();
}

QString PollyIntegration::getCurrentUsername()
{
    return _username;
}

// This function checks if node executable path has been defined for the library or not..
int PollyIntegration::checkNodeExecutable() {
    if (nodePath == "") {
        return 0;
    }
    return 1;
    
}

int PollyIntegration::askForLogin() {
    _log()->debug() << "Credentials File -\n"
                  << credFile.toStdString()
                  << std::flush;
    QFile file (credFile);
    QFile refreshTokenFile (credFile + "_refreshToken");
    if (file.exists() && refreshTokenFile.exists()) {
        _log()->debug() << "Both tokens exist, moving on to refresh now…"
                      << std::flush;
        return 0;
    }
    _log()->debug() <<"Both tokens do not exist, moving on to login now…"
                  << std::flush;
    return 1;
}

// This function deletes the token and logs out the user..
void PollyIntegration::logout() {
    _username = "";
    QFile file (credFile);
    file.remove();
    QFile refreshTokenFile (credFile + "_refreshToken");
    refreshTokenFile.remove();
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


QVariantMap PollyIntegration::getUserProjects() {
    QVariantMap userProjects;
    QString getProjectsCommand = "get_Project_names";
    QList<QByteArray> resultAndError = runQtProcess(getProjectsCommand, QStringList() << credFile);
    if (_hasError(resultAndError))
        return userProjects;

    QList<QByteArray> testList = resultAndError.at(0).split('\n');
    QByteArray resultJsons = testList[testList.size() - 2];
    QJsonDocument doc(QJsonDocument::fromJson(resultJsons));
    // Get JSON object
    QJsonArray jsonArray = doc.array();
    for (int i = 0; i < jsonArray.size(); ++i) {
        QJsonValue projectJson = jsonArray.at(i);
        QVariantMap projectMap = projectJson.toObject().toVariantMap();
        userProjects[projectMap["id"].toString()] = projectMap["name"].toString();
    }
    return userProjects;
}

// name OF FUNCTION: getUserProjectFilesMap
// PURPOSE:
//    This function parses the output of "get_Project_files" command run from Qtprocess..

// Return a map of projectid and files uploaded to that project on polly..
// CALLS TO: getUserProjects
//
// CALLED FROM: getUserProjectFiles


QStringList PollyIntegration::getUserProjectFilesMap(QByteArray result2) {
    QStringList userProjectfiles;
    QList<QByteArray> testList = result2.split('\n');
    int size = testList.size();
    QByteArray resultJsons = testList[size-2];
    QJsonDocument doc(QJsonDocument::fromJson(resultJsons));
    // Get JSON object
    QJsonObject projectJsonObject = doc.object();
    QVariantMap projectJsonObjectMap = projectJsonObject.toVariantMap();
    userProjectfiles = projectJsonObjectMap["project_files"].toStringList();
    return userProjectfiles;
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

QVariantMap PollyIntegration::getUserProjectFiles(QStringList projectIds) {
    QVariantMap userProjectfilesmap;
    for (int i=0; i < projectIds.size(); ++i){
        QString projectId = projectIds.at(i);
        QString getProjectsCommand = "get_Project_files";
        QList<QByteArray> resultAndError = runQtProcess(getProjectsCommand, QStringList() << credFile << projectId);
        if (_hasError(resultAndError))
            return userProjectfilesmap;

        QStringList userProjectfiles = getUserProjectFilesMap(resultAndError.at(0));
        userProjectfilesmap[projectId] = userProjectfiles;
    }
    return userProjectfilesmap;
}

// name OF FUNCTION: createProjectOnPolly
// PURPOSE:
//    This function parses the output of "createproject" command run from Qtprocess..
// Return project id for the new project..
// CALLS TO: runQtProcess,parseId
//
// CALLED FROM: external clients


QString PollyIntegration::createProjectOnPolly(QString projectname) {
    QString runId;
    QString command2 = "createProject";
    QList<QByteArray> resultAndError = runQtProcess(command2, QStringList() << credFile << projectname);
    if (_hasError(resultAndError))
        return runId;

    runId = parseId(resultAndError.at(0));
    return runId;
}

QString PollyIntegration::createWorkflowRequest(QString projectId,
                                                QString workflowName,
                                                QString workflowId) {
    QString workflowRequestId;
    QString command2 = "createWorkflowRequest";
    QList<QByteArray> resultAndError = runQtProcess(command2, QStringList() << credFile 
                                                                            << projectId
                                                                            << workflowName
                                                                            << workflowId);
    if (_hasError(resultAndError))
        return workflowRequestId;

    workflowRequestId = parseId(resultAndError.at(0));
    return workflowRequestId;
}

QString PollyIntegration::createRunRequest(QString componentId,
                                           QString projectId,
                                           QString extraInfo)
{
    QString runId;
    QString command = "createRunRequest";
    QStringList arguments = QStringList() << credFile
                                          << componentId
                                          << projectId
                                          << extraInfo;
    QList<QByteArray> resultAndError = runQtProcess(command, arguments);
    if (_hasError(resultAndError))
        return runId;

    runId = parseId(resultAndError.at(0));
    return runId;
}

QByteArray PollyIntegration::redirectionUiEndpoint()
{
    QByteArray result;
    QString command = "getEndpointForRuns";
    QList<QByteArray> resultAndError = runQtProcess(command, QStringList(credFile));
    if (_hasError(resultAndError))
        return result;

    result = resultAndError.at(0);

    // split based on newlines, JSON is second last string after split
    QList<QByteArray> resultList = result.split('\n');
    result = resultList[resultList.size() - 2];

    return result;
}

QString PollyIntegration::getProjectUrl(QString projectId)
{
    QString command2 = "getProjectUrl";
    QList<QByteArray> resultAndError = runQtProcess(command2,
                                                    QStringList() << projectId);
    if (_hasError(resultAndError))
        return "";

    QByteArray result = resultAndError.at(0);
    return QString(result).trimmed();
}

QString PollyIntegration::getComponentEndpoint(QString componentId,
                                               QString runId,
                                               QString datetimestamp)
{
    QByteArray result = redirectionUiEndpoint();

    QJsonDocument doc(QJsonDocument::fromJson(result));
    auto array = doc.array();
    for (auto elem : array) {
        auto jsonObject = elem.toObject();
        auto id = QString::number(jsonObject.value("component_id").toInt());
        if (id == componentId) {
            auto url = jsonObject.value("url").toString();
            url = url.replace("<runid>", runId)
                     .replace("<timestamp>", datetimestamp);
            return url;
        }
    }
    return "";
}

QString PollyIntegration::getWorkflowEndpoint(QString workflowId,
                                              QString workflowRequestId,
                                              QString landingPage,
                                              QString uploadProjectIdThread,
                                              QString datetimestamp)
{
    QByteArray result = redirectionUiEndpoint();

    QJsonDocument doc(QJsonDocument::fromJson(result));
    auto array = doc.array();
    for (auto elem : array) {
        auto jsonObject = elem.toObject();
        auto id = QString::number(jsonObject.value("workflow_id").toInt());
        if (id == workflowId) {
            auto url = jsonObject.value("url").toString();
            url = url.replace("<workflowRequestId>", workflowRequestId)
                     .replace("<landingPage>", landingPage)
                     .replace("<uploadProjectIdThread>", uploadProjectIdThread)
                     .replace("<datetimestamp>", datetimestamp);
            return url;
        }
    }
    return "";
}

ErrorStatus PollyIntegration::sendEmail(QString userEmail,
                                 QString emailMessage,
                                 QString emailContent,
                                 QString appName)
{
    QString command2 = "send_email";
    QList<QByteArray> resultAndError = runQtProcess(command2,
                                                    QStringList() << userEmail
                                                                  << emailMessage
                                                                  << emailContent
                                                                  << appName);
    if (_hasError(resultAndError))
        return ErrorStatus::Error;

    QList<QByteArray> testList = resultAndError.at(0).split('\n');
    int size = testList.size();
    if (size > 2) {
        QByteArray result2 = testList[size-2];
        if (result2 == "1")
            return ErrorStatus::Success;
    }

    return ErrorStatus::Failure;
}

// name OF FUNCTION: exportData
// PURPOSE:
//    This function uploads specified files to the given project
//First this function will run "get_upload_Project_urls" command which provide the upload url..
//then this function will call get_project_upload_url_commands function to upload all the given files to that url.. 
// Return patch ids for all the uploads..
// CALLS TO: runQtProcess,get_project_upload_url_commands
//
// CALLED FROM: external clients


QPair<ErrorStatus, QStringList> PollyIntegration::exportData(QStringList filenames, QString projectId) {
    _log()->debug() << "Files to be uploaded: "
                  << filenames.join(", ").toStdString()
                  << std::flush;
    QStringList patchId;

    QElapsedTimer timer;
    timer.start();
    QString get_upload_Project_urls = "get_upload_Project_urls";
    QList<QByteArray> resultAndError = runQtProcess(get_upload_Project_urls, QStringList() << credFile << projectId);
    if (_hasError(resultAndError))
        return qMakePair(ErrorStatus::Error, patchId);

    QString url_with_wildcard = getFileUploadURLs(resultAndError.at(0));
    patchId = get_project_upload_url_commands(url_with_wildcard, filenames);
    _log()->debug() << "Time spent uploading JSON file, through Polly CLI: "
                  << timer.elapsed()
                  << std::flush;
    
    return qMakePair(ErrorStatus::Success, patchId);
}

QString PollyIntegration::getFileUploadURLs(QByteArray result2) {
    QList<QByteArray> test_list = result2.split('\n');
    int size = test_list.size();
    QByteArray url_jsons = test_list[size-2];
    QJsonDocument doc(QJsonDocument::fromJson(url_jsons));
    // Get JSON object
    QJsonObject json = doc.object();
    QVariantMap json_map = json.toVariantMap();
    QString url_with_wildcard =  json_map["file_upload_urls"].toString();

    return url_with_wildcard;
}

ErrorStatus PollyIntegration::UploadToCloud(QString uploadUrl, QString filePath)
{
    QString upload_command = "uploadCuratedPeakDataToCloud";
    QList<QByteArray> resultAndError = runQtProcess(upload_command, QStringList() << uploadUrl << filePath);
    if (_hasError(resultAndError))
        return ErrorStatus::Error;

    return ErrorStatus::Success;
}

ErrorStatus PollyIntegration::UploadPeaksToCloud(QString session_indentifier, QString fileName, QString filePath){
    QElapsedTimer timer;
    timer.start();
    QString command = "getPeakUploadUrls";
    QList<QByteArray> resultAndError = runQtProcess(command, QStringList() << session_indentifier << fileName);
    if (_hasError(resultAndError))
        return ErrorStatus::Error;

    QString uploadUrl = getFileUploadURLs(resultAndError.at(0));
    ErrorStatus status = UploadToCloud(uploadUrl, filePath);
    _log()->debug() << "Time spent uploading JSON file, through Polly CLI: "
                  << timer.elapsed()
                  << std::flush;
    return status;
}

bool PollyIntegration::validSampleCohort(QString sampleCohortFile, QStringList loadedSamples) {
    _log()->debug() << "Validating sample cohort file…" << std::flush;
	
    QFile file(sampleCohortFile);
    if (!file.open(QIODevice::ReadOnly)) {
        _log()->debug() << file.errorString().toStdString() << std::flush;
		return false;
    }

    QStringList samples;
	QStringList cohorts;
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        QList<QByteArray> splitRow = line.split(',');
        if (splitRow.size() < 2) {
            _log()->debug() << "Missing column(s)" << std::flush;
			return false;
        }
		
		//skip header row
		if (splitRow.at(0) == "Sample")
			continue;
		
		QString sampleName = splitRow.at(0);
		QString cohortName = splitRow.at(1);
        
        if (cohortName.trimmed().isEmpty()) {
            _log()->debug() << "Cohort missing for some samples" << std::flush;
            return false;
        }

		samples.append(QString::fromStdString(sampleName.toStdString()));
		cohorts.append(QString::fromStdString(cohortName.toStdString()));
    }

	//this check is only required when user provides a sample cohort file through cli
    if (!loadedSamples.isEmpty()) {
        qSort(samples);
	    qSort(loadedSamples);
	
	    if (!(samples == loadedSamples)) {
            _log()->debug() << "The sample cohort file contains different sample "
                             "names than the samples loaded in Elmaven… Please "
                             "try again with the correct file"
                          << std::flush;
		    return false;
	    }                               
    }
	
	if (!validCohorts(cohorts)) {
        _log()->debug() << "The sample cohort file contains more than 9 cohorts. "
                         "As of now, Polly supports only 9 or less cohorts. "
                         "Please try reducing the number of cohorts."
                      << std::flush;
		return false;
	}

	return true;
}

bool PollyIntegration::validCohorts(QStringList cohorts) {
	cohorts.removeDuplicates();
	if(cohorts.size() > 9)
		return false;
	
	return true;
}
