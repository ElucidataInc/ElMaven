#ifndef MZFILEIO_H
#define MZFILEIO_H
#include "globals.h"
#include "mainwindow.h"

class mzFileIO : public QThread
{
Q_OBJECT

    public:
        mzFileIO(QWidget*);
        void setFileList(QStringList& flist) { filelist = flist; }
        void loadSamples(QStringList& flist);
        mzSample* loadSample(QString filename);
        mzSample* parseMzData(QString fileName);
        void setMainWindow(MainWindow*);
        int loadNISTLibrary(QString filename);

    signals:
        void updateProgressBar(QString,int,int);

    protected:
      void run(void);
      void fileImport(void);

    private:
         QStringList filelist;
         MainWindow* _mainwindow;
         bool _stopped;


};

#endif // MZFILEIO_H
