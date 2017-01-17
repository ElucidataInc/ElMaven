#include "alignmentvizwidget.h"

using namespace std;

AlignmentVizWidget::AlignmentVizWidget(MainWindow* mw)
{
    this->_mw = mw;
}

void AlignmentVizWidget::plotGraph(PeakGroup*  group) {

    _mw->alignmentVizPlot->clearPlottables();

    PeakGroup* shadowGrp;

    bool groupFound = false;

    for (unsigned int ii =0 ; ii <_mw->mavenParameters->allgroups.size(); ii++) {
        PeakGroup* previousGrp = &_mw->mavenParameters->allgroups[ii];
        if (previousGrp->meanMz == group->meanMz && previousGrp->maxMz == group->maxMz && previousGrp->minMz == group->minMz) {
            shadowGrp = previousGrp;
            groupFound = true;
            break;
        }
    }

    if(!groupFound) {
        shadowGrp = group;
    }

    vector<mzSample*> samples = getSamplesFromGroup(group);
    vector<mzSample*> shadowSamples = getSamplesFromGroup(shadowGrp);

    QVector<double> retentionTimes = getRetentionTime(samples, group);
    QVector<double> retentionTimesShadow = getRetentionTime(shadowSamples, shadowGrp);

    setXAxis(group);
    QVector<double> ticks = setYAxis(samples);

    Q_FOREACH(double tick, ticks) {

        QCPBars *sample = new QCPBars(_mw->alignmentVizPlot->yAxis, _mw->alignmentVizPlot->xAxis);
        sample->setAntialiased(false);
        sample->setPen(QPen(QColor(111, 9, 176).lighter(170)));
        sample->setBrush(QColor(111, 9, 176, 50));

        QCPBars *shadow = new QCPBars(_mw->alignmentVizPlot->yAxis, _mw->alignmentVizPlot->xAxis);
        shadow->setAntialiased(false);
        shadow->setPen(QPen(QColor(111, 9, 176).lighter(170)));
        shadow->setBrush(QColor(0, 0, 0, 100));

        float maxDiff = max(group->medianRt() - group->minRt, group->maxRt - group->medianRt());
        float shadowMaxDiff = max(shadowGrp->medianRt() - shadowGrp->minRt, shadowGrp->maxRt - shadowGrp->medianRt());

        double baseValue = retentionTimes[tick] - maxDiff/20;
        double shadowBaseValue = retentionTimesShadow[tick] - shadowMaxDiff/20;


        QVector<double> retentionTimeSolidBar;
        QVector<double> retentionTimeShadowBar;

        retentionTimeSolidBar << (2*maxDiff)/20;
        retentionTimeShadowBar << (2*shadowMaxDiff)/20;

        sample->setBaseValue(baseValue);
        shadow->setBaseValue(shadowBaseValue);

        QVector<double> tickVector;
        tickVector << tick;
        sample->setData(tickVector, retentionTimeSolidBar);
        shadow->setData(tickVector, retentionTimeShadowBar);

    }


    _mw->alignmentVizPlot->replot();
}

vector<mzSample*> AlignmentVizWidget::getSamplesFromGroup(PeakGroup* group) {

    vector<Peak>& peaks = group->getPeaks();
    vector<mzSample*> samples;
    for(unsigned int i=0; i < peaks.size(); i++ ) {
        mzSample* s = peaks[i].getSample();
        samples.push_back(s);
    }
    sort (samples.begin(), samples.end());
    return samples;
}

QVector<double> AlignmentVizWidget::getRetentionTime(vector<mzSample*> samples, PeakGroup* group) {

    QVector<double> retentionTimes;
    Q_FOREACH(mzSample* sample, samples) {
        Peak* peak = group->getPeak(sample);
        if (peak) retentionTimes << peak->rt;
    }
    return retentionTimes;
}

void AlignmentVizWidget::setXAxis(PeakGroup* group) {

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTick(group->medianRt(), QString::number(group->medianRt()));
    _mw->alignmentVizPlot->xAxis->setTicks(true);
    _mw->alignmentVizPlot->xAxis->setSubTicks(true);

    _mw->alignmentVizPlot->xAxis->setVisible(true);

    float maxDiff = max(group->medianRt() - group->minRt, group->maxRt - group->medianRt());
    float lowerRange =  group->medianRt() - 2*(maxDiff);
    float upperRange = group->medianRt() + 2*(maxDiff);

    _mw->alignmentVizPlot->xAxis->setRange(lowerRange, upperRange);
    _mw->alignmentVizPlot->xAxis->setLabel("Retention Time");
}

QVector<double> AlignmentVizWidget::setYAxis(vector<mzSample*> samples) {

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    QVector<double> ticks;
    int i=1;

    Q_FOREACH(mzSample* sample, samples) {
        textTicker->addTick(i, QString::fromStdString(sample->getSampleName()));
        ticks << i;
        i++;
    }

    _mw->alignmentVizPlot->yAxis->setTicks(true);
    _mw->alignmentVizPlot->yAxis->setRange(-1, samples.size() + 1);
    _mw->alignmentVizPlot->yAxis->setTicker(textTicker);
    _mw->alignmentVizPlot->yAxis->setTickLabelRotation(-20);
    _mw->alignmentVizPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    return ticks;
}