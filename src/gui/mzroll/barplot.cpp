#include "barplot.h"
#include "eiclogic.h"
#include "eicwidget.h"
#include "mainwindow.h"
#include "mzSample.h"

PeakGroup::QType BarPlot::qtype = PeakGroup::AreaTop;

BarPlot::BarPlot(QGraphicsItem* parent, QGraphicsScene *scene)
    :QGraphicsItem(parent) {
    // uninitialised variable - Kiran
    _mw=NULL;
    _width = 0;
    _height = 0;
    _barwidth=10;
    _showSampleNames=true;
    _showIntensityText=true;
    _showQValueType=true;
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);

}

void BarPlot::switchQValue() {  
    //TODO: updated equation, didn't understand why - Kiran
    //VC: I'm so confused by what this next line was supposed to do.
    //I suspect it caused a bug when I increased the number of qtypes, but I don't see what it was
    //supposed to do in the first place. qtype is set in setPeakGroup()
    //it seems like it temporarily changes the qtype?
    //BarPlot::qtype = (PeakGroup::QType) (((int) qtype+1) % 7);
	PeakGroup* g = NULL;
    if ( _mw != NULL && _mw->getEicWidget() ) g =  _mw->getEicWidget()->getParameters()->displayedGroup();
	if ( g != NULL ) {
		setPeakGroup(g);
		scene()->update();
	}
}

BarPlot::~BarPlot() {
    clear();
}

QRectF BarPlot::boundingRect() const
{
	return(QRectF(0,0,_width,_height));
}

void BarPlot::clear() {
	_yvalues.clear();
	_labels.clear();
	_colors.clear();
}


void BarPlot::setPeakGroup(PeakGroup* group) {
    clear();
    if (group == NULL ) return;
    if (_mw == NULL   ) return;

    qtype = _mw->getUserQuantType();
    vector<mzSample*> vsamples = _mw->getVisibleSamples();
    sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
    vector<float> yvalues = group->getOrderedIntensityVector(vsamples,BarPlot::qtype);

    if (vsamples.size() <=0 ) return;

    for(int i=0; i < vsamples.size(); i++ ) {
        mzSample* sample = vsamples[i];
        QColor color = QColor::fromRgbF(sample->color[0], sample->color[1],sample->color[2],sample->color[3]);
        QString sampleName( sample->sampleName.c_str());
        if (sample->getNormalizationConstant() != 1.0 )
            sampleName = QString::number(sample->getNormalizationConstant(), 'f', 2) + "x " + sampleName;

        _labels.push_back(sampleName);
        _colors.push_back(color);
        _yvalues.push_back(yvalues[i]);
    }

    if (scene()) {
        _width =   scene()->width()*0.20;
        _barwidth = scene()->height()*0.75/vsamples.size();
        if (_barwidth<3)  _barwidth=3;
        if (_barwidth>15) _barwidth=15;
        _height = _yvalues.size()*_barwidth;
    }

}

void BarPlot::wheelEvent ( QGraphicsSceneWheelEvent * event ) {
    qDebug() << "wheelEvent()" ;
    qreal scale = this->scale();
    event->delta() > 0 ? scale *= 1.2 :  scale *= 0.9;
    if (scale < 0.1) scale=0.1;
    if (scale > 2 ) scale=2;
    this->setScale(scale);
}

int BarPlot::intensityTextShift() {

    QFont font("Helvetica");
    float fontsize = _barwidth*0.8;
    if  (fontsize < 1 ) fontsize=1;
    font.setPointSizeF(fontsize);
    QFontMetrics fm( font );
    return fm.size(0,"100e+10",0,NULL).width();   

}

void BarPlot::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    int visibleSamplesCount = _yvalues.size();
    if (visibleSamplesCount == 0) return;
    if (!scene()) return;

    float maxYvalue=0;
    for(int i=0;i<_yvalues.size();i++) { if (_yvalues[i]>maxYvalue) maxYvalue=_yvalues[i]; }
    if (maxYvalue==0) return;

    float maxBarHeight =   scene()->width()*0.25;
    if ( maxBarHeight < 10 ) maxBarHeight=10;
    if ( maxBarHeight > 200 ) maxBarHeight=200;


    int barSpacer=1;

    QFont font("Helvetica");
    float fontsize = _barwidth*0.8;
    if  (fontsize < 1 ) fontsize=1;
    font.setPointSizeF(fontsize);
    painter->setFont(font);
    QFontMetrics fm( font );
    int lagendShift = fm.size(0,"100e+10",0,NULL).width();   

    QColor color = QColor::fromRgbF(0.2,0.2,0.2,1.0);
    QBrush brush(color);

    int legendX = 0;
    int legendY = 0;

    QString title;
    switch (qtype ) {
    case PeakGroup::AreaTop: title = "Peak AreaTop"; break;
    case PeakGroup::Area: title = "Peak Area"; break;
    // new feature - Kiran
    case PeakGroup::AreaNotCorrected: title = "Peak Area Not Corrected"; break;
    case PeakGroup::AreaTopNotCorrected: title = "Peak AreaTop Not Corrected"; break;
    case PeakGroup::Height: title = "Peak Height"; break;
    case PeakGroup::Quality: title = "Peak Quality"; break;
    case PeakGroup::RetentionTime: title = "RetentionTime"; break;
    case PeakGroup::SNRatio: title = "Peak S/N Ratio"; break;
    default: title = "?"; break;
    }

    if (_showQValueType) {
        painter->drawText(legendX-lagendShift,legendY-1,title);
    }

    for(int i=0; i < _yvalues.size(); i++ ) {
        int posX = legendX;
        int posY = legendY + i*_barwidth;
        int width = _barwidth;
        int height = _yvalues[i] / maxYvalue * maxBarHeight;

        //qDebug() << _yvalues[i] << " " << height << " " << maxYvalue << endl;

        painter->setPen(Qt::black);        // do not draw outline
        if (_yvalues[i] == 0 ) painter->setPen(Qt::gray);

        brush.setColor(_colors[i]);
        brush.setStyle(Qt::SolidPattern);

        painter->setBrush(brush);
        painter->drawRect(posX+3,posY,height,width);

        if (_showSampleNames) {
            painter->drawText(posX+6,posY+_barwidth-2,_labels[i]);
        }

        char numType='g';
        int  numPrec=2;
        if (maxYvalue < 10000 ) { numType='f'; numPrec=0;}
        if (maxYvalue < 1000 ) { numType='f';  numPrec=1;}
        if (maxYvalue < 100 ) { numType='f';   numPrec=2;}
        if (maxYvalue < 1 ) { numType='f';     numPrec=3;}

        if (_yvalues[i] > 0 && _showIntensityText) {
            QString value = QString::number(_yvalues[i],numType,numPrec);
            painter->drawText(posX-lagendShift,posY+_barwidth-2,value);
        }

        if ( posY+_barwidth > _height) _height = posY+_barwidth+barSpacer;
    }

    painter->setPen(Qt::black);        // do not draw outline
    painter->setBrush(Qt::NoBrush);
    painter->drawLine(legendX,legendY,legendX,legendY+_height);

    _width = lagendShift+maxBarHeight;

}
