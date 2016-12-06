#include "isotopeplot.h"

// import most common Eigen types
//USING_PART_OF_NAMESPACE_EIGEN
using namespace Eigen;


IsotopePlot::IsotopePlot(QGraphicsItem* parent, QGraphicsScene *scene)
    :QGraphicsItem(parent, scene) {
	// Initialised existing values - Kiran
	_barwidth=10;
	_mw=NULL;
	_group=NULL;
    if ( scene != NULL ) {
        _width = scene->width()*0.25;
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
}

void IsotopePlot::setPeakGroup(PeakGroup* group) {
    //cerr << "IsotopePlot::setPeakGroup()" << group << endl;

    if ( group == NULL ) return;

    if (group->isIsotope() && group->getParent() ) {
        setPeakGroup(group->getParent());
    }

    if ( isVisible() == true && group == _group) return;
    _group = group;

	_samples.clear();
	_samples = _mw->getVisibleSamples();
	 sort(_samples.begin(), _samples.end(), mzSample::compSampleOrder);

    _isotopes.clear();
    for(int i=0; i < group->childCountBarPlot(); i++ ) {
        if (group->childrenBarPlot[i].isIsotope() ) {
            PeakGroup* isotope = &(group->childrenBarPlot[i]);
            _isotopes.push_back(isotope);
        }
    }
    std::sort(_isotopes.begin(), _isotopes.end(), PeakGroup::compC13);
	/*
	for(int i=0; i < _isotopes.size(); i++ )  {
		cerr << _isotopes[i]->tagString <<  " " << _isotopes[i]->isotopeC13count << endl; 
	}
	*/

    showBars();
}


IsotopePlot::~IsotopePlot() {}

QRectF IsotopePlot::boundingRect() const
{
    return(QRectF(0,0,_width,_height));
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

    if (scene()) {
        _width =   scene()->width()*0.20;
        _barwidth = scene()->height()*0.75/visibleSamplesCount;
        if (_barwidth<3)  _barwidth=3;
        if (_barwidth>15) _barwidth=15;
        _height = visibleSamplesCount*_barwidth;
    }

    for(int i=0; i<MM.rows(); i++ ) {		//samples
        float sum= MM.row(i).sum();
        if (sum == 0) continue;
        MM.row(i) /= sum;
    }

    //_mw->customPlot->plotLayout()->clear();
    _mw->customPlot->clearPlottables();
    _mw->setIsotopicPlotStyling();
    //_mw->customPlot->plotLayout()->clear();

    QCPTextElement *title = new QCPTextElement(_mw->customPlot);
    title->setText(_isotopes[0]->compound->name.c_str());
    title->setFont(QFont("sans", 12, QFont::Bold));
    //_mw->customPlot->plotLayout()->insertRow(0);
  // then we add it to the main plot layout:
    // insert an empty row above the axis rect
    _mw->customPlot->plotLayout()->addElement(0, 0, title); 

    _mw->customPlot->legend->setVisible(true);
    QCPAxisRect *bottomAxisRect = new QCPAxisRect(_mw->customPlot);
    _mw->customPlot->plotLayout()->addElement(1, 0, bottomAxisRect);

    _mw->customPlot->plotLayout()->addElement(1, 1, _mw->customPlot->legend);

    _mw->customPlot->plotLayout()->setColumnStretchFactor(0, 8);
    _mw->customPlot->plotLayout()->setColumnStretchFactor(1, 2);

    //_mw->customPlot->plotLayout()->addElement(0, 0, customPlot->legend);

    QVector<QCPBars *> isotopesType(MM.cols());
    for(int j=0; j < MM.cols(); j++ ) {
        isotopesType[j] = new QCPBars(_mw->customPlot->xAxis, _mw->customPlot->yAxis);
        isotopesType[j]->setAntialiased(true); // gives more crisp, pixel aligned bar borders
        isotopesType[j]->setStackingGap(1);
        int h = j ;
        isotopesType[j]->setPen(QPen(QColor::fromHsvF(h/20.0,1.0,1.0,1.0)));
	    isotopesType[j]->setBrush(QColor::fromHsvF(h/20.0,1.0,1.0,1.0));
        if (j != 0 ){
            isotopesType[j]->moveAbove(isotopesType[j - 1]);
        }
        QVector<double> isotopeData(MM.rows());
        QVector<double> sampleData(MM.rows());
        isotopesType[j]->setName(_isotopes[j]->tagString.c_str());
        for(int i=0; i<MM.rows(); i++ ) {
            double length  = MM(i,j);
            if(length < 0 ) length = 0;
            isotopeData << length;
            sampleData << i;
        }

        isotopesType[j]->setData(sampleData, isotopeData);
    }
    _mw->customPlot->rescaleAxes();
    _mw->customPlot->replot();
    delete(title);
    delete(bottomAxisRect);
    //detete()

    // for(int i=0; i<MM.rows(); i++ ) {		//samples
    //     float sum= MM.row(i).sum();
    //     if (sum == 0) continue;
    //     MM.row(i) /= sum;

    //     double ycoord = _barwidth*i; 
    //     double xcoord = 0;

    //     for(int j=0; j < MM.cols(); j++ ) {	//isotopes
    //         double length  = MM(i,j) * _width;
    //         int h = j % 20;
    //         if(length<0 ) length=0;
    //         //qDebug() << length << " " << xcoord << " " << ycoord;
    //         QBrush brush(QColor::fromHsvF(h/20.0,1.0,1.0,1.0));
    //         isotopesType[j]
    //         IsotopeBar* rect = new IsotopeBar(this,scene());
    //         rect->setBrush(brush);
    //         rect->setRect(xcoord,ycoord,length,_barwidth);

    //         QString name = tr("%1 <br> %2 <b>%3\%</b>").arg(_samples[i]->sampleName.c_str(),
    //                                                         _isotopes[j]->tagString.c_str(),
    //                                                         QString::number(MM(i,j)*100));

    //         rect->setData(0,QVariant::fromValue(name));
    //         rect->setData(1,QVariant::fromValue(_isotopes[j]));

    //         if(_mw) {
    //             //connect(rect,SIGNAL(groupSelected(PeakGroup*)),_mw, SLOT(setPeakGroup(PeakGroup*)));
    //             //connect(rect,SIGNAL(groupUpdated(PeakGroup*)),_mw->pathwayWidget, SLOT(updateCompoundConcentrations()));
    //             //connect(rect,SIGNAL(showInfo(QString,int,int)),_mw->getEicWidget(),SLOT(setStatus(QString,int,int)));
    //         }
    //         xcoord += length;
    //     }
    // }
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

