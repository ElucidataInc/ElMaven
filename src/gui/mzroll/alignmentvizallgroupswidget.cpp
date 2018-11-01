#include "alignmentvizallgroupswidget.h"
#include <QDockWidget>
#include <QToolBar>

using namespace std;

AlignmentVizAllGroupsWidget::AlignmentVizAllGroupsWidget(MainWindow* mw, QDockWidget* dockWidget):
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

void AlignmentVizAllGroupsWidget::replotGraph() {
    plotGraph(saveGroups);
}

void AlignmentVizAllGroupsWidget::plotGraph(QList<PeakGroup> allgroups) {

    if (!_mw->alignmentVizAllGroupsDockWidget->isVisible()) return;
    saveGroups = allgroups;

    _mw->alignmentVizAllGroupsPlot->clearPlottables();

    pairPeakGroup.clear();
    retentionTime.clear();
    retentionTimeDeviation.clear();

    Q_FOREACH(PeakGroup grp, allgroups) {
        for(unsigned int i=0;i<grp.getPeaks().size();i++) {
            Peak peak = grp.getPeaks().at(i);
            mzSample* sample = peak.getSample();
            retentionTime[sample] << peak.rt;
            retentionTimeDeviation[sample] << peak.rt - grp.medianRt();
            pairPeakGroup[make_pair(peak.rt, peak.rt - grp.medianRt())] = grp;
        }
    }

    vector<mzSample*> samples;
    
    Q_FOREACH(mzSample* sample, _mw->getSamples()) {
        if(sample->isSelected) {
            samples.push_back(sample);
        }
    }


    QPen pen;

    unsigned int i = 0;

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
        _mw->alignmentVizAllGroupsPlot->graph(i)->setPen(pen);
        _mw->alignmentVizAllGroupsPlot->graph(i)->setName(QString::fromStdString(sample->getSampleName()));
        _mw->alignmentVizAllGroupsPlot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 11));
        _mw->alignmentVizAllGroupsPlot->graph(i)->setLineStyle(QCPGraph::lsLine);

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
        
        _mw->alignmentVizAllGroupsPlot->graph(i)->setData(retentionTime[sample], retentionTimeDeviation[sample]);
        _mw->alignmentVizAllGroupsPlot->graph(i)->rescaleAxes(true);
        _mw->alignmentVizAllGroupsPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |  QCP::iSelectPlottables);
        _mw->alignmentVizAllGroupsPlot->graph(i)->setSelectable(QCP::stSingleData );
        connect(_mw->alignmentVizAllGroupsPlot->graph(i), SIGNAL(selectionChanged(bool)), this, SLOT(displayGroup()));
        i++;
    }

    _mw->alignmentVizAllGroupsPlot->rescaleAxes();
    _mw->alignmentVizAllGroupsPlot->replot();
}

void AlignmentVizAllGroupsWidget::displayGroup() {

    vector<mzSample*> samples;
    
    Q_FOREACH(mzSample* sample, _mw->getSamples()) {
        if(sample->isSelected) {
            samples.push_back(sample);
        }
    }

    QCPDataRange dataRange;
    int index;
    PeakGroup grp;

    for (unsigned int i =0; i<samples.size(); i++) {
        mzSample* sample = samples[i];

        QCPDataSelection selection = _mw->alignmentVizAllGroupsPlot->graph(i)->selection();

        if (selection.isEmpty()) {
            continue;
        } else {
            dataRange =  selection.dataRange();
            if (dataRange.isValid() && dataRange.begin() > 0) {
                index = dataRange.begin();
                float x = retentionTime[sample][index];
                float y = retentionTimeDeviation[sample][index];
                pair<float, float> xy = make_pair(x, y);
                grp = pairPeakGroup[xy];
                if (_mw != NULL) {
                    _mw->setPeakGroup(&grp);
                    _mw->getEicWidget()->replotForced();
                    //_mw->rconsoleDockWidget->updateStatus();
                }
            }
        }
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
