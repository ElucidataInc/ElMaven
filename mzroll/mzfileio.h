#ifndef MZFILEIO_H
#define MZFILEIO_H
#include "globals.h"
#include "mainwindow.h"


/**
 * \class mzFileIO
 *
 * \ingroup mzroll
 *
 * \brief Used to load metabolite samples from mz files.
 *
 * This class is used when a mz file is imported in order to load metabolite samples
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class mzFileIO : public QThread
{
        Q_OBJECT

public:

        /** Create a mzFileIO instance.
         * @param pointer to QWidget
         */
        mzFileIO(QWidget*);

        void setFileList(QStringList& flist) {
                filelist = flist;
        }

        /** Load metabolite samples from
         * the list of input files.
         * @param QStringList& list of files
         */
        void loadSamples(QStringList& flist);

        /** Load metabolite sample from
         * an input file. Returns a pointer to
         * mzSample.
         * @param Qstring inpute filename
         */
        mzSample* loadSample(QString filename);

        /** Parse mzData file.
         * Returns a pointer to mzSample.
         * @param Qstring inpute filename
         */
        mzSample* parseMzData(QString fileName);

        void setMainWindow(MainWindow*);

        /** Load NIST library.
         * @param Qstring inpute filename
         */
        int loadNISTLibrary(QString filename);

signals:

        /** Update the progress bar at the bottom of Maven.
         * Shows progress of the file imports.
         * @param Qstring inpute filename, int, int
         */
        void updateProgressBar(QString, int, int);

protected:

        /** Run method. start() method calls this internally
         * to create a thread.
         */
        void run(void);

        /** fileImport()
         */
        void fileImport(void);

private:
        QStringList filelist;
        MainWindow* _mainwindow;
        bool _stopped;

};

#endif // MZFILEIO_H
