#ifndef MZFILEIO_H
#define MZFILEIO_H

#include <mutex>
#include "globals.h"
#include "mainwindow.h"
#include "mzAligner.h"

class ProjectDatabase;
class ProjectDockWidget;

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

        /**
         * [load NIST Library]
         * @param  filename [name of the file]
         * @return          [int]
         */
        int loadNISTLibrary(QString filename);        
        int loadMassBankLibrary(QString filename);
        /**
         * [load Pep XML]
         * @param  filename [name of the file]
         * @return          [int]
         */
        int loadPepXML(QString filename);
        int ThermoRawFileImport(QString fileName);

        bool isKnownFileType(QString filename);
        bool isSampleFileType(QString filename);
        bool isProjectFileType(QString filename);
        bool isSpectralHitType(QString filename);
        bool isPeakListType(QString filename);

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
        bool isSQLiteProject(QString filename);

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
         * @brief Read samples data from a SQLite database previously saved
         * using `writeSQLiteProject` method.
         * @param filename String name of SQLite database to be read.
         * @return A vector of sample paths found in the database.
         */
        vector<string> readSamplesFromSQLiteProject(QString filename);

        /**
         * @brief For a given set of samples, load the peak groups and their
         * peaks from the currently open SQLite database project.
         * @param newSamples A vector of pointers to mzSample objects to which
         * loaded peaks will be associated (according to their sample ID).
         */
        void readAllPeakTablesSQLite(const vector<mzSample*> newSamples);

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

        void writeGroupXML(QXmlStreamWriter& stream, PeakGroup* group);
        void writeGroupsXML(QXmlStreamWriter& stream,
                            vector<PeakGroup>& groups);

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
        void _postSampleLoadOperations();

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
     void spectraLoaded();
     void projectLoaded();
     void peaklistLoaded();
     void createPeakTableSignal(QString);
     void addNewSample(mzSample*);
     void peakTablesPopulated();

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
};

#endif // MZFILEIO_H
