#include "spectrawidget.h"

SpectraWidget::SpectraWidget(MainWindow* mw) { 
    this->mainwindow = mw;
    eicparameters = new EICLogic();
   _currentScan = NULL;
   _avgScan = NULL;

    initPlot();

    _drawXAxis = true;
    _drawYAxis = true;
    _resetZoomFlag = true;
    _profileMode = false;
    _nearestCoord = QPointF(0,0);
    _focusCoord = QPointF(0,0);
}

void SpectraWidget::initPlot() {
    _titleText = QString();
    _zoomFactor = 0;
    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    scene()->setSceneRect(0, 0, width()-0, height()-0);

    setDragMode(QGraphicsView::RubberBandDrag);
    //setCacheMode(CacheBackground);
    //setRenderHint(QPainter::Antialiasing);
    //setTransformationAnchor(AnchorUnderMouse);
    //setResizeAnchor(AnchorViewCenter);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _arrow = new QGraphicsLineItem(0,0,0,0);
    scene()->addItem(_arrow);
    QPen redpen(Qt::red, 1,  Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
    QPen bluepen(Qt::blue, 1,  Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
    _arrow->setPen(bluepen);

    _varrow = new QGraphicsLineItem(0,0,0,0);
    scene()->addItem(_varrow);
    _varrow->setPen(redpen);

    QFont font = QApplication::font();
    font.setWeight(QFont::Bold);

    _note = new QGraphicsTextItem(0,0);
    scene()->addItem(_note);
    _note->setFont(font);
    _note->setDefaultTextColor(Qt::blue);

    _vnote = new QGraphicsTextItem(0,0);
    scene()->addItem(_vnote);
    _vnote->setFont(font);
    _vnote->setDefaultTextColor(Qt::red);

    _title = new QGraphicsTextItem(0,0);
    scene()->addItem(_title);

}

void SpectraWidget::setCurrentScan(Scan* scan) {

    qDebug() << "setCurrentScan: " << scan;

    if (!_currentScan) {
        _currentScan = new Scan(0,0,0,0,0,0); //empty scan;
    }

    if (scan ) {
        //if (_currentScan and scan->mslevel != _currentScan->mslevel) {
        //  _resetZoomFlag = true;
        //}
        links.clear();
        chargeStates.clear();
        peakClusters.clear();
        _currentScan->deepcopy(scan);
        _scanset.clear();

        if (scan->mslevel == 1) {
            chargeStates=_currentScan->assignCharges(mainwindow->getUserMassCutoff());
        }
    }
}

void SpectraWidget::replot() {
    drawGraph();
}


void SpectraWidget::setTitle(QString titleText) {

    // if (mainwindow) {
    //     mainwindow->spectraDockWidget->setWindowTitle("Spectra: " + title);

    QFont font = QApplication::font();

    int pxSize = scene()->height()*0.03;
    if ( pxSize < 12 ) pxSize = 12;
    if ( pxSize > 20 ) pxSize = 20;
    font.setPixelSize(pxSize);

    if (!_title) _title = scene()->addText(titleText, font);
    _title->setHtml(titleText);
    int titleWith = _title->boundingRect().width();
    _title->setPos(scene()->width()/2-titleWith/2, 3);
    _title->update();

    if (_currentScan->nobs() == 0) {
        font.setPixelSize(pxSize*3);
        _title->setHtml("EMPTY SCAN");
        int textWith = _title->boundingRect().width();
        _title->setPos(scene()->width()/2-textWith/2, scene()->height()/2);
        _title->setDefaultTextColor(QColor(200,200,200));
        _title->update();
    }
}

void SpectraWidget::setScan(Scan* scan) {
    if ( scan == NULL ) return;
    setCurrentScan(scan);
    cerr << "SpectraWidget::setScan(scan) " << endl;
    findBounds(true,true);
    drawGraph();
    repaint();
}

void SpectraWidget::setScan(Scan* scan, float mzmin, float mzmax) {
    if ( scan == NULL ) return;
    cerr << "SpectraWidget::setScan(scan,min,max) : " << scan->scannum << endl;
    setCurrentScan(scan);
    _minX = mzmin;
    _maxX = mzmax;
    findBounds(false,true);
	//mainwindow->getEicWidget()->setFocusLine(scan->rt); //TODO: Sahil, Removed while merging spectrawidget
    drawGraph();
    repaint();
}

void SpectraWidget::setScan(mzSample* sample, int scanNum=-1) {
    if (!sample) return;
    if (sample->scans.size() == 0 ) return;
    if (_currentScan && scanNum < 0 ) scanNum = _currentScan->scannum;
    if (scanNum > sample->scans.size() ) scanNum = sample->scans.size()-1;

    if ( scanNum >= 0 && scanNum < sample->scans.size() ) {
        setCurrentScan(sample->scans[ scanNum ]);
        cerr << "SpectraWidget::setScan(scan) " << endl;
        findBounds(false,true);
        drawGraph();
        repaint();
    }
}


void SpectraWidget::setScan(Peak* peak) {
    cerr << "SpectraWidget::setScan(peak) " << endl;
    links.clear();

    if (peak == NULL ) return;

    mzSample* sample = peak->getSample();
    if ( sample == NULL ) return;

    Scan* scan = sample->getScan(peak->scan);
    if ( scan == NULL ) return;

    setCurrentScan(scan);

    _focusCoord = QPointF(peak->peakMz,peak->peakIntensity);
    _minX = peak->peakMz-2;
    _maxX = peak->peakMz+4;
    _maxY = peak->peakIntensity*1.3;
    _minY = 0;

    //annotateScan(); //TODO: Sahil, Removed while merging spectrawidget
    drawGraph();
    repaint();
}

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging point
void SpectraWidget::overlayPeptideFragmentation(QString peptideSeq,MassCutoff *productMassCutoff) {
    qDebug() << "overlayPeptideFragmentation(): " << peptideSeq << " amuTolr=" << productMassCutoff->getMassCutoff() << endl;
    if(!_currentScan) return;
	if(peptideSeq.isEmpty()) return;

    Peptide record(peptideSeq.toStdString(),0,"");
	vector<FragmentIon*>ions;
    record.generateFragmentIons(ions,"CID");

	SpectralHit hit;
	hit.score = 0;
	hit.matchCount=0;
    hit.sampleName="";
    hit.productMassCutoff=productMassCutoff;
    hit.precursorMz=record.monoisotopicMZ();
	hit.scan = _currentScan;
	
    vector<bool>seen(_currentScan->nobs(),false);
	for(unsigned int i=0; i < ions.size(); i++) {
		FragmentIon* ion = ions[i];
        int pos = _currentScan->findClosestHighestIntensityPos(ion->m_mz,productMassCutoff);
        if(pos != -1 and seen[pos] == false) {
            ion->m_mzDiff = abs(_currentScan->mz[pos]-ion->m_mz);
            qDebug() << "overlayPeptideFragmentation: IONS: " << ion->m_ion.c_str() << " ->" << "ionType" << " " << ion->m_mz << " mzdiff=" << ion->m_mzDiff;

            hit.mzList << _currentScan->mz[pos];
            hit.intensityList << _currentScan->intensity[pos];
            hit.annotations << ion->m_ion.c_str();
            seen[pos]=true;
		}
	}

	delete_all(ions);
    overlaySpectralHit(hit);
}


/*
@author: Sahil
*/
//TODO: Sahil, Added while merging spectrawidget
void SpectraWidget::overlayCompoundFragmentation(Compound* c) {
    if(_currentScan and c->fragment_mzs.size()) {
		SpectralHit hit;
	  	hit.score = 0;
        hit.precursorMz = c->precursorMz;
        hit.matchCount=0;
        hit.sampleName="";
        hit.productMassCutoff=mainwindow->getUserMassCutoff();
        hit.scan=NULL;
        for(int i=0; i < c->fragment_mzs.size();i++)        hit.mzList << c->fragment_mzs[i];
        for(int i=0; i < c->fragment_intensity.size();i++)  hit.intensityList<< c->fragment_intensity[i];
        _spectralHit = hit;//copy hit
       	cerr << "SpectraWidge::overlayCompoundfragmentation(Compound)" << c->name << " " << c->precursorMz << endl;

       if(_currentScan) {
            if (_currentScan->mslevel != 1) {
                 overlaySpectralHit(_spectralHit);
            }
       }
	}
}


void SpectraWidget::overlaySpectralHit(SpectralHit& hit) {
        _spectralHit = hit;//copy hit

        //determine limits of overlayed spectra
        if(hit.scan and _currentScan != hit.scan) {
            setScan(hit.scan, hit.getMinMz()-0.5, hit.getMaxMz()+0.5);
            findBounds(false,true);
        }

        drawGraph();
        repaint();

        double focusMz = hit.mzList.first();
        int pos = _currentScan->findHighestIntensityPos(focusMz,hit.productMassCutoff);
        if(pos>=0) {
                _focusCoord.setX(focusMz);
                _focusCoord.setY(_currentScan->intensity[pos]);
                //setMzFocus(focusMz);
        }
}


void SpectraWidget::showConsensusSpectra(PeakGroup* group) {
    qDebug() << "showConsensusSpectra()";
    _scanset.clear();

    Scan* cons = group->getAverageFragmenationScan(mainwindow->getUserMassCutoff());

    if (cons) {
        _scanset = group->getFragmenationEvents();
        _currentScan=cons;
        _currentScan->scannum=0;
        _currentScan->sample=0;
        this->drawGraph();
    }
}


void SpectraWidget::drawSpectralHit(SpectralHit& hit) {

    MassCutoff *massCutoffWindow=hit.productMassCutoff;
    double maxIntensityHit= hit.getMaxIntensity();

    qDebug() << "overlaySpectra() started.." <<  massCutoffWindow->getMassCutoff() << "  " << maxIntensityHit <<  " " << hit.mzList.size() << endl;
    QPen redpen(Qt::red, 3);
    QPen bluepen(Qt::blue, 3);

    for(int i=0; i < hit.mzList.size(); i++) {
        int pos = _currentScan->findHighestIntensityPos(hit.mzList[i],massCutoffWindow);


        double hitIntensity=0;
            if (i < hit.intensityList.size()) hitIntensity= hit.intensityList[i];
        QString label;
            if (i < hit.annotations.size()) label += hit.annotations[i];

        //cerr << "drawSpectralHit() " << pos << " mz=" << setprecision(6) << hit.mzList[i];

        if (pos >= 0 && pos < _currentScan->nobs()) {
			//matched peak
            int x = toX(_currentScan->mz[pos]);
            int yA = toY(_currentScan->intensity[pos]);
            int yB = toY(_maxY*(hitIntensity/maxIntensityHit));
            QGraphicsLineItem* line = new QGraphicsLineItem(x,yA,x,toY(0),0);

            line->setPen(bluepen);
            scene()->addItem(line);
            _items.push_back(line);

            EicPoint* pA  = new EicPoint(x,yA,NULL,mainwindow);
            QColor bluecolorA = QColor(0,0,250);
            QBrush bluebrushA = QBrush(bluecolorA,Qt::SolidPattern);
            pA->setColor(bluecolorA); pA->setPen(bluepen); pA->setBrush(bluebrushA);
            pA->setPointShape(EicPoint::TRIANGLE_DOWN);
            scene()->addItem(pA);
            _items.push_back(pA);

            EicPoint* pB  = new EicPoint(x,yB,NULL,mainwindow);
            QColor bluecolorB = QColor(0,0,200);
            QBrush bluebrushB = QBrush(bluecolorB,Qt::SolidPattern);
            pB->setColor(bluecolorB); pB->setPen(bluepen); pB->setBrush(bluebrushB);
            pB->setPointShape(EicPoint::TRIANGLE_UP);
            scene()->addItem(pB);
            _items.push_back(pB);

            if (!label.isEmpty() ) {
                QGraphicsTextItem* text = new QGraphicsTextItem(label,0);
                scene()->addItem(text);
                text->setPos(x,yA-10);
                _items.push_back(text);
            }

        } else {
            /*
			//unmatched paek
            int x = toX(hit.mzList[i]);
            int y = toY(_maxY*(hitIntensity/maxIntensityHit));
            QGraphicsLineItem* line = new QGraphicsLineItem(x,y,x,toY(0),0);
            line->setPen(redpen);
            scene()->addItem(line);
            _items.push_back(line);
        	text->setPos(x-2,y-20);
            */
        }
    }

    /*
    QPen bluepen(Qt::blue,3);
    for(int i=0; i < mzs.size(); i++) {
        int x = toX(mzs[i]+0.01);
        int y = toY(_maxY*0.75);
        if (i < ints.size()) y = toY(_maxY*0.75*ints[i]);   // qDebug()<< "Overlay: " << x << " " << y;
        QGraphicsLineItem* line = new QGraphicsLineItem(x,y,x,toY(0),0);
        line->setPen(bluepen);
        scene()->addItem(line);
        _items.push_back(line);
    }
    */

    scene()->update();
}

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging spectrawidget
void SpectraWidget::clearGraph() {
    qDebug() << "drawSpectra() mzrange= " << _minX << "-" << _maxX;

    //clean up previous plot
    if ( _arrow ) _arrow->setVisible(false);
    for(unsigned int i=0; i < _items.size(); i++) {
        if(_items[i] != NULL) delete(_items[i]); _items[i]=NULL;
    }
    _items.clear();
    scene()->setSceneRect(10,10,this->width()-10, this->height()-10);
}
/*
@author: Sahil
*/
//TODO: Sahil, Added while merging spectrawidget
void SpectraWidget::setTitle(Scan* scan) {
    _titleText = QString();

    QString polarity;
    scan->getPolarity() > 0 ? polarity = "+" : polarity = "-";
    QString sampleName;
    if (_currentScan->sample)  sampleName = QString(scan->sample->sampleName.c_str());

    _titleText = tr("%1 scan#<b>%2</b>   rt:<b>%3</b>    ms:<b>%4</b>   ion:<b>%5</b> ").arg(
            sampleName,
            QString::number(scan->scannum),
            QString::number(scan->rt,'f',2),
            QString::number(scan->mslevel),
            polarity
    );

    if (scan->precursorMz) {
        _titleText += tr("preMZ:<b>%1</b> ").arg(QString::number(scan->precursorMz,'f',4));
    }

    if (scan->collisionEnergy) {
        _titleText += tr("ce:<b>%1</b> ").arg(QString::number(scan->collisionEnergy,'f',0));
    }

    if (scan->productMz) {    
       _titleText += tr("prodMz:<b>%1</b> ").arg(QString::number(scan->productMz,'f',3));
    }

    setTitle(_titleText);
}

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging spectrawidget
void SpectraWidget::drawScan(Scan* scan, QColor sampleColor, int
                             offsetXpx=0, int offsetYpx=0) {
    float _focusedMz = _focusCoord.x();

    QPen slineColor(sampleColor, 2);
    EicLine* sline = new EicLine(NULL,scene());
    sline->setColor(sampleColor);
    sline->setPen(slineColor);
   _items.push_back(sline);

   if( _profileMode ) {
        QBrush slineFill(sampleColor);
        sline->setFillPath(true);
        sline->setBrush(slineFill);
    } else {
       sline->setClosePath(false);
       sline->setFillPath(false);
   }

    int yzero = toY(0)+offsetYpx;
    for(int j=0; j<scan->nobs(); j++ ) {
        if ( scan->mz[j] < _minX  || scan->mz[j] > _maxX ) continue;
        int x = toX(scan->mz[j])+offsetXpx;
        int y = toY(scan->intensity[j])+offsetYpx;

        if( _profileMode ) {
               sline->addPoint(x,y);
        } else {
                sline->addPoint(x,yzero);
                sline->addPoint(x,y);

                //sline->addPoint(x,yzero);
        }

        if( abs(scan->mz[j]-_focusedMz)<0.002 ) {
            QPen redpen(Qt::red, 3);
            QGraphicsLineItem* line = new QGraphicsLineItem(x,y,x,yzero,0);
            scene()->addItem(line);
            line->setPen(redpen);
            _items.push_back(line);
        }
    }
}

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging spectrawidget
void SpectraWidget::drawScanSet(vector<Scan*>& scanset) {
    qDebug() << "drawScanSet() " << scanset.size();
 /*
    _minX = scanset[0]->minMz();
    _maxX = scanset[0]->maxMz();
    _maxY = scanset[0]->maxIntensity();

    for (int i=0; i < scanset.size(); i++ ) {
        Scan* scan = scanset[i];
        if (scan->minMz() < _minX) _minX = scanset[i]->minMz();
        if (scan->maxMz() > _maxX) _maxX = scanset[i]->maxMz();
        if (scan->maxIntensity() > _maxY) _maxY = scanset[i]->maxIntensity();
    }

    this->setTitle("Cluster Set");
    */

    for (unsigned int i=0; i < scanset.size(); i++ ) {

        QColor sampleColor(Qt::gray);
        if (scanset[i]->sample) {
            mzSample* sample = scanset[i]->sample;
            sampleColor = QColor::fromRgbF( sample->color[0], sample->color[1], sample->color[2], 0.6 );
        }
        drawScan(scanset[i],sampleColor,i,-i-3);
     }
}

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging spectrawidget
void SpectraWidget::drawMzLabels(Scan* scan) {
    if (!scan) return;

    //show labels
    unsigned int labelCount=0;
    QFont font = QApplication::font(); font.setPointSizeF(font.pointSize()*0.8);

    vector<int> positions = scan->intensityOrderDesc();
    float xRange = _maxX - _minX;
    float mzFloatAccuracy=1;
    if (xRange < 10 ) mzFloatAccuracy=6;
    else if (xRange < 50 ) mzFloatAccuracy=4;
    else if (xRange < 100 ) mzFloatAccuracy=2;

    for(int i=0; i < positions.size(); i++) {
        int pos = positions[i];
        if (scan->mz[pos] < _minX or scan->mz[pos] > _maxX) continue;
        if (scan->intensity[pos] / _maxY < 0.005 ) continue;

        //position label
        int x = toX(scan->mz[pos]);
        int y = toY(scan->intensity[pos]);

        //create label
        QString labelText = QString::number(scan->mz[pos],'f',mzFloatAccuracy);
        if (pos< chargeStates.size() and chargeStates[pos]>0)
            labelText += "<sup>z=" +  QString::number(chargeStates[pos]) + "</sup>";

        QGraphicsTextItem* text = new QGraphicsTextItem(labelText,0);
        text->setHtml(labelText);
        text->setFont(font);

        scene()->addItem(text);
        _items.push_back(text);
        text->setPos(x-2,y-20);

        Q_FOREACH(QGraphicsItem* item,  scene()->collidingItems(text) ) {
            if (qgraphicsitem_cast<QGraphicsTextItem *>(item)) {
                text->hide(); break;
            }
        }

        if(++labelCount > 50 ) break;
    }
}

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging spectrawidget
void SpectraWidget::drawAnnotations() {
   QFont font = QApplication::font(); font.setPointSizeF(font.pointSize()*0.8);

    for(int i=0; i < links.size(); i++ ) {
        if ( links[i].mz2 < _minX || links[i].mz2 > _maxX ) continue;

        QString labelText(links[i].note.c_str());
        QGraphicsTextItem* text = new QGraphicsTextItem(labelText,0);
        text->setHtml(labelText);
        text->setFont(font);
        scene()->addItem(text);
        _items.push_back(text);

        //position label
        int x = toX(links[i].mz2);
        int y = toY(links[i].value2);
        text->setPos(x-5,y-25);
    }
}

/*
@author: Sahil
*/
//TODO: Sahil, Refactored this to small functions whiler merging spectrawidget
void SpectraWidget::drawGraph() {
    if (_currentScan == NULL) return;

    qDebug() << "showSpectra() mzrange= " << _minX << "-" << _maxX;
    clearGraph();

    QColor sampleColor(Qt::black);
    if (_currentScan->sample) {
        mzSample* sample = _currentScan->sample;
        sampleColor = QColor::fromRgbF( sample->color[0], sample->color[1], sample->color[2], 1 );
    }

    setTitle(_currentScan);
    drawScanSet(_scanset);
    drawScan(_currentScan,sampleColor);
    drawMzLabels(_currentScan);
    drawAnnotations();

    // overlay spectra
    if ( fabs(_spectralHit.precursorMz - _currentScan->precursorMz) < 0.5 ) {
          drawSpectralHit(_spectralHit);
    //} else if (_spectralHit.scan !=NULL and _spectralHit.scan->scannum == _currentScan->scannum  ) {
    //      drawSpectralHit(_spectralHit);
    } else {
        qDebug() << " overlaySpectra() skipped: " << _spectralHit.precursorMz << " " << _currentScan->precursorMz;
    }

    addAxes();
}



void SpectraWidget::findBounds(bool checkX, bool checkY) {
    //bounds
    if (_currentScan == NULL) return;

    if (_currentScan->mz.size() == 0) {
        qDebug() << "Empty scan: " << _currentScan->scannum << endl;
        return;
    }

	float minMZ; float maxMZ;
	if ( _currentScan->mz.size() == 1 ) {
		minMZ = _currentScan->mz[0]-0.5;
		maxMZ = _currentScan->mz[0]+0.5;
	} else {
		minMZ = _currentScan->mz[0]-0.01;
		maxMZ = _currentScan->mz[_currentScan->mz.size()-1]+0.01;
	}



	cerr << _currentScan->filterLine << " " << _currentScan->nobs() << endl;
    cerr << "findBounds():  RANGE=" << minMZ << "-" << maxMZ << endl;
	if( _minX < minMZ) _minX = minMZ;
	if( _maxX > maxMZ) _maxX = maxMZ;

    if ( checkX ) { _minX = minMZ; _maxX = maxMZ; }
    if ( _minX == _maxX ) { _minX-=0.5; _maxX+=0.5; }

    if (checkY)  {
    	_minY = 0;
		_maxY = 1;
		for(int j=0; j<_currentScan->nobs(); j++ ) {
			if (_currentScan->mz[j] >= _minX && _currentScan->mz[j] <= _maxX) {
				if (_currentScan->intensity[j] > _maxY) _maxY = _currentScan->intensity[j];
            }
        }
    }

    _minY=0; _maxY *= 1.3;
   // cerr << "findBounds():  mz=" << _minX << "-" << _maxX << " ints=" << _minY << "-" << _maxY << endl;
}

void SpectraWidget::keyPressEvent( QKeyEvent *e ) {
    switch( e->key() ) {
    case Qt::Key_Left:
        showLastScan(); return;
    case Qt::Key_Right :
        showNextScan(); return;
    case Qt::Key_0 :
        resetZoom(); return;
    case Qt::Key_Plus :
        zoomIn(); return;
    case Qt::Key_Minus:
        zoomOut(); return;
    default:
        return;
    }
    e->accept();
}


void SpectraWidget::showNextScan() { incrementScan(+1, 0); }
void SpectraWidget::showLastScan() { incrementScan(-1, 0); }
void SpectraWidget::showNextFullScan() { incrementScan(+1, 1); }
void SpectraWidget::showLastFullScan() { incrementScan(-1, 1); }


void SpectraWidget::incrementScan(int increment, int msLevel=0 ) {
	if (_currentScan == NULL || _currentScan->sample == NULL) return;

	mzSample* sample = _currentScan->getSample();
    if (sample == NULL) return;

	Scan* newScan=sample->getScan(_currentScan->scannum+increment);
    if (msLevel != 0 && newScan && newScan->mslevel != msLevel ) {
        for(unsigned int i=newScan->scannum; i >= 0 && i< sample->scans.size(); i+=increment ) {
            newScan =sample->getScan(i);
            if ( newScan && newScan->mslevel==msLevel) break;
        }
    }
    if(newScan==NULL) return;

    //do not reset soom when moving between full scans
	if(newScan->mslevel == 1 && _currentScan->mslevel == 1) {
		setScan(newScan,_minX,_maxX);
    }  else {
        setScan(newScan);
    }
    //if ( _resetZoomFlag == true ) { resetZoom(); _resetZoomFlag=false; }
}


void SpectraWidget::resizeEvent (QResizeEvent * event) {
    QSize newsize = event->size();
    replot();
}

void SpectraWidget::enterEvent (QEvent *) {
    grabKeyboard();
}

void SpectraWidget::leaveEvent (QEvent*) {
    releaseKeyboard();
}

void SpectraWidget::addAxes() {

	if (_drawXAxis ) {
		Axes* x = new Axes(0,_minX, _maxX,10);
		scene()->addItem(x);
		x->setZValue(998);
		_items.push_back(x);
	}

    if (_drawYAxis ) {
    	Axes* y = new Axes(1,_minY, _maxY,10);
    	scene()->addItem(y);
        // y->setZValue(999);
        // y->showTicLines(false);
        y->setZValue(-10);
        y->showTicLines(true);
		y->setOffset(5);
    	_items.push_back(y);
    }

}

void SpectraWidget::mousePressEvent(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);
    _mouseStartPos = event->pos();

}

void SpectraWidget::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);

    _mouseEndPos	= event->pos();

    int deltaX =  _mouseEndPos.x() - _mouseStartPos.x();
    float deltaXfrac = (float) deltaX / (width()+1);


    if ( deltaXfrac > 0.01 ) {
        float xmin = invX( std::min(_mouseStartPos.x(), _mouseEndPos.x()) );
        float xmax = invX( std::max(_mouseStartPos.x(), _mouseEndPos.x()) );
        _minX = xmin;
        _maxX = xmax;
    } else if ( deltaXfrac < -0.01 ) {
		if ( _currentScan->mz.size() > 0 ) {
			float minmz = _currentScan->mz[0];
			float maxmz = _currentScan->mz[_currentScan->nobs()-1];
            _minX *= 0.9;
            _maxX *= 1.1;
            if (_minX < minmz ) _minX=minmz;
            if (_maxX > maxmz ) _maxX=maxmz;
        }
    } else if (_nearestCoord.x() > 0 ) {
        setMzFocus(_nearestCoord.x());
    }
    findBounds(false,true);
    replot();
}

void SpectraWidget::setMzFocus(Peak* peak) {
    setMzFocus(peak->peakMz);
}

void SpectraWidget::setMzFocus(float mz) {
	if (_currentScan == NULL) return;
    //int bestMatch=-1;
    //float bestMatchDiff=FLT_MAX;

	MassCutoff *massCutoff= mainwindow->getUserMassCutoff();

	if (_currentScan->mslevel==1) {
		int pos = _currentScan->findHighestIntensityPos(mz,massCutoff);
		if(pos>=0) { 
			float bestMz = _currentScan->mz[pos];
			mainwindow->setMzValue(bestMz);
			mainwindow->massCalcWidget->setCharge(_currentScan->getPolarity());
			mainwindow->massCalcWidget->setMass(bestMz);
		}

	} else if (_currentScan->mslevel==2 and _currentScan->precursorMz > 0) {
			float bestMz = _currentScan->precursorMz;
			mainwindow->setMzValue(bestMz);
			mainwindow->massCalcWidget->setCharge(_currentScan->getPolarity());
			mainwindow->massCalcWidget->setMass(bestMz);

	} else if (!_currentScan->filterLine.empty() ) {
		float mzmin = mz - massCutoff->massCutoffValue(mz);
		float mzmax = mz + massCutoff->massCutoffValue(mz);
    	mzSlice eicSlice = mainwindow->getEicWidget()->getParameters()->getMzSlice();
        mzSlice slice(mzmin,mzmax,eicSlice.rtmin,eicSlice.rtmax); 
		slice.srmId=_currentScan->filterLine;

        mainwindow->getEicWidget()->setMzSlice(slice);
		mainwindow->getEicWidget()->setFocusLine(_currentScan->rt);
        mainwindow->getEicWidget()->replotForced();
        return;
    }

/*

    int bestMatch=-1; 
    float bestMatchDiff=FLT_MAX;
	for (int i=0; i < _currentScan->nobs(); i++ ) {
		float diff = abs(_currentScan->mz[i] - mz);
        if ( diff < bestMatchDiff ) { bestMatchDiff = diff; bestMatch=i; }
    }

    if ( bestMatchDiff < 1 ) {
		float bestMz = _currentScan->mz[bestMatch];

        mainwindow->setMzValue(bestMz);
        mainwindow->massCalcWidget->setCharge(_currentScan->getPolarity());
        mainwindow->massCalcWidget->setMass(bestMz);
        //mainwindow->massCalc->compute();
    }
*/
}

void SpectraWidget::mouseDoubleClickEvent(QMouseEvent* event){
    QGraphicsView::mouseDoubleClickEvent(event);
    _focusCoord = _nearestCoord;
 //   annotateScan();
    drawGraph();

}

void SpectraWidget::addLabel(QString text,float x, float y) {
    QFont font = QApplication::font(); font.setPointSizeF(font.pointSize()*0.8);

    QGraphicsTextItem* _label = scene()->addText(text, font);
    _label->setPos(toX(x), toY(y));  
}

void SpectraWidget::mouseMoveEvent(QMouseEvent* event){
	if (_currentScan == NULL ) return;

    QGraphicsView::mouseMoveEvent(event);
    QPointF pos = event->pos();

    if (pos.y() < 5 || pos.y() > height()-5 || pos.x() < 5 || pos.y() > width()-5 ) {
        _vnote->hide(); _note->hide(); _varrow->hide(); _arrow->hide();
        return;
    }


    int nearestPos = findNearestMz(pos);
    if (nearestPos >= 0) {
		_nearestCoord = QPointF(_currentScan->mz[nearestPos], _currentScan->intensity[nearestPos]);
		drawArrow(_currentScan->mz[nearestPos], _currentScan->intensity[nearestPos], invX(pos.x()), invY(pos.y()));
        if (mainwindow->massCalcWidget->isVisible())
			mainwindow->massCalcWidget->setMass(_currentScan->mz[nearestPos]);
    } else {
        _vnote->hide(); _note->hide(); _varrow->hide(); _arrow->hide();
    }


}

int SpectraWidget::findNearestMz(QPointF pos) {

    float mz = invX(pos.x());
    float mzmin = invX(pos.x()-50);
    float mzmax = invX(pos.x()+50);
    float ycoord  =invY(pos.y());
    int best=-1;

	vector<int> matches = _currentScan->findMatchingMzs(mzmin,mzmax);
    if (matches.size() > 0) {
        float dist=FLT_MAX;
        for(int i=0; i < matches.size(); i++ ) {
            int p = matches[i];
			float d = sqrt(POW2(_currentScan->intensity[p]-ycoord)+POW2(_currentScan->mz[p]-mz));
            if ( d < dist ){ best=p; dist=d; }
        }


    }
    return best;
}

void SpectraWidget::drawArrow(float mz1, float intensity1, float mz2, float intensity2) {
    int x1=  toX(mz1);
    int y1=  toY(intensity1);

    int x2 = toX(mz2);
    int y2 = toY(intensity2);

    int x3 = toX(_focusCoord.x());
    int y3 = toY(_focusCoord.y());
    float mz3 = _focusCoord.x();

    if ( massCutoffDist(mz1,mz2,mainwindow->getUserMassCutoff()) < 0.1 ) return;


    if (_arrow != NULL ) {
        _arrow->setVisible(true);
        _arrow->setLine(x1,y1,x2,y2);
    }

    float distance = mz1-_focusCoord.x();
    //float totalInt = _focusCoord.y()+intensity1;
    float totalInt = _focusCoord.y();
    float diff=0;
    if (totalInt)  diff = intensity1/totalInt;

    if (_varrow != NULL && abs(distance) > 0.1 ) {
        _varrow->setLine(x1,y1,x3,y3);
        _vnote->setPos(x1+(x3-x1)/2.0, y1+(y3-y1)/2.0);
        _vnote->setPlainText(QString::number(diff*100,'f',2) + "%" );
        _varrow->show(); _vnote->show();
    } else {
        _varrow->hide(); _vnote->hide();
    }


    QString note = tr("m/z: %1 &Delta;%2").arg( QString::number(mz1,'f',6),QString::number(distance,'f',3));

    if (_note  != NULL ) {
        _note->setPos(x2+1,y2-30);
        _note->setHtml(note);
    }

    _note->show();
    _arrow->show();
}


void SpectraWidget::wheelEvent(QWheelEvent *event) {
    if ( event->delta() > 0 ) {
        zoomOut();
    } else {
        zoomIn();
    }
    replot();
}

/*
void SpectraWidget::annotateScan() {
    return;

    float mz1 = _focusCoord.x();
	if (mz1==0 || _currentScan == NULL) return;

	links = findLinks(mz1,_currentScan, 20, _currentScan->getPolarity());

    for(int i=0; i < links.size(); i++ ) {
		vector<int> matches = _currentScan->findMatchingMzs(links[i].mz2-0.01, links[i].mz2+0.01);
        //qDebug() << "annotateScan() " << links[i].note.c_str() << " " << links[i].mz2 << " " << matches.size();
        links[i].value2=0;

        if (matches.size() > 0) {
			links[i].value2 = _currentScan->intensity[matches[0]];

            for(int i=1; i < matches.size(); i++ ) {
				if(_currentScan->intensity[ matches[i] ] > links[i].value2 ) {
					links[i].value2==_currentScan->intensity[ matches[i] ];
                }
            }
        }
    }
}
*/

void SpectraWidget::assignCharges() {
    chargeStates = _currentScan->assignCharges(mainwindow->getUserMassCutoff());
}

void SpectraWidget::annotateScan() {

    float mzfocus = _focusCoord.x();
    if (mzfocus==0 || _currentScan == NULL || _currentScan->nobs() < 2 ) return;
    float noiseLevel=1;
    MassCutoff massCutoffMerge=*mainwindow->getUserMassCutoff();
    massCutoffMerge.setMassCutoff(100);
    float minSigNoiseRatio=3;
    int minDeconvolutionCharge=100;
    int maxDeconvolutionCharge=500;
    int minDeconvolutionMass=600;
    int maxDeconvolutionMass=2e5;
    int minChargeStates=2;

    ChargedSpecies* x = _currentScan->deconvolute(mzfocus,
                                                  noiseLevel,
                                                  &massCutoffMerge,
                                                  minSigNoiseRatio,
                                                  minDeconvolutionCharge,
                                                  maxDeconvolutionCharge,
                                                  minDeconvolutionMass,
                                                  maxDeconvolutionMass,
                                                  minChargeStates);

    if (x and x->observedMzs.size() >= 5) {
        for(unsigned int i=0; i< x->observedMzs.size(); i++) {
            QString noteText = tr("z=%1 M=%2").arg(x->observedCharges[i]).arg(x->deconvolutedMass);
            mzLink link = mzLink(mzfocus,x->observedMzs[i],noteText.toStdString());
            link.value2 = x->observedIntensities[i];
            links.push_back(link);
        }
        delete(x);
    }    
}



void SpectraWidget::resetZoom() {
    findBounds(true,true);
    replot();
}

void SpectraWidget::zoomIn() {
    float D = (_maxX-_minX)/2;
    if (D < 0.5 ) return;

    float _centerX = _minX+D;

    if (_focusCoord.x() != 0 && _focusCoord.x() > _minX && _focusCoord.x() < _maxX ) _centerX = _focusCoord.x();
	//cerr << "zoomIn center=" << _centerX << " D=" << D <<  " focus=" << _focusCoord.x() << endl;

    _minX = _centerX-D/2;
    _maxX = _centerX+D/2;
    //cerr << _centerX << " " << _minX << " " << _maxX << " " << _minZ << " " << _maxZ << endl;
    findBounds(false,true);
    replot();

}

void SpectraWidget::zoomOut() {
	cerr << "zoomOut" << endl;
    _minX = _minX * 0.9;
    _maxX = _maxX * 1.1;
    findBounds(false,true);
    replot();
}


void SpectraWidget::timerEvent(QTimerEvent* event) {


}

void SpectraWidget::compareScans(Scan* s1, Scan* s2) {

}

void SpectraWidget::contextMenuEvent(QContextMenuEvent * event) {
    event->ignore();
    QMenu menu;

    QAction* a0 = menu.addAction("Reset Zoom");
    connect(a0, SIGNAL(triggered()), SLOT(resetZoom()));

    QAction* a1 = menu.addAction("Go To Scan");
    connect(a1, SIGNAL(triggered()), SLOT(gotoScan()));

    QAction* a3b = menu.addAction("Find Similar Scans");
    connect(a3b, SIGNAL(triggered()), SLOT(findSimilarScans()));

    QAction* a3a = menu.addAction("Copy Top Peaks to Clipboard");
    connect(a3a, SIGNAL(triggered()), SLOT(spectraToClipboardTop()));

    QAction* a3 = menu.addAction("Copy Spectra to Clipboard");
    connect(a3, SIGNAL(triggered()), SLOT(spectraToClipboard()));

    QAction* a3c = menu.addAction("Copy Image to Clipboard");
    connect(a3c, SIGNAL(triggered()), SLOT(copyImageToClipboard()));

    QAction* a4 = menu.addAction("Profile Mode");
    connect(a4, SIGNAL(triggered()), SLOT(setProfileMode()));
    (a4, SIGNAL(triggered()), SLOT(spectraToClipboard()));

    QAction* a5 = menu.addAction("Centroided Mode");
    connect(a5, SIGNAL(triggered()), SLOT(setCentroidedMode()));


    QAction *selectedAction = menu.exec(event->globalPos());
}

void SpectraWidget::spectraToClipboard() {
	if(!_currentScan) return;

    QStringList clipboardText;
	for(int i=0; i < _currentScan->nobs(); i++ ) {
        clipboardText  << tr("%1\t%2")
				.arg(QString::number(_currentScan->mz[i],'f', 6))
				.arg(QString::number(_currentScan->intensity[i],'f',6));
    }
    QApplication::clipboard()->setText(clipboardText.join("\n"));

}

void SpectraWidget::spectraToClipboardTop() {
    if(!_currentScan) return;
    vector< pair<float,float> > mzarray= _currentScan->getTopPeaks(0.05, 3.0,40);

    QStringList clipboardText;
    for(int i=0; i < mzarray.size(); i++ ) {
    pair<float,float> p = mzarray[i];
            clipboardText  << tr("%1\t%2")
                .arg(QString::number(p.second,'f', 5))
                .arg(QString::number(p.first, 'f', 2));
    }
    QApplication::clipboard()->setText(clipboardText.join("\n"));
}

void SpectraWidget::gotoScan() { 
        if (_currentScan == NULL or _currentScan->sample == NULL) return;
		int curScanNum = _currentScan->scannum;
		int maxScanNum = _currentScan->sample->scans.size()-1;
		bool ok=false;

		int scanNumber = QInputDialog::getInt (this,
						"Go To Scan Number", "Enter Scan Number", curScanNum,
						0, maxScanNum, 1, &ok, 0);
		if (ok && scanNumber > 0 && scanNumber < maxScanNum) {
			Scan* newscan = _currentScan->sample->scans[scanNumber];
			if (newscan) setScan(newscan);
		}
}

vector<mzLink> SpectraWidget::findLinks(float centerMz, Scan* scan, MassCutoff *massCutoff, int ionizationMode) {

    vector<mzLink> links;
    //check for possible C13s
    /*
	for(int i=1; i<20; i++ ) {
        if(i==0) continue;
        float mz=centerMz+(i*1.0034);
        float mzz=centerMz+(i*1.0034)/2;
        float mzzz=centerMz+(i*1.0034)/3;

        if( scan->hasMz(mz,ppm) ) {
            QString noteText = tr("C13-[%1]").arg(i);
            links.push_back(mzLink(centerMz,mz,noteText.toStdString()));
        }

        if( i % 2 !=0 && mzz!=mz && scan->hasMz(mzz,ppm) ) {
            QString noteText = tr("C13-[%1] z=2").arg(i);
            links.push_back(mzLink(centerMz,mzz,noteText.toStdString()));
        }

        if( i % 3 !=0 && scan->hasMz(mzzz,ppm) ) {
            QString noteText = tr("C13-[%1] z=3").arg(i);
            links.push_back(mzLink(centerMz,mzzz,noteText.toStdString()));
        }
    }
	*/

    for(int i=0; i < DB.fragmentsDB.size(); i++ ) {
        Adduct* frag  = DB.fragmentsDB[i];
    	if(frag->charge != 0 && SIGN(frag->charge) != SIGN(ionizationMode) ) continue;
        float mzMinus=centerMz-frag->mass;
        float mzPlus =centerMz+frag->mass;
        if( scan->hasMz(mzPlus,massCutoff)) {
            QString noteText = tr("%1 Fragment").arg(QString(DB.fragmentsDB[i]->name.c_str()));
            links.push_back(mzLink(centerMz,mzPlus,noteText.toStdString()));
        }

        if( scan->hasMz(mzMinus,massCutoff)) {
            QString noteText = tr("%1 Fragment").arg(QString(DB.fragmentsDB[i]->name.c_str()));
            links.push_back(mzLink(centerMz,mzMinus,noteText.toStdString()));
        }
    }

    //parent check
    for(int i=0; i < DB.adductsDB.size(); i++ ) {
    	if ( SIGN(DB.adductsDB[i]->charge) != SIGN(ionizationMode) ) continue;
        float parentMass=DB.adductsDB[i]->computeParentMass(centerMz);
        parentMass += ionizationMode*HMASS;   //adjusted mass

        if( abs(parentMass-centerMz)>0.1 && scan->hasMz(parentMass,massCutoff)) {
            cerr << DB.adductsDB[i]->name << " " << DB.adductsDB[i]->charge << " " << parentMass << endl;
            QString noteText = tr("Possible Parent %1").arg(QString(DB.adductsDB[i]->name.c_str()));
            links.push_back(mzLink(centerMz,parentMass,noteText.toStdString()));
        }
    }

    //adduct check
    for(int i=0; i < DB.adductsDB.size(); i++ ) {
    	if ( SIGN(DB.adductsDB[i]->charge) != SIGN(ionizationMode) ) continue;
        float parentMass = centerMz-ionizationMode*HMASS;   //adjusted mass
        float adductMass=DB.adductsDB[i]->computeAdductMass(parentMass);
        if( abs(adductMass-centerMz)>0.1 && scan->hasMz(adductMass,massCutoff)) {
            QString noteText = tr("Adduct %1").arg(QString(DB.adductsDB[i]->name.c_str()));
            links.push_back(mzLink(centerMz,adductMass,noteText.toStdString()));
        }
    }


    return links;
}


void SpectraWidget::constructAverageScan(float rtmin, float rtmax) {
    if (_avgScan != NULL) delete(_avgScan);
    _avgScan=NULL;

    if (_currentScan && _currentScan->getSample()) {
        Scan* avgScan = _currentScan->getSample()->getAverageScan(rtmin,rtmax,_currentScan->mslevel,_currentScan->getPolarity(),(float)100.0);
        qDebug() << "constructAverageScan() " << rtmin << " " << rtmax << " mslevel=" << _currentScan->mslevel << endl;
        avgScan->simpleCentroid();
        if(avgScan) setScan(avgScan);
    }
}


void SpectraWidget::findSimilarScans() {
    if(!_currentScan) return;
    vector< pair<float,float> > mzarray= _currentScan->getTopPeaks(0.05, 1.0,40);

    QStringList clipboardText;
    for(int i=0; i < mzarray.size(); i++ ) {
    pair<float,float> p = mzarray[i];
            clipboardText  << tr("%1\t%2")
                .arg(QString::number(p.second,'f', 5))
                .arg(QString::number(p.first, 'f', 2));
    }

    mainwindow->spectraMatchingForm->fragmentsText->setPlainText(clipboardText.join("\n"));
    mainwindow->spectraMatchingForm->precursorMz->setText(QString::number(_currentScan->precursorMz,'f',6));
    mainwindow->spectraMatchingForm->show();

}

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging spectrawidget
void SpectraWidget::copyImageToClipboard() {
    QPixmap image(this->width(),this->height());
    image.fill(Qt::white);
    QPainter painter;
    painter.begin(&image);
    render(&painter);
    painter.end();
    QApplication::clipboard()->setPixmap(image);
}
