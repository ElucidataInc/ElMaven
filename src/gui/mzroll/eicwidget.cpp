
#include "eicwidget.h"

EicWidget::EicWidget(QWidget *p) {
	eicParameters = new EICLogic();
	parent = p;

	//default values
	_zoomFactor = 0.5;
	_minX = _minY = 0;
	_maxX = _maxY = 0;

	ymin=0;
	ymax=0;
	zoomFlag=false;

	setScene(new QGraphicsScene(this));

	_barplot = NULL;
	_boxplot = NULL;
	_isotopeplot = NULL;
	_focusLine = NULL;
	_selectionLine = NULL;
	_statusText = NULL;

	autoZoom(true);
	showPeaks(true);
	showEIC(true);
	showSpline(false);
	showCubicSpline(false);
	showBaseLine(true);
	showTicLine(false);
	showBicLine(false); //TODO: Sahil, added while merging eicwidget
    showNotes(false); //TODO: Sahil, added while merging eicwidget
	showIsotopePlot(true);
	showBarPlot(true);
	showBoxPlot(false);
    automaticPeakGrouping(true); //TODO: Sahil, added while merging eicwidget
    showMergedEIC(false); //TODO: Sahil, added while merging eicwidget
    showEICLines(false); //TODO: Sahil, added while merging eicwidget
    showMS2Events(false); //TODO: Sahil, added while merging eicwidget

    scene()->setItemIndexMethod(QGraphicsScene::NoIndex); //TODO: Sahil, uncommed this while merging eicwidget
    //scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex); //TODO: Sahil, commented it while merging eicwidget
	setDragMode(QGraphicsView::RubberBandDrag);
	setCacheMode(CacheBackground);
	setMinimumSize(QSize(1, 1));
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _areaIntegration=false; //TODO: Sahil, added while merging eicwidget
    _spectraAveraging=false; //TODO: Sahil, added while merging eicwidget
	_frozen=false;
	_freezeTime=0;
	_timerId=0;

    _mouseEndPos = _mouseStartPos = QPointF(0,0); //TODO: Sahil, added while merging eicwidget

	connect(scene(), SIGNAL(selectionChanged()), SLOT(selectionChangedAction()));

}

EicWidget::~EicWidget() {
	cleanup();
	scene()->clear();
}

void EicWidget::mousePressEvent(QMouseEvent *event) {
	//qDebug << "EicWidget::mousePressEvent(QMouseEvent *event)";
	//setFocus();

	_lastClickPos = event->pos();
	QGraphicsView::mousePressEvent(event);
	
	if (event->button() == Qt::LeftButton) {
		_mouseStartPos = event->pos();
	}
}

void EicWidget::mouseReleaseEvent(QMouseEvent *event) {
	//qDebug <<" EicWidget::mouseReleaseEvent(QMouseEvent *event)";
	QGraphicsView::mouseReleaseEvent(event);

	//int selectedItemCount = scene()->selectedItems().size();
	mzSlice bounds = eicParameters->visibleEICBounds();

	_mouseEndPos = event->pos();
	float rtmin = invX(std::min(_mouseStartPos.x(), _mouseEndPos.x()));
	float rtmax = invX(std::max(_mouseStartPos.x(), _mouseEndPos.x()));
	int deltaX = _mouseEndPos.x() - _mouseStartPos.x();
	ymin=invY(_mouseEndPos.y());
	ymax=invY(_mouseStartPos.y());
	_mouseStartPos = _mouseEndPos; //

	//user is holding shift while releasing the mouse.. integrate area
	if (_areaIntegration
			|| (event->button() == Qt::LeftButton
					&& event->modifiers() == Qt::ShiftModifier)) {
		toggleAreaIntegration(false);
		//minimum size for region to integrate is 0.01 seconds
		if (rtmax - rtmin > 0.01)
			integrateRegion(rtmin, rtmax);

	} else if (_spectraAveraging
			|| (event->button() == Qt::LeftButton
					&& event->modifiers() == Qt::ControlModifier)) {
		toggleSpectraAveraging(false);
		getMainWindow()->getSpectraWidget()->constructAverageScan(rtmin, rtmax);

	} else if (abs(deltaX) > 10 && event->button() == Qt::LeftButton) { //user released button and no items are selected
		if (deltaX > 0) {  //zoom in
			eicParameters->_slice.rtmin = rtmin;
			eicParameters->_slice.rtmax = rtmax;
			if (eicParameters->selectedGroup) {
				if (eicParameters->selectedGroup->meanRt > rtmin
						&& eicParameters->selectedGroup->meanRt < rtmax) {
					float d = std::max(
							abs(eicParameters->selectedGroup->meanRt - rtmin),
							abs(eicParameters->selectedGroup->meanRt - rtmax));
					eicParameters->_slice.rtmin =
							eicParameters->selectedGroup->meanRt - d;
					eicParameters->_slice.rtmax =
							eicParameters->selectedGroup->meanRt + d;
				}
			}
			zoomFlag=true;
		} else if (deltaX < 0) {	 //zoomout
			//zoom(_zoomFactor * 1.2 );
			eicParameters->_slice.rtmin *= 0.8;
			eicParameters->_slice.rtmax *= 1.22;
			if (eicParameters->_slice.rtmin < bounds.rtmin)
				eicParameters->_slice.rtmin = bounds.rtmin;
			if (eicParameters->_slice.rtmax > bounds.rtmax)
				eicParameters->_slice.rtmax = bounds.rtmax;
		}
		replot (eicParameters->selectedGroup);
		if(eicParameters->getSelectedGroup()) addPeakPositions(eicParameters->getSelectedGroup());
	}
}

void EicWidget::integrateRegion(float rtmin, float rtmax) {
	//qDebug <<" EicWidget::integrateRegion(float rtmin, float rtmax)";
	//qDebug << "Integrating area from " << rtmin << " to " << rtmax;

	eicParameters->_integratedGroup.clear();
	QSettings *settings = getMainWindow()->getSettings();
    eicParameters->_integratedGroup.minQuality = getMainWindow()->mavenParameters->minQuality;
	eicParameters->_integratedGroup.compound = eicParameters->_slice.compound;
	eicParameters->_integratedGroup.srmId = eicParameters->_slice.srmId;
	eicParameters->_integratedGroup.setSelectedSamples(getMainWindow()->samples);
	for (int i = 0; i < eicParameters->eics.size(); i++) {
		EIC* eic = eicParameters->eics[i];
		Peak peak(eic, 0);

		for (int j = 0; j < eic->size(); j++) {
			if (eic->rt[j] >= rtmin && eic->rt[j] <= rtmax) {
				if (peak.minpos == 0) {
					peak.minpos = j;
					peak.rtmin = eic->rt[j];
				}
				if (peak.maxpos < j) {
					peak.maxpos = j;
					peak.rtmax = eic->rt[j];
				}
				peak.peakArea += eic->intensity[j];
				peak.rtmin = rtmin;
				peak.rtmax = rtmax;
				peak.mzmin = eicParameters->_slice.mzmin;
				peak.mzmax = eicParameters->_slice.mzmax;

				if (eic->intensity[j] > peak.peakIntensity) {
					peak.peakIntensity = eic->intensity[j];
					peak.pos = j;
					peak.rt = eic->rt[j];
					peak.peakMz = eic->mz[j];
				}
			}
		}
		if (peak.pos > 0) {

			eic->getPeakDetails(peak);

			ClassifierNeuralNet* clsf = getMainWindow()->getClassifier();
			if (clsf != NULL) {
				peak.quality = clsf->scorePeak(peak);
			}

			bool isIsotope = false;
			PeakFiltering peakFiltering(getMainWindow()->mavenParameters, isIsotope);

			if (!peakFiltering.filter(peak))
			{
				eicParameters->_integratedGroup.addPeak(peak);
				qDebug() << eic->sampleName.c_str() << " " << peak.peakArea << " "
						<< peak.peakAreaCorrected;
				this->showPeakArea(&peak);
			}
		}
	}

	eicParameters->_integratedGroup.groupStatistics();
	getMainWindow()->isotopeWidget->updateIsotopicBarplot(&eicParameters->_integratedGroup);
	getMainWindow()->isotopeWidget->setPeakGroupAndMore(&eicParameters->_integratedGroup, true);
}

void EicWidget::mouseDoubleClickEvent(QMouseEvent* event) {
	//qDebug <<" EicWidget::mouseDoubleClickEvent(QMouseEvent* event)";
	QGraphicsView::mouseDoubleClickEvent(event);

	QPointF pos = event->pos();
	float rt = invX(pos.x());

	vector<mzSample*> samples = getMainWindow()->getVisibleSamples();

	Scan* selScan = NULL;
	float minDiff = FLT_MAX;
	for (int i = 0; i < samples.size(); i++) {
		for (int j = 0; j < samples[i]->scans.size(); j++) {
			//if ( samples[i]->scans[j]->mslevel != 1) continue;
			float diff = abs(samples[i]->scans[j]->rt - rt);
			if (diff < minDiff) {
				minDiff = diff;
				selScan = samples[i]->scans[j];
			}
		}
	}

	if (selScan != NULL) { 
        //setFocusLine(selScan->rt);
        Q_EMIT(scanChanged(selScan)); //TODO: Sahil, added while merging eicwidget
	}
}

void EicWidget::selectionChangedAction() {
	//qDebug <<" EicWidget::selectionChangedAction()";
	QList<QGraphicsItem*> items = scene()->selectedItems();
	if (items.size()) {
		if (QGraphicsPixmapItem *note =
				qgraphicsitem_cast<QGraphicsPixmapItem *>(items[0])) {
			QVariant v = note->data(0);
			int noteid = v.value<int>();
			getMainWindow()->notesDockWidget->showNote(noteid);
		}
	}
}

void EicWidget::setFocusLine(float rt) {
	//qDebug <<" EicWidget::setFocusLine(float rt)";
	_focusLineRt = rt;
	if (_focusLine == NULL)
		_focusLine = new QGraphicsLineItem(0);
	if (_focusLine->scene() != scene())
		scene()->addItem(_focusLine);

	QPen pen(Qt::red, 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
	_focusLine->setPen(pen);
	_focusLine->setLine(toX(rt), 0, toX(rt), height());
}

void EicWidget::drawSelectionLine(float rtmin, float rtmax) {
	//qDebug <<" EicWidget::drawSelectionLine(float rtmin, float rtmax)";
	if (_selectionLine == NULL)
		_selectionLine = new QGraphicsLineItem(0);
	if (_selectionLine->scene() != scene())
		scene()->addItem(_selectionLine);

	QPen pen(Qt::red, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	_selectionLine->setPen(pen);
	_selectionLine->setZValue(1000);
	_selectionLine->setLine(toX(rtmin), height() - 20, toX(rtmax),
			height() - 20);
	_selectionLine->update();
}

void EicWidget::setScan(Scan* scan) {
	//qDebug <<" EicWidget::setScan(Scan* scan)";
	if (scan == NULL)
		return;
	getMainWindow()->spectraWidget->setScan(scan,
			eicParameters->_slice.rtmin - 5, eicParameters->_slice.rtmax + 5);

}

void EicWidget::mouseMoveEvent(QMouseEvent* event) {
// //qDebug <<" EicWidget::mouseMoveEvent(QMouseEvent* event)";
	QGraphicsView::mouseMoveEvent(event);
	//QPoint posi = event->globalPos();
	//QToolTip::showText(posi, "(" + QString::number(rt,'f',4) + " " + QString::number(intensity,'f',2) + ")" , this);
	if (_mouseStartPos != _mouseEndPos) {

		if (event->modifiers() == Qt::ShiftModifier) {
			QPointF pos = event->pos();
			float rt = invX(pos.x());
			float rtA = invX(_mouseStartPos.x());
			drawSelectionLine(min(rt, rtA), max(rt, rtA));
		}

	}
}

void EicWidget::cleanup() {
	//qDebug <<" EicWidget::cleanup()";
	//remove groups
	delete_all(eicParameters->eics);
	eicParameters->peakgroups.clear();
	if (_showTicLine == false && eicParameters->tics.size() > 0) {
		delete_all(eicParameters->tics);
	}
	clearPlot();
}

void EicWidget::computeEICs() {

	//CAN BE REPLACED BY PULLING SETTINGS FROM mavenParameters.	
	vector<mzSample*> samples = getMainWindow()->getVisibleSamples();
	if (samples.size() == 0)
		return;

	QSettings *settings = getMainWindow()->getSettings();
	int eic_smoothingWindow =
			getMainWindow()->mavenParameters->eic_smoothingWindow;
	int eic_smoothingAlgorithm =
			getMainWindow()->mavenParameters->eic_smoothingAlgorithm;
    float amuQ1 = getMainWindow()->mavenParameters->amuQ1;
    float amuQ3 = getMainWindow()->mavenParameters->amuQ3;
	int baseline_smoothing = getMainWindow()->mavenParameters->baseline_smoothingWindow;
	int baseline_quantile = getMainWindow()->mavenParameters->baseline_dropTopX;
	double minSignalBaselineDifference = getMainWindow()->mavenParameters->minSignalBaselineDifference;
	int eic_type = getMainWindow()->mavenParameters->eicType;
	string filterline = getMainWindow()->mavenParameters->filterline;

	mzSlice bounds = visibleSamplesBounds();

	eicParameters->getEIC(bounds, samples, eic_smoothingWindow,
			eic_smoothingAlgorithm, amuQ1, amuQ3, baseline_smoothing,
			baseline_quantile, minSignalBaselineDifference, eic_type,
			filterline);

	//score peak quality
	ClassifierNeuralNet* clsf = getMainWindow()->getClassifier();
	if (clsf != NULL) {
		clsf->scoreEICs(eicParameters->eics);
	}

	bool isIsotope = false;
	PeakFiltering peakFiltering(getMainWindow()->mavenParameters, isIsotope);
	peakFiltering.filter(eicParameters->eics);

	if(_groupPeaks) groupPeaks(); //TODO: Sahil, added while merging eicwidget
	eicParameters->associateNameWithPeakGroups();

}

mzSlice EicWidget::visibleSamplesBounds() {

	vector<mzSample*> samples = getMainWindow()->getVisibleSamples();
	return eicParameters->visibleSamplesBounds(samples);

}

void EicWidget::findPlotBounds() {
	//qDebug <<" EicWidget::findPlotBounds()";

	//mzSlice bounds = eicParameters->visibleEICBounds();

	//get bounds
	if (eicParameters->eics.size() == 0)
		return;
	if (eicParameters->_slice.rtmin == 0 && eicParameters->_slice.rtmax == 0)
		return;
	//qDebug() << "EicWidget::findPlotBounds()";

	_minX = eicParameters->_slice.rtmin;
	_maxX = eicParameters->_slice.rtmax;

	_minY =0 ;
	_maxY = 0;       //intensity
	if(zoomFlag){
		_maxY=std::max(ymin,ymax);
		_maxY = (_maxY * 1.3) + 1;
		zoomFlag=false;
		return;
	}

	/*/
	 //full proof version
	 for(int i=0; i < eics.size(); i++ ) {
	 EIC* eic = eics[i];
	 for (int j=0; j < eic->size(); j++ ){
	 if ( eic->rt[j] < _slice.rtmin) continue;
	 if ( eic->rt[j] > _slice.rtmax) continue;
	 if ( eic->intensity[j] > _maxY ) _maxY=eic->intensity[j];
	 }
	 }
	 */

	//approximate version
	for (int i = 0; i < eicParameters->peakgroups.size(); i++) {
		if (mzUtils::checkOverlap(eicParameters->peakgroups[i].minRt,
				eicParameters->peakgroups[i].maxRt, eicParameters->_slice.rtmin,
				eicParameters->_slice.rtmax) > 0) {
			if (eicParameters->peakgroups[i].maxHeightIntensity > _maxY) {
				_maxY = eicParameters->peakgroups[i].maxHeightIntensity;
			}
		}
	}
	float mx=0;
	for(int i=0;i<eicParameters->eics.size();++i)
			mx=max(mx,eicParameters->eics[i]->maxIntensity);
	if(_maxY==0) _maxY=mx;
	//if(_minY <= 0) _minY = 0;
	_maxY = (_maxY * 1.3) + 1;
	if (_minX > _maxX)
		swap(_minX, _maxX);
	//qDebug() << "EicWidget::findPlotBounds()" << _slice.rtmin << " " << _slice.rtmax << " " << _minY << " " << _maxY;
}

float EicWidget::toX(float x) {
	if (_minX == _maxX || x < _minX || x > _maxX)
		return 0;
	return ((x - _minX) / (_maxX - _minX) * scene()->width());
}

float EicWidget::toY(float y) {
	if (_minY == _maxY || y < _minY || y > _maxY)
		return 0;
	return (scene()->height()
			- ((y - _minY) / (_maxY - _minY) * scene()->height()));
}

float EicWidget::invX(float x) {
	if (_minX == _maxX)
		return 0;
	return (x / scene()->width()) * (_maxX - _minX) + _minX;
}

float EicWidget::invY(float y) {
	if (_minY == _maxY)
		return 0;
	return -1
			* ((y - scene()->height()) / scene()->height() * (_maxY - _minY)
					+ _minY);

}

void EicWidget::replotForced() {
	//qDebug <<" EicWidget::replotForced()";
	if (isVisible()) {
		recompute();
		replot();

	}
}

void EicWidget::replot() {
	//qDebug <<" EicWidget::replot()";
	if (isVisible()) {	
		replot(eicParameters->getSelectedGroup());
	}
}

void EicWidget::addEICLines(bool showSpline, bool showEIC) {
	//qDebug <<" EicWidget::addEICLines(bool showSpline)";

	//sort eics by peak height of selected group
	vector<Peak> peaks;
	if (eicParameters->getSelectedGroup()) {
		PeakGroup* group = eicParameters->getSelectedGroup();
		peaks = group->getPeaks();
		sort(peaks.begin(), peaks.end(), Peak::compIntensity);
	} else {
		std::sort(eicParameters->eics.begin(), eicParameters->eics.end(),
				EIC::compMaxIntensity);
	}

	//display eics
	for (unsigned int i = 0; i < eicParameters->eics.size(); i++) {
		EIC* eic = eicParameters->eics[i];
		if (eic->size() == 0)
			continue;
		if (eic->sample != NULL && eic->sample->isSelected == false)
			continue;
		if (eic->maxIntensity <= 0)
			continue;
		EicLine* lineEIC = new EicLine(0, scene());
		EicLine* lineSpline = new EicLine(0, scene());

		//sample stacking..
		int zValue = 0;
		for (int j = 0; j < peaks.size(); j++) {
			if (peaks[j].getSample() == eic->getSample()) {
				zValue = j;
				break;
			}
		}

		//ignore EICs that do not fall within current time range
		for (int j = 0; j < eic->size(); j++) {
			if (eic->rt[j] < eicParameters->_slice.rtmin)
				continue;
			if (eic->rt[j] > eicParameters->_slice.rtmax)
				continue;
			if (showSpline) {
				lineSpline->addPoint(QPointF(toX(eic->rt[j]), toY(eic->spline[j])));
			}
			if (showEIC){
				lineEIC->addPoint(
					QPointF(toX(eic->rt[j]), toY(eic->intensity[j])));
			}
		}
		QColor pcolor = QColor::fromRgbF(eic->color[0], eic->color[1],
				eic->color[2], 0.5).darker();
		QPen pen(pcolor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
		QColor bcolor = QColor::fromRgbF(eic->color[0], eic->color[1],
				eic->color[2], 0.5);
		QBrush brush(bcolor);

		//TODO: Sahil, added while merging eicwidget
        if(_showEICLines) {
             brush.setStyle(Qt::NoBrush);
            lineEIC->setFillPath(false);
        } else {
             brush.setStyle(Qt::SolidPattern);
             lineEIC->setFillPath(true);
        }


		lineEIC->setZValue(zValue);
		lineEIC->setFillPath(true);
		lineEIC->setEIC(eic);
		lineEIC->setBrush(brush);
		lineEIC->setPen(pen);
		lineEIC->setColor(pcolor);

		QColor splinebcolor = QColor::fromRgbF(eic->color[0], eic->color[1],
				eic->color[2], 0.7);
		QBrush splineBrush(splinebcolor);

		lineSpline->setZValue(zValue);
		lineSpline->setFillPath(true);
		lineSpline->setEIC(eic);
		lineSpline->setBrush(splineBrush);
		lineSpline->setPen(pen);
		lineSpline->setColor(pcolor);


		//lineEIC->fixEnds();
	}
}

/*
@author: Sahil
*/
//TODO: sahil Added while merging eicWidget
void EicWidget::addCubicSpline() {
    qDebug() <<" EicWidget::addCubicSpline()";
    QTime timerZ; timerZ.start();
    //sort eics by peak height of selected group
    vector<Peak> peaks;
    if (eicParameters->getSelectedGroup()) {
        PeakGroup* group=eicParameters->getSelectedGroup();
        peaks=group->getPeaks();
        sort(peaks.begin(), peaks.end(), Peak::compIntensity);
    } else {
        std::sort(eicParameters->eics.begin(), eicParameters->eics.end(), EIC::compMaxIntensity);
    }

    //display eics
    for( unsigned int i=0; i< eicParameters->eics.size(); i++ ) {
        EIC* eic = eicParameters->eics[i];
        if (eic->size()==0) continue;
        if (eic->sample != NULL && eic->sample->isSelected == false) continue;
        if (eic->maxIntensity <= 0) continue;
        EicLine* line = new EicLine(0,scene());

        //sample stacking..
        int zValue=0;
        for(int j=0; j < peaks.size(); j++ ) {
            if (peaks[j].getSample() == eic->getSample()) { zValue=j; break; }
        }


        unsigned int n = eic->size();
        float* x = new float[n];
        float* f = new float[n];
        float* b = new float[n];
        float* c = new float[n];
        float* d = new float[n];

        int N=0;
        for(int j=0; j<n; j++) {
            if ( eic->rt[j] < eicParameters->_slice.rtmin || eic->rt[j] > eicParameters->_slice.rtmax) continue;
            x[j]=eic->rt[j];
            f[j]=eic->intensity[j];
            b[j]=c[j]=d[j]=0; //init all elements to 0
            if(eic->spline[j]>eic->baseline[j] and eic->intensity[j]>0) {
                x[N]=eic->rt[j]; f[N]=eic->intensity[j];
                N++;
            } else if (eic->spline[j] <= eic->baseline[j]*1.1) {
                x[N]=eic->rt[j]; f[N]=eic->baseline[j];
                N++;
            }
        }

        if(N <= 2) continue;
        mzUtils::cubic_nak(N,x,f,b,c,d);

        for(int j=1; j<N; j++) {
            float rtstep = (x[j]-x[j-1])/10;
            for(int k=0; k<10; k++) {
                float dt = rtstep*k;
                float y = f[j-1] + ( dt ) * ( b[j-1] + ( dt ) * ( c[j-1] + (dt) * d[j-1] ) );
                //float y = mzUtils::spline_eval(n,x,f,b,c,d,x[j]+dt);
                if(y < 0) y= 0;
                line->addPoint(QPointF(toX(x[j-1]+dt), toY(y)));
            }
        }

        delete[] x;
        delete[] f;
        delete[] b;
        delete[] c;
        delete[] d;

        QColor pcolor = QColor::fromRgbF( eic->color[0], eic->color[1], eic->color[2], 0.3 );
        QPen pen(pcolor, 2);
        QColor bcolor = QColor::fromRgbF( eic->color[0], eic->color[1], eic->color[2], 0.3 );
        QBrush brush(bcolor);

        line->setZValue(zValue);
        line->setEIC(eic);
        line->setBrush(brush);
        line->setPen(pen);
        line->setColor(pcolor);
        brush.setStyle(Qt::SolidPattern);
        line->setFillPath(true);

    }
    qDebug() << "\t\taddCubicSpline() done. msec=" << timerZ.elapsed();
}


void EicWidget::addTicLine() {
	//qDebug <<" EicWidget::addTicLine()";

	vector<mzSample*> samples = getMainWindow()->getVisibleSamples();
	if (eicParameters->tics.size() == 0
			|| eicParameters->tics.size() != samples.size()) {
		delete_all (eicParameters->tics);
		for (int i = 0; i < samples.size(); i++) {
			int mslevel = 1;
			//attempt at automatically detecting correct scan type for construstion of TIC

            if  (samples[i]->scans.size() > 0) mslevel=samples[i]->scans[0]->mslevel;

			EIC* chrom=NULL;

			if (_showBicLine ) { 
            	chrom = samples[i]->getBIC(0,0,mslevel);
			} else if (_showTicLine ) {
            	chrom = samples[i]->getTIC(0,0,mslevel);
			}
			if (chrom != NULL) eicParameters->tics.push_back(chrom);
		}
	}

	float tmpMaxY = _maxY;
	float tmpMinY = _minY;

	for (unsigned int i = 0; i < eicParameters->tics.size(); i++) {
		EIC* tic = eicParameters->tics[i];
		if (tic->size() == 0)
			continue;
		if (tic->sample != NULL && tic->sample->isSelected == false)
			continue;
		EicLine* line = new EicLine(0, scene());
		line->setEIC(tic);

		_maxY = tic->maxIntensity;
		_minY = 0;

		for (int j = 0; j < tic->size(); j++) {
			if (tic->rt[j] < eicParameters->_slice.rtmin)
				continue;
			if (tic->rt[j] > eicParameters->_slice.rtmax)
				continue;
			line->addPoint(QPointF(toX(tic->rt[j]), toY(tic->intensity[j])));
		}

		mzSample* s = tic->sample;
		QColor color = QColor::fromRgbF(s->color[0], s->color[1], s->color[2],
				s->color[3]);
		line->setColor(color);

		QBrush brush(color, Qt::Dense5Pattern);
		line->setBrush(brush);
		line->setFillPath(true);
		line->setZValue(-i);	//tic should not be in foreground

		QPen pen = Qt::NoPen;
		line->setPen(pen);

		//line->fixEnds();
	}

	//restore min and max Y
	_maxY = tmpMaxY;
	_minY = tmpMinY;
}

void EicWidget::addMergedEIC() {
	//qDebug <<" EicWidget::addMergedEIC()";

	QSettings* settings = this->getMainWindow()->getSettings();

    int eic_smoothingWindow = getMainWindow()->mavenParameters->eic_smoothingWindow;
    int eic_smoothingAlgorithm = getMainWindow()->mavenParameters->eic_smoothingAlgorithm;

    EicLine* line = new EicLine(0, scene());

	EIC* eic = EIC::eicMerge(eicParameters->eics);
	eic->setSmootherType((EIC::SmootherType) eic_smoothingAlgorithm);
	eic->computeSpline(eic_smoothingWindow);


	for (int j = 0; j < eic->size(); j++) {
		if (eic->rt[j] < eicParameters->_slice.rtmin)
			continue;
		if (eic->rt[j] > eicParameters->_slice.rtmax)
			continue;
		line->addPoint(QPointF(toX(eic->rt[j]), toY(eic->spline[j])));
	}

	QColor color = QColor::fromRgbF(0.1, 0.1, 0.1, 1);
	QPen pen(color, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	QBrush brush(color);

	line->setEIC(eic);
	line->setBrush(brush);
	line->setPen(pen);
	line->setColor(color);

    if(_showBaseline) {
        addBaseLine(eic);
    }

}

void EicWidget::addBaseLine(EIC* eic) {
    QSettings* settings = this->getMainWindow()->getSettings();
    int baseline_smoothing = getMainWindow()->mavenParameters->baseline_smoothingWindow;
    int baseline_quantile = getMainWindow()->mavenParameters->baseline_dropTopX;

    eic->computeBaseLine(baseline_smoothing, baseline_quantile);
//        //debug
//        for(int j=0;j<eic->size();j++) {
//            qDebug() << eic->rt[j] << "\t" << eic->baseline[j];
//        }
    if (eic->size() == 0)
        return;
    EicLine* line = new EicLine(0, scene());
    line->setEIC(eic);

    float baselineSum = 0;
    for (int j = 0; j < eic->size(); j++) {
        if (eic->rt[j] < eicParameters->_slice.rtmin)
            continue;
        if (eic->rt[j] > eicParameters->_slice.rtmax)
            continue;
        baselineSum += eic->baseline[j];
        line->addPoint(QPointF(toX(eic->rt[j]), toY(eic->baseline[j])));
    }

    if (baselineSum == 0) {
        qDebug() << "baseline sum was 0";
        return;
    }

    QColor color = QColor::fromRgbF( eic->color[0], eic->color[1], eic->color[2], 1 );
    line->setColor(color);
	line->setZValue(5);

    float linewidth=2.0;
    QPen pen(color, linewidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    line->setPen(pen);
}

void EicWidget::addBaseLine() {
	qDebug() << " EicWidget::addBaseLine()";

	for (unsigned int i = 0; i < eicParameters->eics.size(); i++) {
		EIC* eic = eicParameters->eics[i];
        addBaseLine(eic);
    }
}



void EicWidget::showPeakArea(Peak* peak) {
	//qDebug <<" EicWidget::showPeakArea(Peak* peak)";

	if (peak == NULL)
		return;
	if (peak->hasEIC() == false)
		return;

	float rtWidth = peak->rtmax - peak->rtmin;

	//make sure that this is not a dead pointer to lost eic
	bool matched = false;
	EIC* eic = peak->getEIC();
	for (int i = 0; i < eicParameters->eics.size(); i++)
		if (eicParameters->eics[i] == eic) {
			matched = true;
			break;
		}
	if (!matched)
		return;

	//get points around the peak.
	vector<mzPoint> observed = eic->getIntensityVector(*peak);
	if (observed.size() == 0)
		return;

	EicLine* line = new EicLine(0, scene());
	line->setClosePath(true);
	for (int i = 0; i < observed.size(); i++) {
		line->addPoint(QPointF(toX(observed[i].x), toY(observed[i].y)));
	}
	QColor color = Qt::black;
	line->setColor(color);
	QBrush brush(color, Qt::CrossPattern);
	line->setBrush(brush);

	//QPen pen(Qt::black,Qt::SolidLine);
	//pen.setWidth(3);
	//line->setPen(pen);
}

void EicWidget::setupColors() {
	//qDebug <<" EicWidget::setupColors()";
	for (unsigned int i = 0; i < eicParameters->eics.size(); i++) {
		EIC* eic = eicParameters->eics[i];
		if (eic == NULL)
			continue;
		if (eic->sample != NULL) {
			eic->color[0] = eic->sample->color[0];
			eic->color[1] = eic->sample->color[1];
			eic->color[2] = eic->sample->color[2];
			eic->color[3] = eic->sample->color[3];
		} else {
			eic->color[0] = 0.6;
			eic->color[1] = 0.1;
			eic->color[2] = 0.1;
			eic->color[3] = 0;
		}
	}
}

void EicWidget::clearPlot() {
	//qDebug <<" EicWidget::clearPlot()";
	if (_isotopeplot && _isotopeplot->scene()) {
		_isotopeplot->clear();
		scene()->removeItem(_isotopeplot);
	}
	if (_barplot && _barplot->scene()) {
		_barplot->clear();
		scene()->removeItem(_barplot);
	}
	if (_boxplot && _boxplot->scene()) {
		_boxplot->clear();
		scene()->removeItem(_boxplot);
	}
	if (_focusLine && _focusLine->scene()) {
		scene()->removeItem(_focusLine);
	}
	if (_selectionLine && _selectionLine->scene()) {
		scene()->removeItem(_selectionLine);
	}
	if (_statusText && _statusText->scene()) {
		scene()->removeItem(_statusText);
	}
	scene()->clear();
	scene()->setSceneRect(10, 10, this->width() - 10, this->height() - 10);
}

void EicWidget::replot(PeakGroup* group) {
	//qDebug <<" EicWidget::replot(PeakGroup* group )";

	if (eicParameters->eics.size() == 0)
		return;
	if (eicParameters->_slice.rtmin <= 0 && eicParameters->_slice.rtmax <= 0)
		return;

	findPlotBounds(); //plot xmin and xmax etc..
	setupColors();	  //associate color with each EIC

	//qDebug << "EicWidget::replot() " << " group=" << group << " mz: " << _slice.mzmin << "-" << _slice.mzmax << " rt: " << _slice.rtmin << "-" << _slice.rtmax;
	clearPlot();
	
	setSelectedGroup(group);
	setTitle();
	addEICLines(_showSpline, _showEIC);
	showAllPeaks();

	if (group && group->compound != NULL && group->compound->expectedRt > 0)
			_focusLineRt = group->compound->expectedRt;
	else _focusLineRt = 0;

    if(_showCubicSpline) addCubicSpline();
    if(_showBaseline)  addBaseLine();   //qDebug() << "\tshowBaseLine msec=" << timerX.elapsed();
	if(_showTicLine or _showBicLine)   addTicLine();    //qDebug() << "\tshowTic msec=" << timerX.elapsed();
    if(_showMergedEIC) addMergedEIC();
    if(_focusLineRt >0) setFocusLine(_focusLineRt);  //qDebug() << "\tsetFocusLine msec=" << timerX.elapsed();
    if(_showNotes)	 getNotes(eicParameters->_slice.mzmin,eicParameters->_slice.mzmax);	//get notes that fall withing this mzrange
    if(_showMS2Events && eicParameters->_slice.mz>0) { addMS2Events(eicParameters->_slice.mzmin, eicParameters->_slice.mzmax); }

	addAxes();

	//setStatusText("Unknown Expected Retention Time!");

	getMainWindow()->addToHistory(eicParameters->_slice);
	scene()->update();

	//qDebug << "\t Number of eics " << eics.size();
	//qDebug << "\t Number of peakgroups " << peakgroups.size();
	//qDebug << "\t Number of graphic objects " << scene()->items().size();
	//qDebug << "\t BSP Depth " << scene()->bspTreeDepth();
	//qDebug << "\t EicWidget::replot() done ";
}

void EicWidget::setTitle() {
	//qDebug <<" EicWidget::setTitle()";
    QFont font = QApplication::font();
	int pxSize = scene()->height() * 0.03;
	if (pxSize < 14)
		pxSize = 14;
	if (pxSize > 20)
		pxSize = 20;
	font.setPixelSize(pxSize);

	QString tagString;

	if (eicParameters->selectedGroup != NULL) {
		tagString = QString(eicParameters->selectedGroup->getName().c_str());
	} else if (eicParameters->_slice.compound != NULL) {
		tagString = QString(eicParameters->_slice.compound->name.c_str());
	} else if (!eicParameters->_slice.srmId.empty()) {
		tagString = QString(eicParameters->_slice.srmId.c_str());
	}

	QString titleText = tr("<b>%1</b> m/z: %2-%3").arg(tagString,
			QString::number(eicParameters->_slice.mzmin, 'f', 4),
			QString::number(eicParameters->_slice.mzmax, 'f', 4));

	QGraphicsTextItem* title = scene()->addText(titleText, font);
	title->setHtml(titleText);
	int titleWith = title->boundingRect().width();
	title->setPos(scene()->width() / 2 - titleWith / 2, 5);
	title->update();

	//TODO: Sahil, added this whole thing while merging eicwidget
    bool hasData=false;
    for(int i=0;  i < eicParameters->eics.size(); i++ ) { if(eicParameters->eics[i]->size() > 0) { hasData=true; break; } }
    if (hasData == false ) {
        font.setPixelSize(pxSize*3);
        QGraphicsTextItem* text = scene()->addText("EMPTY EIC", font);
        int textWith = text->boundingRect().width();
        text->setPos(scene()->width()/2-textWith/2, scene()->height()/2);
        text->setDefaultTextColor(QColor(200,200,200));
        text->update();
    }
}

void EicWidget::recompute() {
	//qDebug <<" EicWidget::recompute()";
	cleanup(); //more clean up
	computeEICs();	//retrive eics
	eicParameters->selectedGroup = NULL;

}

void EicWidget::wheelEvent(QWheelEvent *event) {
	//qDebug <<" EicWidget::wheelEvent(QWheelEvent *event)";

	if (_barplot != NULL && _barplot->isSelected()) {
		QGraphicsView::wheelEvent(event);
	}
	// eicParameters->selectedGroup = NULL;
	// if (eicParameters->getSelectedGroup()) {
	// 	event->delta() > 0 ? _zoomFactor *= 2 : _zoomFactor /= 2;
	// 	zoom(_zoomFactor);
	// 	return;
	// }

	float scale = 1;
	event->delta() > 0 ? scale = 1.2 : scale = 0.9;
	eicParameters->_slice.rtmin *= scale;
	eicParameters->_slice.rtmax /= scale;
	mzSlice bounds = visibleSamplesBounds();
	if (eicParameters->_slice.rtmin > eicParameters->_slice.rtmax)
		swap(eicParameters->_slice.rtmin, eicParameters->_slice.rtmax);
	if (eicParameters->_slice.rtmin < bounds.rtmin)
		eicParameters->_slice.rtmin = bounds.rtmin;
	if (eicParameters->_slice.rtmax > bounds.rtmax)
		eicParameters->_slice.rtmax = bounds.rtmax;
	// qDebug() << "EicWidget::wheelEvent() " << _slice.rtmin << " " << _slice.rtmax << endl;
	replot(eicParameters->getSelectedGroup());
	if(eicParameters->getSelectedGroup()) addPeakPositions(eicParameters->getSelectedGroup());
}

void EicWidget::addFocusLine(PeakGroup* group) {
	//qDebug <<" EicWidget::addFocusLine(PeakGroup* group)";
	//focus line
	if (group == NULL)
		return;

	if (group->compound != NULL and group->compound->expectedRt > 0)
		_focusLineRt = group->compound->expectedRt;
	else _focusLineRt = 0;

	if (group->peaks.size() > 0) {
		Peak& selPeak = group->peaks[0];
		for (int i = 1; i < group->peaks.size(); i++) {
			if (group->peaks[i].peakIntensity > selPeak.peakIntensity) {
				selPeak = group->peaks[i];
			}
		}
		Scan* scan = selPeak.getScan();

		if (getMainWindow()->spectraWidget
				&& getMainWindow()->spectraWidget->isVisible()) {
			getMainWindow()->spectraWidget->setScan(scan, selPeak.peakMz - 5,
					selPeak.peakMz + 5);
		}
	}
	return;
}

void EicWidget::addAxes() {
	//qDebug <<" EicWidget::addAxes()";
//	 qDebug() << "EicWidget: addAxes() " << _minY << " " << _maxY << endl;
	Axes* x = new Axes(0, _minX, _maxX, 10);
	Axes* y = new Axes(1, _minY, _maxY, 10);
	scene()->addItem(x);
	scene()->addItem(y);
	y->setOffset(20);
	y->showTicLines(true);
	x->setZValue(0);
	y->setZValue(0);
	return;
}

void EicWidget::setBarplotPosition(PeakGroup* group) {

	if(!group || !_barplot) return;
	vector<mzSample*> samples = getMainWindow()->getVisibleSamples();
	if(samples.size()==0) return;

	int bwidth = _barplot->boundingRect().width();
	int bheight = _barplot->boundingRect().height();
	int legendShift = _barplot->intensityTextShift();

	int xpos_right = scene()->width() * 0.95 - bwidth;
	int xpos_left = scene()->width() * 0.10;
	int ypos = scene()->height() * 0.10;

	int count_right = 0, count_left = 0;

	for(int i = 0; i < group->peaks.size(); i++) {
		Peak& peak = group->peaks[i];
		int x = toX(peak.rt);
		int y = toY(peak.peakIntensity);
		if(x >= xpos_right-legendShift-5 && x <= xpos_right+bwidth+5 && y <= ypos+bheight) count_right++;
		if(x >= xpos_left-legendShift-5 && x <= xpos_left+bwidth+5 && y <= ypos+bheight) count_left++;
	}

	if(count_right == 0) {
		_barplot->setPos(xpos_right, ypos);
		return;
	}

	if(count_left == 0) {
		_barplot->setPos(xpos_left, ypos);
		return;
	}

	if(count_right <= count_left) _barplot->setPos(xpos_right, ypos);
	else _barplot->setPos(xpos_left, ypos);

}

void EicWidget::addBarPlot(PeakGroup* group) {
	//qDebug <<" EicWidget::addBarPlot(PeakGroup* group )";
	if (group == NULL)
		return;
	if (_barplot == NULL)
		_barplot = new BarPlot(NULL, 0);
	if (_barplot->scene() != scene())
		scene()->addItem(_barplot);

	_barplot->setMainWindow(getMainWindow());
	_barplot->setPeakGroup(group);

	// int bwidth = _barplot->boundingRect().width();
	// int bheight = _barplot->boundingRect().height();
	// int xpos = scene()->width() * 0.95 - bwidth;
	// int ypos = scene()->height() * 0.10;
	// _barplot->setPos(xpos, ypos);
	setBarplotPosition(group);
	_barplot->setZValue(1000);

	float medianRt = group->medianRt();
	if (group->parent)
		medianRt = group->parent->medianRt();

	if (medianRt && group->parent) {
		float rt = toX(medianRt);
		float y1 = toY(_minY);
		float y2 = toY(_maxY);

		QPen pen2(Qt::blue, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
		QGraphicsLineItem* focusLine = new QGraphicsLineItem(0);
		focusLine->setPen(pen2);
		focusLine->setLine(rt, y1, rt, y2);

		/*
		 float x1 = toX(group->parent->minRt);
		 float x2 = toX(group->parent->maxRt);
		 QColor color = QColor::fromRgbF( 0.2, 0, 0 , 0.1 );
		 QBrush brush(color);
		 QPen pen(color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
		 scene()->addRect(QRectF(x1,y1,x2-x1,y2-y1),pen,brush);
		 */
	}
	return;
}

void EicWidget::addIsotopicPlot(PeakGroup* group) {
	//qDebug <<" EicWidget::addIsotopicPlot(PeakGroup* group)";
	if (group == NULL)
		return;
	if (_isotopeplot == NULL)
		_isotopeplot = new IsotopePlot(0, scene());
	if (_isotopeplot->scene() != scene())
		scene()->addItem(_isotopeplot);
	_isotopeplot->hide();

	if (group->childCountBarPlot() == 0)
		return;

	vector<mzSample*> samples = getMainWindow()->getVisibleSamples();
	if (samples.size() == 0)
		return;

	_isotopeplot->setPos(scene()->width() * 0.10, scene()->height() * 0.10);
	_isotopeplot->setZValue(1000);
	_isotopeplot->setMainWindow(getMainWindow());
	_isotopeplot->setPeakGroup(group);
	_isotopeplot->show();
	return;
}

void EicWidget::addBoxPlot(PeakGroup* group) {
	//qDebug <<" EicWidget::addBoxPlot(PeakGroup* group)";
	if (group == NULL)
		return;
	if (_boxplot == NULL)
		_boxplot = new BoxPlot(NULL, scene());
	if (_boxplot->scene() != scene())
		scene()->addItem(_boxplot);

	_boxplot->setMainWindow(getMainWindow());
	_boxplot->setPeakGroup(group);

	int bwidth = _boxplot->boundingRect().width();
	int bheight = _boxplot->boundingRect().height();

	int xpos = scene()->width() * 0.95 - bwidth;
	int ypos = scene()->height() * 0.10;

	_boxplot->setPos(xpos, ypos);
	_boxplot->setZValue(1000);
	//_boxplot->setPos(scene()->width()*0.20,scene()->height()*0.10);
	//_boxplot->setZValue(1000);

	return;
}

void EicWidget::addFitLine(PeakGroup* group) {
	//qDebug <<"EicWidget::addFitLine(PeakGroup* group) ";
	if (group == NULL)
		return;
	vector<mzSample*> samples = getMainWindow()->getVisibleSamples();
	int steps = 50;
	for (int i = 0; i < group->peakCount(); i++) {
		Peak& p = group->peaks[i];
		if (p.hasEIC() == false)
			return;

		float rtWidth = p.rtmax - p.rtmin;
		float rtStep = rtWidth / steps;
		float s = 1.00;

		EIC* eic = p.getEIC();
		vector<mzPoint> observed = eic->getIntensityVector(p);
		if (observed.size() == 0)
			return;

		//find max point and total intensity
		float sum = 0;
		float maxpoint = 0;
		float max = observed[0].y;
		for (int i = 0; i < observed.size(); i++) {
			sum += observed[i].y;
			if (observed[i].y > max) {
				max = observed[i].y;
				maxpoint = i;
			}
		}
		if (sum == 0)
			return;
		/*
		 //normalize
		 for(int i=0; i < observed.size(); i++ ) { observed.y[i] /= sum; }

		 Line* line = new Line(0,scene());
		 for(int i=0; i < observed.size(); i++ ) {
		 line->addPoint(QPointF( toX(observed[i].x), toY(observed[i].y)));
		 }

		 QBrush brush(Qt::NoBrush);
		 QPen   pen(Qt::black); pen.setWidth(3);
		 line->setBrush(brush);
		 line->setPen(pen);
		 */

		//samples
		vector<float> x(steps);
		vector<float> y(steps);

		for (int i = 0; i < steps; i++) {
			x[i] = p.rtmin + (float) i / steps * rtWidth;
			y[i] = mzUtils::pertPDF(x[i], p.rtmin, p.rt, p.rtmax)
					* p.peakIntensity / 3;
			if (y[i] < 1e-3)
				y[i] = 0;
			qDebug() << x[i] << " " << y[i] << endl;
		}

		//for(int i=0; i < x.size(); i++ ) x[i]=p.rtmin+(i*rtStep);
		EicLine* line = new EicLine(0, scene());
		for (int i = 0; i < y.size(); i++) {
			line->addPoint(QPointF(toX(x[i]), toY(y[i])));
		}

		QBrush brush(Qt::NoBrush);
		QPen pen(Qt::black);
		line->setBrush(brush);
		line->setPen(pen);
		//line->fixEnds();
	}
	return;
}

void EicWidget::showAllPeaks() {
	//qDebug <<"EicWidget::showAllPeaks() ";
	for (int i = 0; i < eicParameters->peakgroups.size(); i++) {
		PeakGroup& group = eicParameters->peakgroups[i];
		addPeakPositions(&group);
	}
}

void EicWidget::addPeakPositions() {
	if (eicParameters->getSelectedGroup()) {
		addPeakPositions(eicParameters->getSelectedGroup());
	}
}

void EicWidget::addPeakPositions(PeakGroup* group) {
	////qDebug <<"EicWidget::addPeakPositions(PeakGroup* group) ";
	if (_showPeaks == false)
		return;

	bool setZValue = false;
	if (eicParameters->selectedGroup && group == eicParameters->selectedGroup) {
		setZValue = true;
	}
	sort(group->peaks.begin(), group->peaks.end(), Peak::compIntensity);

	for (unsigned int i = 0; i < group->peaks.size(); i++) {
		Peak& peak = group->peaks[i];

		if (peak.getSample() != NULL && peak.getSample()->isSelected == false)
			continue;
		if (eicParameters->_slice.rtmin != 0 && eicParameters->_slice.rtmax != 0
				&& (peak.rt < eicParameters->_slice.rtmin
						|| peak.rt > eicParameters->_slice.rtmax))
			continue;

		QColor color = Qt::black;
		if (peak.getSample() != NULL) {
			mzSample* s = peak.getSample();
			color = QColor::fromRgbF(s->color[0], s->color[1], s->color[2],
					s->color[3]);
		}

		QPen pen(color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
		QBrush brush(color);
		brush.setStyle(Qt::NoBrush);

		EicPoint* p = new EicPoint(toX(peak.rt), toY(peak.peakIntensity), &peak,
				getMainWindow());
		if (setZValue)
			p->setZValue(i);
		p->setColor(color);
		p->setBrush(brush);
		p->setPen(pen);
	 	p->setPeakGroup(group);
		//connect(p,SIGNAL(addNote(Peak*)),this,SLOT(addNote(Peak*)));
		//scene()->addItem(NULL);
		scene()->addItem(p);
	}
}

void EicWidget::resetZoom() {
	//qDebug <<"EicWidget::resetZoom() ";
	mzSlice bounds(0, 0, 0, 0);

	bool hasData = false;
	for (int i = 0; i < eicParameters->eics.size(); i++)
		if (eicParameters->eics[i]->size() > 0) hasData = true;

	if (hasData) {
		bounds = eicParameters->visibleEICBounds();
	} else if (getMainWindow()->sampleCount() > 0) {
		vector<mzSample*> samples = getMainWindow()->getVisibleSamples();
		bounds = visibleSamplesBounds();
	}

	eicParameters->_slice.rtmin = bounds.rtmin;
	eicParameters->_slice.rtmax = bounds.rtmax;
	//qDebug() << "EicWidget::resetZoom() " << _slice.rtmin << " " << _slice.rtmax << endl;
	replot(NULL);
}

void EicWidget::zoom(float factor) {
	//qDebug <<"EicWidget::zoom(float factor) ";
	_zoomFactor = factor;
	if (_zoomFactor < 0.1)
		_zoomFactor = 0.1;
	if (_zoomFactor > 20)
		_zoomFactor = 20;

	if (eicParameters->getSelectedGroup()) {
		setPeakGroup(eicParameters->getSelectedGroup());
		replot(eicParameters->getSelectedGroup());
	}
}

MainWindow* EicWidget::getMainWindow() {
// //qDebug <<"EicWidget::getMainWindow() ";
	return (MainWindow*) parent; //NG: typecast is redundant
}

void EicWidget::setRtWindow(float rtmin, float rtmax) {
	//qDebug <<"EicWidget::setRtWindow(float rtmin, float rtmax ) ";
	eicParameters->_slice.rtmin = rtmin;
	eicParameters->_slice.rtmax = rtmax;
}

void EicWidget::setSrmId(string srmId) {
	//qDebug <<"EicWidget::setSrmId(string srmId) ";
	//qDebug << "EicWidget::setSrmId" <<  srmId.c_str();
	eicParameters->_slice.compound = NULL;
	eicParameters->_slice.srmId = srmId;
	recompute();
	resetZoom();
	replot();
}

PeakGroup* EicWidget::setCompound(Compound* c) {
	//qDebug << "EicWidget::setCompound()";
	//benchmark
//	timespec tS;
	//timespec tE;
	//clock_gettime(CLOCK_REALTIME, &tS);

//	for(int i =0; i < 100000; i++ ) { findPlotBounds(); }

	if (c == NULL)
		return NULL;
	if (getMainWindow()->sampleCount() == 0)
		return NULL;

	vector<mzSample*> samples = getMainWindow()->getVisibleSamples();
	if (samples.size() == 0)
		return NULL;

	int ionizationMode = samples[0]->getPolarity();
	ionizationMode = getMainWindow()->mavenParameters->ionizationMode; //user specified ionization mode

	MassCutoff* massCutoff=getMainWindow()->getUserMassCutoff(); 
	float mz = 0;

	if (!c->formula.empty()) {
		int charge = getMainWindow()->mavenParameters->getCharge(c);
		mz = c->adjustedMass(charge);
	} else {
		mz = c->mass;
	}

	//else {
	//    MassCalculator mcalc;
	//    mz =mcalc.adjustMass(c->mass,ionizationMode);
	//}
	cerr<<"massCutoffValue   eicWidget\n";
	float minmz = mz -massCutoff->massCutoffValue(mz);
	float maxmz = mz + massCutoff->massCutoffValue(mz);
	float rtmin = eicParameters->_slice.rtmin;
	float rtmax = eicParameters->_slice.rtmax;

	if (_autoZoom) {
		if (c->expectedRt > 0) {
			rtmin = c->expectedRt - 2;
			rtmax = c->expectedRt + 2;
		}
	}
	//clock_gettime(CLOCK_REALTIME, &tE);
	//qDebug() << "Time taken" << (tE.tv_sec-tS.tv_sec)*1000 + (tE.tv_nsec - tS.tv_nsec)/1e6;

	mzSlice slice(minmz, maxmz, rtmin, rtmax);
	slice.compound = c;
	if (!c->srmId.empty())
		slice.srmId = c->srmId;
	setMzSlice(slice);

	//clock_gettime(CLOCK_REALTIME, &tE);
	//qDebug() << "Time taken" << (tE.tv_sec-tS.tv_sec)*1000 + (tE.tv_nsec - tS.tv_nsec)/1e6;

	for (int i = 0; i < eicParameters->peakgroups.size(); i++)
		eicParameters->peakgroups[i].compound = c;
	if (c->expectedRt > 0) {
		setFocusLine(c->expectedRt);
		selectGroupNearRt(c->expectedRt);
	}
	else {
		//remove previous focusline
		if (_focusLine && _focusLine->scene())
			scene()->removeItem(_focusLine);
		getMainWindow()->mavenParameters->setPeakGroup(NULL);
		resetZoom();
	}
	return eicParameters->selectedGroup;
	//clock_gettime(CLOCK_REALTIME, &tE);
	// qDebug() << "Time taken" << (tE.tv_sec-tS.tv_sec)*1000 + (tE.tv_nsec - tS.tv_nsec)/1e6;
}

void EicWidget::setMzSlice(const mzSlice& slice) {
	//qDebug << "EicWidget::setmzSlice()";
	if (slice.mzmin != eicParameters->_slice.mzmin
			|| slice.mzmax != eicParameters->_slice.mzmax
			|| slice.srmId != eicParameters->_slice.srmId
			|| slice.compound != eicParameters->_slice.compound) {
		eicParameters->_slice = slice;
		if (slice.compound) {
			if (slice.compound->precursorMz != 0
					&& slice.compound->productMz != 0) {
				eicParameters->_slice.mzmin = slice.compound->precursorMz;
				eicParameters->_slice.mzmax = slice.compound->productMz;
				eicParameters->_slice.mz = slice.compound->precursorMz;
				eicParameters->_slice.compound = slice.compound;
				eicParameters->_slice.srmId = slice.srmId;
			}
		} else if (slice.mzmin != 0 && slice.mzmax != 0) {
					eicParameters->_slice.mzmin = slice.mzmin;
					eicParameters->_slice.mzmax = slice.mzmax;
					eicParameters->_slice.mz = slice.mz;
				}

		recompute();
	} else {
		eicParameters->_slice = slice;
	}
	replot(NULL);
}

void EicWidget::setMzRtWindow(float mzmin, float mzmax, float rtmin,
		float rtmax) {
	mzSlice slice(mzmin, mzmax, rtmin, rtmax);
	setMzSlice(slice);
}

void EicWidget::setPeakGroup(PeakGroup* group) {

	if (group == NULL) return;

	int charge = getMainWindow()->mavenParameters->getCharge(group->compound);
	if (group->getExpectedMz(charge) != -1) {
		eicParameters->_slice.mz = group->getExpectedMz(charge);
	} else {
		eicParameters->_slice.mz = group->meanMz;
	}

	eicParameters->_slice.compound = group->compound;
	eicParameters->_slice.srmId = group->srmId;

	if (!group->srmId.empty()) {
		setSrmId(group->srmId);
	} else if (group->compound) {
		setCompound(group->compound);
	}

	if (_autoZoom && group->parent != NULL) {
		eicParameters->_slice.rtmin = group->parent->minRt - 2 * _zoomFactor;
		eicParameters->_slice.rtmax = group->parent->maxRt + 2 * _zoomFactor;
	} else if (_autoZoom) {
		eicParameters->_slice.rtmin = group->minRt - 2 * _zoomFactor;
		eicParameters->_slice.rtmax = group->maxRt + 2 * _zoomFactor;
	}

	//make sure that plot region is within visible samPle bounds;
	mzSlice bounds = eicParameters->visibleEICBounds();
	if (eicParameters->_slice.rtmin < bounds.rtmin)
		eicParameters->_slice.rtmin = bounds.rtmin;
	if (eicParameters->_slice.rtmax > bounds.rtmax)
		eicParameters->_slice.rtmax = bounds.rtmax;
	charge = getMainWindow()->mavenParameters->getCharge(group->compound);

	if (group->getExpectedMz(charge) != -1) {
		eicParameters->_slice.mz = group->getExpectedMz(charge);
	} else {
		eicParameters->_slice.mz = group->meanMz;
	}

	// if (group->minMz != eicParameters->_slice.mzmin
	// 		|| group->maxMz != eicParameters->_slice.mzmax) {
	MassCutoff* massCutoff=getMainWindow()->getUserMassCutoff();
	eicParameters->_slice.mzmin = eicParameters->_slice.mz - massCutoff->massCutoffValue(eicParameters->_slice.mz);
	eicParameters->_slice.mzmax = eicParameters->_slice.mz + massCutoff->massCutoffValue(eicParameters->_slice.mz);
	recompute();
	// }

	if (group->compound)
		for (int i = 0; i < eicParameters->peakgroups.size(); i++)
			eicParameters->peakgroups[i].compound = group->compound;
	if (eicParameters->_slice.srmId.length())
		for (int i = 0; i < eicParameters->peakgroups.size(); i++)
			eicParameters->peakgroups[i].srmId = eicParameters->_slice.srmId;

	replot(group);
	addPeakPositions(group);
}

void EicWidget::setMassCutoff(MassCutoff *massCutoff) {
	//qDebug <<"EicWidget::setMassCutoff(double massCutoff) ";
	mzSlice x = eicParameters->_slice;
	if (x.mz <= 0)
		x.mz = x.mzmin + (x.mzmax - x.mzmin) / 2.0;
	x.mzmin = x.mz - massCutoff->massCutoffValue(x.mz);
	x.mzmax = x.mz + massCutoff->massCutoffValue(x.mz);
	setMzSlice(x);
}

void EicWidget::setMzSlice(float mz1, float mz2) {

	MassCutoff *massCutoff = getMainWindow()->getUserMassCutoff();
	mzSlice x = eicParameters->setMzSlice(mz1, massCutoff, mz2);
	setMzSlice(x);
}

void EicWidget::groupPeaks() {
	//qDebug() << "EicWidget::groupPeaks() " << endl;
	//delete previous set of pointers to groups
	QSettings *settings = getMainWindow()->getSettings();
    int eic_smoothingWindow = getMainWindow()->mavenParameters->eic_smoothingWindow;
    float grouping_maxRtWindow = getMainWindow()->mavenParameters->grouping_maxRtWindow;

	eicParameters->groupPeaks(eic_smoothingWindow,
								grouping_maxRtWindow,
                                getMainWindow()->mavenParameters->minQuality,
								getMainWindow()->mavenParameters->distXWeight,
								getMainWindow()->mavenParameters->distYWeight,
								getMainWindow()->mavenParameters->overlapWeight,
								getMainWindow()->mavenParameters->useOverlap,
								getMainWindow()->mavenParameters->minSignalBaselineDifference);


}

void EicWidget::print(QPaintDevice* printer) {
	//qDebug <<"EicWidget::print(QPaintDevice* printer) ";
	QPainter painter(printer);

	if (!painter.begin(printer)) { // failed to open file
		qWarning("failed to open file, is it writable?");
		return;
	}
	render(&painter);
}

void EicWidget::addNote() {
	//qDebug <<"EicWidget::addNote() ";
	QPointF pos = _lastClickPos;
	float rt = invX(pos.x());
	float intensity = invY(pos.y());
	QString text;
	addNote(rt, intensity, text);
}

void EicWidget::addNote(Peak* peak) {
	//qDebug <<"EicWidget::addNote(Peak* peak) ";
	QString text;
	addNote(peak->rt, peak->peakIntensity, text);
}

void EicWidget::addNote(float rt, float intensity, QString text) {
	//qDebug <<"EicWidget::addNote(float rt, float intensity, QString text) ";

	if (text.isEmpty()) {
		bool ok;
		text = QInputDialog::getText(this, tr("Add Note"), tr("Note:"),
				QLineEdit::Normal, "Your note", &ok);
		if (!ok)
			return;
	}

	QSettings* settings = getMainWindow()->getSettings();
	QString link = settings->value("data_server_url").toString();

	if (!text.isEmpty() && !link.isEmpty()) {
		QUrl url(link);
		QUrlQuery query;

		query.addQueryItem("action", "addnote");

		if (eicParameters->_slice.compound) {
			if (!eicParameters->_slice.compound->name.empty())
				query.addQueryItem("compound_name",
						QString(eicParameters->_slice.compound->name.c_str()));

			if (!eicParameters->_slice.compound->id.empty())
				query.addQueryItem("compound_id",
						QString(eicParameters->_slice.compound->id.c_str()));
		}

		if (!eicParameters->_slice.srmId.empty())
			query.addQueryItem("srm_id",
					QString(eicParameters->_slice.srmId.c_str()));
		query.addQueryItem("mz",
				QString::number(
						eicParameters->_slice.mzmin
								+ (eicParameters->_slice.mzmax
										- eicParameters->_slice.mzmin) / 2, 'f',
						6));
		query.addQueryItem("mzmin",
				QString::number(eicParameters->_slice.mzmin, 'f', 6));
		query.addQueryItem("mzmax",
				QString::number(eicParameters->_slice.mzmax, 'f', 6));
		query.addQueryItem("rt", QString::number(rt, 'f', 6));
		query.addQueryItem("intensity", QString::number(intensity, 'f', 2));
		query.addQueryItem("title", text);
		url.setQuery(query);
		QDesktopServices::openUrl(url);

	}
	getNotes(eicParameters->_slice.mzmin, eicParameters->_slice.mzmax);
}

void EicWidget::updateNote(Note* note) {
	//qDebug <<"EicWidget::updateNote(Note* note) ";
	if (note == NULL)
		return;
	//getMainWindow()->notesDockWidgeth->updateNote(Note* note);
}

void EicWidget::getNotes(float mzmin, float mzmax) {
	//qDebug <<"EicWidget::getNotes(float mzmin, float mzmax) ";

	QSettings* settings = getMainWindow()->getSettings();

	if (getMainWindow()->notesDockWidget->isVisible() == false)
		return;
	QList<UserNote*> notes = getMainWindow()->notesDockWidget->getNotes(mzmin,
			mzmax);
	Q_FOREACH( UserNote* usernote, notes ){

	float xpos = toX(usernote->rt);
	float ypos = toY(usernote->intensity);
	if ( ypos < 10 ) ypos=10;
	if ( ypos > scene()->height()) ypos=scene()->height()+10;
	if ( xpos < 10 ) xpos=10;
	if ( xpos > scene()->width()) xpos=scene()->width()-10;

	Note* note = new Note(usernote);

	if ( settings->contains("data_server_url"))
	note->setRemoteNoteLink(usernote,settings->value("data_server_url").toString());

	note->setStyle(Note::showNoteIcon);
	scene()->addItem(note);
	note->setPos(xpos,ypos);
}
}

void EicWidget::contextMenuEvent(QContextMenuEvent * event) {
	//qDebug <<"EicWidget::contextMenuEvent(QContextMenuEvent * event) ";

	event->ignore();
	QMenu menu;
	QMenu options("Options");

	SettingsForm* settingsForm = getMainWindow()->settingsForm;

	QAction* d = menu.addAction("Peak Grouping Options");
	connect(d, SIGNAL(triggered()), settingsForm, SLOT(showPeakDetectionTab()));
	connect(d, SIGNAL(triggered()), settingsForm, SLOT(show()));

	QAction* a = menu.addAction("Add a Note");
	connect(a, SIGNAL(triggered()), SLOT(addNote()));

	QAction* b = menu.addAction("Recalculate EICs");
	connect(b, SIGNAL(triggered()), SLOT(replotForced()));

	QAction* c = menu.addAction("Copy EIC(s) to Clipboard");
	connect(c, SIGNAL(triggered()), SLOT(eicToClipboard()));

	menu.addMenu(&options);

	QAction* o4 = options.addAction("Show Peaks");
	o4->setCheckable(true);
	o4->setChecked(_showPeaks);
	connect(o4, SIGNAL(toggled(bool)), SLOT(showPeaks(bool)));
	connect(o4, SIGNAL(toggled(bool)), SLOT(replot()));


	//TODO: Sahil, added this action while merging eicwidget
    QAction* o44 = options.addAction("Group Peaks Automatically");
    o44->setCheckable(true);
    o44->setChecked(_groupPeaks);
    connect(o44, SIGNAL(toggled(bool)), SLOT(automaticPeakGrouping(bool)));
    connect(o44, SIGNAL(toggled(bool)), SLOT(replot()));

	QAction* o10 = options.addAction("Show EIC");
	o10->setCheckable(true);
	o10->setChecked(_showEIC);
	connect(o10, SIGNAL(toggled(bool)), SLOT(showEIC(bool)));
	connect(o10, SIGNAL(toggled(bool)), SLOT(replot()));

	QAction* o1 = options.addAction("Show Spline");
	o1->setCheckable(true);
	o1->setChecked(_showSpline);
	connect(o1, SIGNAL(toggled(bool)), SLOT(showSpline(bool)));
	connect(o1, SIGNAL(toggled(bool)), SLOT(replot()));

	QAction* o9 = options.addAction("Show Cubic Spline");
	o9->setCheckable(true);
	o9->setChecked(_showCubicSpline);
	connect(o9, SIGNAL(toggled(bool)), SLOT(showCubicSpline(bool)));
	connect(o9, SIGNAL(toggled(bool)), SLOT(replot()));

	QAction* o2 = options.addAction("Show Baseline");
	o2->setCheckable(true);
	o2->setChecked(_showBaseline);
	connect(o2, SIGNAL(toggled(bool)), SLOT(showBaseLine(bool)));
	connect(o2, SIGNAL(toggled(bool)), SLOT(replot()));

	QAction* o3 = options.addAction("Show TIC");
	o3->setCheckable(true);
	o3->setChecked(_showTicLine);
	connect(o3, SIGNAL(toggled(bool)), SLOT(showTicLine(bool)));
	connect(o3, SIGNAL(toggled(bool)), SLOT(replot()));

	QAction* o5 = options.addAction("Show Bar Plot");
	o5->setCheckable(true);
	o5->setChecked(_showBarPlot);
	connect(o5, SIGNAL(toggled(bool)), SLOT(showBarPlot(bool)));
	connect(o5, SIGNAL(toggled(bool)), SLOT(replot()));
    
	//TODO: Sahil, added this action while merging eicwidget
    QAction* o31 = options.addAction("Show BIC");
    o31->setCheckable(true);
    o31->setChecked(_showBicLine);
    connect(o31, SIGNAL(toggled(bool)), SLOT(showBicLine(bool)));
    connect(o31, SIGNAL(toggled(bool)), SLOT(replot()));

	//TODO: Sahil, added this action while merging eicwidget
    QAction* o33 = options.addAction("Show Merged EIC");
    o33->setCheckable(true);
    o33->setChecked(_showMergedEIC);
    connect(o33, SIGNAL(toggled(bool)), SLOT(showMergedEIC(bool)));
    connect(o33, SIGNAL(toggled(bool)), SLOT(replot()));

	//TODO: Sahil, added this action while merging eicwidget
    QAction* o34 = options.addAction("Show EICs as Lines");
    o34->setCheckable(true);
    o34->setChecked(_showEICLines);
    connect(o34, SIGNAL(toggled(bool)), SLOT(showEICLines(bool)));
    connect(o34, SIGNAL(toggled(bool)), SLOT(replot()));

	QAction* o6 = options.addAction("Show Isotope Plot");
	o6->setCheckable(true);
	o6->setChecked(_showIsotopePlot);
	connect(o6, SIGNAL(toggled(bool)), SLOT(showIsotopePlot(bool)));
	connect(o6, SIGNAL(toggled(bool)), SLOT(replot()));

	QAction* o7 = options.addAction("Show Box Plot");
	o7->setCheckable(true);
	o7->setChecked(_showBoxPlot);
	connect(o7, SIGNAL(toggled(bool)), SLOT(showBoxPlot(bool)));
	connect(o7, SIGNAL(toggled(bool)), SLOT(replot()));

	//TODO: Sahil, added this action while merging eicwidget
    QAction* o8 = options.addAction("Show MS2 Events");
    o8->setCheckable(true);
    o8->setChecked(_showMS2Events);
    connect(o8, SIGNAL(toggled(bool)), SLOT(showMS2Events(bool)));
    connect(o8, SIGNAL(toggled(bool)), SLOT(replot()));

	QAction *selectedAction = menu.exec(event->globalPos());
	scene()->update();

}

/*
@author : Sahil 
*/
// TODO: Sahil Added while merging eicwidget
//This function returns the information like mz, intensity & rt
//of eics
QString EicWidget::eicToTextBuffer() {
    QString eicText;
    for(int i=0; i < eicParameters->eics.size(); i++ ) {
        EIC* e = eicParameters->eics[i];
        if (e == NULL ) continue;
        mzSample* s = e->getSample();
        if (s == NULL ) continue;

        eicText += QString(s->sampleName.c_str()) + "\n";

        for(int j=0;  j<e->size(); j++ ) {
                if (e->rt[j] >= eicParameters->_slice.rtmin && e->rt[j] <= eicParameters->_slice.rtmax ) {
                        eicText += tr("%1,%2,%3,%4,%5\n").arg(
                                QString::number(i),
                                QString::number(e->rt[j], 'f', 2),
                                QString::number(e->intensity[j], 'f', 4),
                                QString::number(e->spline[j], 'f', 4),
                                QString::number(e->mz[j], 'f', 4)
                        );
                }
        }
    }
    return eicText;
}

void EicWidget::eicToClipboard() { 
 //qDebug <<"EicWidget::eicToClipboard() "; 
	if (eicParameters->eics.size() == 0 ) return;
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText( eicToTextBuffer() );
}

void EicWidget::selectGroupNearRt(float rt) {
	if (eicParameters->peakgroups.size() == 0)
		return;

	PeakGroup* selGroup = NULL;
	selGroup = eicParameters->selectGroupNearRt(rt,
												selGroup,
												getMainWindow()->mavenParameters->deltaRtCheckFlag,
												getMainWindow()->mavenParameters->qualityWeight,
												getMainWindow()->mavenParameters->intensityWeight,
												getMainWindow()->mavenParameters->deltaRTWeight);

	if (selGroup) {
		//Sabu Iso
		setSelectedGroup(selGroup);
	}
	getMainWindow()->mavenParameters->setPeakGroup(selGroup);
}

void EicWidget::showIsotopicBarPlot(bool _showIsotopicBarPlot) {
	if (_showIsotopicBarPlot) {
		getMainWindow()->isotopePlotDockWidget->show();
		//addIsotopicPlot(group);
	} else {
		getMainWindow()->isotopePlotDockWidget->hide();
	}
}

void EicWidget::updateIsotopicBarplot(PeakGroup* group) {
	if (_frozen || group == NULL)
		return;

	if (_showIsotopePlot) {
		getMainWindow()->isotopePlotDockWidget->show();
		getMainWindow()->isotopePlotDockWidget->raise();
		addIsotopicPlot(group);
	} else {
		getMainWindow()->isotopePlotDockWidget->hide();
	}
}

void EicWidget::setSelectedGroup(PeakGroup* group) {
	//qDebug <<"EicWidget::setSelectedGroup(PeakGroup* group ) ";
	if (_frozen || group == NULL)
		return;
	if (_showBarPlot)
		addBarPlot(group);
	if (_showBoxPlot)
		addBoxPlot(group);
	//drawSelectionLine(group->minRt, group->maxRt); // TODO: Sahil commented this 
	//addFitLine(group);
	eicParameters->selectedGroup = group;
}

void EicWidget::setGallaryToEics() {
	//todo fix spelling
	if (getMainWindow()->galleryDockWidget->isVisible()) {
		getMainWindow()->galleryWidget->addIdividualEicPlots(
				eicParameters->eics, eicParameters->selectedGroup);
	}
}

void EicWidget::saveRetentionTime() {
	//qDebug <<"EicWidget::saveRetentionTime() ";
	if (!eicParameters->selectedGroup
			|| eicParameters->selectedGroup->compound == NULL)
		return;

	QPointF pos = _lastClickPos;
	float rt = invX(pos.x());
	eicParameters->selectedGroup->compound->expectedRt = rt;

	DB.saveRetentionTime(eicParameters->selectedGroup->compound, rt,
			"user_method");
}

void EicWidget::keyPressEvent(QKeyEvent *e) {
	//qDebug <<"EicWidget::keyPressEvent( QKeyEvent *e ) ";

	QGraphicsView::keyPressEvent(e);

	switch (e->key()) {
	case Qt::Key_Down:
		getMainWindow()->ligandWidget->showNext();
		break;
	case Qt::Key_Up:
		getMainWindow()->ligandWidget->showLast();
		break;
	case Qt::Key_Left:
		getMainWindow()->spectraWidget->showLastScan();
		break;
	case Qt::Key_Right:
		getMainWindow()->spectraWidget->showNextScan();
		break;
	case Qt::Key_0:
		resetZoom();
		break;
	case Qt::Key_Plus:
		zoom(_zoomFactor * 0.9);
		break;
	case Qt::Key_Minus:
		zoom(_zoomFactor * 1.1);
		break;
	case Qt::Key_C:
		copyToClipboard();
		break;
	case Qt::Key_G:
		markGroupGood();
		break;
	case Qt::Key_B:
		markGroupBad();
		break;
	case Qt::Key_F5:
		replotForced();

	default:
		break;
	}
	e->accept();
	return;
}

void EicWidget::setStatusText(QString text) {
	//qDebug <<"EicWidget::setStatusText(QString text) ";
	if (_statusText == NULL) {
		_statusText = new Note(text, 0, scene());
		_statusText->setExpanded(true);
		_statusText->setFlag(QGraphicsItem::ItemIsSelectable, false);
		_statusText->setFlag(QGraphicsItem::ItemIsFocusable, false);
		_statusText->setFlag(QGraphicsItem::ItemIsMovable, false);
		_statusText->setZValue(2000);
	}

	if (_statusText->scene() != scene())
		scene()->addItem(_statusText);
	_statusText->setHtml(text);
	_statusText->setTimeoutTime(10);

	QRectF size = _statusText->boundingRect();
	_statusText->setPos(scene()->width() - size.width() - 5,
			scene()->height() - size.height() - 5);

}

void EicWidget::markGroupGood() {
	getMainWindow()->markGroup(eicParameters->getSelectedGroup(), 'g');
	getMainWindow()->peaksMarked++;
	getMainWindow()->autoSaveSignal();
}
void EicWidget::markGroupBad() {
	getMainWindow()->markGroup(eicParameters->getSelectedGroup(), 'b');
	getMainWindow()->peaksMarked++;
	getMainWindow()->autoSaveSignal();
}
void EicWidget::copyToClipboard() {
	getMainWindow()->setClipboardToGroup(eicParameters->getSelectedGroup());
}

void EicWidget::freezeView(bool freeze) {
	if (freeze == true) {
		_frozen = true;
		_freezeTime = 100;
		_timerId = startTimer(50);
	} else {
		_frozen = false;
		_freezeTime = 0;
		killTimer(_timerId);
		_timerId = 0;
	}
}

void EicWidget::timerEvent(QTimerEvent * event) {
	//qDebug <<"EicWidget::timerEvent( QTimerEvent * event ) ";
	_freezeTime--;
	if (_freezeTime <= 0) {
		killTimer(_timerId);
		_frozen = false;
		_freezeTime = 0;
		_timerId = 0;
	}
}

/*
@author: Sahil
*/
//TODO: Sahil, Added this function while merging eicwidget
void EicWidget::addMS2Events(float mzmin, float mzmax) {

    qDebug() << "addMS2Events() " << mzmin << " " << mzmax;

    MainWindow* mw = getMainWindow();
    vector <mzSample*> samples = mw->getVisibleSamples();

    if (samples.size() <= 0 ) return;
    MassCutoff *massCutoff = mw->getUserMassCutoff();

    mw->fragPanel->clearTree();
    int count=0;
    for ( unsigned int i=0; i < samples.size(); i++ ) {
        mzSample* sample = samples[i];

        for (unsigned int j=0; j < sample->scans.size(); j++ ) {
            Scan* scan = sample->scans[j];

            if (scan->mslevel > 1 && scan->precursorMz >= mzmin && scan->precursorMz <= mzmax) {

                mw->fragPanel->addScanItem(scan);
                if (scan->rt < eicParameters->_slice.rtmin || scan->rt > eicParameters->_slice.rtmax) continue;

        		QColor color = QColor::fromRgbF( sample->color[0], sample->color[1], sample->color[2], 1 );
                EicPoint* p  = new EicPoint(toX(scan->rt), toY(10), NULL, getMainWindow());
                p->setPointShape(EicPoint::TRIANGLE_UP);
                p->forceFillColor(true);;
                p->setScan(scan);
                p->setSize(30);
                p->setColor(color);
                p->setZValue(1000);
                p->setPeakGroup(NULL);
                scene()->addItem(p);
                count++;
            }
        }
    }

    qDebug() << "addMS2Events()  found=" << count;

}
