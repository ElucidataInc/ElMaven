#include "autoupdate.h"
#include <QProcess>
#include <QDebug>
#include <pugixml.hpp>


// UPDATE: Checking of whether an update is available or not is done. Mainwindow needs to show the "Update available window"
// as soon as El-MAVEN is started. The 'showEvent' function of MainWindow is being called before we get know whether update is available
// or not.

AutoUpdate::AutoUpdate(DownloadManager* dlManager):
    updateAvailable(false)
{
    qDebug() << "auto updater";
    checkForUpdate();
}

AutoUpdate::~AutoUpdate()
{
    qDebug() << " free all the resources";
}

void AutoUpdate::checkForUpdate()
{
    qDebug() << "checking for updates";
    _proc = new QProcess;

    _proc->setProgram("/home/g_rishabh/elmaven-0.1/maintenancetool");
    _proc->setArguments(QStringList() << "--checkupdates");

    connect(_proc, &QProcess::started, this, &AutoUpdate::processStarted);
    connect(_proc, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &AutoUpdate::processFinished);
    connect(_proc, &QProcess::errorOccurred, this, &AutoUpdate::processError);
    connect(_proc, &QProcess::readyReadStandardOutput, this, &AutoUpdate::readOutput);
    connect(_proc, &QProcess::readyReadStandardError, this, &AutoUpdate::readError);


    _proc->start();


}

void AutoUpdate::installUpdate()
{
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

void AutoUpdate::start()
{
    _proc = new QProcess;
    _proc->setProgram("/home/g_rishabh/elmaven-0.1/maintenancetool");
    _proc->setArguments(QStringList() << "--silentUpdate");

    connect(_proc, &QProcess::started, this, &AutoUpdate::processStarted);
    connect(_proc, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &AutoUpdate::processFinished);
    connect(_proc, &QProcess::errorOccurred, this, &AutoUpdate::processError);

    installUpdate();
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
    qDebug() << pnode.name();

    for(pugi::xml_node_iterator it = pnode.begin(); it != pnode.end(); ++it) {
        qDebug() << it->attribute("version").value();
        newVersion = it->attribute("version").value();
        updateAvailable = true;
    }

}

void AutoUpdate::processFinished(int exitCode, QProcess::ExitStatus status)
{

    qDebug() << "exit code : " << exitCode;
    qDebug() << "exit status: " << status;

    if(exitCode == 0 && status == QProcess::NormalExit) {
        qDebug() << "process finished without errors";
        parseOutput();
    }
    else {
        qDebug() << "process finished with errors";
        qDebug() << _error;
    }
}

void AutoUpdate::processError(QProcess::ProcessError err)
{
    qDebug() << "process error: " << err;
}
