#ifndef GALLERYWIDGET_H
#define GALLERYWIDGET_H

#include "stable.h"

class EIC;
class MavenParameters;
class mzSample;
class PeakGroup;
class TinyPlot;

class GalleryWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GalleryWidget(QWidget* parent);
    ~GalleryWidget();

    vector<EIC*> eics() { return _eics; }

signals:
    void plotIndexChanged(int);
    void peakRegionChanged(mzSample*, float, float);

public Q_SLOTS:
    void replot();

    void clear();

    void addEicPlots(PeakGroup* grp, MavenParameters* mp);
    void showPlotFor(int index);
    void copyImageToClipboard();

private:
    QList<TinyPlot*> _plotItems;
    int _boxW;
    int _boxH;
    int _axesOffset;
    int _nItemsVisible;
    int _indexOfVisibleItem;
    vector<EIC*> _eics;
    map<EIC*, pair<float, float>> _peakBounds;
    QGraphicsLineItem* _leftMarker;
    QGraphicsLineItem* _rightMarker;
    QGraphicsLineItem* _markerBeingDragged;

    float _minRt;
    float _maxRt;
    float _minIntensity;
    float _maxIntensity;

    void _ensureCurrentItemIsVisible(bool topToBottom = true);
    void _drawBoundaryMarkers();
    QGraphicsLineItem* _markerNear(QPointF pos);
    void _refillVisiblePlot(float x1, float x2);

protected:
    bool recursionCheck;

    void drawMap();
    void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent* event);
    void contextMenuEvent(QContextMenuEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
};

#endif
