#ifndef SPECTAMATCHING_FORM_H
#define SPECTAMATCHING_FORM_H

#include "ui_spectramatching.h"
#include "mainwindow.h"
#include "numeric_treewidgetitem.h"

class MainWindow;


class SpectraMatching : public QDialog, public Ui_SpectraMatchingForm
{
    Q_OBJECT
public:
    SpectraMatching(MainWindow *w);

public slots:
    void getFormValues();
    void findMatches();
    void showScan();
    void doSearch();
    double scoreScan(Scan*);

private:
    MainWindow *mainwindow;
    int _msScanType;
    double _precursorMz;
    QList<double> _mzsList;
    QList<double> _intensityList;

};

#endif
