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
        foreach (QGraphicsItem *child, mychildren) {
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
    for(int i=0; i < group->childCount(); i++ ) {
        if (group->children[i].isIsotope() ) {
            PeakGroup* isotope = &(group->children[i]);
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

    //qDebug() << "showBars: " << _width << " " << _height;


    for(int i=0; i<MM.rows(); i++ ) {		//samples
        float sum= MM.row(i).sum();
        if (sum == 0) continue;
        MM.row(i) /= sum;

        double ycoord = _barwidth*i; 
        double xcoord = 0;

        for(int j=0; j < MM.cols(); j++ ) {	//isotopes
            double length  = MM(i,j) * _width;
            int h = j % 20;
            if(length<0 ) length=0;
            //qDebug() << length << " " << xcoord << " " << ycoord;
            QBrush brush(QColor::fromHsvF(h/20.0,1.0,1.0,1.0));
            IsotopeBar* rect = new IsotopeBar(this,scene());
            rect->setBrush(brush);
            rect->setRect(xcoord,ycoord,length,_barwidth);

            QString name = tr("%1 <br> %2 <b>%3\%</b>").arg(_samples[i]->sampleName.c_str(),
                                                            _isotopes[j]->tagString.c_str(),
                                                            QString::number(MM(i,j)*100));

            rect->setData(0,QVariant::fromValue(name));
            rect->setData(1,QVariant::fromValue(_isotopes[j]));

            if(_mw) {
                connect(rect,SIGNAL(groupSelected(PeakGroup*)),_mw, SLOT(setPeakGroup(PeakGroup*)));
                connect(rect,SIGNAL(groupUpdated(PeakGroup*)),_mw->pathwayWidget, SLOT(updateCompoundConcentrations()));
                //connect(rect,SIGNAL(showInfo(QString,int,int)),_mw->getEicWidget(),SLOT(setStatus(QString,int,int)));
            }
            xcoord += length;
        }
    }
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
    emit(showInfo(htmlNote, posG.x(), posG.y()+5));
}

void IsotopeBar::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Delete || e->key() == Qt::Key_Backspace ) {
        QVariant v = data(1);
    	PeakGroup*  g = v.value<PeakGroup*>();
        if (g && g->parent && g->parent != g) { g->parent->deleteChild(g); emit(groupUpdated(g->parent)); }
        IsotopePlot* parent = (IsotopePlot*) parentItem();
        if (parent) parent->showBars();
    }
}

