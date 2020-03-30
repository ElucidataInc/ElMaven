#include <qcustomplot.h>

#include "Compound.h"
#include "isotopeplot.h"
#include "mainwindow.h"
#include "mzSample.h"
#include "settingsform.h"

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

void IsotopePlot::replot()
{
    clear();
    _samples.clear();
    _samples = _mw->getVisibleSamples();
    if (_samples.size() == 0)
        return;
    sort(_samples.begin(), _samples.end(), mzSample::compRevSampleOrder);
    showBars();
}

void IsotopePlot::setPeakGroup(PeakGroup* group) {
    //cerr << "IsotopePlot::setPeakGroup()" << group << endl;
    if ( group == NULL ) return;
    if (group->childCountBarPlot() == 0) return;

    if (group->isIsotope() && group->getParent() ) {
        setPeakGroup(group->getParent());
        return;
    }

    clear();

    if (_group)
        delete _group;
    _group = new PeakGroup;
    _group->copyObj(*group);

	_samples.clear();
	_samples = _mw->getVisibleSamples();
    if (_samples.size() == 0) return;
	    sort(_samples.begin(), _samples.end(), mzSample::compRevSampleOrder);

    _isotopes.clear();
    for(int i=0; i < _group->childCountBarPlot(); i++ ) {
        if (_group->childrenBarPlot[i].isIsotope() ) {
            PeakGroup isotope = _group->childrenBarPlot[i];
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

void IsotopePlot::showBars() {
    clear();
    if (_isotopes.size() == 0 ) return;
    if (_samples.size() == 0 ) return;

    int visibleSamplesCount = _samples.size();
    sort(_samples.begin(), _samples.end(), mzSample::compRevSampleOrder);

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

    title->setText(_isotopes[0].getCompound()->name().c_str());
    title->setFont(QFont("Helvetica", 12, QFont::Bold));
    _mw->customPlot->plotLayout()->addElement(0, 0, title); 

    bottomAxisRect = new QCPAxisRect(_mw->customPlot);

    _mw->customPlot->plotLayout()->addElement(1, 0, bottomAxisRect);
    //+1 to add a bar for 'Other' labels
    isotopesType.resize(MM.cols()+1);
    QPen barPen(Qt::black);
    barPen.setWidthF(0.5);

    QVector<double> poolData(MM.rows(), 0);

    for(int j=0; j < MM.cols()+1; j++ )
    {
        isotopesType[j] = new QCPBars(_mw->customPlot->yAxis, _mw->customPlot->xAxis);
        isotopesType[j]->setAntialiased(true); // gives more crisp, pixel aligned bar borders
        isotopesType[j]->setStackingGap(0);
        int h = j % 10;
        isotopesType[j]->setPen(barPen);
	    isotopesType[j]->setBrush(QColor::fromHsvF(h/10.0,1.0,1.0,1.0));
        if (j != 0 ){
            isotopesType[j]->moveAbove(isotopesType[j - 1]);
        }

        QVector<double> isotopeData(MM.rows(), 0);
        QVector<double> sampleData(MM.rows(), 0);

        for(int i=0; i<MM.rows(); i++ )
        {
            //if j is a valid label
            if (j < MM.cols())
            {
                double length  = MM(i,j);
                if(length < 0) length = 0;
                //add to 'Other' bar if < poolThreshold
                if (length*100 < _poolThreshold)
                {
                    poolData[i] += length;
                    length = 0;
                }
                isotopeData[i] = length;
            }
            sampleData[i] = i;
        }
        
        //add individual label info
        if (j < MM.cols()) isotopesType[j]->setData(sampleData, isotopeData);
        else isotopesType[j]->setData(sampleData, poolData);
        isotopesType[j]->rescaleKeyAxis(false);
        isotopesType[j]->rescaleValueAxis(true);
    }

    if(mpMouseText) {
        _mw->customPlot->removeItem(mpMouseText);
    }
    mpMouseText = new QCPItemText(_mw->customPlot);
    
    _mw->setIsotopicPlotStyling();
    _mw->customPlot->yAxis->setRange(-0.5, MM.rows());

    disconnect(_mw->customPlot, SIGNAL(mouseMove(QMouseEvent*)));
    connect(_mw->customPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showPointToolTip(QMouseEvent*)));

    _mw->customPlot->replot();
}

void IsotopePlot::showPointToolTip(QMouseEvent *event) {

    if (!event) return;
    if (_mw->customPlot->plotLayout()->elementCount() <= 0) return;

    int y = _mw->customPlot->yAxis->pixelToCoord(event->pos().y());
    double keyPixel =  _mw->customPlot->yAxis->coordToPixel(y);
    double shiftAbove =  _mw->customPlot->yAxis->coordToPixel(y + .75 * 0.5) - keyPixel;
    y = _mw->customPlot->yAxis->pixelToCoord(event->pos().y() + shiftAbove);
    int x = _mw->customPlot->xAxis->pixelToCoord(event->pos().x());

    if (y < labels.count() && y >= 0) {
        QString name = labels.at(y);
        if (MMDuplicate.cols() != _isotopes.size()) return;
        float pool = 0;

        for(int j=0; j < MMDuplicate.cols(); j++ ) {
            if (y >= MMDuplicate.rows()) return;
            if (MMDuplicate(y,j)*100 > _poolThreshold) 
            {
                name += tr("\n %1 : %2\%").arg(_isotopes[j].tagString.c_str(),
                            QString::number(MMDuplicate(y,j)*100, 'f', 2));
            }
            else pool += MMDuplicate(y,j);
        }

        if (pool)
            name += tr("\nOther: %1\%").arg(QString::number(pool*100, 'f', 2));

        if(!mpMouseText) return;

        mpMouseText->setFont(QFont("Helvetica", 12));
        mpMouseText->position->setType(QCPItemPosition::ptPlotCoords);
        mpMouseText->setPositionAlignment(Qt::AlignLeft);
        mpMouseText->position->setCoords(QPointF(0, 0));
        mpMouseText->setText("");
        mpMouseText->setPen(QPen(Qt::black)); // show black border around text
        mpMouseText->setBrush(QColor::fromRgb(255,255,255));
        mpMouseText->setTextAlignment(Qt::AlignLeft);
        mpMouseText->setClipToAxisRect(true);
        
        int g = QString::compare(name, labels.at(y), Qt::CaseInsensitive);
        if(g == 0) {
            mpMouseText->setText("");
        } else {
            mpMouseText->setText(name);
        }

        double xPos = _mw->customPlot->xAxis->pixelToCoord(event->pos().x());
        double yPos = _mw->customPlot->yAxis->pixelToCoord(event->pos().y());
        mpMouseText->position->setCoords(xPos, yPos);
        mpMouseText->setFont(QFont("Helvetica", 9, QFont::Bold));
        QRectF textRect(mpMouseText->topLeft->pixelPosition(), mpMouseText->bottomRight->pixelPosition  ());
        QRectF plotRect = _mw->customPlot->axisRect()->rect();

        //check for textItem moving out of plotbounds
        if (textRect.intersected(plotRect) != textRect) {
            //check for textItem moving out of right bound
            if (mpMouseText->topRight->pixelPosition().x() > plotRect.topRight().x())
                {
                    //shift label to left of cursor
                    mpMouseText->position->setCoords(xPos, yPos);
                    mpMouseText->setPositionAlignment(Qt::AlignRight);
                }

            //check for textItem moving out of bottom bound
            if (textRect.bottomRight().y() > plotRect.bottomRight().y())
            {
                //shift label to top of plot
                double yPos_new = _mw->customPlot->yAxis->pixelToCoord(plotRect.topLeft().y());
                mpMouseText->position->setCoords(xPos, yPos_new);
                //move to topright
                mpMouseText->setPositionAlignment(Qt::AlignLeft);
                //move to topleft if out of right bound
                if (mpMouseText->topRight->pixelPosition().x() > plotRect.topRight().x())
                    mpMouseText->setPositionAlignment(Qt::AlignRight);

                //check for label floating on top
                if (mpMouseText->bottomLeft->pixelPosition().y() < event->pos().y())
                {
                    //move to topright of cursor
                    mpMouseText->position->setCoords(xPos, yPos);
                    mpMouseText->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
                    //move to topleft of cursor if out of right bound
                    if (mpMouseText->topRight->pixelPosition().x() > plotRect.topRight().x())
                        mpMouseText->setPositionAlignment(Qt::AlignRight | Qt::AlignBottom);
                }
            }
        }
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
