#include <limits>

#include "EIC.h"
#include "plot_axes.h"
#include "tinyplot.h"

TinyPlot::TinyPlot(QGraphicsItem* parent, QGraphicsScene *scene):QGraphicsItem(parent)  {
	_width=100;
	_height=100;
	_currentXCoord=0;
	_minXValue = _minYValue = _maxXValue = _maxYValue = 0;
	//QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
	//effect->setOffset(8); 
	//setGraphicsEffect(effect);
    _noPeakData = false;
    _axesOffset = 18.0f;
    _drawAxes = true;
}


QRectF TinyPlot::boundingRect() const
{
	return(QRectF(0,0,_width,_height));
}

void TinyPlot::addDataColor(QColor c) {
	colors << c;
}

void TinyPlot::addData(QVector<float>&v) { 
	QVector<QPointF>d;
	for(int i=0; i < v.size(); i++ ) d << QPointF(i,v[i]);
	data << d;
}

void TinyPlot::addData(vector<float>&v) { 
	QVector<QPointF>d;
	for(int i=0; i < v.size(); i++ ) d << QPointF(i,v[i]);
	data << d;
}

void TinyPlot::addData(EIC* eic) {
	if(!eic) return;
	QVector<QPointF>d;
	for(int i=0; i<eic->size();i++) {
		d<< QPointF( eic->rt[i], eic->intensity[i]);
	}
	data << d;
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
    }
    data << left << center << right;

    // find bounds
    _minXValue = _minYValue = numeric_limits<float>::max();
    _maxXValue = _maxYValue = numeric_limits<float>::min();
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

void TinyPlot::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	if (_width <= 0 || _height <=0 ) return;
	float nSeries=data.size();

    // TinyPlot will only be used for displaying EICs of singular peaks.
    if (nSeries != 3)
        return;

    float maxPointIntensity=0;
    for(int i=0; i < points.size(); i++ ) {        
        if(points[i].y() > maxPointIntensity) { 
            maxPointIntensity=points[i].y();
        }
	}

	//title
	if (!_title.isEmpty()) {
		setToolTip(_title);
		painter->setBrush(Qt::black);
		painter->setPen(Qt::black);
        float _fontH = static_cast<float>(_height) / 10;
        if (_fontH > 17) {
            _fontH = 17;
        }
        if (_fontH < 11) {
            _fontH = 11;
        }
        QFont fontSmall("Helvetica",_fontH);
        painter->setFont(fontSmall);
        painter->drawText(_axesOffset + 6, _fontH+1,_title);
	}

	if (maxPointIntensity) {
		setToolTip(_title);
		painter->setBrush(Qt::black);
		painter->setPen(Qt::black);

        float _fontH = static_cast<float>(_height) / 10;
        if (_fontH > 17) {
            _fontH = 17;
        }
        if (_fontH < 11) {
            _fontH = 11;
        }

        int prec = 0;
        if (maxPointIntensity < 100) {
            prec = 1;
        } else if (maxPointIntensity < 10) {
            prec = 2;
        }
        QString rightText = QString::number(maxPointIntensity, 'f', prec);
        QFont font("Helvetica",_fontH);
        QFontMetrics fm( font );
        int legendShift = fm.size(0,rightText,0,NULL).width();
        painter->setFont(font);
        painter->drawText(_width-legendShift-2,_fontH+1,rightText);
    }

    QColor colorSolid = colors.at(0);
    QColor colorFaded = QColor::fromRgbF(colorSolid.redF(),
                                         colorSolid.greenF(),
                                         colorSolid.blueF(),
                                         0.1);
    for (int i = 0; i < nSeries; i++) {
        auto color = colorSolid;
        auto pen = QPen(color.darker());
        if (i % 2 == 0 || _noPeakData) {
            color = colorFaded;
            pen = QPen(Qt::lightGray);
        }
        painter->setBrush(color);
        painter->setPen(pen);

        int nPoints = data[i].size();
        QPolygonF path;
        if (nPoints >= 1)
            path << mapToPlot(data[i][0].x(), _minYValue);
        for (int j = 0; j < nPoints; j++)
            path << mapToPlot(data[i][j].x(), data[i][j].y());

        // close path
        if (nPoints >= 1)
            path << mapToPlot(data[i][nPoints - 1].x(), _minYValue);

        path << mapToPlot(_minXValue, _minYValue);
        painter->drawPolygon(path);
    }

    if (_drawAxes)
        _addAxes(painter);
}
