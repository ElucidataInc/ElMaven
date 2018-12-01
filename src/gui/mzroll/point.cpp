#include "point.h"
EicPoint::EicPoint(float x, float y, Peak* peak, MainWindow* mw)
{

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    _x = x;
    _y = y;
    _mw = mw;
    _peak = peak;
    _group = NULL;
    _scan = NULL;
    _cSize = 10;
    _color=QColor(Qt::black);
    _pen=QPen(_color);
    _brush=QBrush(_color);

    setPointShape(CIRCLE);
    forceFillColor(false);

    if (_peak) {
        _cSize += 20*(_peak->quality);
        //mouse press events
         connect(this, SIGNAL(peakSelected(Peak*)), mw, SLOT(showPeakInfo(Peak*)));
         connect(this, SIGNAL(peakSelected(Peak*)), mw->getEicWidget(), SLOT(showPeakArea(Peak*)));

         //mouse hover events
         connect(this, SIGNAL(peakGroupFocus(PeakGroup*)), mw->getEicWidget(), SLOT(setSelectedGroup(PeakGroup*)));
         connect(this, SIGNAL(peakGroupFocus(PeakGroup*)), mw->getEicWidget()->scene(), SLOT(update()));
    }


}

EicPoint::~EicPoint() {}

QRectF EicPoint::boundingRect() const
{
    return(QRectF(_x-_cSize/2,_y-_cSize/2,_cSize,_cSize));
}

void EicPoint::hoverEnterEvent (QGraphicsSceneHoverEvent*) {
	this->setFocus(Qt::MouseFocusReason);

	//update colors of all peaks belonging to this group
	if(_group) { 
		Q_FOREACH (QGraphicsItem *item, scene()->items()) {
			if (qgraphicsitem_cast<EicPoint *>(item)) {
				if (((EicPoint*) item)->getPeakGroup() == _group) item->update();
			}
		}
	}

	string sampleName;
    if (_peak && _peak->getSample() ) {
        sampleName = _peak->getSample()->sampleName;

        setToolTip( "<b>  Sample: </b>"   + QString( sampleName.c_str() ) +
                          "<br> <b>intensity: </b>" +   QString::number(_peak->peakIntensity) +
                            "<br> <b>area: </b>" + 		  QString::number(_peak->peakAreaCorrected) +
                            "<br> <b>Spline Area: </b>" + 		  QString::number(_peak->peakSplineArea) +
                            "<br> <b>rt: </b>" +   QString::number(_peak->rt, 'f', 2 ) +
                            "<br> <b>scan#: </b>" +   QString::number(_peak->scan ) + 
                            "<br> <b>sample number: </b>" +   QString::number(_peak->getSample()->sampleNumber) + 
                            "<br> <b>m/z: </b>" + QString::number(_peak->peakMz, 'f', 6 )
                        );

		update();
		/*
		   "<br> <b>quality:  </b>"  + QString::number(_peak->quality, 'f', 2) +
		   "<br> <b>sigma:  </b>"  + QString::number(_peak->gaussFitSigma, 'f', 2) +
		   "<br> <b>fitR2:  </b>"  + QString::number(_peak->gaussFitR2*100, 'f', 2)
		   "<br> <b>Group Overlap Frac: </b>" + QString::number(_peak->groupOverlapFrac)
		   "<br> <b>peakAreaFractional: </b>" + QString::number(_peak->peakAreaFractional) +
		   "<br> <b>noNoiseFraction: </b>" + QString::number(_peak->noNoiseFraction) +
		   "<br> <b>symmetry: </b>" + QString::number(_peak->symmetry) +
		   "<br> <b>sigma: </b>" + QString::number(_peak->gaussFitSigma) +
		   "<br> <b>r2: </b>" + QString::number(_peak->gaussFitR2) +
		   "<br> <b>angle: </b>" + QString::number(_peak->angle) +
		   "<br> <b>rank: </b>" + QString::number(_peak->peakRank) +
		   "<br> <b>S/N: </b>" + QString::number(_peak->signalBaselineRatio, 'f', 4) +
		   "<br> <b>Width: </b>" + QString::number(_peak->width) +
		   "<br> <b>No NoiseObs: </b>" + QString::number(_peak->noNoiseObs) +
		   "<br> <b>Group Overlap Frac: </b>" + QString::number(_peak->groupOverlapFrac) +
		 */
	} 
	if (_scan) { 
		setToolTip( "<b>  Sample: </b>"   + QString( _scan->sample->sampleName.c_str() ) +
					"<br> <b>FilterLine: </b>" + 		  QString(_scan->filterLine.c_str() ) + 
					"<br> <b>Scan#: </b>" +   QString::number(_scan->scannum) +
                    "<br> <b>sample number: </b>" +   QString::number(_scan->sample->sampleNumber) + 
					"<br> <b>PrecursorMz: </b>" +   QString::number(_scan->precursorMz, 'f', 2 )
		);
	}



    if(_group) {
        _group->isFocused = true;
        Q_EMIT(peakGroupFocus(_group));
    }
}

void EicPoint::hoverLeaveEvent ( QGraphicsSceneHoverEvent*) {
    clearFocus();

    if (_group) {
        _group->isFocused = false;

        Q_FOREACH (QGraphicsItem *item, scene()->items()) {
            if (qgraphicsitem_cast<EicPoint *>(item)) {
                if (((EicPoint*) item)->getPeakGroup() == _group) item->update();
            }
        }
    }
    update(); 
}

void EicPoint::mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) {

    if (_group) Q_EMIT peakGroupSelected(_group);
    if (_peak)  Q_EMIT peakSelected(_peak);

    if ( _group && _group->isIsotope() == false ) {
        _mw->isotopeWidget->setPeakGroupAndMore(_group, true);
        _mw->isotopeWidget->peakSelected(_peak, _group);
    }

}
void EicPoint::mousePressEvent (QGraphicsSceneMouseEvent* event) {
    //if (_group) _group->groupOveralMatrix();

    if (event->button() == Qt::RightButton)  {
        contextMenuEvent(event);
        return;
    }

    setZValue(10);

    if (_group) Q_EMIT peakGroupSelected(_group);
    if (_peak)  Q_EMIT peakSelected(_peak);

    _mw->groupRtWidget->plotGraph(_group);

    if ( _group && _group->isIsotope() == false ) {
        _mw->isotopeWidget->updateIsotopicBarplot(_group);
    }

    if(_scan) {
        if (_mw->spectraWidget->isVisible())
            _mw->spectraWidget->setScan(_scan);
            _mw->peptideFragmentation->setScan(_scan);
            if(_scan->mslevel >= 2)  _mw->spectralHitsDockWidget->limitPrecursorMz(_scan->precursorMz);

    }

    //if (_peak && _mw->spectraWidget->isVisible()) {
    //    _mw->spectraWidget->setScan(_peak);
    //}

    if (_peak && _mw->covariantsPanel->isVisible()) {
        _mw->getLinks(_peak);
    }

    if (_peak && _mw->adductWidget->isVisible()) {
        _mw->adductWidget->setPeak(_peak);
    }

    if (_peak && _group && _mw->isotopeWidget->isVisible()) {
        _mw->isotopeWidget->peakSelected(_peak, _group);
    }

    scene()->update();
}



void EicPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPen pen = _pen;
    QBrush brush = _brush;

    float scale = min(scene()->height(),scene()->width())/500;
    float paintDiameter = _cSize*scale;  
    if (paintDiameter<5) paintDiameter=5; if (paintDiameter>30) paintDiameter=30;

    //int maxRadius = scene()->height()*0.05;
    //if (maxRadius > 30) maxRadius = 30;
    //if (maxRadius < 5) maxRadius=5;

    PeakGroup* selGroup = _mw->getEicWidget()->getParameters()->getSelectedGroup();

    if (_group != NULL && selGroup == _group ) {
        brush.setStyle(Qt::SolidPattern);
        pen.setColor(_color.darker());
        pen.setWidth(_pen.width()+1);
    } else {
        brush.setStyle(Qt::NoBrush);
    }

    if (_forceFill) {
        brush.setStyle(Qt::SolidPattern);
    }

    painter->setPen(pen);
    painter->setBrush(brush);
    //float cSize = 6 + maxRadius*(_peak->quality);
    //painter->drawEllipse(_x-cSize/2, _y-cSize/2, cSize,cSize);

    if(pointShape == CIRCLE ) {
        painter->drawEllipse(_x-paintDiameter/2, _y-paintDiameter/2, paintDiameter,paintDiameter);
    } else if (pointShape == SQUARE) {
        painter->drawRect(_x-paintDiameter/2, _y-paintDiameter/2, paintDiameter,paintDiameter);
    } else if (pointShape == TRIANGLE_DOWN ) {
        painter->drawPie(_x-paintDiameter/2,_y-paintDiameter/2,paintDiameter,paintDiameter,30*16,120*16);
    } else if (pointShape == TRIANGLE_UP ) {
        painter->drawPie(_x-paintDiameter/2,_y-paintDiameter/2,paintDiameter,paintDiameter,260*16,20*16);
    }
}

void EicPoint::setClipboardToGroup() { if(_group) _mw->setClipboardToGroup(_group); }

void EicPoint::bookmark() { if(_group) _mw->bookmarkPeakGroup(_group); }

void EicPoint::setClipboardToIsotopes() {
    if (_group &&_group->compound != NULL && ! _group->compound->formula.empty() )  {
        _mw->isotopeWidget->updateIsotopicBarplot(_group);
        _mw->isotopeWidget->setPeakGroupAndMore(_group, true);
        if (_peak)
            _mw->isotopeWidget->peakSelected(_peak, _group);
    }
}

void EicPoint::linkCompound() {
    if (_group &&_group->compound != NULL )  {
            //link group to compound
            _group->compound->setPeakGroup(*_group);

            //update compound retention time
            if (_peak) _group->compound->expectedRt=_peak->rt;

            //log information about retention time change
           // _mw->getEicWidget()->addNote(_peak->peakMz,_peak->peakIntensity, "Compound Link");
            _mw->getEicWidget()->saveRetentionTime();

            //upadte ligand widget
            QString dbname(_group->compound->db.c_str());
            _mw->ligandWidget->setDatabaseAltered(dbname,true);
            //_mw->ligandWidget->updateTable();
            _mw->ligandWidget->updateCurrentItemData();

            //update pathway widget with new concentration information
            _mw->pathwayWidget->updateCompoundConcentrations();
	}
}

void EicPoint::reorderSamples() { if (_mw && _group ) _mw->reorderSamples(_group ); }

void EicPoint::contextMenuEvent ( QGraphicsSceneMouseEvent* event ) {
    QMenu menu;

    QAction* c1 = menu.addAction("Copy Details to Clipboard");
    c1->setIcon(QIcon(rsrcPath + "/copyCSV.png"));
    connect(c1, SIGNAL(triggered()), SLOT(setClipboardToGroup()));

    if (_group && _group->compound ) {
       if ( _group->isIsotope() == false && !_group->compound->formula.empty() ) {
            QAction* z = menu.addAction("Copy Isotope Information to Clipboard");
            z->setIcon(QIcon(rsrcPath + "/copyCSV.png"));
            connect(z, SIGNAL(triggered()), SLOT(setClipboardToIsotopes()));
        }

        QAction* e = menu.addAction("Link to Compound");
        e->setIcon(QIcon(rsrcPath + "/link.png"));
        connect(e, SIGNAL(triggered()), SLOT(linkCompound()));
    }

    QAction* c2 = menu.addAction("Mark Good");
    c2->setIcon(QIcon(rsrcPath + "/markgood.png"));
    connect(c2, SIGNAL(triggered()), _mw->getEicWidget(), SLOT(markGroupGood()));

    QAction* c3 = menu.addAction("Mark Bad");
    c3->setIcon(QIcon(rsrcPath + "/markbad.png"));
    connect(c3, SIGNAL(triggered()), _mw->getEicWidget(), SLOT(markGroupBad()));

    if ( _group && _group->peaks.size() > 1  ) {
        QAction* d = menu.addAction("Sort Samples by Peak Intensity");
        connect(d, SIGNAL(triggered()), SLOT(reorderSamples()));
    }

    QAction *selectedAction = menu.exec(event->screenPos());


    //event->ignore();
}

void EicPoint::keyPressEvent( QKeyEvent *e ) {
	bool marked=false;
	if (!_group) return;

	//qDebug() << "Point::keyPressEvent() " << e->key() << endl;

	switch( e->key() ) {
	}
	update();
	e->accept();
}
