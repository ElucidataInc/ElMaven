#include "boxplot.h"

PeakGroup::QType BoxPlot::qtype = PeakGroup::AreaTop;

BoxPlot::BoxPlot(QGraphicsItem* parent, QGraphicsScene *scene)
    : QGraphicsItem(parent, scene) {
    _width = 0;
    _height = 0;
    _barwidth = 10;
}

void BoxPlot::switchQValue() {
    BoxPlot::qtype = (PeakGroup::QType) (((int) qtype + 1) % 6);
    PeakGroup* g = NULL;
    if ( _mw != NULL && _mw->getEicWidget() ) g =  _mw->getEicWidget()->getSelectedGroup();
    if ( g != NULL ) {
        setPeakGroup(g);
        scene()->update();
    }
}

BoxPlot::~BoxPlot() {
    clear();
}

QRectF BoxPlot::boundingRect() const
{
    return (QRectF(0, 0, _width, _height));
}

void BoxPlot::clear() {
    _yvalues.clear();
    _labels.clear();
    _colors.clear();
}


void BoxPlot::setPeakGroup(PeakGroup* group) {
    clear();
    if (group == NULL ) return;
    if (_mw == NULL   ) return;

    qtype = _mw->getUserQuantType();
    vector<mzSample*> vsamples = _mw->getVisibleSamples();
    sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
    vector<float> yvalues = group->getOrderedIntensityVector(vsamples, BoxPlot::qtype);

    if (vsamples.size() == 0) return;

    if (scene()) {
        _width =   scene()->width() * 0.20;
        _barwidth = scene()->height() * 0.75 / vsamples.size();
        if (_barwidth < 3)  _barwidth = 3;
        if (_barwidth > 15) _barwidth = 15;
        _height = _yvalues.size() * _barwidth;
    }


    for (int i = 0; i < vsamples.size(); i++ ) {
        mzSample* sample = vsamples[i];
        QColor color = QColor::fromRgbF(sample->color[0], sample->color[1], sample->color[2], sample->color[3]);
        QRegExp splitStr("[\\t*|\\s*|,|;]");
        QString qname( sample->getSetName().c_str());
        qname = qname.simplified();
        QList<QString> names = qname.split(splitStr);
        foreach (QString name, names) {
            name = name.simplified();
            if (!name.isEmpty()) {
                _labels.push_back(name);
                _yvalues.push_back(yvalues[i]);
                _colors.push_back(color);
            }
        }
    }
}

void BoxPlot::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{

    //qDebug() << "BoxPlot::paint:" << _yvalues.size() ;

    int visibleSamplesCount = _yvalues.size();
    if (visibleSamplesCount == 0) return;
    if (!scene()) return;

    float maxYvalue = 0;
    for (int i = 0; i < _yvalues.size(); i++) {
        if (_yvalues[i] > maxYvalue) maxYvalue = _yvalues[i];
    }
    if (maxYvalue <= 0 ) return;

    QMap<QString, int>setnames;
    for (int i = 0; i < _labels.size(); i++) {
        if (!setnames.contains(_labels[i])) setnames[_labels[i]] = setnames.size();
    }

    QMap<QString, StatisticsVector<float> >setvalues;
    for (int i = 0; i < _labels.size(); i++) {
        setvalues[_labels[i]].push_back(_yvalues[i]);
    }

    QMap<QString, QColor>setcolors;
    for (int i = 0; i < _labels.size(); i++) {
        QString setname = _labels[i];
        if (setcolors.contains(setname) ) {
            QRgb c1 = setcolors[setname].rgb();
            QRgb c2 = _colors[i].rgb();
            QColor c = QColor((c1 + c2) / 2);
            c.setAlphaF(0.5);
            setcolors[setname] = QColor(c);
        } else {
            QColor c = _colors[i];
            c.setAlphaF(0.5);
            setcolors[setname] = c;
        }
    }


    int maxBarHeight =   scene()->width() * 0.25;
    if ( maxBarHeight < 10 ) maxBarHeight = 10;
    if ( maxBarHeight > 200 ) maxBarHeight = 200;

    int barSpacer = 1;

    QFont font("Helvetica");
    float fontsize = _barwidth * 0.8;
    if  (fontsize < 1 ) fontsize = 1;
    font.setPointSizeF(fontsize);
    painter->setFont(font);
    QFontMetrics fm( font );
    int lagendShift = fm.size(0, "100e+10", 0, NULL).width();

    QColor color = QColor::fromRgbF(0.2, 0.2, 0.2, 1.0);
    QBrush brush(color);

    int legendX = 0;
    int legendY = 0;

    //points
    for (int i = 0; i < _yvalues.size(); i++ ) {
        QString setname = _labels[i];
        int setIndex = setnames[setname];
        int posX = legendX;
        int posY = legendY + setIndex * _barwidth;
        int width = _barwidth;
        int height = _yvalues[i] / maxYvalue * maxBarHeight;
        painter->setPen(Qt::black);
        if (_yvalues[i] == 0 ) painter->setPen(Qt::gray);
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(_colors[i]);
        painter->setBrush(brush);
        painter->drawEllipse(posX + height - _barwidth / 2, posY, _barwidth / 2, _barwidth / 2);
    }

    //legend
    foreach (QString setname, setnames.keys()) {
        int setIndex = setnames[setname];
        int posX = legendX;
        int posY = legendY + setIndex * _barwidth;
        painter->drawText(posX + 6, posY + _barwidth - 2, setname);
    }

    //mean values
    foreach (QString setname, setnames.keys()) {
        int setIndex = setnames[setname];
        int posX = legendX;
        int posY = legendY + setIndex * _barwidth;
        float meanValue = setvalues[setname].mean();
        float minValue = setvalues[setname].minimum();
        float maxValue = setvalues[setname].maximum();

        int lowNotch =  minValue / maxYvalue * maxBarHeight;
        int highNotch = maxValue / maxYvalue * maxBarHeight;
        QBrush brush(setcolors[setname]);
        painter->setBrush(brush);
        painter->drawRect(posX + lowNotch, posY, highNotch - lowNotch, _barwidth);

        char numType = 'g';
        int  numPrec = 2;
        if (maxYvalue < 10000 ) {
            numType = 'f';
            numPrec = 0;
        }
        if (maxYvalue < 1000 ) {
            numType = 'f';
            numPrec = 1;
        }
        if (maxYvalue < 100 ) {
            numType = 'f';
            numPrec = 2;
        }
        if (maxYvalue < 1 ) {
            numType = 'f';
            numPrec = 3;
        }

        if (meanValue > 0) {
            QString value = QString::number(meanValue, numType, numPrec);
            painter->drawText(posX - lagendShift, posY + _barwidth - 2, value);
        }
        if ( posY + _barwidth > _height) _height = posY + _barwidth + barSpacer;
    }

    painter->setPen(Qt::black);        // do not draw outline
    painter->setBrush(Qt::NoBrush);
    painter->drawLine(legendX, legendY, legendX, legendY + _height);
    _width = lagendShift + maxBarHeight;

}
