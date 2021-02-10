#include "mainwindow.h"
#include "mzfileio.h"
#include "mzSample.h"
#include "projectsaveworker.h"

ProjectSaveWorker::ProjectSaveWorker(MainWindow *mw)
    : _mw(mw)
    , _isTempProject(false)
{
}

void ProjectSaveWorker::saveProject(const QString fileName,
                                    const bool saveRawData, 
                                    const bool saveChromatogram)
{
    if (fileName.isEmpty())
        return;

    _currentProjectName = fileName;
    _saveRawData = saveRawData;
    _saveChromatogram = saveChromatogram;
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
    if (!_groupsToSave.isEmpty() && !_currentProjectName.isEmpty()) {
        for (auto queuedGroup : _groupsToSave)
            _savePeakGroupInSqlite(queuedGroup);
    } else {
        _saveSqliteProject();
    }
}

void ProjectSaveWorker::_saveSqliteProject()
{
    if (_currentProjectName.isEmpty())
        return;

    while (_mw->fileLoader->sqliteDbSaveInProgress());

    auto success = _mw->fileLoader->writeSQLiteProject(_currentProjectName,
                                                       _saveRawData,
                                                       _isTempProject,
                                                       _saveChromatogram);
    if (!success)
        _currentProjectName = "";
}

void ProjectSaveWorker::_savePeakGroupInSqlite(shared_ptr<PeakGroup> group)
{
    if (group == nullptr)
        return;

    while (_mw->fileLoader->sqliteDbSaveInProgress());

    if (!_mw->fileLoader->sqliteProjectIsOpen()) {
        _saveSqliteProject();
    } else {
        auto tableName = QString::fromStdString(group->tableName());
        _mw->fileLoader->updateGroup(group.get(), tableName);
    }
}

TempProjectSaveWorker::TempProjectSaveWorker(MainWindow *mw)
    : ProjectSaveWorker(mw)
{
    _isTempProject = true;
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
