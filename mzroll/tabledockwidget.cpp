#include "tabledockwidget.h";

TableDockWidget::TableDockWidget(MainWindow* mw, QString title, int numColms, int bookmarkFlag) {
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFloating(false);
    _mainwindow = mw;
    setObjectName(title);

    numColms=11;
    viewType = groupView;

    treeWidget=new QTreeWidget(this);
    treeWidget->setSortingEnabled(false);
    treeWidget->setColumnCount(numColms);
    treeWidget->setDragDropMode(QAbstractItemView::DragOnly);
    treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeWidget->setAcceptDrops(false);    
    treeWidget->setObjectName("PeakGroupTable");
    treeWidget->setFocusPolicy(Qt::NoFocus);
    treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setFocusPolicy(Qt::ClickFocus);
    tableSelectionFlagUp = false;
    tableSelectionFlagDown =  false;
    this->setAcceptDrops(true);

    setWidget(treeWidget);
    setupPeakTable();
    setTableId();

    traindialog = new TrainDialog(this);
    connect(traindialog->saveButton,SIGNAL(clicked(bool)),SLOT(saveModel()));
    connect(traindialog->trainButton,SIGNAL(clicked(bool)),SLOT(Train()));
    connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),SLOT(showSelectedGroup()));
    connect(treeWidget, SIGNAL(itemSelectionChanged()),SLOT(showSelectedGroup()));

    clusterDialog = new ClusterDialog(this);
    connect(clusterDialog->clusterButton,SIGNAL(clicked(bool)),SLOT(clusterGroups()));
    connect(clusterDialog->clearButton,SIGNAL(clicked(bool)),SLOT(clearClusters()));

    connect(this,SIGNAL(updateProgressBar(QString,int,int)), _mainwindow,SLOT(setProgressBar(QString, int,int)));
    //connect(btnGroupCSV, SIGNAL(clicked()), SLOT(exportGroupsToSpreadsheet()));


    QToolBar *toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    btnMerge = new QToolButton(toolBar);
    btnMerge->setIcon(QIcon(rsrcPath + "/merge.png"));
    btnMerge->setToolTip("Merge Groups With");
    btnMergeMenu = new QMenu("Merge Groups");
    btnMerge->setMenu(btnMergeMenu);
    btnMerge->setPopupMode(QToolButton::InstantPopup);
    if(!bookmarkFlag) btnMerge->setVisible(false);
	connect(btnMergeMenu, SIGNAL(aboutToShow()), SLOT(showMergeTableOptions())); 
    connect(btnMergeMenu, SIGNAL(triggered(QAction*)), SLOT(mergeGroupsIntoPeakTable(QAction*)));




    QWidgetAction *titlePeakTable = new TableToolBarWidgetAction(toolBar, this,  "titlePeakTable");
    QWidgetAction *btnSwitchView = new TableToolBarWidgetAction(toolBar, this,  "btnSwitchView");
    QWidgetAction *btnGroupCSV = new TableToolBarWidgetAction(toolBar, this, "btnGroupCSV");
    QWidgetAction *btnSaveJson = new TableToolBarWidgetAction(toolBar, this,  "btnSaveJson");
    QWidgetAction *btnGallery = new TableToolBarWidgetAction(toolBar, this,  "btnGallery");
    QWidgetAction *btnScatter = new TableToolBarWidgetAction(toolBar, this,  "btnScatter");
    QWidgetAction *btnCluster = new TableToolBarWidgetAction(toolBar, this,  "btnCluster");
    QWidgetAction *btnTrain = new TableToolBarWidgetAction(toolBar, this,  "btnTrain");
    QWidgetAction *btnXML = new TableToolBarWidgetAction(toolBar, this,  "btnXML");
    QWidgetAction *btnLoad = new TableToolBarWidgetAction(toolBar, this,  "btnLoad");
    QWidgetAction *btnGood = new TableToolBarWidgetAction(toolBar, this,  "btnGood");
    QWidgetAction *btnBad = new TableToolBarWidgetAction(toolBar, this,  "btnBad");
    QWidgetAction *btnHeatmapelete = new TableToolBarWidgetAction(toolBar, this,  "btnHeatmapelete");
    QWidgetAction *btnPDF = new TableToolBarWidgetAction(toolBar, this,  "btnPDF");
    QWidgetAction *btnX = new TableToolBarWidgetAction(toolBar, this,  "btnX");

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    toolBar->addAction(titlePeakTable);
    toolBar->addAction(btnSwitchView);
    toolBar->addAction(btnGood);
    toolBar->addAction(btnBad);
    toolBar->addAction(btnTrain);
    toolBar->addAction(btnHeatmapelete);
    toolBar->addWidget(btnMerge);

    toolBar->addSeparator();
    toolBar->addAction(btnScatter);
    toolBar->addAction(btnGallery);
    toolBar->addAction(btnCluster);


    toolBar->addSeparator();
    toolBar->addAction(btnPDF);
    toolBar->addAction(btnGroupCSV);
    toolBar->addAction(btnSaveJson);


    toolBar->addSeparator();
    toolBar->addAction(btnXML);
    toolBar->addAction(btnLoad);


    toolBar->addWidget(spacer);
    toolBar->addAction(btnX);


    setTitleBarWidget(toolBar);
    setupFiltersDialog();

    setAcceptDrops(true);

}

TableDockWidget::~TableDockWidget() { 
    if(traindialog != NULL) delete traindialog;
}

void TableDockWidget::showLog() {
    LOGD << "Table " << this->tableId;
}

void TableDockWidget::showTrainDialog() {
    LOGD;
    traindialog->show();
}

void TableDockWidget::showClusterDialog() {
    LOGD;
    clusterDialog->show();
}

void TableDockWidget::setTableId() {
    tableId = (_mainwindow->noOfPeakTables)++;
}

void TableDockWidget::showMergeTableOptions() {
    LOGD;
    QList<QPointer<TableDockWidget> > peaksTableList = _mainwindow->getPeakTableList();
    int n = peaksTableList.size();
    btnMergeMenu->clear();
    mergeAction.clear(); 
    for(int i=0; i<n; i++) {
        mergeAction.insert(btnMergeMenu->addAction(peaksTableList[i]->titlePeakTable->text()),
            peaksTableList[i]->tableId);
    }

}

void TableDockWidget::showMsgBox(bool check, int tableNo) {

    QMessageBox* msgBox = new QMessageBox( this );
	msgBox->setAttribute( Qt::WA_DeleteOnClose );
	msgBox->setStandardButtons( QMessageBox::Ok );

    if(check) {
        msgBox->setIconPixmap(QPixmap(rsrcPath + "/success.png"));
        msgBox->setText("Successfully Merged from Bookmark Table to Table " + QString::number(tableNo) + "   ");
    }
    else {
        msgBox->setIcon(QMessageBox::Warning);
        if(tableNo==-1) msgBox->setText("Error while merging");
        else msgBox->setText("Error while merging from Bookmark Table to Table " + QString::number(tableNo) + "   ");
    }

    msgBox->open();

}

void TableDockWidget::mergeGroupsIntoPeakTable(QAction* action) {
    LOGD;
    QList<QPointer<TableDockWidget> > peaksTableList = _mainwindow->getPeakTableList();
    int n = peaksTableList.size();
    int j = mergeAction.value(action, -1);

    if(j==-1) {
        showMsgBox(false, j);
        return;     
    }

    if(!allgroups.size() || !n) {
        showMsgBox(true, j);
        return;
    }
    int sz = peaksTableList[j-1]->allgroups.size();
    int total = allgroups.size() + sz;
    for(unsigned int i=0; i<allgroups.size(); i++) {
        allgroups[i].groupId = ++sz;
        peaksTableList[j-1]->allgroups.append(allgroups[i]);
    }
    
    deleteAll();
    peaksTableList[j-1]->showAllGroups();
    showAllGroups();

    if(total==peaksTableList[j-1]->allgroups.size()) showMsgBox(true, j);
    else showMsgBox(false, j);

}

void TableDockWidget::sortBy(int col) { 
    treeWidget->sortByColumn(col,Qt::AscendingOrder);
}

void TableDockWidget::setIntensityColName() {
    QTreeWidgetItem* header = treeWidget->headerItem();
    QString temp;
    PeakGroup::QType qtype = _mainwindow->getUserQuantType();
    switch(qtype) {
        case PeakGroup::AreaTop: temp = "Max AreaTop"; break;
        case PeakGroup::Area: temp = "Max Area"; break;
        case PeakGroup::Height: temp = "Max Height"; break;
        case PeakGroup::AreaNotCorrected: temp = "Max AreaNotCorrected"; break;
        case PeakGroup::AreaTopNotCorrected: temp = "Max AreaTopNotCorrected"; break;
        default: temp = _mainwindow->currentIntensityName; break;
    }
    _mainwindow->currentIntensityName = temp;
    header->setText(9, temp);
}

void TableDockWidget::setupPeakTable() {

    QStringList colNames;
    //Added when Merging to Maven776 - Kiran
    //Add a coulmn to the Peaks Table
    colNames << "#";
    colNames << "ID";
    colNames << "Observed m/z";
    colNames << "Expected m/z";
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

    if (viewType == groupView) {
        //Added when Merging to Maven776 - Kiran
       //Add a coulmn to the Peaks Table
       colNames << "rt delta";
        colNames << "#peaks";
        colNames << "#good";
        colNames << "Max Width";
        colNames << "Max AreaTop";
        colNames << "Max S/N";
        colNames << "Max Quality";
        //Added when Merging to Maven776 - Kiran
        //Add a coulmn to the Peaks Table
        colNames << "Rank";
        colNames << "Ratio Change";
        colNames << "P-value";
    } else if (viewType == peakView) {
        vector<mzSample*> vsamples = _mainwindow->getVisibleSamples();
        sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
        for(unsigned int i=0; i<vsamples.size(); i++ ) {
            colNames << QString(vsamples[i]->sampleName.c_str());
        }
    }

    treeWidget->setColumnCount(colNames.size());
    treeWidget->setHeaderLabels(colNames);
    treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    treeWidget->header()->adjustSize();

    treeWidget->setSortingEnabled(true);



}

void TableDockWidget::updateTable() {
    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        updateItem(*it);
        ++it;
    }
    updateStatus();
}

void TableDockWidget::updateItem(QTreeWidgetItem* item) {
    QVariant v = item->data(0,Qt::UserRole);
    PeakGroup*  group =  v.value<PeakGroup*>();
    if ( group == NULL ) return;
    heatmapBackground(item);

    //score peak quality
    Classifier* clsf = _mainwindow->getClassifier();
    if (clsf != NULL) {
        clsf->classify(group);
        group->updateQuality();
        //Added when Merging to Maven776 - Kiran
        if(viewType == groupView) item->setText(10,QString::number(group->maxQuality,'f',2));
        item->setText(1,groupTagString(group));
    }

    //Updating the peakid
    item->setText(0,QString::number(group->groupId));

    //Added when Merging to Maven776 - Kiran
    if ( viewType == groupView && fabs(group->changeFoldRatio) >= 0 ) {
        item->setText(13,QString::number(group->changeFoldRatio, 'f', 3));
        item->setText(14,QString::number(group->changePValue,    'f', 6));
    }

    int good=0; int bad=0;
    int total=group->peakCount();
    for(int i=0; i < group->peakCount(); i++ ) {
        group->peaks[i].quality > _mainwindow->mavenParameters->minQuality ? good++ : bad++;
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
}
void TableDockWidget::heatmapBackground(QTreeWidgetItem* item) {
    if(viewType != peakView) return;

    //Added when Merging to Maven776 - Kiran
    int firstColumn=4;
    StatisticsVector<float>values; float sum=0;
    for(unsigned int i=firstColumn; i< item->columnCount(); i++) {
          values.push_back(item->text(i).toFloat());
    }

    if (values.size()) {
        //normalize
        float mean = values.mean();
        float sd  = values.stddev();
        //Added when Merging to Maven776 - Kiran
        float max = values.maximum();
        float min = values.minimum();
        float range = max-min;

        for(int i=0; i<values.size();i++) {
            if(max!=0) values[i] = abs((max-values[i])/max); //Z-score
        }

        //float maxValue=max(std::fabs(values.maximum()),fabs(values.minimum()));
        QColor color = Qt::white;

        float colorramp=0.5;
        //Updated when Merging to Maven776 - Kiran
        for(int i=0; i<values.size();i++) {
           float value=values[i];
           float prob = value;
           if(prob < 0 ) prob=0;
           color.setHsvF(0.0,prob,1,1);
           //qDebug() << value << " " << prob;

           //float S=abs(cellValue/maxValue);
           //if (S > 1) S=1;
           //float H=0.67; if(cellValue<0) H=0;
           // color.setHsvF(H,S,1,1);

            //item->setText(firstColumn+i,QString::number(values[i])) ;
            item->setBackgroundColor(firstColumn+i,color);
       }
    }
}


QString TableDockWidget::groupTagString(PeakGroup* group){ 
    if (!group) return QString();
	QString tag(group->tagString.c_str());
    if (group->compound) tag = QString(group->compound->name.c_str());
    //Added when Merging to Maven776 - Kiran
    if (! group->tagString.empty() and tag.toStdString() != group->tagString) tag += " | " + QString(group->tagString.c_str());
    if (! group->srmId.empty()) tag +=  " | " + QString(group->srmId.c_str());
    //Added when Merging to Maven776 - Kiran
    if ( tag.isEmpty() && group->meanMz > 0) tag = QString::number(group->meanMz,'f',6) + "@" + QString::number(group->meanRt,'f',2);
    if ( tag.isEmpty() ) tag = QString::number(group->groupId);     
    return tag;

    qDebug() << "groupTagString() tagString:" << tag;
}

void TableDockWidget::addRow(PeakGroup* group, QTreeWidgetItem* root) { 

    if (group == NULL) return;
    if (group->peakCount() == 0 ) return;
    if (group->meanMz <= 0 ) return;

    NumericTreeWidgetItem *item = new NumericTreeWidgetItem(root,0);
    item->setFlags(Qt::ItemIsSelectable |  Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
    item->setData(0,Qt::UserRole,QVariant::fromValue(group));


    //Updated when Merging to Maven776 - Kiran
    item->setText(0,QString::number(group->groupId));
    item->setText(1,groupTagString(group));
    item->setText(2,QString::number(group->meanMz, 'f', 4));
    int charge = _mainwindow->mavenParameters->getCharge(group->compound);

    if (group->getExpectedMz(charge, 
            _mainwindow->mavenParameters->isotopeAtom, _mainwindow->mavenParameters->noOfIsotopes) != -1) {
        float mz = group->getExpectedMz(charge, 
                _mainwindow->mavenParameters->isotopeAtom, _mainwindow->mavenParameters->noOfIsotopes);

        item->setText(3,QString::number(mz,'f', 4));
    } else {
        item->setText(3, "NA");
    }

    item->setText(4,QString::number(group->meanRt, 'f', 2));

    if (group->label == 'g' ) item->setIcon(0,QIcon(":/images/good.png"));
    if (group->label == 'b' ) item->setIcon(0,QIcon(":/images/bad.png"));

    if (viewType == groupView) {
        //Updated when Merging to Maven776 - Kiran
        item->setText(5,QString::number(group->expectedRtDiff,'f',2));
        item->setText(6,QString::number(group->sampleCount));
        item->setText(7,QString::number(group->goodPeakCount));
        item->setText(8,QString::number(group->maxNoNoiseObs));
        item->setText(9,QString::number(extractMaxIntensity(group),'g',2));
        item->setText(10,QString::number(group->maxSignalBaselineRatio,'f',0));
        item->setText(11,QString::number(group->maxQuality,'f',2));
        item->setText(12,QString::number(group->groupRank,'e',6));

        if ( group->changeFoldRatio != 0 ) {
            //Updated when Merging to Maven776 - Kiran
            item->setText(13,QString::number(group->changeFoldRatio, 'f', 2));
            item->setText(14,QString::number(group->changePValue,    'e', 4));
        }

    } else if ( viewType == peakView) {
        vector<mzSample*> vsamples = _mainwindow->getVisibleSamples();
        sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
        vector<float>yvalues = group->getOrderedIntensityVector(vsamples,_mainwindow->getUserQuantType());
        for(unsigned int i=0; i<yvalues.size(); i++ ) {
         //Updated when Merging to Maven776 - Kiran
         item->setText(5+i,QString::number(yvalues[i]));
        }
        heatmapBackground(item);
    }
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
            for (unsigned int i = 0; i <  allgroups.size(); i++) {
                allgroups[i].groupId = i + 1;
            }
            // keep note of the samples that were used for a group and it's child
            for(mzSample* sm: _mainwindow->samples) {
                if(sm->isSelected){
                    g.samplesUsed.push_back(sm);
                    for(PeakGroup &childGrp: g.children) {
                        childGrp.samplesUsed.push_back(sm);
                    }
                }
            }
            //g.groupId = allgroups.size();
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
     //Added when Merging to Maven776 - Kiran
    _mainwindow->removePeaksTable(this);
    _mainwindow->getEicWidget()->replotForced();
     //Added when Merging to Maven776 - Kiran
    this->hide();

    if ( _mainwindow->heatmap ) {
        HeatMap* _heatmap = _mainwindow->heatmap;
        _heatmap->setTable(this);
        _heatmap->replot();
    }
}


void TableDockWidget::showAllGroups() {
    treeWidget->clear();
    if (allgroups.size() == 0 ) {
        if (viewType == groupView) setIntensityColName();
        setVisible(false);
        return;
    }

    treeWidget->setSortingEnabled(false);
     //Added when Merging to Maven776 - Kiran
    setupPeakTable();
    if (viewType == groupView) setIntensityColName();
    
    QMap<int,QTreeWidgetItem*> parents;
    for(int i=0; i < allgroups.size(); i++ ) { 
        int clusterId  = allgroups[i].clusterId;
        if (clusterId && allgroups[i].meanMz > 0 && allgroups[i].peakCount()>0) {
            if (!parents.contains(clusterId)) {
                parents[clusterId]= new QTreeWidgetItem(treeWidget);
                parents[clusterId]->setText(0,QString("Cluster ") + QString::number(clusterId));
                parents[clusterId]->setText(5,QString::number(allgroups[i].meanRt,'f',2));
                parents[clusterId]->setExpanded(true);
            }
            QTreeWidgetItem* parent = parents[ clusterId ];
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

float TableDockWidget::extractMaxIntensity(PeakGroup* group) {
    float temp;
    PeakGroup::QType qtype = _mainwindow->getUserQuantType();
    switch(qtype) {
        case PeakGroup::AreaTop: temp = group->maxAreaTopIntensity; break;
        case PeakGroup::Area: temp =  group->maxAreaIntensity; break;
        case PeakGroup::Height: temp = group->maxHeightIntensity; break;
        case PeakGroup::AreaNotCorrected: temp = group->maxAreaNotCorrectedIntensity; break;
        case PeakGroup::AreaTopNotCorrected: temp = group->maxAreaTopNotCorrectedIntensity; break;
        default: temp = group->currentIntensity; break;
    }
    group->currentIntensity = temp;
    return temp;
}

void TableDockWidget::exportGroupsToSpreadsheet() {
    LOGD;
    //Merged to Maven776 - Kiran
    // CSVReports* csvreport = new CSVReports;
    vector<mzSample*> samples = _mainwindow->getSamples();
    CSVReports* csvreports = new CSVReports(samples);
    csvreports->setMavenParameters(_mainwindow->mavenParameters);
    if (allgroups.size() == 0 ) {
        QString msg = "Peaks Table is Empty";
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();

    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    QString groupsTAB = "Groups Summary Matrix Format (*.tab)";
    QString groupsSTAB = "Groups Summary Matrix Format Without Set Name (*.tab)";    
    QString peaksTAB =  "Peaks Detailed Format (*.tab)";
    QString groupsCSV = "Groups Summary Matrix Format Comma Delimited (*.csv)";
    QString groupsSCSV = "Groups Summary Matrix Format Comma Delimited Without Set Name (*.csv)";
    QString peaksCSV =  "Peaks Detailed Format Comma Delimited (*.csv)";
    //Added when Merging to Maven776 - Kiran
    QString peaksListQE= "Inclusion List QE (*.csv)";
    QString mascotMGF=   "Mascot Format MS2 Scans (*.mgf)";

    QString sFilterSel;
    QString fileName = QFileDialog::getSaveFileName(this, 
            tr("Export Groups"), dir, 
            groupsTAB + ";;" + groupsSTAB + ";;" + peaksTAB + ";;" + groupsCSV + ";;" + groupsSCSV + ";;" + peaksCSV + ";;" + peaksListQE + ";;" + mascotMGF,
            &sFilterSel);

    if(fileName.isEmpty()) return;

    if ( sFilterSel == groupsCSV || sFilterSel == peaksCSV) {
        if(!fileName.endsWith(".csv",Qt::CaseInsensitive)) fileName = fileName + ".csv";
    }
    if ( sFilterSel == groupsSCSV) {
        if(!fileName.endsWith(".csv",Qt::CaseInsensitive)) fileName = fileName + ".csv";
        cerr <<"csv without:";
        csvreports->flag = 0;
        cerr <<"csv without:1";
    }
    if ( sFilterSel == groupsTAB || sFilterSel == peaksTAB) {
        if(!fileName.endsWith(".tab",Qt::CaseInsensitive)) fileName = fileName + ".tab";
    }
    if ( sFilterSel == groupsSTAB) {
        cerr <<"tab without:";
        if(!fileName.endsWith(".tab",Qt::CaseInsensitive)) fileName = fileName + ".tab";
        csvreports->flag = 0;
        cerr <<"tab without:1";
    }
    
    if ( samples.size() == 0) return;

    //Added when Merging to Maven776 - Kiran
	if (sFilterSel == peaksListQE ) { 
		writeQEInclusionList(fileName); 
		return;
    } else if (sFilterSel == mascotMGF ) {
        writeMascotGeneric(fileName);
        return;
    }

   
    csvreports->setUserQuantType( _mainwindow->getUserQuantType() );

    //Added to pass into csvreports file when merged with Maven776 - Kiran
    bool includeSetNamesLines=true;

    if (sFilterSel == groupsCSV) {
        csvreports->openGroupReport(fileName.toStdString(),includeSetNamesLines);
    } else if (sFilterSel == groupsTAB )  {
        csvreports->openGroupReport(fileName.toStdString(),includeSetNamesLines);
    } else if (sFilterSel == peaksCSV )  {
        csvreports->openPeakReport(fileName.toStdString());
    } else if (sFilterSel == peaksTAB )  {
        csvreports->openPeakReport(fileName.toStdString());
    } else { 	//default to group summary
        //Updated when csvreports file was merged with Maven776 - Kiran
        csvreports->openGroupReport(fileName.toStdString(),includeSetNamesLines);
    }

    QList<PeakGroup*> selectedGroups = getSelectedGroups();
    csvreports->setSelectionFlag(static_cast<int>(peakTableSelection));

    for(int i=0; i<allgroups.size(); i++ ) {
        if (selectedGroups.contains(&allgroups[i])) {
            PeakGroup& group = allgroups[i];
            csvreports->addGroup(&group);
        }
    }
    csvreports->closeFiles();

    if (csvreports->getErrorReport() != "") {
        QMessageBox msgBox(_mainwindow);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText(csvreports->getErrorReport());
        msgBox.exec();
    }
}

void TableDockWidget::exportJson() {
    LOGD;
    if (allgroups.size() == 0 ) {
        QString msg = "Peaks Table is Empty";
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }
    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save EICs to Json File"),dir,tr("*.json"));
    if (fileName.isEmpty()) return;
    if(!fileName.endsWith(".json",Qt::CaseInsensitive)) fileName = fileName + ".json";

    saveJson * jsonSaveThread = new saveJson();
    jsonSaveThread->setMainwindow(_mainwindow);
    jsonSaveThread->setPeakTable(this);
    jsonSaveThread->setfileName(fileName.toStdString());
    jsonSaveThread->start();    
}

void TableDockWidget::writeGroupMzEICJson(PeakGroup& grp,ofstream& myfile, vector<mzSample*> vsamples) {

    double mz,mzmin,mzmax,rtmin,rtmax;

    myfile << setprecision(10);
    myfile << "{\n";
    myfile << "\"groupId\": " << grp.groupId ;
    myfile << ",\n" << "\"metaGroupId\": " << grp.metaGroupId ;
    myfile << ",\n" << "\"meanMz\": " << grp.meanMz  ;
    myfile << ",\n" << "\"meanRt\": " << grp.meanRt ;
    myfile << ",\n" << "\"rtmin\": " << grp.minRt ;
    myfile << ",\n" << "\"rtmax\": " << grp.maxRt ;
    myfile << ",\n" << "\"maxQuality\": " << grp.maxQuality ;

    if ( grp.hasCompoundLink() ) {

        myfile << ",\n" << "\"compound\": { " ;

        string compoundID = grp.compound->id;
        myfile << "\"compoundId\": "<< sanitizeJSONstring(compoundID) ;
        string compoundName = grp.compound->name;
        myfile << ",\n" << "\"compoundName\": "<< sanitizeJSONstring(compoundName);
        string formula = grp.compound->formula;
        myfile << ",\n" << "\"formula\: "<< sanitizeJSONstring(formula);

        myfile << ",\n" << "\"expectedRt\": " << grp.compound->expectedRt;

        mz = grp.compound->mass;
        if (!grp.compound->formula.empty()) {
            float formula_mz =  _mainwindow->mavenParameters->mcalc.computeMass(grp.compound->formula,_mainwindow->mavenParameters->charge);
            if(formula_mz) {
                mz = formula_mz;
            }
        }

        myfile << ",\n" << "\"expectedMz\": " << mz ;

        myfile << ",\n" << "\"srmID\": " << sanitizeJSONstring(grp.srmId) ;
        myfile << ",\n" << "\"tagString\": " << sanitizeJSONstring(grp.tagString) ;

        string fullTag = grp.srmId + grp.tagString;
        string fullName=compoundName;
        string fullID=compoundID;
        if (fullTag.length()) {
            fullName = compoundName + " [" + fullTag + "]";
            fullID = compoundID + " [" + fullTag + "]";
        }
        myfile << ",\n" << "\"fullCompoundName\": "<< sanitizeJSONstring(fullName);
        myfile << ",\n" << "\"fullCompoundID\": "<< sanitizeJSONstring(fullID) ;

        myfile << "}" ; // compound
    }
    myfile << ",\n"<< "\"peaks\": [ " ;

    for(std::vector<mzSample*>::iterator it = vsamples.begin(); it != vsamples.end(); ++it) {
        if (it!=vsamples.begin()) {
            myfile << ",\n";
        }
        Peak* peak = grp.getSamplePeak(*it);
        if(peak) {
            //TODO: add slice information here: e.g. what ppm was used
            myfile << "{\n";
            myfile << "\"sampleName\": " << sanitizeJSONstring((*it)->sampleName) ;
            myfile << ",\n" << "\"peakMz\": " << peak->peakMz ;
            myfile << ",\n" << "\"medianMz\": " << peak->medianMz ;
            myfile << ",\n" << "\"baseMz\": " << peak->baseMz ;
            myfile << ",\n" << "\"mzmin\": " << peak->mzmin ;
            myfile << ",\n" << "\"mzmax\": " << peak->mzmax ;
            myfile << ",\n" << "\"rt\": " << peak->rt ;
            myfile << ",\n" << "\"rtmin\": " << peak->rtmin ;
            myfile << ",\n" << "\"rtmax\": " << peak->rtmax ;
            myfile << ",\n" << "\"quality\": " << peak->quality ;
            myfile << ",\n" << "\"peakIntensity\": " << peak->peakIntensity ;
            myfile << ",\n" << "\"peakBaseLineLevel\": " << peak->peakBaseLineLevel ;
            myfile << ",\n" << "\"peakArea\": " << peak->peakAreaCorrected ;
            myfile << ",\n" << "\"peakAreaTop\": " << peak->peakAreaTopCorrected;
            myfile << ",\n" << "\"peakAreaNotCorrected\": " << peak->peakArea;
            myfile << ",\n" << "\"peakAreaTopNotCorrected\": " << peak->peakAreaTop;
            myfile << ",\n" << "\"noNoiseObs\": " << peak->noNoiseObs ;
            myfile << ",\n" << "\"signalBaselineRatio\": " << peak->signalBaselineRatio ;
            myfile << ",\n" << "\"fromBlankSample\": " << peak->fromBlankSample ;
            myfile << ",\n" << "\"peakAreaFractional\": " << peak->peakAreaFractional ;
            myfile << ",\n" << "\"symmetry\": " << peak->symmetry ;
            myfile << ",\n" << "\"noNoiseFraction\": " << peak->noNoiseFraction ;
            myfile << ",\n" << "\"groupOverlap\": " << peak->groupOverlap ;
            myfile << ",\n" << "\"groupOverlapFrac\": " << peak->groupOverlapFrac ;
            myfile << ",\n" << "\"gaussFitR2\": " << peak->gaussFitR2 ;
            myfile << ",\n" << "\"peakRank\": " << peak->peakRank ;
            myfile << ",\n" << "\"peakWidth\": " << peak->width ;
        }
        else {
            myfile << "{\n";
            myfile << "\"sampleName\": " << sanitizeJSONstring((*it)->sampleName) ;
            myfile << ",\n" << "\"peakMz\": " << "\"NA\"" ;
            myfile << ",\n" << "\"medianMz\": " << "\"NA\"" ;
            myfile << ",\n" << "\"baseMz\": " << "\"NA\"" ;
            myfile << ",\n" << "\"mzmin\": " << "\"NA\"" ;
            myfile << ",\n" << "\"mzmax\": " << "\"NA\"" ;
            myfile << ",\n" << "\"rt\": " << "\"NA\"" ;
            myfile << ",\n" << "\"rtmin\": " << "\"NA\"" ;
            myfile << ",\n" << "\"rtmax\": " << "\"NA\"" ;
            myfile << ",\n" << "\"quality\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakIntensity\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakBaseLineLevel\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakArea\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakAreaTop\": " << "\"NA\"";
            myfile << ",\n" << "\"peakAreaNotCorrected\": " << "\"NA\"";
            myfile << ",\n" << "\"peakAreaTopNotCorrected\": " << "\"NA\"";
            myfile << ",\n" << "\"noNoiseObs\": " << "\"NA\"" ;
            myfile << ",\n" << "\"signalBaselineRatio\": " << "\"NA\"" ;
            myfile << ",\n" << "\"fromBlankSample\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakAreaFractional\": " << "\"NA\"" ;
            myfile << ",\n" << "\"symmetry\": " << "\"NA\"" ;
            myfile << ",\n" << "\"noNoiseFraction\": " << "\"NA\"" ;
            myfile << ",\n" << "\"groupOverlap\": " << "\"NA\"";
            myfile << ",\n" << "\"groupOverlapFrac\": " << "\"NA\"" ;
            myfile << ",\n" << "\"gaussFitR2\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakRank\": " << "\"NA\"" ;
            myfile << ",\n" << "\"peakWidth\": " << "\"NA\"" ;
        }

        EIC* eic=NULL;
        //TODO: replace this by putting mzSlice pointer in peakgroup and using that
        if (grp.hasCompoundLink()) {
            if ( !grp.srmId.empty() ) { //MS-MS case 1
                eic = (*it)->getEIC(grp.srmId, _mainwindow->mavenParameters->eicType);
            }
            else if((grp.compound->precursorMz > 0) & (grp.compound->productMz > 0)) { //MS-MS case 2
                //TODO: this is a problem -- amuQ1 and amuQ3 that were used to generate the peakgroup are not stored anywhere
                //will use mainWindow->MavenParameters for now but those values may have changed between generation and export
                eic =(*it)->getEIC(grp.compound->precursorMz, grp.compound->collisionEnergy, grp.compound->productMz,_mainwindow->mavenParameters->eicType,
                                   _mainwindow->mavenParameters->filterline, _mainwindow->mavenParameters->amuQ1, _mainwindow->mavenParameters->amuQ3);
            }
            else {//MS1 case
                //TODO: same problem here: need the ppm that was used, or the slice object
                //for mz could rely on same computation being done way above
                //redoing it only for code clarity
                mz = grp.compound->mass;
                if (!grp.compound->formula.empty()) {
                    float formula_mz =  _mainwindow->mavenParameters->mcalc.computeMass(grp.compound->formula,_mainwindow->mavenParameters->charge);
                    if(formula_mz) {
                        mz = formula_mz;
                    }
                }
                float ppm=_mainwindow->mavenParameters->compoundPPMWindow;
                mzmin = mz - mz / 1e6 * ppm;
                mzmax = mz + mz / 1e6 * ppm;
                rtmin = grp.minRt - outputRtWindow;
                rtmax = grp.maxRt + outputRtWindow;
                eic = (*it)->getEIC(mzmin,mzmax,rtmin,rtmax,1,
                                    _mainwindow->mavenParameters->eicType,
                                    _mainwindow->mavenParameters->filterline);
            }
        }

        else {//no compound information
            //TODO: same problem here: need the ppm that was used, or the slice object
            mz=grp.meanMz;
            float ppm=_mainwindow->mavenParameters->compoundPPMWindow;
            mzmin = mz - mz / 1e6 * ppm;
            mzmax = mz + mz / 1e6 * ppm;
            rtmin = grp.minRt - outputRtWindow;
            rtmax = grp.maxRt + outputRtWindow;
            eic = (*it)->getEIC(mzmin,mzmax,rtmin,rtmax,1,
                                _mainwindow->mavenParameters->eicType,
                                _mainwindow->mavenParameters->filterline);
        }

        //TODO: for MS1 we've already limited RT range, but for MS/MS the entire RT range of the SRM will be output
        //either check here or edit getEIC functionality
        if(eic) {
            int N = eic->rt.size();

            //myfile << setprecision(4);

            myfile << ",\n" << "\"eic\": {" ;

            myfile << "\"rt\": [";

            for(int i=0;i<N;i++){
                if ( eic->rt[i] > 0) {
                    myfile << eic->rt[i];
                    if (i < N - 1) myfile << ",";
                }
            }
            myfile << "],\n" ; //rt
            myfile << "\"intensity\": [";
            for(int i=0;i<N;i++){
                if ( eic->rt[i] > 0) {
                    myfile << eic->intensity[i];
                    if (i < N - 1) myfile << ",";
                }
            }
            myfile << "]" ; //intensity
            myfile << "\n}" ;//eic

            myfile << "\n}" ; //peak
            delete(eic);
        }
    }


    myfile << "\n]" ; //peaks

    myfile << "}" ; //group
}


void TableDockWidget::saveMzEICJson(string filename) {
    ofstream myfile(filename.c_str());
    myfile << setprecision(10);

    myfile << "{\"groups\": [" <<endl;

    int groupId=0;
    int metaGroupId=0;
    vector<mzSample*> vsamples = _mainwindow->getVisibleSamples();

    for(int i=0; i < allgroups.size(); i++ ) {
        PeakGroup& grp = allgroups[i];

        //if compound is unknown, output only the unlabeled form information
        if( grp.compound == NULL || grp.childCount() == 0 ) {
            grp.groupId= ++groupId;
            grp.metaGroupId= ++metaGroupId;
            if(groupId>1) myfile << "\n,";
            writeGroupMzEICJson(grp,myfile, vsamples);
        }
        else { //output all relevant isotope info otherwise
            //does this work? is children[0] always the same as grp (parent)?
            grp.metaGroupId = ++ metaGroupId;
            for (unsigned int k=0; k < grp.children.size(); k++) {
                grp.children[k].metaGroupId = grp.metaGroupId;
                grp.children[k].groupId= ++groupId;
                if(groupId>1) myfile << "\n,";
                writeGroupMzEICJson(grp.children[k], myfile, vsamples);
            }
        }
        Q_EMIT(updateProgressBar("Writing to json file. Please wait...", i, allgroups.size() - 1));
    }
    myfile << "]}"; //groups
    myfile.close();
    Q_EMIT(updateProgressBar("Writing to json complete.", 1, 1));
}

string TableDockWidget::sanitizeJSONstring(string s) {
    QString out(s.c_str());
    out.replace("\"","\\\""); //escape quotes
    out="\""+out+"\""; //quote the whole string
    return out.toStdString();
}

vector<EIC*> TableDockWidget::getEICs(float rtmin, float rtmax, PeakGroup& grp) {
    vector<EIC*> eics;
    for (int i = 0; i < grp.peaks.size(); i++) {
        float mzmin = grp.meanMz - 0.2;
        float mzmax = grp.meanMz + 0.2;
        //cerr <<setprecision(5) << "getEICs: mz:" << mzmin << "-" << mzmax << " rt:" << rtmin << "-" << rtmax << endl;
        vector<mzSample*> samples = _mainwindow->getSamples();
        for (unsigned int j = 0; j < samples.size(); j++) {
            if (!grp.srmId.empty()) {
                EIC* eic = samples[j]->getEIC(grp.srmId, _mainwindow->mavenParameters->eicType);
                eics.push_back(eic);
            } else {
                EIC* eic = samples[j]->getEIC(mzmin, mzmax, rtmin, rtmax, 1,
                                        _mainwindow->mavenParameters->eicType,
                                        _mainwindow->mavenParameters->filterline);
                eics.push_back(eic);
            }
        }
    }
    return (eics);
}


void TableDockWidget::showSelectedGroup() { 
    LOGD;
    //sortBy(treeWidget->header()->sortIndicatorSection());
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) return;

    QVariant v = item->data(0,Qt::UserRole);
    PeakGroup*  group =  v.value<PeakGroup*>();
    _mainwindow->alignmentVizWidget->plotGraph(group);
    
    if ( group != NULL && _mainwindow != NULL) {
        _mainwindow->setPeakGroup(group);
        // _mainwindow->rconsoleDockWidget->updateStatus();
    }

    if ( item->childCount() > 0 ) {
        vector<PeakGroup*>children;
        for(int i=0; i < item->childCount(); i++ ) {
            QTreeWidgetItem* child = item->child(i);
            QVariant data = child->data(0,Qt::UserRole);
            PeakGroup*  group =  data.value<PeakGroup*>();
            if(group) children.push_back(group);
        }
    
        /*if (children.size() > 0) {
            if (_mainwindow->galleryWidget->isVisible() ) {
                _mainwindow->galleryWidget->clear();
                _mainwindow->galleryWidget->addEicPlots(children);
            }
        }*/
    }
}

QList<PeakGroup*> TableDockWidget::getSelectedGroups() {
    QList<PeakGroup*> selectedGroups;
    Q_FOREACH(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
        if (item) {
            QVariant v = item->data(0,Qt::UserRole);
            PeakGroup*  group =  v.value<PeakGroup*>();
            if ( group != NULL ) { selectedGroups.append(group); }
        }
    }
    return selectedGroups;
}

QList<PeakGroup*> TableDockWidget::getCustomGroups(peakTableSelectionType peakSelection) {
    QList<PeakGroup*> selectedGroups;
    peakTableSelectionType temppeakSelection = peakSelection;
    Q_FOREACH(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
        if (item) {
            QVariant v = item->data(0,Qt::UserRole);
            PeakGroup*  group =  v.value<PeakGroup*>();
            if ( group != NULL ) {
                if (temppeakSelection == peakTableSelectionType::Good) {
                    if (group->label == 'g') {
                        selectedGroups.append(group);
                    }
                } else if (temppeakSelection == peakTableSelectionType::Bad) {
                    if (group->label == 'b') {
                        selectedGroups.append(group);
                    }
                } else {
                    selectedGroups.append(group);
                }
            }
        }
    }
    return selectedGroups;
}

//@author:Giridhari -- Refactored this function
//TODO: To select one or more item in Qtreewidget in peaktable
PeakGroup* TableDockWidget::getSelectedGroup() { 
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) return NULL;
    QVariant v = item->data(0,Qt::UserRole);
    PeakGroup*  group =  v.value<PeakGroup*>();
    if ( group != NULL ) { return group; }
    else
    return NULL;
}

void TableDockWidget::setGroupLabel(char label) {
    Q_FOREACH(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
        if (item) {
            QVariant v = item->data(0,Qt::UserRole);
            PeakGroup*  group =  v.value<PeakGroup*>();
            if ( group != NULL ) {
                 group->setLabel(label);
            }
            updateItem(item);
        }
    }
    updateStatus();
}

// merged with maven776 - Kiran
void TableDockWidget::deleteGroup(PeakGroup *groupX) {
    if(!groupX) return;

    int pos=-1;
    for(int i=0; i < allgroups.size(); i++) {
        if ( &allgroups[i] == groupX ) {
            pos=i; break;
        }
    }
    if (pos == -1) return;

    //qDebug() << "Delete pos=" << pos;
    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        QTreeWidgetItem* item = (*it);
        if (item->isHidden()) { ++it; continue; }
        QVariant v = item->data(0,Qt::UserRole);
        PeakGroup*  group =  v.value<PeakGroup*>();
        if ( group != NULL and group == groupX) {
            item->setHidden(true);

            //Deleteing
            int posTree = treeWidget->indexOfTopLevelItem(item);
            if (posTree != -1) treeWidget->takeTopLevelItem(posTree);
            allgroups.erase(allgroups.begin()+pos);
            break;
        }
        ++it;
    }

    for(unsigned int i = 0; i < allgroups.size(); i++) {
        allgroups[i].groupId = i + 1;
    }
    updateTable();
}

void TableDockWidget::deleteGroups() {

    LOGD;
    QList<PeakGroup*> selectedGroups;
    QTreeWidgetItem* nextItem;

    if (treeWidget->selectedItems().size() == 0) {
        return;
    }

    Q_FOREACH(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
        if (item) {
            nextItem = treeWidget->itemBelow(item);
            QVariant v = item->data(0,Qt::UserRole);
            PeakGroup*  group =  v.value<PeakGroup*>();
            if ( group != NULL ) {
                PeakGroup* parentGroup = group->parent;
                int childrenNum = -1;
                if ( parentGroup == NULL ) { //top level item
                    //Updated when Merging to Maven776 - Kiran
                    if (nextItem) {
                        QVariant vc = nextItem->data(0,Qt::UserRole);
                        PeakGroup*  groupc =  vc.value<PeakGroup*>();
                        if (groupc->tagString == "C12 PARENT") {
                            nextItem = nextItem->parent();
                        } else if ( groupc->isIsotope()){
                            nextItem = nextItem->parent();
                        }
                    }
                    deleteGroup(group);
                } else if ( parentGroup && parentGroup->childCount() ) {	//this a child item
                    childrenNum = parentGroup->childCount();
                    if ( parentGroup->deleteChild(group) ) {
                        QTreeWidgetItem* parentItem = item->parent();
                        if ( parentItem ) {
                            parentItem->removeChild(item);
                            delete(item);
                        }
                    }
                }
                if (parentGroup != NULL) {
                    if (childrenNum == parentGroup->childCount()) {
                        deleteGroup(group);
                        nextItem = treeWidget->itemBelow(item->parent());
                    }
                }
             }
        }
    }
    if(nextItem) treeWidget->setCurrentItem(nextItem,0);
    _mainwindow->getEicWidget()->replotForced();
    showSelectedGroup();
    _mainwindow->getEicWidget()->addPeakPositions();

    return;
}

void TableDockWidget::setClipboard() { 
    QList<PeakGroup*>groups = getSelectedGroups();
    if (groups.size() >0) {
        _mainwindow->isotopeWidget->setClipboard(groups);
    }
}


void TableDockWidget::showConsensusSpectra() {
    //Merged with Maven776 - Kiran
    QList<PeakGroup*>groups = getSelectedGroups();
    if (groups.size() >0) {
        _mainwindow->spectraWidget->showConsensusSpectra(groups[groups.size()-1]);
    }
}


void TableDockWidget::markGroupGood() {
    LOGD;     
    setGroupLabel('g');
    showNextGroup();
    _mainwindow->peaksMarked++;
    if (checkLabeledGroups() && !bookmarkPeaksTAble) _mainwindow->allPeaksMarked = true; 
    _mainwindow->autoSaveSignal();
}

void TableDockWidget::markGroupBad() { 
    LOGD;
    setGroupLabel('b');
    showNextGroup();
    _mainwindow->peaksMarked++;
    if (checkLabeledGroups() && !bookmarkPeaksTAble) _mainwindow->allPeaksMarked = true; 
    _mainwindow->autoSaveSignal();
}

bool TableDockWidget::checkLabeledGroups() {

    int totalCount=0;
    int goodCount=0;
    int badCount=0;

    if (_mainwindow->peaksMarked >= allgroups.size()) {
        for(int i=0; i < allgroups.size(); i++ ) {
            char groupLabel = allgroups[i].label;
            if (groupLabel == 'g' ) {
                goodCount++;
            } else if ( groupLabel == 'b' ) {
                badCount++;
            }
            totalCount++;
        }

        if (totalCount == goodCount + badCount) return true;
    }

    return false;
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
    LOGD; 
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
        QList<QTreeWidgetItem *> items = treeWidget->selectedItems();
        if (items.size() > 0) {
            cerr << items.size() << endl;
            deleteGroups();
        }
    } else if ( e->key() == Qt::Key_T ) {
        if (item) {
            Train();
        }
    } else if ( e->key() == Qt::Key_G ) {
        LOGD;
        if (item) {
            markGroupGood();
        }
    } else if ( e->key() == Qt::Key_B ) {
        LOGD;
        if (item) {
            markGroupBad();
        }
    } else if ( e->key() == Qt::Key_Left ) {
        LOGD;
        if (treeWidget->currentItem()) {
            if (treeWidget->currentItem()->parent()) {
                treeWidget->collapseItem(treeWidget->currentItem()->parent());
                treeWidget->setCurrentItem(treeWidget->currentItem()->parent());
            } else {
                treeWidget->collapseItem(treeWidget->currentItem());
            }
        }
    }  else if ( e->key() == Qt::Key_Right ) {
        LOGD;
        if (treeWidget->currentItem()) {
            if (!treeWidget->currentItem()->isExpanded()) {
                treeWidget->expandItem(treeWidget->currentItem());
            }
        }
    } else if ( e->key() == Qt::Key_O ) {
        if (treeWidget->currentItem()) {
            if (treeWidget->currentItem()->isExpanded()) {
                if (treeWidget->currentItem()->parent()) {
                    treeWidget->collapseItem(treeWidget->currentItem()->parent());
                    treeWidget->setCurrentItem(treeWidget->currentItem()->parent());
                } else {
                    treeWidget->collapseItem(treeWidget->currentItem());
                }
            } else {
                treeWidget->expandItem(treeWidget->currentItem());
            }
        }
    } else if ( e->key() == Qt::Key_Down && e->modifiers()==Qt::ShiftModifier) {
        if(treeWidget->itemBelow(item)) {
            if (tableSelectionFlagDown) {
                treeWidget->selectionModel()->setCurrentIndex( treeWidget->currentIndex(), QItemSelectionModel::Toggle| QItemSelectionModel::Rows);
                tableSelectionFlagDown = false;
            } else  {
                treeWidget->selectionModel()->setCurrentIndex( treeWidget->indexBelow(treeWidget->currentIndex()), QItemSelectionModel::Toggle | QItemSelectionModel::Rows);
            }
            tableSelectionFlagUp = true;
        }
    } else if ( e->key() == Qt::Key_Up && e->modifiers()==Qt::ShiftModifier) {
        if (treeWidget->itemAbove(item)) {
            if(tableSelectionFlagUp) {
                treeWidget->selectionModel()->setCurrentIndex( treeWidget->currentIndex(), QItemSelectionModel::Toggle | QItemSelectionModel::Rows);
                tableSelectionFlagUp = false;
            }   else {
                treeWidget->selectionModel()->setCurrentIndex( treeWidget->indexAbove(treeWidget->currentIndex()), QItemSelectionModel::Toggle | QItemSelectionModel::Rows);
            }
            tableSelectionFlagDown = true;
        }
    }else if ( e->key() == Qt::Key_Down ) {
        LOGD;
        if(treeWidget->itemBelow(item)) {
            treeWidget->setCurrentItem(treeWidget->itemBelow(item));
        }
	} else if ( e->key() == Qt::Key_Up ) {
        LOGD;
        if (treeWidget->itemAbove(item)) {
            treeWidget->setCurrentItem(treeWidget->itemAbove(item));
        }
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

    int fp=0; int fn=0; int tp=0; int tn=0;  int total=0; float accuracy=0;
    int gc=0;
    int bc=0;
    for(int i=0; i < groups.size(); i++ ) {
        if (groups[i]->label == 'g' || groups[i]->label == 'b' ) {
            for(int j=0; j < groups[i]->peaks.size(); j++ ) {
                float q = groups[i]->peaks[j].quality;
                char  l = groups[i]->peaks[j].label;
                if (l == 'g' ) gc++;
                if (l == 'g' && q > _mainwindow->mavenParameters->minQuality ) tp++;
                if (l == 'g' && q < _mainwindow->mavenParameters->minQuality ) fn++;

                if (l == 'b' ) bc++;
                if (l == 'b' && q < _mainwindow->mavenParameters->minQuality ) tn++;
                if (l == 'b' && q > _mainwindow->mavenParameters->minQuality ) fp++;
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
    LOGD;
    if (groupCount() == 0 ) return;
    _mainwindow->scatterDockWidget->setVisible(true);
    ((ScatterPlot*) _mainwindow->scatterDockWidget)->setTable(this);
    ((ScatterPlot*) _mainwindow->scatterDockWidget)->replot();
    ((ScatterPlot*) _mainwindow->scatterDockWidget)->contrastGroups();
}


void TableDockWidget::printPdfReport() {
    LOGD;
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

    //PDF report only for selected groups
    QList<PeakGroup*>selected = getSelectedGroups();

    for(int i=0; i <selected.size(); i++ ) {
        PeakGroup* grp = selected[i];
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
    LOGD;
    if ( _mainwindow->galleryWidget ) {
        _mainwindow->galleryDockWidget->setVisible(true);
        QList<PeakGroup*>selected = getSelectedGroups();
        vector<PeakGroup*>groups(selected.size());
        for(int i=0; i<selected.size(); i++) { groups[i]=selected[i]; }
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

    QAction* z0 = menu.addAction("Copy to Clipboard");
    connect(z0, SIGNAL(triggered()), this ,SLOT(setClipboard()));

    QAction* z3 = menu.addAction("Align Groups");
    connect(z3, SIGNAL(triggered()), SLOT(align()));

    QAction* z4 = menu.addAction("Find Matching Compound");
    connect(z4, SIGNAL(triggered()), SLOT(findMatchingCompounds()));

    QAction* z5 = menu.addAction("Delete All Groups");
    connect(z5, SIGNAL(triggered()), SLOT(deleteAll()));

    QAction* z6 = menu.addAction("Show Hidden Groups");
    connect(z6, SIGNAL(triggered()), SLOT(unhideFocusedGroups()));

    //Added when Merging to Maven776 - Kiran
    QAction* z7 = menu.addAction("Show Consensus Spectra");
    connect(z7, SIGNAL(triggered()), SLOT(showConsensusSpectra()));

    QAction *selectedAction = menu.exec(event->globalPos());
}



void TableDockWidget::saveModel() { 
    LOGD;
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
    float ionizationMode = _mainwindow->mavenParameters->ionizationMode;
    for(int i=0; i < allgroups.size(); i++ ) {
        PeakGroup& g = allgroups[i];
        int charge = _mainwindow->mavenParameters->getCharge(g.compound);
        QSet<Compound*>compounds = _mainwindow->massCalcWidget->findMathchingCompounds(g.meanMz, ppm, 
                    charge);
        if (compounds.size() > 0 ) Q_FOREACH( Compound*c, compounds) { g.tagString += " |" + c->name; break; }
        //cerr << g.meanMz << " " << compounds.size() << endl;
    }
    updateTable();
}

void TableDockWidget::writeQEInclusionList(QString filename) { 
    //Merged with Maven776 - Kiran
	QFile file(filename);
    if ( !file.open(QFile::WriteOnly) ) {
        QErrorMessage errDialog(this);
        errDialog.showMessage("File open " + filename + " failed");
        return; //error
    }

    QList<PeakGroup*>selected = getSelectedGroups();

	float window=1.5;
	int polarity = _mainwindow->mavenParameters->ionizationMode;
	QTextStream out(&file);
	for(int i=0; i < selected.size(); i++ ) { 
        PeakGroup* g = selected[i];
        out << g->meanMz << ",";
		polarity > 0 ? out << "Positive," : out << "Negative,";
		out << g->meanRt-window << ",";
		out << g->meanRt+window << ",";
		out << 25 << ",";     //default CE set to 25
		out << 2  << ",";
		out << groupTagString(g);
		out << endl;
	}
    file.close();
}

void TableDockWidget::writeMascotGeneric(QString filename) {
    QFile file(filename);
    if ( !file.open(QFile::WriteOnly) ) {
        QErrorMessage errDialog(this);
        errDialog.showMessage("File open " + filename + " failed");
        return; //error
    }

    QList<PeakGroup*>selected = getSelectedGroups();
    QTextStream out(&file);
    for(int i=0; i < selected.size(); i++ ) {
        PeakGroup* g = selected[i];
        Scan* cons = g->getAverageFragmenationScan(0.01);

        if (cons) {
            string scandata = cons->toMGF();
            out << scandata.c_str();
        }

        /*
        vector<Scan*> scans = g->getFragmenationEvents();
        if (scans.size() > 0) {
            for(int j=0; j< scans.size(); j++) {
                string scandata = scans[j]->toMGF();
                out << scandata.c_str();
            }
        }
        */
    }
    file.close();
}


void TableDockWidget::writeGroupXML(QXmlStreamWriter& stream, PeakGroup* g) { 
    if (!g)return;

    stream.writeStartElement("PeakGroup");
    stream.writeAttribute("groupId",  QString::number(g->groupId) );
    stream.writeAttribute("tagString",  QString(g->tagString.c_str()) );
    stream.writeAttribute("metaGroupId",  QString::number(g->metaGroupId) );
    stream.writeAttribute("clusterId", QString::number(g->clusterId) );
    stream.writeAttribute("expectedRtDiff",  QString::number(g->expectedRtDiff,'f',6) );
    stream.writeAttribute("groupRank",  QString::number(g->groupRank,'f',6) );
    stream.writeAttribute("label",  QString::number(g->label ));
    stream.writeAttribute("type",  QString::number((int)g->type()));
    stream.writeAttribute("changeFoldRatio",  QString::number(g->changeFoldRatio,'f',6));
    stream.writeAttribute("changePValue",  QString::number(g->changePValue,'e',6 ));
    if(g->srmId.length())	stream.writeAttribute("srmId",  QString(g->srmId.c_str()));

    //for sample contrasts  ratio and pvalue
    if ( g->hasCompoundLink() ) {
        Compound* c = g->compound;
		stream.writeAttribute("compoundId",  QString(c->id.c_str()));
        stream.writeAttribute("compoundDB",  QString(c->db.c_str()) );
		stream.writeAttribute("compoundName",  QString(c->name.c_str()));
        stream.writeAttribute("formula", QString(c->id.c_str()));
    }

    for(int j=0; j < g->peaks.size(); j++ ) {
        Peak& p = g->peaks[j];
        stream.writeStartElement("Peak");
        stream.writeAttribute("pos",  QString::number(p.pos,'f', 6));
        stream.writeAttribute("minpos",  QString::number(p.minpos,'f', 6));
        stream.writeAttribute("maxpos",  QString::number(p.maxpos,'f', 6));
        stream.writeAttribute("rt",  QString::number(p.rt,'f',6));
        stream.writeAttribute("rtmin",  QString::number(p.rtmin,'f',6));
        stream.writeAttribute("rtmax",  QString::number(p.rtmax,'f',6));
        stream.writeAttribute("mzmin",  QString::number(p.mzmin,'f',6));
        stream.writeAttribute("mzmax",  QString::number(p.mzmax,'f',6));
        stream.writeAttribute("scan",   QString::number(p.scan));
        stream.writeAttribute("minscan",   QString::number(p.minscan));
        stream.writeAttribute("maxscan",   QString::number(p.maxscan));
        stream.writeAttribute("peakArea",  QString::number(p.peakArea,'f',6));
        stream.writeAttribute("peakAreaCorrected",  QString::number(p.peakAreaCorrected,'f',6));
        stream.writeAttribute("peakAreaTop",  QString::number(p.peakAreaTop,'f',6));
        stream.writeAttribute("peakAreaTopCorrected",  QString::number(p.peakAreaTopCorrected,'f',6));
        stream.writeAttribute("peakAreaFractional",  QString::number(p.peakAreaFractional,'f',6));
        stream.writeAttribute("peakRank",  QString::number(p.peakRank,'f',6));
        stream.writeAttribute("peakIntensity",  QString::number(p.peakIntensity,'f',6));

        stream.writeAttribute("peakBaseLineLevel",  QString::number(p.peakBaseLineLevel,'f',6));
        stream.writeAttribute("peakMz",  QString::number(p.peakMz,'f',6));
        stream.writeAttribute("medianMz",  QString::number(p.medianMz,'f',6));
        stream.writeAttribute("baseMz",  QString::number(p.baseMz,'f',6));
        stream.writeAttribute("quality",  QString::number(p.quality,'f',6));
        stream.writeAttribute("width",  QString::number(p.width, 'f', 6));
        stream.writeAttribute("gaussFitSigma",  QString::number(p.gaussFitSigma,'f',6));
        stream.writeAttribute("gaussFitR2",  QString::number(p.gaussFitR2,'f',6));
        stream.writeAttribute("groupNum",  QString::number(p.groupNum));
        stream.writeAttribute("noNoiseObs",  QString::number(p.noNoiseObs));
        stream.writeAttribute("noNoiseFraction",  QString::number(p.noNoiseFraction,'f',6));
        stream.writeAttribute("symmetry",  QString::number(p.symmetry,'f',6));
        stream.writeAttribute("signalBaselineRatio",  QString::number(p.signalBaselineRatio, 'f', 6));
        stream.writeAttribute("groupOverlap",  QString::number(p.groupOverlap,'f',6));
        stream.writeAttribute("groupOverlapFrac",  QString::number(p.groupOverlapFrac,'f',6));
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
    g.clusterId = xml.attributes().value("clusterId").toString().toInt();
    //Updated when Merging to Maven776 - Kiran
    g.expectedRtDiff = xml.attributes().value("expectedRtDiff").toString().toFloat();
    g.groupRank = xml.attributes().value("grouRank").toString().toFloat();
    g.label     =  xml.attributes().value("label").toString().toInt();
    g.setType( (PeakGroup::GroupType) xml.attributes().value("type").toString().toInt());
    //Updated when Merging to Maven776 - Kiran
    g.changeFoldRatio = xml.attributes().value("changeFoldRatio").toString().toFloat();
    g.changePValue = xml.attributes().value("changePValue").toString().toFloat();

    string compoundId = xml.attributes().value("compoundId").toString().toStdString();
    string compoundDB = xml.attributes().value("compoundDB").toString().toStdString();
	string compoundName = xml.attributes().value("compoundName").toString().toStdString();

    string srmId = xml.attributes().value("srmId").toString().toStdString();
    if (!srmId.empty()) g.setSrmId(srmId);

    //Updated when Merging to Maven776 - Kiran
    if (!compoundName.empty() && !compoundDB.empty()) {
		vector<Compound*>matches = DB.findSpeciesByName(compoundName,compoundDB);
		if (matches.size()>0) g.compound = matches[0];
    } else if (!compoundId.empty()){
          Compound* c = DB.findSpeciesById(compoundId,DB.ANYDATABASE);
         if (c) g.compound = c;
    }

    if (!g.compound) {
        if (!compoundId.empty()) g.tagString=compoundId;
        else if (!compoundName.empty()) g.tagString=compoundName;
    }


    if (parent) {
        parent->addChild(g);
        if (parent->children.size() > 0 ) {
            _mainwindow->mavenParameters->isotopeAtom["ShowIsotopes"] = true;
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
    p.peakAreaTopCorrected = xml.attributes().value("peakAreaTopCorrected").toString().toDouble();
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
        if ( samples[i]->sampleName == sampleName ) { p.setSample(samples[i]); break;}
    }

    //cerr << "\t\t\t" << p.getSample() << " " << p.rt << endl;
    parent->addPeak(p);
}

void TableDockWidget::savePeakTable() {
    LOGD;
    if (allgroups.size() == 0 ) { 
        QString msg = "Peaks Table is Empty";
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save to Project File"),dir,
            "Maven Project File(*.mzroll)");
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
    QString selFilter;
    QStringList filters;
    filters << "Maven Project File(*.mzroll)"
            << "mzPeaks XML(*.mzPeaks *.mzpeaks)"
            << "XCMS peakTable Tab Delimited(*.tab *.csv *.txt *.tsv)";

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Load Saved Peaks",
                                                    dir,
                                                    filters.join(";;"),
                                                    &selFilter);
    if (fileName.isEmpty()) return;
    if (selFilter == filters[2]) {
        loadCSVFile(fileName,"\t");
    } else {
        loadPeakTable(fileName);
    }
    //Added when Merging to Maven776 - Kiran
	showAllGroups();
}

// void TableDockWidget::runScript() {
//     QString dir = ".";
//     QSettings* settings = _mainwindow->getSettings();

//     treeWidget->selectAll();
//     _mainwindow->getRconsoleWidget()->linkTable(this);
//     _mainwindow->getRconsoleWidget()->updateStatus();
//     _mainwindow->getRconsoleWidget()->show();
//     _mainwindow->getRconsoleWidget()->raise();

//     //find R executable
//     QString Rprogram = "R.exe";
//     if (settings->contains("Rprogram") ) Rprogram = settings->value("Rprogram").value<QString>();
//     if (!QFile::exists( Rprogram)) { QErrorMessage dialog(this); dialog.showMessage("Can't find R executable"); return; }


// }

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
            if (xml.name() == "PeakGroup") { group=readGroupXML(xml,parent); }
            if (xml.name() == "Peak" && group ) { readPeakXML(xml,group); }
            if (xml.name() == "children" && group) { stack.push(group); parent=stack.top(); }
        }

        if (xml.isEndElement()) {
            if (xml.name()=="children")  {
                if(stack.size() > 0) parent = stack.pop();
                if(parent && parent->childCount()) {
                    for(int i=0; i < parent->children.size(); i++ ) {
                        parent->children[i].minQuality = _mainwindow->mavenParameters->minQuality;
                        parent->children[i].groupStatistics();
                    }
                }
                if (stack.size()==0) parent=NULL;  }
            if (xml.name()=="PeakGroup") { 
                if(group) {
                    group->minQuality = _mainwindow->mavenParameters->minQuality;
                    group->groupStatistics();
                } 
                group  = NULL; }
        }
    }
    for(int i=0; i < allgroups.size(); i++ ) {
        allgroups[i].minQuality = _mainwindow->mavenParameters->minQuality;
        allgroups[i].groupStatistics();
    }
}

void TableDockWidget::clearClusters() {
    LOGD;
    for(unsigned int i=0; i<allgroups.size(); i++) allgroups[i].clusterId=0;
    showAllGroups();
}

void TableDockWidget::clusterGroups() {
    LOGD;
    sort(allgroups.begin(),allgroups.end(), PeakGroup::compRt);
    qDebug() << "Clustering..";
    int clusterId = 0;

    QSettings* settings = _mainwindow->getSettings();
    double maxRtDiff =  clusterDialog->maxRtDiff_2->value();
    double minSampleCorrelation =  clusterDialog->minSampleCorr->value();
    double minRtCorrelation = clusterDialog->minRt->value();
    double ppm	= _mainwindow->getUserPPM();

    vector<mzSample*> samples = _mainwindow->getSamples();

    //clear cluster information
    for(unsigned int i=0; i<allgroups.size(); i++) allgroups[i].clusterId=0;
    map<int,PeakGroup*>parentGroups;


    for(unsigned int i=0; i<allgroups.size(); i++) {
        PeakGroup& grup1 = allgroups[i];

        if (grup1.clusterId == 0) {  //create new cluster
            grup1.clusterId=++clusterId;
            parentGroups[clusterId]=&grup1;
        }

        //cluster parent
        PeakGroup* parent = parentGroups[ clusterId ];

        mzSample* largestSample=NULL;
        double maxIntensity=0; int countCheckRaghu=0;

        for(int i=0; i < grup1.peakCount(); i++ ) {
            mzSample* sample = grup1.peaks[i].getSample();
            if ( grup1.peaks[i].peakIntensity > maxIntensity ) largestSample=sample;
        }

        if (largestSample == NULL ) continue;
        vector<float>peakIntensityA = grup1.getOrderedIntensityVector(samples,PeakGroup::AreaTop);

        for(unsigned int j=i+1; j<allgroups.size(); j++) {
            PeakGroup& grup2 = allgroups[j];
            if (grup2.clusterId > 0 ) continue;

            //retention time distance
            float rtdist  = abs(parent->meanRt-grup2.meanRt);
            if (rtdist > maxRtDiff*2 ) continue;

            //retention time overlap
            float rtoverlap = mzUtils::checkOverlap(grup1.minRt, grup1.maxRt, grup2.minRt, grup2.maxRt );
            if (rtoverlap < 0.1) continue;

            //peak intensity correlation
            vector<float>peakIntensityB = grup2.getOrderedIntensityVector(samples,PeakGroup::AreaTop);
            float cor = correlation(peakIntensityA,peakIntensityB);
            if (cor < minSampleCorrelation) continue;

            //peak shape correlation
            float cor2 = largestSample->correlation(grup1.meanMz,grup2.meanMz,ppm,grup1.minRt,grup1.maxRt,
                                                    _mainwindow->mavenParameters->eicType,
                                                    _mainwindow->mavenParameters->filterline);
            if (cor2 < minRtCorrelation) continue;

            //passed all the filters.. group grup1 and grup2 into a single metagroup
            //cerr << rtdist << " " << cor << " " << cor2 << endl;
            grup2.clusterId = grup1.clusterId;
        }
        if (i%10==0) _mainwindow->setProgressBar("Clustering.,",i+1,allgroups.size());

    }

    _mainwindow->setProgressBar("Clustering., done!",allgroups.size(),allgroups.size());
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

    Q_FOREACH(QHistogramSlider* slider, sliders) {
        connect(slider,SIGNAL(minBoundChanged(double)),SLOT(filterPeakTable()));
        connect(slider,SIGNAL(maxBoundChanged(double)),SLOT(filterPeakTable()));
        layout->addWidget(slider);
    }

    filtersDialog->setLayout(layout);
}

void TableDockWidget::showFiltersDialog() {
    filtersDialog->setVisible(! filtersDialog->isVisible() );
    if (filtersDialog->isVisible() == false) return;

    Q_FOREACH(QHistogramSlider* slider, sliders) { slider->clearData(); }

    for(int i=0; i <100; i++ ) sliders["PeakQuality"]->addDataPoint(QPointF((float)i/100.00,i));
    for(int i=0; i <50; i++ ) sliders["GoodPeakCount"]->addDataPoint(QPointF(i,5));
    for(int i=0; i <100; i++ ) sliders["PeakIntensity"]->addDataPoint(QPointF(i,i));
    sliders["PeakQuality"]->setPrecision(2);
    Q_FOREACH(QHistogramSlider* slider, sliders) slider->recalculatePlotBounds();
}

void TableDockWidget::filterPeakTable() {
    updateTable();
}


void TableDockWidget::showFocusedGroups() {
    int N=treeWidget->topLevelItemCount();
    for(int i=0; i < N; i++ ) {
        QTreeWidgetItem* item = treeWidget->topLevelItem(i);
        QVariant v = item->data(0,Qt::UserRole);
        PeakGroup*  group =  v.value<PeakGroup*>();
        if (group && group->isFocused) item->setHidden(false); else item->setHidden(true);
       
        if ( item->text(0).startsWith("Cluster") ) {
            bool showParentFlag=false;
            for(int j=0; j<item->childCount();j++) {
                QVariant v = (item->child(j))->data(0,Qt::UserRole);
                PeakGroup*  group =  v.value<PeakGroup*>();
                if (group && group->isFocused) { item->setHidden(false); showParentFlag=true; } else item->setHidden(true);
            }
            if (showParentFlag) item->setHidden(false);
        }
    }
}

void TableDockWidget::clearFocusedGroups() {
    for(int i=0; i< allgroups.size();i++) {
        allgroups[i].isFocused=false;
    }
}

void TableDockWidget::unhideFocusedGroups() {
    clearFocusedGroups();
    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        (*it)->setHidden(false);
        ++it;
    }
}


void TableDockWidget::dragEnterEvent(QDragEnterEvent *event)
{
    Q_FOREACH (QUrl url, event->mimeData()->urls() ) {
        std::cerr << "dragEnterEvent:" << url.toString().toStdString() << endl;
        if (url.toString() == "ok") {
            event->acceptProposedAction();
            return;
        } else {
            return;
        }
    }
}

void TableDockWidget::dropEvent(QDropEvent *event)
 {
    Q_FOREACH (QUrl url, event->mimeData()->urls() ) {
         std::cerr << "dropEvent:" << url.toString().toStdString() << endl;
    }
 }



int TableDockWidget::loadSpreadsheet(QString fileName){
     qDebug() << "Loading SpreadSheet   : " << fileName;

     if( fileName.endsWith(".txt",Qt::CaseInsensitive)) {
       loadCSVFile(fileName,"\t");
    } else if( fileName.endsWith(".csv",Qt::CaseInsensitive)) {
        loadCSVFile(fileName,",");
    } else if( fileName.endsWith(".tsv",Qt::CaseInsensitive)) {
        loadCSVFile(fileName,"\t");
    } else if( fileName.endsWith(".tab",Qt::CaseInsensitive)) {
        loadCSVFile(fileName,"\t");
    }
}

int TableDockWidget::loadCSVFile(QString filename, QString sep="\t"){

    if(filename.isEmpty()) return 0;

    QFile myfile(filename);
    if(!myfile.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;

    QTextStream stream(&myfile);
    if (stream.atEnd()) return 0;

    QString line;
    int lineCount=0;
    QMap<QString, int>headerMap;
    QStringList header;

    do {
         line = stream.readLine();
         if (line.isEmpty() || line[0] == '#') continue;
         QStringList fields = line.split(sep);
        lineCount++;
         if (lineCount==1) { //header line
             for(int i=0; i < fields.size(); i++ ) {
                 fields[i] = fields[i].toLower();
                 fields[i].replace("\"","");
                 headerMap[ fields[i] ] = i;
                 header << fields[i];
             }
             qDebug() << header  << endl;
         } else {
            PeakGroup* g = new PeakGroup();
            if (headerMap.contains("name")) g->tagString= fields[ headerMap["name"]].toStdString();
            if (headerMap.contains("mz"))   g->meanMz= fields[ headerMap["mz"]].toFloat();
            if (headerMap.contains("mzmed")) g->meanMz= fields[ headerMap["mzmed"]].toFloat();
            if (headerMap.contains("mzmin")) g->minMz= fields[ headerMap["mzmin"]].toFloat();
            if (headerMap.contains("mzmax")) g->maxMz= fields[ headerMap["mzmax"]].toFloat();

            if (headerMap.contains("rt")) g->meanRt= fields[ headerMap["rt"]].toFloat()/60;
            if (headerMap.contains("rtmed")) g->meanRt= fields[ headerMap["rtmed"]].toFloat()/60;
            if (headerMap.contains("rtmin")) g->minRt= fields[ headerMap["rtmin"]].toFloat()/60;
            if (headerMap.contains("rtmax")) g->maxRt= fields[ headerMap["rtmax"]].toFloat()/60;

             if (headerMap.contains("fold")) g->changeFoldRatio= fields[ headerMap["fold"]].toFloat();
             if (headerMap.contains("pvalue")) g->changePValue= fields[ headerMap["pvalue"]].toFloat();

            //qDebug() << headerMap["mz"] << " " << g->meanRt;


            for(unsigned int i=14; i<header.size();i++) {
                Peak p;
                p.peakIntensity = fields[i].toInt();
                p.rt = g->meanRt; p.rtmin = g->minRt; p.rtmax=g->maxRt;
                p.peakMz = g->meanMz; p.mzmin = g->minMz; p.mzmax=g->maxMz;

                g->addPeak(p);
            }


            if (g->meanMz > 0) {
                addPeakGroup(g);
            }
            delete(g);
         }
     } while (!line.isNull());

    showAllGroups();
    return lineCount;
}


void TableDockWidget::switchTableView() {
    LOGD;
    viewType == groupView ? viewType=peakView: viewType=groupView;
    setupPeakTable();
    showAllGroups();
    updateTable();
}

MatrixXf TableDockWidget::getGroupMatrix() {
    //Merged with Maven776 - Kiran
    vector<mzSample*>vsamples = _mainwindow->getVisibleSamples();
    return getGroupMatrix(vsamples,_mainwindow->getUserQuantType());
}

MatrixXf TableDockWidget::getGroupMatrix(vector<mzSample*>& samples, PeakGroup::QType qtype) {
    //Merged with Maven776 - Kiran
    MatrixXf X;  //matrix of floats
    X = MatrixXf::Zero(allgroups.size(),samples.size());
    for(int i=0; i < allgroups.size(); i++ ) {
            vector<float>values=allgroups[i].getOrderedIntensityVector(samples,qtype);
            for(int j=0; j < values.size(); j++ ) {  X(i,j)=values[j];
            }
    }
    return X;
}

QWidget* TableToolBarWidgetAction::createWidget(QWidget *parent) {


    if (btnName == "titlePeakTable") {
 
        td->titlePeakTable = new QLabel(parent);
        QFont font;
        font.setPointSize(14);
        td->titlePeakTable->setFont(font);

        if(td->tableId==0) td->titlePeakTable->setText(" Bookmark Table  ");
        else td->titlePeakTable->setText("Peak Table "+ QString::number(td->tableId) + "  ");

        td->titlePeakTable->setStyleSheet("font-weight: bold; color: black");
        td->setWindowTitle(td->titlePeakTable->text());

        return td->titlePeakTable;

    } else if (btnName == "btnSwitchView") {

        QToolButton *btnSwitchView = new QToolButton(parent);
        btnSwitchView->setIcon(QIcon(rsrcPath + "/flip.png"));
        btnSwitchView->setToolTip("Switch between Group and Peak Views");
        connect(btnSwitchView,SIGNAL(clicked()), td, SLOT(switchTableView()));
        return btnSwitchView;

    } else if (btnName == "btnGroupCSV") {

        QToolButton *btnGroupCSV = new QToolButton(parent);

        btnGroupCSV->setIcon(QIcon(rsrcPath + "/exportcsv.png"));
        btnGroupCSV->setToolTip(tr("Export Groups To SpreadSheet (.csv) "));
        btnGroupCSV->setMenu(new QMenu("Export Groups"));
        btnGroupCSV->setPopupMode(QToolButton::InstantPopup);
        QAction* exportSelected = btnGroupCSV->menu()->addAction(tr("Export Selected"));
        QAction* exportAll = btnGroupCSV->menu()->addAction(tr("Export All Groups"));
        QAction* exportGood = btnGroupCSV->menu()->addAction(tr("Export Good"));
        QAction* exportBad = btnGroupCSV->menu()->addAction(tr("Export Bad"));

        connect(exportSelected, SIGNAL(triggered()), td, SLOT(selectedPeakSet()));
        connect(exportSelected, SIGNAL(triggered()), td,  SLOT(exportGroupsToSpreadsheet()));
        
        connect(exportAll, SIGNAL(triggered()), td,  SLOT(wholePeakSet()));
        connect(exportAll, SIGNAL(triggered()), td->treeWidget, SLOT(selectAll()));
        connect(exportAll, SIGNAL(triggered()), td,  SLOT(exportGroupsToSpreadsheet()));

        connect(exportGood, SIGNAL(triggered()), td,  SLOT(goodPeakSet()));
        connect(exportGood, SIGNAL(triggered()), td->treeWidget, SLOT(selectAll()));
        connect(exportGood, SIGNAL(triggered()), td,  SLOT(exportGroupsToSpreadsheet()));

        connect(exportBad, SIGNAL(triggered()), td,  SLOT(badPeakSet()));
        connect(exportBad, SIGNAL(triggered()), td->treeWidget, SLOT(selectAll()));
        connect(exportBad, SIGNAL(triggered()), td, SLOT(exportGroupsToSpreadsheet()));
        return btnGroupCSV;

    } else if (btnName == "btnSaveJson") {

        QToolButton* btnSaveJson = new QToolButton(parent);
        btnSaveJson->setIcon(QIcon(rsrcPath + "/JSON.png"));
        btnSaveJson->setToolTip(tr("Export EICs to Json (.json)"));
        connect(btnSaveJson, SIGNAL(clicked()), td, SLOT(exportJson()));
        return btnSaveJson;

    } else if (btnName == "btnGallery") {

        QToolButton* btnGallery = new QToolButton(parent);
        btnGallery->setIcon(QIcon(rsrcPath + "/gallery.png"));
        btnGallery->setToolTip("Show Gallery");
        connect(btnGallery, SIGNAL(clicked()), td, SLOT(showGallery()));
        return btnGallery;

    } else if (btnName == "btnScatter") {

        QToolButton* btnScatter = new QToolButton(parent);
        btnScatter->setIcon(QIcon(rsrcPath + "/scatterplot.png"));
        btnScatter->setToolTip("Show ScatterPlot");
        connect(btnScatter, SIGNAL(clicked()), td, SLOT(showScatterPlot()));
        return btnScatter;

    } else if (btnName == "btnCluster") {

        QToolButton* btnCluster = new QToolButton(parent);
        btnCluster->setIcon(QIcon(rsrcPath + "/cluster.png"));
        btnCluster->setToolTip("Cluster Groups");
        connect(btnCluster, SIGNAL(clicked()), td, SLOT(showClusterDialog()));
        return btnCluster;

    } else if (btnName == "btnTrain") {

        QToolButton* btnTrain = new QToolButton(parent);
        btnTrain->setIcon(QIcon(rsrcPath + "/train.png"));
        btnTrain->setToolTip("Train Neural Net");
        connect(btnTrain,SIGNAL(clicked()), td, SLOT(showTrainDialog()));
        return btnTrain;

    } else if (btnName == "btnXML") {

        QToolButton* btnXML = new QToolButton(parent);
        btnXML->setIcon(QIcon(rsrcPath + "/exportxml.png"));
        btnXML->setToolTip("Save Peaks");
        connect(btnXML, SIGNAL(clicked()), td, SLOT(savePeakTable()));
        return btnXML;

    } else if (btnName == "btnLoad") {

        QToolButton* btnLoad = new QToolButton(parent);
        btnLoad->setIcon(QIcon(rsrcPath + "/fileopen.png"));
        btnLoad->setToolTip("Load Peaks");
        connect(btnLoad, SIGNAL(clicked()), td->_mainwindow, SLOT(open()));
        return btnLoad;

    } else if (btnName == "btnGood") {

        QToolButton* btnGood = new QToolButton(parent);
        btnGood->setIcon(QIcon(rsrcPath + "/markgood.png"));
        btnGood->setToolTip("Mark Group as Good");
        connect(btnGood, SIGNAL(clicked()), td, SLOT(markGroupGood()));
        return btnGood;

    } else if (btnName == "btnBad") {

        QToolButton* btnBad = new QToolButton(parent);
        btnBad->setIcon(QIcon(rsrcPath + "/markbad.png"));
        btnBad->setToolTip("Mark Good as Bad");
        connect(btnBad, SIGNAL(clicked()), td, SLOT(markGroupBad()));
        return btnBad;

    } else if (btnName == "btnHeatmapelete") {
        
        QToolButton* btnHeatmapelete = new QToolButton(parent);
        btnHeatmapelete->setIcon(QIcon(rsrcPath + "/delete.png"));
        btnHeatmapelete->setToolTip("Delete Group");
        connect(btnHeatmapelete, SIGNAL(clicked()), td, SLOT(deleteGroups()));
        return btnHeatmapelete;

    } else if (btnName == "btnPDF") {

        QToolButton* btnPDF = new QToolButton(parent);
        btnPDF->setIcon(QIcon(rsrcPath + "/PDF.png"));
        btnPDF->setToolTip("Generate PDF Report");
        connect(btnPDF, SIGNAL(clicked()), td, SLOT(printPdfReport()));
        return btnPDF;

    } else if (btnName == "btnX") {

        QToolButton* btnX = new QToolButton(parent);        
        btnX->setIcon(td->style()->standardIcon(QStyle::SP_DialogCloseButton));
        connect(btnX, SIGNAL(clicked()), td, SLOT(hide()));
        return btnX;

    } else {
        return NULL;
    }
 
}
