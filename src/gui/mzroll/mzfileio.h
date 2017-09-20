#ifndef MZFILEIO_H
#define MZFILEIO_H
#include "globals.h"
#include "mainwindow.h"


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
        mzSample* loadSample(QString filename);

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
        int loadMassBankLibrary(QString filename); //TODO: Sahil, Added while merging mzfileio
        /**
         * [load Pep XML]
         * @param  filename [name of the file]
         * @return          [int]
         */
        int loadPepXML(QString filename);
        int ThermoRawFileImport(QString fileName); //TODO: Sahil, Added while merging mzfileio

        bool isKnownFileType(QString filename); //TODO: Sahil, Added while merging mzfileio
        bool isSampleFileType(QString filename); //TODO: Sahil, Added while merging mzfileio
        bool isProjectFileType(QString filename); //TODO: Sahil, Added while merging mzfileio
        bool isSpectralHitType(QString filename); //TODO: Sahil, Added while merging mzfileio
        bool isPeakListType(QString filename); //TODO: Sahil, Added while merging mzfileio




    public Q_SLOTS:
        void readThermoRawFileImport(); //TODO: Sahil, Added while merging mzfileio
        void addFileToQueue(QString f); //TODO: Sahil, Added while merging mzfileio
        void removeAllFilefromQueue();

    Q_SIGNALS:

    /**
     * [update ProgressBar]
     * @param QString [message]
     * @param int     [progress value]
     * @param int     [total value]
     */
     void updateProgressBar(QString,int,int);
     void sampleLoaded(); //TODO: Sahil, Added while merging mzfileio
     void spectraLoaded(); //TODO: Sahil, Added while merging mzfileio
     void projectLoaded(); //TODO: Sahil, Added while merging mzfileio
     void peaklistLoaded(); //TODO: Sahil, Added while merging mzfileio
     void createPeakTableSignal(QString);

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
         QProcess* process; //TODO: Sahil, Added while merging mzfileio


};

#endif // MZFILEIO_H
