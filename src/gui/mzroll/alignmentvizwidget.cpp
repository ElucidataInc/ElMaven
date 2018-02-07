#include "alignmentvizwidget.h"

using namespace std;

AlignmentVizWidget::AlignmentVizWidget(MainWindow* mw)
{
    this->_mw = mw;
}

void AlignmentVizWidget::plotGraph(PeakGroup*  group) {

    if (!_mw->alignmentVizDockWidget->isVisible()) return;
    currentDisplayedGroup=group;
    intialSetup();
    PeakGroup grp = *group;

    refRtLine(grp);

    PeakGroup newGroup = getNewGroup(grp);

    drawMessageBox(newGroup, grp);

    plotIndividualGraph(newGroup, 100);

    plotIndividualGraph(grp, 40);

    float rtRange = grp.medianRt();
    vector<mzSample*> samples = getSamplesFromGroup(grp);

    _mw->alignmentVizPlot->yAxis->setRange(0, samples.size() + 1);
    _mw->alignmentVizPlot->xAxis->setRange(rtRange-1, rtRange+1);
    _mw->alignmentVizPlot->replot();
}
void AlignmentVizWidget::updateGraph(){
    if(currentDisplayedGroup) plotGraph(currentDisplayedGroup);
}
void AlignmentVizWidget::intialSetup() {
    _mw->alignmentVizPlot->clearPlottables();

    if (_mw) {
        if (_mw->alignmentVizPlot) {
            if(textLabel) {
                _mw->alignmentVizPlot->removeItem(textLabel);
            }
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
    pen.setWidth(2);

    _mw->alignmentVizPlot->addGraph();
    _mw->alignmentVizPlot->graph()->setPen(pen);

    QVector<double> x, y;

    vector<mzSample*> samples = getSamplesFromGroup(group);
    x << refRt << refRt;
    y << 0 << (samples.size() + 1);

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

    float newGroupR2 = calculateRsquare(newGroup,group);
    // float groupR2 = calculateRsquare(group);
    double refRt = getRefRt(group);

    QString message;

    message = "R-squared = " + QString::number(newGroupR2, 'f', 5);
    // message += "\ncurrent R2 = " + QString::number(newGroupR2, 'f', 5);
 
    if(group.hasCompoundLink()) {
        message += "\nCompound RT = " + QString::number(refRt, 'f', 5);
    } else {
        message += "\nMedian RT = " + QString::number(refRt, 'f', 5);
    }

    textLabel = new QCPItemText(_mw->alignmentVizPlot);
    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel->position->setCoords(0.001, 0); // place position at center/top of axis rect
    textLabel->setText(message);
    textLabel->setFont(QFont("Times", 12)); 
    textLabel->setPen(QPen(Qt::black)); 

}

float AlignmentVizWidget::calculateRsquare(PeakGroup newGroup,PeakGroup oldGroup) {

    float SSres = 0;
    float SStot=0;
    float mean=0;

    vector<mzSample*> oldSamples = getSamplesFromGroup(oldGroup);
    vector<mzSample*> newSamples = getSamplesFromGroup(newGroup);
    vector<float> newRts,oldRts;
    for(int i=0 ; i < oldSamples.size() ; ++i){
        float rtNew = getRetentionTime(newSamples[i], newGroup);
        float rtOld = getRetentionTime(oldSamples[i], oldGroup);
        if(rtNew == -1 || rtOld == -1) continue;
        oldRts.push_back(rtOld);
        newRts.push_back(rtNew);
    }

    for(int i=0 ; i < oldRts.size() ; ++i){
        mean += oldRts[i];
    }
    mean = 1.0*mean/oldRts.size();
    
    for( int i = 0 ; i < oldRts.size() ; ++i ){
        SSres += pow(newRts[i]-mean,2);
        SStot += pow(oldRts[i]-mean,2);
    }

    float RSquared = 1-SSres/SStot;

    return RSquared;

}

PeakGroup AlignmentVizWidget::getNewGroup(PeakGroup group) {

    PeakGroup newGroup;

    bool groupFound = false;


    float min = FLT_MAX;

    Q_FOREACH(PeakGroup currentGroup, currentGroups) {

        float Rsquare = checkGroupEquality(currentGroup, group);
        if (min > Rsquare) {
            min = Rsquare;
            newGroup = currentGroup;
            groupFound = true;
        }
    }

    if(!groupFound) {
        newGroup = group;
    }

    return newGroup;

}

float AlignmentVizWidget::checkGroupEquality(PeakGroup grup1, PeakGroup grup2) {
    float R2Sq = FLT_MAX;
    if (abs(grup1.medianRt() - grup2.medianRt()) < 0.5) {
        R2Sq = (pow(grup1.meanMz - grup2.meanMz, 2) +
                    pow(grup1.maxMz - grup2.maxMz, 2) + 
                        pow(grup1.minMz - grup2.minMz, 2) + 
                                pow(grup1.medianRt() - grup2.medianRt(), 2));
    }
    return R2Sq;

}

void AlignmentVizWidget::plotIndividualGraph(PeakGroup group, int  alpha) {

    vector<mzSample*> samples = getSamplesFromGroup(group);
    // QVector<double> retentionTimes = getRetentionTime(samples, group);
    connect(_mw->alignmentVizPlot, SIGNAL(mouseMove(QMouseEvent* )) , this, SLOT(mouseQCPBar(QMouseEvent* )));

    float widthOfBar = getWidthOfBar(group);

    int i = 1;
    Q_FOREACH(mzSample* sample, samples) {
        QColor color = QColor(255*sample->color[0],255*sample->color[1],255*sample->color[2] ,alpha* sample->color[3]);
        bar = new QCPBars(_mw->alignmentVizPlot->yAxis, _mw->alignmentVizPlot->xAxis);
        bar->setAntialiased(false);
        QPen pen;
        pen.setColor(color);
        bar->setPen(pen);
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
    sort (samples.begin(), samples.end(),mzSample::compSampleOrder);
    reverse(samples.begin(),samples.end());
    return samples;
}

float AlignmentVizWidget::getWidthOfBar(PeakGroup group) {

    float maxDiff, widthOfBar;
    
    maxDiff = max(getRefRt(group) - group.minRt, group.maxRt - getRefRt(group));

    widthOfBar = 0.01; //hard-coded on purpose

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
    sampleLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignRight);
    sampleLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    sampleLabel->position->setCoords(0.99, 0.001); // place position at center/top of axis rect

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