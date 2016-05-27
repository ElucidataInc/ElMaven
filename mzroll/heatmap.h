#ifndef HEATMAPWIDGET_H
#define HEATMAPWIDGET_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"
#include "tabledockwidget.h"
#include "statistics.h"



class HeatMap : public QGraphicsView
{
    Q_OBJECT

public:
    HeatMap(MainWindow* mw);
    ~HeatMap();

public slots:
    void setTable(TableDockWidget* t);
    void replot();
    void updateColors();

private:
    QColor getColor(float cellValue, float minValue, float maxValue);
    MainWindow* mainwindow;
    TableDockWidget* _table;

    MatrixXf heatmap;
    float _heatMax;
    float _heatMin;
    float _colorramp;
    int _sampleSpacer;
    int _rowSpacer;
    float _boxW;
    float _boxH;


protected:
    void drawMap();
    void resizeEvent ( QResizeEvent *event );
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

};

#endif

