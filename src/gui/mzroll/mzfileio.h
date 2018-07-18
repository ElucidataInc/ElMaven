#ifndef MZFILEIO_H
#define MZFILEIO_H

#include <mutex>
#include "globals.h"
#include "mainwindow.h"
#include "mzAligner.h"


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
     void updateProgressBar(QString,int,int);
     void sampleLoaded();
     void spectraLoaded();
     void projectLoaded();
     void peaklistLoaded();
     void createPeakTableSignal(QString);
     void addNewSample(mzSample*);

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


};

#endif // MZFILEIO_H
