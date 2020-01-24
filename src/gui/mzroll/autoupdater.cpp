#define _STR(X) #X
#define STR(X) _STR(X)

#include <QProcess>
#include <QDebug>
#include <QApplication>

#include <pugixml.hpp>

#include <common/downloadmanager.h>
#include "autoupdater.h"
#include "base64.h"
#include "projectversioning.h"

void AutoUpdater::startMaintenanceTool()
{
    // Windows: <install_dir>/El-MAVEN/bin/
    // MacOS:   <install_dir>/El-MAVEN/bin/El-MAVEN.app/Contents/MacOS/
    QDir applicationDir(QCoreApplication::applicationDirPath());

#ifdef __OSX_AVAILABLE
    applicationDir.cdUp(); // <install_dir>/El-MAVEN/bin/El-MAVEN.app/Contents/
    applicationDir.cdUp(); // <install_dir>/El-MAVEN/bin/El-MAVEN.app/
    applicationDir.cdUp(); // <install_dir>/El-MAVEN/bin/
#endif
    applicationDir.cdUp(); // <install_dir>/El-MAVEN/
    applicationDir.cdUp(); // <install_dir>/

    QString maintenanceToolPath = "";
#ifdef __OSX_AVAILABLE
    maintenanceToolPath = applicationDir.path()
                          + QDir::separator()
                          + "maintenancetool.app/Contents/MacOS/maintenancetool";
#endif
#ifdef Q_OS_WIN
    maintenanceToolPath = applicationDir.path()
                          + QDir::separator()
                          + "maintenancetool.exe";
#endif

    qDebug() << "Using path"
             << maintenanceToolPath
             << "for maintenancetool executable.";
    if (!QFile::exists(maintenanceToolPath)) {
        qWarning() << "Maintenance tool was not found at its expected path. "
                      "Cannot proceed with update process.";
        return;
    }

    QProcess* maintenanceToolProcess = new QProcess;
    connect(maintenanceToolProcess,
            &QProcess::started,
            this,
            &AutoUpdater::maintenanceToolStarted);
    connect(maintenanceToolProcess,
            static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this,
            &AutoUpdater::maintenanceToolClosed);

    maintenanceToolProcess->setProgram(maintenanceToolPath);
    maintenanceToolProcess->setArguments({"--updater"});
    maintenanceToolProcess->start();
}

void AutoUpdater::_checkForUpdate()
{
#ifndef UPDATE_REPO_URL_BASE64
    return;
#endif
    string updateRepoUrl = STR(UPDATE_REPO_URL_BASE64);
    updateRepoUrl = base64::decodeString(updateRepoUrl.c_str(),
                                         updateRepoUrl.size());
#ifdef __OSX_AVAILABLE
    // remove attached newline at the end
    updateRepoUrl = updateRepoUrl.substr(0, updateRepoUrl.size() - 1);
#endif
    QString updatesXmlUrl = QString::fromStdString(updateRepoUrl)
                            + "/Updates.xml";

    DownloadManager downloader;
    downloader.setRequest(updatesXmlUrl, this, false);
    if (!downloader.err) {
        auto downloadedData = downloader.getData();
        auto result = _parseUpdatesXml(downloadedData);
        if (result.first) {
            if (!result.second.isEmpty())
                emit updateAvailable(result.second);
            return;
        }
    }
    emit updateCheckFailed();
}

pair<bool, QString> AutoUpdater::_parseUpdatesXml(QByteArray& downloadedData)
{
    const char* output = downloadedData.constData();
    pugi::xml_document xmlDoc;
    pugi::xml_parse_result parseResult = xmlDoc.load_string(output);

    // return if parsing data as XML failed
    if(parseResult.status != pugi::xml_parse_status::status_ok) {
        qWarning() << "Failed to parse update data:"
                   << parseResult.description();
        return make_pair(false, "");
    }

    pugi::xml_node unode = xmlDoc.child("Updates");
    if(!unode.empty()) {
        pugi::xml_node pnode = unode.child("PackageUpdate");
        if(!pnode.empty()) {
            string version = pnode.child("Version").child_value();
            auto latestVersion = ProjectVersioning::Version(version);

            string tagString = STR(EL_MAVEN_VERSION);
            auto thisVersionData =
                ProjectVersioning::extractVersionInfoFromTag(tagString);
            auto thisVersion = ProjectVersioning::Version(thisVersionData.first);

            if (latestVersion > thisVersion)
                return make_pair(true, latestVersion.toString().c_str());
        }
    }
    return make_pair(true, "");
}
