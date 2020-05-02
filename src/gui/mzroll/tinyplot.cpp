#include <limits>

#include "EIC.h"
#include "plot_axes.h"
#include "tinyplot.h"

TinyPlot::TinyPlot(QGraphicsItem* parent,
                   QGraphicsScene *scene)
    : QGraphicsItem(parent)
{
    _width = 100;
    _height = 100;
    _minXValue = _minYValue = _maxXValue = _maxYValue = 0.0f;
    _noPeakData = false;
    _axesOffset = 18.0f;
    _drawAxes = true;
}

QRectF TinyPlot::boundingRect() const
{
	return(QRectF(0,0,_width,_height));
}

void TinyPlot::clearData()
{
    _data.clear();
    _minYValue = _maxYValue = _minXValue = _maxXValue = 0.0f;
}

void TinyPlot::addData(EIC* eic,
                       float rtMin,
                       float rtMax,
                       bool highlightRange,
                       float peakRtMin,
                       float peakRtMax)
{
    if (eic == nullptr)
        return;

    QVector<QPointF> left;
    QVector<QPointF> center;
    QVector<QPointF> right;
    QVector<QPointF> baseline;
    for (int i = 0; i < eic->size(); ++i) {
        if (eic->rt[i] < rtMin)
            continue;
        if (eic->rt[i] > rtMax)
            break;

        if (peakRtMin < 0.0f || peakRtMax < 0.0f) {
            center << QPointF( eic->rt[i], eic->intensity[i]);
            if (highlightRange)
                _noPeakData = true;
        } else {
            if (eic->rt[i] < peakRtMin) {
                left << QPointF( eic->rt[i], eic->intensity[i]);
            } else if (eic->rt[i] > peakRtMax) {
                right << QPointF( eic->rt[i], eic->intensity[i]);
            } else if (eic->rt[i] == peakRtMin) {
                left << QPointF( eic->rt[i], eic->intensity[i]);
                center << QPointF( eic->rt[i], eic->intensity[i]);
            } else if (eic->rt[i] == peakRtMax) {
                right << QPointF( eic->rt[i], eic->intensity[i]);
                center << QPointF( eic->rt[i], eic->intensity[i]);
            } else {
                center << QPointF( eic->rt[i], eic->intensity[i]);
            }
        }
        baseline << QPointF(eic->rt[i], eic->baseline[i]);
    }
    _data.leftRegion = left;
    _data.peakRegion = center;
    _data.rightRegion = right;
    _data.baseline = baseline;

    // find bounds
    _minXValue = _minYValue = numeric_limits<float>::max();
    _maxXValue = _maxYValue = numeric_limits<float>::min();
    QList<QVector<QPointF>> data = {
        _data.leftRegion,
        _data.peakRegion,
        _data.rightRegion,
        _data.baseline
    };
    for(QVector<QPointF> shape : data) {
        for (auto point : shape) {
            if (point.y() > _maxYValue)
                _maxYValue = point.y() * 1.2;
            if (point.y() < _minYValue)
                _minYValue = point.y() * 0.8;
            if (point.x() > _maxXValue)
                _maxXValue = point.x();
            if (point.x() < _minXValue)
                _minXValue = point.x();
        }
    }
}

QPointF TinyPlot::mapToPlot(float x, float y)
{
    float xorigin = 0;
    float yorigin = _height;
    if (_maxXValue == 0 && _minXValue == 0)
        return QPointF(xorigin, yorigin);
    if (_maxYValue == 0 && _minYValue == 0)
        return QPointF(xorigin, yorigin);

    if (y > _maxYValue)
        y = _maxYValue;

    float px = xorigin
               + (((x - _minXValue) / (_maxXValue - _minXValue))
                  * (_width - _axesOffset))
               + _axesOffset;

    float py = yorigin
               - (((y - _minYValue) / (_maxYValue - _minYValue))
                  * (_height - _axesOffset))
               - _axesOffset;

    return QPointF(px, py);
}

void TinyPlot::_addAxes(QPainter *painter)
{
    Axes::paintAxes(painter,
                    0,
                    _minXValue,
                    _maxXValue,
                    _width + _axesOffset,
                    _height - _axesOffset,
                    _axesOffset,
                    0,
                    6,
                    true);
    Axes::paintAxes(painter,
                    1,
                    _minYValue,
                    _maxYValue,
                    _width - _axesOffset,
                    _height - _axesOffset,
                    0,
                    _axesOffset,
                    5,
                    true);
}

void TinyPlot::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *,
                     QWidget *)
{
    if (_width <= 0 || _height <=0 )
        return;

    auto drawPath = [this, painter](const QVector<QPointF>& points) {
        int nPoints = points.size();
        QPolygonF path;
        if (nPoints >= 1)
            path << mapToPlot(points[0].x(), _minYValue);
        for (auto& point : points)
            path << mapToPlot(point.x(), point.y());
        if (nPoints >= 1) // close path
            path << mapToPlot(points[nPoints - 1].x(), _minYValue);

        path << mapToPlot(_minXValue, _minYValue);
        painter->drawPolygon(path);
        return path;
    };

    QColor colorFaded = QColor::fromRgbF(_color.redF(),
                                         _color.greenF(),
                                         _color.blueF(),
                                         0.1);
    QPen penDark = QPen(_color.darker());
    QPen penFaded = QPen(Qt::lightGray);

    painter->setClipping(false);
    painter->setBrush(colorFaded);
    painter->setPen(penFaded);
    drawPath(_data.leftRegion);

    QPen penDash(_color.darker(), 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(penDash);
    QGraphicsPolygonItem baseline(drawPath(_data.baseline));

    // find region to clip below the baseline
    QPainterPath sceneBounds;
    sceneBounds.addPolygon(scene()->sceneRect());
    auto baselinePath = baseline.shape();
    auto correctedPath = sceneBounds - baselinePath;

    if (_noPeakData) {
        painter->setClipping(false);
        painter->setBrush(colorFaded);
        painter->setPen(penFaded);

        QString message = "NO PEAK";
        QFont font("Helvetica", 14);
        QFontMetrics fm(font);
        painter->setFont(font);
        painter->drawText((_width / 2) - (fm.width(message) / 2),
                          (_height / 2) + (fm.height() / 2),
                          message);
    } else {
        // first we paint the area below the baseline
        painter->setBrush(colorFaded);
        painter->setPen(penFaded);
        painter->setClipPath(baselinePath);
        drawPath(_data.peakRegion);

        // setup for painting above the baseline
        painter->setBrush(_color);
        painter->setPen(penDark);
        painter->setClipPath(correctedPath);
    }
    drawPath(_data.peakRegion);

    painter->setClipping(false);
    painter->setBrush(colorFaded);
    painter->setPen(penFaded);
    drawPath(_data.rightRegion);

    if (_drawAxes)
        _addAxes(painter);
}
