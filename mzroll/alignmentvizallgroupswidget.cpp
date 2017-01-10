#include "alignmentvizallgroupswidget.h"

using namespace std;

AlignmentVizAllGroupsWidget::AlignmentVizAllGroupsWidget(MainWindow* mw) {
    this->_mw = mw;
}

void AlignmentVizAllGroupsWidget::plotGraph(QList<PeakGroup> allgroups) {

    _mw->alignmentVizAllGroupsPlot->clearPlottables();
    setXAxis();
    setYAxis();

    map<mzSample*, QVector<double> > retentionTime;
    map<mzSample*, QVector<double> > retentionTimeDeviation;

    Q_FOREACH(PeakGroup grp, allgroups) {
        for(unsigned int i=0;i<grp.getPeaks().size();i++) {
            Peak peak = grp.getPeaks().at(i);
            mzSample* sample = peak.getSample();
            retentionTime[sample] << peak.rt*60;
            retentionTimeDeviation[sample] << peak.rt*60 - grp.medianRt()*60;
        }
    }

    vector<mzSample*> samples = _mw->getSamples();

    QPen pen;

    Q_FOREACH(mzSample* sample, samples) {

        QMap<double, double> map;
        for(int i = 0; i < retentionTime[sample].size(); ++i)
            map.insert(retentionTime[sample][i], retentionTimeDeviation[sample][i]);

        QList<double> retentionTimel = map.keys();
        QList<double> retentionTimeDeviationl = map.values();

        retentionTime[sample] = QVector<double>::fromList(retentionTimel);
        retentionTimeDeviation[sample] = QVector<double>::fromList(retentionTimeDeviationl);


        QColor color = _mw->projectDockWidget->storeSampleColors[sample];

        _mw->alignmentVizAllGroupsPlot->addGraph();
        pen.setColor(color);
        pen.setWidth(1);
        _mw->alignmentVizAllGroupsPlot->graph()->setPen(pen);
        _mw->alignmentVizAllGroupsPlot->graph()->setLineStyle(QCPGraph::lsLine);
        _mw->alignmentVizAllGroupsPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
        _mw->alignmentVizAllGroupsPlot->graph()->setData(retentionTime[sample], retentionTimeDeviation[sample]);
        _mw->alignmentVizAllGroupsPlot->graph()->rescaleAxes(true);

    }
}

void AlignmentVizAllGroupsWidget::setXAxis() {

    _mw->alignmentVizAllGroupsPlot->xAxis->setTicks(true);
    _mw->alignmentVizAllGroupsPlot->xAxis->setVisible(true);
    _mw->alignmentVizAllGroupsPlot->xAxis->setLabel("Retention Time");
    _mw->alignmentVizAllGroupsPlot->xAxis->setRange(0, 100);
}

void AlignmentVizAllGroupsWidget::setYAxis() {

    _mw->alignmentVizAllGroupsPlot->yAxis->setTicks(true);
    _mw->alignmentVizAllGroupsPlot->yAxis->setVisible(true);
    _mw->alignmentVizAllGroupsPlot->yAxis->setLabel("Retention Time Deviation");
    _mw->alignmentVizAllGroupsPlot->yAxis->setRange(-0.5, 0.5);
}