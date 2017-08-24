#ifndef SAVEJSON_H
#define SAVEJSON_H

#include "mainwindow.h"
#include "tabledockwidget.h"
#include "../libmaven/jsonReports.h"

class saveJson : public QThread
{
    Q_OBJECT
private:
    void run();

public:
    void setMainwindow(MainWindow* mw) {
        _mw = mw;
    }
    void setPeakTable(TableDockWidget * pt) {
        _pt = pt;
    }
    void setfileName(string fn) {
        _fn = fn;
    }
    MainWindow* _mw;
    TableDockWidget * _pt;
    string _fn;
    JSONReports* jsonReports;

};
#endif
