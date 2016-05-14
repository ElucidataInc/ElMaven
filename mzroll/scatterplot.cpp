#include "scatterplot.h"

ScatterPlot::ScatterPlot(QWidget* w):PlotDockWidget(w,0) {


    compareSamplesDialog = new CompareSamplesDialog(this);
    compareSamplesDialog->minFoldDiff->setVisible(1);
    compareSamplesDialog->minPValue->setValue(1.0);
    compareSamplesDialog->minIntensity->setValue(0);
    compareSamplesDialog->missingValue->setValue(100);
    compareSamplesDialog->minGoodSamples->setValue(0);
    compareSamplesDialog->hide();
    connect(compareSamplesDialog,SIGNAL(setProgressBar(QString,int,int)), (MainWindow*) w, SLOT(setProgressBar(QString,int,int)));

    //add buttons to toolbar
    QToolButton *btnF = new QToolButton(toolBar);
    btnF->setIcon(QIcon(rsrcPath + "/contrast.png"));
    btnF->setToolTip("Contrast Groups");
    connect(btnF, SIGNAL(clicked()),this,SLOT(contrastGroups()));
    toolBar->addWidget(btnF);

    //add options to context menu
    showSimilarOptions = contextMenu->addAction("Show Similar Groups");
    showSimilarOptions->setCheckable(true);
    showSimilarOptions->setChecked(showSimilarFlag);

    connect(showSimilarOptions, SIGNAL(toggled(bool)), this, SLOT(replot()));
    connect(scene(),SIGNAL(selectArea(QPointF, QPointF)),this,SLOT(showSelectedGroups(QPointF,QPointF)));

    setObjectName("Scatter Plot");
    setWindowTitle("Scatter Plot");


    setTable(NULL);
}


ScatterPlot::~ScatterPlot() {
    delete(compareSamplesDialog);
}


void ScatterPlot::setTable(TableDockWidget* t) {
    _table=t;
    compareSamplesDialog->setTableWidget(t);
}

void ScatterPlot::showSelectedGroups(QPointF from, QPointF to) {
    QRectF rect(from,to);
    QPainterPath path;
    path.addRect(rect);
    scene()->setSelectionArea(path);
    qDebug() << "selectArea " << rect << " " << scene()->selectedItems().size();

    QSet<PeakGroup*>similar;
    foreach(QGraphicsItem* item, scene()->selectedItems() ) {
        if (QGraphicsEllipseItem *circle = qgraphicsitem_cast<QGraphicsEllipseItem *>(item)) {
            QVariant v = circle->data(0);
            PeakGroup*  groupX =  v.value<PeakGroup*>();
            similar.insert(groupX);
        }
    }

    if (similar.size() > 0 ) {
        vector<mzSlice*>_similar;
        QSetIterator<PeakGroup*> i(similar);
        while (i.hasNext())  {
            PeakGroup* groupX = i.next();
            mzSlice* slice = new mzSlice(groupX->minMz, groupX->maxMz, groupX->minRt-2, groupX->maxRt+2);
            _similar.push_back(slice);
        }

        MainWindow* mw = (MainWindow*) parent();
        mw->galleryWidget->clear();
        mw->galleryWidget->addEicPlots(_similar);
        delete_all(_similar);
    }
}


void ScatterPlot::drawScatter(StatisticsVector<float>vecA,StatisticsVector<float>vecB, vector<PeakGroup*>groups) {


    if (vecA.size() == 0 || vecB.size() == 0 ) return;
    if (vecA.size() != vecB.size() ) return;

    //get data minimum and mazimum values
    float minX = vecA.minimum();
    float maxX = vecA.maximum();
    scene()->setXDim(minX*0.8,maxX*1.1);

    float minY = vecB.minimum();
    float maxY = vecB.maximum();
    scene()->setYDim(minX*0.8,maxY*1.1);

    //reset zoom if no zoom history is present
    if (zoomHistory.isEmpty()) {
        scene()->resetZoom();
    }

    // plot is in log scale
    scene()->setLogTransformed(true,true);

    //draw x and y axes
    drawAxes();
    scene()->showVLine(true);
    scene()->showHLine(true);
    scene()->showXLabel("Sample Set 1");
    scene()->showYLabel("Sample Set 2");

    QPen pen(Qt::black);
    QBrush brush(QColor::fromRgbF(0.9,0,0,0.3));

    //get a visible limits of the plot
    QPointF xlimits = scene()->getZoomXDim();
    QPointF ylimits = scene()->getZoomYDim();

    //qDebug() << " drawScatter: " << xlimits << " " << ylimits << endl;

    for (int i=0; i<vecA.size(); i++ ) {
        float x = vecA[i];
        float y = vecB[i];
        if (x < xlimits.x() || x > xlimits.y() ) continue;
        if (y < ylimits.x() || y > ylimits.y() ) continue;

        PeakGroup* group=NULL;
        if (i < groups.size()) group=groups[i];

        double r=1.0;
        if (group) r=group->changePValue;
        if (r<=0) r=0.001;
        if (r> 1) r=1;
        float alpha = 1-pow(r,0.2); //pvalue^0.2

        if (vecA[i] > vecB[i] )  {
            brush = QBrush(QColor::fromRgbF(alpha,0,0,alpha));
        }  else {
            brush = QBrush(QColor::fromRgbF(0.0,0,alpha,alpha));
        }

        QPointF pos = scene()->plotToMap(x,y);
        float R=alpha*20;
        if (group) R=group->changeFoldRatio;
        if (R < 1) R=1;
        if (R > 10) R=10;
        R = (1-1.0/R)*15;
        if (group->compound) {
            qDebug() << QString(group->compound->name.c_str()) << "\t" << pos.x() << " " << pos.y() << r << " " << R;
        }


        if (R < 1) R=10;
        QGraphicsItem* item = scene()->addEllipse(pos.x()-R/2,pos.y()-R/2,R,R,pen,brush);
        item->setFlag(QGraphicsItem::ItemIsSelectable);
        item->setToolTip( tr("%1,%2").arg(x).arg(y) );

        if(group) {
            item->setData(0, QVariant::fromValue(group));
            item->setToolTip( tr("m/z:%1 rt:%2 pvalue:%3").arg(group->meanMz).arg(group->meanRt).arg(group->changePValue));
        }
    }
}



void ScatterPlot::draw() {

    scene()->clear();
    /*
    StatisticsVector<float>meanA;
    StatisticsVector<float>meanB;
    vector<PeakGroup*> goodgroups;

    meanA.push_back(1);
    meanA.push_back(1);
    meanA.push_back(2);
    meanA.push_back(3);

    meanB.push_back(1);
    meanB.push_back(5);
    meanB.push_back(10);
    meanB.push_back(20);

    drawScatter(meanA,meanB,goodgroups);
    return;
    */

    if (!_table) return;

    QList<PeakGroup*>allgroups = _table->getGroups();
    if (allgroups.size() == 0) return;

    float _maxPvalue  = 0.01;
    float _minIntensity   = 1;
    float _minRatio   = 1;
    int   _minGoodSamples = 1;

    if (compareSamplesDialog) {
        _minIntensity = compareSamplesDialog->minIntensity->value();
        _maxPvalue =   compareSamplesDialog->minPValue->value();
        _minRatio =   compareSamplesDialog->minFoldDiff->value();
        _minGoodSamples = compareSamplesDialog->minGoodSamples->value();

    }

    StatisticsVector<float>meanA;
    StatisticsVector<float>meanB;
    vector<PeakGroup*> goodgroups;

    cerr << "Groups=" << allgroups.size() << endl;
    cerr << " minIntensity=" << _minIntensity << endl;
    cerr << " minRatio=" << _minRatio<< endl;
    cerr << " maxPvalue=" << _maxPvalue<< endl;

    vector<mzSample*>set1 = compareSamplesDialog->getSampleSet1();
    vector<mzSample*>set2 = compareSamplesDialog->getSampleSet2();
    if (set1.size() == 0) return;
    if (set2.size() == 0) return;

    for(int i=0; i < allgroups.size(); i++ ) {
        //filter display
        PeakGroup* group = allgroups[i];
        if(group->changePValue > _maxPvalue) continue;
        if(group->maxIntensity < _minIntensity ) continue;
        if(group->changeFoldRatio < _minRatio ) continue;
        if(group->goodPeakCount < _minGoodSamples ) continue;

        StatisticsVector<float>groupA;
        StatisticsVector<float>groupB;

        for(int s=0; s< set1.size(); s++ ) {
            for(int j=0; j < group->peaks.size(); j++ ) {
                if(group->peaks[j].getSample() == set1[s]) groupA.push_back(group->peaks[j].peakAreaTop);
            }
        }

        for(int s=0; s< set2.size(); s++ ) {
            for(int j=0; j < group->peaks.size(); j++ ) {
                if(group->peaks[j].getSample() == set2[s]) groupB.push_back(group->peaks[j].peakAreaTop);
            }
        }

        float mA=groupA.mean();
        float mB=groupB.mean();
        meanA.push_back(mA);
        meanB.push_back(mB);
        goodgroups.push_back(group);
    }

    drawScatter(meanA,meanB,goodgroups);
}

void ScatterPlot::showSimilar(PeakGroup* group) {
    if (!showSimilarOptions->isChecked()) return;
    if (!_table) return;

    QList<PeakGroup*>allgroups = _table->getGroups();

    vector<mzSample*>sampleSet;
    mzSample* largestSample=NULL;
    double maxIntensity=0;
    for(int i=0; i < group->peakCount(); i++ ) {
        mzSample* sample = group->peaks[i].getSample();
        sampleSet.push_back(sample);
        if ( group->peaks[i].peakIntensity > maxIntensity ) largestSample=sample;
    }

    if (sampleSet.size() == 0 ) return;
    if (!largestSample) return;

    MainWindow* mw = (MainWindow*) parentWidget();
    QSettings* settings = mw->getSettings();
    double maxRtDiff =  settings->value("maxRtDiff").value<double>();
    double minSampleCorrelation =  settings->value("minSampleCorrelation").value<double>();
    double minRtCorrelation =  settings->value("minRtCorrelation").value<double>();
    double ppm	= mw->getUserPPM();

    QSet<PeakGroup*>similar;
    vector<float>peakIntensityA = group->getOrderedIntensityVector(sampleSet,PeakGroup::AreaTop);

    for(int i=0; i < allgroups.size(); i++ ) {
        if ( group->metaGroupId ) {
            if ( allgroups[i]->metaGroupId == group->metaGroupId ) {
                similar.insert(allgroups[i]);
            }
        } else {
            if(abs(allgroups[i]->meanRt - group->meanRt) < maxRtDiff ) {
                vector<float>peakIntensityB = allgroups[i]->getOrderedIntensityVector(sampleSet,PeakGroup::AreaTop);
                if (correlation(peakIntensityA,peakIntensityB) > minSampleCorrelation ) {
                    if(largestSample->correlation(group->meanMz,allgroups[i]->meanMz,ppm,group->minRt,group->maxRt) > minRtCorrelation ) {
                        similar.insert(allgroups[i]);
                    }
                }
            }
        }
    }

    foreach(QGraphicsItem* item, view()->items() ) {
        if (QGraphicsEllipseItem *circle = qgraphicsitem_cast<QGraphicsEllipseItem *>(item)) {
            QVariant v = circle->data(0);
            PeakGroup*  groupX =  v.value<PeakGroup*>();
            if ( groupX == group ) {
                circle->setPen(QPen(Qt::yellow,3));
                circle->setZValue(100);
            } else if (similar.size() > 0 && similar.contains(groupX)) {
                circle->setPen(QPen(Qt::yellow,3));
                circle->setZValue(100);
            } else {
                circle->setPen(QPen(Qt::black,0));
                //	circle->setZValue(0);
            }
        }
    }

    if (similar.size() > 0 ) {
        vector<mzSlice*>_similar;
        QSetIterator<PeakGroup*> i(similar);
        while (i.hasNext())  {
            PeakGroup* groupX = i.next();
            mzSlice* slice = new mzSlice(groupX->minMz, groupX->maxMz, group->minRt, group->maxRt);
            _similar.push_back(slice);
        }

        MainWindow* mw = (MainWindow*) parent();
        mw->galleryWidget->clear();
        mw->galleryWidget->addEicPlots(_similar);
        delete_all(_similar);
    }

}

void ScatterPlot::contrastGroups() {
    if (!_table) return;
    if (!compareSamplesDialog) return;
    if (!isVisible()) setVisible(true);
    compareSamplesDialog->setTableWidget(_table);
    compareSamplesDialog->show();
}

