#include "alignmentvizallgroupswidget.h"

using namespace std;

AlignmentVizAllGroupsWidget::AlignmentVizAllGroupsWidget(MainWindow* mw) {
    this->_mw = mw;
}

void AlignmentVizAllGroupsWidget::plotGraph(QList<PeakGroup> allgroups) {

    connect(_mw->alignmentVizAllGroupsPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
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
        _mw->alignmentVizAllGroupsPlot->graph()->setName(QString::fromStdString(sample->getSampleName()));
        _mw->alignmentVizAllGroupsPlot->graph()->setLineStyle(QCPGraph::lsLine);
        _mw->alignmentVizAllGroupsPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
        _mw->alignmentVizAllGroupsPlot->graph()->setData(retentionTime[sample], retentionTimeDeviation[sample]);
        _mw->alignmentVizAllGroupsPlot->graph()->rescaleAxes(true);
        _mw->alignmentVizAllGroupsPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    }

    setLegend();
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

void AlignmentVizAllGroupsWidget::setLegend() {

    QPen pen;
    QColor color = QColor(0, 0, 0);
    pen.setColor(color);

    _mw->alignmentVizAllGroupsPlot->legend->setVisible(true);
    _mw->alignmentVizAllGroupsPlot->legend->setBrush(QColor(255, 255, 255, 100));
    _mw->alignmentVizAllGroupsPlot->legend->setBorderPen(pen);
    QFont legendFont("Times", 10);
    legendFont.setPointSize(10);
    _mw->alignmentVizAllGroupsPlot->legend->setFont(legendFont);
    _mw->alignmentVizAllGroupsPlot->legend->setSelectedFont(legendFont);
    _mw->alignmentVizAllGroupsPlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

}

void AlignmentVizAllGroupsWidget::selectionChanged()

{
    /*
    normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
    the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
    and the axis base line together. However, the axis label shall be selectable individually.
    
    The selection state of the left and right axes shall be synchronized as well as the state of the
    bottom and top axes.
    
    Further, we want to synchronize the selection of the graphs with the selection state of the respective
    legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
    or on its legend item.
    */

    makeAllGraphsVisible();

    // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (_mw->alignmentVizAllGroupsPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || _mw->alignmentVizAllGroupsPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        _mw->alignmentVizAllGroupsPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || _mw->alignmentVizAllGroupsPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        _mw->alignmentVizAllGroupsPlot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        _mw->alignmentVizAllGroupsPlot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (_mw->alignmentVizAllGroupsPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || _mw->alignmentVizAllGroupsPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        _mw->alignmentVizAllGroupsPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || _mw->alignmentVizAllGroupsPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
    _mw->alignmentVizAllGroupsPlot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    _mw->alignmentVizAllGroupsPlot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }

    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i=0; i<_mw->alignmentVizAllGroupsPlot->graphCount(); ++i)
    {
        QCPGraph *graph = _mw->alignmentVizAllGroupsPlot->graph(i);
        QCPPlottableLegendItem *item = _mw->alignmentVizAllGroupsPlot->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())
        {
        item->setSelected(true);
        graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        }
        else {
            _mw->alignmentVizAllGroupsPlot->graph(i)->setVisible(false);
        }
    }
}

void AlignmentVizAllGroupsWidget::makeAllGraphsVisible() {

    for (int i=0; i<_mw->alignmentVizAllGroupsPlot->graphCount(); ++i) {
        _mw->alignmentVizAllGroupsPlot->graph(i)->setVisible(true);
    }
}