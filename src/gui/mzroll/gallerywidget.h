#ifndef GALLERYWIDGET_H
#define GALLERYWIDGET_H

#include <QGraphicsProxyWidget>

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
    void setYZoomScale(float yZoomScale);

signals:
    void peakRegionSet(mzSample*, float, float);

public Q_SLOTS:
    void replot();
    void clear();
    void addEicPlots(PeakGroup* grp);
    void recomputeBaselinesThresh(int dropTopX, int smoothingWindow);
    void recomputeBaselinesAsLS(int smoothness, int asymmetry);
    void showPlotFor(vector<int> indexes);
    void setScaleForHighestPeak(bool scale);
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
    QGraphicsProxyWidget* _leftProxyEdit;
    QGraphicsProxyWidget* _rightProxyEdit;
    QLineEdit* _leftEdit;
    QLineEdit* _rightEdit;
    QGraphicsLineItem* _markerBeingDragged;
    bool _scaleForHighestPeak;

    float _minRt;
    float _maxRt;
    float _rtBuffer;

    float _yZoomScale;

    void _drawBoundaryMarkers();
    void _drawBoundaryEditables(float rt1,
                                float x1,
                                float rt2,
                                float x2,
                                bool allPeaksEmpty);
    QGraphicsLineItem* _markerNear(QPointF pos);
    void _refillVisiblePlots(float x1, float x2);
    void _scaleVisibleYAxis();
    void _fillPlotData();
    bool _visibleItemsHavePeakData();
    float _closestRealRt(float approximateRt, EIC* eic);
    float _convertXCoordinateToRt(float x, EIC* eic);

private slots:
    void _createNewPeak();
    void _deleteCurrentPeak();
    void _setRtRegionForVisiblePeaks(float minRt, float maxRt);

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
