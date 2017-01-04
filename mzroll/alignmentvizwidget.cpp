#include "alignmentvizwidget.h"

using namespace std;

AlignmentVizWidget::AlignmentVizWidget(MainWindow* mw)
{
    this->_mw = mw;
}

void AlignmentVizWidget::plotGraph(PeakGroup*  group) {

    _mw->alignmentVizPlot->clearPlottables();

    vector<mzSample*> samples = getSamplesFromGroup(group);
    QVector<double> retentionTimes = getRetentionTime(samples, group);

    setXAxis(group);
    QVector<double> ticks = setYAxis(samples);

    Q_FOREACH(double tick, ticks) {

        QCPBars *sample = new QCPBars(_mw->alignmentVizPlot->yAxis, _mw->alignmentVizPlot->xAxis);
        sample->setAntialiased(false);
        sample->setBrush(QColor(111, 9, 176, 150));


        QVector<double> retentionTimeSolidBar; 
        float maxDiff = max(group->medianRt() - group->minRt, group->maxRt - group->medianRt());
        retentionTimeSolidBar << (2*maxDiff)/20;
        QVector<double> tickVector;
        tickVector << tick;

        if (_mw->aligned) {
            QCPBars *shadow = new QCPBars(_mw->alignmentVizPlot->yAxis, _mw->alignmentVizPlot->xAxis);
            shadow->setAntialiased(false);
            shadow->setBrush(QColor(0, 0, 0, 10));
            double shadowBaseValue = retentionTimes[tick] - maxDiff/20;
            double baseValue = shadowBaseValue + _mw->deltaRt[make_pair(group->getName(), samples[tick - 1]->getSampleName())];
            sample->setBaseValue(baseValue);
            shadow->setBaseValue(shadowBaseValue);
            sample->setData(tickVector, retentionTimeSolidBar);
            shadow->setData(tickVector, retentionTimeSolidBar);
        } else {
            double baseValue = retentionTimes[tick] - maxDiff/20;
            sample->setBaseValue(baseValue);
            sample->setData(tickVector, retentionTimeSolidBar);
        }
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
    _mw->alignmentVizPlot->xAxis->setTicker(textTicker);

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