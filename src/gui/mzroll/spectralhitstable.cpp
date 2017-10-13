#include "spectralhitstable.h";

SpectralHitsDockWidget::SpectralHitsDockWidget(MainWindow* mw, QString title) {
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFloating(false);
    _mainwindow = mw;
    filterDialog=new QDialog();
    setObjectName(title);

    // viewType = hitView
    viewType = peakView;
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
    // setupPeakTable();

    QToolBar *toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);

    QToolButton *btnLoad = new QToolButton(toolBar);
    btnLoad->setIcon(QIcon(rsrcPath + "/fileopen.png"));
    btnLoad->setToolTip("Load Peaks");
    connect(btnLoad, SIGNAL(clicked()), SLOT(loadSpectralHitsTable()));


    /*
    @author: Kiran - Sahil
    */
    //TODO: Added while merging spectralhitstable
    QToolButton *btnFilter = new QToolButton(toolBar);
    btnFilter->setIcon(QIcon(rsrcPath + "/filter.png"));
    btnFilter->setToolTip("Load Peaks");
    btnFilter->setCheckable(true);
    btnFilter->setChecked(false);
    //connect(btnFilter, SIGNAL(clicked(bool)), SLOT(unhideFocusedHits()));
    connect(btnFilter, SIGNAL(clicked()), SLOT(showFiltersDialog()));

    /*
    @author: Kiran - Sahil
    */
    //TODO: Added while merging spectralhitstable
    QToolButton *btnIntegrate = new QToolButton(toolBar);
    btnIntegrate->setIcon(QIcon(rsrcPath + "/integrateArea.png"));
    btnIntegrate->setToolTip("Integrate MS1 Events");
    btnIntegrate->setChecked(false);
    connect(btnIntegrate, SIGNAL(clicked()), SLOT(integrateMS1()));


    /*
    @author: Kiran - Sahil
    */
    //TODO: Added while merging spectralhitstable
    QToolButton *btnExportTSV = new QToolButton(toolBar);
    btnExportTSV->setIcon(QIcon(rsrcPath + "/exportcsv.png"));
    btnExportTSV->setToolTip("Export to TSV");
    btnExportTSV->setCheckable(false);
    connect(btnExportTSV, SIGNAL(clicked()), SLOT(exportToTSV()));


    QToolButton *btnX = new QToolButton(toolBar);
    btnX->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    connect(btnX, SIGNAL(clicked()),SLOT(hide()));

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    toolBar->addWidget(btnLoad);
    toolBar->addWidget(btnFilter); //TODO: Kiran-Sahil, Added while merging spectralhitstable
    toolBar->addWidget(btnIntegrate); //TODO: Kiran-Sahil, Added while merging spectralhitstable
    toolBar->addWidget(btnExportTSV); //TODO: Kiran-Sahil, Added while merging spectralhitstable

    toolBar->addWidget(btnX);
    setTitleBarWidget(toolBar);
    setAcceptDrops(true);

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


void SpectralHitsDockWidget::addRow(SpectralHit* hit, QTreeWidgetItem* root) {
    if (hit == NULL) return;
    NumericTreeWidgetItem* item = new NumericTreeWidgetItem(treeWidget,0);
    item->setData(0,Qt::UserRole,QVariant::fromValue(hit));
    item->setText(0,hit->sampleName);
    item->setText(1,hit->getProteinIds());
    item->setText(2,hit->fragmentId);
    item->setText(3,QString::number(hit->rt, 'f', 2));
    item->setText(4,QString::number(hit->precursorMz, 'f', 4));
    item->setText(5,QString::number(hit->charge));
    item->setText(6,QString::number(hit->scannum));
    item->setText(7,QString::number(hit->score, 'f', 2));
    item->setText(8,QString::number(hit->matchCount));
    item->setText(9,QString::number(hit->rank));
    item->setFlags(Qt::ItemIsSelectable |  Qt::ItemIsEnabled );
    if(hit->decoy) item->setBackground(0,QBrush(Qt::red));
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
    //Merged with Maven776 - Kiran
    if (viewType == hitView) {
        showSpectralHits();
    } else {
        showSpectralCounts();
    }
}

void SpectralHitsDockWidget::showSpectralCounts() {

    treeWidget->clear();

    QMap<QString, QMap<QString,float> >pivitTable;
    QSet<QString>colNames;
    QMap<QString,SpectralHit*> bestHit;

    Q_FOREACH(SpectralHit* hit, allhits) {
        QString rowId = hit->fragmentId;
        QString colId = hit->sampleName;

        if(!pivitTable.contains(rowId)) {
            pivitTable[rowId][colId]=1;
        } else {
            pivitTable[rowId][colId]++;
        }

        if(bestHit.count(rowId)==0 or bestHit[rowId]->score > hit->score) bestHit[rowId]=hit;
        colNames << colId;
   }

    QStringList rowNames = pivitTable.keys();

    QStringList headerNames;
    headerNames << "Peptide" << "Protein" << "GeneSymbol" << "rt" << "preMz" << colNames.toList();
    treeWidget->setColumnCount(headerNames.size());
    treeWidget->setHeaderLabels(headerNames);
    //treeWidget->header()->setResizeMode(QHeaderView::Interactive);
    //treeWidget->header()->adjustSize();
    treeWidget->setSortingEnabled(false);

    QColor color = Qt::white;


    Q_FOREACH(QString rowId, rowNames) {
            NumericTreeWidgetItem* item = new NumericTreeWidgetItem(treeWidget,0);
            SpectralHit* hit = bestHit[rowId];

            item->setText(0,rowId);
            if(hit->getProteins().size()>0) {
                item->setText(1,hit->getProteinIds());
                item->setText(2,uniqGeneList(hit).join(";"));
            }
            item->setText(3,QString::number(hit->rt, 'f', 2));
            item->setText(4,QString::number(hit->precursorMz, 'f', 4));


            item->setData(0,Qt::UserRole,QVariant::fromValue(hit));
            for(int i=5; i < 5+colNames.size(); i++) {
                QString colId = headerNames[i];
                float value = pivitTable[rowId][colId];
                float prob =  1.0 - 1.2*exp(-0.5*value);
                if(prob < 0 ) prob=0;
                color.setHsvF(0.4+prob/5,prob,1,1);
                item->setText(i,QString::number(value, 'f', 0));
                item->setBackgroundColor(i,color);
           }

    }

    QScrollBar* vScroll = treeWidget->verticalScrollBar();
    if ( vScroll ) { vScroll->setSliderPosition(vScroll->maximum()); }
    treeWidget->verticalScrollBar()->setEnabled(true);
    treeWidget->setSortingEnabled(true);

}

//TODO: Kiran-Sahil, replacement of function exportHItsToSpreadSheet 
void SpectralHitsDockWidget::exportToTSV(){

    if (allhits.size() == 0 ) {
        QString msg = "Peaks Table is Empty";
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();

    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    QString groupsTAB = "Peptide Summary Matrix Format (*.tab)";
    QString sFilterSel;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Groups"), dir, groupsTAB, &sFilterSel);

    if(fileName.isEmpty()) return;

    if ( sFilterSel == groupsTAB) {
        if(!fileName.endsWith(".tab",Qt::CaseInsensitive)) fileName = fileName + ".tab";
    }

     QFile file(fileName);
     if ( !file.open(QFile::WriteOnly) ) {
         QErrorMessage errDialog(this);
         errDialog.showMessage("File open " + fileName + " failed");
         return; //error
     }

     QTextStream out(&file);
     QTreeWidgetItemIterator it(treeWidget);
     int colmCount=treeWidget->columnCount();
     for(unsigned int i=0; i<colmCount;i++) {
         out << treeWidget->headerItem()->text(i) << "\t";
     }
     out << endl;

     while (*it) {
         QTreeWidgetItem* item = (*it);
         if(!item) continue;

         for(unsigned int i=0; i< colmCount;i++) {
             out <<  item->text(i) << "\t";
         }
         out << endl;
         ++it;
     }
     file.close();
}

void SpectralHitsDockWidget::showSpectralHits() {
    //Merged with Maven776 - Kiran
    treeWidget->clear();
    if (allhits.size() == 0 ) return;
    treeWidget->setSortingEnabled(false);

    QStringList colNames;
    colNames << "Sample" << "Protein" << "Peptide" << "rt" << "Precursor MZ" << "Charge" << "Scan Number" << "Score" << "matchCount"  << "Hit Rank";
    treeWidget->setColumnCount(colNames.size());
    treeWidget->setHeaderLabels(colNames);
    //treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //treeWidget->header()->adjustSize();
    treeWidget->setSortingEnabled(false);


    qSort(allhits.begin(),allhits.end(),SpectralHit::compScore);
    for(int i=0; i < allhits.size(); i++ ) {
	    if(allhits[i]) addRow(allhits[i], NULL);
    }


    QScrollBar* vScroll = treeWidget->verticalScrollBar();
    if ( vScroll ) { vScroll->setSliderPosition(vScroll->maximum()); }
    treeWidget->verticalScrollBar()->setEnabled(true);
    treeWidget->setSortingEnabled(true);
}

void SpectralHitsDockWidget::showSelectedGroup() { 
    QTreeWidgetItem *item = treeWidget->currentItem();
    if (!item) return;
    if(!_mainwindow) return;

    QVariant v = item->data(0,Qt::UserRole);
    SpectralHit*  hit =  v.value<SpectralHit*>();
    if(!hit) return;


    _mainwindow->setUrl("http://idms.pfizer.com/idms/target_search_view.php?ipi=" + hit->getProteins().first(), hit->getProteins().first());

        /*
        if (hit->rt > 0) {
            _mainwindow->getEicWidget()->setFocusLine(hit->rt);
        }
        _mainwindow->getEicWidget()->setMzSlice(hit->precursorMz);
        */

        //(if (!ok) {
        //  _mainwindow->getEicWidget()->setMzSlice(hit->precursorMz);
        //}

        _mainwindow->setPeptideSequence( hit->getModPeptideString() );

        float ppmWindow=hit->precursorMz/1e6*_mainwindow->getUserPPM();
        float mzmin = hit->precursorMz -ppmWindow;
        float mzmax = hit->precursorMz +ppmWindow;

        if(hit->rt > 0) {
             mzSlice slice(mzmin,mzmax,hit->rt-3,hit->rt+3);
             //if(peptideCompoundMap[hit->fragmentId]) slice.compound=peptideCompoundMap[hit->fragmentId];
            _mainwindow->getEicWidget()->setMzSlice(slice);
            _mainwindow->getEicWidget()->setFocusLine(hit->rt);
        } else {
            _mainwindow->getEicWidget()->setMzSlice(hit->precursorMz);
        }

        if(hit->scan)
            _mainwindow->getSpectraWidget()->setScan(hit->scan);

}

QList<SpectralHit*> SpectralHitsDockWidget::getSelectedHits() {
    QList<SpectralHit*> selectedHits;
    Q_FOREACH(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
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

void SpectralHitsDockWidget::loadSpectralHitsTable() {
    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();

    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();
    QString selFilter;
    QStringList filters;
    filters << "All Known Formats (*.pepXML *.pep.xml *.idpDB)" << "pepXML File (*.pepXML *.pep.xml)" << "IdPicker DB (*.idpDB)";

    QStringList filelist = QFileDialog::getOpenFileNames (this,
                                                    "Load Spectral Hits",
                                                    dir,
                                                    filters.join(";;"),
                                                    &selFilter);

    Q_FOREACH(QString fileName, filelist ) {
        if (fileName.isEmpty()) continue;

        if (fileName.endsWith("pepXML",Qt::CaseInsensitive)) {
            loadPepXML(fileName);
        }
        else if (fileName.endsWith("pep.xml",Qt::CaseInsensitive)) {
            loadPepXML(fileName);
        }
        else if (fileName.endsWith("idpDB",Qt::CaseInsensitive)) {
            loadIdPickerDB(fileName);
        }
    }

    showAllHits();
}

void SpectralHitsDockWidget::loadPepXML(QString fileName) {

    qDebug() << "loadPepXML(): " << fileName;
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
	float scanRetentionTime=0;

    SpectralHit* lasthit=NULL;
    map<int,float> mods;
    double PROTON = HMASS-EMASS;
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
                taglist << xml.name();
                if (xml.name() == "spectrum_query") {
                    scannum = xml.attributes().value("start_scan").toString().toInt();
                    charge = xml.attributes().value("assumed_charge").toString().toInt();
                    precursorMz = xml.attributes().value("precursor_neutral_mass").toString().toDouble();
                    scanRetentionTime = xml.attributes().value("retention_time_sec").toString().toDouble()/60;


                } else if (xml.name() == "search_hit") {
                    int hit_rank = xml.attributes().value("hit_rank").toString().toInt();
                    int num_matched_ions = xml.attributes().value("num_matched_ions").toString().toInt();
                    double massdiff = xml.attributes().value("massdiff").toString().toInt();
                    QString peptide = xml.attributes().value("peptide").toString();
                    QString protein = xml.attributes().value("protein").toString();
                    mods.clear();;

                    bool decoy = false;
                    if (protein.startsWith("r-") or protein.startsWith("DECOY")) decoy=true;

                    if(precursorMz and hit_rank < 2 and !decoy) {
                        SpectralHit* hit = new SpectralHit();
                        hit->scannum = scannum;
                        hit->precursorMz = precursorMz; if (charge) hit->precursorMz = (precursorMz+charge*PROTON)/charge;
                        hit->charge = charge;
                        hit->decoy = decoy;
                        hit->rank = hit_rank;
                        hit->matchCount = num_matched_ions;
                        hit->unmodPeptideSeq  = peptide;
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
                    if(lasthit) lasthit->mods[pos] += massshift;
                } else if (xml.name() == "search_score" ) {
                    QString name = xml.attributes().value("name").toString();
                    QString value = xml.attributes().value("value").toString();
                    if(lasthit and name == "mvh")   {  lasthit->mvh = value.toDouble();  lasthit->score=value.toDouble(); }
                    if(lasthit and name == "xcorr") {  lasthit->xcorr = value.toDouble(); }

                }
        } else if (xml.isEndElement()) {
            if (xml.name() == "search_hit") {

                if (hitCount++ % 1000 == 0) {
                    _mainwindow->setProgressBar("Parsing " + fileName + "hits:" + QString::number(hitCount), hitCount % 1000, 10000);
                     QApplication::processEvents();
                }
                if (lasthit) lasthit->fragmentId = lasthit->getModPeptideString();

            }
            if (!taglist.isEmpty()) taglist.pop_back();
        }
    }
    data.close();
}

/*
@author: Kiran - Sahil
*/
//TODO: Kiran-Sahil, Added while merging spectralhitstable
void SpectralHitsDockWidget::setupFiltersDialog() {
    if(filterDialog) { delete(filterDialog); filterDialog=NULL; }
    if(!treeWidget) return;

    filterDialog = new QDialog(this);

    int fieldCount=treeWidget->headerItem()->columnCount();
    int rowCount=treeWidget->topLevelItemCount();
    if (rowCount == 0) return;

    QGridLayout *layout = new QGridLayout(filterDialog);
    QTreeWidgetItem* firstItem = treeWidget->topLevelItem(0);

    for(int i=0; i < fieldCount; i++ ) {
        QString colmName = treeWidget->headerItem()->text(i);
        bool isNumeric=false; firstItem->text(i).toFloat(&isNumeric);

        if(isNumeric) {
            QDoubleSpinBox* min = new QDoubleSpinBox(filterDialog);
            min->setRange(-1e24,+1e24);
            min->setObjectName("min_" + QString::number(i));

            QDoubleSpinBox* max = new QDoubleSpinBox(filterDialog);
            max->setObjectName("max_" + QString::number(i));
            max->setRange(-1e24,+1e24);

            QLabel* l = new QLabel(colmName);

            layout->addWidget(l,i,0);
            layout->addWidget(min,i,1);
            layout->addWidget(max,i,2);
            connect(min,SIGNAL(valueChanged(QString)),this,SLOT(addFilter(QString)));
            connect(max,SIGNAL(valueChanged(QString)),this,SLOT(addFilter(QString)));
        } else {
            QLineEdit* x = new QLineEdit(filterDialog);
            x->setObjectName("filter_" + QString::number(i));
            QLabel* l = new QLabel(colmName);
            layout->addWidget(l,i,0);
            layout->addWidget(x,i,1,1,2);
            connect(x,SIGNAL(textChanged(QString)),this,SLOT(addFilter(QString)));
        }

    }

    QPushButton *ok = new QPushButton("Apply Filters",filterDialog);
    layout->addWidget(ok,fieldCount+2,0);
    connect(ok,SIGNAL(clicked()),SLOT(applyAllFilters()));

    QPushButton *clear = new QPushButton("Clear Filters",filterDialog);
    layout->addWidget(clear,fieldCount+2,1);
    connect(clear,SIGNAL(clicked()),SLOT(unhideFocusedHits()));


    filterDialog->setLayout(layout);
}

/*
@author: Kiran - Sahil
*/
//TODO: Kiran-Sahil, Added while merging spectralhitstable
void SpectralHitsDockWidget::showFiltersDialog() {
    setupFiltersDialog();
    if(!filterDialog) return;
    filterDialog->setVisible(! filterDialog->isVisible() );
    if (filterDialog->isVisible() == false) return;
}

/*
@author: Kiran - Sahil
*/
//TODO: Kiran-Sahil, Added while merging spectralhitstable
void SpectralHitsDockWidget::addFilter(QString value) {
    QObject* x = sender();
    if (x) {
       QString objName = x->objectName();
       setFilters[objName]=value;
    }
}

/*
@author: Kiran - Sahil
*/
//TODO: Kiran-Sahil, Added while merging spectralhitstable
void SpectralHitsDockWidget::applyAllFilters() {
    int N=treeWidget->topLevelItemCount();

    QTreeWidgetItemIterator it(treeWidget);
    while (*it) { (*it)->setHidden(false);  ++it; }; //unhide all rows

    //apply filters sequentially
    Q_FOREACH( QString filter_column, setFilters.keys()) {
        applyFilter(filter_column,setFilters[filter_column]);
    }

    updateTable();
}

/*
@author: Kiran - Sahil
*/
//TODO: Kiran-Sahil, Added while merging spectralhitstable
void SpectralHitsDockWidget::applyFilter(QString filter_column, QString value) {
    QStringList fields = filter_column.split("_");
    if(fields.size() == 2 ) {
        QString filterType = fields[0];
        int columnNum = fields[1].toInt();
        double fValue = value.toDouble();
        qDebug() << "applyFilter: type=" << filterType << " colm=" << columnNum << " value=" << value;

        int N=treeWidget->topLevelItemCount();
        for(int i=0; i < N; i++ ) {
            QTreeWidgetItem* item = treeWidget->topLevelItem(i);
            if(!item or item->isHidden()) continue;
            QString itemValue = item->text(columnNum);
          //  qDebug() << itemValue << " " << itemValue.toDouble();

            if( filterType == "max" and itemValue.toDouble() > fValue) {
                item->setHidden(true);
            }

            else if( filterType == "min" and itemValue.toDouble() < fValue) {
                item->setHidden(true);
            }

            else if( filterType == "filter" and not itemValue.contains(value,Qt::CaseInsensitive)) {
                item->setHidden(true);
            }
        }
    }
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

    setFilters.clear();
    for(int i=0; i< allhits.size();i++)   allhits[i]->isFocused=false;

    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        (*it)->setHidden(false);
        ++it;
    }
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
    Q_FOREACH(SpectralHit* hit, hitlist) {
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
        //   Q_EMIT(updateProgressBar("Loading Protein Sequences", rowCounter,rowCount));
      }
    }
}

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging mzfileio
void SpectralHitsDockWidget::getRetentionTimes() {
    qDebug() << "getRetentionTimes()";
    Q_FOREACH(SpectralHit* hit, allhits)
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


/*
@author: Kiran - Sahil
*/
//TODO: Kiran-Sahil, Added while merging spectralhitstable
void SpectralHitsDockWidget::integrateMS1() {
    Q_EMIT(updateProgressBar("Starting Integration",0,0));

    vector <mzSample*> samples = _mainwindow->getVisibleSamples();
    if (samples.size() == 0) return;

   float ppm = _mainwindow->getUserPPM();
   float rtWinMin = 3;

   QSettings *settings 		= _mainwindow->getSettings();
   int eic_smoothingWindow = settings->value("eic_smoothingWindow").toInt();
   int   eic_smoothingAlgorithm = settings->value("eic_smoothingAlgorithm").toInt();
   float amuQ1 = settings->value("amuQ1").toDouble();
   float amuQ3 = settings->value("amuQ3").toDouble();
   int baseline_smoothing = _mainwindow->mavenParameters->baseline_smoothingWindow;
   int baseline_quantile =  _mainwindow->mavenParameters->baseline_dropTopX;
   double minSignalBaselineDifference = _mainwindow->mavenParameters->minSignalBaselineDifference;
   float grouping_maxRtWindow =  settings->value("grouping_maxRtWindow").toDouble();
   int eic_type = _mainwindow->mavenParameters->eicType;
   string filterline = _mainwindow->mavenParameters->filterline;

   QMap<QString,mzSlice>  peptideMap;

   Q_FOREACH(SpectralHit* hit, allhits) {
       QString peptideId = hit->fragmentId;

       if(! peptideMap.count(peptideId)) {
           float amuTol = hit->precursorMz/1e6*ppm;
           mzSlice slice(hit->precursorMz-amuTol,hit->precursorMz+amuTol, hit->rt-rtWinMin, hit->rt+rtWinMin);
           slice.rt = hit->rt;
           slice.ionCount = 1;

           qDebug() << peptideId;
           if(peptideCompoundMap.count(peptideId)==0) {
               Compound* cpd = new Compound(peptideId.toStdString(),hit->getProteinIds().toStdString(),string(),hit->charge);
               cpd->expectedRt=hit->rt;
               cpd->precursorMz = hit->precursorMz;
               cpd->mass=hit->precursorMz;
               peptideCompoundMap[peptideId]=cpd;
           }
           peptideMap[peptideId] = slice;
       } else {
           mzSlice* slice = &peptideMap[peptideId];
           slice->rt += hit->rt;
           slice->ionCount++;
           if (slice->rtmin > hit->rt-0.5) slice->rtmin = hit->rt-0.5;
           if (slice->rtmax < hit->rt+0.5) slice->rtmax = hit->rt+0.5;
           if (slice->mz-hit->precursorMz > 0.5) {
               qDebug() << "Check peptide mz: " << slice->mz << " " << hit->precursorMz;
           }
       }
   }


   Q_FOREACH(QString peptide, peptideMap.keys()) {
        mzSlice* slice = &peptideMap[peptide];
        slice->rt = slice->rt/slice->ionCount;
        if(slice->rtmin < 0) slice->rtmin=0;
        if(slice->rtmax < 0) slice->rtmax=0;

       //qDebug() << "group: " << peptide << " rt=" << slice->rt;

       vector<EIC*> eics = PeakDetector::pullEICs(slice,
                                                    samples,
                                                    EicLoader::PeakDetection,
                                                    eic_smoothingWindow,
                                                    eic_smoothingAlgorithm,
                                                    amuQ1,
                                                    amuQ3,
                                                    baseline_smoothing,
                                                    baseline_quantile,
                                                    minSignalBaselineDifference,
                                                    eic_type,
                                                    filterline);

       //qDebug() << "here.. .here.. here " << eics.size();

       vector<PeakGroup> peakgroups = EIC::groupPeaks(eics,
                                                    eic_smoothingWindow,
                                                    grouping_maxRtWindow,
                                                    _mainwindow->mavenParameters->minQuality,
                                                    _mainwindow->mavenParameters->distXWeight,
                                                    _mainwindow->mavenParameters->distYWeight,
                                                    _mainwindow->mavenParameters->overlapWeight,
                                                    _mainwindow->mavenParameters->useOverlap,
                                                    _mainwindow->mavenParameters->minSignalBaselineDifference);


       PeakGroup* nearestGrp = NULL;
       for(int i=0; i < peakgroups.size();i++ ) {
           PeakGroup* grp = &peakgroups[i];
           float deltaRt = abs(grp->meanRt - slice->rt);
           if (! nearestGrp or deltaRt < abs(nearestGrp->meanRt - slice->rt) ) nearestGrp = grp;
       }

       //qDebug() << " \t nearestGrp=" << nearestGrp;

       if (nearestGrp) {
           //qDebug() << nearestGrp->meanRt << " " << nearestGrp->maxIntensity;
           nearestGrp->tagString = peptide.toStdString();

           if(peptideCompoundMap.count(peptide)) {
               Compound* cpd = peptideCompoundMap[peptide];
               nearestGrp->compound = cpd;
               nearestGrp->compound->expectedRt = slice->rt;

               if(proteinAccessionMap.contains(cpd->name.c_str())) {
                   ProteinHit* prot = proteinAccessionMap[cpd->name.c_str()];
                   if (prot) {
                        nearestGrp->tagString=prot->geneSymbol.toStdString();
                        nearestGrp->metaGroupId=prot->proteinGroup;
                   }
               }
           }
           //qDebug() << "\t bookmarking..";
           _mainwindow->getBookmarkedPeaks()->addPeakGroup(nearestGrp);
       }
       delete_all(eics);

   }

   _mainwindow->getBookmarkedPeaks()->align();
   _mainwindow->getBookmarkedPeaks()->setTableView(TableDockWidget::peakView);
   _mainwindow->getBookmarkedPeaks()->showAllGroups();
   _mainwindow->getBookmarkedPeaks()->show();
   _mainwindow->getBookmarkedPeaks()->raise();

   qDebug() << "integrateMS1() done.";
}


QStringList SpectralHitsDockWidget::uniqGeneList(SpectralHit* hit) {
    QStringList proteinList = hit->getProteins();
    QSet<QString> geneSymbols;

    Q_FOREACH (QString protein, proteinList ) {
        ProteinHit* prot = proteinAccessionMap[protein];
        if (prot) geneSymbols << prot->geneSymbol;
    }
    QStringList uniqGeneList = geneSymbols.toList();
    return uniqGeneList;
}