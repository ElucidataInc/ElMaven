#include "autoupdate.h"
#include <QProcess>
#include <QDebug>
#include <pugixml.hpp>
#include <QApplication>


AutoUpdate::AutoUpdate():
    _state(State::NotStarted)
{
    _proc = new QProcess;
    QString maintenanceToolPath = qApp->applicationDirPath() + "/../" + "../" + "../" + "../" + "../"
                                  + "maintenancetool.app/" + "Contents/" + "MacOS/" + "maintenancetool";
    // QString maintenanceToolPath = "/Users/osx/Applications/El-MAVEN2/maintenancetool.app/Contents/MacOS/maintenancetool";
    qDebug() <<  "path:  " <<  maintenanceToolPath;
    _proc->setProgram(maintenanceToolPath);

    connect(_proc, &QProcess::started, this, &AutoUpdate::processStarted);
    connect(_proc, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &AutoUpdate::processFinished);
    connect(_proc, &QProcess::errorOccurred, this, &AutoUpdate::processError);
    connect(_proc, &QProcess::readyReadStandardOutput, this, &AutoUpdate::readOutput);
    connect(_proc, &QProcess::readyReadStandardError, this, &AutoUpdate::readError);
}

AutoUpdate::~AutoUpdate()
{
    qDebug() << " free all the resources";
}

void AutoUpdate::checkForUpdate()
{
    qDebug() << "checking for updates";
    _state = State::CheckingForUpdates;
    _proc->setArguments(QStringList() << "--checkupdates");
    _proc->start();
}

void AutoUpdate::readError()
{
    qDebug() << "reading error";
    _error = _proc->readAllStandardError();
}

void AutoUpdate::readOutput()
{
    while(_proc->bytesAvailable()) {
        _output += _proc->readLine();
    }
}

void AutoUpdate::update()
{
    _state = State::Updating;
    _proc->setArguments(QStringList() << "--silentUpdate");
    _proc->start();
}

void AutoUpdate::processStarted()
{
    qDebug() << "process started";
}

void AutoUpdate::parseOutput()
{
    // using the output check whether update is available or not
    qDebug() << "reading output";
    qDebug() << _output;
    const char* output = _output.constData();
    pugi::xml_document xmlDoc;
    pugi::xml_parse_result parseResult = xmlDoc.load_string(output);

    // return if parsing the xml failed
    if(parseResult.status != pugi::xml_parse_status::status_ok) {
        qDebug() << parseResult.description() << endl;
        return;
    }

    pugi::xml_node pnode = xmlDoc.child("updates");
    if(!pnode.empty()) {
        pugi::xml_node cnode = pnode.child("update");
        if(!cnode.empty()) {
            newVersion = cnode.attribute("version").value();
            emit updateAvailable();
        }
    }
}

void AutoUpdate::processFinished(int exitCode, QProcess::ExitStatus status)
{

    qDebug() << "exit code : " << exitCode;
    qDebug() << "exit status: " << status;


    if(exitCode == 0 && status == QProcess::NormalExit) {
        qDebug() << "process finished without errors";
        
        switch(_state) {
            case State::Updating:
                qDebug() << "Update has been completed successfully";
                break;
            case State::CheckingForUpdates:
                parseOutput();
                break;
            default:
                break;
        }
    }
    else {
        qDebug() << "process finished with errors";
        qDebug() << _error;
    }

    _output = "";
    _error = "";
}

void AutoUpdate::processError(QProcess::ProcessError err)
{
    qDebug() << "process error: " << err;
}
