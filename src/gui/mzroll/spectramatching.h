#ifndef SPECTAMATCHING_FORM_H
#define SPECTAMATCHING_FORM_H

#include "statistics.h"
#include "spectralhit.h"
#include "ui_spectramatching.h"

class MainWindow;
class Scan;
class MassCutoff;

class SpectraMatching : public QDialog, public Ui_SpectraMatchingForm
{
    Q_OBJECT
    public:
        SpectraMatching(MainWindow *w);

        public Q_SLOTS:
        void getFormValues();
        void findMatches();
        void showScan();
        void doSearch();
        void exportMatches();
        double scoreScan(Scan* scan);
        double matchPattern(Scan* scan);


    private:
        MainWindow *mainwindow;
        int _msScanType;
        double _precursorMz;
        MassCutoff *_precursorMassCutoff;
        MassCutoff *_productMassCutoff;
        QVector<double> _mzsList;
        QVector<double> _intensityList;
        QVector<double> _intensityMinErr;
        QVector<double> _intensityMaxErr;

        bool bound_checking_pattern;
	StatisticsVector<float>allscores;

        QList<SpectralHit> matches;
        void addHit(double score, float precursormz, QString samplename, int matchCount, Scan* scan, QVector<double>&mzs, QVector<double>&ints); //add hit to matches

};

#endif
