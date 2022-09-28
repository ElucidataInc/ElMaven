#ifndef PROJECTSAVEWORKER_H
#define PROJECTSAVEWORKER_H

#include <QThread>

class MainWindow;
class PeakGroup;

class ProjectSaveWorker : public QThread
{
    Q_OBJECT

public:
    ProjectSaveWorker(MainWindow* mw);

    /**
     * @brief Issue save command for user-specified emDB project.
     * @param fileName Path of the file where the emDB file will be saved.
     * @param saveRawData Whether the file should contain raw EIC and spectra
     * information for peaks.
     * @param saveChromatogram If emDB is saved with raw EIC data, saveChromatogram
     * depicts whether the EIC(s) are to be sliced or whole chromatogram must be saved. 
     */
    virtual void saveProject(const QString fileName,
                             const bool saveRawData = false,
                             const bool saveChromatogram = false);

    /**
     * @brief Update the currently set emDB project. This should be the last
     * project that was saved using this thread.
     * @param groupsToSave If any new groups need to be added or modified in
     * this file, they can be sent in this list.
     */
    void updateProject(QList<shared_ptr<PeakGroup> > groupsToSave = {});

    /**
     * @brief Obtain the project name for the file that is set as the current
     * project of this thread. Any save commands will save to this file.
     * @return
     */
    QString currentProjectName() const;

protected:
    MainWindow* _mw;
    QString _currentProjectName;
    QList<shared_ptr<PeakGroup>> _groupsToSave;
    bool _saveRawData;
    bool _saveChromatogram;
    bool _isTempProject;

    void run();

private:
    /**
     * @brief Write project data into the currently set emDB file name. If the
     * file already exists, all project tables are deleted and created anew.
     */
    void _saveSqliteProject();

    /**
     * @brief Save or update the information of a peak group in the current
     * emDB project.
     * @param group Shared pointer to the `PeakGroup` object which will be
     * saved, or updated.
     */
    void _savePeakGroupInSqlite(shared_ptr<PeakGroup> group);
};

class TempProjectSaveWorker : public ProjectSaveWorker
{
    Q_OBJECT

public:
    TempProjectSaveWorker(MainWindow* mw);
    ~TempProjectSaveWorker();

    /**
     * @brief Overrides `ProjectSaveWorker` class' original `saveProject`
     * method, such that calling this will just call the overloaded alternative,
     * thus completely ignoring the file path provided.
     */
    void saveProject(const QString filename,
                     const bool saveRawData = false,
                     const bool saveChromatogram = false) override
    {
        saveProject(saveRawData);
    }

    /**
     * @brief This method will save a temporary (time-stamped) emDB file for the
     * current session. If a temporary file has already being saved to, then it
     * will continue to be used until `deleteCurrentProject` is called.
     * @param saveRawData Whether the file should contain raw EIC and spectra
     * information for peaks.
     */
    void saveProject(const bool saveRawData = false);

    /**
     * @brief Deletes the current project (most like the project last saved
     * using this thread) from the filesystem. The next time `saveProject` is
     * called, a new temporary file will be created and save to.
     */
    void deleteCurrentProject();
};

#endif // PROJECTSAVEWORKER_H
