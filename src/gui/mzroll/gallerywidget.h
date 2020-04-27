#ifndef GALLERYWIDGET_H
#define GALLERYWIDGET_H

#include "stable.h"

class TinyPlot;
class MainWindow;
class EIC;
class mzLink;
class PeakGroup;
class mzSlice;
class Compound;

class GalleryWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GalleryWidget(MainWindow* mw);
    ~GalleryWidget();

public Q_SLOTS:
    void replot();

    void clear()
    {
        scene()->clear();
        _plotItems.clear();
    }

    void addEicPlots(const mzSlice& slice);
    void addEicPlotsWithGroup(vector<EIC*> eics, PeakGroup* grp);
    void copyImageToClipboard();

private:
    MainWindow* _mainWindow;
    QList<QGraphicsItem*> _plotItems;
    int _boxW;
    int _boxH;

    TinyPlot* _addEicPlot(vector<EIC*>& eics);
    TinyPlot* _addEicPlot(const mzSlice &slice);

protected:
    bool recursionCheck;

    void drawMap();
    void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void contextMenuEvent(QContextMenuEvent* event);
};

#endif
