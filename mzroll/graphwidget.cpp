#include "graphwidget.h" 


void MyScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent ) {
        down =  mouseEvent->buttonDownScenePos(Qt::LeftButton);
        QGraphicsScene::mousePressEvent(mouseEvent);
        emit(mousePressed());
        qDebug() << "mousePressed() ";
}

void MyScene::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent ) {
    lastCursorPos = mouseEvent->scenePos();

    if (_showArrow) {
        if (!_arrow) {
            _arrow = new QGraphicsLineItem(0,0,0,0);
            QPen redpen(Qt::red, 1,  Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
            _arrow->setPen(redpen);
            addItem(_arrow);
        }
        _arrow->setLine(down.x(), down.y(), lastCursorPos.x(), lastCursorPos.y());
        _arrow->show();

    } else {
        if (_arrow) _arrow->hide();
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void MyScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent ) {
    up =  mouseEvent->scenePos();
    int width =  up.x()-down.x();
    int height = up.y()-down.y();
    QRectF area(down.x(),down.y(),width,height);

    QGraphicsScene::mouseReleaseEvent(mouseEvent);

    if ( mouseEvent->modifiers() == Qt::ControlModifier ) {
        emit(zoomArea(area));
    } else {
        emit(mouseReleased());
    }
    return;
};


GraphWidget::GraphWidget()
{
    _myscene = new MyScene(this);
    _myscene->showArrow(false);

    setScene(_myscene);

    //setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene()->setSceneRect(-width()/2,-height()/2,width(),height()); //scene = physical dimentions

    setCacheMode(CacheBackground);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);
    setDragMode(QGraphicsView::RubberBandDrag);
    setNodeSizeNormalization(GraphWidget::AbsoluteSize);

    setNodeSizeScale(1);
    setLabelSizeScale(1);
    setEdgeSizeScale(1);
    setLayoutAlgorithm(FMMM);

    connect(scene(),SIGNAL(zoomArea(QRectF)), this,SLOT(zoomArea(QRectF)));
    layoutTree = new QTreeWidget(this);
    layoutTree->hide();

    _title = NULL;
    _averageEdgeSize=50;

}

GraphWidget::~GraphWidget() { clear(); }

void GraphWidget::clear() { 
    nodelist.clear();
    layoutMap.clear();
    layoutTree->clear();
    _myscene->clear();
    _backgroundImage = QPixmap(0,0);
    _title=NULL;
}

Node* GraphWidget::addNode(string id, void* data) {
	if ( nodelist.count(id.c_str()) == 0 ) {
			Node* n = new Node(0,scene());
			n->setId(id.c_str());
			n->setNote(id.c_str());
			n->setDataReference(data);
			n->setGraphWidget(this);
			nodelist[id.c_str()]=n;
			return n;
	} else {
			return nodelist[id.c_str()];
	}
}

Edge* GraphWidget::findEdge(Node* n1, Node* n2) {
		if (!n1 || !n2 ) return NULL;

		foreach( Edge* e, n1->edges() ) {
			if (e->sourceNode() == n1 && e->destNode() == n2 ) return e;
			if (e->sourceNode() == n2 && e->destNode() == n1 ) return e;
		}

		foreach( Edge* e, n2->edges() ) {
			if (e->sourceNode() == n1 && e->destNode() == n2 ) return e;
			if (e->sourceNode() == n2 && e->destNode() == n1 ) return e;
		}
		return NULL;
}

Edge* GraphWidget::addEdge(Node* n1, Node* n2, string note, void* data) {
		if ( ! n1 || ! n2 ) return NULL; 

		

		Edge* e = new Edge();
		e->setSourceNode(n1);
		e->setDestNode(n2);
		e->setNote(note.c_str());
		e->setData(data);
		n1->addEdge(e);
		n2->addEdge(e);
		scene()->addItem(e);
		return(e);
}

void GraphWidget::removeNode(Node* n) {
	if (!n) return;
	qDebug() << "Removing: " << n->getId() << endl;
	n->setVisible(false);
	scene()->removeItem(n);
	foreach (Edge* e, n->edges() ) {
		e->setVisible(false);
		scene()->removeItem(e);
		n->removeEdge(e);
		if (e->sourceNode() == n) e->destNode()->removeEdge(e);
		if (e->destNode()   == n) e->sourceNode()->removeEdge(e);
		delete(e);
	}

	if (layoutMap.contains(n)) {
		layoutMap.clear();
		layoutTree->clear();
	}

	if (nodelist.contains(n->getId()) ) {
		nodelist.remove(n->getId());
	}

	delete(n);
}

void GraphWidget::removeSelectedNodes() {
    foreach (QGraphicsItem *item, scene()->selectedItems()) {
		if (Node *node = qgraphicsitem_cast<Node *>(item)) {
			if (node->isMetabolite() && node->unlinkGroup() ) {
				continue;
			}
			removeNode(node);
		}
	}
}

QList<Node*> GraphWidget::getNodes(int type) {
	QList<Node*>x;
	foreach(Node* n, nodelist ) if (n->molClass() == type) x<<n; 
	return x;
}

void GraphWidget::itemMoved()
{
	cerr << "Item Moved" << endl;
}

void GraphWidget::randomNodePositions() { 
    foreach (QGraphicsItem *item, scene()->items()) {
          if (qgraphicsitem_cast<Node *>(item)) item->setPos(-100 + qrand() % 100, -100 + qrand() % 100);
    }
}


void GraphWidget::setBackgroundImage(QString filename) { 

	if (filename.isEmpty() ) { //clear background image
			_backgroundImageFile.clear();
			_backgroundImage = QPixmap();
	} else {				   //set background image
			_backgroundImageFile=filename; 
			_backgroundImage = QPixmap(_backgroundImageFile);  
	}
}

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    //return;
    Q_UNUSED(rect);

	if (!_backgroundImage.isNull() ) {
		painter->drawPixmap(-_backgroundImage.width()/2,-_backgroundImage.height()/2, _backgroundImage);
		/*
		QGraphicsPixmapItem* backgound = scene()->addPixmap(pixmap);
		backgound->setZValue(-1);
		backgound->setFlag(QGraphicsItem::ItemIsSelectable);
		backgound->setFlag(QGraphicsItem::ItemIsMovable);
		*/
	}

	return;
	/*
	painter->drawRect(sceneRect());
	painter->drawLine(QPointF(0,0), QPointF(w/2, 0));
	painter->drawLine(QPointF(0,0), QPointF(0, h/2));
	painter->drawEllipse(QPointF(0,0), 5,5);
	painter->drawText(w/2-20,h/2-20,QString::number(w) + " " + QString::number(h) );
	*/
}

void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void GraphWidget::setTitle(const QString& titleText) { 
	emit(titleChanged(titleText));
}

void GraphWidget::updateSceneRect() {
	float minX=0;
	float minY=0;
	float maxX=0;
	float maxY=0;
	int itemCount=0;
	float centerX = 0;
	float centerY = 0;

	foreach (Node* item, nodelist ) {
			if (!item->isVisible()) continue;
			if (item->pos().x() < minX) minX = item->pos().x();
			if (item->pos().x() > maxX) maxX = item->pos().x();
			if (item->pos().y() < minY) minY = item->pos().y();
			if (item->pos().y() > maxY) maxY = item->pos().y();
			centerX += item->pos().x();
			centerY += item->pos().y();
			itemCount++;
	}

	if (itemCount) { centerX /= itemCount; centerY /= itemCount; }
	QPointF center(centerX,centerY);

	//shift nodes to center
	//foreach (QGraphicsItem *item, scene()->items()) item->setPos(item->pos()+center);
	//minX += centerX; maxX += centerX;
	//minY += centerY; maxY += centerY;
	minX -= 10; maxX += 10; minY -= 10; maxY += 10;
	float W = maxX-minX;
	float H = maxY-minY;
	
	scene()->setSceneRect(-W/2-50,-H/2-50,W+100,H+100);
    fitInView(sceneRect(),Qt::KeepAspectRatio);
   //qDebug() << "updateSceneRect:  WxH=" << W << " " << H << " " << center << endl;
	return;


	if (W < 300 ) W=300;
	if (H < 300 ) H=300;

	float aH = H*0.2;
	float aW = W*0.2;
	if ( scene()->width() > W )  aW=0;
	if ( scene()->height() > H ) aH=0;
    scene()->setSceneRect(-W/2-aW/2,-H/2-aH/2,W+aW,H+aH);
    fitInView(sceneRect(),Qt::KeepAspectRatio);
    //qDebug() << "updateSceneRect:  WxH=" << W << " " << H << " " << center << endl;
}

	

void GraphWidget::resetZoom() { 
    cerr << "resetZoom()" << endl;
    computeAvgEdgeLength();

    float minX=0;
    float minY=0;
    float maxX=0;
    float maxY=0;
	int itemCount=0;
	float centerX = 0;
	float centerY = 0;

    foreach (QGraphicsItem *item, scene()->items()) {
			if ( item->isVisible() ) {
					if (Node *node = qgraphicsitem_cast<Node *>(item)) {
					float w =item->boundingRect().width();
					float h =item->boundingRect().height();
					if (item->pos().x() < minX) minX = item->pos().x()-w;
					if (item->pos().x() > maxX) maxX = item->pos().x()+w;
					if (item->pos().y() < minY) minY = item->pos().y()-h;
					if (item->pos().y() > maxY) maxY = item->pos().y()+h;
					centerX += item->pos().x();
					centerY += item->pos().y();
					itemCount++;
				}
			}
    }

	minX -= 10; maxX += 10; minY -= 10; maxY += 0;
	if ( itemCount == 0 ) return;

	centerX /= itemCount;
	centerY /= itemCount;
	    
	/*
	float sceneRectWidth=width();
	if ( sceneRectWidth < 100 )  sceneRectWidth=100;
	float sceneRectHeight= sceneRectWidth* (maxY-minY)/(maxX-minX);

	//ratio 
	float Rhw = (maxY-minY)/(maxX-minX);
	float W = 	sceneRectWidth;
	float H  =  sceneRectWidth*Rhw;
	*/

	float scale=1;
	float W = maxX-minX;
	float H = maxY-minY;
    //cerr << "GraphWidget::resetZoom() xDim=" << W << " " << H << " " << scale << endl;

    if ( _averageEdgeSize > 0 ) { scale = 50/_averageEdgeSize; } 
 	W*=scale; 
	H*=scale;
	if (W<300) W=300;
	if (H<300) H=300;
    
	foreach (QGraphicsItem *item, scene()->items()) {
			if ( item->isVisible() ) {
					if (Node *node = qgraphicsitem_cast<Node *>(item)) {
						float x = item->pos().x();
						float y = item->pos().y();
						float newx =  (x-centerX)*scale;
						float newy =  (y-centerY)*scale;
						item->setPos(newx, newy);

					} 
			}
	}

	float aH = H*0.1;
	float aW = W*0.1;
    scene()->setSceneRect(-W/2-aW/2,-H/2-aH/2,W+aW,H+aH);
    fitInView(sceneRect(),Qt::KeepAspectRatio);
	scene()->update();
	computeAvgEdgeLength();
    //cerr << "GraphWidget::resetZoom() sceneSize WxH=" << W << " " << H << " " << scale << endl;
}

void GraphWidget::newLayout() {
	cerr << "newLayout() " << endl;
	clearLayout();
	getLayoutAlgorithm() == Random ? adjustLayout() : layoutOGDF();
	resetZoom();
}

void GraphWidget::updateLayout() {
	cerr << "updateLayout() " << endl;
	if (nodelist.size() == 0 ) return;
	setLayoutAlgorithm(Balloon);

    QPointF zero(0,0);
    foreach(Node* x, nodelist) if(x && x->pos() != zero) x->setFixedPosition(true); 
    foreach(Node* x, nodelist) if(x && !layoutMap.contains(x)) recursiveDepth(x,0); 

	if (layoutMap.size() > 0) {
		for(int i=0; i<layoutTree->topLevelItemCount() ; i++ ) {
			QTreeWidgetItem* x = layoutTree->topLevelItem(i);
			if (x) recursiveDraw(x);
		}
	} else { 
		layoutOGDF();
	}

    foreach(Node* x, nodelist){ if(x) x->setFixedPosition(false); }
	adjustEnzymePositions();
	//updateSceneRect();
}

void GraphWidget::showEdges(bool flag) {
    foreach (Node* n, nodelist ) {
        foreach (Edge* e, n->edges() ) e->setVisible(flag);
    }
}

void GraphWidget::showNodes(bool flag) {
	foreach (Node* n, nodelist ) { n->setVisible(flag); }
}

void GraphWidget::showLabels() {
    showLabels(true);
}

void GraphWidget::showLabels(bool flag) {

    if ( flag == true ) {
        foreach(Node* x, nodelist) {
            if (x->isVisible()) x->drawLabel();
        }
    } else {
	    foreach (Node* n, nodelist ) { 
            n->showLabel(false); 
        }
    }
}


void GraphWidget::hideLongEdges() {
	computeAvgEdgeLength();
	adjustEnzymePositions();

	foreach (Node* n, nodelist ) {
		foreach (Edge* e, n->edges() ) {
			if ( e->length() > 2*getAvgEdgeLength() ) e->hide();
		}
	}
}

void GraphWidget::adjustLayout(Node* n1) {
	cerr << "adjustLayout(n1) " << endl;
    if(!n1) return;
	foreach (Node* n, nodelist) n->setFixedPosition(true);

    n1->setFixedPosition(true);
    foreach (Edge* e, n1->edges() ) { 
        if (e->sourceNode() && e->sourceNode() != n1 ) e->sourceNode()->setFixedPosition(false);
        if (e->destNode()   && e->destNode()   != n1 ) e->destNode()->setFixedPosition(false);
    }
    adjustLayout();
	foreach (Node* n, nodelist) n->setFixedPosition(false);
            
    /*
	foreach (Node* n2, nodelist) {
			if (n1 != n2 && n2->isVisible() && n1->isVisible() && n1->collidesWithItem(n2,Qt::IntersectsItemBoundingRect)) {
				n2->calculateForces();
				n2->advance();
			}
	}
    */
}


void GraphWidget::clearLayout() {
    cerr << "clearLayout() " << endl;
    layoutTree->clear();
    layoutMap.clear();

    if ( nodelist.size() > 0 )  {
        foreach(Node* n, nodelist) { n->setDepth(-1); n->setPos(0,0); }
    }
}

void GraphWidget::addToTree(Node* a, Node* b) {
	//qDebug() << "addToTree: " << a->getNote() << " " << b->getNote();

    if(!a) { //missing parent
       foreach(Node* n, nodelist) {
           if(layoutMap.contains(n) ){ 
               QList<Edge*> edges = n->findConnectedEdges(b); 
               foreach(Edge* e, edges) { 
                   if (layoutMap.contains(e->sourceNode())) { a = e->sourceNode(); break; }
                   if (layoutMap.contains(e->destNode()))   { a = e->destNode(); break; }
               }
           }
       }
    }

    if (!a || !b ) return;
	if (layoutMap.contains(a) == false) return;
	if (layoutMap.contains(b) == true) return;
	QTreeWidgetItem* parent = layoutMap[a];
	if (!parent) return;

    QTreeWidgetItem* item  = new QTreeWidgetItem(parent);
    item->setText(0,b->getId());
    layoutMap[b]=item;
}

void GraphWidget::recursiveDepth(Node* n0,int depth) {
	//cerr << "recursiveDepth() " << endl;

	QTreeWidgetItem* parent=0;
	if (layoutMap.contains(n0)) { 
        parent=layoutMap[n0];
    } else {
        foreach(Edge* e, n0->edges() ) {
            Node* other = e->sourceNode();
            if (other == n0 ) other= e->destNode();
            if (other == n0 ) continue;
	        if (layoutMap.contains(other)) { parent=layoutMap[other]; n0=other; break; }
        }
    } 

	if (!parent) {
		QTreeWidgetItem* item  = new QTreeWidgetItem(layoutTree);
		item->setText(0,n0->getId());
		if (layoutMap.size()) n0->setPos( (float) scene()->width(), 0 );
		layoutMap[n0]=item;
	    parent = item;
	}
	if (!parent) return;

	for(int i=0; i<depth+1; i++) cerr << " ";
	//qDebug() << depth << " " << n0->getId() << " " << n0->getDepth() << " " << n0->boundingRect().width();
	QList<Node*>newnodes;
    foreach(Edge* e, n0->edgesOut() ) {
        Node* other= e->destNode();
        if (!other || other == n0 || other->isCofactor() || layoutMap.contains(other))  continue;
        addToTree(n0,other); 
        newnodes.push_back(other); 
    }

    foreach(Edge* e, n0->edgesIn() ) {
        Node* other = e->sourceNode();
        if (!other || other == n0 || other->isCofactor() || layoutMap.contains(other))  continue;
        addToTree(n0,other); 
        newnodes.push_back(other); 
    }

	foreach(Node* n, newnodes) { recursiveDepth(n,depth+1); }

	
}



void GraphWidget::computeAvgEdgeLength() {
	int count=0; double len=0;
    _averageEdgeSize=50; 

	vector<float>lvector;
	foreach(Node* n, nodelist) { 
		foreach (Edge* e, n->edges() ) {
           if (e->sourceNode()->isVisible() && e->destNode()->isVisible() ){
               QPointF p1 = e->sourceNode()->pos();
               QPointF p2 = e->destNode()->pos();
               float l = QLineF(p1,p2).length();
               if (e->sourceNode()) l -= e->sourceNode()->getNodeSize()/2;
               if (e->destNode())   l -= e->destNode()->getNodeSize()/2;
               if (l) { lvector.push_back(l); }
               count++; 
            }
		}
	}
    if (lvector.size() > 0) _averageEdgeSize = median(lvector);
    return;
}


void GraphWidget::deepChildCount(QTreeWidgetItem* x, int* count) {
	if (x->childCount()==0) return;
	int childcount=x->childCount();
	(*count) += childcount;
	for(int i=0; i<childcount;i++) deepChildCount(x->child(i),count);
	//qDebug() << "deepChildCount: " << x->text(0) << " " << *count;
}

void GraphWidget::recursiveDraw(QTreeWidgetItem* parent) {
	if (!parent) return;
	//int deepcount=0; deepChildCount(item,&deepcount);
	int childcount=parent->childCount();
	QString name = parent->text(0);
	Node* parentNode =  locateNode(name);
	if (parentNode == NULL ) return;

	float px = parentNode->pos().x();
	float py = parentNode->pos().y();

	parentNode->setNewPos(px,py);

    int half = childcount/2;
	if (childcount== 1) half=0;

	float angle0=atan2(py,px);

	for(int i=0; i<childcount;i++) {
		QTreeWidgetItem* child = parent->child(i);
		//QVariant v =  child->data(0,Qt::UserRole);
		//Node* n =  (Node*) v.value<QGraphicsItem*>();
		Node* n = locateNode(child->text(0));
		if (n) {
           float dist=_averageEdgeSize;
		   if (dist < 10 ) dist=10;
		   if (childcount > 2) dist *= log2(childcount);

		   if (getLayoutAlgorithm() == FMMM ) {
				   float cy= py+(i-half)*dist; 
				   float cx= px+dist;
				   n->setNewPos(cx,cy);
		   } else if (getLayoutAlgorithm() == Circular ) {
				   float angle = (float) (i+1)/childcount*2*Pi;
				   float cx= px+cos(angle0+angle)*dist; 
				   float cy= py+sin(angle0+angle)*dist;
				   n->setNewPos(cx,cy);
		   } else if (getLayoutAlgorithm() == Balloon) {
				   float angle = angle0+(float) (i-half)/childcount*0.9*Pi;
				   float cx= px+cos(angle)*dist; 
				   float cy= py+sin(angle)*dist;
				   n->setNewPos(cx,cy);
		   }
		}
	}

	for(int i=0; i<childcount;i++) { recursiveDraw(parent->child(i)); }
}


void GraphWidget::layoutOGDF() { 
    cerr << "layoutOGDF() " << endl;
	if (nodelist.size()==0) return;

	QList<Node*>metabolites = getNodes(Node::Metabolite);
	if(metabolites.size()==0) metabolites = getNodes(Node::Unassigned);

	foreach(Node* n, metabolites) { 
		if(layoutMap.count(n)) continue;
		if(n->edgesIn().size()>0 ) continue;
		recursiveDepth(n,0);
	}
	foreach(Node* n, metabolites) { if(layoutMap.count(n)==0) recursiveDepth(n,0); }

	QList<Node*>enzymes = getNodes(Node::Enzyme);
	foreach(Node* n, enzymes)     { if(layoutMap.count(n)==0) recursiveDepth(n,0); }

	for(int i=0; i<layoutTree->topLevelItemCount() ; i++ ) {
		QTreeWidgetItem* x = layoutTree->topLevelItem(i);
		recursiveDraw(x);
	}
	adjustEnzymePositions();
}

void GraphWidget::dump() {
	foreach(Node* n, nodelist) { 
		qDebug() << "Network:" << n->getId(); 
		if (n->isVisible()) { 
			qDebug() << "\t Node:" << n->getId(); 
			qDebug() << "\t Edge: ";
			foreach(Edge* e, n->edges()) { qDebug() << "\t\t:" << e->destNode()->getId() << " " << e->sourceNode()->getId(); }
			foreach(Edge* e, n->edgesOut()) { qDebug() << "\t\tout:" << e->destNode()->getId(); }
			foreach(Edge* e, n->edgesIn())  { qDebug() << "\t\tin:" <<  e->sourceNode()->getId(); }
		}
    }
}

Node* GraphWidget::locateNode(QString id) {
	if ( nodelist.count(id) ) return nodelist[id];
	return NULL;
}

void GraphWidget::keyPressEvent(QKeyEvent *event)
{
		switch (event->key()) {
				case Qt::Key_Delete:
						removeSelectedNodes();
						break;
				case Qt::Key_0:
						resetZoom();	
						break;
				case Qt::Key_Minus:
						zoomOut();
						break;
				case Qt::Key_Plus:
						zoomIn();
						break;
				case Qt::Key_H:
						showEdges(false);
						break;
				case Qt::Key_L:
						hideLongEdges();
						break;
				case Qt::Key_A:
						adjustEnzymePositions();
						resetZoom();
						break;
				case Qt::Key_E:
						adjustLayout();
						break;
				case Qt::Key_N:
						newLayout();
						break;
				default:
						QGraphicsView::keyPressEvent(event);
		}

	scene()->update();
}

void GraphWidget::adjustEnzymePositions() {
	
		const QPointF origin(0,0);
		foreach (Node* enz, nodelist ) {
			int count=0; float xpos=0; float ypos=0;
			if ( enz->isEnzyme() ) {
					//qDebug() << enz->getId();
					foreach (Edge* e, enz->edges() ) {
						Node* met=e->sourceNode(); if (met==enz) met=e->destNode();
						if (met->isCofactor() || met->pos() == origin)  continue;
						if ( enz == e->destNode() )   { xpos += met->pos().x()*2; ypos += met->pos().y()*2; count +=2; }
						if ( enz == e->sourceNode() ) { xpos += met->pos().x(); ypos += met->pos().y(); count +=1; }
						//qDebug() << "\t" << met->getId();
					}
				}
			if (count>1) { 
				xpos/=count; ypos/=count; 
				enz->setPos(xpos,ypos);
			}
		}
}

void GraphWidget::adjustLayout() { 

    int W=scene()->width()*1.5; int H=scene()->height()*1.5;
    int A = W*H;
    float k = sqrt((float) A/nodelist.size());
    float t_init = 100;
    float t = t_init;
    int rep_max = 100;

    map<Node*, vector<float> > newDisp;
	
	cerr << "GraphWidget::adjustLayout() " << endl;
    for(int itr=0; itr<rep_max; itr++ ) {
		//cerr << "Itr=" << itr << endl;
        
        foreach(Node* n1, nodelist ) {
            // calculate repulsion
            vector<float> disp(2,0); disp[0]=n1->pos().x(); disp[1]=n1->pos().y();
            newDisp[n1] = disp;
            float rep = 0.0;
            foreach(Node* n2, nodelist ) {
                if (n1 == n2) continue;
                if (n1->isFixedPosition()) continue;
                float delta_x = n1->pos().x() - n2->pos().x();
                float delta_y = n1->pos().y() - n2->pos().y();
                float dist = len(delta_x, delta_y);
//                dist = dist == 0 ? 0.00001 : dist;
                rep = repulsion(dist, k);
                newDisp[n1][0] += (delta_x/dist)*rep;
                newDisp[n1][1] += (delta_y/dist)*rep;
            }
            
            // calculate attration
            float aX=0; float aY=0;
            float att = 0.0;
            foreach (Edge* e, n1->edges() ) { 
                Node* n2 = e->destNode(); if(n2 == n1) n2 = e->sourceNode();
                if (newDisp.count(n2) == 0) newDisp[n2] = disp; 
                float delta_x = n1->pos().x() - n2->pos().x();
                float delta_y = n1->pos().y() - n2->pos().y();
                float dist = len(delta_x, delta_y);
//                dist = dist == 0 ? 0.00001 : dist;
                att = attraction(dist, k);

                if (n1->isFixedPosition() == false) {
                    newDisp[n1][0] -= (delta_x/dist)*att;
                    newDisp[n1][1] -= (delta_y/dist)*att;
                }

                if( n2->isFixedPosition() == false) {
                    newDisp[n2][0] += (delta_x/dist)*att;
                    newDisp[n2][1] += (delta_y/dist)*att;
                }
            }

        }

        foreach(Node* n1, nodelist ) {
            if (n1 && n1->isFixedPosition()) continue;
            float disp_x = newDisp[n1][0];
            float disp_y = newDisp[n1][1];

            float disp_magnitude = len(disp_x, disp_y);
//            disp_magnitude = disp_magnitude == 0 ? 0.00001 : disp_magnitude;
            float newX = n1->pos().x() + (disp_x/disp_magnitude)*min(disp_magnitude,t);
            newX = min((float) W/2, max((float) -W/2, newX));

            float newY = n1->pos().y() + (disp_y/disp_magnitude)*min(disp_magnitude,t);
            newY = min((float) H/2, max((float) -H/2, newY));

            n1->setPos( newX, newY);

			if (newX < -W/2 || newX > W/2) W*=1.2;
			if (newY < -H/2 || newY > H/2) H*=1.2;
            t = cool(t, t_init, rep_max);
        }
    }

    //int sceneW = W; int sceneH = H;
    //scene()->setSceneRect(-sceneW/2,-sceneH/2,sceneW,sceneH);
}
 


