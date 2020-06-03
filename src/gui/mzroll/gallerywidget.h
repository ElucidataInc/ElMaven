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
    pair<float, float> rtBounds();
    void setRtBounds(float minRt, float maxRt);
    float rtBuffer() { return _rtBuffer; }

signals:
    void peakRegionChanged(mzSample*, float, float);

public Q_SLOTS:
    void replot();
    void clear();
    void addEicPlots(PeakGroup* grp);
    void showPlotFor(vector<int> indexes);
    void copyImageToClipboard();

private:
    QList<TinyPlot*> _plotItems;
    int _boxW;
    int _boxH;
    int _axesOffset;
    vector<int> _indexesOfVisibleItems;
    vector<EIC*> _eics;
    map<EIC*, pair<float, float>> _peakBounds;
    QGraphicsLineItem* _leftMarker;
    QGraphicsLineItem* _rightMarker;
    QGraphicsLineItem* _markerBeingDragged;

    float _minRt;
    float _maxRt;
    float _rtBuffer;

    void _drawBoundaryMarkers();
    QGraphicsLineItem* _markerNear(QPointF pos);
    void _refillVisiblePlots(float x1, float x2);
    void _scalePlotsToIncludeMaxIntensity();
    void _fillPlotData();
    bool _visibleItemsHavePeakData();
    void _createNewPeak();
    void _deleteCurrentPeak();

protected:
    bool recursionCheck;

    void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent* event);
    void contextMenuEvent(QContextMenuEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
};

#endif
