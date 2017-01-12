#include "alignmentvizallgroupswidget.h"

using namespace std;

AlignmentVizAllGroupsWidget::AlignmentVizAllGroupsWidget(MainWindow* mw) {
    this->_mw = mw;
}

void AlignmentVizAllGroupsWidget::plotGraph(QList<PeakGroup> allgroups) {

    connect(_mw->alignmentVizAllGroupsPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    connect(_mw->alignmentVizAllGroupsPlot, SIGNAL(selectionChangedByUser()), this, SLOT(resetGraph()));
    _mw->alignmentVizAllGroupsPlot->clearPlottables();
    setXAxis();
    setYAxis();
    setupReset();

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

        vector<float> xy_points;

        for (unsigned int i = 0; i< retentionTime[sample].size(); i++) {
            xy_points.push_back(retentionTime[sample][i]);
            xy_points.push_back(retentionTimeDeviation[sample][i]);
        } 

        vector<float> vertices;
        vertices = populate_bezier(xy_points);


        QVector<double> rt;
        QVector<double> rtDeviation;


        for (unsigned int j =0; j< vertices.size(); j = j+2) {
            rt << vertices[j];
            rtDeviation << vertices[j+1];
        }
        
        _mw->alignmentVizAllGroupsPlot->graph()->setData(rt, rtDeviation);
        _mw->alignmentVizAllGroupsPlot->graph()->rescaleAxes(true);
        _mw->alignmentVizAllGroupsPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    }

    setLegend();
    _mw->alignmentVizAllGroupsPlot->replot();
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

void AlignmentVizAllGroupsWidget::setupReset() {
    _mw->alignmentVizAllGroupsPlot->addGraph();
    QPen pen;
    QColor color = QColor(0,0,0);
    pen.setColor(color);
    pen.setWidth(1);
    _mw->alignmentVizAllGroupsPlot->graph()->setPen(pen);
    _mw->alignmentVizAllGroupsPlot->graph()->setName("Reset");
    _mw->alignmentVizAllGroupsPlot->graph()->setLineStyle(QCPGraph::lsLine);

}

void AlignmentVizAllGroupsWidget::resetGraph() {

    QCPGraph *graph = _mw->alignmentVizAllGroupsPlot->graph(0);
    QCPPlottableLegendItem *item = _mw->alignmentVizAllGroupsPlot->legend->itemWithPlottable(graph);
    if (item->selected())
    {
        makeAllGraphsVisible();
    }
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
    if (legendSelected()) {
        makeAllGraphsVisible();
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
}


bool AlignmentVizAllGroupsWidget::legendSelected() {

    bool itemSelected = false;


    for (int i=0; i<_mw->alignmentVizAllGroupsPlot->graphCount(); ++i)
    {
        QCPGraph *graph = _mw->alignmentVizAllGroupsPlot->graph(i);
        QCPPlottableLegendItem *item = _mw->alignmentVizAllGroupsPlot->legend->itemWithPlottable(graph);
        if (item->selected())
        {
            itemSelected = true;
        }
    }
    return itemSelected;
}

void AlignmentVizAllGroupsWidget::makeAllGraphsVisible() {

    for (int i=0; i<_mw->alignmentVizAllGroupsPlot->graphCount(); ++i) {
        _mw->alignmentVizAllGroupsPlot->graph(i)->setVisible(true);
    }
}


vector<float> AlignmentVizAllGroupsWidget::populate_bezier(vector<float> xy_points) {

    vector<float> bezier_vector;
    vector<float> bezier_points;
    vector<float> vertices;

    if(xy_points.size() > 6) {

        bezier_vector.push_back(xy_points[0]);
        bezier_vector.push_back(xy_points[1]);
        for(int i = 2; i < xy_points.size() - 4; i = i + 2) {
            // add a point
            bezier_vector.push_back(xy_points[i]);
            bezier_vector.push_back(xy_points[i + 1]);

            // add mid point with the next point
            bezier_vector.push_back(mid_point(xy_points[i],
                xy_points[i + 2]));
            bezier_vector.push_back(mid_point(xy_points[i + 1],
                xy_points[i + 3]));
        }


        bezier_vector.push_back(xy_points[xy_points.size() - 4]);
        bezier_vector.push_back(xy_points[xy_points.size() - 3]);

        bezier_vector.push_back(xy_points[xy_points.size() - 2]);
        bezier_vector.push_back(xy_points[xy_points.size() - 1]);

        for(int i = 0; i < bezier_vector.size() - 2; i = i + 4) {
            // append bezier curve points to the vertices array
            bezier_points = generate_bezier(
                bezier_vector[i],
                bezier_vector[i + 1],
                bezier_vector[i + 2],
                bezier_vector[i + 3],
                bezier_vector[i + 4],
                bezier_vector[i + 5]
            );
            vertices.insert(vertices.end(), bezier_points.begin(), bezier_points.end());
        }
    } else if(xy_points.size() == 6) {
        // 3 points
        vertices = generate_bezier(
            xy_points[0],
            xy_points[1],
            xy_points[2],
            xy_points[3],
            xy_points[4],
            xy_points[5]
        );
    } else if(xy_points.size() == 4) {
        // 2 points, draw a line
        vertices.push_back(xy_points[0]);
        vertices.push_back(xy_points[1]);

        vertices.push_back(xy_points[2]);
        vertices.push_back(xy_points[3]);
    }

    return vertices;

}

float AlignmentVizAllGroupsWidget::mid_point(float a, float b) {
    
    return (a+b)/2;
}

/*
 * function to generate bezier points using 3 points
 */
vector<float> AlignmentVizAllGroupsWidget::generate_bezier(float x1, float y1, float x2, float y2, float x3, float y3) {
    vector<float> bezier_points;

    for(int t = 0; t <= 100; t++) {
        bezier_points.push_back(quadratic_bezier_point(t / 100.0f, x1, x2, x3));
        bezier_points.push_back(quadratic_bezier_point(t / 100.0f, y1, y2, y3));
    }

    return bezier_points;
}

/*
 *
 */
 float AlignmentVizAllGroupsWidget::quadratic_bezier_point(float t, float point_0, float point_1, float point_2) {
     return powf((1 - t), 2) * point_0 + 2 * (1 - t) * t * point_1 + powf(t, 2) * point_2;
 }
