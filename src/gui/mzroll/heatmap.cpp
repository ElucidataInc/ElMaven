#include <QGraphicsScene>

#include "Compound.h"
#include "globals.h"
#include "heatmap.h"
#include "mainwindow.h"
#include "mzSample.h"
#include "Scan.h"
#include "statistics.h"
#include "tabledockwidget.h"

HeatMap::HeatMap(MainWindow* mw) { 
    this->mainwindow = mw;
    _table=NULL;
    _colorramp=1;
    _sampleSpacer=100;
    _rowSpacer=150;
    _boxW = 30;
    _boxH = 30;

    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    setObjectName("Heatmap");
}


HeatMap::~HeatMap() {
  if (scene()!=NULL) delete(scene());
}

void HeatMap::setTable(TableDockWidget* t) { 
_table = t;
}

void HeatMap::replot() {
    drawMap();
}

void HeatMap::wheelEvent(QWheelEvent *event) {
    float scaleFactor= pow((double)2, -event->delta() / 240.0);
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100) return;
    scale(scaleFactor, scaleFactor);
    cerr << "HeatMap::wheelEvent() " << scaleFactor << endl;
}

void HeatMap::updateColors() {

    cerr << "HeatMap::updateColors()" << endl;
    Q_FOREACH (QGraphicsItem *item, scene()->items()) {
        if (QGraphicsRectItem *rect = qgraphicsitem_cast<QGraphicsRectItem *>(item)) {
           float cellValue = rect->data(1).toDouble();
           QColor color = getColor(cellValue,_heatMin,_heatMax);
           if(cellValue) ((QGraphicsRectItem*) rect)->setBrush(color);
        }
    }

    cerr << "HeatMap::updateColors() .. done" << endl;
}


void HeatMap::drawMap() { 

        scene()->clear();
	if (_table == NULL) return;


	QList<PeakGroup*>allgroups = _table->getGroups();
        int Nrows= allgroups.size();
	vector<mzSample*> vsamples = mainwindow->getVisibleSamples();
   	sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
	int Ncols= vsamples.size();
	if ( Nrows == 0 || Ncols == 0) return;

	heatmap.resize(Nrows,Ncols);
        heatmap.setZero();
        _heatMax=0;
        _heatMin=0;

        sort(allgroups.begin(), allgroups.end(), PeakGroup::compPvalue);

        for (int i=0; i < Nrows; i++ ) {
            PeakGroup* group = allgroups[i];
            StatisticsVector<float> yvalues = group->getOrderedIntensityVector(vsamples,PeakGroup::AreaTop);

            float center = median(yvalues);
            if ( center == 0 ) center = yvalues.mean();
            if ( center == 0 ) center = yvalues[0];
            if ( center == 0 ) center = 1;
            if ( center )
               for(int j=0; j< yvalues.size(); j++ ) {
                float ratio = yvalues[j]/center;
                if (ratio !=0 ) ratio = log2(ratio);   //fold change on log2 scale
                heatmap(i,j)=ratio;
                if (ratio > _heatMax)  _heatMax=ratio;
                if (ratio < _heatMin)  _heatMin=ratio;
            }
	}

	//heatmap.print();
        float range  =  _heatMax-_heatMin;
        cerr << _heatMin << " " << _heatMax << " " << range << endl;

	int sceneWidth=Ncols*_boxW+_rowSpacer;
	int sceneHeight=Nrows*_boxH+_sampleSpacer;

	//draw heatmap
        scene()->setSceneRect(0,0,sceneWidth,sceneHeight);
        for (int i=0; i < heatmap.rows(); i++ ) {
            PeakGroup* group = allgroups[i];
            for (int j=0; j < heatmap.cols(); j++ ) {
                float cellValue = heatmap(i,j);
                QColor color = getColor(cellValue,_heatMin,_heatMax);
                int xpos = _rowSpacer+j*_boxW;
                int ypos = _sampleSpacer+i*_boxH;

                QBrush brush(color);
                QPen pen(Qt::black);
                QGraphicsRectItem* item = scene()->addRect(QRectF(xpos,ypos,_boxW,_boxH),pen,brush);
                item->setData(0, QVariant::fromValue(group));
                item->setData(1, QVariant::fromValue(cellValue));
                item->setFlag(QGraphicsItem::ItemIsSelectable);
            }

            Compound* c  = group->getCompound();
            if ( c != NULL) {
                QGraphicsTextItem* item = scene()->addText(QString(c->name().c_str()));
                int textWidth = item->boundingRect().width();
                float ratio = _rowSpacer/(float) textWidth;
                item->setPos(0,_sampleSpacer+i*_boxH);
                if ( ratio < 1 ) item->setScale(ratio);
                item->setData(0, QVariant::fromValue(group));

            }
        }

	//draw labels
	 for(int i=0; i < vsamples.size(); i++ ) {
		 QGraphicsTextItem* item = scene()->addText(QString(vsamples[i]->sampleName.c_str()));
		 int textWidth = item->boundingRect().width();
		 float ratio = _sampleSpacer/(float) textWidth;
	  	 if ( ratio < 1 ) item->setScale(ratio);
		 int textHeight = item->boundingRect().height();
	 	 item->setPos(_rowSpacer+i*_boxW+textHeight,0);
		 item->setRotation(90);
	 }

	//draw lagend
        int steps=16; int boxSize = (scene()->width()-_rowSpacer)/steps;
	for(int i=0; i < steps; i++ ) {
                 int ypos = i*boxSize;
                 int xpos = -boxSize;
                 float cellValue=_heatMin+(i/(float)steps*range);
                 QPen pen(Qt::black);
                 QBrush brush(getColor(cellValue,_heatMin,_heatMax));
                 QGraphicsRectItem* item = scene()->addRect(QRectF(-boxSize/2,-boxSize/2,boxSize,boxSize),pen,brush);
                 QGraphicsTextItem* text = scene()->addText(QString::number(cellValue,'f',0));
                 text->setPos(xpos,ypos);
                 item->setPos(xpos,ypos);
                 item->setData(1, QVariant::fromValue(cellValue));
	}

     scene()->update();
}

QColor HeatMap::getColor(float cellValue, float minValue, float maxValue) { 
	QColor color = Qt::black;
        if (_colorramp < 0.01) _colorramp=0.01;
        if (_colorramp > 20) _colorramp=20;

	if (cellValue < 0)  { 
                float intensity=pow(abs(cellValue/minValue),_colorramp);
                if (intensity > 1 ) intensity=1;
                color.setHsvF(0.6,intensity,intensity);
	}

	if (cellValue > 0 )  { 
                float intensity=pow(abs(cellValue/maxValue),_colorramp);
                if (intensity > 1 ) intensity=1;
                color.setHsvF(0.1,intensity,intensity);
	}
	return color;
}


void HeatMap::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem* item = itemAt(event->pos());
        cerr << "Item=" << item << endl;
        if ( item != NULL )  {
			QVariant v = item->data(0);
   			PeakGroup*  group =  v.value<PeakGroup*>();
            if (group != NULL && mainwindow != NULL) {
                mainwindow->setPeakGroup(group);
            }
        }
	}
}

void HeatMap::resizeEvent ( QResizeEvent * event ) {
      QSize newsize = event->size();
	  update();
}

void HeatMap::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Right:
        _colorramp *= 1.2;
        updateColors();
        break;
    case Qt::Key_Left:
        _colorramp /= 1.2;
        updateColors();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
    scene()->update();
}


