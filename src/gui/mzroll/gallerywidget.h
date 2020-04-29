#ifndef GALLERYWIDGET_H
#define GALLERYWIDGET_H

#include "stable.h"

class EIC;
class MavenParameters;
class mzSample;
class PeakGroup;

class GalleryWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GalleryWidget(QWidget* parent);
    ~GalleryWidget();

signals:
    void plotIndexChanged(int);

public Q_SLOTS:
    void replot();

    void clear()
    {
        scene()->clear();
        _plotItems.clear();
    }

    void addEicPlots(PeakGroup* grp, MavenParameters* mp);
    void showPlotFor(int index);
    void copyImageToClipboard();

private:
    QList<QGraphicsItem*> _plotItems;
    int _boxW;
    int _boxH;
    int _nItemsVisible;
    int _indexOfVisibleItem;

    void _ensureCurrentItemIsVisible(bool topToBottom = true);

protected:
    bool recursionCheck;

    void drawMap();
    void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent* event);
    void contextMenuEvent(QContextMenuEvent* event);
};

#endif
