#ifndef GALLERYWIDGET_H
#define GALLERYWIDGET_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"
#include "tabledockwidget.h"
#include "statistics.h"

/**
 * \class GalleryWidget
 *
 * \ingroup mzroll
 *
 * \brief Class for GalleryWidget.
 *
 * This class is used for GalleryWidget.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class GalleryWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GalleryWidget(MainWindow* mw);
    ~GalleryWidget();

public slots:
    void replot();
    void clear() {
        scene()->clear();
        plotitems.clear();
    }
    void addEicPlots(std::vector<PeakGroup*>& groups);
    void addEicPlots(std::vector<Compound*>&compounds);
    void addEicPlots(std::vector<mzSlice*>&slices);
    void addEicPlots(std::vector<mzLink>&links);
    void fileGallery(const QString& dir);
private:
    MainWindow* mainwindow;
    QList<QGraphicsItem*> plotitems;
    int _rowSpacer;
    int _colSpacer;
    int _boxW;
    int _boxH;
    TinyPlot* addEicPlot(std::vector<EIC*>& eics);
    TinyPlot* addEicPlot(mzSlice& slice);


protected:
    void drawMap();
    void resizeEvent ( QResizeEvent *event );
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    bool recursionCheck;

};

#endif

