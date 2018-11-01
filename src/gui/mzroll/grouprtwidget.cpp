#include "grouprtwidget.h"
#include <QDockWidget>
#include <QToolBar>
using namespace std;

GroupRtWidget::GroupRtWidget(MainWindow* mw, QDockWidget* dockWidget):
    _mw(mw),
    _dockWidget(dockWidget)
{

    QToolBar *toolBar = new QToolBar(_dockWidget);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setLayoutDirection(Qt::RightToLeft);

    QToolButton *btnHide = new QToolButton(toolBar);
    btnHide->setIcon(_dockWidget->style()->standardIcon(QStyle::SP_DialogCloseButton));
    connect(btnHide, SIGNAL(clicked()), _dockWidget, SLOT(hide()));
    toolBar->addWidget(btnHide);

    dockWidget->setTitleBarWidget(toolBar);

    setXAxis();
    setYAxis();
}

void GroupRtWidget::plotGraph(PeakGroup*  group) {

    if (!_mw->groupRtDockWidget->isVisible()) return;
    currentDisplayedGroup=group;
    intialSetup();
    PeakGroup groupUnalignedShadowed = *group;

    refRtLine(groupUnalignedShadowed);

    PeakGroup groupAlignedBrightened = getNewGroup(groupUnalignedShadowed);

    drawMessageBox(groupAlignedBrightened, groupUnalignedShadowed);

    plotIndividualGraph(groupAlignedBrightened, 100);

    plotIndividualGraph(groupUnalignedShadowed, 40);

    float rtRange = groupUnalignedShadowed.medianRt();
    vector<mzSample*> samples = getSamplesFromGroup(groupUnalignedShadowed);

    _mw->groupRtVizPlot->yAxis->setRange(0, samples.size() + 1);
    _mw->groupRtVizPlot->xAxis->setRange(rtRange-1, rtRange+1);
    _mw->groupRtVizPlot->replot();
}
void GroupRtWidget::updateGraph(){
    if(currentDisplayedGroup) plotGraph(currentDisplayedGroup);
}
void GroupRtWidget::intialSetup() {
    _mw->groupRtVizPlot->clearPlottables();

    if (_mw) {
        if (_mw->groupRtVizPlot) {
            if(textLabel) {
                _mw->groupRtVizPlot->removeItem(textLabel);
            }
            _mw->groupRtVizPlot->replot();
        }
    }

}

void GroupRtWidget::setXAxis() {

    _mw->groupRtVizPlot->xAxis->setTicks(true);
    _mw->groupRtVizPlot->xAxis->setSubTicks(true);
    _mw->groupRtVizPlot->xAxis->setVisible(true);
    _mw->groupRtVizPlot->xAxis->setLabel("Retention Time");
}

void GroupRtWidget::setYAxis() {

    _mw->groupRtVizPlot->yAxis->setVisible(true);
    _mw->groupRtVizPlot->yAxis->setLabel("Samples");

}

void GroupRtWidget::refRtLine(PeakGroup  group) {

    double refRt = getRefRt(group);

    QPen pen;
    pen.setStyle(Qt::DotLine);
    pen.setColor(Qt::red);
    pen.setWidth(2);

    _mw->groupRtVizPlot->addGraph();
    _mw->groupRtVizPlot->graph()->setPen(pen);

    QVector<double> x, y;

    vector<mzSample*> samples = getSamplesFromGroup(group);
    x << refRt << refRt;
    y << 0 << (samples.size() + 1);

    _mw->groupRtVizPlot->graph()->setLineStyle(QCPGraph::lsLine);
    _mw->groupRtVizPlot->graph()->setData(x, y);

    _mw->groupRtVizPlot->replot();

}

double GroupRtWidget::getRefRt(PeakGroup group) {

    double refRt;
    if (group.hasCompoundLink()) {
        refRt = group.compound->expectedRt;
    } else {
        refRt = group.medianRt();
    }

    return refRt;
}

void GroupRtWidget::drawMessageBox(PeakGroup newGroup, PeakGroup group) {

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

    textLabel = new QCPItemText(_mw->groupRtVizPlot);
    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel->position->setCoords(0.001, 0); // place position at center/top of axis rect
    textLabel->setText(message);
    textLabel->setFont(QFont("Times", 12)); 
    textLabel->setPen(QPen(Qt::black)); 

}

float GroupRtWidget::calculateRsquare(PeakGroup newGroup,PeakGroup oldGroup) {

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

PeakGroup GroupRtWidget::getNewGroup(PeakGroup group) {

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

float GroupRtWidget::checkGroupEquality(PeakGroup grup1, PeakGroup grup2) {
    float R2Sq = FLT_MAX;
    if (abs(grup1.medianRt() - grup2.medianRt()) < 0.5) {
        R2Sq = (pow(grup1.meanMz - grup2.meanMz, 2) +
                    pow(grup1.maxMz - grup2.maxMz, 2) + 
                        pow(grup1.minMz - grup2.minMz, 2) + 
                                pow(grup1.medianRt() - grup2.medianRt(), 2));
    }
    return R2Sq;

}

void GroupRtWidget::plotIndividualGraph(PeakGroup group, int  alpha) {

    vector<mzSample*> samples = getSamplesFromGroup(group);
    // QVector<double> retentionTimes = getRetentionTime(samples, group);
    connect(_mw->groupRtVizPlot, SIGNAL(mouseMove(QMouseEvent* )) , this, SLOT(mouseQCPBar(QMouseEvent* )));

    float widthOfBar = getWidthOfBar(group);

    int i = 1;
    Q_FOREACH(mzSample* sample, samples) {
        /** choose color of corresponding sample, older(with unaligned rt) group will have 40% of brightness and new group
         * will be 100% bright (alpha =40,100).
         */
        QColor color = QColor(255*sample->color[0],255*sample->color[1],255*sample->color[2] ,alpha* sample->color[3]);
        bar = new QCPBars(_mw->groupRtVizPlot->yAxis, _mw->groupRtVizPlot->xAxis);
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

vector<mzSample*> GroupRtWidget::getSamplesFromGroup(PeakGroup group) {

    vector<Peak>& peaks = group.getPeaks();
    vector<mzSample*> samples;
    for(unsigned int i=0; i < peaks.size(); i++ ) {
        mzSample* s = peaks[i].getSample();
        samples.push_back(s);
    }
    sort (samples.begin(), samples.end(),mzSample::compSampleOrder);
    reverse(samples.begin(),samples.end());
    /** Alignment visualization plot will show peaks from different samples in corresponding sample color.
     * ordering of peaks( and their color) start from top to down as sample are shown. If we don't reverse sample here,
     * reverse will happen which is not so consistent.
     */
    return samples;
}

float GroupRtWidget::getWidthOfBar(PeakGroup group) {

    float maxDiff, widthOfBar;
    
    maxDiff = max(getRefRt(group) - group.minRt, group.maxRt - getRefRt(group));

    widthOfBar = 0.01; //hard-coded on purpose

    return widthOfBar;
}


double GroupRtWidget::getRetentionTime(mzSample* sample, PeakGroup group) {

    double rt = -1;
    Peak* peak = group.getPeak(sample);
    if (peak) rt = peak->rt;

    return rt;
}

void GroupRtWidget::mouseQCPBar(QMouseEvent *event)
{
    double x = _mw->groupRtVizPlot->xAxis->pixelToCoord(event->pos().x());
    double y = round(_mw->groupRtVizPlot->yAxis->pixelToCoord(event->pos().y()));

    pair<double, double> qcpBarRange = mapXAxis[y];

    if(sampleLabel) {
        _mw->groupRtVizPlot->removeItem(sampleLabel);
    }

    sampleLabel = new QCPItemText(_mw->groupRtVizPlot);
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
    _mw->groupRtVizPlot->replot();

}
