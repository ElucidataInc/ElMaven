#ifndef TINYPLOT_H
#define TINYPLOT_H

#include "stable.h"

class EIC;

class TinyPlot: public QObject, public QGraphicsItem {
    Q_OBJECT

#if QT_VERSION >= 0x040600
    Q_INTERFACES( QGraphicsItem )
#endif

public:
    TinyPlot(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
	void addData(QVector<float>&v);
	void addData(std::vector<float>&v);
	void addData(EIC* eic);
    void addData(EIC* eic,
                 float rtMin,
                 float rtMax,
                 bool highlightRange = false,
                 float peakRtMin = -1.0f,
                 float peakRtMax = -1.0f);
	void addDataColor(QColor c);
	void clearData() { data.clear(); _minYValue=_maxYValue=_minXValue=_maxXValue=0; }
	void setCurrentXCoord(float x) { _currentXCoord=x; }
	float getCurrentXCoord() { return _currentXCoord; }
	void setWidth(int w)  { _width=w; }
	void setHeight(int h) { _height=h; }
    void setTitle(QString title) { _title=title; }
    void setXBounds(float x1, float x2) { _minXValue = x1; _maxXValue = x2; }
    void setYBounds(float y1, float y2) { _minYValue = y1; _maxYValue = y2; }
    void setDrawAxes(bool draw) { _drawAxes = draw; }
    bool drawAxes() { return _drawAxes; }
    void setAxesOffset(float offset) { _axesOffset = offset; }
    void addPoint(float x, float y) { points << QPointF(x,y); }
    QPointF mapToPlot(float x, float y);

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
//  void hoverEnterEvent( QGraphicsSceneHoverEvent * event);
//	void hoverLeaveEvent( QGraphicsSceneHoverEvent * event);
//  void mouseDoubleClickEvent (QGraphicsSceneMouseEvent * event);
//	void mouseMoveEvent (QGraphicsSceneMouseEvent*);
    
private:
    void _addAxes(QPainter* painter);

	QString _title;

	int _width;
	int _height;
    float _axesOffset;
    bool _drawAxes;

    bool _noPeakData;
    float _minXValue, _minYValue, _maxXValue, _maxYValue;
	float _currentXCoord;
	QVector< QVector<QPointF> >data;
	QVector<QColor> colors;
	QVector<QPointF> points;
};

#endif
