#include "Compound.h"
#include "edge.h"
#include "enzyme_node.h"
#include "globals.h"
#include "graphwidget.h"
#include "mzSample.h"
#include "datastructures/mzSlice.h"
#include "Peak.h"
#include "PeakGroup.h"
#include "Scan.h"

Edge::Edge() {
	_arrowSize = 10;
	_reversable = true;
	_color = Qt::black;
	setRateForward(0.01);
	setRateReverse(0.01);
	setReversable(false);
    setFlux(0);
	setLastFlux(0);
    setVisible(false);
    setFlag(ItemIsSelectable);
	setAcceptHoverEvents(true);
	setHighlighted(false);
	setZValue(0);

	//animation
	_timerId = 0;
	_timerStep = 0;
	_timerMaxSteps=20;

	_showNote=false;

}

Edge::~Edge() {
}

Node *Edge::sourceNode() const
{
    return source;
}

void Edge::setSourceNode(Node *node)
{
    source = node;
    adjust();
}

Node *Edge::destNode() const
{
    return dest;
}

void Edge::setDestNode(Node *node)
{
    dest = node;
    adjust();
}

void Edge::adjust()
{
    if (!isVisible()) return;
    if (!source || !dest) return;
	if (!source->isVisible()) return;
	if (!dest->isVisible()) return;

    sourcePoint = mapFromItem(source, 0, 0);
    destPoint = mapFromItem(dest,   0, 0);
    /*
    QLineF line(s,d);
    prepareGeometryChange();
    sourcePoint = line.p1();
    destPoint = line.p2();
    */
}

double Edge::angle() { 
	return computeAngle(sourcePoint,destPoint);
}

double Edge::length() { 
	return QLineF(sourcePoint,destPoint).length();
}

QRectF Edge::boundingRect() const
{
    if (!source || !dest) return QRectF();
    qreal extra = 5;
    QLineF line(sourcePoint,destPoint);
    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(), destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QPainterPath Edge::shape() const
{
		return _shape;
}

double Edge::computeAngle(const QPointF& a, const QPointF& b) { 
    return (double) atan2( (b.y()-a.y()), (b.x()-a.x()));

	/*
    float dy = (b.y()-a.y());
    float dx = (b.x()-a.x());
    if (dy == 0 && dx == 0 ) return 0;
    return atan2(dy,dx);
    */

    /*
	float llen = sqrt(POW2(a.x() - b.x()) + POW2(a.y()-b.y()));
	if ( llen == 0 ) return 0;
    double angle = acos((b.x()-a.x())/llen);
	if ( b.y()-a.y() < 0 ) angle = -angle;
	return angle;
	*/

}

float Edge::computeArrowSize(float flux) { 

	GraphWidget* g=NULL; if ( source ) g = source->getGraphWidget();
	float size = g->getAvgEdgeLength()*0.25;
	float scale=1; if(g) scale *= g->getEdgeSizeScale();
	float arrowSize =  size*scale;
    if ( flux > 0 ) { arrowSize += 5*log2(flux); }
	if ( arrowSize < 2)    arrowSize=2;

    return arrowSize;
}

void Edge::drawArrow(QPainter *painter, int direction) {

	if (!source || !source->isVisible()) return;
	if (!dest   || !dest->isVisible()) return;

    sourcePoint = mapFromItem(source, 0, 0);
    destPoint = mapFromItem(dest,   0, 0);
    //qDebug() << "drawArrow: " << sourcePoint << " " << destPoint;
    
	if(sourcePoint == destPoint) return;
	float edgeLength =  length();
    if (edgeLength <= 0) return;

	float lastArrowSize = computeArrowSize(getLastFlux());
    float arrowSize = computeArrowSize(getFlux());
	float sceneLength = sqrt(POW2(scene()->width())+POW2(scene()->height()));
	if ( edgeLength >  sceneLength ) return;

 	 _arrowSize = arrowSize;
	 if ( _arrowSize > edgeLength ) _arrowSize = edgeLength/2;

	//boubd boxes
	double arrowAngle=Pi/10;
	double angle = computeAngle(sourcePoint,destPoint);
	double da = sourceNode()->getNodeSize()/2;
	double db = destNode()->getNodeSize()/2;

    QPointF midPoint = sourcePoint-(sourcePoint-destPoint)/2;

	QPointF a = sourcePoint + QPointF(    cos(angle)*da,  sin(angle) *da);
	QPointF b = destPoint   + QPointF(cos(Pi+angle)*db, sin(Pi+angle)*db);

	if (source->isEnzyme() ) a = ((EnzymeNode*) source)->activeSitePosition();
	if (dest->isEnzyme() )   b = ((EnzymeNode*) dest)->activeSitePosition();

    painter->setBrush(Qt::black);
	QPen pen(_color, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
	if ( isSelected() || isHighlighted() ) { pen.setColor(Qt::yellow);  pen.setWidth(3); }

	/*
	QLinearGradient gradient(midPoint,b);
	gradient.setSpread(QGradient::ReflectSpread);
    QColor color1 = Qt::white;
    QColor color2= Qt::gray;
    gradient.setColorAt(0,   color1);
    gradient.setColorAt(1.0, color2);
    painter->setBrush(gradient);
	*/

    painter->setBrush(Qt::gray);
	if ( isSelected() || isHighlighted() )  painter->setBrush(Qt::yellow);


	if ( direction != 0 ) { //forward

		if ( direction < 0 ) {
			QPointF tmp= a; a=b; b=tmp;	//swap
		}
		angle = computeAngle(a,b);
		QPainterPath path;  
		const double arrowAngleSize=pen.width()+_arrowSize;
    	QPointF p1 =   b + QPointF(cos(angle+Pi-arrowAngle)*arrowAngleSize, sin(angle+Pi-arrowAngle)*arrowAngleSize);
    	QPointF p1m =  b + QPointF(cos(angle+Pi-arrowAngle/2)*arrowAngleSize, sin(angle+Pi-arrowAngle/2)*arrowAngleSize);
    	QPointF p2 =   b + QPointF(cos(angle+Pi+arrowAngle)*arrowAngleSize, sin(angle+Pi+arrowAngle)*arrowAngleSize);
    	QPointF p2m =   b + QPointF(cos(angle+Pi+arrowAngle/2)*arrowAngleSize, sin(angle+Pi+arrowAngle/2)*arrowAngleSize);

	    path.moveTo(b); path.lineTo(p1);  path.lineTo(p1m); path.lineTo(a); 
						path.lineTo(p2m); path.lineTo(p2); path.lineTo(b);
		_shape=path;
		painter->drawPath(path);
	} else {
		QPainterPath path; 
		angle = computeAngle(a,b);
		const double arrowAngleSize=pen.width()+_arrowSize;

    	QPointF b1 =   b + QPointF(cos(angle+Pi-arrowAngle)*arrowAngleSize, sin(angle+Pi-arrowAngle)*arrowAngleSize);
    	QPointF b1m =  b + QPointF(cos(angle+Pi-arrowAngle/2)*arrowAngleSize, sin(angle+Pi-arrowAngle/2)*arrowAngleSize);
    	QPointF b2 =   b + QPointF(cos(angle+Pi+arrowAngle)*arrowAngleSize, sin(angle+Pi+arrowAngle)*arrowAngleSize);
    	QPointF b2m =   b + QPointF(cos(angle+Pi+arrowAngle/2)*arrowAngleSize, sin(angle+Pi+arrowAngle/2)*arrowAngleSize);

    	QPointF a1m =   a + QPointF(cos(angle+Pi/2)*arrowAngleSize/20, sin(angle+Pi/2)*arrowAngleSize/20);
    	QPointF a2m =   a + QPointF(cos(angle-Pi/2)*arrowAngleSize/20, sin(angle-Pi/2)*arrowAngleSize/20);

		if (dest->isEnzyme() ) {
			b1=b1m =   b + QPointF(cos(angle+Pi/2)*arrowAngleSize/20, sin(angle+Pi/2)*arrowAngleSize/20);
			b2=b2m =   b + QPointF(cos(angle-Pi/2)*arrowAngleSize/20, sin(angle-Pi/2)*arrowAngleSize/20);
		}
	
	    path.moveTo(b); path.lineTo(b1);  path.lineTo(b1m); 
						path.lineTo(a1m); path.lineTo(a2m); 
						path.lineTo(b2m); path.lineTo(b2); 
						path.lineTo(b);
		_shape=path;
		painter->drawPath(path);
	}

	//cerr << a.x() << " " << a.y() << " " << b.x() << " " << b.y() << " " << endl;
	sourcePoint = a;
	destPoint = b;

	if (_showNote) {
			GraphWidget* g=NULL; if ( source ) g = source->getGraphWidget();
			double fontHeight = g->getAvgEdgeLength() * 0.08;
			QFont f( "Helvetica",fontHeight);
			painter->setFont(f);
			painter->save();
			painter->translate(midPoint+QPoint(0,pen.width()+fontHeight));
			painter->rotate(angle);
			painter->drawText(QPoint(0,0),_note);
			painter->restore();
	}

	/*
	painter->setBrush(Qt::NoBrush);
	painter->setPen(QPen(Qt::black));
    painter->drawRect(boundingRect());
	*/
}

void Edge::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
	setSelected(true);
    update();
}


void Edge::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
	setSelected(false);
    update();
}

void Edge::hoverEnterEvent (QGraphicsSceneHoverEvent*) {
    setToolTip(_note);
	update();
}

void Edge::hoverLeaveEvent ( QGraphicsSceneHoverEvent*) {
	update();
}

void Edge::reverseDirection() {
	Node* tmpNode = source;
	source = dest;
	dest = tmpNode;
}

void Edge::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event ) {
	reverseDirection();
	update();
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
	if (_reversable) {
		if ( source->isEnzyme()) {  drawArrow(painter,+1); return; }	//forward arrow
		if ( dest->isEnzyme() ) { drawArrow(painter,-1);  return; }	//reverse arrow
			drawArrow(painter,-1);	//reverse arrow
			drawArrow(painter,+1);	//reverse arrow
	} else {
		drawArrow(painter,0);	//forward arrow
	}
}

void Edge::timerEvent(QTimerEvent *event) {
	_timerStep++;
	//cerr << "\t edge: " << _timerStep << " " << _timerMaxSteps << endl;
	if (_timerStep > _timerMaxSteps ) killAnimation();
	update();
}

void Edge::startAnimation() { 
	killAnimation();
	_timerStep = 0;
    _timerId = startTimer(_timerSpeed); //50 ms steps
}

void Edge::killAnimation() { 
	if (_timerId) killTimer(_timerId);
	_timerStep=0;
	_timerId=0;
}

