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

    void clear();

    void addEicPlots(PeakGroup* grp, MavenParameters* mp);
    void showPlotFor(int index);
    void copyImageToClipboard();

private:
    QList<QGraphicsItem*> _plotItems;
    int _boxW;
    int _boxH;
    int _axesOffset;
    int _nItemsVisible;
    int _indexOfVisibleItem;
    vector<EIC*> _eics;
    map<EIC*, pair<float, float>> _peakBounds;
    QGraphicsLineItem* _leftMarker;
    QGraphicsLineItem* _rightMarker;

    void _ensureCurrentItemIsVisible(bool topToBottom = true);
    void _drawBoundaryMarkers();

protected:
    bool recursionCheck;

    void drawMap();
    void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent* event);
    void contextMenuEvent(QContextMenuEvent* event);
    void keyPressEvent(QKeyEvent* event);
};

#endif
