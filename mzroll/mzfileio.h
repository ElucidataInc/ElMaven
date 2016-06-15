#ifndef MZFILEIO_H
#define MZFILEIO_H
#include "globals.h"
#include "mainwindow.h"

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

        /**
         * [load Pep XML]
         * @param  filename [name of the file]
         * @return          [int]
         */
        int loadPepXML(QString filename);

    signals:

    /**
     * [update ProgressBar]
     * @param QString [message]
     * @param int     [progress value]
     * @param int     [total value]
     */
     void updateProgressBar(QString,int,int);

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
         bool _stopped;
};

#endif // MZFILEIO_H
