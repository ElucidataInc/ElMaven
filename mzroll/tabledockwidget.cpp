#include "tabledockwidget.h";
#include "csvreports.h";

TableDockWidget::TableDockWidget(MainWindow* mw, QString title, int numColms) {
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFloating(false);
    _mainwindow = mw;
    setObjectName(title);

    numColms=11;
    treeWidget=new QTreeWidget(this);
    treeWidget->setSortingEnabled(false);
    treeWidget->setColumnCount(numColms);

    setWidget(treeWidget);
    setWindowTitle(title);
    setupPeakTable();

    traindialog = new TrainDialog(this);

    connect(traindialog->saveButton,SIGNAL(clicked(bool)),SLOT(saveModel()));
    connect(traindialog->trainButton,SIGNAL(clicked(bool)),SLOT(Train()));
    connect(treeWidget, SIGNAL(itemSelectionChanged()),SLOT(showSelectedGroup()));

    QToolBar *toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);

    QToolButton *btnGroupCSV = new QToolButton(toolBar);
    btnGroupCSV->setIcon(QIcon(rsrcPath + "/exportcsv.png"));
    btnGroupCSV->setToolTip(tr("Export Groups To SpreadSheet (.csv) "));
    connect(btnGroupCSV, SIGNAL(clicked()), SLOT(exportGroupsToSpreadsheet()));

    QToolButton *btnPeaksCSV = new QToolButton(toolBar);
    btnPeaksCSV->setIcon(QIcon(rsrcPath + "/exportcsv.png"));
    btnPeaksCSV->setToolTip(tr("Export Peaks To SpreadSheet (.csv) "));
    connect(btnPeaksCSV, SIGNAL(clicked()), SLOT(exportPeaksToSpreadsheet()));


    //QToolButton *btnHeatmap = new QToolButton(toolBar);
    //btnHeatmap->setIcon(QIcon(rsrcPath + "/heatmap.png"));
    //btnHeatmap->setToolTip("Show HeatMap");
    //connect(btnHeatmap, SIGNAL(clicked()), SLOT(showHeatMap()));

    QToolButton *btnGallery = new QToolButton(toolBar);
    btnGallery->setIcon(QIcon(rsrcPath + "/gallery.png"));
    btnGallery->setToolTip("Show Gallery");
    connect(btnGallery, SIGNAL(clicked()), SLOT(showGallery()));

    QToolButton *btnScatter = new QToolButton(toolBar);
    btnScatter->setIcon(QIcon(rsrcPath + "/scatterplot.png"));
    btnScatter->setToolTip("Show ScatterPlot");
    connect(btnScatter, SIGNAL(clicked()), SLOT(showScatterPlot()));

    QToolButton *btnCluster = new QToolButton(toolBar);
    btnCluster->setIcon(QIcon(rsrcPath + "/cluster.png"));
    btnCluster->setToolTip("Cluster Groups");
    connect(btnCluster, SIGNAL(clicked()), SLOT(clusterGroups()));

    /*
    	QToolButton *btnFilter = new QToolButton(toolBar);
    	btnFilter->setIcon(QIcon(rsrcPath + "/filter.png"));
    	btnFilter->setToolTip("Filter Groups");
    	connect(btnFilter, SIGNAL(clicked()), SLOT(showFiltersDialog()));
    	*/

    QToolButton *btnTrain = new QToolButton(toolBar);
    btnTrain->setIcon(QIcon(rsrcPath + "/train.png"));
    btnTrain->setToolTip("Train Neural Net");
    connect(btnTrain,SIGNAL(clicked()),traindialog,SLOT(show()));

    QToolButton *btnXML = new QToolButton(toolBar);
    btnXML->setIcon(QIcon(rsrcPath + "/exportxml.png"));
    btnXML->setToolTip("Save Peaks");
    connect(btnXML, SIGNAL(clicked()), SLOT(savePeakTable()));

    QToolButton *btnLoad = new QToolButton(toolBar);
    btnLoad->setIcon(QIcon(rsrcPath + "/fileopen.png"));
    btnLoad->setToolTip("Load Peaks");
    connect(btnLoad, SIGNAL(clicked()), SLOT(loadPeakTable()));

    QToolButton *btnGood = new QToolButton(toolBar);
    btnGood->setIcon(QIcon(rsrcPath + "/markgood.png"));
    btnGood->setToolTip("Mark Group as Good");
    connect(btnGood, SIGNAL(clicked()), SLOT(markGroupGood()));

    QToolButton *btnBad = new QToolButton(toolBar);
    btnBad->setIcon(QIcon(rsrcPath + "/markbad.png"));
    btnBad->setToolTip("Mark Good as Bad");
    connect(btnBad, SIGNAL(clicked()), SLOT(markGroupBad()));

    QToolButton *btnHeatmapelete = new QToolButton(toolBar);
    btnHeatmapelete->setIcon(QIcon(rsrcPath + "/delete.png"));
    btnHeatmapelete->setToolTip("Delete Group");
    connect(btnHeatmapelete, SIGNAL(clicked()), this, SLOT(deleteGroup()));

    QToolButton *btnPDF = new QToolButton(toolBar);
    btnPDF->setIcon(QIcon(rsrcPath + "/PDF.png"));
    btnPDF->setToolTip("Generate PDF Report");
    connect(btnPDF, SIGNAL(clicked()), this, SLOT(printPdfReport()));

    QToolButton *btnX = new QToolButton(toolBar);
    btnX->setIcon(QIcon(rsrcPath + "/hide.png"));
    connect(btnX, SIGNAL(clicked()),SLOT(hide()));

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    toolBar->addWidget(btnGood);
    toolBar->addWidget(btnBad);
    toolBar->addWidget(btnTrain);
    toolBar->addWidget(btnHeatmapelete);
    toolBar->addSeparator();

    //toolBar->addWidget(btnHeatmap);
    toolBar->addWidget(btnScatter);
    toolBar->addWidget(btnGallery);
    toolBar->addWidget(btnCluster);
    //toolBar->addWidget(btnFilter);

    toolBar->addSeparator();
    toolBar->addWidget(btnPDF);
    toolBar->addWidget(btnGroupCSV);
    toolBar->addWidget(btnPeaksCSV);

    toolBar->addSeparator();
    toolBar->addWidget(btnXML);
    toolBar->addWidget(btnLoad);

    toolBar->addWidget(spacer);
    toolBar->addWidget(btnX);

    setTitleBarWidget(toolBar);
    setupFiltersDialog();

}

TableDockWidget::~TableDockWidget() {
    if(traindialog != NULL) delete traindialog;
}

void TableDockWidget::sortBy(int col) {
    treeWidget->sortByColumn(col,Qt::AscendingOrder);
}

void TableDockWidget::setupPeakTable() {
    QStringList colNames;
    //colNames << "Class";
    //colNames << "#";
    colNames << "ID";
    colNames << "m/z";
    colNames << "rt";

    /*
    	colNames << "peakAreaFractional";
    	colNames << "noNoiseFraction";
    	colNames << "symmetry";
    	colNames << "width";
    	colNames << "sym/width";
    	colNames << "signalBaselineRatio";
    	colNames << "overlap";
    	colNames << "gaussFitR2";
    	colNames << "Quality";
    	colNames  << "Label";
    	colNames  << "FP/FN";
    	colNames  << "changeFoldRatio";
    	*/

    colNames << "#peaks";
    colNames << "#good";
    colNames << "Max Width";
    colNames << "Max Intensity";
    colNames << "Max S/N";
    colNames << "Max Quality";
    colNames << "Ratio Change";
    colNames << "P-value";
    treeWidget->setHeaderLabels(colNames);
    treeWidget->setSortingEnabled(true);

}

void TableDockWidget::updateTable() {
    int N=treeWidget->topLevelItemCount();

    for(int i=0; i < N; i++ ) {
        updateItem(treeWidget->topLevelItem(i));
    }
    updateStatus();
}

void TableDockWidget::updateItem(QTreeWidgetItem* item) {
    QVariant v = item->data(0,Qt::UserRole);
    PeakGroup*  group =  v.value<PeakGroup*>();
    if ( group == NULL ) return;

    //score peak quality
    Classifier* clsf = _mainwindow->getClassifier();
    if (clsf != NULL) {
        clsf->classify(group);
        group->updateQuality();
        item->setText(8,QString::number(group->maxQuality,'f',2));
        item->setText(0,groupTagString(group));
    }

    if ( group->changeFoldRatio >= 0 ) {
        item->setText(9,QString::number(group->changeFoldRatio, 'f', 3));
        item->setText(10,QString::number(group->changePValue,    'f', 6));
    }

    int good=0;
    int bad=0;
    int total=group->peakCount();
    for(int i=0; i < group->peakCount(); i++ ) {
        group->peaks[i].quality > 0.5 ? good++ : bad++;
    }

    QBrush brush=Qt::NoBrush;
    if (good>0 && group->label == 'b' ) {
        float incorrectFraction= ((float) good)/total;
        brush  = QBrush(QColor::fromRgbF(0.8,0,0,incorrectFraction));
    } else if(bad>0 && group->label == 'g') {
        float incorrectFraction= ((float) bad)/total;
        brush  = QBrush(QColor::fromRgbF(0.8,0,0,incorrectFraction));
    }
    item->setBackground(0,brush);

    if (group->label == 'g' ) item->setIcon(0,QIcon(":/images/good.png"));
    if (group->label == 'b' ) item->setIcon(0,QIcon(":/images/bad.png"));

    if (filtersDialog->isVisible()) {
        float minG = sliders["GoodPeakCount"]->minBoundValue();
        float maxG = sliders["GoodPeakCount"]->maxBoundValue();

        if ( group->goodPeakCount < minG  || group->goodPeakCount > maxG ) {
            item->setHidden(true);
        } else {
            item->setHidden(false);
        }
    }

    /*
    item->setBackground(0,QBrush(color));
    item->setText(13,QString(group->label));
    item->setText(14,fpfn);
    */
}

QString TableDockWidget::groupTagString(PeakGroup* group) {
    if (!group) return QString();
    QString tag(group->tagString.c_str());
    if (group->compound) tag = QString(group->compound->name.c_str());
    if (! group->tagString.empty()) tag += " | " + QString(group->tagString.c_str());
    if (! group->srmId.empty()) tag +=  " | " + QString(group->srmId.c_str());
    if ( tag.isEmpty() ) tag = QString::number(group->groupId);
    return tag;
}

void TableDockWidget::addRow(PeakGroup* group, QTreeWidgetItem* root) {

    if (group == NULL) return;
    if (group->peakCount() == 0 ) return;
    if (group->meanMz <= 0 ) return;

    Peak& p = group->peaks[0];

    NumericTreeWidgetItem *item = new NumericTreeWidgetItem(root,0);

    //item->setText(0,QString::number(group->metaGroupId));
    item->setText(0,groupTagString(group));
    item->setText(1,QString::number(group->meanMz, 'f', 4));
    item->setText(2,QString::number(group->meanRt, 'f', 2));

    if (group->label == 'g' ) item->setIcon(0,QIcon(":/images/good.png"));
    if (group->label == 'b' ) item->setIcon(0,QIcon(":/images/bad.png"));

    /*
    item->setText(4,QString::number(p.peakAreaFractional, 'f', 2));
    item->setText(5,QString::number(p.noNoiseFraction, 'f', 2));
    item->setText(6,QString::number(p.symmetry));
    item->setText(7,QString::number(p.width));
    item->setText(8,QString::number(p.symmetry/(p.width+1)*log2(p.width+1),'f',2));
    item->setText(9,QString::number(p.signalBaselineRatio, 'f', 3));
    item->setText(10,QString::number(p.groupOverlapFrac, 'f', 2));
    item->setText(11,QString::number(p.gaussFitR2*100, 'f', 2));
        item->setText(12,QString::number(p.quality, 'f', 2));
    item->setText(13,QString(group->label));
    item->setText(14,QString::number(group->changeFoldRatio, 'f', 2));
    */

    item->setText(3,QString::number(group->sampleCount));
    item->setText(4,QString::number(group->goodPeakCount));
    item->setText(5,QString::number(group->maxNoNoiseObs));
    item->setText(6,QString::number(group->maxIntensity,'g',2));
    item->setText(7,QString::number(group->maxSignalBaselineRatio,'f',0));
    item->setText(8,QString::number(group->maxQuality,'f',2));

    if ( group->changeFoldRatio != 0 ) {
        item->setText(9,QString::number(group->changeFoldRatio, 'f', 2));
        item->setText(10,QString::number(group->changePValue,    'e', 4));
    }

    item->setData(0,Qt::UserRole,QVariant::fromValue(group));
    if ( root == NULL ) treeWidget->addTopLevelItem(item);


    updateItem(item);

    if ( group->childCount() > 0 ) {
        for( int i=0; i < group->childCount(); i++ ) addRow(&(group->children[i]), item);
    }
}

bool TableDockWidget::hasPeakGroup(PeakGroup* group) {
    for(int i=0; i < allgroups.size(); i++ ) {
        if ( &allgroups[i] == group ) return true;
        if ((double) std::abs(group->meanMz - allgroups[i].meanMz) < 1e-5 && (double)
                std::abs(group->meanRt-allgroups[i].meanRt) < 1e-5) {
            return true;
        }
    }
    return false;
}

PeakGroup* TableDockWidget::addPeakGroup(PeakGroup* group) {
    if (group != NULL ) {
        allgroups.push_back(*group);
        if ( allgroups.size() > 0 ) {
            PeakGroup& g = allgroups[ allgroups.size()-1 ];
            g.groupId = allgroups.size();
            return &g;
        }
    }

    return NULL;
    //if ( _mainwindow->heatmap ) _mainwindow->heatmap->replot();
    //	if ( _mainwindow->treemap ) _mainwindow->treemap->replot();
}

QList<PeakGroup*> TableDockWidget::getGroups() {
    QList<PeakGroup*> groups;
    for(int i=0; i < allgroups.size(); i++ ) {
        groups.push_back(&allgroups[i]);
    }
    return groups;
}

void TableDockWidget::deleteAll() {
    treeWidget->clear();
    allgroups.clear();
    _mainwindow->getEicWidget()->replotForced();

    if ( _mainwindow->heatmap ) {
        HeatMap* _heatmap = _mainwindow->heatmap;
        _heatmap->setTable(this);
        _heatmap->replot();
    }
}



void TableDockWidget::showAllGroups() {
    treeWidget->clear();
    if (allgroups.size() == 0 ) return;

    treeWidget->setSortingEnabled(false);

    QMap<int,QTreeWidgetItem*> parents;
    for(int i=0; i < allgroups.size(); i++ ) {
        int metaGroupId  = allgroups[i].metaGroupId;
        if (metaGroupId && allgroups[i].meanMz > 0 && allgroups[i].peakCount()>0) {
            if (!parents.contains(metaGroupId)) {
                parents[metaGroupId]= new QTreeWidgetItem(treeWidget);
                parents[metaGroupId]->setText(0,QString("MetaGroup ") + QString::number(metaGroupId));
                parents[metaGroupId]->setText(3,QString::number(allgroups[i].meanRt,'f',2));
                parents[metaGroupId]->setExpanded(true);
            }
            QTreeWidgetItem* parent = parents[ metaGroupId ];
            addRow(&allgroups[i], parent);
        } else {
            addRow(&allgroups[i], NULL);
        }
    }

    QScrollBar* vScroll = treeWidget->verticalScrollBar();
    if ( vScroll ) {
        vScroll->setSliderPosition(vScroll->maximum());
    }
    treeWidget->setSortingEnabled(true);
    updateStatus();

    /*
    if (allgroups.size() > 0 ) { //select last item
    	treeWidget->setCurrentItem(treeWidget->topLevelItem(allgroups.size()-1));
    }
    (*/
}

void TableDockWidget::exportGroupsToSpreadsheet() {

    if (allgroups.size() == 0 ) {
        QString msg = "Peaks Table is Empty";
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Peaks Table to .csv File"), dir, "CSV File(*.csv)");
    if (fileName.isEmpty()) return;
    if(!fileName.endsWith(".csv",Qt::CaseInsensitive)) fileName = fileName + ".csv";


    vector<mzSample*> samples = _mainwindow->getSamples();
    if ( samples.size() == 0) return;

    CSVReports* csvreports = new CSVReports(samples);
    csvreports->setUserQuantType( _mainwindow->getUserQuantType() );
    csvreports->openGroupReport(fileName.toStdString());

    for(int i=0; i<allgroups.size(); i++ ) {
        PeakGroup& group = allgroups[i];
        csvreports->addGroup(&group);
    }
    csvreports->closeFiles();

}

void TableDockWidget::exportPeaksToSpreadsheet() {
    if (allgroups.size() == 0 ) {
        QString msg = "Peaks Table is Empty";
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Peaks Table to .csv File"), dir, "CSV File(*.csv)");
    if (fileName.isEmpty()) return;
    if(!fileName.endsWith(".csv",Qt::CaseInsensitive)) fileName = fileName + ".csv";


    vector<mzSample*> samples = _mainwindow->getSamples();
    if ( samples.size() == 0) return;

    CSVReports* csvreports = new CSVReports(samples);
    csvreports->setUserQuantType( _mainwindow->getUserQuantType() );
    csvreports->openPeakReport(fileName.toStdString());

    for(int i=0; i<allgroups.size(); i++ ) {
        PeakGroup& group = allgroups[i];
        csvreports->addGroup(&group);
    }
    csvreports->closeFiles();

}

void TableDockWidget::showSelectedGroup() {

    //sortBy(treeWidget->header()->sortIndicatorSection());
    foreach(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
        QVariant v = item->data(0,Qt::UserRole);
        PeakGroup*  group =  v.value<PeakGroup*>();

        if ( group != NULL && _mainwindow != NULL) {
            _mainwindow->setPeakGroup(group);
        }

        if ( item->childCount() > 0 ) {
            vector<PeakGroup*>children;
            for(int i=0; i < item->childCount(); i++ ) {
                QTreeWidgetItem* child = item->child(i);
                QVariant data = child->data(0,Qt::UserRole);
                PeakGroup*  group =  data.value<PeakGroup*>();
                if(group) children.push_back(group);
            }

            if (children.size() > 0) {
                if (_mainwindow->galleryWidget->isVisible() ) {
                    _mainwindow->galleryWidget->clear();
                    _mainwindow->galleryWidget->addEicPlots(children);
                }
            }
        }

        return; //single itteration only
    }
}

PeakGroup* TableDockWidget::getSelectedGroup() {
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) return NULL;
    QVariant v = item->data(0,Qt::UserRole);
    PeakGroup*  group =  v.value<PeakGroup*>();
    if ( group != NULL ) {
        return group;
    }
    return NULL;
}

void TableDockWidget::setGroupLabel(char label) {
    PeakGroup* group = getSelectedGroup();
    if ( group != NULL ) {
        group->setLabel(label);
        QTreeWidgetItem *item = treeWidget->currentItem();
        if ( item != NULL ) updateItem(item);
    }
    updateStatus();
}

void TableDockWidget::deleteGroup() {

    QTreeWidgetItem *item = treeWidget->currentItem();
    if ( item == NULL ) return;

    PeakGroup* group = getSelectedGroup();
    if ( group == NULL ) return;

    PeakGroup* parentGroup = group->parent;

    if ( parentGroup == NULL ) { //top level item
        for(int i=0; i < allgroups.size(); i++) {
            if ( &allgroups[i] == group ) {
                treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(item));
                treeWidget->update();
                delete(item);
                allgroups.erase(allgroups.begin()+i);
                break;
            }
        }
    } else if ( parentGroup && parentGroup->childCount() ) {	//this a child item
        if ( parentGroup->deleteChild(group) ) {
            QTreeWidgetItem* parentItem = item->parent();
            if ( parentItem ) {
                parentItem->removeChild(item);
                delete(item);
            }
            treeWidget->update();
        }
    }
    _mainwindow->getEicWidget()->replotForced();
    return;
}

void TableDockWidget::setClipboard() {
    PeakGroup* group = getSelectedGroup();
    if ( group != NULL ) {
        _mainwindow->isotopeWidget->setClipboard(group);
    }
}
void TableDockWidget::markGroupGood() {
    setGroupLabel('g');
    showNextGroup();
}

void TableDockWidget::markGroupBad() {
    setGroupLabel('b');
    showNextGroup();
}

void TableDockWidget::markGroupIgnored() {
    setGroupLabel('i');
    showNextGroup();
}

void TableDockWidget::showPeakGroup(int row) {

    QTreeWidgetItem *item = treeWidget->itemAt(row,0);
    if ( item == NULL) return;

    QVariant v = item->data(0,Qt::UserRole);
    PeakGroup*  group =  v.value<PeakGroup*>();

    if ( group != NULL ) {
        treeWidget->setCurrentItem(item);
        _mainwindow->setPeakGroup(group);
    }
}

void TableDockWidget::showLastGroup() {
    QTreeWidgetItem *item= treeWidget->currentItem();
    if ( item != NULL )  {
        treeWidget->setCurrentItem(treeWidget->itemAbove(item));
    }
}

void TableDockWidget::showNextGroup() {

    QTreeWidgetItem *item= treeWidget->currentItem();
    if ( item == NULL ) return;

    QTreeWidgetItem* nextitem = treeWidget->itemBelow(item); //get next item
    if ( nextitem != NULL )  treeWidget->setCurrentItem(nextitem);
}

void TableDockWidget::Train() {
    Classifier* clsf = _mainwindow->getClassifier();


    if (allgroups.size() == 0 ) return;
    if (clsf == NULL ) return;

    vector<PeakGroup*> train_groups;
    vector<PeakGroup*> test_groups;
    vector<PeakGroup*> good_groups;
    vector<PeakGroup*> bad_groups;
    MTRand mtrand;

    for(int i=0; i <allgroups.size(); i++ ) {
        PeakGroup* grp = &allgroups[i];
        if (grp->label == 'g') good_groups.push_back(grp);
        if (grp->label == 'b') bad_groups.push_back(grp);
    }

    mzUtils::shuffle(good_groups);
    for(int i=0; i <good_groups.size(); i++ ) {
        PeakGroup* grp = good_groups[i];
        i%2 == 0 ? train_groups.push_back(grp): test_groups.push_back(grp);
    }

    mzUtils::shuffle(bad_groups);
    for(int i=0; i <bad_groups.size(); i++ ) {
        PeakGroup* grp =bad_groups[i];
        i%2 == 0 ? train_groups.push_back(grp): test_groups.push_back(grp);
    }

    //cerr << "Groups Total=" <<allgroups.size() << endl;
    //cerr << "Good Groups=" << good_groups.size() << " Bad Groups=" << bad_groups.size() << endl;
    //cerr << "Splitting: Train=" << train_groups.size() << " Test=" << test_groups.size() << endl;

    clsf->train(train_groups);
    clsf->classify(test_groups);
    showAccuracy(test_groups);
    updateTable();
}

void TableDockWidget::keyPressEvent(QKeyEvent *e ) {
    //cerr << "TableDockWidget::keyPressEvent()" << e->key() << endl;

    QTreeWidgetItem *item = treeWidget->currentItem();
    if (e->key() == Qt::Key_Delete ) {
        deleteGroup();
    } else if ( e->key() == Qt::Key_T ) {
        Train();
    } else if ( e->key() == Qt::Key_G ) {
        markGroupGood();
    } else if ( e->key() == Qt::Key_B ) {
        markGroupBad();
    }
    QDockWidget::keyPressEvent(e);
    updateStatus();
}

void TableDockWidget::updateStatus() {

    int totalCount=0;
    int goodCount=0;
    int badCount=0;
    int ignoredCount=0;
    int predictedGood=0;
    for(int i=0; i < allgroups.size(); i++ ) {
        char groupLabel = allgroups[i].label;
        if (groupLabel == 'g' ) {
            goodCount++;
        } else if ( groupLabel == 'b' ) {
            badCount++;
        }
        totalCount++;
    }
    QString title = tr("Group Validation Status: Good=%2 Bad=%3 Total=%1").arg(
                        QString::number(totalCount),
                        QString::number(goodCount),
                        QString::number(badCount));
    _mainwindow->setStatusText(title);
}

float TableDockWidget::showAccuracy(vector<PeakGroup*>&groups) {
    //check accuracy
    if ( groups.size() == 0 ) return 0;

    int fp=0;
    int fn=0;
    int tp=0;
    int tn=0;
    int total=0;
    float accuracy=0;
    int gc=0;
    int bc=0;
    for(int i=0; i < groups.size(); i++ ) {
        if (groups[i]->label == 'g' || groups[i]->label == 'b' ) {
            for(int j=0; j < groups[i]->peaks.size(); j++ ) {
                float q = groups[i]->peaks[j].quality;
                char  l = groups[i]->peaks[j].label;
                if (l == 'g' ) gc++;
                if (l == 'g' && q > 0.5 ) tp++;
                if (l == 'g' && q < 0.5 ) fn++;

                if (l == 'b' ) bc++;
                if (l == 'b' && q < 0.5 ) tn++;
                if (l == 'b' && q > 0.5 ) fp++;
                total++;
            }
        }
    }
    if ( total > 0 ) accuracy = 1.00-((float)(fp+fn)/total);
    cerr << "TOTAL=" << total << endl;
    if ( total == 0 ) return 0;

    cerr << "GC=" << gc << " BC=" << bc << endl;
    cerr << "TP=" << tp << " FN=" << fn << endl;
    cerr << "TN=" << tn << " FP=" << fp << endl;
    cerr << "Accuracy=" << accuracy << endl;

    traindialog->FN->setText(QString::number(fn));
    traindialog->FP->setText(QString::number(fp));
    traindialog->TN->setText(QString::number(tn));
    traindialog->TP->setText(QString::number(tp));
    traindialog->accuracy->setText(QString::number(accuracy*100,'f',2));
    traindialog->show();
    _mainwindow->setStatusText(tr("Good Groups=%1 Bad Groups=%2 Accuracy=%3").arg(
                                   QString::number(gc),
                                   QString::number(bc),
                                   QString::number(accuracy*100)
                               ));

    return accuracy;
}

void TableDockWidget::showScatterPlot() {
    if (groupCount() == 0 ) return;
    _mainwindow->scatterDockWidget->setVisible(true);
    ((ScatterPlot*) _mainwindow->scatterDockWidget)->setTable(this);
    ((ScatterPlot*) _mainwindow->scatterDockWidget)->replot();
    ((ScatterPlot*) _mainwindow->scatterDockWidget)->contrastGroups();
}


void TableDockWidget::printPdfReport() {

    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Group Report a PDF File"),dir,tr("*.pdf"));
    if (fileName.isEmpty()) return;
    if(!fileName.endsWith(".pdf",Qt::CaseInsensitive)) fileName = fileName + ".pdf";

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOrientation(QPrinter::Landscape);
    //printer.setResolution(QPrinter::HighResolution);
    printer.setCreator("MAVEN Metabolics Analyzer");
    printer.setOutputFileName(fileName);

    QPainter painter;

    if (! painter.begin(&printer)) { // failed to open file
        qWarning("failed to open file, is it writable?");
        return;
    }

    if(printer.printerState() != QPrinter::Active) {
        qDebug() << "PrinterState:" << printer.printerState();
    }

    for(int i=0; i <allgroups.size(); i++ ) {
        PeakGroup* grp = &allgroups[i];
        _mainwindow->getEicWidget()->setPeakGroup(grp);
        _mainwindow->getEicWidget()->render(&painter);

        if (! printer.newPage()) {
            qWarning("failed in flushing page to disk, disk full?");
            return;
        }
    }
    painter.end();
}

void TableDockWidget::showHeatMap() {

    _mainwindow->heatMapDockWidget->setVisible(true);
    HeatMap* _heatmap = _mainwindow->heatmap;
    if ( _heatmap ) {
        _heatmap->setTable(this);
        _heatmap->replot();
    }
}

void TableDockWidget::showGallery() {

    _mainwindow->galleryDockWidget->setVisible(true);
    if ( _mainwindow->galleryWidget ) {
        vector<PeakGroup*>groups(allgroups.size());
        for(int i=0; i<allgroups.size(); i++) {
            groups[i]=&allgroups[i];
        }
        _mainwindow->galleryWidget->clear();
        _mainwindow->galleryWidget->addEicPlots(groups);
    }
}


void TableDockWidget::showTreeMap() {

    /*
    _mainwindow->treeMapDockWidget->setVisible(true);
    TreeMap* _treemap = _mainwindow->treemap;
    if ( _treemap ) {
    		_treemap->setTable(this);
    		_treemap->replot();
    }
    */
}

void TableDockWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu;

    QMenu analysis("Cluster Analysis");
    menu.addMenu(&analysis);

    QAction* z0 = menu.addAction("Copy to Clipboard");
    connect(z0, SIGNAL(triggered()), this ,SLOT(setClipboard()));

    QAction* zz0 = analysis.addAction("Cluster Groups by Retention Time");
    connect(zz0, SIGNAL(triggered()), this ,SLOT(clusterGroups()));

    QAction* zz1 = analysis.addAction("Collapse All");
    connect(zz1, SIGNAL(triggered()), treeWidget,SLOT(collapseAll()));

    QAction* zz2 = analysis.addAction("Expand All");
    connect(zz2, SIGNAL(triggered()), treeWidget,SLOT(expandAll()));

    QAction* z3 = menu.addAction("Align Groups");
    connect(z3, SIGNAL(triggered()), SLOT(align()));

    QAction* z4 = menu.addAction("Find Matching Compound");
    connect(z4, SIGNAL(triggered()), SLOT(findMatchingCompounds()));

    QAction* z5 = menu.addAction("Delete All Groups");
    connect(z5, SIGNAL(triggered()), SLOT(deleteAll()));

    QAction* z6 = menu.addAction("Print PDF Report");
    connect(z6, SIGNAL(triggered()), SLOT(printPdfReport()));

    QAction *selectedAction = menu.exec(event->globalPos());
}



void TableDockWidget::saveModel() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Classification Model to a File"));
    if (fileName.isEmpty()) return;

    if(!fileName.endsWith(".model",Qt::CaseInsensitive)) fileName = fileName + ".model";

    Classifier* clsf = _mainwindow->getClassifier();
    if (clsf != NULL ) {
        clsf->saveModel(fileName.toStdString());
    }

    if (clsf) {
        vector<PeakGroup*>groups;
        for(int i=0; i < allgroups.size(); i++ )
            if(allgroups[i].label == 'g' || allgroups[i].label == 'b' )
                groups.push_back(&allgroups[i]);
        clsf->saveFeatures(groups,fileName.toStdString() + ".csv");
    }
}


void TableDockWidget::findMatchingCompounds() {
    //matching compounds
    float ppm = _mainwindow->getUserPPM();
    float ionizationMode = _mainwindow->getIonizationMode();
    for(int i=0; i < allgroups.size(); i++ ) {
        PeakGroup& g = allgroups[i];
        QSet<Compound*>compounds = _mainwindow->massCalcWidget->findMathchingCompounds(g.meanMz, ppm, ionizationMode);
        if (compounds.size() > 0 ) foreach( Compound*c, compounds) {
            g.tagString += " |" + c->name;
            break;
        }
        cerr << g.meanMz << " " << compounds.size() << endl;
    }
    updateTable();
}

void TableDockWidget::writeGroupXML(QXmlStreamWriter& stream, PeakGroup* g) {
    if (!g)return;

    stream.writeStartElement("PeakGroup");
    stream.writeAttribute("groupId",  QString::number(g->groupId) );
    stream.writeAttribute("tagString",  QString(g->tagString.c_str()) );
    stream.writeAttribute("metaGroupId",  QString::number(g->metaGroupId) );
    stream.writeAttribute("expectedRtDiff",  QString::number(g->expectedRtDiff,'f',4) );
    stream.writeAttribute("groupRank",  QString::number(g->groupRank,'f',4) );
    stream.writeAttribute("label",  QString::number(g->label ));
    stream.writeAttribute("type",  QString::number((int)g->type()));
    stream.writeAttribute("changeFoldRatio",  QString::number(g->changeFoldRatio,'f',4 ));
    stream.writeAttribute("changePValue",  QString::number(g->changePValue,'e',6 ));
    if(g->srmId.length())	stream.writeAttribute("srmId",  QString(g->srmId.c_str()));

    //for sample contrasts  ratio and pvalue
    if ( g->hasCompoundLink() ) {
        Compound* c = g->compound;
        stream.writeAttribute("compoundId",  QString(c->id.c_str()));
        stream.writeAttribute("compoundDB",  QString(c->db.c_str()) );
        stream.writeAttribute("compoundName",  QString(c->name.c_str()));
    }

    for(int j=0; j < g->peaks.size(); j++ ) {
        Peak& p = g->peaks[j];
        stream.writeStartElement("Peak");
        stream.writeAttribute("pos",  QString::number(p.pos));
        stream.writeAttribute("minpos",  QString::number(p.minpos));
        stream.writeAttribute("maxpos",  QString::number(p.maxpos));
        stream.writeAttribute("rt",  QString::number(p.rt,'f',4));
        stream.writeAttribute("rtmin",  QString::number(p.rtmin,'f',4));
        stream.writeAttribute("rtmax",  QString::number(p.rtmax,'f',4));
        stream.writeAttribute("mzmin",  QString::number(p.mzmin,'f',4));
        stream.writeAttribute("mzmax",  QString::number(p.mzmax,'f',4));
        stream.writeAttribute("scan",   QString::number(p.scan));
        stream.writeAttribute("minscan",   QString::number(p.minscan));
        stream.writeAttribute("maxscan",   QString::number(p.maxscan));
        stream.writeAttribute("peakArea",  QString::number(p.peakArea,'f',4));
        stream.writeAttribute("peakAreaCorrected",  QString::number(p.peakAreaCorrected,'f',4));
        stream.writeAttribute("peakAreaTop",  QString::number(p.peakAreaTop,'f',4));
        stream.writeAttribute("peakAreaFractional",  QString::number(p.peakAreaFractional,'f',4));
        stream.writeAttribute("peakRank",  QString::number(p.peakRank,'f',4));
        stream.writeAttribute("peakIntensity",  QString::number(p.peakIntensity,'f',4));

        stream.writeAttribute("peakBaseLineLevel",  QString::number(p.peakBaseLineLevel,'f',4));
        stream.writeAttribute("peakMz",  QString::number(p.peakMz,'f',4));
        stream.writeAttribute("medianMz",  QString::number(p.medianMz,'f',4));
        stream.writeAttribute("baseMz",  QString::number(p.baseMz,'f',4));
        stream.writeAttribute("quality",  QString::number(p.quality,'f',4));
        stream.writeAttribute("width",  QString::number(p.width));
        stream.writeAttribute("gaussFitSigma",  QString::number(p.gaussFitSigma,'f',4));
        stream.writeAttribute("gaussFitR2",  QString::number(p.gaussFitR2,'f',4));
        stream.writeAttribute("groupNum",  QString::number(p.groupNum));
        stream.writeAttribute("noNoiseObs",  QString::number(p.noNoiseObs));
        stream.writeAttribute("noNoiseFraction",  QString::number(p.noNoiseFraction,'f',4));
        stream.writeAttribute("symmetry",  QString::number(p.symmetry,'f',4));
        stream.writeAttribute("signalBaselineRatio",  QString::number(p.signalBaselineRatio, 'f', 4));
        stream.writeAttribute("groupOverlap",  QString::number(p.groupOverlap,'f',4));
        stream.writeAttribute("groupOverlapFrac",  QString::number(p.groupOverlapFrac,'f',4));
        stream.writeAttribute("localMaxFlag",  QString::number(p.localMaxFlag));
        stream.writeAttribute("fromBlankSample",  QString::number(p.fromBlankSample));
        stream.writeAttribute("label",  QString::number(p.label));
        stream.writeAttribute("sample",  QString(p.getSample()->sampleName.c_str()));
        stream.writeEndElement();
    }

    if ( g->childCount() ) {
        stream.writeStartElement("children");
        for(int i=0; i < g->children.size(); i++ ) {
            PeakGroup* child = &(g->children[i]);
            writeGroupXML(stream,child);
        }
        stream.writeEndElement();
    }
    stream.writeEndElement();
}

void TableDockWidget::writePeakTableXML(QXmlStreamWriter& stream) {

    if (allgroups.size() ) {
        stream.writeStartElement("PeakGroups");
        for(int i=0; i < allgroups.size(); i++ ) writeGroupXML(stream,&allgroups[i]);
        stream.writeEndElement();
    }
}

void TableDockWidget::align() {
    if ( allgroups.size() > 0 ) {
        vector<PeakGroup*> groups;
        for(int i=0; i <allgroups.size(); i++ ) groups.push_back(&allgroups[i]);
        Aligner aligner;
        aligner.setMaxItterations(_mainwindow->alignmentDialog->maxItterations->value());
        aligner.setPolymialDegree(_mainwindow->alignmentDialog->polynomialDegree->value());
        aligner.doAlignment(groups);
        _mainwindow->getEicWidget()->replotForced();
        showSelectedGroup();
    }
}

PeakGroup* TableDockWidget::readGroupXML(QXmlStreamReader& xml,PeakGroup* parent) {
    PeakGroup g;
    PeakGroup* gp=NULL;

    g.groupId = xml.attributes().value("groupId").toString().toInt();
    g.tagString = xml.attributes().value("tagString").toString().toStdString();
    g.metaGroupId = xml.attributes().value("metaGroupId").toString().toInt();
    g.expectedRtDiff = xml.attributes().value("expectedRtDiff").toString().toDouble();
    g.groupRank = xml.attributes().value("grouRank").toString().toInt();
    g.label     =  xml.attributes().value("label").toString().toInt();
    g.setType( (PeakGroup::GroupType) xml.attributes().value("type").toString().toInt());
    g.changeFoldRatio = xml.attributes().value("changeFoldRatio").toString().toDouble();
    g.changePValue = xml.attributes().value("changePValue").toString().toDouble();

    string compoundId = xml.attributes().value("compoundId").toString().toStdString();
    string compoundDB = xml.attributes().value("compoundDB").toString().toStdString();
    string compoundName = xml.attributes().value("compoundName").toString().toStdString();

    string srmId = xml.attributes().value("srmId").toString().toStdString();
    if (!srmId.empty()) g.setSrmId(srmId);

    if (!compoundId.empty()) {
        Compound* c = DB.findSpeciesById(compoundId);
        if (c) g.compound = c;
    } else if (!compoundName.empty() && !compoundDB.empty()) {
        vector<Compound*>matches = DB.findSpeciesByName(compoundName,compoundDB);
        if (matches.size()>0) g.compound = matches[0];
    }


    if (parent) {
        parent->addChild(g);
        if (parent->children.size() > 0 ) {
            gp = &(parent->children[ parent->children.size()-1]);
            //cerr << parent << "\t addChild() " << gp << endl;
        }
    } else {
        gp = addPeakGroup(&g);
        //cerr << "addParent() " << gp << endl;
    }

    return gp;
}

void TableDockWidget::readPeakXML(QXmlStreamReader& xml,PeakGroup* parent) {

    Peak p;
    p.pos = xml.attributes().value("pos").toString().toInt();
    p.minpos = xml.attributes().value("minpos").toString().toInt();
    p.maxpos = xml.attributes().value("maxpos").toString().toInt();
    p.rt = xml.attributes().value("rt").toString().toDouble();
    p.rtmin = xml.attributes().value("rtmin").toString().toDouble();
    p.rtmax = xml.attributes().value("rtmax").toString().toDouble();
    p.mzmin = xml.attributes().value("mzmin").toString().toDouble();
    p.mzmax = xml.attributes().value("mzmax").toString().toDouble();
    p.scan = xml.attributes().value("scan").toString().toInt();
    p.minscan = xml.attributes().value("minscan").toString().toInt();
    p.maxscan = xml.attributes().value("maxscan").toString().toInt();
    p.peakArea = xml.attributes().value("peakArea").toString().toDouble();
    p.peakAreaCorrected = xml.attributes().value("peakAreaCorrected").toString().toDouble();
    p.peakAreaTop = xml.attributes().value("peakAreaTop").toString().toDouble();
    p.peakAreaFractional = xml.attributes().value("peakAreaFractional").toString().toDouble();
    p.peakRank = xml.attributes().value("peakRank").toString().toDouble();
    p.peakIntensity = xml.attributes().value("peakIntensity").toString().toDouble();
    p.peakBaseLineLevel = xml.attributes().value("peakBaseLineLevel").toString().toDouble();
    p.peakMz = xml.attributes().value("peakMz").toString().toDouble();
    p.medianMz = xml.attributes().value("medianMz").toString().toDouble();
    p.baseMz = xml.attributes().value("baseMz").toString().toDouble();
    p.quality = xml.attributes().value("quality").toString().toDouble();
    p.width = xml.attributes().value("width").toString().toInt();
    p.gaussFitSigma = xml.attributes().value("gaussFitSigma").toString().toDouble();
    p.gaussFitR2 = xml.attributes().value("gaussFitR2").toString().toDouble();
    p.groupNum = xml.attributes().value("groupNum").toString().toInt();
    p.noNoiseObs = xml.attributes().value("noNoiseObs").toString().toInt();
    p.noNoiseFraction = xml.attributes().value("noNoiseFraction").toString().toDouble();
    p.symmetry = xml.attributes().value("symmetry").toString().toDouble();
    p.signalBaselineRatio = xml.attributes().value("signalBaselineRatio").toString().toDouble();
    p.groupOverlap = xml.attributes().value("groupOverlap").toString().toDouble();
    p.groupOverlapFrac = xml.attributes().value("groupOverlapFrac").toString().toDouble();
    p.localMaxFlag = xml.attributes().value("localMaxFlag").toString().toInt();
    p.fromBlankSample = xml.attributes().value("fromBlankSample").toString().toInt();
    p.label = xml.attributes().value("label").toString().toInt();
    string sampleName = xml.attributes().value("sample").toString().toStdString();
    vector<mzSample*> samples = _mainwindow->getSamples();
    for(int i=0; i< samples.size(); i++ ) {
        if ( samples[i]->sampleName == sampleName ) {
            p.setSample(samples[i]);
            break;
        }
    }

    //cerr << "\t\t\t" << p.getSample() << " " << p.rt << endl;
    parent->addPeak(p);
}

void TableDockWidget::savePeakTable() {

    if (allgroups.size() == 0 ) {
        QString msg = "Peaks Table is Empty";
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Peaks Table to a .mzPeaks File"),dir,"mzPeaks File (*.mzPeaks)");
    if (fileName.isEmpty()) return;
    if(!fileName.endsWith(".mzroll",Qt::CaseInsensitive)) fileName = fileName + ".mzroll";

    _mainwindow->getProjectWidget()->saveProject(fileName,this);

    //savePeakTable(fileName);
}

void TableDockWidget::savePeakTable(QString fileName) {
    QFile file(fileName);
    if ( !file.open(QFile::WriteOnly) ) {
        QErrorMessage errDialog(this);
        errDialog.showMessage("File open " + fileName + " failed");
        return; //error
    }

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    writePeakTableXML(stream);
    file.close();


}

void TableDockWidget::loadPeakTable() {
    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    QString fileName = QFileDialog::getOpenFileName( this, "Load Saved Peaks", dir, "All Files(*.mzPeaks *.mzpeaks)");
    if (fileName.isEmpty()) return;
    loadPeakTable(fileName);
}

void TableDockWidget::loadPeakTable(QString fileName) {

    QFile data(fileName);
    if ( !data.open(QFile::ReadOnly) ) {
        QErrorMessage errDialog(this);
        errDialog.showMessage("File open: " + fileName + " failed");
        return;
    }
    QXmlStreamReader xml(&data);

    PeakGroup* group=NULL;
    PeakGroup* parent=NULL;
    QStack<PeakGroup*>stack;

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == "PeakGroup") {
                group=readGroupXML(xml,parent);
            }
            if (xml.name() == "Peak" && group ) {
                readPeakXML(xml,group);
            }
            if (xml.name() == "children" && group) {
                stack.push(group);
                parent=stack.top();
            }
        }

        if (xml.isEndElement()) {
            if (xml.name()=="children")  {
                if(stack.size() > 0) parent = stack.pop();
                if(parent && parent->childCount()) {
                    for(int i=0; i < parent->children.size(); i++ ) parent->children[i].groupStatistics();
                }
                if (stack.size()==0) parent=NULL;
            }
            if (xml.name()=="PeakGroup") {
                if(group) group->groupStatistics();
                group  = NULL;
            }
        }
    }
    for(int i=0; i < allgroups.size(); i++ ) allgroups[i].groupStatistics();
    showAllGroups();
}

void TableDockWidget::clusterGroups() {
    sort(allgroups.begin(),allgroups.end(), PeakGroup::compIntensity);

    int metaGroupId = 0;

    QSettings* settings = _mainwindow->getSettings();
    double maxRtDiff =  settings->value("maxRtDiff").value<double>();
    double minSampleCorrelation =  settings->value("minSampleCorrelation").value<double>();
    double minRtCorrelation =  settings->value("minRtCorrelation").value<double>();
    double ppm	= _mainwindow->getUserPPM();

    vector<mzSample*> samples = _mainwindow->getSamples();

    for(unsigned int i=0; i<allgroups.size(); i++) allgroups[i].metaGroupId=0;
    map<int,PeakGroup*>parentGroups;

    for(unsigned int i=0; i<allgroups.size(); i++) {
        PeakGroup& grp1 = allgroups[i];
        if (grp1.metaGroupId == 0) {
            grp1.metaGroupId=++metaGroupId;
            parentGroups[metaGroupId]=&grp1;
        }
        PeakGroup* parent = parentGroups[ metaGroupId ];

        mzSample* largestSample=NULL;
        double maxIntensity=0;

        for(int i=0; i < grp1.peakCount(); i++ ) {
            mzSample* sample = grp1.peaks[i].getSample();
            if ( grp1.peaks[i].peakIntensity > maxIntensity ) largestSample=sample;
        }

        if (largestSample == NULL ) continue;
        vector<float>peakIntensityA = grp1.getOrderedIntensityVector(samples,PeakGroup::AreaTop);

        for(unsigned int j=i+1; j<allgroups.size(); j++) {
            PeakGroup& grp2 = allgroups[j];
            if (grp2.metaGroupId > 0 ) continue;

            float rtdist    = abs(parent->meanRt-grp2.meanRt);
            if (rtdist > maxRtDiff ) continue;

            float rtoverlap = mzUtils::checkOverlap(grp1.minRt, grp1.maxRt, grp2.minRt, grp2.maxRt );
            if (rtoverlap < 0.1) continue;

            vector<float>peakIntensityB = grp2.getOrderedIntensityVector(samples,PeakGroup::AreaTop);
            float cor = correlation(peakIntensityA,peakIntensityB);
            if (cor < minSampleCorrelation) continue;

            float cor2 = largestSample->correlation(grp1.meanMz,grp2.meanMz,ppm,grp1.minRt,grp1.maxRt);
            if (cor2 < minRtCorrelation) continue;

            //passed all the filters.. group grp1 and grp2 into a single metagroup
            //cerr << rtdist << " " << cor << " " << cor2 << endl;
            grp2.metaGroupId = grp1.metaGroupId;
        }
    }
    showAllGroups();
}

void TableDockWidget::setupFiltersDialog() {

    filtersDialog = new QDialog(this);
    QVBoxLayout *layout = new QVBoxLayout(filtersDialog);

    sliders["PeakQuality"] =  new QHistogramSlider(this);
    sliders["PeakIntensity"] =  new QHistogramSlider(this);
    sliders["PeakWidth"] =  new QHistogramSlider(this);
    sliders["GaussianFit"] =  new QHistogramSlider(this);
    sliders["PeakAreaFractional"] =  new QHistogramSlider(this);
    sliders["PeakAreaTop"] =  new QHistogramSlider(this);
    sliders["S/N Ratio"] =  new QHistogramSlider(this);
    sliders["GoodPeakCount"] =  new QHistogramSlider(this);

    foreach(QHistogramSlider* slider, sliders) {
        connect(slider,SIGNAL(minBoundChanged(double)),SLOT(filterPeakTable()));
        connect(slider,SIGNAL(maxBoundChanged(double)),SLOT(filterPeakTable()));
        layout->addWidget(slider);
    }

    filtersDialog->setLayout(layout);
}

void TableDockWidget::showFiltersDialog() {
    filtersDialog->setVisible(! filtersDialog->isVisible() );
    if (filtersDialog->isVisible() == false) return;

    foreach(QHistogramSlider* slider, sliders) {
        slider->clearData();
    }

    for(int i=0; i <100; i++ ) sliders["PeakQuality"]->addDataPoint(QPointF((float)i/100.00,i));
    for(int i=0; i <50; i++ ) sliders["GoodPeakCount"]->addDataPoint(QPointF(i,5));
    for(int i=0; i <100; i++ ) sliders["PeakIntensity"]->addDataPoint(QPointF(i,i));
    sliders["PeakQuality"]->setPrecision(2);
    foreach(QHistogramSlider* slider, sliders) slider->recalculatePlotBounds();
}

void TableDockWidget::filterPeakTable() {
    int N=treeWidget->topLevelItemCount();
    for(int i=0; i < N; i++ ) {
        updateItem(treeWidget->topLevelItem(i));
    }
}

