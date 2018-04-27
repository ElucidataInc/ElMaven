#include "isotopeplot.h"

// import most common Eigen types
//USING_PART_OF_NAMESPACE_EIGEN
using namespace Eigen;


IsotopePlot::IsotopePlot(MainWindow *mw)
    :QGraphicsItem() {
	// Initialised existing values - Kiran
	_barwidth=10;
	_mw=mw;
	_group=NULL;
    mpMouseText = NULL;
    title = NULL;
    bottomAxisRect = NULL;
    if (scene()) {
        _width = scene()->width()*0.25;
        _height = 10;
    }
}

void IsotopePlot::setMainWindow(MainWindow* mw) { _mw = mw; }

void IsotopePlot::clear() { 
    QList<QGraphicsItem *> mychildren = QGraphicsItem::childItems();
    if (mychildren.size() > 0 ) {
        Q_FOREACH (QGraphicsItem *child, mychildren) {
            scene()->removeItem(child);
            delete(child);
        }
    }
    if (_mw) {
        if (_mw->customPlot) {
            if(mpMouseText) {
                _mw->customPlot->removeItem(mpMouseText);
            }
            disconnect(_mw->customPlot, SIGNAL(mouseMove(QMouseEvent*)));
            _mw->customPlot->plotLayout()->clear();
            _mw->customPlot->clearPlottables();
            _mw->customPlot->replot();
        }
    }
}

void IsotopePlot::setPeakGroup(PeakGroup* group) {
    //cerr << "IsotopePlot::setPeakGroup()" << group << endl;

    clear();
    if ( group == NULL ) return;
    if (group->childCountBarPlot() == 0) return;

    if (group->isIsotope() && group->getParent() ) {
        setPeakGroup(group->getParent());
    }

    if ( isVisible() == true && group == _group) return;
    _group = group;

	_samples.clear();
	_samples = _mw->getVisibleSamples();
    if (_samples.size() == 0) return;
	 sort(_samples.begin(), _samples.end(), mzSample::compSampleOrder);

    _isotopes.clear();
    for(int i=0; i < group->childCountBarPlot(); i++ ) {
        if (group->childrenBarPlot[i].isIsotope() ) {
            PeakGroup* isotope = &(group->childrenBarPlot[i]);
            _isotopes.push_back(isotope);
        }
    }

    showBars();
}


IsotopePlot::~IsotopePlot() {}

QRectF IsotopePlot::boundingRect() const
{
    return(QRectF(0,0,_width,_height));
}

void IsotopePlot::setBelowAbThresholdMatrixEntries(MatrixXf &MM, MainWindow* _mw) {
    for(int i = 0; i < MM.rows(); i++) {
        for(int j = 0; j < MM.cols(); j++) {
            double percent = (double) (MM(i,j)*100);
            if(percent <= _mw->getSettings()->value("AbthresholdBarplot").toDouble()) MM(i,j) = 0;
        }
    }
}

void IsotopePlot::normalizeIsotopicMatrix(MatrixXf &MM) {

    for(int i = 0; i < MM.rows(); i++) {
		float sum = 0;
		for(int j = 0; j < MM.cols(); j++) sum += MM(i,j);
		if(sum<=0) continue;
		for(int j = 0; j < MM.cols(); j++) MM(i,j) /= sum;
	}

}

void IsotopePlot::showBars() {
    clear();
    if (_isotopes.size() == 0 ) return;
    if (_samples.size() == 0 ) return;

    int visibleSamplesCount = _samples.size();
    sort(_samples.begin(), _samples.end(), mzSample::compSampleOrder);

    PeakGroup::QType qtype = PeakGroup::AreaTop;
    if ( _mw ) qtype = _mw->getUserQuantType();

    MatrixXf MM = _mw->getIsotopicMatrix(_group);
    setBelowAbThresholdMatrixEntries(MM,_mw);
    normalizeIsotopicMatrix(MM);

    if (scene()) {
        _width =   scene()->width()*0.20;
        _barwidth = scene()->height()*0.75/visibleSamplesCount;
        if (_barwidth<3)  _barwidth=3;
        if (_barwidth>15) _barwidth=15;
        _height = visibleSamplesCount*_barwidth;
    }

    labels.resize(0);
    for(int i=0; i<MM.rows(); i++ ) {		//samples
        //float sum= MM.row(i).sum();
        labels << QString::fromStdString(_samples[i]->sampleName.c_str());
        //if (sum == 0) continue;
        //MM.row(i) /= sum;
        //ticks << i ;
    }

    MMDuplicate = MM;

    _mw->customPlot->plotLayout()->clear();
    _mw->customPlot->clearPlottables();

    title = new QCPTextElement(_mw->customPlot);

    title->setText(_isotopes[0]->compound->name.c_str());
    title->setFont(QFont("Helvetica", 12, QFont::Bold));
    _mw->customPlot->plotLayout()->addElement(0, 0, title); 

    bottomAxisRect = new QCPAxisRect(_mw->customPlot);

    _mw->customPlot->plotLayout()->addElement(1, 0, bottomAxisRect);
    isotopesType.resize(MM.cols());

    for(int j=0; j < MM.cols(); j++ ) {
        isotopesType[j] = new QCPBars(_mw->customPlot->xAxis, _mw->customPlot->yAxis);
        isotopesType[j]->setAntialiased(true); // gives more crisp, pixel aligned bar borders
        isotopesType[j]->setStackingGap(0);
        int h = j % 20;
        isotopesType[j]->setPen(QPen(QColor::fromHsvF(h/20.0,1.0,1.0,1.0)));
	    isotopesType[j]->setBrush(QColor::fromHsvF(h/20.0,1.0,1.0,1.0));
        if (j != 0 ){
            isotopesType[j]->moveAbove(isotopesType[j - 1]);
        }
        QVector<double> isotopeData(MM.rows());
        QVector<double> sampleData(MM.rows());

        for(int i=0; i<MM.rows(); i++ ) {
            double length  = MM(i,j);
            if(length < 0 ) length = 0;
            isotopeData << length;
            sampleData << i;
        }
        isotopesType[j]->setData(sampleData, isotopeData);

    }

    if(mpMouseText) {
        _mw->customPlot->removeItem(mpMouseText);
    }
    mpMouseText = new QCPItemText(_mw->customPlot);

    if(!mpMouseText) return;

    //_mw->customPlot->addItem(mpMouseText); 
    mpMouseText->setFont(QFont("Helvetica", 12)); // make font a bit larger
    mpMouseText->position->setType(QCPItemPosition::ptAxisRectRatio);
    mpMouseText->setPositionAlignment(Qt::AlignLeft);
    mpMouseText->position->setCoords(QPointF(0, 0));
    mpMouseText->setText("");
    mpMouseText->setPen(QPen(Qt::black)); // show black border around text

    _mw->setIsotopicPlotStyling();
    //_mw->customPlot->rescaleAxes();
    _mw->customPlot->xAxis->setRange(-0.5, MM.rows());

    disconnect(_mw->customPlot, SIGNAL(mouseMove(QMouseEvent*)));
    connect(_mw->customPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showPointToolTip(QMouseEvent*)));

    _mw->customPlot->replot();
}

void IsotopePlot::showPointToolTip(QMouseEvent *event) {

    if (!event) return;
    if (_mw->customPlot->plotLayout()->elementCount() <= 0) return;

    int x = _mw->customPlot->xAxis->pixelToCoord(event->pos().x());
    double keyPixel =  _mw->customPlot->xAxis->coordToPixel(x);
    double shiftRight =  _mw->customPlot->xAxis->coordToPixel(x + .75 * 0.5) - keyPixel;
    x = _mw->customPlot->xAxis->pixelToCoord(event->pos().x() + shiftRight);
    int y = _mw->customPlot->yAxis->pixelToCoord(event->pos().y());

    if (x < labels.count() && x >= 0) {
        QString name = labels.at(x);
        if (MMDuplicate.cols() != _isotopes.size()) return;

        for(int j=0; j < MMDuplicate.cols(); j++ ) {
            if (x  >= MMDuplicate.rows()) return;
            if (MMDuplicate(x,j)*100 > _mw->getSettings()->value("AbthresholdBarplot").toDouble()) 
            {
                name += tr("\n %1 : %2\%").arg(_isotopes[j]->tagString.c_str(),
                                                    QString::number(MMDuplicate(x,j)*100));
            }
        }
        if(!mpMouseText) return;
        int g = QString::compare(name, labels.at(x), Qt::CaseInsensitive);
        if(g == 0) {
            mpMouseText->setText("");
        } else {
            mpMouseText->setText(name);
        }

        mpMouseText->setFont(QFont("Helvetica", 9, QFont::Bold));
    }
    _mw->customPlot->replot();
}


void IsotopePlot::contextMenuEvent(QContextMenuEvent * event) {
    QMenu menu;

    SettingsForm* settingsForm = _mw->settingsForm;

    QAction* d = menu.addAction("Isotope Detection Options");
    connect(d, SIGNAL(triggered()), settingsForm, SLOT(showIsotopeDetectionTab()));
    connect(d, SIGNAL(triggered()), settingsForm, SLOT(show()));

    menu.exec(event->globalPos());

}

void IsotopePlot::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) { return; }

/*
void IsotopeBar::mouseDoubleClickEvent (QGraphicsSceneMouseEvent*event) {
	QVariant v = data(1);
   	PeakGroup*  x = v.value<PeakGroup*>();
}

void IsotopeBar::mousePressEvent (QGraphicsSceneMouseEvent*event) {}
*/


void IsotopeBar::hoverEnterEvent (QGraphicsSceneHoverEvent*event) {
    QVariant v = data(0);
    QString note = v.value<QString>();
    if (note.length() == 0 ) return;

    QString htmlNote = note;
    setToolTip(note);
    QPointF posG = mapToScene(event->pos());
    Q_EMIT(showInfo(htmlNote, posG.x(), posG.y()+5));
}

void IsotopeBar::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Delete || e->key() == Qt::Key_Backspace ) {
        QVariant v = data(1);
    	PeakGroup*  g = v.value<PeakGroup*>();
        if (g && g->parent && g->parent != g) { g->parent->deleteChild(g); Q_EMIT(groupUpdated(g->parent)); }
        IsotopePlot* parent = (IsotopePlot*) parentItem();
        if (parent) parent->showBars();
    }
}

