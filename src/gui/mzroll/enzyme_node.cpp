#include "Compound.h"
#include "datastructures/mzSlice.h"
#include "edge.h"
#include "enzyme_node.h"
#include "globals.h"
#include "graphwidget.h"
#include "Peak.h"
#include "PeakGroup.h"
#include "Scan.h"
#include "mzSample.h"

EnzymeNode::EnzymeNode(QGraphicsItem* parent, QGraphicsScene *scene):Node(parent,scene) {
		setMolClass(Node::Enzyme);
		setZValue(3);
		showLabel(false);
}

void EnzymeNode::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget *)
{
	paintEnzyme(painter);
}

float EnzymeNode::summary() {
	cerr << "\tTotal: " << getInitConcentration() << endl;
	cerr << "\tFree: " << _freeConcentration << endl;
	cerr << "\tIn Complex: " << _complexConcentration << endl;
}


float EnzymeNode::getFreeConcentration() { return _freeConcentration; }

float EnzymeNode::computeComplexFormation() { 
	cerr << "computeComplexFormation() " << endl;
	_complexConcentration=0;
	float complexForming=0;
	float complexBreaking=0;
	Q_FOREACH (Edge* e, edges() ) {
	}
	_complexConcentration = complexForming - complexBreaking;
	_freeConcentration    = getInitConcentration()-_complexConcentration;
	return _complexConcentration;
}


void EnzymeNode::setInitConcentration(float x) { 
		Node::setInitConcentration(x); 
		setConcentration(x);
		setFreeConcentration(x); 
		setComplexConcentration(0.0); 
}

void EnzymeNode::paintEnzyme(QPainter *painter) {

	//qDebug() << "paintEnzyme() " << getNote() << " " << pos().x() << " " << pos().y();

	_shape.addEllipse(-2,2,4,4);
	painter->drawPath(_shape);

	if ( isSelected() || isHighlighted() || labelIsVisible() )  {
		 paintLabel(painter);
		_shape = QPainterPath();
		float scale = 0.05;
		if ( _graph ) scale *= _graph->getNodeSizeScale();
		double circleSize= _graph->getAvgEdgeLength()*0.05;

		painter->setPen(Qt::gray);
		painter->setBrush(Qt::gray);
		painter->drawEllipse(-circleSize/2,-circleSize/2,circleSize,circleSize);
		_shape.addEllipse(-circleSize/2,-circleSize/2,circleSize,circleSize);
	}
}

float EnzymeNode::getMinRate() {
	//called with enzyme node

	float minRate=-1;
	Q_FOREACH(Edge* e, edges() ) {
		float x = e->getRateForward();
		if ( minRate < 0 || x < minRate ) minRate=x;
	}
	return minRate;
}

void EnzymeNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
	Reaction* r = getReaction();
	if (r) r->setReversable( ! r->reversable ); 

	Q_FOREACH(Edge* e, edges()) {
		e->reverseDirection();
	}
}

void EnzymeNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent(event);
	layoutCofactors();
    update();
}

void EnzymeNode::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
	Reaction* r = getReaction(); 
	setHighlighted(true);
	Q_FOREACH(Edge* e, edges() ) {
		e->setHighlighted(true);
		if(e->sourceNode()) { e->sourceNode()->setHighlighted(true); e->sourceNode()->update(); }
		if(e->destNode())   { e->destNode()->setHighlighted(true); e->destNode()->update(); }
		e->update();
	}

 	if (r) {
		QStringList reactants;
		QStringList products;

		for( unsigned int j=0; j < r->reactants.size(); j++ ) {
                                if(r->reactants[j] != NULL) reactants << QString(r->reactants[j]->name().c_str());
		}

		for( unsigned int j=0; j < r->products.size(); j++ )  {
                                if(r->products[j] != NULL) products << QString(r->products[j]->name().c_str());
		}
		QString direction(" => ");
		if ( r->reversable ) direction = " &lt;=&gt; ";
		QString reaction = reactants.join(" + ") + direction + products.join(" + ");
		setToolTip(reaction);
	}

	//if (r) Q_EMIT enzymeFocused(r); 
    update();
}

void EnzymeNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {

	setHighlighted(false);
	Q_FOREACH(Edge* e, edges() ) {
		e->setHighlighted(false);
		if(e->sourceNode()) { e->sourceNode()->setHighlighted(false); e->sourceNode()->update(); }
		if(e->destNode())   { e->destNode()->setHighlighted(false); e->destNode()->update(); }
		e->update();
	}
    update();
}


void EnzymeNode::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsItem::mousePressEvent(event);
	this->setSelected(true);
	Reaction* r = getReaction(); 
	if (r) Q_EMIT enzymeFocused(r); 
    update();
}

void EnzymeNode::layoutCofactors() {
	QRectF box = boundingRect();
	QPointF epos = pos() + QPointF(box.width()/2+10, 0);
	double angl1 = 0;
	double angl2 = 0;
	double D  = 30;
	double angleTo  =  0;
	double angleFrom=  0;
	bool hasCofactors=false;

	Q_FOREACH(Edge* e, edges()) {
		if ( e->sourceNode()->isCofactor() || e->destNode()->isCofactor()) { hasCofactors=true; continue; }
		if ( e->sourceNode() == this ) { angleFrom = e->angle(); }	//angle from Enzyme
		if ( e->destNode()   == this ) { angleTo  =  e->angle(); }	//angle to Enzyme
	}

	//cerr << "angle --> E --> =" << angleTo << " " << angleFrom << endl;

	if (hasCofactors==false) return;
	Q_FOREACH(Edge* e, edges()) {
		if ( e->sourceNode()->isCofactor() == true) { 
		  	 angl1 += 0.3;
			 float newAngle = (angleTo+Pi)+angl1;
			 QPointF npos = epos + QPointF(cos(newAngle)*D,sin(newAngle)*D);
			 e->sourceNode()->setPos(npos);
		} 
		
		if ( e->destNode()->isCofactor() == true) 	 { 
		  	 angl2 += 0.3;
			 float newAngle = angleFrom+angl2;
			 QPointF npos = epos + QPointF(cos(newAngle)*D,sin(newAngle)*D);
			 e->destNode()->setPos(npos);
		} 

	}
}

QPointF EnzymeNode::activeSitePosition() { 

	QList<QPointF> links;
	QList<QPointF> positions; 
	QRectF box = boundingRect();
	float hW = box.width()/2;
	float hH = box.height()/2;
	QPointF bestPoint = pos();
	/*
	Q_FOREACH(Edge* e, edges()) {
		if ( e->sourceNode()->isCofactor() || e->destNode()->isCofactor()) continue;
		if ( e->sourceNode() == this ) {  links << e->destNode()->pos(); 	}
		if ( e->destNode()   == this ) {  links << e->sourceNode()->pos();  }
	}
	positions << box.bottomLeft() << box.topLeft() << box.topRight() << box.bottomRight();

	QPointF bestPoint;
	float minDist = 100000;
	Q_FOREACH( QPointF a, positions ) {
		float totalDist=0;
		Q_FOREACH(QPointF b, links ) { 
			totalDist += sqrt(POW2(a.x()-b.x())+POW2(a.y()-b.y()));
		}
		if ( totalDist < minDist ) { 
				bestPoint = a;
				minDist = totalDist;
		}
	}
		
	return bestPoint + QPoint(boundingRect().width()/2+10,0);
	*/
	return bestPoint;
}

void EnzymeNode::contextMenuEvent (QGraphicsSceneContextMenuEvent * event ) {
		QMenu menu;

		QAction* a1 = menu.addAction("Remove Reaction");
		connect(a1, SIGNAL(triggered()),getGraphWidget(),SLOT(removeSelectedNodes()));

		QAction *selectedAction = menu.exec(event->screenPos());
}
