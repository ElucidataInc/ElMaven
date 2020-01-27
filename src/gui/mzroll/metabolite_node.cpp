#include "barplot.h"
#include "Compound.h"
#include "globals.h"
#include "mainwindow.h"
#include "metabolite_node.h"
#include "mzSample.h"
#include "pathwaywidget.h"
#include "Scan.h"

MetaboliteNode::MetaboliteNode(QGraphicsItem* parent, QGraphicsScene *scene):Node(parent,scene) {
		setMolClass(Node::Metabolite);
		setLinked(false);
        setAcceptDrops(true);

		//QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
		//effect->setOffset(3); setGraphicsEffect(effect);
		coordinates=NULL;
                _barplot=NULL;
		_showCoordinates=false;
		_defaultAtomSize=20;
		_selectedAtom=-1;
                _showBarPlot=false;
}


void MetaboliteNode::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget *)
{
    Compound* c = getCompound();
    _shape = QPainterPath();

	if (isCofactor()) {
            if(coordinates && coordinates->atoms.size()<20){
                paintCarbonBalls(painter);
                paintLabel(painter);
            } else {
                paintCofactor(painter);
            }
	} else if (coordinates && _showCoordinates==true) {	//else if (c && c->formula.length())
		paintCarbonBalls(painter);
	} else {
		paintMetabolite(painter);
	}

        if(_showBarPlot){
            addBarPlot();
         } else if (_barplot) {
            _barplot->hide();
        }
}

void MetaboliteNode::addBarPlot() {
    Compound* c = getCompound();
    if (!c) return;

    if (!_barplot) { _barplot = new BarPlot(this,scene());  }

    if (_barplot) {
         PathwayWidget* parentView = (PathwayWidget*)(this->scene()->views().first());
        _barplot->setMainWindow(parentView->getMainWindow());
        _barplot->showIntensityText(false);
        _barplot->showSampleNames(false);
        _barplot->showQValueType(false);
     }

}

void MetaboliteNode::paintCofactor(QPainter* painter) { 
    _shape.addEllipse(QRect(-5,-5,10,10));
    paintLabel(painter);
    showLabel(false);
}

float MetaboliteNode::getTextWidth() { 
	return 1;
}

void MetaboliteNode::paintCarbonBalls(QPainter *painter) {

	if (!coordinates) return;
	QVector<QPointF> atomsXY = coordinates->coord;
	int atomCount = atomsXY.size();
    if (atomCount == 0) return;

	double concentration  = getConcentration();
	double labeledConcentration = getLabeledConcentration();
	double circleSize = computeNodeSize(concentration);
	
	double minX, minY, maxX, maxY;
	minX=maxX =atomsXY[0].x();
	minY=maxY =atomsXY[0].y();
	for(int i=0; i < atomsXY.size(); i++ ) { 
		if (minX> atomsXY[i].x()) minX=atomsXY[i].x();
		if (minY> atomsXY[i].y()) minY=atomsXY[i].y();
		if (maxX< atomsXY[i].x()) maxX=atomsXY[i].x();
		if (maxY< atomsXY[i].y()) maxY=atomsXY[i].y();
	}
	double W = maxX-minX;
	double H = maxY-minY;
	double range = max(W,H);
    if(range == 0) return;

	atomMap.clear();
	_shape = QPainterPath();

	double atomSize=circleSize;
	double scale = atomSize;
	if (atomCount > 4)  scale +=atomCount/4*atomSize;

	QColor carbon = QColor(30,30,30); carbon.setAlphaF(0.9);
	for(int i=0; i < atomCount; i++ ) {
		QString atom  =  coordinates->atoms[i];
		double xcoord =   -scale/2+((atomsXY[i].x()-minX)/range) * scale;
		double ycoord =   -scale/2+((atomsXY[i].y()-minY)/range) * scale;
		double R = atomSize;

		painter->setPen(Qt::gray);

		if (atom.startsWith("C")) { painter->setBrush(carbon); }
		else if (atom.startsWith("O")) { painter->setBrush(Qt::darkRed); }
		else if (atom.startsWith("N")) { painter->setBrush(Qt::darkBlue);  } 
		else if (atom.startsWith("P")) { painter->setBrush(Qt::darkCyan); }
		else if (atom.startsWith("S")) { painter->setBrush(Qt::darkYellow); }
		else { painter->setBrush(Qt::gray); }

		//highlight selected atom
		if ( i == _selectedAtom ) { 
                    //qDebug() << "selectedAtom: " << getId() << " " << getNote() << " " << _selectedAtom << " " << atom;
                    painter->setPen(Qt::black);
                    painter->setBrush(Qt::yellow);
		}

		//shift coordinates to center
		atomMap.push_back(QPointF(xcoord,ycoord));
		xcoord -= R/2.0; ycoord -= R/2.0;
		_shape.addEllipse(xcoord,ycoord,R,R);
		painter->drawEllipse(xcoord,ycoord,R,R);
	}
	//painter->setBrush(Qt::NoBrush);
   // painter->drawRect(boundingRect());

}
/*
void MetaboliteNode::paintCarbonBalls(QPainter *painter) {
    Compound* c = getCompound();
	if(!c) return;
	MassCalculator mcalc;

	map<string,int>composition= mcalc.getComposition(c->formula);
	int Ccount = composition["C"];

	double concentration  = getConcentration();
	double labeledConcentration = getLabeledConcentration();
	double circleSize = computeNodeSize(concentration);
	int cPerLine=6;
	int nrows = (Ccount/cPerLine+1);
	int ncols = Ccount/nrows;

	int maxW=0; int maxH;
	if (Ccount > 0 ) {
    	painter->setBrush(Qt::gray);
		for(int i=0; i < Ccount; i++ ) {
			float xcoord =  (i % cPerLine)*circleSize-circleSize/2;
			float ycoord = -circleSize/2.0 + ((int) i/cPerLine * circleSize);
			xcoord -= (ncols*circleSize/2)/2;
			ycoord -= ((float)nrows*circleSize/2)/2;
    		painter->drawEllipse(xcoord,ycoord,circleSize,circleSize);
		}
	}

	setBoundingBox(ncols*circleSize,nrows*circleSize);
	painter->setBrush(Qt::NoBrush);
    painter->drawRect(boundingRect());
}
*/


void MetaboliteNode::paintMetabolite(QPainter *painter) {
    Compound* c = getCompound();

	double concentration  = getConcentration();
	double labeledConcentration = getLabeledConcentration();

	double circleSize = computeNodeSize(concentration);

	float  fractionLabeld = 0;
	if ( labeledConcentration >0 && concentration > 0 ) {
		fractionLabeld=labeledConcentration/concentration;
	}

	//qDebug() << "paintMetabolite() " << getId() << " " << getConcentration() << " " << fractionLabeld;
    painter->setPen(Qt::black);

    QColor color1 = Qt::white;
    QColor color2 = Qt::gray;
        if ( getConcentration()>0)  color2 = Qt::blue;

	QPen pen(Qt::black,0);
	if ( isHighlighted() ) pen.setColor(Qt::yellow);

    QRadialGradient gradient(QPointF(-circleSize/3,-circleSize/3), circleSize);
    gradient.setColorAt(0.2,  color2.lighter());
    gradient.setColorAt(0.9,  color2);
    gradient.setColorAt(1.0,  color2.darker());
    painter->setBrush(gradient);
    painter->setPen(pen);
    painter->drawEllipse(-circleSize/2,-circleSize/2,circleSize,circleSize);
    _shape.addEllipse(-circleSize/2,-circleSize/2,circleSize,circleSize);

	//show fraction labeled
	if ( fractionLabeld > 0 ) {
			QColor color2 = Qt::red;
    		gradient.setColorAt(0.2,color2.lighter());
    		gradient.setColorAt(0.9,color2);
    		gradient.setColorAt(1.0,color2.darker());
    		painter->setBrush(gradient);
			painter->setPen(Qt::NoPen);
			double pieSize = fractionLabeld;
			painter->drawPie(-circleSize/2,-circleSize/2,circleSize,circleSize,0,pieSize*360*16);
	}
	setNodeSize(circleSize);

	/*
	painter->setBrush(Qt::NoBrush);
	painter->setPen(QPen(Qt::black));
    painter->drawRect(boundingRect());
	*/
}


void MetaboliteNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsItem::mouseDoubleClickEvent(event);

	Compound* c = getCompound();
	if(!c) return;

	float xpos = event->pos().x();
	float ypos = event->pos().y();

	_selectedAtom=-1;  //clear atom selection
	float dist=FLT_MAX;
	for(int i=0; i < atomMap.size(); i++ ) { 
		float xdist = atomMap[i].x()-xpos;
		float ydist = atomMap[i].y()-ypos;
		if( abs(xdist) < _defaultAtomSize/2 && abs(ydist) < _defaultAtomSize/2 && sqrt(xdist*xdist+ydist*ydist)<dist) {
			_selectedAtom=i;
		}
	}

	if (_selectedAtom >= 0 ) {
		qDebug() << "atom=" << coordinates->atoms[_selectedAtom] << " atom# " << _selectedAtom;
	}

	 if (_selectedAtom >= 0) Q_EMIT(atomSelected(c,_selectedAtom));
	//expandOnCompound();
}


void MetaboliteNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
	setZValue(zValue()+1);
	Compound* c = getCompound();
	if ( c ) {
    	Q_EMIT(nodePressed(this));
		Q_EMIT compoundFocused(c);
        update();
	}
}

void MetaboliteNode::hoverEnterEvent (QGraphicsSceneHoverEvent*event ) {
    QGraphicsItem::hoverEnterEvent(event);
	//QString mynote=QString::number(pos().x()) + " " + QString::number(pos().y());
	//setToolTip(mynote);
	//
	setToolTip(getNote());
	setZValue(zValue()+1);
	setHighlighted(true);
	Compound* c = getCompound();
	if (c )  {
		Q_EMIT compoundHover( c );
                QString title(c->name().c_str());
		if(_graph) _graph->setTitle(title);
		update(); 
	}

}

void MetaboliteNode::hoverLeaveEvent ( QGraphicsSceneHoverEvent*) {
	setHighlighted(false);
	setZValue(zValue()-1);
    //QGraphicsItem::hoverLeaveEvent(event);
    update(); 
}

void MetaboliteNode::editGroup() {
}


void MetaboliteNode::expandOnCompound() {
   Compound* c = getCompound();
   if (c!=NULL) Q_EMIT(expandOnCompound(c));
}

void MetaboliteNode::contextMenuEvent (QGraphicsSceneContextMenuEvent * event ) {
    QMenu menu;

    if ( getCompound()) {
        QAction* a1 = menu.addAction("Edit Group");
        connect(a1, SIGNAL(triggered()),SLOT(editGroup()));

        QAction* a0 = menu.addAction("Unlink Group");
        connect(a0, SIGNAL(triggered()),SLOT(unlinkGroup()));
    }

    QAction* a1 = menu.addAction("Remove Metabolite");
    connect(a1, SIGNAL(triggered()),getGraphWidget(),SLOT(removeSelectedNodes()));

    QAction* a2 = menu.addAction("Expend on Metabolite");
    connect(a2, SIGNAL(triggered()),SLOT(expandOnCompound()));


 //   QAction* a3 = menu.addAction("Show BarPlot");
 //  connect(a3, SIGNAL(triggered()),SLOT(showBarPlot()));


    QAction *selectedAction = menu.exec(event->screenPos());
}

void MetaboliteNode::keyPressEvent(QKeyEvent *e ) {
	if (e->key() == Qt::Key_Delete ) { unlinkGroup(); }
}
	

void MetaboliteNode::dropEvent( QGraphicsSceneDragDropEvent * event ) { 
    qDebug() << "METABLITENODE:: Dropping...";
    if (event) {
        QTreeWidget* treeWidget = qobject_cast<QTreeWidget*>(event->source());
        if (treeWidget) {
            qDebug() << "DROP FROM TreeWidet:" << treeWidget;
            QTreeWidgetItem *item = treeWidget->currentItem();
            if (!item) return;
            QVariant v = item->data(0,Qt::UserRole);
            PeakGroup*  group =  v.value<PeakGroup*>();

            if (group && group->getCompound()) { 
                setCompound(group->getCompound()); 
                qDebug() << "Setting Compound:" << group->getCompound()->name().c_str();
            }
        }
    }
}

void MetaboliteNode::dragEnterEvent ( QGraphicsSceneDragDropEvent * event ) {
    qDebug() << "METABLITENODE:: Entering..";
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        event->setDropAction(Qt::LinkAction);
        event->acceptProposedAction();
        event->accept();
    } else {
        return;
    }
}

void MetaboliteNode::dragLeaveEvent ( QGraphicsSceneDragDropEvent * event ) {
    qDebug() << "METABLITENODE: Leaving..";
}


