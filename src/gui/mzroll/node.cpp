#include "node.h"
#include "Compound.h"
#include "edge.h"
#include "graphwidget.h"
#include "Peak.h"
#include "PeakGroup.h"
#include "Scan.h"
#include "mzSample.h"

Node::Node(QGraphicsItem* parent, QGraphicsScene *scene):QGraphicsItem(parent)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    acceptHoverEvents();
    setVisible(false);
    setHighlighted(false);

    setInitConcentration(0);
    setLabeledConcentration(0);
    setConcentration(0);

    setZValue(1);
    setFontSize(0);
    setFixedPosition(false);
    showLabel(true);

    setMolClass(Node::Unassigned);
    setPos(0,0);
    setScalingFactor(1);
    setDepth(-1);

    _nodeSize=5;
    _label=NULL;
    _labelBox=NULL;

    _brush = QBrush(Qt::white);
    _graph = NULL;

    setAcceptDrops(true);
    if(scene) scene->addItem(this);
}

Node::~Node() {
	if (_label && _label->scene()) { scene()->removeItem(_label); } 
	if (_labelBox && _labelBox->scene()) { scene()->removeItem(_labelBox); } 

	if( _label) delete(_label);
	if( _labelBox) delete(_labelBox);
}

QList<Edge*> Node::edgesIn() { 
		QList<Edge*>elist;
		Q_FOREACH(Edge* e, edgeList) if (e->destNode() == this ) elist << e;
		return elist;
}

QList<Edge*> Node::edgesOut() { 
		QList<Edge*>elist;
		Q_FOREACH(Edge* e, edgeList) if (e->sourceNode() == this ) elist << e;
		return elist;
}

void Node::addEdge(Edge *edge) { 
    edgeList << edge; edge->adjust(); 
}

QList<Edge*> Node::findConnectedEdges(Node* other) {
	QList<Edge*>elist;
	Q_FOREACH(Edge* e, edgeList) if (e->sourceNode() == other || e->destNode() == other ) elist << e;
    return elist;
}

bool Node::unlinkGroup() { 
	if (getDataReference() && isMetabolite()) {
		Compound* c = (Compound*) getDataReference();
                if (c ) {

				QVector<float>v;
				setConcentrations(v);
				setLabeledConcentrations(v);
				setInitConcentration(0);
				setConcentration(0);
				setLabeledConcentration(0);
				update();
				return true;
		}
	}
	return false;
}

bool Node::setNewPos(float x, float y) {
	if (isFixedPosition()) return false;
	if (pos().x() == x && pos().y() == y ) return true;

	QRectF SR = scene()->sceneRect();
	int W=scene()->width();
	int H=scene()->height();
	
	//adjust using grid
	float fx = (x-SR.x())/(SR.width()-SR.x()); 
	fx=(float)(int)(fx*50)/50; //grid
	x=SR.x()+fx*(SR.width()-SR.x());

	float fy = (y-SR.y())/(SR.height()-SR.y()); 
	fy=(float)(int)(fy*50)/50; //grid
	y=SR.y()+fy*(SR.height()-SR.y());

    if (pos().x() == x && pos().y() == y ) return true;
    QTransform transform;

    if (scene()->itemAt(x,y,transform) == this) {
		setPos(x,y); 
	} else {
		for(int j=0; j<100; j++ ) {
            if (scene()->itemAt(x,y,transform) && scene()->itemAt(x,y,transform) != this) {
                    x+=cos(j)*j; y+=sin(j)*j;
			}
		}
		setPos(x,y); 
	}
	return true;
}

double Node::computeNodeSize(float concentration) { 

	float scale = 0.25;
	if ( _graph ) scale *= _graph->getNodeSizeScale();

	float totalArea=scene()->height()*scene()->width();
	if (totalArea==0) return 0;

	double circleSize= _graph->getAvgEdgeLength()*scale*getScalingFactor();

    if ( _graph->getNodeSizeNormalization() == GraphWidget::RelativeSize ) {
	setZValue(zValue()+1);
		if ( getInitConcentration() > 0 ) {
    		float conRatio = concentration/getInitConcentration();
    		if (conRatio < 0.1) conRatio = 0.1; 
			if (conRatio > 8) conRatio = 8;
    		circleSize = circleSize*conRatio;
		} else {
    		circleSize = circleSize*0.1;
		}

	} else if (_graph->getNodeSizeNormalization() == GraphWidget::AbsoluteSize ) {

			if (concentration > 1 )  
					circleSize *= log2(concentration);

	}  else if (_graph->getNodeSizeNormalization() == GraphWidget::PairwiseSize ) {
			float concRatio=concentration;
			if (concRatio < 0.1) concRatio = 0.1; 
			if (concRatio > 20) concRatio = 20;
    		circleSize = sqrt(circleSize*concRatio);
	}



	float area = POW2(circleSize/2)*3.15;
	//cerr << area << "t=" << totalArea << endl;
	if (area > totalArea/10) circleSize=sqrt(totalArea/10);
	if (circleSize < 1 ) circleSize=0;
	//qDebug() << getId() << " " << concentration << " " << circleSize;
    _nodeSize = circleSize;
	return circleSize;
}

float Node::calculateMetaboliteConcentrations() { 

}
		
float Node::getFontSize() { 
    if(!scene()) return 0;
	float scale=1; 
    int height = 100;
	if(_graph) {
        scale *=_graph->getLabelSizeScale();
	    height=_graph->getAvgEdgeLength();
		if (height > scene()->height()/5) height=scene()->height()/5;
    }

	if (_fontSize>0) return _fontSize*scale;

	float fontSize=0;
	if( isMetabolite() )  fontSize=0.12*height*scale;
	else if (isEnzyme() ) fontSize=0.08*height*scale;
	else fontSize = 0.10*height*scale;

	if ( fontSize > scene()->height()/20 ) fontSize = scene()->height()/20;
	if ( fontSize < 1 ) fontSize=0;
	return fontSize;
}

QRect Node::getTextRect(const QString text, float fontsize=0.0) { 

	if (fontsize==0) fontsize= getFontSize();
	QFont font("Helvetica");
	font.setPointSizeF(fontsize);
	QFontMetrics fm( font );
	return fm.boundingRect(text);
}

float Node::getTextWidth() { 
	return getTextRect(getNote()).width();
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget *)
{
	_shape=QPainterPath();
	int nodesize = computeNodeSize(getConcentration());
    painter->setBrush(QBrush(Qt::gray));
    painter->drawEllipse(-nodesize/2+1,-nodesize/2+1,nodesize,nodesize);
    painter->setBrush(QBrush(_brush));
    painter->drawEllipse(-nodesize/2,-nodesize/2,nodesize,nodesize);
    _shape.addEllipse(-nodesize/2,-nodesize/2,nodesize,nodesize);
}

void Node::drawLabel() {
    if (!scene()) return;
    if ( _showLabelFlag == false ) {
        if (_label) _label->hide();
        if (_labelBox) _labelBox->hide();
        return;
    }

    float fontSize=getFontSize();

    if (fontSize < 1) {
        if (_label) _label->hide();
        if (_labelBox) _labelBox->hide();
        return;
    }

    QFont f("Helvetica",fontSize);

    int itemW=_shape.boundingRect().width();
    int itemH=_shape.boundingRect().height();
    if (!_label) { _label = new QGraphicsTextItem(this); }
    if (!_labelBox) {  _labelBox = new QGraphicsRectItem(this); }

    if (_label && _labelBox)  {
        _label->setFont(f);
        _label->setHtml("<b>" + _note + "</b>");

        int lw=_label->boundingRect().width();
        int lh=_label->boundingRect().height();

        _label->setPos(-lw/2,lh/2+5);
        _label->setZValue(1);

        QColor brush = Qt::white;
        brush.setAlphaF(0.5);
        _labelBox->setRect(_label->boundingRect());
        _labelBox->setPen(QPen(Qt::black));
        _labelBox->setBrush(brush);
        _labelBox->setZValue(0);
        _labelBox->setPos(-lw/2,lh/2+5);

        _labelBox->show();
        _label->show();

        _shape.moveTo(-lw/2,lh/2+5);
        _shape.addRect(_labelBox->boundingRect());

    }
			
}

void Node::paintLabel(QPainter *painter) {
	float fontSize=getFontSize();
	if (fontSize == 0 ) return;

	int itemW=_shape.boundingRect().width();
	int itemH=_shape.boundingRect().height();

	QFont font("Helvetica",fontSize);
	QRect textbox = getTextRect(_note,fontSize);
	textbox.adjust(-2,-2,+2,+2);
	textbox.moveCenter(QPoint(0,itemW/2+2+textbox.height()/2));

	painter->setPen(QPen(Qt::black));
	if (isSelected() || isHighlighted())  painter->setPen(QPen(Qt::yellow));

	painter->setBrush(Qt::white);
	painter->drawRect(textbox);
	painter->setPen(QPen(Qt::black));
	QFont fontSmall("Helvetica",fontSize);
	painter->setFont(fontSmall);
	painter->drawText(textbox,_note, QTextOption(Qt::AlignCenter));

        _shape.moveTo(QPoint(0,itemW/2+2+textbox.height()/2));
        _shape.addRect(textbox);
}


void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    Q_EMIT(nodeDoubleClicked(this));
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	scene()->clearSelection();
    Q_EMIT(nodePressed(this));
}


void Node::mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) {
    QGraphicsItem::mouseMoveEvent(event);
    Q_FOREACH (Edge *edge, edgeList) edge->adjust();
    scene()->update();
	setNewPos(pos().x(), pos().y());
    Q_EMIT(nodeMoved(this));
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
/*    switch (change) {
    case ItemPositionHasChanged:
        qDebug() << "itemChange:";
        Q_FOREACH (Edge *edge, edgeList) edge->adjust();
        Q_EMIT(nodeMoved(this));
        break;
    default:
        break;
    };
*/
    return QGraphicsItem::itemChange(change, value);

}

void Node::removeEdge(Edge* edge) { 
	edgeList.removeAll(edge);
}


void Node::setGraphWidget(GraphWidget *g) { _graph = g; }
GraphWidget* Node::getGraphWidget() { return _graph; }

void Node::wheelEvent ( QGraphicsSceneWheelEvent * event ) {
	if ( event->delta() > 0 ) {
			setScalingFactor( getScalingFactor()*1.2 );
	} else {
			setScalingFactor( getScalingFactor()*0.8 );
	}
	cerr << "scalingFactor=" << getScalingFactor() << endl;
}	
