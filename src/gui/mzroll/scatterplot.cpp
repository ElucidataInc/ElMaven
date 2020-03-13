#include "comparesamplesdialog.h"
#include "Compound.h"
#include "eicwidget.h"
#include "gallerywidget.h"
#include "globals.h"
#include "mainwindow.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "pls.h"
#include "pls.h"
#include "Scan.h"
#include "scatterplot.h"
#include "tabledockwidget.h"
#include "utility.h"

ScatterPlot::ScatterPlot(QWidget* w):PlotDockWidget(w,0) { 


	compareSamplesDialog = new CompareSamplesDialog(this);
	compareSamplesDialog->minFoldDiff->setVisible(1);
	compareSamplesDialog->minPValue->setValue(1.0);
	compareSamplesDialog->minIntensity->setValue(0);
        compareSamplesDialog->missingValue->setValue(100);
        compareSamplesDialog->minGoodSamples->setValue(0);
	compareSamplesDialog->hide();
	connect(compareSamplesDialog,SIGNAL(setProgressBar(QString,int,int)), (MainWindow*) w, SLOT(setProgressBar(QString,int,int)));

	connect(scene(),SIGNAL(selectArea(QPointF, QPointF)),this,SLOT(showSelectedGroups(QPointF,QPointF)));

	setObjectName("Scatter Plot");
	setWindowTitle("Scatter Plot");

    showSimilarFlag = false;
    // updated new emun value -Kiran
    plotType = SCATTERPLOT;

    setupToolBar();
    _table = NULL;
    setPeakTable(w);

}

void ScatterPlot::setPeakTable(QWidget* w) {

    _peakTable = new ScatterplotTableDockWidget((MainWindow*) w);
    _peakTable->setVisible(false);
    connect(_peakTable, SIGNAL(visibilityChanged(bool)),
            btnPeakTable, SLOT(setChecked(bool)));
    ((MainWindow*) w)->addDockWidget(Qt::BottomDockWidgetArea, _peakTable, Qt::Horizontal);

}

void ScatterPlot::setupToolBar() { 

    QToolBar *toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24));

    QToolButton *bntResetZoom = new QToolButton(toolBar);
    bntResetZoom->setIcon(QIcon(rsrcPath + "/resetzoom.png"));
    bntResetZoom->setToolTip("Reset Zoom");
    connect(bntResetZoom,SIGNAL(clicked()),SLOT(resetZoom()));

    QToolButton *btnScatter = new QToolButton(toolBar);
    btnScatter->setIcon(QIcon(rsrcPath + "/scatterplot.png"));
    btnScatter->setToolTip("Scatter Plot");
    connect(btnScatter,SIGNAL(clicked()),this,SLOT(setPlotTypeScatter()));

    QToolButton *btnFlower = new QToolButton(toolBar);
    btnFlower->setIcon(QIcon(rsrcPath + "/flowerplot.png"));
    btnFlower->setToolTip("Flower Plot");
    connect(btnFlower,SIGNAL(clicked()),this,SLOT(setPlotTypeFlower()));

    // new feature added - Kiran
    QToolButton *btnPLS = new QToolButton(toolBar);
    btnPLS->setIcon(QIcon(rsrcPath + "/cluster.png"));
    btnPLS->setToolTip("PLS Plot");
    connect(btnPLS,SIGNAL(clicked()),this,SLOT(setPlotTypePLS()));

    // new feature added - Kiran
    QToolButton *btnDelete = new QToolButton(toolBar);
    btnDelete->setIcon(QIcon(rsrcPath + "/delete.png"));
    btnDelete->setToolTip("Delete Selected Groups");
    connect(btnDelete,SIGNAL(clicked()),this,SLOT(deleteGroup()));

    btnPeakTable = new QToolButton(toolBar);
    btnPeakTable->setIcon(QIcon(rsrcPath + "/Peak Table.png"));
    btnPeakTable->setToolTip("Show Peaks Table");
    btnPeakTable->setCheckable(true);
    connect(btnPeakTable,SIGNAL(clicked()),this,SLOT(showPeakTable()));

    // merged with maven776 - Kiran
   /* QToolButton *btnCovariants = new QToolButton(toolBar);
	btnCovariants->setIcon(QIcon(rsrcPath + "/covariants.png"));
	btnCovariants->setToolTip("Highlight Covariants on Click");
    btnCovariants->setCheckable(true); btnCovariants->setChecked(false);
    connect(btnCovariants, SIGNAL(toggled(bool)),this,SLOT(showSimilarOnClick(bool)));
*/
	QToolButton *btnF = new QToolButton(toolBar);
	btnF->setIcon(QIcon(rsrcPath + "/contrast.png"));
	btnF->setToolTip("Show Contrast Groups Dialog");
    connect(btnF, SIGNAL(clicked()),this,SLOT(contrastGroups()));

    QWidget *spacerWidget = new QWidget(this);
	spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	spacerWidget->setVisible(true);

    QToolButton *btnClose = new QToolButton(toolBar);
    btnClose->setIcon(style()->standardIcon(QStyle::SP_DockWidgetCloseButton));
    connect(btnClose, SIGNAL(clicked()), this, SLOT(hide()));

    toolBar->addWidget(bntResetZoom);
    toolBar->addWidget(btnF);
    toolBar->addWidget(btnScatter);
    toolBar->addWidget(btnFlower);
    // new feature added - Kiran
    toolBar->addWidget(btnPLS);
    // new feature added - Kiran
    toolBar->addWidget(btnDelete);
    toolBar->addWidget(btnPeakTable);
    toolBar->addWidget(spacerWidget);
    toolBar->addWidget(btnClose);
    // merged with maven776 - Kiran
    //  toolBar->addWidget(btnCovariants);

    setTitleBarWidget(toolBar);

}


ScatterPlot::~ScatterPlot() { delete(compareSamplesDialog); }


void ScatterPlot::setTable(TableDockWidget* peakTable) {
    if (_table != NULL)
        delete _table;

    MainWindow* mw = (MainWindow*) parent();
    _table = new ScatterplotTableDockWidget(mw);
    QList<PeakGroup *> groups = peakTable->getGroups();
    Q_FOREACH (PeakGroup *group, groups) {
        _table->addPeakGroup(group);
    }
    compareSamplesDialog->setTableWidget(_table);
}

QSet<PeakGroup*> ScatterPlot::getGroupsInRect(QPointF from, QPointF to) {
    // merged with maven776 - Kiran
    QRectF rect(from,to);
    QPainterPath path; path.addRect(rect);
    scene()->setSelectionArea(path);

    QSet<PeakGroup*>selected;
    Q_FOREACH(QGraphicsItem* item, scene()->selectedItems() ) {
            if (QGraphicsEllipseItem *circle = qgraphicsitem_cast<QGraphicsEllipseItem *>(item)) {
            QVariant v = circle->data(0);
            PeakGroup*  groupX =  v.value<PeakGroup*>();
            selected.insert(groupX);

        }
    }
    return selected;
}


void ScatterPlot::showSelectedGroups(QPointF from, QPointF to) { 
    // merged with maven776 - Kiran

        QSet<PeakGroup*>similar = getGroupsInRect(from,to);

        if (similar.size()>0) {
            if(_table) _table->clearFocusedGroups();

            QSetIterator<PeakGroup*> i(similar);
            while (i.hasNext())  {
                PeakGroup* groupX = i.next();
                groupX->isFocused=true;
            }
            if(_table )_table->showFocusedGroups();
        }


}

void ScatterPlot::showSelectedGroupGallery(QPointF from, QPointF to) {
    // merged with maven776 - Kiran

    QSet<PeakGroup*>similar = getGroupsInRect(from,to);
    if (similar.size() > 0 ) {
            vector<mzSlice*>slices;
            QSetIterator<PeakGroup*> i(similar);
            while (i.hasNext())  {
                    PeakGroup* groupX = i.next();
                    mzSlice* slice = new mzSlice(groupX->minMz, groupX->maxMz, groupX->minRt-2, groupX->maxRt+2);
                    slices.push_back(slice);
            }
            MainWindow* mw = (MainWindow*) parent();
            mw->galleryWidget->clear();
            mw->galleryWidget->addEicPlots(slices);
            mzUtils::delete_all(slices);
    }
}


void ScatterPlot::drawScatter(StatisticsVector<float>vecA,StatisticsVector<float>vecB, vector<PeakGroup*>groups) { 
    
    presentGroups.clear();
	if (vecA.size() == 0 || vecB.size() == 0 ) return;
    if (vecA.size() != vecB.size() ) return;

	//get data minimum and mazimum values
	float minX = vecA.minimum();
	float maxX = vecA.maximum();
    scene()->setXDim(minX*0.8,maxX*1.1);

	float minY = vecB.minimum();
	float maxY = vecB.maximum();
    scene()->setYDim(minY*0.8,maxY*1.1);

	//reset zoom if no zoom history is present
    if (zoomHistory.isEmpty()) { scene()->resetZoom(); }

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

        if (R < 1) R=10;
                QGraphicsItem* item = scene()->addEllipse(pos.x()-R/2,pos.y()-R/2,R,R,pen,brush);
                item->setFlag(QGraphicsItem::ItemIsSelectable);
                item->setToolTip( tr("%1,%2").arg(x).arg(y) );

                if(group) {
                    QString groupName = group->getSrmId().c_str(); //to implement
                    presentGroups.push_back(group);
                    item->setData(0, QVariant::fromValue(group));
                    item->setToolTip( tr("(%1,%2) <br>%3 m/z: %4<br>rt: %5<br>pvalue: %6")
                                      .arg(x)
                                      .arg(y)
                                      .arg(groupName)
                                      .arg(group->meanMz)
                                      .arg(group->meanRt)
                                      .arg(group->changePValue));
                }
	 }
}

void ScatterPlot::drawPLS(vector<PeakGroup*>groups) {
        // new feature added - Kiran
        qDebug() << "ScatterPlot::drawPLS()";
        presentGroups.clear();
        sort(groups.begin(), groups.end());
        StatisticsVector<float>foldChanges;
        StatisticsVector<float>pValues;

		MainWindow* mw = (MainWindow*) parent();
		PeakGroup::QType qtype = mw->getUserQuantType();
		vector<mzSample*> vsamples = mw->getVisibleSamples();

		QMap<QString,int>setNumericIds;
		QMultiMap<mzSample*,QString>sampleSetnameMap;
		for(unsigned int i=0; i < vsamples.size(); i++ ) {
			mzSample* sample = vsamples[i];
            QRegExp splitStr(";");
			QString qname( sample->getSetName().c_str());
			qname=qname.simplified();
			QList<QString> names = qname.split(splitStr);
			Q_FOREACH(QString name, names) { 
				name=name.simplified(); 
				if (name.isEmpty()) continue;
				sampleSetnameMap.insert(sample,name);
                if (!setNumericIds.contains(name)) setNumericIds[name] = setNumericIds.size()-1;
			}
		}

        qDebug() << setNumericIds;

		if (setNumericIds.size() == 0 ) return;
        int nSets = setNumericIds.size();

        Mat2D X; X = Mat2D::Zero(groups.size(),vsamples.size());
        Mat2D Y; Y=  Mat2D::Zero(groups.size(),setNumericIds.size());

		for(int i=0; i < groups.size(); i++ ) {
        	 vector<float>values=groups[i]->getOrderedIntensityVector(vsamples,qtype);
             float sum=0;
             for(int j=0; j < values.size(); j++ ) {  sum += values[j]; }
             if(sum==0) continue;
             float meanValue=sum/values.size();

             for(int j=0; j < values.size(); j++ ) {  X(i,j)=(values[j]/sum)-(meanValue/sum);}

             for(int j=0; j < values.size(); j++ ) {
                    mzSample* sample = vsamples[j];
					QList<QString>setnames = sampleSetnameMap.values(sample);
					Q_FOREACH(QString setname, setnames ) { 
							int setNumber = setNumericIds[setname];
                            Y(i,setNumber) += (values[j]/sum)-(1.0f/nSets);
					}	
			 }
        }

        //cerr << Y;

        //X = colwise_z_scores( X );
        //Y = colwise_z_scores( X );

        PLS_Model plsm;
        int nobj  = X.rows();
        int npred = X.cols();
        int nresp = Y.cols();
        int ncomp = 3;
        plsm.initialize(npred, nresp, ncomp);
        plsm.plsr(X,Y,KERNEL_TYPE1);

        // A is number of components to use
        for (int A = 1; A<=ncomp; A++) {
            // How well did we do with this many components?
            cerr << A << " components\t";
            cerr << "explained variance: " << plsm.explained_variance(X, Y, A);
            //cerr << "root mean squared error of prediction (RMSEP):" << plsm.rmsep(X, Y, A) << endl;
            cerr << " SSE: " << plsm.SSE(X,Y,A) <<  endl;
        }


        cerr << "Validation (PRESS):\n";
        cerr << plsm.loo_validation(X, Y, PRESS) << endl;

        cerr << "Optimal number of components:\t" << plsm.loo_optimal_num_components(X,Y) << endl;

        cerr << "Fitted Values:\n";
        //cerr << plsm.fitted_values(X,ncomp);

        cerr << "Regression coeeffientss:\n";
        cerr << plsm.coefficients(ncomp).transpose();
        cout << plsm.T;

        //convert to reals
        Mat2D T = plsm.T.real();


        foldChanges = vector<float>(groups.size(),0);
        pValues = vector<float>(groups.size(),0);

        for(unsigned int i=0; i < groups.size(); i++) {
            foldChanges[i]=(float) T(i,0);
            pValues[i]=(float) T(i,1);
        }

        //get data minimum and mazimum values
        float minX = foldChanges.minimum();
        float maxX = foldChanges.maximum();
         scene()->setXDim(minX*0.8,maxX*1.1);

        float minY = pValues.minimum();
        float maxY = pValues.maximum();
        scene()->setYDim(minY*0.8,maxY*1.1);

        //reset zoom if no zoom history is present
        if (zoomHistory.isEmpty()) { scene()->resetZoom(); }

        // plot is in log scale
        scene()->setLogTransformed(false,false);

        //draw x and y axes
        drawAxes();
        scene()->showVLine(true);
        scene()->showHLine(true);
        scene()->showXLabel("Fold Change");
        scene()->showYLabel("Significance");

        QPen pen(Qt::black);
        QBrush brush(QColor::fromRgbF(0.9,0,0,0.3));

        //get a visible limits of the plot
        QPointF xlimits = scene()->getZoomXDim();
        QPointF ylimits = scene()->getZoomYDim();

    //qDebug() << " drawScatter: " << xlimits << " " << ylimits << endl;

        for (int i=0; i<groups.size(); i++ ) {
                float x = foldChanges[i];
                float y = pValues[i];
                if (x < xlimits.x() || x > xlimits.y() ) continue;
                if (y < ylimits.x() || y > ylimits.y() ) continue;

                PeakGroup* group=NULL;
                if (i < groups.size()) group=groups[i];

                double r=1.0;
                if (group) r = group->changePValue;
                if (r<=0) r=0.001;
                if (r> 1) r=1;
                float alpha = 1-pow(r,0.2); //pvalue^0.2

                if (foldChanges[i] >0 )  {
                        brush = QBrush(QColor::fromRgbF(alpha,0,0,alpha));
                }  else if (foldChanges[i]<0 ) {
                        brush = QBrush(QColor::fromRgbF(0.0,0,alpha,alpha));
                }

                QPointF pos = scene()->plotToMap(x,y);

                //radius
                float R=1.0-(((float)i)/groups.size());
                R = (R+0.25)*10;

                QGraphicsItem* item = scene()->addEllipse(pos.x()-R/2,pos.y()-R/2,R,R,pen,brush);
                item->setFlag(QGraphicsItem::ItemIsSelectable);
                item->setToolTip( tr("%1,%2,%3").arg(x).arg(y) );

                if(group) {
                    presentGroups.push_back(group);
                    item->setData(0, QVariant::fromValue(group));
                    item->setToolTip( tr("m/z:%1 rt:%2 pvalue:%3").arg(group->meanMz).arg(group->meanRt).arg(group->changePValue));
                }
         }
}

void ScatterPlot::drawFlower(vector<PeakGroup*>groups) {

        presentGroups.clear();
        sort(groups.begin(), groups.end());

        StatisticsVector<float>foldChanges;
        StatisticsVector<float>pValues;

        for(int i=0;i<groups.size();i++) {
            PeakGroup* g = groups[i];
            float x = g->changeFoldRatio; 
            float y = g->changePValue;    y <=0 ? y=-log10(0.001) : y=-log10(y);
            //cerr << g->changeFoldRatio << " " << x << "    " << g->changePValue << " " << y << std::endl;
            foldChanges.push_back(x);
            pValues.push_back(y);
        }

        if (foldChanges.size() == 0 || pValues.size() == 0 ) return;

        //get data minimum and mazimum values
        float minX = foldChanges.minimum();
        float maxX = foldChanges.maximum();
         scene()->setXDim(minX*0.8,maxX*1.1);

        float minY = pValues.minimum();
        float maxY = pValues.maximum();
        scene()->setYDim(minY*0.8,maxY*1.1);

        //reset zoom if no zoom history is present
        if (zoomHistory.isEmpty()) { scene()->resetZoom(); }

        // plot is in log scale
        scene()->setLogTransformed(false,false);

        //draw x and y axes
        drawAxes();
        scene()->showVLine(true);
        scene()->showHLine(true);
        scene()->showXLabel("Fold Change");
        scene()->showYLabel("Significance");

        QPen pen(Qt::black);
        QBrush brush(QColor::fromRgbF(0.9,0,0,0.3));

        //get a visible limits of the plot
        QPointF xlimits = scene()->getZoomXDim();
        QPointF ylimits = scene()->getZoomYDim();

    //qDebug() << " drawScatter: " << xlimits << " " << ylimits << endl;

        for (int i=0; i<groups.size(); i++ ) {
                float x = foldChanges[i];
                float y = pValues[i];
                if (x < xlimits.x() || x > xlimits.y() ) continue;
                if (y < ylimits.x() || y > ylimits.y() ) continue;

                PeakGroup* group=NULL;
                if (i < groups.size()) group=groups[i];

                double r=1.0;
                if (group) r = group->changePValue;
                if (r<=0) r=0.001;
                if (r> 1) r=1;
                float alpha = 1-pow(r,0.2); //pvalue^0.2

                if (foldChanges[i] >0 )  {
                        brush = QBrush(QColor::fromRgbF(alpha,0,0,alpha));
                }  else if (foldChanges[i]<0 ) {
                        brush = QBrush(QColor::fromRgbF(0.0,0,alpha,alpha));
                }

                QPointF pos = scene()->plotToMap(x,y);

                //radius
                float R=1.0-(((float)i)/groups.size());
                R = (R+0.25)*10;

                QGraphicsItem* item = scene()->addEllipse(pos.x()-R/2,pos.y()-R/2,R,R,pen,brush);
                item->setFlag(QGraphicsItem::ItemIsSelectable);
                item->setToolTip( tr("%1,%2").arg(x).arg(y) );

                if(group) {
                    presentGroups.push_back(group);
                    item->setData(0, QVariant::fromValue(group));
                    item->setToolTip( tr("m/z:%1 rt:%2 pvalue:%3").arg(group->meanMz).arg(group->meanRt).arg(group->changePValue));
                }
         }
}



void ScatterPlot::draw() { 
    qDebug() << "ScatterPlot::draw()";

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

    MainWindow* mw = (MainWindow*) parent();
    PeakGroup::QType qtype = mw->getUserQuantType();


    vector<mzSample*>set1 = compareSamplesDialog->getSampleSet1();
    vector<mzSample*>set2 = compareSamplesDialog->getSampleSet2();


    if (set1.size() == 0) return;
    if (set2.size() == 0) return;

	for(int i=0; i < allgroups.size(); i++ ) {
		//filter display
		PeakGroup* group = allgroups[i];
		if(group->changePValue > _maxPvalue) continue;
		if(group->maxIntensity < _minIntensity ) continue;
		// updated abs to fabs  - Kiran
                if(fabs(group->changeFoldRatio) < _minRatio ) continue;
		if(group->goodPeakCount < _minGoodSamples ) continue;
		
		StatisticsVector<float>groupA;
        StatisticsVector<float>groupB;

        groupA = group->getOrderedIntensityVector(set1,qtype);
        groupB = group->getOrderedIntensityVector(set2,qtype);

        /*
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
        */

		float mA=groupA.mean();
		float mB=groupB.mean();
		meanA.push_back(mA);
		meanB.push_back(mB);
		goodgroups.push_back(group);
	}
        // Updated with new enum values  - Kiran
        if(plotType == FLOWRPLOT) drawFlower(goodgroups);
        else if(plotType == PLSPLOT) drawPLS(goodgroups);
        else if(plotType == SCATTERPLOT) drawScatter(meanA,meanB,goodgroups);
}

void ScatterPlot::showSimilar(PeakGroup* group) {
	if (!showSimilarFlag) return;
	if (!_table) return;

    QList<PeakGroup*>allgroups = _table->getGroups();
    if (!group->clusterId) _table->clusterGroups();

    QSet<PeakGroup*>similar;

    for(int i=0; i < allgroups.size(); i++ ) {
        if ( group->clusterId ) {
            if ( allgroups[i]->clusterId == group->clusterId ) {
                similar.insert(allgroups[i]);
            }
        }
    }

	Q_FOREACH(QGraphicsItem* item, view()->items() ) {
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
}

void ScatterPlot::contrastGroups() {
    if (!_table) return;
    if (!compareSamplesDialog) return;
    if (!isVisible()) { setVisible(true); }

    MainWindow* mw = (MainWindow*) parent();
    compareSamplesDialog->setQuantitationType(mw->getUserQuantType());

    compareSamplesDialog->show();
    mw->scatterDockWidget->setVisible(true);
    mw->scatterDockWidget->raise();
}

// new Feature added - Kiran
void ScatterPlot::keyPressEvent(QKeyEvent *e ) {
    qDebug() << "ScatterPlot:keypressed";

   if (e->key() == Qt::Key_Delete ) {
        deleteGroup();
    }
    PlotDockWidget::keyPressEvent(e);
}

void ScatterPlot::deleteGroup() {
    qDebug() << "ScatterPlot:deleteGroup()";
    Q_FOREACH(QGraphicsItem* item, scene()->selectedItems() ) {
            if (QGraphicsEllipseItem *circle = qgraphicsitem_cast<QGraphicsEllipseItem *>(item)) {
            QVariant v = circle->data(0);
            PeakGroup*  groupX =  v.value<PeakGroup*>();
            if (groupX and _table) {
                _table->deleteGroup(groupX);
                 circle->hide();
            }
        }
    }

   MainWindow* mw = (MainWindow*) parent();
   mw->getEicWidget()->replotForced();
}

void ScatterPlot::showPeakTable() {

    _peakTable->deleteAll();
    for(int i=0 ;i < presentGroups.size(); i++) {
        _peakTable->addPeakGroup(presentGroups[i]);
    }
    _peakTable->showAllGroups();
    _peakTable->setVisible(btnPeakTable->isChecked());

}
