
#ifndef LINE_H
#define LINE_H

#include "stable.h"

class EIC;

class EicLine : public QGraphicsItem
{
public:
    EicLine(QGraphicsItem* parent, QGraphicsScene *scene);
    void addPoint(float x, float y)  { _line << QPointF(x,y); }
    void addPoint(QPointF p)  { _line << p; }
    void setColor(const QColor &c)  { _color = c; }
    void setPen(QPen &p)  { _pen = p; }
    void setBrush(QBrush &b)  { _brush = b; }
    void fixEnds();
    void setEIC(EIC* e) { _eic = e; }
    EIC* getEIC() { return _eic; }
    bool isHighlighed() { return _highlighted; }
    void setHighlighted(bool value) { _highlighted=value; }
    void setFillPath(bool value) { _fillPath=value; }
    QPainterPath shape() const;
    void setClosePath(bool value ) {_closePath=value;}
    void removeFromScene();
    void setClipPath(QPainterPath& path) { _clipPath = path; }
    QPolygonF line() const { return _line; }
    void setLine(const QPolygonF& line) { _line = line; }

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    // redundant - Kiran
    // void hoverEnterEvent (QGraphicsSceneHoverEvent*event );
    // void hoverLeaveEvent (QGraphicsSceneHoverEvent*event );
    
    
private:
    bool _highlighted;
    EIC* _eic;
    QPolygonF _line;
    QColor _color;
    QPen _pen;
    QBrush _brush;       
    bool _endsFixed;
    bool _closePath;
    bool _fillPath;
    QPainterPath _clipPath;
};

#endif
