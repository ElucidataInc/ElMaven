#include "spectralhitstable.h";

SpectralHitsDockWidget::SpectralHitsDockWidget(MainWindow* mw, QString title) {
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFloating(false);
    _mainwindow = mw;
    setObjectName(title);

    viewType = hitView;
    treeWidget=new QTreeWidget(this);
    treeWidget->setSortingEnabled(false);
    treeWidget->setDragDropMode(QAbstractItemView::DragOnly);
    treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeWidget->setAcceptDrops(false);    
    treeWidget->setObjectName("SpectralHitTable");
    connect(treeWidget, SIGNAL(itemSelectionChanged()),SLOT(showSelectedGroup()));

    this->setAcceptDrops(true);

    setWidget(treeWidget);
    setWindowTitle(title);
    setupPeakTable();

    QToolBar *toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);

    QToolButton *btnLoad = new QToolButton(toolBar);
    btnLoad->setIcon(QIcon(rsrcPath + "/fileopen.png"));
    btnLoad->setToolTip("Load Peaks");
    connect(btnLoad, SIGNAL(clicked()), SLOT(loadSpectralHitsTable()));

    QToolButton *btnX = new QToolButton(toolBar);
    btnX->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    connect(btnX, SIGNAL(clicked()),SLOT(hide()));

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    toolBar->addWidget(btnLoad);
    toolBar->addWidget(btnX);
    setTitleBarWidget(toolBar);
    setAcceptDrops(true);

}

SpectralHitsDockWidget::~SpectralHitsDockWidget() { 
}

void SpectralHitsDockWidget::updateTable() {
    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        updateItem(*it);
        ++it;
    }
    updateStatus();
}

void SpectralHitsDockWidget::updateItem(QTreeWidgetItem* item) {
    QVariant v = item->data(0,Qt::UserRole);
    SpectralHit* hit =  v.value<SpectralHit*>();
    if ( hit == NULL ) return;
    //heatmapBackground(item);
}

void SpectralHitsDockWidget::heatmapBackground(QTreeWidgetItem* item) {
    if(viewType != peakView) return;

    int firstColumn=3;
    StatisticsVector<float>values; float sum=0;
    for(unsigned int i=firstColumn; i< item->columnCount(); i++) {
          values.push_back(item->text(i).toFloat());
    }

    if (values.size()) {
        //normalize
        float mean = values.mean();
        float sd  = values.stddev();

        for(int i=0; i<values.size();i++) {
            values[i] = (values[i]-mean)/sd; //Z-score
        }

        float maxValue=max(std::fabs(values.maximum()),fabs(values.minimum()));

        float colorramp=0.5;

        for(int i=0; i<values.size();i++) {
            float cellValue=values[i];
            QColor color = Qt::white;


            if (cellValue<0)  {
                float intensity=pow(abs(cellValue/maxValue),colorramp);
                if (intensity > 1 ) intensity=1;
                color.setHsvF(0.6,intensity,intensity,0.5);
            }

            if (cellValue>0 )  {
                float intensity=pow(abs(cellValue/maxValue),colorramp);
                if (intensity > 1 ) intensity=1;
                color.setHsvF(0.1,intensity,intensity,0.5);
            }
            //item->setText(firstColumn+i,QString::number(values[i])) ;
            item->setBackgroundColor(firstColumn+i,color);
       }
    }
}

void SpectralHitsDockWidget::setupPeakTable() {

    QStringList colNames;
    colNames << "precursorMz" << "charge" << "scannum" << "score" << "matchCount" << "compoundId" << "fragmentId" << "rank";
    treeWidget->setColumnCount(colNames.size());
    treeWidget->setHeaderLabels(colNames);
    treeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);
    treeWidget->header()->adjustSize();
    treeWidget->setSortingEnabled(false);
}


void SpectralHitsDockWidget::addRow(SpectralHit* hit, QTreeWidgetItem* root) {

    if (hit == NULL) return;
    //if (hit->precursorMz <= 0 ) return;
    //if(hit->scan) scanNum = hit->scan->scannum;
    //
    //NumericTreeWidgetItem *item = NULL;
    //root ? item = new NumericTreeWidgetItem(root,0): item = new NumericTreeWidgetItem(treeWidget,0);
    
    NumericTreeWidgetItem* item = new NumericTreeWidgetItem(treeWidget,0);

    item->setData(0,Qt::UserRole,QVariant::fromValue(hit));
    item->setText(0,QString::number(hit->precursorMz, 'f', 4));
    item->setText(1,QString::number(hit->charge));
    item->setText(2,QString::number(hit->scannum));
    item->setText(3,QString::number(hit->score, 'f', 2));
    item->setText(4,QString::number(hit->matchCount));
    item->setText(5,hit->compoundId);
    item->setText(6,hit->fragmentId);
    item->setText(7,QString::number(hit->rank));
    item->setFlags(Qt::ItemIsSelectable |  Qt::ItemIsEnabled );
    if(hit->decoy) item->setBackground(0,QBrush(Qt::red));
 //  for( int i=0; i < group->childCount(); i++ ) addRow(&(group->children[i]), item);
 //
 
}

bool SpectralHitsDockWidget::hasSpectralHit(SpectralHit* group) {
    for(int i=0; i < allhits.size(); i++ ) {
        if ( allhits[i] == group ) return true;
        if ((double) std::abs(group->precursorMz - allhits[i]->precursorMz) < 1e-5 ) {
            return true;
        }
    }
    return false;
}


/*
@author: Sahil
*/
//TODO: Sahil, Added while merging mzfileio
void SpectralHitsDockWidget::limitPrecursorMz(float pMZ) {
    float ppm = _mainwindow->getUserPPM();
    for(int i=0; i < allhits.size(); i++ ) {
        if (mzUtils::withinXppm(pMZ,allhits[i]->precursorMz,ppm) ) {
            allhits[i]->isFocused=true;
        } else {
            allhits[i]->isFocused=false;
        }
    }

    showFocusedGroups();
}

SpectralHit* SpectralHitsDockWidget::addSpectralHit(SpectralHit* hit) {
    if (hit) { allhits.push_back(hit); return hit; }
    return NULL;
}

QList<SpectralHit*> SpectralHitsDockWidget::getHits() { return allhits; }

void SpectralHitsDockWidget::deleteAll() {
    treeWidget->clear();
    qDeleteAll(allhits);
    allhits.clear();
    _mainwindow->getEicWidget()->replotForced();
}


void SpectralHitsDockWidget::showAllHits() {
    treeWidget->clear();
    if (allhits.size() == 0 ) return;
    treeWidget->setSortingEnabled(false);

    qSort(allhits.begin(),allhits.end(),SpectralHit::compScore);
    for(int i=0; i < allhits.size() && i < 1000 ; i++ ) { 
	    if(allhits[i]) addRow(allhits[i], NULL);
    }


    QScrollBar* vScroll = treeWidget->verticalScrollBar();
    if ( vScroll ) { vScroll->setSliderPosition(vScroll->maximum()); }
    treeWidget->verticalScrollBar()->setEnabled(true);
    treeWidget->setSortingEnabled(true);
}

void SpectralHitsDockWidget::exportHitsToSpreadsheet() {

    if (allhits.size() == 0 ) {
        QString msg = "Peaks Table is Empty";
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();

    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    QString groupsTAB = "Hits  Summary Matrix Format (*.tab)";
    QString peaksTAB =  "Peaks   Detailed Format   (*.tab)";
    QString groupsCSV = "Hits  Summary Matrix Format Comma Delimited (*.csv)";
    QString peaksCSV =  "Peaks   Detailed Format Comma Delimited  (*.csv)";
/*
    QString sFilterSel;
    QString fileName = QFileDialog::getSaveFileName(this, 
            tr("Export Hits"), dir, 
            groupsTAB + ";;" + peaksTAB + ";;" + groupsCSV + ";;" + peaksCSV,
            &sFilterSel);

    if(fileName.isEmpty()) return;

    if ( sFilterSel == groupsCSV || sFilterSel == peaksCSV) {
        if(!fileName.endsWith(".csv",Qt::CaseInsensitive)) fileName = fileName + ".csv";
    }

    if ( sFilterSel == groupsTAB || sFilterSel == peaksTAB) {
        if(!fileName.endsWith(".tab",Qt::CaseInsensitive)) fileName = fileName + ".tab";
    }

    vector<mzSample*> samples = _mainwindow->getSamples();
    if ( samples.size() == 0) return;

    CSVReports* csvreports = new CSVReports(samples);
    csvreports->setUserQuantType( _mainwindow->getUserQuantType() );

    if (sFilterSel == groupsCSV) {
        csvreports->openGroupReport(fileName.toStdString());
    } else if (sFilterSel == groupsTAB )  {
        csvreports->openGroupReport(fileName.toStdString());
    } else if (sFilterSel == peaksCSV )  {
        csvreports->openPeakReport(fileName.toStdString());
    } else if (sFilterSel == peaksTAB )  {
        csvreports->openPeakReport(fileName.toStdString());
    }

    for(int i=0; i<allhits.size(); i++ ) {
        SpectralHit& group = allhits[i];
        csvreports->addGroup(&group);
    }
    csvreports->closeFiles();
*/
}


void SpectralHitsDockWidget::showSelectedGroup() { 
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) return;

    QVariant v = item->data(0,Qt::UserRole);
    SpectralHit*  hit =  v.value<SpectralHit*>();

    if ( hit != NULL && _mainwindow != NULL) {
        _mainwindow->getEicWidget()->setMzSlice(hit->precursorMz);
        if(_mainwindow->fragPanel->isVisible()) _mainwindow->showFragmentationScans(hit->precursorMz);

	//get sample by samplename
	vector<mzSample*>samples = _mainwindow->getVisibleSamples(); 
	for(int i=0; i<samples.size(); i++) {
		qDebug() << samples[i]->sampleName.c_str() << " <-- " << hit->sampleName;
		if(samples[i]->sampleName == hit->sampleName.toStdString() ) {
			Scan* scan =  samples[i]->getScan( hit->scannum-1 );
        		if(scan) _mainwindow->getSpectraWidget()->setScan(scan);
			break;
		}
	}
    }
/*
    if ( item->childCount() > 0 ) {
        vector<SpectralHit*>children;
        for(int i=0; i < item->childCount(); i++ ) {
            QTreeWidgetItem* child = item->child(i);
            QVariant data = child->data(0,Qt::UserRole);
            SpectralHit*  group =  data.value<SpectralHit*>();
            if(group) children.push_back(group);
        }
    }
*/
}

QList<SpectralHit*> SpectralHitsDockWidget::getSelectedHits() {
    QList<SpectralHit*> selectedHits;
    foreach(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
        if (item) {
            QVariant v = item->data(0,Qt::UserRole);
            SpectralHit*  hit =  v.value<SpectralHit*>();
            if ( hit != NULL ) { selectedHits.append(hit); }
        }
    }
    return selectedHits;
}

SpectralHit* SpectralHitsDockWidget::getSelectedGroup() { 
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) return NULL;
    QVariant v = item->data(0,Qt::UserRole);
    SpectralHit*  group =  v.value<SpectralHit*>();
    if ( group != NULL ) { return group; }
    return NULL;
}

void SpectralHitsDockWidget::setGroupLabel(char label) {
/*
    foreach(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
        if (item) {
            QVariant v = item->data(0,Qt::UserRole);
            SpectralHit*  group =  v.value<SpectralHit*>();
            if ( group != NULL ) {
                 group->setLabel(label);
            }
            updateItem(item);
        }
    }
    updateStatus();
*/
}

void SpectralHitsDockWidget::deleteGroup() {
/*
    QTreeWidgetItem *item = treeWidget->currentItem();
    if ( item == NULL ) return;

    SpectralHit* group = getSelectedGroup();
    if ( group == NULL ) return;

    SpectralHit* parentGroup = group->parent;

    if ( parentGroup == NULL ) { //top level item
        for(int i=0; i < allhits.size(); i++) {
            if ( &allhits[i] == group ) {
                treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(item));
                treeWidget->update();
                delete(item);
                allhits.erase(allhits.begin()+i);
                break;
            }
        }
    } else if ( parentGroup && parentGroup->childCount() ) {	//this a child item
        if ( parentGroup->deleteChild(group) ) {
            QTreeWidgetItem* parentItem = item->parent();
            if ( parentItem ) { parentItem->removeChild(item); delete(item); }
            treeWidget->update();
        }
    }
    _mainwindow->getEicWidget()->replotForced();
    return;
*/
}

void SpectralHitsDockWidget::setClipboard() { 
/*
    QList<SpectralHit*>groups = getSelectedHits();
    if (groups.size() >0) {
        _mainwindow->isotopeWidget->setClipboard(groups);
    }
*/
}

void SpectralHitsDockWidget::markGroupGood() { 
 //   setGroupLabel('g');
  //  showNextGroup();
}

void SpectralHitsDockWidget::markGroupBad() { 
   // setGroupLabel('b');
    //showNextGroup();
}

void SpectralHitsDockWidget::markGroupIgnored() { 
    //setGroupLabel('i');
    //showNextGroup();
}

void SpectralHitsDockWidget::showSpectralHit(int row) {
/*
    QTreeWidgetItem *item = treeWidget->itemAt(row,0);
    if ( item == NULL) return;

    QVariant v = item->data(0,Qt::UserRole);
    SpectralHit*  group =  v.value<SpectralHit*>();

    if ( group != NULL ) {
        treeWidget->setCurrentItem(item);
        _mainwindow->setSpectralHit(group);
    }
*/
}

void SpectralHitsDockWidget::showLastGroup() {
/*
    QTreeWidgetItem *item= treeWidget->currentItem();
    if ( item != NULL )  {
        treeWidget->setCurrentItem(treeWidget->itemAbove(item));
    }
*/
}

void SpectralHitsDockWidget::showNextGroup() {
/*
    QTreeWidgetItem *item= treeWidget->currentItem();
    if ( item == NULL ) return;

    QTreeWidgetItem* nextitem = treeWidget->itemBelow(item); //get next item
    if ( nextitem != NULL )  treeWidget->setCurrentItem(nextitem);
*/
}


void SpectralHitsDockWidget::keyPressEvent(QKeyEvent *e ) {
    //cerr << "SpectralHitsDockWidget::keyPressEvent()" << e->key() << endl;
/*
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (e->key() == Qt::Key_Delete ) {
        deleteGroup();
    } else if ( e->key() == Qt::Key_G ) {
        markGroupGood();
    } else if ( e->key() == Qt::Key_B ) {
        markGroupBad();
    }
    QDockWidget::keyPressEvent(e);
    updateStatus();
*/
}

void SpectralHitsDockWidget::updateStatus() {
/*
    int totalCount=0;
    int goodCount=0;
    int badCount=0;
    int ignoredCount=0;
    int predictedGood=0;
    for(int i=0; i < allhits.size(); i++ ) {
        char groupLabel = allhits[i].label;
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
*/
}


void SpectralHitsDockWidget::showScatterPlot() { 
/*
    if (groupCount() == 0 ) return;
    _mainwindow->scatterDockWidget->setVisible(true);
    ((ScatterPlot*) _mainwindow->scatterDockWidget)->setTable(this);
    ((ScatterPlot*) _mainwindow->scatterDockWidget)->replot();
    ((ScatterPlot*) _mainwindow->scatterDockWidget)->contrastHits();
*/
}


void SpectralHitsDockWidget::printPdfReport() {
/*
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
    QList<SpectralHit*>selected = getSelectedHits();

    for(int i=0; i <selected.size(); i++ ) {
        SpectralHit* grp = selected[i];
        _mainwindow->getEicWidget()->setSpectralHit(grp);
        _mainwindow->getEicWidget()->render(&painter);

        if (! printer.newPage()) {
            qWarning("failed in flushing page to disk, disk full?");
            return;
        }
    }
    painter.end();
*/
 }


void SpectralHitsDockWidget::showGallery() { 
/*
    if ( _mainwindow->galleryWidget ) {
        _mainwindow->galleryDockWidget->setVisible(true);
        QList<SpectralHit*>selected = getSelectedHits();
        vector<SpectralHit*>groups(selected.size());
        for(int i=0; i<selected.size(); i++) { groups[i]=selected[i]; }
        _mainwindow->galleryWidget->clear();
        _mainwindow->galleryWidget->addEicPlots(groups);
    }
*/
}


void SpectralHitsDockWidget::showTreeMap() { 

    /*
	_mainwindow->treeMapDockWidget->setVisible(true);
	TreeMap* _treemap = _mainwindow->treemap;
	if ( _treemap ) {
			_treemap->setTable(this);
			_treemap->replot();
	}
	*/
}

void SpectralHitsDockWidget::contextMenuEvent ( QContextMenuEvent * event ) 
{
    QMenu menu;
/*
    QAction* z0 = menu.addAction("Copy to Clipboard");
    connect(z0, SIGNAL(triggered()), this ,SLOT(setClipboard()));

    QAction* z3 = menu.addAction("Align Hits");
    connect(z3, SIGNAL(triggered()), SLOT(align()));

    QAction* z4 = menu.addAction("Find Matching Compound");
    connect(z4, SIGNAL(triggered()), SLOT(findMatchingCompounds()));

    QAction* z5 = menu.addAction("Delete All Hits");
    connect(z5, SIGNAL(triggered()), SLOT(deleteAll()));

    QAction* z6 = menu.addAction("Show Hidden Hits");
    connect(z6, SIGNAL(triggered()), SLOT(unhideFocusedHits()));

    QAction *selectedAction = menu.exec(event->globalPos());


    QMenu analysis("Cluster Analysis");
    QAction* zz1 = analysis.addAction("Collapse All");
    connect(zz1, SIGNAL(triggered()), treeWidget,SLOT(collapseAll()));
    QAction* zz2 = analysis.addAction("Expand All");
    connect(zz2, SIGNAL(triggered()), treeWidget,SLOT(expandAll()));

    menu.addMenu(&analysis);
*/
}



void SpectralHitsDockWidget::writeGroupXML(QXmlStreamWriter& stream, SpectralHit* g) { 
/*
    if (!g)return;

    stream.writeStartElement("SpectralHit");
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
            SpectralHit* child = &(g->children[i]);
            writeGroupXML(stream,child);
        }
        stream.writeEndElement();
    }
    stream.writeEndElement();
*/
}

void SpectralHitsDockWidget::writePeakTableXML(QXmlStreamWriter& stream) {
/*
    if (allhits.size() ) {
        stream.writeStartElement("PeakHits");
        for(int i=0; i < allhits.size(); i++ ) writeGroupXML(stream,&allhits[i]);
        stream.writeEndElement();
    }
*/
}

SpectralHit* SpectralHitsDockWidget::readGroupXML(QXmlStreamReader& xml,SpectralHit* parent) {
/*
    SpectralHit g;
    SpectralHit* gp=NULL;

    g.groupId = xml.attributes().value("groupId").toString().toInt();
    g.tagString = xml.attributes().value("tagString").toString().toStdString();
    g.metaGroupId = xml.attributes().value("metaGroupId").toString().toInt();
    g.expectedRtDiff = xml.attributes().value("expectedRtDiff").toString().toDouble();
    g.groupRank = xml.attributes().value("grouRank").toString().toInt();
    g.label     =  xml.attributes().value("label").toString().toInt();
    g.setType( (SpectralHit::GroupType) xml.attributes().value("type").toString().toInt());
    g.changeFoldRatio = xml.attributes().value("changeFoldRatio").toString().toDouble();
    g.changePValue = xml.attributes().value("changePValue").toString().toDouble();

    string compoundId = xml.attributes().value("compoundId").toString().toStdString();
    string compoundDB = xml.attributes().value("compoundDB").toString().toStdString();
	string compoundName = xml.attributes().value("compoundName").toString().toStdString();

    string srmId = xml.attributes().value("srmId").toString().toStdString();
    if (!srmId.empty()) g.setSrmId(srmId);

	if (!compoundId.empty()){
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
        gp = addSpectralHit(&g);
        //cerr << "addParent() " << gp << endl;
    }

    return gp;
*/
}

void SpectralHitsDockWidget::readPeakXML(QXmlStreamReader& xml,SpectralHit* parent) {
/*
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
        if ( samples[i]->sampleName == sampleName ) { p.setSample(samples[i]); break;}
    }

    //cerr << "\t\t\t" << p.getSample() << " " << p.rt << endl;
    parent->addPeak(p);
*/
}

void SpectralHitsDockWidget::savePeakTable() {
/*
    if (allhits.size() == 0 ) { 
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
*/
}

void SpectralHitsDockWidget::savePeakTable(QString fileName) {
/*
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
*/
}

void SpectralHitsDockWidget::loadSpectralHitsTable() {
    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();
    QString selFilter;
    QStringList filters;
    filters << "pepXML File (*.pepXML)";

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Load Spectral Hits",
                                                    dir,
                                                    filters.join(";;"),
                                                    &selFilter);
    if (fileName.isEmpty()) return;
    if (selFilter == filters[0]) {
        loadPepXML(fileName);
    }
    showAllHits();
}

void SpectralHitsDockWidget::loadPepXML(QString fileName) {

    qDebug() << "Loading pepXML sample: " << fileName;
    QFile data(fileName);
    QString dbname = QString( mzUtils::cleanFilename(fileName.toStdString()).c_str() );

    if (!data.open(QFile::ReadOnly) ) {
        qDebug() << "Can't open " << fileName;
    }

    QXmlStreamReader xml(&data);
    xml.setNamespaceProcessing(false);
    QList<QStringRef> taglist;

/*
    <spectrum_query spectrum="BSA_run_120909192952.2.2.2" spectrumNativeID="controllerType=0 controllerNumber=1 scan=2" start_scan="2" end_scan="2" precursor_neutral_mass="887.14544706624" assumed_charge="2" index="2">
      <search_result num_target_comparisons="0" num_decoy_comparisons="0">
        <search_hit hit_rank="1" peptide="SCHTGLGR" peptide_prev_aa="K" peptide_next_aa="S" protein="sp|P02787|TRFE_HUMAN" num_tot_proteins="1" calc_neutral_pep_mass="886.94606" massdiff="-0.19938706624" num_tol_term="2" num_missed_cleavages="0" num_matched_ions="5" tot_num_ions="42">
          <modification_info>
            <mod_aminoacid_mass position="2" mass="160.0306444778"/>
          </modification_info>
          <search_score name="number of matched peaks" value="5"/>
          <search_score name="number of unmatched peaks" value="37"/>
*/
 
    int   hitCount=0;
    int   charge=0;
    float precursorMz=0;
    int   scannum=0;
    SpectralHit* lasthit=NULL;

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
                taglist << xml.name();
                if (xml.name() == "spectrum_query") {
                    scannum = xml.attributes().value("start_scan").toString().toInt();
                    charge = xml.attributes().value("assumed_charge").toString().toInt();
                    precursorMz = xml.attributes().value("precursor_neutral_mass").toString().toDouble();
		    if (charge) { precursorMz = (precursorMz+charge)/charge; }
                } else if (xml.name() == "search_hit") {
		    hitCount++;
                    int hit_rank = xml.attributes().value("hit_rank").toString().toInt();
                    int num_matched_ions = xml.attributes().value("num_matched_ions").toString().toInt();
                    double massdiff = xml.attributes().value("massdiff").toString().toInt();
                    QString peptide = xml.attributes().value("peptide").toString();
                    QString protein = xml.attributes().value("protein").toString();

		    bool decoy = false;
		    if (protein.startsWith("r-") or protein.startsWith("DECOY")) decoy=true;

		    if(precursorMz and hit_rank < 2) {
			    SpectralHit* hit = new SpectralHit();
			    hit->scannum = scannum;
			    hit->precursorMz = precursorMz;
			    hit->charge = charge;
			    hit->decoy = decoy;
			    hit->rank = hit_rank;
			    hit->matchCount = num_matched_ions;
			    hit->compoundId = protein;
			    hit->fragmentId  = peptide;
			    hit->massdiff = massdiff;
			    hit->sampleName = dbname;
			    addSpectralHit(hit);
			    lasthit = hit;
		    } else {
			   lasthit = NULL;
		    }

                } else if (xml.name() == "mod_aminoacid_mass" ) {
                    int pos =          xml.attributes().value("position").toString().toInt();
		    double massshift = xml.attributes().value("mass").toString().toDouble();
                } else if (xml.name() == "search_score" ) {
                    QString name = xml.attributes().value("name").toString();
                    QString value = xml.attributes().value("value").toString();
		    if(lasthit and name == "mvh")   {  lasthit->mvh = value.toDouble();  lasthit->score=value.toDouble(); }
		    if(lasthit and name == "xcorr") {  lasthit->xcorr = value.toDouble(); }
                }
        } else if (xml.isEndElement()) {
               if (!taglist.isEmpty()) taglist.pop_back();
        }
    }
    data.close();
}


void SpectralHitsDockWidget::runScript() {
/*
    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();

    treeWidget->selectAll();
    _mainwindow->getRconsoleWidget()->linkTable(this);
    _mainwindow->getRconsoleWidget()->updateStatus();
    _mainwindow->getRconsoleWidget()->show();
    _mainwindow->getRconsoleWidget()->raise();

    //find R executable
    QString Rprogram = "R.exe";
    if (settings->contains("Rprogram") ) Rprogram = settings->value("Rprogram").value<QString>();
    if (!QFile::exists( Rprogram)) { QErrorMessage dialog(this); dialog.showMessage("Can't find R executable"); return; }

*/
}

void SpectralHitsDockWidget::showFocusedHits() {
/*
    int N=treeWidget->topLevelItemCount();
    for(int i=0; i < N; i++ ) {
        QTreeWidgetItem* item = treeWidget->topLevelItem(i);
        QVariant v = item->data(0,Qt::UserRole);
        SpectralHit*  group =  v.value<SpectralHit*>();
        if (group && group->isFocused) item->setHidden(false); else item->setHidden(true);
       
        if ( item->text(0).startsWith("Cluster") ) {
            bool showParentFlag=false;
            for(int j=0; j<item->childCount();j++) {
                QVariant v = (item->child(j))->data(0,Qt::UserRole);
                SpectralHit*  group =  v.value<SpectralHit*>();
                if (group && group->isFocused) { item->setHidden(false); showParentFlag=true; } else item->setHidden(true);
            }
            if (showParentFlag) item->setHidden(false);
        }
    }
*/
}

void SpectralHitsDockWidget::clearFocusedHits() {
  //  for(int i=0; i< allhits.size();i++) { allhits[i].isFocused=false; }
}


/*
@author: Sahil
*/
//TODO: Sahil, Added while merging mzfileio
void SpectralHitsDockWidget::showFocusedGroups() {
    int N=treeWidget->topLevelItemCount();
    for(int i=0; i < N; i++ ) {
        QTreeWidgetItem* item = treeWidget->topLevelItem(i);
        QVariant v = item->data(0,Qt::UserRole);
        SpectralHit*  group =  v.value<SpectralHit*>();
        if (group && group->isFocused) item->setHidden(false); else item->setHidden(true);
    }
}


void SpectralHitsDockWidget::unhideFocusedHits() {
   // clearFocusedHits();
    //QTreeWidgetItemIterator it(treeWidget);
   // while (*it) {
    //    (*it)->setHidden(false);
     //   ++it;
    //}
}


/*
@author: Sahil
*/
//TODO: Sahil, Added while merging mzfileio
void SpectralHitsDockWidget::loadIdPickerDB(QString fileName) {
    qDebug()  << "loadIdPickerDB() " << fileName << endl;
    this->idpickerDB = QSqlDatabase::addDatabase("QSQLITE", "idpickerDB");
    this->idpickerDB.setDatabaseName(fileName);
    this->idpickerDB.open();

    if (not this->idpickerDB.isOpen()) {
        qDebug()  << "Failed to open " + fileName;
    }

    queryIdPickerProteins();
    queryIdPickerPrecursors(0.02);
    getRetentionTimes();
}

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging mzfileio
void SpectralHitsDockWidget::queryIdPickerPrecursors(float qValue) {
    if ( not idpickerDB.isOpen()) return;

    qDebug() << "Loading peptides matched with qvalue < " << qValue;

    QRegExp scanNumber("scan=(\\d+)");

    QString querySQL = QString("SELECT \
                     PSM.Id, \
                     substr(Prot.Sequence,PI.Offset+1,PI.Length) as peptideSeq, \
                     Pep.MonoisotopicMass, \
                     Spectrum.NativeID,\
                     Spectrum.precursorMZ, \
                     PSM.charge,\
                     Spectrum.ScanTimeInSeconds, \
                     Spectrum.NativeID,  \
                     PSM.QValue,  \
                     Protein.Accession, \
                     Protein.IsDecoy, \
                     SpectrumSource.Name as sampleName,\
                     PSM.rank, \
                     PI.Offset+1 \
                     FROM \
                     peptideSpectrumMatch PSM, peptideInstance PI, ProteinData Prot, Peptide Pep, Protein, Spectrum, SpectrumSource \
                     where QValue < ? and \
                     Protein.IsDecoy = 0 and \
                     PSM.Peptide = Pep.Id and  \
                     Pep.Id = PI.Peptide  and  \
                     PI.Protein  = Prot.Id and  \
                     Prot.Id = Protein.Id and  \
                     SpectrumSource.Id = Spectrum.Source and\
                     Spectrum.Id = PSM.Spectrum");


    QString sqlQueryPSM(querySQL);
    QSqlQuery query(idpickerDB);
    query.prepare(sqlQueryPSM);
    query.addBindValue(QString::number(qValue,'f',6));

    if (!query.exec())   qDebug() << query.lastError();

    QVector<SpectralHit*> hitlist;
    QMap<int,SpectralHit*> hitIdMap;


    int hitCount=0;
    while (query.next()) {
      int hitId = query.value(0).toInt();
      hitCount++;

      QString proteinId = query.value(9).toString();

      int proteinPos  = query.value(13).toInt();

      SpectralHit* hit = NULL;
      if (! hitIdMap.count(hitId)) {
          hit = new SpectralHit();
          hit->id = query.value(0).toInt();
          hit->unmodPeptideSeq  = query.value(1).toString();
          hit->precursorMz = query.value(4).toDouble();
          hit->charge = query.value(5).toInt();
          hit->rt = query.value(6).toDouble()/60;
          hit->score = query.value(8).toDouble();	//qvalue
          hit->decoy = query.value(10).toInt();
          hit->sampleName = query.value(11).toString();
          hit->rank = query.value(12).toInt();
          //hit->matchCount = num_matched_ions;
          //hit->massdiff = massdiff;
          //hit->sampleName = dbname;

          if ( query.value(3).toString().contains(scanNumber)) {
              int scannum = scanNumber.capturedTexts().at(1).toInt();
              hit->scannum=scannum;
          }
            hitlist << hit;
            hitIdMap[hit->id]=hit;
            hit->proteins[proteinId]=proteinPos;
      } else {
            hit = hitIdMap[hitId];
            hit->proteins[proteinId]=proteinPos;
      }

      if (hit) allhits.push_back(hit);
      if(hitCount and hitCount % 1000 == 0) { qDebug() << "PSMS=" << hitCount << "  Peptides=" << hitIdMap.size(); }
    } //end all spectral hits

    //get modificaitons
    qDebug() << "\t getting modifications";
    QSqlQuery queryMods(idpickerDB);
    QString sqlQueryMods = "SELECT PeptideSpectrumMatch, Offset, MonoMassDelta from PeptideModification A ,Modification B where A.Modification = B.Id";
    queryMods.prepare(sqlQueryMods);
    if (!queryMods.exec())   qDebug() << queryMods.lastError();

    while (queryMods.next()) {
        int hitId = queryMods.value(0).toInt();
        int pos = queryMods.value(1).toInt();
        float massShift = queryMods.value(2).toFloat();

        if(hitIdMap.contains(hitId)) {
            SpectralHit* hit = hitIdMap[hitId];
            hit->mods[pos]=massShift;
        }
    }

    qDebug() << "\t checking precurursor";
    foreach(SpectralHit* hit, hitlist) {
        hit->fragmentId=hit->getModPeptideString();
        //qDebug() << hit->fragmentId << " " << confirmMods(hit);

        Peptide record(hit->fragmentId.toStdString(),hit->charge);
        float theoryMz = record.monoisotopicMZ();
        if ( abs(theoryMz - hit->precursorMz ) > 0.05 ) {
            qDebug() << "CHECK: id=" << hit->id << " mods=" << hit->mods.size() << " " << hit->fragmentId << " pull:" << hit->precursorMz << " theory:" << theoryMz;
        }
        //hit->precursorMz = theoryMz;
        //hit->precursorMz = theoryMz;
    }

    qDebug() << "queryIdPickerPrecursors() loaded " << allhits.size();
    //exit(-1);
}

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging mzfileio
void SpectralHitsDockWidget::queryIdPickerProteins() {
    if ( not idpickerDB.isOpen()) return;

    QSqlQuery query(idpickerDB);
    QString querySQL("select A.Id, Accession, isDecoy, Cluster,ProteinGroup,Length,Description,Sequence from Protein A, ProteinMetadata B, ProteinData D where A.id = B.id and  B.id=D.id");
    query.prepare(querySQL);
    qDebug() << "QUERY:" << querySQL;
    if (!query.exec())   qDebug() << query.lastError();


    QRegExp geneSymbol("Gene_Symbol=(\\S+)");
    int rowCount=query.size(); int rowCounter=0;
    while (query.next()) {
      rowCounter++;
      ProteinHit* protein = new ProteinHit();
      //hit->scannum = scannum;
      protein->id = query.value(0).toInt();
      protein->accession=query.value(1).toString();
      protein->isDecoy = query.value(2).toInt();
      protein->cluster = query.value(3).toInt();
      protein->proteinGroup = query.value(4).toInt();
      protein->length = query.value(5).toInt();
      protein->description = query.value(6).toString();
      protein->sequence = query.value(7).toString();

      if(protein->description.contains(geneSymbol)) {
          protein->geneSymbol=geneSymbol.capturedTexts().at(1);
      }

      proteinAccessionMap[protein->accession] = protein;
      if (proteinAccessionMap.size() % 10 == 0) {
        //   emit(updateProgressBar("Loading Protein Sequences", rowCounter,rowCount));
      }
    }
}

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging mzfileio
void SpectralHitsDockWidget::getRetentionTimes() {
    qDebug() << "getRetentionTimes()";
    foreach(SpectralHit* hit, allhits)
    {
        if(hit->rt > 0) continue;
        if(hit->scannum == 0) continue;

        mzSample* sample = _mainwindow->getSampleByName(hit->sampleName);
        if(!sample) _mainwindow->getSampleByName(hit->sampleName + ".mzXML");

        if(!sample) {
           // qDebug() << "Can't find scan data for sample " << hit->sampleName;
            continue;
        }


        Scan* scan = sample->getScan(hit->scannum);
        if (scan) {
           // qDebug() << hit->sampleName << " " << hit->scan << " rt=" << scan->rt;
            hit->rt = scan->rt;
            hit->scan  = scan;
        }
    }
}



void SpectralHitsDockWidget::dragEnterEvent(QDragEnterEvent *event)
{
/*
    foreach (QUrl url, event->mimeData()->urls() ) {
        std::cerr << "dragEnterEvent:" << url.toString().toStdString() << endl;
        if (url.toString() == "ok") {
            event->acceptProposedAction();
            return;
        } else {
            return;
        }
    }
*/
}

void SpectralHitsDockWidget::dropEvent(QDropEvent *event)
 {
/*
    foreach (QUrl url, event->mimeData()->urls() ) {
         std::cerr << "dropEvent:" << url.toString().toStdString() << endl;
    }
*/
 }



int SpectralHitsDockWidget::loadSpreadsheet(QString fileName){
/*
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
*/
}

int SpectralHitsDockWidget::loadCSVFile(QString filename, QString sep="\t"){

    if(filename.isEmpty()) return 0;

    QFile myfile(filename);
    if(!myfile.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;

    QTextStream stream(&myfile);
    if (stream.atEnd()) return 0;

    QString line;
    int lineCount=0;
    QMap<QString, int>headerMap;
    QStringList header;

/*
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
            SpectralHit* g = new SpectralHit();
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

            qDebug() << headerMap["mz"] << " " << g->meanRt;


            for(unsigned int i=14; i<header.size();i++) {
                Peak p;
                p.peakIntensity = fields[i].toInt();
                p.rt = g->meanRt; p.rtmin = g->minRt; p.rtmax=g->maxRt;
                p.peakMz = g->meanMz; p.mzmin = g->minMz; p.mzmax=g->maxMz;

                g->addPeak(p);
            }


            if (g->meanMz > 0) {
                addSpectralHit(g);
            }
            delete(g);
         }
     } while (!line.isNull());

    showAllHits();
    */
    return lineCount;
}


void SpectralHitsDockWidget::switchTableView() {
/*
    viewType == groupView ? viewType=peakView: viewType=groupView;
    setupPeakTable();
    showAllHits();
    updateTable();
*/
}
