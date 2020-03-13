#include "Compound.h"
#include "datastructures/mzSlice.h"
#include "globals.h"
#include "mzSample.h"
#include "note.h"
#include "plotdock.h"
#include "Scan.h"

PlotScene::PlotScene(QObject * parent): QGraphicsScene(parent) {
	selectionRect = new QGraphicsRectItem(); selectionRect->hide(); selectionRect->setPen(QPen(Qt::DotLine));
	vline = new QGraphicsLineItem(); vline->hide(); vline->setPen(QPen(Qt::DotLine));
	hline = new QGraphicsLineItem(); hline->hide();	hline->setPen(QPen(Qt::DotLine));
	ylabel = new QGraphicsTextItem(); ylabel->hide(); ylabel->setFont(QFont("Helvetica",10)); ylabel->setRotation(-90);
	xlabel = new QGraphicsTextItem(); xlabel->hide(); xlabel->setFont(QFont("Helvetica",10));
	xValueLabel   = new Note("", hline, this);  xValueLabel->setExpanded(true); xValueLabel->hide();
	yValueLabel   = new Note("", vline, this);  yValueLabel->setExpanded(true); yValueLabel->hide();
    // New Variables Initialisation - Kiran
	logBase=10;
    logX=false;
    logY=false;
	
	_mousePressed=false;
	_mouseReleased=true;
	
};

PlotScene::~PlotScene() {
	clear();
}

void PlotScene::clear() { 
	if(vline && vline->scene() == this) removeItem(vline);
	if(hline && hline->scene() == this) removeItem(hline);
	if(ylabel && ylabel->scene() == this) removeItem(ylabel);
	if(xlabel && xlabel->scene() == this) removeItem(xlabel);
	if(xValueLabel && xValueLabel->scene() == this) removeItem(xValueLabel);
	if(yValueLabel && yValueLabel->scene() == this) removeItem(yValueLabel);
	if(selectionRect && selectionRect->scene() == this) removeItem(selectionRect);
	QGraphicsScene::clear();
}

void PlotScene::showXLabel(QString text) {
	if(xlabel) { 
		addItem(xlabel); 
		xlabel->setHtml(text); 
		xlabel->setVisible(true);  
		xlabel->setPos( plotRect.width()/2 - xlabel->boundingRect().width()/2, height()-10 );
	}
}

void PlotScene::showYLabel(QString text) {
	if(ylabel) { 
		addItem(ylabel); 
		ylabel->setHtml(text); 
		ylabel->setVisible(true);  
		ylabel->setPos(-20, plotRect.height()/2 );
	}
}


void PlotScene::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent ) {

	QPointF p = mouseEvent->scenePos();
	if(vline) vline->setLine(p.x(), p.y(), p.x(), plotRect.height()*1.05 );
	if(hline) hline->setLine(0, p.y(),p.x(),p.y() );
	QGraphicsScene::mouseMoveEvent(mouseEvent);

	if (_mousePressed && !_mouseReleased) {  //drag
		if(selectionRect->scene() != this) addItem(selectionRect);
		QRectF x;
		int w = p.x()-down.x();
		int h = p.y()-down.y();
		if (w > 0 && h > 0 ) {
			x.setTopLeft(down); x.setBottomRight(p);
		} else if (w > 0 && h < 0 ) {
			x.setTopRight(p); x.setBottomLeft(down);
		} else if (w < 0 && h > 0 ) {
			x.setTopRight(down); x.setBottomLeft(p);
		} else {
			x.setTopLeft(p); x.setBottomRight(down);
		}

		selectionRect->setRect(x);
		selectionRect->show();
	}

	/*
	QPointF P = mapToPlot(p.x(), p.y());

	if(vline && vline->isVisible() ) { 
		xValueLabel->setPos(p.x(),height()); 
		xValueLabel->setPlainText(QString::number(P.x())); 
		xValueLabel->setVisible(true);
	}
	if(hline && hline->isVisible() ) { 
		yValueLabel->setPos(0,p.y());  
		yValueLabel->setPlainText(QString::number(P.y())); 
		yValueLabel->setVisible(true);
	}
	*/
}

void PlotScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent ) {
	down =  mouseEvent->buttonDownScenePos(Qt::LeftButton);
	up    = down;

    _mousePressed=true;
	_mouseReleased=false;
	selectionRect->hide();
  /*
	QPointF downF = down;

	QList<QGraphicsItem*>foundItems = items(downF);
	if(foundItems.size()>0){
        Q_FOREACH(QGraphicsItem* item, foundItems )  item->setSelected(true);
	}
	qDebug() << "PlotScene::mousePressEvent: " << selectedItems();
    */

	QGraphicsScene::mousePressEvent(mouseEvent);
	return;
}


void PlotScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent ) {
		up =  mouseEvent->scenePos();
		int width =  up.x()-down.x();
		int height = up.y()-down.y();

		_mousePressed=false;
		_mouseReleased=true;


		if ( mouseEvent->modifiers() == Qt::ShiftModifier ) {
			QRectF rect(down,up);
			QPainterPath path; path.addRect(rect);
			setSelectionArea(path);
			qDebug() << "selectArea " << rect << " " << selectedItems().size();
			Q_EMIT(selectArea(down,up));
		}   else if ( abs(width) > 10 & abs(height) > 0 )  {
		    cerr <<  "mouseReleaseEvent() zoomArea : " << endl;
			Q_EMIT(zoomArea(down,up));
        }
	
		QGraphicsScene::mouseReleaseEvent(mouseEvent);
		return;
};


QPointF PlotScene::mapToPlot(float px,float py) {
	int W = plotRect.width();
	int H = plotRect.height();

    QPointF origin = plotRect.bottomLeft();
	float xorigin = origin.x();
	float yorigin = origin.y();

    if ( xDim.x() == 0 && xDim.y() == 0 ) return origin;
    if ( yDim.x() == 0 && yDim.y() == 0 ) return origin;

    float X0 = zoomXDim.x();
    float X1 = zoomXDim.y();
    if (logX) {
		//TODO: Equations updated, don't know why - Kiran
        X0>0 ? X0=log(X0)/log(logBase): X0=1.001;
        X1>0 ? X1=log(X1)/log(logBase): X1=1.001;
    }

    float Y0 = zoomYDim.x();
    float Y1 = zoomYDim.y();
    if (logY) { 
		//TODO: Equations updated, don't know why - Kiran
		Y0>0 ? Y0=log(Y0)/log(logBase): Y0=1.0001;
        Y1>0 ? Y1=log(Y1)/log(logBase): Y1=1.0001;
    }

	float fx = (px-xorigin)/W;
	float fy = (py-yorigin)/H*-1;


    float x =  fx*(X1-X0)+X0;
    float y =  fy*(Y1-Y0)+Y0;
	//TODO: Equations updated, don't know why - Kiran
    if (logX) x = pow((double) logBase,(double) x); //10^x
    if (logY) y = pow((double) logBase,(double) y); //10^y

    return QPointF(x,y);
}

QPointF PlotScene::plotToMap(float x,float y) {


	int W = plotRect.width();
	int H = plotRect.height();

    QPointF origin = plotRect.bottomLeft();
	float xorigin = origin.x();
	float yorigin = origin.y();

    if ( xDim.x() == 0 && xDim.y() == 0 ) return origin;
    if ( yDim.x() == 0 && yDim.y() == 0 ) return origin;

    float X0 = zoomXDim.x();
    float X1 = zoomXDim.y();

    if (logX) {
	//TODO: Equations updated, don't know why - Kiran
     x > 0 ?  x=log(x)/log(logBase): x=0;  X0>0 ? X0=log(X0)/log(logBase): X0=1.0001; X1>0 ? X1=log(X1)/log(logBase) : X1=1.0001; }

    float Y0 = zoomYDim.x();
    float Y1 = zoomYDim.y();
    //TODO: Equations updated, don't know why - Kiran
    if (logY) { y > 0 ?  y=log(y)/log(logBase): y=0;  Y0>0 ? Y0=log(Y0)/log(logBase): Y0=1.00001; Y1>0 ? Y1=log(Y1)/log(logBase) : Y1=1.0001; }

	float fx= X1-X0 != 0 ? (x-X0)/(X1-X0) : 0;
	float fy= Y1-Y0 != 0 ? (y-Y0)/(Y1-Y0) : 0;
	float px = xorigin+fx*W;
	float py = yorigin-fy*H;

	return QPointF(px,py);
}


PlotDockWidget::PlotDockWidget(QWidget * parent,Qt::WindowFlags flags):QDockWidget(parent,flags) {

	mainWidget = new QWidget(this);

	myView =  new QGraphicsView(this);
	myScene = new PlotScene(view());
    	myScene->setXDim(0,0);
   	 myScene->setYDim(0,0);
    	myScene->setZoomXDim(0,0);
    	myScene->setZoomYDim(0,0);

	view()->setScene(myScene);
	view()->setMouseTracking(true);
	view()->setDragMode(QGraphicsView::NoDrag);
	view()->setRubberBandSelectionMode(Qt::ContainsItemShape);
	view()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
	connect(myScene,SIGNAL(selectionChanged()),this,SLOT(selectionChanged()));
	connect(myScene,SIGNAL(zoomArea(QPointF, QPointF)),this,SLOT(zoomArea(QPointF,QPointF)));

	addToolBar();
	toolBar->hide();

	//layout dockwidget content
	QVBoxLayout *vlayout= new QVBoxLayout(mainWidget);
	vlayout->addWidget(toolBar);
	vlayout->addWidget(view());
	mainWidget->setLayout(vlayout);

	//dockwidget control
	setAllowedAreas(Qt::AllDockWidgetAreas);
  	setFloating(false);
   	setVisible(false);
	//setWidget(mainWidget);
	setWidget(view());

	//context menu
	contextMenu = new QMenu(this);
	QAction* pdf = contextMenu->addAction("Save To PDF File");
    connect(pdf, SIGNAL(triggered()), SLOT(exportPDF()));


        setWindowTitle("PlotDockWidget");
        setObjectName("PlotDockWidget");
}



PlotDockWidget::~PlotDockWidget() {
  if(scene()) scene()->clear();
  if(scene()) delete(scene());
  if(view())  delete(view());
  if(toolBar) delete(toolBar);
  delete(mainWidget);

}
void PlotDockWidget::resetZoom() {
  myScene->resetZoom(); 
  zoomHistory.clear();
  replot(); 
}

void PlotDockWidget::replot(){
    scene()->clear();
    int viewW = view()->width();
    int viewH = view()->height();
    scene()->setSceneRect(0,0,viewW*0.95,viewH*0.95);
    int marginX=  viewW*0.95*0.02;
    int marginY = viewH*0.95*0.02;

    QRectF plotRect = scene()->sceneRect();
    plotRect.adjust(+marginY,+marginY,-marginX,-marginY);
    scene()->setPlotRect(plotRect);

		draw();
}

void PlotDockWidget::zoomArea(QPointF from, QPointF to) {
    int zoomDirection = +1;
    if (from.x() > to.x() ) { swap(from,to);  zoomDirection=-1; }

    if ( zoomDirection < 0 ) { //zoom out
         float X0= scene()->getXDim().x();
         float X1= scene()->getXDim().y(); 
         float Y0= scene()->getYDim().x();
         float Y1= scene()->getYDim().y();

         if (zoomHistory.size() > 0 ) {
            QRectF lastView = zoomHistory.pop(); 
            X0 = lastView.x(); X1 = lastView.y();
            Y0 = lastView.width(); Y1 = lastView.height();
         }
        scene()->setZoomXDim(X0,X1);
        scene()->setZoomYDim(Y0,Y1);
        replot();
    } else {
        QPointF plotPoint1 = scene()->mapToPlot(from.x(), from.y());
        QPointF plotPoint2 = scene()->mapToPlot(to.x(), to.y());
        float X0 = plotPoint1.x();
        float X1 = plotPoint2.x();
        if (X0 > X1) swap(X0,X1);

        float Y0 = plotPoint1.y();
        float Y1 = plotPoint2.y();
        if (Y0 > Y1) swap(Y0,Y1);

        scene()->setZoomXDim(X0,X1);
        scene()->setZoomYDim(Y0,Y1);

        zoomHistory.push_back(QRectF(X0,X1,Y0,Y1));
        replot();
    }
       qDebug() << "PlotDockWidget: zoomArea" 
           << scene()->getZoomXDim() << " " << scene()->getZoomYDim();
}

void PlotDockWidget::drawAxes() { 

	 //QPen pen3(Qt::red);
	 //scene()->addRect(scene()->getPlotRect(),pen3,Qt::NoBrush);

     PlotAxes* xAxis= new PlotAxes(0,10,scene());
	 xAxis->setZValue(0);
	 xAxis->setOffset(0);

     PlotAxes* yAxis= new PlotAxes(1,10,scene());
	 yAxis->setZValue(0);
	 yAxis->setOffset(0);

	 scene()->addItem(xAxis);
	 scene()->addItem(yAxis);
}


void PlotDockWidget::draw() { 
  	drawAxes();
}

void PlotDockWidget::draw(QPointF a, QPointF b) { 
  	drawAxes();
}

void PlotDockWidget::selectionChanged() { 
	cerr << "PlotDockWidget::selectionChanged() " << endl;
	Q_FOREACH(QGraphicsItem *item, scene()->selectedItems()) {
		if (item == NULL) continue;
			QVariant v = item->data(0);
			PeakGroup*  group =  v.value<PeakGroup*>();
			if (group != NULL ) {
				Q_EMIT groupSelected(group);
				break;
			}

	}
	//scene()->clearSelection();
}

/*
void PlotDockWidget::mousePressEvent(QMouseEvent *event) {
	QDockWidget::mousePressEvent(event);
	cerr << "mousePressEvent() " << endl;
	view()->setMouseTracking(true);
	view()->setFocus();
}
*/

void PlotDockWidget::resizeEvent ( QResizeEvent * event ) {
      QSize newsize = event->size();
	  replot();
}

void PlotDockWidget::keyPressEvent(QKeyEvent *event)
{
	scene()->update();
}


void PlotDockWidget::addToolBar() { 

    toolBar = new QToolBar(mainWidget);
	toolBar->setFloatable(true);
	toolBar->setMovable(true);
    toolBar->setIconSize(QSize(24, 24));
}



QRectF PlotAxes::boundingRect() const
{   

    int textmargin=50;
	if (!scene())return QRectF(0,0,0,0);

	if(type == 0 ) {
    	return(QRectF(0,scene()->height()-textmargin,scene()->width(),scene()->height()-textmargin));
	} else {
    	return(QRectF(0,0,+textmargin,scene()->height()+textmargin));
	}
}

void PlotAxes::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{ 
	QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	painter->setPen(pen);

    int fontsize = 8;
    QFont font("Helvetica",fontsize);
	painter->setFont(font);

    if (nticks == 0 ) nticks = 2;
    PlotScene* myscene = (PlotScene*) scene();

    QRectF plotRect = myscene->getPlotRect();
    QPointF origin = plotRect.bottomLeft();
    float x0 = origin.x();
    float x1 = plotRect.bottomRight().x();

    int y0 = origin.y();
    int y1 = plotRect.topLeft().y();

    float min = 0;
    float max = 0;

    if (type == 0 ) { min = myscene->getZoomXDim().x(); max = myscene->getZoomXDim().y(); }
    if (type == 1 ) { min = myscene->getZoomYDim().x(); max = myscene->getZoomYDim().y(); }


    float ticks = nticks;

    if ( type == 0) { 	//X axes
        painter->drawLine(x0,y0,x1,y0);
        float ix = (x1-x0)/ticks;
        for (int i=0; i <= ticks; i++ ) painter->drawLine(x0+ix*i,y0-5,x0+ix*i,y0+5);

        for (int i=0; i <= ticks; i++ ) {
             float tickX = myscene->mapToPlot(x0+ix*i,y0+10).x();
			 // made precsion 1 - Kiran
             QString label= QString::number(tickX,'f',1);
             painter->drawText(x0+ix*i,y0+10,label);
        }

    } else if ( type == 1 ) { //Y axes
        painter->drawLine(x0,y0,x0,y1);
        float iy = (y1-y0)/ticks;
        for (int i=0; i <= ticks; i++ ) painter->drawLine(x0-5,y0+iy*i,x0+5,y0+iy*i);

        for (int i=0; i <= ticks; i++ ) { 
             float tickY = myscene->mapToPlot(x0+2,y0+iy*i).y();
			 //made precision 1 - Kiran
             QString label= QString::number(tickY,'f',1);
             painter->drawText(x0+2,y0+iy*i,label);
		}

		if(tickLinesFlag) {
			//horizontal tick lines
			QPen pen(Qt::gray, 0.1,Qt::DotLine);
			painter->setPen(pen);
			for (int i=0; i <= ticks; i++ ) painter->drawLine(x0-5,y0+iy*i,x1,y0+iy*i);
		}
    }
}

void PlotDockWidget::exportPDF(){
    const QString fileName = QFileDialog::getSaveFileName(
        this, "Export File Name", QString(),
        "PDF Documents (*.pdf)");

    if (fileName.isEmpty()) return;

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
	printer.setFontEmbeddingEnabled(false);
	printer.setPageSize(QPrinter::Letter);
    printer.setOrientation(QPrinter::Landscape);
	printer.setResolution(600);
    QPainter painter(&printer);
    painter.setRenderHint(QPainter::Antialiasing);
	scene()->setFont( QFont("Helvetica",8) );
	painter.setFont( QFont("Helvetica", 8) );
    view()->render(&painter);
}


void PlotDockWidget::contextMenuEvent(QContextMenuEvent * event) {
    QDockWidget::contextMenuEvent(event);
	//show menu
    QAction *selectedAction = contextMenu->exec(event->globalPos());
}

