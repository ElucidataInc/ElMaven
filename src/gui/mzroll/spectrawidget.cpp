#include "datastructures/adduct.h"
#include "line.h"
#include "Compound.h"
#include "constants.h"
#include "eiclogic.h"
#include "eicwidget.h"
#include "Fragment.h"
#include "mainwindow.h"
#include "masscalcgui.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "Peptide.hpp"
#include "plot_axes.h"
#include "Scan.h"
#include "spectramatching.h"
#include "spectrawidget.h"

SpectraWidget::SpectraWidget(MainWindow* mw, bool isFragSpectra) {
    this->mainwindow = mw;
    eicparameters = new EICLogic();
   _currentScan = nullptr;
   _avgScan = nullptr;
   _currentGroup = PeakGroup();
   _spectralHit = SpectralHit();
   _lowerLabel = nullptr;
   _upperLabel = nullptr;
   _overlayMode = isFragSpectra ? OverlayMode::Raw : OverlayMode::None;

    initPlot();

    _drawXAxis = true;
    _drawYAxis = true;
    _showOverlay = false;
    _resetZoomFlag = true;
    _profileMode = false;
    _nearestCoord = QPointF(0,0);
    _focusCoord = QPointF(0,0);
}

void SpectraWidget::initPlot()
{
    _titleText = QString();
    _zoomFactor = 0;
    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    scene()->setSceneRect(10, 10, width()-10, height()-10);

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

void SpectraWidget::setCurrentScan(Scan* scan)
{
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

void SpectraWidget::_placeLabels()
{
    if (!_currentScan || _currentScan->nobs() == 0)
        return;

    QString upperLabelText = tr("<b>Group Spectra</b>");
    QString lowerLabelText = tr("<b>Reference Spectra</b>");
    if (_overlayMode == OverlayMode::Raw)
        upperLabelText = tr("<b>Raw Spectra (Rt: %1)</b>")
                             .arg(QString::number(_currentScan->rt, 'f', 2));

    QFont font = QApplication::font();
    if (!_upperLabel) {
        _upperLabel = scene()->addText("", font);
        _upperLabel->setHtml(upperLabelText);
        _upperLabel->setDefaultTextColor(Qt::black);
        _upperLabel->update();
    } else if (_upperLabel->toHtml() != upperLabelText) {
        _upperLabel->setHtml(upperLabelText);
        _upperLabel->update();
        _upperLabel->setVisible(true);
    } else {
        _upperLabel->setVisible(true);
    }
    int upperLabelWidth = _upperLabel->boundingRect().width();
    _upperLabel->setPos(scene()->width() - upperLabelWidth, 3);

    if (!_lowerLabel) {
        _lowerLabel = scene()->addText("", font);
        _lowerLabel->setHtml(lowerLabelText);
        _lowerLabel->setDefaultTextColor(Qt::black);
        _lowerLabel->update();
    } else {
        _lowerLabel->setVisible(true);
    }
    int lowerLabelWidth = _lowerLabel->boundingRect().width();
    _lowerLabel->setPos(scene()->width() - lowerLabelWidth,
                        scene()->height() / 2);
}

void SpectraWidget::setTitle(QString titleText) 
{
    QFont font = QApplication::font();
    _title->setDefaultTextColor(Qt::black);

    font.setPointSizeF(font.pointSize()*0.8);

    if (!_title) _title = scene()->addText(titleText, font);
    _title->setHtml(titleText);
    int titleWidth = _title->boundingRect().width();
    _title->setPos(scene()->width() / 2 - titleWidth / 2, 3);
    _title->update();

    if (_currentScan && _currentScan->nobs() == 0) {
        font.setPointSizeF(font.pointSize()*0.8);
        _title->setHtml("EMPTY SCAN");
        int textWidth = _title->boundingRect().width();
        _title->setPos(scene()->width() / 2 - textWidth / 2, scene()->height() / 2);
        _title->setDefaultTextColor(QColor(200, 200, 200));
        _title->update();
    }
}

void SpectraWidget::setScan(Scan* scan)
{
    if ( scan == NULL ) return;
    setCurrentScan(scan);
    cerr << "SpectraWidget::setScan(scan) " << endl;
    findBounds(true,true);
    drawGraph();
    repaint();
}

void SpectraWidget::setScan(Scan* scan, float mzmin, float mzmax)
{
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

void SpectraWidget::setScan(mzSample* sample, int scanNum=-1)
{
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

void SpectraWidget::setScan(Peak* peak)
{
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

void SpectraWidget::overlayPeptideFragmentation(QString peptideSeq,MassCutoff *productMassCutoff)
{
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
    hit.productPPM = productMassCutoff->getMassCutoff();
    hit.precursorMz=record.monoisotopicMZ();
	hit.scan = _currentScan;
	
    vector<bool>seen(_currentScan->nobs(),false);
	for(unsigned int i=0; i < ions.size(); i++) {
		FragmentIon* ion = ions[i];
        int pos = _currentScan->findClosestHighestIntensityPos(ion->m_mz, productMassCutoff);
        if(pos != -1 and seen[pos] == false) {
            ion->m_mzDiff = abs(_currentScan->mz[pos]-ion->m_mz);
            qDebug() << "overlayPeptideFragmentation: IONS: " << ion->m_ion.c_str() << " ->" << "ionType" << " " << ion->m_mz << " mzdiff=" << ion->m_mzDiff;

            hit.mzList << _currentScan->mz[pos];
            hit.intensityList << _currentScan->intensity[pos];
            seen[pos]=true;
		}
	}

	delete_all(ions);
    overlaySpectralHit(hit);
}

void SpectraWidget::overlayPeakGroup(PeakGroup* group)
{
    _currentGroup = PeakGroup();
    if (!group) return;
    
    _overlayMode = OverlayMode::Consensus;
    float productPpmTolr = mainwindow->mavenParameters->fragmentTolerance;
    Scan* avgScan = group->getAverageFragmentationScan(productPpmTolr);
    setScan(avgScan);
    if (group->getCompound()) {
        _currentGroup.copyObj(*group);
        overlayCompoundFragmentation(group->getCompound());
        //if (!group->compound->smileString.empty()) overlayTheoreticalSpectra(group->compound);
    }
    delete(avgScan);
}

void SpectraWidget::overlayCompoundFragmentation(Compound* c)
{
    clearOverlay();
    if (!_currentScan || !c || c->fragmentMzValues().size() == 0) return;

    SpectralHit hit;
        hit.score = 0;
    //TODO: precursormz should be preset as the compound m/z
    //compound->mass should be reserved for exact mass or renamed
        if (!c->formula().empty() || c->neutralMass() != 0.0f)
        c->setPrecursorMz ( c->adjustedMass(mainwindow->mavenParameters->getCharge(c)));
    if (mzUtils::almostEqual(c->precursorMz(), 0.0f))
        c->setPrecursorMz( c->mz());
    hit.precursorMz = c->precursorMz();
    hit.matchCount = 0;
    hit.sampleName = "";
    hit.productPPM = mainwindow->mavenParameters->fragmentTolerance;
    hit.scan = nullptr;
    auto fragmentMzValues = c->fragmentMzValues();
    auto fragmentIntensities = c->fragmentIntensities();
    for (unsigned int i = 0; i < fragmentMzValues.size(); i++) {
        hit.mzList << fragmentMzValues[i];
        hit.intensityList << fragmentIntensities[i];
    }

    links.clear();

    _spectralHit = hit;

    cerr << "SpectraWidge::overlayCompoundfragmentation(Compound)" << c->name() << " " << c->precursorMz() << endl;

    if (_currentScan && _currentScan->mslevel == 2) {
        _showOverlay = true;
        overlaySpectralHit(_spectralHit);
        resetZoom();
    }
}

void SpectraWidget::overlaySpectralHit(SpectralHit& hit)
{
        _spectralHit = hit;

        //determine limits of overlayed spectra
        if(hit.scan and _currentScan != hit.scan) {
            setScan(hit.scan, hit.getMinMz()-0.5, hit.getMaxMz()+0.5);
            findBounds(false,true);
        }

        drawGraph();
        repaint();

        if (!_currentScan) return;
        MassCutoff* productMassCutoff = new MassCutoff();
        productMassCutoff->setMassCutoffAndType(hit.productPPM, "ppm");
        int pos = _currentScan->findHighestIntensityPos(hit.precursorMz, productMassCutoff);
        if (pos >= 0) {
            _focusCoord.setX(hit.precursorMz);
            _focusCoord.setY(_currentScan->intensity[pos]);
        } else {
            _focusCoord.setX(0.0f);
            _focusCoord.setY(0.0f);
        }
        delete productMassCutoff;
}

void SpectraWidget::overlayScan(Scan *scan)
{
    if (_overlayMode == OverlayMode::None || scan->mslevel != 2)
        return;

    _overlayMode = OverlayMode::Raw;
    setScan(scan);
    if (_currentGroup.getCompound())
        overlayCompoundFragmentation(_currentGroup.getCompound());
}

void SpectraWidget::showConsensusSpectra(PeakGroup* group)
{
    qDebug() << "showConsensusSpectra()";
    if (!group) return;
    _scanset.clear();

    float fragTolerance = mainwindow->mavenParameters->fragmentTolerance;
    Scan* consensus = group->getAverageFragmentationScan(fragTolerance);

    if (consensus) {
        _scanset = group->getFragmentationEvents();
        _currentScan->deepcopy(consensus);
        this->findBounds(true, true);
        this->drawGraph();
    }
}

void SpectraWidget::drawSpectralHit(SpectralHit& hit)
{
    MassCutoff *massCutoffWindow = new MassCutoff();
    massCutoffWindow->setMassCutoffAndType(hit.productPPM, "ppm");
    double maxIntensity = hit.getMaxIntensity();

    QPen redpen(Qt::red, 1);
    QPen bluepen(Qt::blue, 2);
    QPen graypen(Qt::gray, 1);

    float SCALE = 0.45;

    QGraphicsTextItem* text = new QGraphicsTextItem(hit.compoundId);
    text->setFont(_title->font());
    text->setPos(_title->pos().x(), toY(_maxY * 0.95, SCALE));
    scene()->addItem(text);
    _items.push_back(text);

    for(int i = 0; i < hit.mzList.size(); i++) {
        int pos = _currentScan->findHighestIntensityPos(hit.mzList[i], massCutoffWindow);
        double hitIntensity = 0;
        if (i < hit.intensityList.size())
            hitIntensity = hit.intensityList[i];

        int x = toX(hit.mzList[i]);
        int y = toY((hitIntensity / maxIntensity) * _maxY, SCALE);

        QGraphicsLineItem* line = new QGraphicsLineItem(x, y, x, toY(0), 0);
        pos > 0 ? line->setPen(bluepen) : line->setPen(redpen);
        scene()->addItem(line);
        _items.push_back(line);

        if (pos >= 0 && pos < _currentScan->nobs()) {
            //matched peak
            int x = toX(_currentScan->mz[pos]);
            int y = toY(_maxY * (hitIntensity / maxIntensity));
            QGraphicsLineItem* line = new QGraphicsLineItem(x, y, x, toY(0), 0);
            line->setPen(graypen);
            scene()->addItem(line);
            _items.push_back(line);
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
    scene()->update();
    delete massCutoffWindow;
}

void SpectraWidget::clearGraph() {
    qDebug() << "drawSpectra() mzrange= " << _minX << "-" << _maxX;

    //clean up previous plot
    if ( _arrow ) _arrow->setVisible(false);
    if (_upperLabel)
        _upperLabel->setVisible(false);
    if (_lowerLabel)
        _lowerLabel->setVisible(false);

    for(unsigned int i=0; i < _items.size(); i++) {
        if(_items[i] != NULL) delete(_items[i]); _items[i]=NULL;
    }
    _items.clear();
    scene()->setSceneRect(10,10,this->width()-10, this->height()-10);
}

void SpectraWidget::clearOverlay()
{
    _spectralHit = SpectralHit();
    _showOverlay = false;
}

void SpectraWidget::setScanTitle()
{
    _titleText = QString();

    if (! _currentScan) {
        setTitle(_titleText);
        return;
    }

    QString sampleName("");
    if (_currentScan->sample)  sampleName = QString(_currentScan->sample->sampleName.c_str());
    
    if (_currentScan->scannum)
        _titleText += tr("<b>Scan#</b> %1  ").arg(QString::number(_currentScan->scannum));

    if (_currentScan->rt)
        _titleText += tr("<b>Rt:</b> %1  ").arg(QString::number(_currentScan->rt, 'f', 2));

    if (_currentScan->mslevel)
        _titleText += tr("<b>MS Level:</b> %1  ").arg(QString::number(_currentScan->mslevel));

    if (_currentScan->precursorMz) {
        _titleText += tr("<b>Pre m/z:</b> %1  ").arg(QString::number(_currentScan->precursorMz,'f',4));
    }

    if (_currentScan->collisionEnergy) {
        _titleText += tr("<b>CE:</b> %1  ").arg(QString::number(_currentScan->collisionEnergy,'f',0));
    }

    if (_currentScan->productMz) {    
       _titleText += tr("<b>Prod m/z:</b> %1  ").arg(QString::number(_currentScan->productMz,'f',3));
    }

    if (_currentScan->precursorMz > 0) {
        //TODO: Use masscutoff set by user
		double purity = _currentScan->getPrecursorPurity(10.0) * 100.0;
        _titleText += tr("<b>Purity:</b> %1  ").arg(QString::number(purity,
                                                               'f',
                                                               1));
        _titleText += tr("<b>Isolation Window:</b> %1  ").arg(QString::number(_currentScan->isolationWindow,
                                                                'f',
                                                                1));
    }

    setTitle(_titleText);
    mainwindow->spectraDockWidget->setWindowTitle("Spectra: " + sampleName);
}

void SpectraWidget::setGroupTitle()
{
    _titleText = QString();

    QString compoundName("");
    if (_currentGroup.getCompound()) compoundName = QString(_currentGroup.getCompound()->name().c_str());
    
    float purity = 0;
    if (_currentGroup.fragmentationPattern.mzValues.size()) {
        purity = _currentGroup.fragmentationPattern.purity * 100;
    }

    float rt = 0;
    if (_currentGroup.fragmentationPattern.mzValues.size()) {
        //mean RT of all MS2 events in this group
        rt = _currentGroup.fragmentationPattern.rt;
    }
    else {
        //mean RT of the precursor group
        rt = _currentGroup.meanRt;
    }

    float meanMz = 0.0f;
    if (_currentGroup.meanMz)
        meanMz = _currentGroup.meanMz;
    
    _titleText += tr("<b>Rt:</b> %1  ").arg(QString::number(rt, 'f', 2));
    
    _titleText += tr("<b>Pre m/z:</b> %1  ").arg(QString::number(meanMz, 'f', 4));
    
    _titleText += tr("<b>Purity:</b> %1  ").arg(QString::number(purity, 'f', 2));

    setTitle(_titleText);
    mainwindow->fragSpectraDockWidget->setWindowTitle("Fragmentation spectra: " + compoundName);
}

void SpectraWidget::drawScan(Scan* scan, QColor sampleColor)
{
    float _focusedMz = _focusCoord.x();

    QPen slineColor(sampleColor, 2);
    EicLine* sline = new EicLine(NULL, scene());
    sline->setColor(sampleColor);
    sline->setPen(slineColor);
   _items.push_back(sline);

    if (_profileMode) {
        QBrush slineFill(sampleColor);
        sline->setFillPath(true);
        sline->setBrush(slineFill);
    } else {
        sline->setClosePath(false);
        sline->setFillPath(false);
    }

    float SCALE = 1.0;
    float OFFSET = 0;
    if (_showOverlay) {
        SCALE = 0.45;
        OFFSET = -scene()->height() / 2.0;
    }

    int yzero = toY(0, SCALE, OFFSET);
    sline->addPoint(toX(_maxX),yzero);
    sline->addPoint(toX(_minX),yzero);
    
    for (int j = 0; j < scan->nobs(); j++) {
        if ( scan->mz[j] < _minX  || scan->mz[j] > _maxX ) continue;
        int x = toX(scan->mz[j]);
        int y = toY(scan->intensity[j], SCALE, OFFSET);

        if (_profileMode) {
               sline->addPoint(x, y);
        } else {
                sline->addPoint(x, yzero);
                sline->addPoint(x, y);
        }

        if (!mzUtils::almostEqual(_focusedMz, 0.0f)
            && abs(scan->mz[j] - _focusedMz) < 0.005) {
            QPen redpen(Qt::red, 3);
            QGraphicsLineItem* line = new QGraphicsLineItem(x, y, x, yzero, 0);
            scene()->addItem(line);
            line->setPen(redpen);
            _items.push_back(line);
        }
    }
}

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
        drawScan(scanset[i],sampleColor);
     }
}

void SpectraWidget::drawMzLabels(Scan* scan)
{
    if (!scan) return;

    unsigned int labelCount = 0;
    
    QFont font = QApplication::font();
    font.setPointSizeF(font.pointSize()*0.8);
    
    float SCALE = 1.0;
    float OFFSET = 0;
    if (_showOverlay) {
        SCALE = 0.45;
        OFFSET = -scene()->height() / 2.0;
    }

    vector<int> positions = scan->intensityOrderDesc();
    float xRange = abs(_maxX - _minX);
    float precision = 2;
    if (xRange > 100) precision = 3;
    if (xRange > 50) precision = 4;
    if (xRange > 10) precision = 6;

    for(auto pos : positions) {
        if ((scan->mz[pos] < _minX) || (scan->mz[pos] > _maxX)) continue;
        if ((scan->intensity[pos] / _maxY) < 0.005 ) continue;

        //position label
        int x = toX(scan->mz[pos]);
        int y = toY(scan->intensity[pos], SCALE, OFFSET);

        //create label
        QString labelText = QString::number(scan->mz[pos], 'f', precision);
        QGraphicsTextItem* text = new QGraphicsTextItem(labelText, 0);
        text->setHtml(labelText);
        text->setFont(font);
        text->setPos(x - 2, y - 20);

        scene()->addItem(text);
        _items.push_back(text);

        Q_FOREACH(QGraphicsItem* item, scene()->collidingItems(text) ) {
            if (qgraphicsitem_cast<QGraphicsTextItem *>(item)) {
                text->hide(); break;
            }
        }

        if(++labelCount > 50 ) break;
    }
}

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

void SpectraWidget::drawGraph()
{
    clearGraph();
    
    if (_currentScan == NULL) return;

    qDebug() << "showSpectra() mzrange= " << _minX << "-" << _maxX;

    QColor sampleColor(Qt::black);
    if (_currentScan->sample) {
        mzSample* sample = _currentScan->sample;
        sampleColor = QColor::fromRgbF( sample->color[0], sample->color[1], sample->color[2], 1 );
    }

    drawScan(_currentScan, sampleColor);
    drawMzLabels(_currentScan);
    drawAnnotations();

    if (_spectralHit.mzList.size() > 0) {
        setGroupTitle();
        _placeLabels();
        if (fabs(_spectralHit.precursorMz - _currentScan->precursorMz) < 0.1f)
            drawSpectralHit(_spectralHit);
        else {
            //TODO: either remove the check or inform user on the UI in case of failure
            qDebug() << " overlaySpectra() skipped: "
                     << _spectralHit.precursorMz
                     << " "
                     << _currentScan->precursorMz;
        }
    } else
        setScanTitle();

    addAxes();
}

void SpectraWidget::findBounds(bool checkX, bool checkY)
{
    // TODO: Do we only check for existence of a scan? Having an "mzList" in a
    // `_spectralHit` should also be checked, but right now, once set
    // `_spectralHit` is never unset/cleared. What would be the best place to
    // do so.
    if (_currentScan == NULL)
        return;

    // TODO: similarly size of `_spectralHit->mzList` can also be checked here
    if (_currentScan->mz.size() == 0) {
        qDebug() << "Empty scan: " << _currentScan->scannum << endl;
        return;
    }

    // obtain the minimum and maximum m/z values of the current scan
    float minMZ = _currentScan->mz.front();
    float maxMZ = _currentScan->mz.back();

    // if spectral hit has an m/z list, then set minMZ and maxMZ to be the
    // minimum and maximum of this list, if they are smaller or greater in
    // magnitude, respectively
    if (_spectralHit.mzList.size() > 0) {
        QVector<double> mzListCopy = _spectralHit.mzList;
        qSort(mzListCopy.begin(), mzListCopy.end(), qLess<double>());
        minMZ = min(minMZ, static_cast<float>(mzListCopy.front()));
        maxMZ = max(maxMZ, static_cast<float>(mzListCopy.back()));
    }

    // buffer of ± 20 Da, to ensure that labels and bars on the edges come into
    // the picture fully
    minMZ -= 20;
    maxMZ += 20;

    cerr << _currentScan->filterLine << " " << _currentScan->nobs() << endl;
    cerr << "findBounds(): range [" << minMZ << ", " << maxMZ << "]" << endl;

    if (_minX < minMZ)
        _minX = minMZ;
    if (_maxX > maxMZ)
        _maxX = maxMZ;

    if (checkX) {
        _minX = minMZ;
        _maxX = maxMZ;
    }

    // is this condition ever going to be true?
    if (_minX == _maxX) {
        _minX -= 0.5;
        _maxX += 0.5;
    }

    if (checkY) {
        _minY = 0;
        _maxY = 1;
        for (int j = 0; j < _currentScan->nobs(); j++) {
            if (_currentScan->mz[j] >= _minX && _currentScan->mz[j] <= _maxX) {
                if (_currentScan->intensity[j] > _maxY)
                    _maxY = _currentScan->intensity[j];
            }
        }
    }

    _minY = 0;
    _maxY *= 1.3;
}

void SpectraWidget::keyPressEvent( QKeyEvent *e )
{
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

void SpectraWidget::incrementScan(int increment, int msLevel=0 )
{
    // increment only for non-overlay spectra having a scan for a sample
    if (_currentScan == NULL
        || _currentScan->sample == NULL
        || _overlayMode != OverlayMode::None) {
        return;
    }

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


void SpectraWidget::resizeEvent (QResizeEvent * event)
{
    QSize newsize = event->size();
    replot();
}

void SpectraWidget::enterEvent (QEvent *) {
    grabKeyboard();
}

void SpectraWidget::leaveEvent (QEvent*) {
    releaseKeyboard();
}

void SpectraWidget::addAxes()
{
	if (_drawXAxis) {
		Axes* x = new Axes(0, _minX, _maxX, 10);
		scene()->addItem(x);
		x->setZValue(998);
		_items.push_back(x);
	}

    if (_drawYAxis) {
    	Axes* y = new Axes(1, _minY, _maxY, 10);
    	scene()->addItem(y);
        y->setZValue(999);
        y->showTicLines(false);
		y->setOffset(5);
    	_items.push_back(y);
    }
}

float SpectraWidget::toY(float y, float scale, float offset)
{ 
    float height = scene()->height();
    return(height - (((y - _minY) / (_maxY - _minY) * height) * scale) + offset);
}

float SpectraWidget::invY(float y)
{
    float height = scene()->height();
    return  -1 * ((y - height) / scene()->height() * (_maxY - _minY) + _minY);
}

void SpectraWidget::mousePressEvent(QMouseEvent *event)
{
    if (_currentScan == nullptr)
        return;

    QGraphicsView::mousePressEvent(event);
    _mouseStartPos = event->pos();
}

void SpectraWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (_currentScan == nullptr)
        return;

    QGraphicsView::mouseReleaseEvent(event);

    _mouseEndPos = event->pos();

    int deltaX = _mouseEndPos.x() - _mouseStartPos.x();
    float deltaXfrac = (float) deltaX / (width() + 1);

    auto nearest = findNearestMz(_mouseEndPos);
    if (mainwindow->massCalcWidget->isVisible())
        mainwindow->massCalcWidget->setMass(_currentScan->mz[nearest]);

    if (deltaXfrac > 0.01) {
        float xmin = invX(std::min(_mouseStartPos.x(), _mouseEndPos.x()));
        float xmax = invX(std::max(_mouseStartPos.x(), _mouseEndPos.x()));
        _minX = xmin;
        _maxX = xmax;
    } else if (deltaXfrac < -0.01) {
		if (_currentScan->mz.size() > 0) {
			float minmz = _currentScan->mz[0];
			float maxmz = _currentScan->mz[_currentScan->nobs() - 1];
            _minX *= 0.9;
            _maxX *= 1.1;
            if (_minX < minmz ) _minX = minmz;
            if (_maxX > maxmz ) _maxX = maxmz;
        }
    } else if (_nearestCoord.x() > 0) {
        setMzFocus(_nearestCoord.x());
    }
    findBounds(false, true);
    replot();
}

void SpectraWidget::setMzFocus(Peak* peak)
{
    setMzFocus(peak->peakMz);
}

void SpectraWidget::setMzFocus(float mz)
{
	if (_currentScan == NULL) return;
    //int bestMatch=-1;
    //float bestMatchDiff=FLT_MAX;

	MassCutoff *massCutoff= mainwindow->getUserMassCutoff();

	if (_currentScan->mslevel == 1) {
		int pos = _currentScan->findHighestIntensityPos(mz, massCutoff);
		if(pos >= 0) { 
			float bestMz = _currentScan->mz[pos];
			mainwindow->setMzValue(bestMz);
			mainwindow->massCalcWidget->setMass(bestMz);
		}

	} else if (_currentScan->mslevel == 2 && _currentScan->precursorMz > 0) {
			float bestMz = _currentScan->precursorMz;
			mainwindow->setMzValue(bestMz);
			mainwindow->massCalcWidget->setMass(bestMz);

	} else if (!_currentScan->filterLine.empty()) {
		float mzmin = mz - massCutoff->massCutoffValue(mz);
		float mzmax = mz + massCutoff->massCutoffValue(mz);
    	mzSlice eicSlice = mainwindow->getEicWidget()->getParameters()->getMzSlice();
        mzSlice slice(mzmin, mzmax, eicSlice.rtmin, eicSlice.rtmax); 
		slice.srmId =_currentScan->filterLine;

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

void SpectraWidget::addLabel(QString text,float x, float y)
{
    QFont font = QApplication::font(); font.setPointSizeF(font.pointSize()*0.8);

    QGraphicsTextItem* _label = scene()->addText(text, font);
    _label->setPos(toX(x), toY(y));  
}

void SpectraWidget::mouseMoveEvent(QMouseEvent* event)
{
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
    } else {
        _vnote->hide(); _note->hide(); _varrow->hide(); _arrow->hide();
    }


}

int SpectraWidget::findNearestMz(QPointF pos)
{
    float mz = invX(pos.x());
    float mzmin = invX(pos.x() - 50);
    float mzmax = invX(pos.x() + 50);
    float ycoord = invY(pos.y());
    int best = -1;

	vector<int> matches = _currentScan->findMatchingMzs(mzmin,mzmax);
    if (matches.size() > 0) {
        float dist = FLT_MAX;
        for(int i = 0; i < matches.size(); i++) {
            int p = matches[i];
			float d = sqrt(POW2(_currentScan->intensity[p] - ycoord) + 
                                POW2(_currentScan->mz[p] - mz));
            if (d < dist) { best = p; dist = d; }
        }
    }
    return best;
}

void SpectraWidget::drawArrow(float mz1, float intensity1, float mz2, float intensity2)
{
    if (mzUtils::almostEqual(static_cast<float>(_focusCoord.x()), 0.0f))
        return;

    float SCALE = 1.0;
    float OFFSET = 0;
    if (_showOverlay) {
        SCALE = 0.45;
        OFFSET = -scene()->height() / 2.0;
    }
    
    int x1 = toX(mz1);
    int y1 = toY(intensity1, SCALE, OFFSET);

    int x2 = toX(mz2);
    int y2 = toY(intensity2, SCALE, OFFSET);

    int x3 = toX(_focusCoord.x());
    int y3 = toY(_focusCoord.y(), SCALE, OFFSET);

    //change user cutoff to fragment tolerance if this is MS2 m/z
    if ( massCutoffDist(mz1,mz2,mainwindow->getUserMassCutoff()) < 0.1 ) return;

    if (_arrow != NULL ) {
        _arrow->setVisible(true);
        _arrow->setLine(x1,y1,x2,y2);
    }

    float distance = mz1 - _focusCoord.x();
    float totalInt = _focusCoord.y();
    float diff = 0;
    if (totalInt)
        diff = intensity1 / totalInt;

    if (_varrow != NULL && abs(distance) > 0.1 ) {
        _varrow->setLine(x1, y1, x3, y3);
        _vnote->setPos(x1 + (x3 - x1) / 2.0, y1 + (y3 - y1) / 2.0);
        _vnote->setPlainText(QString::number(diff * 100, 'f', 2) + "%" );
        _varrow->show(); _vnote->show();
    } else {
        _varrow->hide(); _vnote->hide();
    }

    QString note = tr("m/z: %1 &Delta;%2").arg(QString::number(mz1, 'f', 6),
                                               QString::number(distance, 'f', 3));

    if (_note != NULL ) {
        _note->setPos(x2 + 1, y2 - 30);
        _note->setHtml(note);
    }

    _note->show();
    _arrow->show();
}


void SpectraWidget::wheelEvent(QWheelEvent *event)
{
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

void SpectraWidget::annotateScan()
{
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

void SpectraWidget::zoomIn()
{
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

void SpectraWidget::zoomOut()
{
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

void SpectraWidget::contextMenuEvent(QContextMenuEvent * event)
{
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

void SpectraWidget::spectraToClipboard()
{
	if(!_currentScan) return;

    QStringList clipboardText;
	for(int i=0; i < _currentScan->nobs(); i++ ) {
        clipboardText  << tr("%1\t%2")
				.arg(QString::number(_currentScan->mz[i],'f', 6))
				.arg(QString::number(_currentScan->intensity[i],'f',6));
    }
    QApplication::clipboard()->setText(clipboardText.join("\n"));

}

void SpectraWidget::spectraToClipboardTop()
{
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

void SpectraWidget::gotoScan()
{ 
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

vector<mzLink> SpectraWidget::findLinks(float centerMz, Scan* scan, MassCutoff *massCutoff, int ionizationMode)
{
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

    //TODO Shubhra: What is this meant to do?
    // for(int i=0; i < DB.fragmentsDB.size(); i++ ) {
    //     Adduct* frag  = DB.fragmentsDB[i];
    // 	if(frag->charge != 0 && SIGN(frag->charge) != SIGN(ionizationMode) ) continue;
    //     float mzMinus=centerMz-frag->mass;
    //     float mzPlus =centerMz+frag->mass;
    //     if( scan->hasMz(mzPlus,massCutoff)) {
    //         QString noteText = tr("%1 Fragment").arg(QString(DB.fragmentsDB[i]->name.c_str()));
    //         links.push_back(mzLink(centerMz,mzPlus,noteText.toStdString()));
    //     }

    //     if( scan->hasMz(mzMinus,massCutoff)) {
    //         QString noteText = tr("%1 Fragment").arg(QString(DB.fragmentsDB[i]->name.c_str()));
    //         links.push_back(mzLink(centerMz,mzMinus,noteText.toStdString()));
    //     }
    // }

    //parent check
    //TODO Shubhra: Figure out the purpose of this function and uncomment as required
    for(int i = 0; i < DB.adductsDB.size(); i++) {
    	if (SIGN(DB.adductsDB[i]->getCharge()) != SIGN(ionizationMode))
            continue;
        //float parentMass=DB.adductsDB[i]->computeParentMass(centerMz);
        //parentMass += ionizationMode * H_MASS;   //adjusted mass

    //TODO Shubhra: uncomment
    //     if( abs(parentMass-centerMz)>0.1 && scan->hasMz(parentMass,massCutoff)) {
    //         cerr << DB.adductsDB[i]->name << " " << DB.adductsDB[i]->charge << " " << parentMass << endl;
    //         QString noteText = tr("Possible Parent %1").arg(QString(DB.adductsDB[i]->name.c_str()));
    //         links.push_back(mzLink(centerMz,parentMass,noteText.toStdString()));
    //     }
    }

    //adduct check
    //TODO Shubhra: uncomment
    for(int i = 0; i < DB.adductsDB.size(); i++) {
        if (SIGN(DB.adductsDB[i]->getCharge()) != SIGN(ionizationMode))
            continue;
    //     float parentMass = centerMz-ionizationMode * H_MASS;   //adjusted mass
    //     float adductMass=DB.adductsDB[i]->computeAdductMass(parentMass);
    //     if( abs(adductMass-centerMz)>0.1 && scan->hasMz(adductMass,massCutoff)) {
    //         QString noteText = tr("Adduct %1").arg(QString(DB.adductsDB[i]->name.c_str()));
    //         links.push_back(mzLink(centerMz,adductMass,noteText.toStdString()));
    //     }
    }


    return links;
}

void SpectraWidget::constructAverageScan(float rtmin, float rtmax)
{
    if (_avgScan != NULL) delete(_avgScan);
    _avgScan=NULL;

    if (_currentScan && _currentScan->getSample()) {
        Scan* avgScan = _currentScan->getSample()->getAverageScan(rtmin,rtmax,_currentScan->mslevel,_currentScan->getPolarity(),(float)100.0);
        qDebug() << "constructAverageScan() " << rtmin << " " << rtmax << " mslevel=" << _currentScan->mslevel << endl;
        avgScan->simpleCentroid();
        if(avgScan) setScan(avgScan);
    }
}

void SpectraWidget::findSimilarScans()
{
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

void SpectraWidget::copyImageToClipboard()
{
    QPixmap image(this->width(), this->height());
    image.fill(Qt::white);
    QPainter painter;
    painter.begin(&image);
    render(&painter);
    painter.end();
    QApplication::clipboard()->setPixmap(image);
}

void SpectraWidget::clearScans()
{
    _currentScan = nullptr;
    _avgScan = nullptr;
    _scanset.clear();
}
