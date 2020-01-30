#include "line.h"
#include "mzSample.h"

EicLine::EicLine(QGraphicsItem* parent, QGraphicsScene *scene):QGraphicsItem(parent)
{
    setHighlighted(false);
    // Obselete Function -Kiran
    // setAcceptsHoverEvents(false);
    _endsFixed=false;
    _fillPath=false;
    _closePath=true;
    //Unintialised Value - Kiran
    _eic=NULL;
    if(scene) scene->addItem(this);
}

void EicLine::removeFromScene()
{
    prepareGeometryChange();
    if (scene() != nullptr)
        scene()->removeItem(this);
}

QRectF EicLine::boundingRect() const
{

    return(_line.boundingRect());
}
void EicLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
   // qDebug() << "EicLine::paint()" << _line.isClosed() << " " << _line.size();

   //creates closed path from end point, to base, and from base up to start point
   if (_closePath == true && _endsFixed == false) fixEnds();
    painter->setPen(_pen);
    painter->setBrush(_brush);

    if (isHighlighed()) {
	QPen pen = _pen;
        pen.setWidth( pen.width() + 2 ) ;
    	painter->setPen(pen);
    }

    if (_fillPath) {
        if (!_clipPath.isEmpty())
            painter->setClipPath(_clipPath);
        painter->drawPolygon(_line);
    //Draw piece by piece line - Kiran
    } else {
        for(int i=0; i <_line.size()-2;i+=2) {
            painter->drawLine(_line[i],_line[i+1]);
        }
    }
    //else painter->drawPolyline(_line);
    //Replaced above - Kiran
    //else painter->drawLines(_line);

}

QPainterPath EicLine::shape() const
{
   QPainterPath path;
   path.addPolygon(_line);
   return path;
}

/*
 * Merged with Maven776 - Kiran

 void EicLine::hoverEnterEvent (QGraphicsSceneHoverEvent*event ) {
     qDebug() << "EicLine:: HoverEntered..";
     QGraphicsItem::hoverEnterEvent(event);
     setHighlighted(true);
     setZValue(zValue()+1);
     scene()->update();
 }


 void EicLine::hoverLeaveEvent ( QGraphicsSceneHoverEvent*) {
     setHighlighted(false);
     setZValue(zValue()-1);
     scene()->update();
 }
*/

void EicLine::fixEnds() { 

    if (! scene()) return;
    // if the ends are already fixed - return; - Kiran
    if(_line.size() < 1 or _endsFixed == true) return;

    //cerr << this << "Fix ends.." << endl;

   // Made more readable - Kiran
    QPointF first = _line.first();
    QPointF last =  _line.last();

    QPointF a(last.x(),scene()->height());    //last point x,0
    QPointF b(first.x(),scene()->height());    //fist point 0,0
    _line.append(a);        //drop to baseline
    _line.append(b);        //move along baseline to origin
    _line.append(first);    //close path

    //qDebug() << last << a << b << first;
    _endsFixed=true;
}
