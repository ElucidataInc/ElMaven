#include "spectrawidget.h"

SpectraWidget::SpectraWidget(MainWindow* mw) { 
    this->mainwindow = mw;
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
    _title = NULL;
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

    QFont font("Helvetica", 10);
    font.setWeight(QFont::Bold);

    _note = new QGraphicsTextItem(0,0);
    scene()->addItem(_note);
    _note->setFont(font);
    _note->setDefaultTextColor(Qt::blue);

    _vnote = new QGraphicsTextItem(0,0);
    scene()->addItem(_vnote);
    _vnote->setFont(font);
    _vnote->setDefaultTextColor(Qt::red);

}

void SpectraWidget::setCurrentScan(Scan* scan) {

    if (!_currentScan) {
        _currentScan = new Scan(0,0,0,0,0,0); //empty scan;
    }

    if (scan ) {
        //if (_currentScan and scan->mslevel != _currentScan->mslevel) {
        //  _resetZoomFlag = true;
        //}
        links.clear();
        _currentScan->deepcopy(scan);
    }
}

void SpectraWidget::replot() {
    drawGraph();
}


void SpectraWidget::setTitle(QString title) {
    if (mainwindow) {
        mainwindow->spectraDockWidget->setWindowTitle("Spectra: " + title);
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
    mainwindow->getEicWidget()->setFocusLine(scan->rt);
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

    annotateScan();
    drawGraph();
    repaint();
}

void SpectraWidget::overlaySpectralHit(SpectralHit& hit) {
        _spectralHit = hit;//copy hit

        //determine limits of overlayed spectra
        setScan(hit.scan, hit.getMinMz()-0.5, hit.getMaxMz()+0.5);
        findBounds(false,true);

        drawGraph();
        repaint();

        double focusMz = hit.mzList.first();
        int pos = _currentScan->findHighestIntensityPos(focusMz,hit.productPPM);
        if(pos>=0) { 
                _focusCoord.setX(focusMz);
                _focusCoord.setY(_currentScan->intensity[pos]); 
                setMzFocus(focusMz);
        }
}

void SpectraWidget::overlaySpectra(QVector<double>mzs, QVector<double>ints) {

    float ppmWindow = _spectralHit.productPPM;
    double maxIntensity=_spectralHit.getMaxIntensity();

    QPen redpen(Qt::red, 3);
    QPen bluepen(Qt::blue, 3);
    for(int i=0; i < mzs.size(); i++) {
        int pos = _currentScan->findHighestIntensityPos(mzs[i],ppmWindow);
	int hitIntensity= 0.5;
	if (i < ints.size() ) hitIntensity= ints[i];


        if (pos > 0 && pos <= _currentScan->nobs()) {
            int x = toX(_currentScan->mz[pos]);
            int y = toY(_currentScan->intensity[pos]);
            QGraphicsLineItem* line = new QGraphicsLineItem(x,y,x,toY(0),0);
            line->setPen(bluepen);
            scene()->addItem(line);
            _items.push_back(line);
        } else {
            int x = toX(mzs[i]);
            int y = toY(_maxY*0.75*hitIntensity/maxIntensity);
            QGraphicsLineItem* line = new QGraphicsLineItem(x,y,x,toY(0),0);
            line->setPen(redpen);
            scene()->addItem(line);
            _items.push_back(line);
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

void SpectraWidget::drawGraph() {

    qDebug() << "showSpectra() mzrange= " << _minX << "-" << _maxX;
    //clean up previous plot
    if ( _arrow ) {
        _arrow->setVisible(false);
    }

    for(unsigned int i=0; i < _items.size(); i++) {
        if(_items[i] != NULL) delete(_items[i]); _items[i]=NULL;
    }
    _items.clear();

    scene()->setSceneRect(10,10,this->width()-10, this->height()-10);
    Scan* scan = _currentScan;
    if (scan == NULL ) return;
    mzSample* sample = scan->sample;
    if ( sample == NULL ) return;

    string sampleName = sample->sampleName;
    QString polarity = "0";
    scan->getPolarity() > 0 ? polarity = "Positive" : polarity = "Negative";


    QString title = tr("Sample:%1   Scan:%2   rt:%3   msLevel:%4  Ionization Mode:%5 ").arg(
            QString(sampleName.c_str()),
            QString::number(scan->scannum),
            QString::number(scan->rt,'f',2),
            QString::number(scan->mslevel),
            polarity
	);


    QColor sampleColor = QColor::fromRgbF( sample->color[0], sample->color[1], sample->color[2], 1 );

    if (scan->precursorMz) {
        QString precursorMzLink= "PrecursorMz: " + QString::number(scan->precursorMz,'f',3);
        title += precursorMzLink ;
    }

    if (scan->productMz) {
        QString precursorMzLink= " ProductMz: " + QString::number(scan->productMz,'f',3);
        title += precursorMzLink ;
    }


    this->setTitle(title);

    QPen pen(Qt::black, 2);
    float fontSize = scene()->height()*0.05;
    if ( fontSize < 1) fontSize=1;
    if ( fontSize > 10) fontSize=10;
    QFont font("Helvetica", fontSize);
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
    }

    QMap<float,int>shownPositions; //items sorted by key
    int yzero = toY(0);

    for(int j=0; j<scan->nobs(); j++ ) {
        if ( scan->mz[j] < _minX  || scan->mz[j] > _maxX ) continue;

        if ( scan->intensity[j] / _maxY > 0.005 ) {
            shownPositions[ scan->intensity[j] ] = j;
        }

        int x = toX(scan->mz[j]);
        int y = toY(scan->intensity[j]);

        if( _profileMode ) {
               sline->addPoint(x,y);
        } else {
                sline->addPoint(x,yzero);
                sline->addPoint(x,y);
                sline->addPoint(x,yzero);
        }

        if( abs(scan->mz[j]-_focusedMz)<0.002 ) {
            QPen redpen(Qt::red, 3);
            QGraphicsLineItem* line = new QGraphicsLineItem(x,y,x,toY(0),0);
            scene()->addItem(line);
            line->setPen(redpen);
            _items.push_back(line);
        }


    }


    //show labels
    QMap<float,int>::iterator itr; int labelCount=0;
    for(itr = shownPositions.end(); itr != shownPositions.begin(); itr--) {
        int pos = itr.value();
        if(pos < 0 || pos >= scan->mz.size()) continue;

        //create label
        QGraphicsTextItem* text = new QGraphicsTextItem(QString::number(scan->mz[pos],'f',6),0,0);
        text->setFont(font);
        scene()->addItem(text);
        _items.push_back(text);

        //position label
        int x = toX(scan->mz[pos]);
        int y = toY(scan->intensity[pos]);
        text->setPos(x-2,y-20);

        labelCount++;
        if(labelCount >= 20 ) break;
    }

    //show annotations
    for(int i=0; i < links.size(); i++ ) {
        if ( links[i].mz2 < _minX || links[i].mz2 > _maxX ) continue;

        //create label
       // Note* text = new Note(QString(links[i].note.c_str()) + " " + QString::number(links[i].mz2,'f',3));
        Note* label = new Note(QString(links[i].note.c_str()));
        scene()->addItem(label);
        _items.push_back(label);

        //position label
        int x = toX(links[i].mz2);
        int y = toY(links[i].value2);
        label->setPos(x-2,y-20);
        label->setExpanded(true);
    }

    if (_spectralHit.scan != NULL && _spectralHit.scan->scannum == _currentScan->scannum ) {
        overlaySpectra(_spectralHit.mzList, _spectralHit.intensityList);
    }

    addAxes();
}



void SpectraWidget::findBounds(bool checkX, bool checkY) {
    //bounds
	if (_currentScan == NULL || _currentScan->mz.size() == 0) return;

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
		y->setZValue(999);
        y->showTicLines(false);
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
    int bestMatch=-1; 
    float bestMatchDiff=FLT_MAX;

	float ppm= mainwindow->getUserPPM();
	float mzmin = mz - mz/1e6*ppm;
	float mzmax = mz + mz/1e6*ppm;
    mzSlice eicSlice = mainwindow->getEicWidget()->getMzSlice();

	if (!_currentScan->filterLine.empty() && _currentScan->mslevel==2) {
        mzSlice slice(mzmin,mzmax,eicSlice.rtmin,eicSlice.rtmax); 
		slice.srmId=_currentScan->filterLine;

        mainwindow->getEicWidget()->setMzSlice(slice);
		mainwindow->getEicWidget()->setFocusLine(_currentScan->rt);
        mainwindow->getEicWidget()->replotForced();
        return;
    }

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
}

void SpectraWidget::mouseDoubleClickEvent(QMouseEvent* event){
    QGraphicsView::mouseDoubleClickEvent(event);
    _focusCoord = _nearestCoord;
    annotateScan();
    drawGraph();

}

void SpectraWidget::addLabel(QString text,float x, float y) {
    QFont f("Helvetica");
    f.setPixelSize(10);
    QGraphicsTextItem* _label = scene()->addText(text, f); 
    _label->translate(toX(x), toY(y));
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

    if ( ppmDist(mz1,mz2) < 0.1 ) return;


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

    QString sign; if ( distance > 0 ) sign="+"; if (distance < 0) sign="-";
    QString note = tr("%1 [%2%3]").arg( QString::number(mz1,'f',4), sign, QString::number(distance,'f',4));

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

void SpectraWidget::annotateScan() {

    float mzfocus = _focusCoord.x();
	if (mzfocus==0 || _currentScan == NULL || _currentScan->nobs() < 2 ) return;
    ChargedSpecies* x = _currentScan->deconvolute(mzfocus,1,100,2,3,100,500,2e5,3);

    if (x) {
        for(unsigned int i=0; i< x->observedMzs.size(); i++) {
            QString noteText = tr("z=%1 M=%2").arg(x->observedCharges[i]).arg(x->deconvolutedMass);
            mzLink link = mzLink(mzfocus,x->observedMzs[i],noteText.toStdString());
            link.value2 = x->observedIntensities[i];
            links.push_back(link);
        }
        delete(x);
    }


    /*
    //construct vector of mz, intensity values
    vector<mzPoint> mz_int_pairs;
    for(int i=0; i < _currentScan->nobs(); i++ ) 
        mz_int_pairs.push_back(mzPoint( _currentScan->mz[i], _currentScan->intensity[i], 0 ));

    //sort mz-intensity values based on intensity
    std::sort(mz_int_pairs.begin(), mz_int_pairs.end(), mzPoint::compY);    


    int N = mz_int_pairs.size();
    float mz1 = mz_int_pairs[ N-1 ].x;    //most intense mz
    int zmin=1000; int zmax=0;      //range of plausable zvalues
    for(int i=N-2; i > 0; i-- ) {
        float mz2 = mz_int_pairs[i].x;
        if (mz1 < mz2) {
            float zf = (mz1+1)/(mz2-mz1); //guess charge
            int z = int(zf+0.5); float remainder = abs(zf-z);   
            if (remainder < 0.01 ) { cerr << mz1 << " " << mz2 << " " << z << " " << remainder << endl; }
            if(z < zmin) zmin=z; if (z > zmax ) zmax=z;
        } else {
            float zf = (mz2+1)/(mz1-mz2); //guess charge
            int z = int(zf+0.5); float remainder = abs(zf-z);   
            if (remainder < 0.01 ) { cerr << mz1 << " " << mz2 << " " << z << " " << remainder << endl; }
            if(z < zmin) zmin=z; if (z > zmax ) zmax=z;
        }
        //generate charge series
    }
    */

    /*
    for(int z=2; z <= 50; z++ ) {
       vector<float>species = _currentScan->chargeSeries(mzfocus,z);

        int count=0; bool lastMatched=false;
        for(int ii=0; ii < species.size(); ii++ ) {
            if (_currentScan->hasMz( species[ii], 50)) {
                //cerr << "\tz=" << ii << " m/z=" << species[ii] << " M=" << (species[ii]*ii)-ii << endl;
                count++;
                lastMatched=true;
            } else if (lastMatched == true) {   //last charge matched ..but this one didn't.. 
                break;
            }
        }
        if (count > 3) {
            cerr << mzfocus << " -> " << z << " " << count << endl;
            for(int ii=0; ii < species.size(); ii++ ) {
                if (_currentScan->hasMz( species[ii], 20)) {
                     cerr << "\tz=" << ii << " m/z=" << species[ii] << " M=" << (species[ii]*ii)-ii << endl;
                }
            }
         
        }
    }
    */
    
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
    vector< pair<float,float> > mzarray= _currentScan->getTopPeaks(0.05);

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
		if (_currentScan == NULL) return;
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

vector<mzLink> SpectraWidget::findLinks(float centerMz, Scan* scan, float ppm, int ionizationMode) {

    vector<mzLink> links;
    //check for possible C13s
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

    for(int i=0; i < DB.fragmentsDB.size(); i++ ) {
        Adduct* frag  = DB.fragmentsDB[i];
    	if(frag->charge != 0 && SIGN(frag->charge) != SIGN(ionizationMode) ) continue;
        float mzMinus=centerMz-frag->mass;
        float mzPlus =centerMz+frag->mass;
        if( scan->hasMz(mzPlus,ppm)) {
            QString noteText = tr("%1 Fragment").arg(QString(DB.fragmentsDB[i]->name.c_str()));
            links.push_back(mzLink(centerMz,mzPlus,noteText.toStdString()));
        }

        if( scan->hasMz(mzMinus,ppm)) {
            QString noteText = tr("%1 Fragment").arg(QString(DB.fragmentsDB[i]->name.c_str()));
            links.push_back(mzLink(centerMz,mzMinus,noteText.toStdString()));
        }
    }

    //parent check
    for(int i=0; i < DB.adductsDB.size(); i++ ) {
    	if ( SIGN(DB.adductsDB[i]->charge) != SIGN(ionizationMode) ) continue;
        float parentMass=DB.adductsDB[i]->computeParentMass(centerMz);
        parentMass += ionizationMode*HMASS;   //adjusted mass

        if( abs(parentMass-centerMz)>0.1 && scan->hasMz(parentMass,ppm)) {
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
        if( abs(adductMass-centerMz)>0.1 && scan->hasMz(adductMass,ppm)) {
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
    vector< pair<float,float> > mzarray= _currentScan->getTopPeaks(0.05);

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
