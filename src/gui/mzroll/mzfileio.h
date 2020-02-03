#ifndef MZFILEIO_H
#define MZFILEIO_H

#include <mutex>

#include <boost/variant.hpp>

using variant = boost::variant<int, float, double, bool, string>;

class MainWindow;
class ProjectDatabase;
class ProjectDockWidget;
class mzSample;
class PeakGroup;

Q_DECLARE_METATYPE(QList<QString>)

/**
 * @class mzPoint
 * @ingroup mzroll
 * @brief Wrapper class for File I/O
 * @author Elucidata
 */
class mzFileIO : public QThread
{
Q_OBJECT

    public:
        mzFileIO(QWidget*);
        void qtSlot(const string& progressText, unsigned int completed_samples, int total_samples);
        /**
         * [set File List]
         * @param flist [file list]
         */
        void setFileList(QStringList& flist) { filelist = flist; }

        /**
         * [load Samples]
         * @param flist [file list]
         */
        void loadSamples(QStringList& flist);

        /**
         * [load Sample]
         * @param  filename [name of the file]
         * @return          [pointer to Sample]
         */
        mzSample* loadSample(const QString& filename);

        /**
         * [parse MzData]
         * @param  fileName [name of the file]
         * @return          [pointer to Sample]
         */
        mzSample* parseMzData(QString fileName);

        /**
         * [set main window]
         * @param  MainWindow* [main window]
         */
        void setMainWindow(MainWindow*);

        int loadMassBankLibrary(QString filename);

        /**
         * [load Pep XML]
         * @param  filename [name of the file]
         * @return          [int]
         */
        int loadPepXML(QString filename);
        int ThermoRawFileImport(QString fileName);

        int loadCompoundsFromFile(QString filename);

        bool isKnownFileType(QString filename);
        bool isSampleFileType(QString filename);
        bool isProjectFileType(QString filename);
        bool isSpectralHitType(QString filename);
        bool isPeakListType(QString filename);
        bool isCompoundDatabaseType(QString filename);

        /**
         * @brief Insert a key-value pair into the `_settingsMap` map of values
         * to be saved.
         * @param key A standard string key.
         * @param var A boost variant capable of storing, integer, floats,
         * doubles, booleans and string values.
         */
        void insertSettingForSave(const string key, const variant var);

        /**
         * @brief Check whether the filename ends with a ".mzroll" extension.
         * @param filename String name of the file to be checked.
         * @return true if the filename ends with ".mzroll" extension, false
         * otherwise.
         */
        bool isMzRollProject(QString filename);

        /**
         * @brief Check whether the filename ends with a ".emDB" extension.
         * @param filename String name of the file to be checked.
         * @return true if the filename ends with ".emDB" extension, false
         * otherwise
         */
        bool isEmdbProject(QString filename);

        /**
         * @brief Check whether the filename ends with a ".mzrollDB" extension.
         * @param filename String name of the file to be checked.
         * @return true if the filename ends with ".mzrollDB" extension, false
         * otherwise
         */
        bool isMzrollDbProject(QString filename);

        /**
         * @brief Check if a SQLite project is currently open.
         * @return true if a SQLite project is open, false otherwise.
         */
        bool sqliteProjectIsOpen();

        /**
         * @brief Close the currently open SQLite project, if any.
         */
        void closeSQLiteProject();

        /**
         * @brief Write a set of parent groups with the name of table they
         * belong to.
         * @param groups A list of PeakGroup pointers that need to be saved.
         * @param tableName Name of peak table to which groups belong.
         */
        void writeGroups(QList<PeakGroup*> groups, QString tableName);

        /**
         * @brief Update an existing group or write anew if it does not exist
         * for the given table.
         * @param group The `PeakGroup` object to be written.
         * @param tableName Name of the table storing this group.
         */
        void updateGroup(PeakGroup* group, QString tableName);

        /**
         * @brief Write current session data into a SQLite database meant to be
         * uploaded to Polly.
         * @param filename Name of the on-disk database file.
         * @return `true` if write was successful, `false` otherwise.
         */
        bool writeSQLiteProjectForPolly(QString filename);

        /**
         * @brief Write current session data into a SQLite database.
         * @details The data saved include samples' metadata, peak groups,
         * peaks, associated compounds and alignment data. Write operation
         * happens into a newly created (or wiped, if already exists) SQLite
         * database. This database also becomes the currently open project.
         * @param filename String representing absolute path of the file to be
         * treated as a SQLite database.
         * @return true if the write operation was successful, false otherwise.
         */
        bool writeSQLiteProject(QString filename);

        /**
         * @brief Create a `ProjectDatabase` instance for the given filename.
         * @details If the filename passed is of the type "mzrollDB", then a
         * conversion is made into a new emDB file and then this file is set
         * as the current project.
         * @param filename Name of SQLite project to be set as `currentProject`.
         * @return Return the filename of the opened project.
         */
        QString openSQLiteProject(QString filename);

        /**
         * @brief Create a new file name from the given file name, with its
         * extension changed.
         * @param filename The name of the file, to be used as a template.
         * @param ext The new extension to be replaced in the given filename.
         * @return A new filename with the given extension.
         */
        QString swapFilenameExtension(QString filename, QString ext);

        /**
         * @brief For making old mzroll compatible, this will act as a flag
         * whether loaded mzroll file is old or new one. this will be set by
         * class method <markv_0_1_5mzroll>
         */
        bool mzrollv_0_1_5;

        /**
         * @brief Mark varible <mzrollv_0_1_5> true or false
         * @details This method marks varible <mzrollv_0_1_5> true if loaded
         * mzroll file is of v0.1.5 or older otherwise false based on one
         * attribute <SamplesUsed> which is introduced here.
         */
        void markv_0_1_5mzroll(QString fileName);

        /**
         * @brief Modify name appropriate for xml attribute naming
         * @details This method makes sample name appropriate for using in
         * attribute naming in mzroll file It is just replacing '#' with '_' and
         * adding 's' for letting sample name start with english letter In
         * future, if sample name has some other special character, we have to
         * replace those also with appropriate character Error can be seen at
         * compilation time
         */
        void cleanString(QString& name);

        /**
         * @brief It will add samples used to group being generated while
         * creating from mzroll file
         * @details This method will add all sample to group being
         * created from mzroll file. It will read SamplesUsed attribute of a
         * group and if it's value is "Used", then assign this mzSample to that
         * group
         */
        void readSamplesXML(QXmlStreamReader& xml,
                            PeakGroup* group,
                            float mzrollVersion);
        PeakGroup* readGroupXML(QXmlStreamReader& xml, PeakGroup* parent);
        void readPeakXML(QXmlStreamReader& xml, PeakGroup* parent);
        vector<PeakGroup*> readGroupsXML(QString infile);

    private Q_SLOTS:
        /**
         * @brief Load in settings and compounds, such that the session is ready
         * to load in samples and other user data.
         * @param filename A SQLite project filename to be read.
         */
        void _beginSQLiteProjectLoad();

        /**
         * @brief Read in samples data from a SQLite database previously saved
         * using `writeSQLiteProject` method. A project must be already open as
         * the `_currentProject` for this object.
         */
        void _readSamplesFromCurrentSQLiteProject();

        /**
         * @brief For a given set of samples, load the peak groups and their
         * peaks from the currently open SQLite database project.
         * @param newSamples A vector of pointers to mzSample objects to which
         * loaded peaks will be associated (according to their sample ID).
         */
        void _readPeakTablesFromSQLiteProject(const vector<mzSample*> newSamples);

        /**
         * @brief Perform some operations in the main thread that need to take
         * place after samples have been loaded.
         */
        void _postSampleLoadOperations();

        /**
         * @brief This method, intended to be run from main thread, handles the
         * case where some samples were missing from the path saved in database.
         * @param foundSamples A list of sample names that were found
         * automatically. This list can be modified by adding samples one-by-one
         * as the user provides their paths.
         */
        void _promptForMissingSamples(QList<QString> foundSamples);

    public Q_SLOTS:
        void readThermoRawFileImport();
        void addFileToQueue(QString f);
        void removeAllFilefromQueue();

    Q_SIGNALS:

    /**
     * [update ProgressBar]
     * @param QString [message]
     * @param int     [progress value]
     * @param int     [total value]
     */
     void updateStatusString(QString);
     void updateProgressBar(QString,int,int);
     void sampleLoaded();
     void sampleLoadFailed(QList<QString>);
     void spectraLoaded();
     void projectLoaded();
     void peaklistLoaded();
     void compoundsLoaded(QString, int);
     void createPeakTableSignal(QString);
     void addNewSample(mzSample*);
     void sqliteDBLoadStarted(QString);
     void sqliteDBSamplesLoaded();
     void sqliteDBPeakTablesCreated();
     void sqliteDBAlignmentDone();
     void sqliteDBPeakTablesPopulated();
     void sqliteDBUnrecognizedVersion(QString);
     void settingsLoaded(map<string, variant>);
     void appSettingsUpdated();

    protected:
      /**
       * [run function]
       */
      void run(void);

      /**
       * [file Import]
       */
      void fileImport(void);

    private:
         QStringList filelist;
         MainWindow* _mainwindow;
         ProjectDockWidget* projectdocwidget;
         bool _stopped;
         QProcess* process;
        /**
         * @brief iterator for sample id
         * @detail sampleId is unique for every sample. Everytime sample gets
         * added, sampleId gets incremented by 1. Static to make sure variable
         * is shared across threads
         */
        unsigned int sampleId;
        
        /**
         * @brief to make updating sampleId thread safe
         */
        mutex mtxSampleId;

        /**
         * @brief An instance of the ProjectData class acting as a proxy for
         * a SQLite project, which data can be written to or read from.
         */
        ProjectDatabase* _currentProject;

        bool _sqliteDBLoadInProgress;

        /**
         * @brief A vector containing names of samples that were not found, and
         * whose paths need to be explicitly specified by the user.
         */
        QList<QString> _missingSamples;

        /**
         * @brief A map of settings (identified by their keys) and their values
         * that will be saved whenever a SQLite project is saved.
         */
        map<string, variant> _settingsMap;
};

#endif // MZFILEIO_H
