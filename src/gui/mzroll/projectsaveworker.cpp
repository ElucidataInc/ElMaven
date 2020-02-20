#include "mainwindow.h"
#include "mzfileio.h"
#include "mzSample.h"
#include "projectsaveworker.h"

ProjectSaveWorker::ProjectSaveWorker(MainWindow *mw)
    : _mw(mw)
{
}

void ProjectSaveWorker::saveProject(const QString fileName,
                                    const bool saveRawData)
{
    if (fileName.isEmpty())
        return;

    _currentProjectName = fileName;
    _saveRawData = saveRawData;
    start();
}

void ProjectSaveWorker::updateProject(QList<shared_ptr<PeakGroup>> groupsToSave)
{
    if (_currentProjectName.isEmpty())
        return;

    _groupsToSave = groupsToSave;
    start();
}

QString ProjectSaveWorker::currentProjectName() const
{
    return _currentProjectName;
}

void ProjectSaveWorker::run()
{
    if (!_groupsToSave.isEmpty()) {
        for (auto queuedGroup : _groupsToSave)
            _savePeakGroupInSqlite(queuedGroup, _currentProjectName);
    } else {
        _saveSqliteProject(_currentProjectName);
    }
}

void ProjectSaveWorker::_saveSqliteProject(const QString fileName)
{
    auto success = _mw->fileLoader->writeSQLiteProject(fileName);
    if (success)
        _currentProjectName = fileName;
}

void ProjectSaveWorker::_savePeakGroupInSqlite(shared_ptr<PeakGroup> group,
                                               QString fileName)
{
    if (group == nullptr)
        return;

    if (!_mw->fileLoader->sqliteProjectIsOpen() && !fileName.isEmpty()) {
        _saveSqliteProject(fileName);
    } else {
        auto tableName = QString::fromStdString(group->tableName());
        _mw->fileLoader->updateGroup(group.get(), tableName);
    }
}

TempProjectSaveWorker::TempProjectSaveWorker(MainWindow *mw)
    : ProjectSaveWorker(mw)
{
}

TempProjectSaveWorker::~TempProjectSaveWorker()
{
    deleteCurrentProject();
}

void TempProjectSaveWorker::saveProject(const bool saveRawData)
{
    if (_currentProjectName.isEmpty()) {
        auto now = QDateTime::currentDateTime();
        auto tempFileName = now.toString("dd_MM_yyyy_hh_mm_ss") + ".emDB";
        auto firstSampleFile = _mw->getSamples()[0]->fileName;
        auto sampleFileInfo = QFileInfo(QString::fromStdString(firstSampleFile));
        auto samplePath = sampleFileInfo.absolutePath();
        auto tempFilePath = samplePath + QDir::separator() + tempFileName;
        ProjectSaveWorker::saveProject(tempFilePath, saveRawData);
        return;
    }
    updateProject();
}

void TempProjectSaveWorker::deleteCurrentProject()
{
    if (!_currentProjectName.isEmpty()) {
        while (isRunning());
        _mw->fileLoader->closeSQLiteProject();
        QFile::remove(_currentProjectName);
    }
    _currentProjectName = "";
}
