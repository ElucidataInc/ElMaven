#include "alignmentvizwidget.h"

using namespace std;

AlignmentVizWidget::AlignmentVizWidget(MainWindow* mw)
{
    this->_mw = mw;
}

void AlignmentVizWidget::plotGraph(PeakGroup*  group) {

    intialSetup();
    PeakGroup grp = *group;


    // refRtLine(grp);

    PeakGroup newGroup = getNewGroup(grp);

    drawMessageBox(newGroup, grp);

    QColor colorCurrentGrp = QColor(100, 100, 100, 100);
    QColor colorShadowGrp  = QColor (0, 0, 0, 50);

    plotIndividualGraph(newGroup, colorCurrentGrp);

    if (!checkGroupEquality(newGroup, grp)) {
        plotIndividualGraph(grp, colorShadowGrp);
    }

    _mw->alignmentVizPlot->rescaleAxes();
    _mw->alignmentVizPlot->replot();
}

void AlignmentVizWidget::intialSetup() {
    _mw->alignmentVizPlot->clearPlottables();

    if (_mw) {
        if (_mw->alignmentVizPlot) {
            if(textLabel) {
                _mw->alignmentVizPlot->removeItem(textLabel);
            }
            if(sampleLabel) {
                _mw->alignmentVizPlot->removeItem(sampleLabel);
            }
            // _mw->alignmentVizPlot->plotLayout()->clear();
            _mw->alignmentVizPlot->replot();
        }
    }

    setXAxis();
    setYAxis();
}

void AlignmentVizWidget::setXAxis() {

    _mw->alignmentVizPlot->xAxis->setTicks(true);
    _mw->alignmentVizPlot->xAxis->setSubTicks(true);
    _mw->alignmentVizPlot->xAxis->setVisible(true);
    _mw->alignmentVizPlot->xAxis->setLabel("Retention Time");
}

void AlignmentVizWidget::setYAxis() {

    _mw->alignmentVizPlot->yAxis->setVisible(true);
    _mw->alignmentVizPlot->yAxis->setLabel("Samples");

}

void AlignmentVizWidget::refRtLine(PeakGroup  group) {

    double refRt = getRefRt(group);

    QPen pen;
    pen.setStyle(Qt::DotLine);
    pen.setColor(Qt::red);

    _mw->alignmentVizPlot->addGraph();
    _mw->alignmentVizPlot->graph()->setPen(pen);

    QVector<double> x, y;

    x << refRt << refRt;
    y << 0 << 1000;

    _mw->alignmentVizPlot->graph()->setLineStyle(QCPGraph::lsLine);
    _mw->alignmentVizPlot->graph()->setData(x, y);

    _mw->alignmentVizPlot->replot();

}

double AlignmentVizWidget::getRefRt(PeakGroup group) {

    double refRt;
    if (group.hasCompoundLink()) {
        refRt = group.compound->expectedRt;
    } else {
        refRt = group.medianRt();
    }

    return refRt;
}

void AlignmentVizWidget::drawMessageBox(PeakGroup newGroup, PeakGroup group) {

    float newGroupR2 = calculateRsquare(newGroup);
    float groupR2 = calculateRsquare(group);

    QString message;

    message = "previous R2 = " + QString::number(groupR2, 'f', 3);
    message += "\ncurrent R2 = " + QString::number(newGroupR2, 'f', 3);

    textLabel = new QCPItemText(_mw->alignmentVizPlot);
    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel->position->setCoords(0.001, 0); // place position at center/top of axis rect
    textLabel->setText(message);
    textLabel->setFont(QFont("Times", 12)); 
    textLabel->setPen(QPen(Qt::black)); 

}

float AlignmentVizWidget::calculateRsquare(PeakGroup group) {

    float r2 = 0;

    Q_FOREACH(Peak peak, group.getPeaks()) {

        float diff = peak.rt - getRefRt(group);
        r2 += pow(diff, 2);
    }

    return r2;

}

PeakGroup AlignmentVizWidget::getNewGroup(PeakGroup group) {

    PeakGroup newGroup;

    bool groupFound = false;

    for (unsigned int ii =0 ; ii <_mw->mavenParameters->allgroups.size(); ii++) {
        PeakGroup currentGroup = _mw->mavenParameters->allgroups[ii];
        if (checkGroupEquality(currentGroup, group)) {
            newGroup = currentGroup;
            groupFound = true;
            break;
        }
    }

    if(!groupFound) {
        newGroup = group;
    }

    return newGroup;

}

bool AlignmentVizWidget::checkGroupEquality(PeakGroup grp1, PeakGroup grp2) {
    if (grp1.meanMz == grp2.meanMz 
                && grp1.maxMz == grp2.maxMz
                        && grp1.minMz == grp2.minMz) {
        return true;

    } else {
        return false;
    }
}

void AlignmentVizWidget::plotIndividualGraph(PeakGroup group, QColor color) {

    vector<mzSample*> samples = getSamplesFromGroup(group);
    // QVector<double> retentionTimes = getRetentionTime(samples, group);
    connect(_mw->alignmentVizPlot, SIGNAL(mouseMove(QMouseEvent* )) , this, SLOT(mouseQCPBar(QMouseEvent* )));

    float widthOfBar = getWidthOfBar(group);

    int i = 1;
    Q_FOREACH(mzSample* sample, samples) {

        bar = new QCPBars(_mw->alignmentVizPlot->yAxis, _mw->alignmentVizPlot->xAxis);
        bar->setAntialiased(false);
        bar->setBrush(color);


        float rt;

        rt = getRetentionTime(sample, group);

        if (rt != -1) {

            double baseValue = rt - widthOfBar;

            QVector<double> solidBar;
            QVector<double> tick;

            solidBar << 2*widthOfBar;
            tick << i;

            bar->setBaseValue(baseValue);
            bar->setData(tick, solidBar);

            mapSample[i] = sample;
            mapXAxis[i] = make_pair(baseValue, baseValue + 2*widthOfBar);

            i++;
        }
    }
}

vector<mzSample*> AlignmentVizWidget::getSamplesFromGroup(PeakGroup group) {

    vector<Peak>& peaks = group.getPeaks();
    vector<mzSample*> samples;
    for(unsigned int i=0; i < peaks.size(); i++ ) {
        mzSample* s = peaks[i].getSample();
        samples.push_back(s);
    }
    sort (samples.begin(), samples.end());
    return samples;
}

float AlignmentVizWidget::getWidthOfBar(PeakGroup group) {

    float maxDiff, widthOfBar;
    
    maxDiff = max(getRefRt(group) - group.minRt, group.maxRt - getRefRt(group));

    widthOfBar = maxDiff/500;

    return widthOfBar;
}


double AlignmentVizWidget::getRetentionTime(mzSample* sample, PeakGroup group) {

    double rt = -1;
    Peak* peak = group.getPeak(sample);
    if (peak) rt = peak->rt;

    return rt;
}

void AlignmentVizWidget::mouseQCPBar(QMouseEvent *event)
{
    double x = _mw->alignmentVizPlot->xAxis->pixelToCoord(event->pos().x());
    double y = round(_mw->alignmentVizPlot->yAxis->pixelToCoord(event->pos().y()));

    pair<double, double> qcpBarRange = mapXAxis[y];

    if(sampleLabel) {
        _mw->alignmentVizPlot->removeItem(sampleLabel);
    }

    sampleLabel = new QCPItemText(_mw->alignmentVizPlot);
    sampleLabel->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
    sampleLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    sampleLabel->position->setCoords(0.001, 1); // place position at center/top of axis rect

    QString message = "";


    if (qcpBarRange.first <= x && x <= qcpBarRange.second) {
        mzSample* sample = mapSample[y];
        if(sample) {

            message = QString::fromStdString(sample->getSampleName());

        }
    }
    sampleLabel->setText(message);
    sampleLabel->setFont(QFont("Times", 12));
    sampleLabel->setPen(QPen(Qt::black)); 
    _mw->alignmentVizPlot->replot();

}