#include "isotopeplot.h"

using namespace Eigen;


IsotopePlot::IsotopePlot(QCustomPlot* customPlot){

	_barwidth=10;
	_group=NULL;
    mpMouseText = NULL;
    title = NULL;
    bottomAxisRect = NULL;
    this->customPlot = customPlot;
}

void IsotopePlot::clear() { 
    QList<QGraphicsItem *> mychildren = QGraphicsItem::childItems();
    if (mychildren.size() > 0 ) {
        Q_FOREACH (QGraphicsItem *child, mychildren) {
            scene()->removeItem(child);
            delete(child);
        }
    }
    if (customPlot) {
        if(mpMouseText) {
            customPlot->removeItem(mpMouseText);
        }
        disconnect(customPlot, SIGNAL(mouseMove(QMouseEvent*)));
        customPlot->plotLayout()->clear();
        customPlot->clearPlottables();
        customPlot->replot();
    }
}

void IsotopePlot::setPeakGroup(PeakGroup* group) {

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
    for(int i=0; i<MM.rows(); i++ )
        labels << QString::fromStdString(_samples[i]->sampleName.c_str());


    MMDuplicate = MM;

    customPlot->plotLayout()->clear();
    customPlot->clearPlottables();

    title = new QCPTextElement(customPlot);

    title->setText(_isotopes[0]->compound->name.c_str());
    title->setFont(QFont("Helvetica", 12, QFont::Bold));
    customPlot->plotLayout()->addElement(0, 0, title); 

    bottomAxisRect = new QCPAxisRect(customPlot);

    customPlot->plotLayout()->addElement(1, 0, bottomAxisRect);
    isotopesType.resize(MM.cols());

    for(int j=0; j < MM.cols(); j++ ) {
        isotopesType[j] = new QCPBars(customPlot->xAxis, customPlot->yAxis);
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
        customPlot->removeItem(mpMouseText);
    }
    mpMouseText = new QCPItemText(customPlot);

    if(!mpMouseText) return;

    mpMouseText->setFont(QFont("Helvetica", 12)); // make font a bit larger
    mpMouseText->position->setType(QCPItemPosition::ptAxisRectRatio);
    mpMouseText->setPositionAlignment(Qt::AlignLeft);
    mpMouseText->position->setCoords(QPointF(0, 0));
    mpMouseText->setText("");
    mpMouseText->setPen(QPen(Qt::black)); // show black border around text

    _mw->setIsotopicPlotStyling();
    customPlot->xAxis->setRange(-0.5, MM.rows());

    disconnect(customPlot, SIGNAL(mouseMove(QMouseEvent*)));
    connect(customPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showPointToolTip(QMouseEvent*)));

    customPlot->replot();
}

void IsotopePlot::showPointToolTip(QMouseEvent *event) {

    if (!event) return;
    if (customPlot->plotLayout()->elementCount() <= 0) return;

    int x = customPlot->xAxis->pixelToCoord(event->pos().x());
    double keyPixel =  customPlot->xAxis->coordToPixel(x);
    double shiftRight =  customPlot->xAxis->coordToPixel(x + .75 * 0.5) - keyPixel;
    x = customPlot->xAxis->pixelToCoord(event->pos().x() + shiftRight);
    int y = customPlot->yAxis->pixelToCoord(event->pos().y());

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
    customPlot->replot();
}


void IsotopePlot::contextMenuEvent(QContextMenuEvent * event) {
    QMenu menu;

    SettingsForm* settingsForm = _mw->settingsForm;

    QAction* d = menu.addAction("Isotope Detection Options");
    connect(d, SIGNAL(triggered()), settingsForm, SLOT(showIsotopeDetectionTab()));
    connect(d, SIGNAL(triggered()), settingsForm, SLOT(show()));

    menu.exec(event->globalPos());

}

