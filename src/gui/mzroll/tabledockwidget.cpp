#include "tabledockwidget.h";
#include "peaktabledeletiondialog.h"

TableDockWidget::TableDockWidget(MainWindow *mw) {
  setAllowedAreas(Qt::AllDockWidgetAreas);
  setFloating(false);
  _mainwindow = mw;
  pal = palette();
  setAutoFillBackground(true);
  pal.setColor(QPalette::Background, QColor(170, 170, 170, 100));
  setPalette(pal);

  viewType = groupView;
  maxPeaks = 0; //Maximum Number of Peaks in a Group

  treeWidget = new QTreeWidget(this);
  treeWidget->setSortingEnabled(false);
  treeWidget->setDragDropMode(QAbstractItemView::DragOnly);
  treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
  treeWidget->setAcceptDrops(false);
  treeWidget->setObjectName("PeakGroupTable");
  treeWidget->setFocusPolicy(Qt::NoFocus);
  treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->setFocusPolicy(Qt::ClickFocus);
  tableSelectionFlagUp = false;
  tableSelectionFlagDown = false;
  this->setAcceptDrops(true);

  setWidget(treeWidget);
  setupPeakTable();

  traindialog = new TrainDialog(this);
  connect(traindialog->saveButton, SIGNAL(clicked(bool)), SLOT(saveModel()));
  connect(traindialog->trainButton, SIGNAL(clicked(bool)), SLOT(Train()));
  connect(treeWidget,
          SIGNAL(itemClicked(QTreeWidgetItem *, int)),
          SLOT(showSelectedGroup()));
  connect(treeWidget,
          SIGNAL(itemSelectionChanged()),
          SLOT(showSelectedGroup()));
  connect(treeWidget,
          SIGNAL(itemExpanded(QTreeWidgetItem *)), this,
          SLOT(sortChildrenAscending(QTreeWidgetItem *)));

  clusterDialog = new ClusterDialog(this);
  connect(clusterDialog->clusterButton,
          SIGNAL(clicked(bool)),
          SLOT(clusterGroups()));
  connect(clusterDialog->clearButton,
          SIGNAL(clicked(bool)),
          SLOT(clearClusters()));

  connect(this,
          SIGNAL(updateProgressBar(QString, int, int)),
          _mainwindow,
          SLOT(setProgressBar(QString, int, int)));
  connect(this, SIGNAL(tenPeaksMarked()), this, SLOT(ShowStatistics()));

  setupFiltersDialog();

  setAcceptDrops(true);
}

TableDockWidget::~TableDockWidget() {
  if (traindialog != NULL)
    delete traindialog;
  if (clusterDialog != NULL)
    delete clusterDialog;

  delete treeWidget;
}

void TableDockWidget::sortChildrenAscending(QTreeWidgetItem *item) {
  item->sortChildren(1, Qt::AscendingOrder);
}

void TableDockWidget::showTrainDialog() { traindialog->show(); }

void TableDockWidget::showClusterDialog() { clusterDialog->show(); }

void TableDockWidget::sortBy(int col) {
  treeWidget->sortByColumn(col, Qt::AscendingOrder);
}

void TableDockWidget::setIntensityColName() {
  QTreeWidgetItem *header = treeWidget->headerItem();
  QString temp;
  PeakGroup::QType qtype = _mainwindow->getUserQuantType();
  switch (qtype) {
  case PeakGroup::AreaTop:
    temp = "Max AreaTop";
    break;
  case PeakGroup::Area:
    temp = "Max Area";
    break;
  case PeakGroup::Height:
    temp = "Max Height";
    break;
  case PeakGroup::AreaNotCorrected:
    temp = "Max AreaNotCorrected";
    break;
  case PeakGroup::AreaTopNotCorrected:
    temp = "Max AreaTopNotCorrected";
    break;
  default:
    temp = _mainwindow->currentIntensityName;
    break;
  }
  _mainwindow->currentIntensityName = temp;
  header->setText(9, temp);
}

void TableDockWidget::setupPeakTable() {

  QStringList colNames;

  // Add common coulmns to the Table
  colNames << "#";
  colNames << "ID";
  colNames << "Observed m/z";
  colNames << "Expected m/z";
  colNames << "rt";

  if (viewType == groupView) {

    // Add group view columns to the peak table
    colNames << "rt delta";
    colNames << "#peaks";
    colNames << "#good";
    colNames << "Max Width";
    colNames << "Max AreaTop";
    colNames << "Max S/N";
    colNames << "Max Quality";
    colNames << "Rank";
  } else if (viewType == peakView) {
    vector<mzSample *> vsamples = _mainwindow->getVisibleSamples();
    sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
    for (unsigned int i = 0; i < vsamples.size(); i++) {
      // Add peak view columns to the table
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

void TableDockWidget::updateItem(QTreeWidgetItem *item) {
  QVariant v = item->data(0, Qt::UserRole);
  PeakGroup *group = v.value<PeakGroup *>();
  if (group == NULL)
    return;
  heatmapBackground(item);

  // score peak quality
  Classifier *clsf = _mainwindow->getClassifier();
  if (clsf != NULL) {
    clsf->classify(group);
    group->updateQuality();

    if (viewType == groupView)
      item->setText(11, QString::number(group->maxQuality, 'f', 2));
    item->setText(1, QString(group->getName().c_str()));
  }
  //Find maximum number of peaks
  if (maxPeaks < group->peakCount()) maxPeaks = group->peakCount();

  //score group quality
  groupClassifier* groupClsf = _mainwindow->getGroupClassifier();
  if (groupClsf != NULL) {
      groupClsf->classify(group);
  }

  //get probability good/bad from svm
  svmPredictor* groupPred = _mainwindow->getSVMPredictor();
  if (groupPred != NULL) {
      groupPred->predict(group);
  }
  
  // Updating the peakid
  item->setText(0, QString::number(group->groupId));

  if (viewType == groupView && fabs(group->changeFoldRatio) >= 0) {
    item->setText(13, QString::number(group->changeFoldRatio, 'f', 3));
    item->setText(14, QString::number(group->changePValue, 'f', 6));
  }

  int good = 0;
  int bad = 0;
  int total = group->peakCount();
  for (int i = 0; i < group->peakCount(); i++) {
    group->peaks[i].quality > _mainwindow->mavenParameters->minQuality ? good++
                                                                       : bad++;
  }

  QBrush brush = Qt::NoBrush;
  if (good > 0 && group->label == 'b') {
    float incorrectFraction = ((float)good) / total;
    brush = QBrush(QColor::fromRgbF(0.8, 0, 0, incorrectFraction));
  } else if (bad > 0 && group->label == 'g') {
    float incorrectFraction = ((float)bad) / total;
    brush = QBrush(QColor::fromRgbF(0.8, 0, 0, incorrectFraction));
  }
  item->setBackground(0, brush);

  if (group->label == 'g')
    item->setIcon(0, QIcon(":/images/good.png"));
  if (group->label == 'b')
    item->setIcon(0, QIcon(":/images/bad.png"));

  if (filtersDialog->isVisible()) {
    float minG = sliders["GoodPeakCount"]->minBoundValue();
    float maxG = sliders["GoodPeakCount"]->maxBoundValue();

    if (group->goodPeakCount < minG || group->goodPeakCount > maxG) {
      item->setHidden(true);
    } else {
      item->setHidden(false);
    }
  }
}

void TableDockWidget::updateCompoundWidget() {
  _mainwindow->ligandWidget->resetColor();
  QTreeWidgetItemIterator itr(treeWidget);
  while (*itr) {
    QTreeWidgetItem *item = (*itr);
    if (item) {
      QVariant v = item->data(0, Qt::UserRole);
      PeakGroup *group = v.value<PeakGroup *>();
      if (group == nullptr)
        continue;
      _mainwindow->ligandWidget->markAsDone(group->compound);
    }
    ++itr;
  }
}

void TableDockWidget::heatmapBackground(QTreeWidgetItem *item) {
  if (viewType != peakView)
    return;

  int firstColumn = 4;
  StatisticsVector<float> values;
  float sum = 0;
  for (unsigned int i = firstColumn; i < item->columnCount(); i++) {
    values.push_back(item->text(i).toFloat());
  }

  if (values.size()) {
    // normalize
    float mean = values.mean();
    float sd = values.stddev();

    float max = values.maximum();
    float min = values.minimum();
    float range = max - min;

    for (int i = 0; i < values.size(); i++) {
      if (max != 0)
        values[i] = abs((max - values[i]) / max); // Z-score
    }

    QColor color = Qt::white;

    float colorramp = 0.5;

    for (int i = 0; i < values.size(); i++) {
      float value = values[i];
      float prob = value;
      if (prob < 0)
        prob = 0;
      color.setHsvF(0.0, prob, 1, 1);

      item->setBackgroundColor(firstColumn + i, color);
    }
  }
}

void TableDockWidget::addRow(PeakGroup *group, QTreeWidgetItem *root) {

  if (group == NULL)
    return;
  if (group->peakCount() == 0)
    return;
  if (group->meanMz <= 0) {
    return;
  }

  NumericTreeWidgetItem *item = new NumericTreeWidgetItem(root, 0);
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled |
                 Qt::ItemIsDragEnabled);
  item->setData(0, Qt::UserRole, QVariant::fromValue(group));

  item->setText(0, QString::number(group->groupId));
  item->setText(1, QString(group->getName().c_str()));
  item->setText(2, QString::number(group->meanMz, 'f', 4));
  int charge = _mainwindow->mavenParameters->getCharge(group->compound);

  if (group->getExpectedMz(charge) != -1) {
    float mz = group->getExpectedMz(charge);

    item->setText(3, QString::number(mz, 'f', 4));
  } else {
    item->setText(3, "NA");
  }

  item->setText(4, QString::number(group->meanRt, 'f', 2));

  if (group->label == 'g')
    item->setIcon(0, QIcon(":/images/good.png"));
  if (group->label == 'b')
    item->setIcon(0, QIcon(":/images/bad.png"));

  if (viewType == groupView) {

    item->setText(5, QString::number(group->expectedRtDiff, 'f', 2));
    item->setText(6, QString::number(group->sampleCount));
    item->setText(7, QString::number(group->goodPeakCount));
    item->setText(8, QString::number(group->maxNoNoiseObs));
    item->setText(9, QString::number(extractMaxIntensity(group), 'g', 2));
    item->setText(10, QString::number(group->maxSignalBaselineRatio, 'f', 0));
    item->setText(11, QString::number(group->maxQuality, 'f', 2));
    item->setText(12, QString::number(group->groupRank, 'e', 6));

    if (group->changeFoldRatio != 0) {

      item->setText(13, QString::number(group->changeFoldRatio, 'f', 2));
      item->setText(14, QString::number(group->changePValue, 'e', 4));
    }
    item->setText(15,QString::number(group->avgPeakQuality,'f',2));
    //Find maximum number of peaks
    if (maxPeaks < group->peakCount()) maxPeaks = group->peakCount();

    //Get group quality from neural network
    groupClassifier* groupClsf = _mainwindow->getGroupClassifier();
    if (groupClsf != NULL) {
        groupClsf->classify(group);
    }
    //Add group quality to peak table
    item->setText(16,QString::number(group->groupQuality,'f',2));
    item->setText(17,QString::number(group->weightedAvgPeakQuality,'f',2));

    //Get prediction labels from svm
    svmPredictor* groupPred = _mainwindow->getSVMPredictor();
    if (groupPred != NULL) {
        groupPred->predict(group);
    }
    item->setText(18,QString::number(group->predictedLabel,'f', 0));
    
  } else if (viewType == peakView) {
    vector<mzSample *> vsamples = _mainwindow->getVisibleSamples();
    sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
    vector<float> yvalues = group->getOrderedIntensityVector(
        vsamples, _mainwindow->getUserQuantType());
    for (unsigned int i = 0; i < yvalues.size(); i++) {

      item->setText(5 + i, QString::number(yvalues[i]));
    }
    heatmapBackground(item);
  }
  if (root == NULL)
    treeWidget->addTopLevelItem(item);
  updateItem(item);

  if (group->childCount() > 0) {
    for (int i = 0; i < group->childCount(); i++)
      addRow(&(group->children[i]), item);
  }
}

void ListView::keyPressEvent(QKeyEvent *event) {
  if (event->matches(QKeySequence::Copy)) {
    // set all selected compound name to clipboard
    QApplication::clipboard()->setText(strings.join("\n"));
  }
}

PeakGroup *TableDockWidget::addPeakGroup(PeakGroup *group) {
  if (group != NULL) {
    allgroups.push_back(*group);
	if (group->childCount() > 0)
		labeledGroups++;
    if (allgroups.size() > 0) {
      PeakGroup &g = allgroups[allgroups.size() - 1];
      for (unsigned int i = 0; i < allgroups.size(); i++) {
        allgroups[i].groupId = i + 1;
      }
      return &g;
    }
  }

  return NULL;
}

QList<PeakGroup *> TableDockWidget::getGroups() {
  QList<PeakGroup *> groups;
  for (int i = 0; i < allgroups.size(); i++) {
    groups.push_back(&allgroups[i]);
  }
  return groups;
}

void TableDockWidget::deleteAll() {
  treeWidget->clear();
  allgroups.clear();

  _mainwindow->removePeaksTable(this);
  _mainwindow->getEicWidget()->replotForced();

  this->hide();

  if (_mainwindow->heatmap) {
    HeatMap *_heatmap = _mainwindow->heatmap;
    _heatmap->setTable(this);
    _heatmap->replot();
  }
}

void TableDockWidget::showAllGroups() {
  treeWidget->clear();

  setFocus();
  if (allgroups.size() == 0) {
    if (viewType == groupView)
      setIntensityColName();
    setVisible(false);
    return;
  }

  treeWidget->setSortingEnabled(false);

  setupPeakTable();
  if (viewType == groupView)
    setIntensityColName();

  QMap<int, QTreeWidgetItem *> parents;
  for (int i = 0; i < allgroups.size(); i++) {
    int clusterId = allgroups[i].clusterId;
    if (clusterId && allgroups[i].meanMz > 0 && allgroups[i].peakCount() > 0) {
      if (!parents.contains(clusterId)) {
        parents[clusterId] = new QTreeWidgetItem(treeWidget);
        parents[clusterId]->setText(0, QString("Cluster ") +
                                           QString::number(clusterId));
        parents[clusterId]->setText(
            5, QString::number(allgroups[i].meanRt, 'f', 2));
        parents[clusterId]->setExpanded(true);
      }
      QTreeWidgetItem *parent = parents[clusterId];
      addRow(&allgroups[i], parent);
    } else {
      addRow(&allgroups[i], NULL);
    }
  }

  QScrollBar *vScroll = treeWidget->verticalScrollBar();
  if (vScroll) {
    vScroll->setSliderPosition(vScroll->maximum());
  }
  treeWidget->setSortingEnabled(true);
  updateStatus();
  updateCompoundWidget();
  //@Kailash: Check and validate all groups automatically
  QTreeWidgetItemIterator itr(treeWidget);
  while(*itr) {
      QTreeWidgetItem* item = (*itr);
      QVariant v = item->data(0,Qt::UserRole);
      PeakGroup* grp = v.value<PeakGroup*>();
      validateGroup(grp, item);
      itr++;
  }

}

float TableDockWidget::extractMaxIntensity(PeakGroup *group) {
  float temp;
  PeakGroup::QType qtype = _mainwindow->getUserQuantType();
  switch (qtype) {
  case PeakGroup::AreaTop:
    temp = group->maxAreaTopIntensity;
    break;
  case PeakGroup::Area:
    temp = group->maxAreaIntensity;
    break;
  case PeakGroup::Height:
    temp = group->maxHeightIntensity;
    break;
  case PeakGroup::AreaNotCorrected:
    temp = group->maxAreaNotCorrectedIntensity;
    break;
  case PeakGroup::AreaTopNotCorrected:
    temp = group->maxAreaTopNotCorrectedIntensity;
    break;
  default:
    temp = group->currentIntensity;
    break;
  }
  group->currentIntensity = temp;
  return temp;
}

void TableDockWidget::exportGroupsToSpreadsheet() {

  vector<mzSample *> samples = _mainwindow->getSamples();
  CSVReports *csvreports = new CSVReports(samples);
  csvreports->setMavenParameters(_mainwindow->mavenParameters);
  if (allgroups.size() == 0) {
    QString msg = "Peaks Table is Empty";
    QMessageBox::warning(this, tr("Error"), msg);
    return;
  }

  QString dir = ".";
  QSettings *settings = _mainwindow->getSettings();

  if (settings->contains("lastDir"))
    dir = settings->value("lastDir").value<QString>();

  QString groupsSTAB = "Groups Summary Matrix Format With Set Names (*.tab)";
  QString groupsTAB = "Groups Summary Matrix Format (*.tab)";
  QString peaksTAB = "Peaks Detailed Format (*.tab)";
  QString groupsSCSV =
      "Groups Summary Matrix Format Comma Delimited With Set Names (*.csv)";
  QString groupsCSV = "Groups Summary Matrix Format Comma Delimited (*.csv)";
  QString peaksCSV = "Peaks Detailed Format Comma Delimited (*.csv)";

  QString peaksListQE = "Inclusion List QE (*.csv)";
  QString mascotMGF = "Mascot Format MS2 Scans (*.mgf)";

  QString sFilterSel;
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Export Groups"), dir,
      groupsCSV + ";;" + groupsSCSV + ";;" + groupsTAB + ";;" + groupsSTAB +
          ";;" + peaksCSV + ";;" + peaksTAB + ";;" + peaksListQE + ";;" +
          mascotMGF,
      &sFilterSel);

  if (fileName.isEmpty())
    return;

  if (sFilterSel == groupsSCSV || sFilterSel == peaksCSV ||
      sFilterSel == groupsCSV) {
    if (!fileName.endsWith(".csv", Qt::CaseInsensitive))
      fileName = fileName + ".csv";
  }

  if (sFilterSel == groupsSTAB || sFilterSel == peaksTAB ||
      sFilterSel == groupsTAB) {
    if (!fileName.endsWith(".tab", Qt::CaseInsensitive))
      fileName = fileName + ".tab";
  }

  if (samples.size() == 0)
    return;

  if (sFilterSel == groupsSCSV || sFilterSel == groupsSTAB ||
      sFilterSel == groupsCSV || sFilterSel == groupsTAB)
    _mainwindow->getAnalytics()->hitEvent("Exports", "CSV", 0);
  if (sFilterSel == peaksCSV || sFilterSel == peaksTAB)
    _mainwindow->getAnalytics()->hitEvent("Exports", "CSV", 1);

  if (sFilterSel == peaksListQE) {
    _mainwindow->getAnalytics()->hitEvent("Exports", "CSV", 2);
    writeQEInclusionList(fileName);
    return;
  } else if (sFilterSel == mascotMGF) {
    _mainwindow->getAnalytics()->hitEvent("Exports", "CSV", 3);
    writeMascotGeneric(fileName);
    return;
  }

  csvreports->setUserQuantType(_mainwindow->getUserQuantType());

  bool includeSetNamesLines = true;

  if (sFilterSel == groupsSCSV) {
    csvreports->openGroupReport(fileName.toStdString(), includeSetNamesLines);
  } else if (sFilterSel == groupsSTAB) {
    csvreports->openGroupReport(fileName.toStdString(), includeSetNamesLines);
  } else if (sFilterSel == peaksCSV) {
    csvreports->openPeakReport(fileName.toStdString());
  } else if (sFilterSel == peaksTAB) {
    csvreports->openPeakReport(fileName.toStdString());
  } else {
    // default to group summary
    csvreports->openGroupReport(fileName.toStdString());
  }

  QList<PeakGroup *> selectedGroups = getSelectedGroups();
  csvreports->setSelectionFlag(static_cast<int>(peakTableSelection));

  for (int i = 0; i < allgroups.size(); i++) {
    if (selectedGroups.contains(&allgroups[i])) {
      PeakGroup &group = allgroups[i];
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

void TableDockWidget::prepareDataForPolly(QString writableTempDir,
                                          QString exportFormat,
                                          QString userFilename) {

  vector<mzSample *> samples = _mainwindow->getSamples();
  CSVReports *csvreports = new CSVReports(samples);
  csvreports->setMavenParameters(_mainwindow->mavenParameters);
  if (allgroups.size() == 0) {
    QString msg = "Peaks Table is Empty";
    QMessageBox::warning(this, tr("Error"), msg);
    return;
  }

  QString groupsSTAB = "Groups Summary Matrix Format With Set Names (*.tab)";
  QString groupsTAB = "Groups Summary Matrix Format (*.tab)";
  QString peaksTAB = "Peaks Detailed Format (*.tab)";
  QString groupsSCSV =
      "Groups Summary Matrix Format Comma Delimited With Set Names (*.csv)";
  QString groupsCSV = "Groups Summary Matrix Format Comma Delimited (*.csv)";
  QString peaksCSV = "Peaks Detailed Format Comma Delimited (*.csv)";

  QString peaksListQE = "Inclusion List QE (*.csv)";
  QString mascotMGF = "Mascot Format MS2 Scans (*.mgf)";

  QString sFilterSel = exportFormat;
  QString fileName = writableTempDir + QDir::separator() + userFilename;
  if (fileName.isEmpty())
    return;

  if (sFilterSel == groupsSCSV || sFilterSel == peaksCSV ||
      sFilterSel == groupsCSV) {
    if (!fileName.endsWith(".csv", Qt::CaseInsensitive))
      fileName = fileName + ".csv";
  }

  if (sFilterSel == groupsSTAB || sFilterSel == peaksTAB ||
      sFilterSel == groupsTAB) {
    if (!fileName.endsWith(".tab", Qt::CaseInsensitive))
      fileName = fileName + ".tab";
  }

  if (samples.size() == 0)
    return;

  if (sFilterSel == peaksListQE) {
    writeQEInclusionList(fileName);
    return;
  } else if (sFilterSel == mascotMGF) {
    writeMascotGeneric(fileName);
    return;
  }

  csvreports->setUserQuantType(_mainwindow->getUserQuantType());

  bool includeSetNamesLines = true;

  if (sFilterSel == groupsSCSV) {
    csvreports->openGroupReport(fileName.toStdString(), includeSetNamesLines);
  } else if (sFilterSel == groupsSTAB) {
    csvreports->openGroupReport(fileName.toStdString(), includeSetNamesLines);
  } else if (sFilterSel == peaksCSV) {
    csvreports->openPeakReport(fileName.toStdString());
  } else if (sFilterSel == peaksTAB) {
    csvreports->openPeakReport(fileName.toStdString());
  } else {
    // default to group summary
    csvreports->openGroupReport(fileName.toStdString());
  }

  QList<PeakGroup *> selectedGroups = getSelectedGroups();
  csvreports->setSelectionFlag(static_cast<int>(peakTableSelection));

  for (int i = 0; i < allgroups.size(); i++) {
    if (selectedGroups.contains(&allgroups[i])) {
      PeakGroup &group = allgroups[i];
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

void TableDockWidget::exportJsonToPolly(QString writableTempDir,
                                        QString jsonfileName) {

  if (allgroups.size() == 0) {
    QString msg = "Peaks Table is Empty";
    QMessageBox::warning(this, tr("Error"), msg);
    return;
  }

  /**
   * copy all groups from <allgroups> to <vallgroups> which is used by
   * < libmaven/jsonReports.cpp>
   */
  vallgroups.clear();
  for (int i = 0; i < allgroups.size(); ++i) {
    vallgroups.push_back(allgroups[i]);
  }
  jsonReports = new JSONReports(_mainwindow->mavenParameters);
  jsonReports->saveMzEICJson(jsonfileName.toStdString(),
                             vallgroups,
                             _mainwindow->getVisibleSamples());
}

void TableDockWidget::ShowStatistics() {
  int accuracy = 0;
  int tp = 0;
  int tn = 0;
  int totalMarked = 0;
  int totalMarkedByMl = 0;
  for (int i = 0; i < allgroups.size(); i++) {
    if (allgroups[i].label =='g' || allgroups[i].label =='b'){
      totalMarked+=1;
    }
    if(allgroups[i].markedBadByCloudModel == 1 || allgroups[i].markedGoodByCloudModel == 1){
      totalMarkedByMl+=1;
    }
    if (allgroups[i].label =='g' & allgroups[i].markedGoodByCloudModel == 1){
      tp+=1;
    }
    if (allgroups[i].label =='b' & allgroups[i].markedBadByCloudModel == 1){
      tn+=1;
    }
  }
  if (totalMarked!=0){
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setIconPixmap(QPixmap(rsrcPath + "/success.png"));
    msgBox->setWindowTitle("Cloud model statistics");
    QString Final_message = " The cloud model detected " + QString::number(((float)(tp + tn) / totalMarked)) + "% of the peaks accurately.";    
    msgBox->setText(Final_message);
    msgBox->open();
  }
}
void TableDockWidget::exportJson() {

  if (allgroups.size() == 0) {
    QString msg = "Peaks Table is Empty";
    QMessageBox::warning(this, tr("Error"), msg);
    return;
  }

  _mainwindow->getAnalytics()->hitEvent("Exports", "JSON");

  /**
   * copy all groups from <allgroups> to <vallgroups> which is used by
   * < libmaven/jsonReports.cpp>
   */
  vallgroups.clear();
  for (int i = 0; i < allgroups.size(); ++i) {
    vallgroups.push_back(allgroups[i]);
  }

  QString dir = ".";
  QSettings *settings = _mainwindow->getSettings();
  if (settings->contains("lastDir"))
    dir = settings->value("lastDir").value<QString>();

  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save EICs to Json File"), dir, tr("*.json"));
  if (fileName.isEmpty())
    return;
  if (!fileName.endsWith(".json", Qt::CaseInsensitive))
    fileName = fileName + ".json";

  saveJson *jsonSaveThread = new saveJson();
  jsonSaveThread->setMainwindow(_mainwindow);
  jsonSaveThread->setPeakTable(this);
  jsonSaveThread->setfileName(fileName.toStdString());
  jsonSaveThread->start();
}

vector<EIC *> TableDockWidget::getEICs(float rtmin,
                                       float rtmax,
                                       PeakGroup &grp) {
  vector<EIC *> eics;
  for (int i = 0; i < grp.peaks.size(); i++) {
    float mzmin = grp.meanMz - 0.2;
    float mzmax = grp.meanMz + 0.2;
    vector<mzSample *> samples = _mainwindow->getSamples();
    for (unsigned int j = 0; j < samples.size(); j++) {
      if (!grp.srmId.empty()) {
        EIC *eic = samples[j]->getEIC(grp.srmId,
                                      _mainwindow->mavenParameters->eicType);
        eics.push_back(eic);
      } else {
        EIC *eic = samples[j]->getEIC(mzmin, mzmax,
                                      rtmin, rtmax, 1,
                                      _mainwindow->mavenParameters->eicType,
                                      _mainwindow->mavenParameters->filterline);
        eics.push_back(eic);
      }
    }
  }
  return (eics);
}

void TableDockWidget::showSelectedGroup() {

  QTreeWidgetItem *item = treeWidget->currentItem();
  if (!item)
    return;

  QVariant v = item->data(0, Qt::UserRole);
  PeakGroup *group = v.value<PeakGroup *>();
  _mainwindow->groupRtWidget->plotGraph(group);

  if (group != NULL && _mainwindow != NULL) {
    _mainwindow->setPeakGroup(group);
  }

  if (item->childCount() > 0) {
    vector<PeakGroup *> children;
    for (int i = 0; i < item->childCount(); i++) {
      QTreeWidgetItem *child = item->child(i);
      QVariant data = child->data(0, Qt::UserRole);
      PeakGroup *group = data.value<PeakGroup *>();
      if (group)
        children.push_back(group);
    }
  }
}

QList<PeakGroup *> TableDockWidget::getSelectedGroups() {
  QList<PeakGroup *> selectedGroups;
  Q_FOREACH (QTreeWidgetItem *item, treeWidget->selectedItems()) {
    if (item) {
      QVariant v = item->data(0, Qt::UserRole);
      PeakGroup *group = v.value<PeakGroup *>();
      if (group != NULL) {
        selectedGroups.append(group);
      }
    }
  }
  return selectedGroups;
}

void TableDockWidget::showNotification()
{
  _mainwindow->showNotification(this);
}

QList<PeakGroup *>
TableDockWidget::getCustomGroups(peakTableSelectionType peakSelection) {
  QList<PeakGroup *> selectedGroups;
  peakTableSelectionType temppeakSelection = peakSelection;
  Q_FOREACH (QTreeWidgetItem *item, treeWidget->selectedItems()) {
    if (item) {
      QVariant v = item->data(0, Qt::UserRole);
      PeakGroup *group = v.value<PeakGroup *>();
      if (group != NULL) {
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

PeakGroup *TableDockWidget::getSelectedGroup() {
  QTreeWidgetItem *item = treeWidget->currentItem();
  if (!item)
    return NULL;
  QVariant v = item->data(0, Qt::UserRole);
  PeakGroup *group = v.value<PeakGroup *>();
  if (group != NULL) {
    return group;
  } else
    return NULL;
}

void TableDockWidget::setGroupLabel(char label) {
  Q_FOREACH (QTreeWidgetItem *item, treeWidget->selectedItems()) {
    if (item) {
      QVariant v = item->data(0, Qt::UserRole);
      PeakGroup *group = v.value<PeakGroup *>();
      if (group != NULL) {
        if (!(group->label=='g'||group->label=='b')){
          numberOfGroupsMarked+=1;  
        }
        if (_mainwindow->sessionCount==1){
          if (numberOfGroupsMarked ==10){
            _mainwindow->sessionCount+=1;
            Q_EMIT(tenPeaksMarked());
          }
        }
        else{
          qDebug()<<"session coiunt exceeds one.";
        }

        group->setLabel(label);
      }
      updateItem(item);
    }
  }
  updateStatus();
}

void TableDockWidget::deleteGroup(PeakGroup *groupX) {
  if (!groupX)
    return;

  int pos = -1;
  for (int i = 0; i < allgroups.size(); i++) {
    if (&allgroups[i] == groupX) {
      pos = i;
      break;
    }
  }
  if (pos == -1)
    return;

  QTreeWidgetItemIterator it(treeWidget);
  while (*it) {
    QTreeWidgetItem *item = (*it);
    if (item->isHidden()) {
      ++it;
      continue;
    }
    QVariant v = item->data(0, Qt::UserRole);
    PeakGroup *group = v.value<PeakGroup *>();
    if (group != NULL and group == groupX) {
      item->setHidden(true);

      if (group->children.size() > 0)
            labeledGroups--;

      // Deleting
      int posTree = treeWidget->indexOfTopLevelItem(item);
      if (posTree != -1)
        treeWidget->takeTopLevelItem(posTree);

      allgroups.erase(allgroups.begin() + pos);
      break;
    }
    ++it;
  }

  for (unsigned int i = 0; i < allgroups.size(); i++) {
    allgroups[i].groupId = i + 1;
  }
  updateTable();
  updateCompoundWidget();
}

void TableDockWidget::deleteGroups() {

  QList<PeakGroup *> selectedGroups;
  QTreeWidgetItem *nextItem;

  if (treeWidget->selectedItems().size() == 0) {
    return;
  }

  Q_FOREACH (QTreeWidgetItem *item, treeWidget->selectedItems()) {
    if (item) {
      nextItem = treeWidget->itemBelow(item);
      QVariant v = item->data(0, Qt::UserRole);
      PeakGroup *group = v.value<PeakGroup *>();
      if (group != NULL) {
        PeakGroup *parentGroup = group->parent;
        int childrenNum = -1;
        if (parentGroup == NULL) {
            // top level item
            if (nextItem) {
                QVariant vc = nextItem->data(0, Qt::UserRole);
                PeakGroup *groupc = vc.value<PeakGroup *>();
            	if (groupc->isIsotope())
            		nextItem = nextItem->parent();
            }
            deleteGroup(group);
        } else if (parentGroup && parentGroup->childCount()) {
            // this a child item
            childrenNum = parentGroup->childCount();
            if (parentGroup->deleteChild(group)) {
                QTreeWidgetItem *parentItem = item->parent();
                if (parentItem) {
                	parentItem->removeChild(item);
              		delete (item);
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
  if (nextItem)
    treeWidget->setCurrentItem(nextItem, 0);
  _mainwindow->getEicWidget()->replotForced();
  showSelectedGroup();
  _mainwindow->getEicWidget()->addPeakPositions();

  return;
}

void TableDockWidget::setClipboard() {
  _mainwindow->getAnalytics()->hitEvent("Exports", "Clipboard", 3);
  QList<PeakGroup *> groups = getSelectedGroups();
  if (groups.size() > 0) {
    _mainwindow->isotopeWidget->setClipboard(groups);
  }
}

void TableDockWidget::showConsensusSpectra() {
  QList<PeakGroup *> groups = getSelectedGroups();
  if (groups.size() > 0) {
    _mainwindow->spectraWidget->showConsensusSpectra(groups[groups.size() - 1]);
  }
}

void TableDockWidget::markGroupGood() {
  qDebug()<<"inside markGroupGood()";
  setGroupLabel('g');
  showNextGroup();
  _mainwindow->peaksMarked++;
  _mainwindow->autoSaveSignal();
}

void TableDockWidget::markGroupBad() {
  qDebug()<<"inside markGroupBad()";
  setGroupLabel('b');
  showNextGroup();
  _mainwindow->peaksMarked++;
  _mainwindow->autoSaveSignal();
}

bool TableDockWidget::checkLabeledGroups() {

  int totalCount = 0;
  int goodCount = 0;
  int badCount = 0;

  if (_mainwindow->peaksMarked >= allgroups.size()) {
    for (int i = 0; i < allgroups.size(); i++) {
      char groupLabel = allgroups[i].label;
      if (groupLabel == 'g') {
        goodCount++;
      } else if (groupLabel == 'b') {
        badCount++;
      }
      totalCount++;
    }

    if (totalCount == goodCount + badCount)
      return true;
  }

  return false;
}

void TableDockWidget::markGroupIgnored() {
  setGroupLabel('i');
  showNextGroup();
}

void TableDockWidget::showLastGroup() {
  QTreeWidgetItem *item = treeWidget->currentItem();
  if (item != NULL) {
    treeWidget->setCurrentItem(treeWidget->itemAbove(item));
  }
}

void TableDockWidget::showNextGroup() {

  QTreeWidgetItem *item = treeWidget->currentItem();
  if (item == NULL)
    return;

  // get next item
  QTreeWidgetItem *nextitem = treeWidget->itemBelow(item);
  if (nextitem != NULL)
    treeWidget->setCurrentItem(nextitem);
}

void TableDockWidget::Train() {

  Classifier *clsf = _mainwindow->getClassifier();

  if (allgroups.size() == 0)
    return;
  if (clsf == NULL)
    return;

  vector<PeakGroup *> train_groups;
  vector<PeakGroup *> test_groups;
  vector<PeakGroup *> good_groups;
  vector<PeakGroup *> bad_groups;
  MTRand mtrand;

  for (int i = 0; i < allgroups.size(); i++) {
    PeakGroup *grp = &allgroups[i];
    if (grp->label == 'g')
      good_groups.push_back(grp);
    if (grp->label == 'b')
      bad_groups.push_back(grp);
  }

  mzUtils::shuffle(good_groups);
  for (int i = 0; i < good_groups.size(); i++) {
    PeakGroup *grp = good_groups[i];
    i % 2 == 0 ? train_groups.push_back(grp) : test_groups.push_back(grp);
  }

  mzUtils::shuffle(bad_groups);
  for (int i = 0; i < bad_groups.size(); i++) {
    PeakGroup *grp = bad_groups[i];
    i % 2 == 0 ? train_groups.push_back(grp) : test_groups.push_back(grp);
  }

  clsf->train(train_groups);
  clsf->classify(test_groups);
  showAccuracy(test_groups);
  updateTable();
}

void TableDockWidget::keyPressEvent(QKeyEvent *e) {

  QTreeWidgetItem *item = treeWidget->currentItem();
  if (e->key() == Qt::Key_Delete) {
    QList<QTreeWidgetItem *> items = treeWidget->selectedItems();
    if (items.size() > 0) {
      cerr << items.size() << endl;
      deleteGroups();
    }
  } else if (e->key() == Qt::Key_T) {
    if (item) {
      Train();
    }
  } else if (e->key() == Qt::Key_G) {

    if (item) {
      markGroupGood();
    }
  } else if (e->key() == Qt::Key_B) {

    if (item) {
      markGroupBad();
    }
  } else if (e->key() == Qt::Key_Left) {

    if (treeWidget->currentItem()) {
      if (treeWidget->currentItem()->parent()) {
        treeWidget->collapseItem(treeWidget->currentItem()->parent());
        treeWidget->setCurrentItem(treeWidget->currentItem()->parent());
      } else {
        treeWidget->collapseItem(treeWidget->currentItem());
      }
    }
  } else if (e->key() == Qt::Key_Right) {

    if (treeWidget->currentItem()) {
      if (!treeWidget->currentItem()->isExpanded()) {
        treeWidget->expandItem(treeWidget->currentItem());
      }
    }
  } else if (e->key() == Qt::Key_O) {
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
  } else if (e->key() == Qt::Key_Down && e->modifiers() == Qt::ShiftModifier) {
    if (treeWidget->itemBelow(item)) {
      if (tableSelectionFlagDown) {
        treeWidget->selectionModel()->setCurrentIndex(
            treeWidget->currentIndex(),
            QItemSelectionModel::Toggle | QItemSelectionModel::Rows);
        tableSelectionFlagDown = false;
      } else {
        treeWidget->selectionModel()->setCurrentIndex(
            treeWidget->indexBelow(treeWidget->currentIndex()),
            QItemSelectionModel::Toggle | QItemSelectionModel::Rows);
      }
      tableSelectionFlagUp = true;
    }
  } else if (e->key() == Qt::Key_Up && e->modifiers() == Qt::ShiftModifier) {
    if (treeWidget->itemAbove(item)) {
      if (tableSelectionFlagUp) {
        treeWidget->selectionModel()->setCurrentIndex(
            treeWidget->currentIndex(),
            QItemSelectionModel::Toggle | QItemSelectionModel::Rows);
        tableSelectionFlagUp = false;
      } else {
        treeWidget->selectionModel()->setCurrentIndex(
            treeWidget->indexAbove(treeWidget->currentIndex()),
            QItemSelectionModel::Toggle | QItemSelectionModel::Rows);
      }
      tableSelectionFlagDown = true;
    }
  } else if (e->key() == Qt::Key_Down) {

    if (treeWidget->itemBelow(item)) {
      treeWidget->setCurrentItem(treeWidget->itemBelow(item));
    }
  } else if (e->key() == Qt::Key_Up) {

    if (treeWidget->itemAbove(item)) {
      treeWidget->setCurrentItem(treeWidget->itemAbove(item));
    }
  }
  QDockWidget::keyPressEvent(e);
  updateStatus();
}

void TableDockWidget::updateStatus() {

  int totalCount = 0;
  int goodCount = 0;
  int badCount = 0;
  int ignoredCount = 0;
  int predictedGood = 0;
  for (int i = 0; i < allgroups.size(); i++) {
    char groupLabel = allgroups[i].label;
    if (groupLabel == 'g') {
      goodCount++;
    } else if (groupLabel == 'b') {
      badCount++;
    }
    totalCount++;
  }
  QString title =
      tr("Group Validation Status: Good=%2 Bad=%3 Total=%1")
          .arg(QString::number(totalCount),
               QString::number(goodCount),
               QString::number(badCount));
  _mainwindow->setStatusText(title);
}

float TableDockWidget::showAccuracy(vector<PeakGroup *> &groups) {
  // check accuracy
  if (groups.size() == 0)
    return 0;

  int fp = 0;
  int fn = 0;
  int tp = 0;
  int tn = 0;
  int total = 0;
  float accuracy = 0;
  int gc = 0;
  int bc = 0;
  for (int i = 0; i < groups.size(); i++) {
    if (groups[i]->label == 'g' || groups[i]->label == 'b') {
      for (int j = 0; j < groups[i]->peaks.size(); j++) {
        float q = groups[i]->peaks[j].quality;
        char l = groups[i]->peaks[j].label;
        if (l == 'g')
          gc++;
        if (l == 'g' && q > _mainwindow->mavenParameters->minQuality)
          tp++;
        if (l == 'g' && q < _mainwindow->mavenParameters->minQuality)
          fn++;

        if (l == 'b')
          bc++;
        if (l == 'b' && q < _mainwindow->mavenParameters->minQuality)
          tn++;
        if (l == 'b' && q > _mainwindow->mavenParameters->minQuality)
          fp++;
        total++;
      }
    }
  }
  if (total > 0)
    accuracy = 1.00 - ((float)(fp + fn) / total);
  cerr << "TOTAL=" << total << endl;
  if (total == 0)
    return 0;

  cerr << "GC=" << gc << " BC=" << bc << endl;
  cerr << "TP=" << tp << " FN=" << fn << endl;
  cerr << "TN=" << tn << " FP=" << fp << endl;
  cerr << "Accuracy=" << accuracy << endl;

  traindialog->FN->setText(QString::number(fn));
  traindialog->FP->setText(QString::number(fp));
  traindialog->TN->setText(QString::number(tn));
  traindialog->TP->setText(QString::number(tp));
  traindialog->accuracy->setText(QString::number(accuracy * 100, 'f', 2));
  traindialog->show();
  _mainwindow->setStatusText(tr("Good Groups=%1 Bad Groups=%2 Accuracy=%3")
                                 .arg(QString::number(gc), QString::number(bc),
                                      QString::number(accuracy * 100)));

  return accuracy;
}

void TableDockWidget::showScatterPlot() {

  if (groupCount() == 0)
    return;
  _mainwindow->scatterDockWidget->setVisible(true);
  ((ScatterPlot *)_mainwindow->scatterDockWidget)->setTable(this);
  ((ScatterPlot *)_mainwindow->scatterDockWidget)->replot();
  ((ScatterPlot *)_mainwindow->scatterDockWidget)->contrastGroups();
}

void TableDockWidget::printPdfReport() {

  _mainwindow->getAnalytics()->hitEvent("Exports", "PDF", 0);
  QString dir = ".";
  QSettings *settings = _mainwindow->getSettings();
  if (settings->contains("lastDir"))
    dir = settings->value("lastDir").value<QString>();

  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save Group Report a PDF File"), dir, tr("*.pdf"));
  if (fileName.isEmpty())
    return;
  if (!fileName.endsWith(".pdf", Qt::CaseInsensitive))
    fileName = fileName + ".pdf";

  QPrinter printer;
  printer.setOutputFormat(QPrinter::PdfFormat);
  printer.setOrientation(QPrinter::Landscape);
  printer.setCreator("MAVEN Metabolics Analyzer");
  printer.setOutputFileName(fileName);

  QPainter painter;

  if (!painter.begin(&printer)) {
    // failed to open file
    qWarning("failed to open file, is it writable?");
    return;
  }

  if (printer.printerState() != QPrinter::Active) {
    qDebug() << "PrinterState:" << printer.printerState();
  }

  // PDF report only for selected groups
  QList<PeakGroup *> selected = getSelectedGroups();

  for (int i = 0; i < selected.size(); i++) {
    PeakGroup *grp = selected[i];
    _mainwindow->getEicWidget()->setPeakGroup(grp);
    _mainwindow->getEicWidget()->render(&painter);

    if (!printer.newPage()) {
      qWarning("failed in flushing page to disk, disk full?");
      return;
    }
  }
  painter.end();
}

void TableDockWidget::showHeatMap() {

  _mainwindow->heatMapDockWidget->setVisible(true);
  HeatMap *_heatmap = _mainwindow->heatmap;
  if (_heatmap) {
    _heatmap->setTable(this);
    _heatmap->replot();
  }
}

void TableDockWidget::contextMenuEvent(QContextMenuEvent *event) {
  QMenu menu;

  QAction *z0 = menu.addAction("Copy to Clipboard");
  connect(z0, SIGNAL(triggered()), this, SLOT(setClipboard()));

  QAction *z3 = menu.addAction("Align Groups");
  connect(z3, SIGNAL(triggered()), SLOT(align()));

  QAction *z4 = menu.addAction("Find Matching Compound");
  connect(z4, SIGNAL(triggered()), SLOT(findMatchingCompounds()));

  QAction *z5 = menu.addAction("Delete All Groups");
  connect(z5, SIGNAL(triggered()), SLOT(deleteAll()));

  QAction *z6 = menu.addAction("Show Hidden Groups");
  connect(z6, SIGNAL(triggered()), SLOT(unhideFocusedGroups()));

  QAction *z7 = menu.addAction("Show Consensus Spectra");
  connect(z7, SIGNAL(triggered()), SLOT(showConsensusSpectra()));

  QAction *selectedAction = menu.exec(event->globalPos());
}

void TableDockWidget::focusInEvent(QFocusEvent *event) {
  if (event->gotFocus()) {
    pal.setColor(QPalette::Background, QColor(255, 255, 255, 100));
    setPalette(pal);
    updateCompoundWidget();
  }
}

void TableDockWidget::focusOutEvent(QFocusEvent *event) {
  if (event->lostFocus()) {
    pal.setColor(QPalette::Background, QColor(170, 170, 170, 100));
    setPalette(pal);
  }
}

void TableDockWidget::saveModel() {

  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save Classification Model to a File"));
  if (fileName.isEmpty())
    return;

  if (!fileName.endsWith(".model", Qt::CaseInsensitive))
    fileName = fileName + ".model";

  Classifier *clsf = _mainwindow->getClassifier();
  if (clsf != NULL) {
    clsf->saveModel(fileName.toStdString());
  }

  if (clsf) {
    vector<PeakGroup *> groups;
    for (int i = 0; i < allgroups.size(); i++)
      if (allgroups[i].label == 'g' || allgroups[i].label == 'b')
        groups.push_back(&allgroups[i]);
    clsf->saveFeatures(groups, fileName.toStdString() + ".csv");
  }
}

void TableDockWidget::findMatchingCompounds() {
  // matching compounds
  MassCutoff *massCutoff = _mainwindow->getUserMassCutoff();
  float ionizationMode = _mainwindow->mavenParameters->ionizationMode;
  for (int i = 0; i < allgroups.size(); i++) {
    PeakGroup &g = allgroups[i];
    int charge = _mainwindow->mavenParameters->getCharge(g.compound);
    QSet<Compound *> compounds =
        _mainwindow->massCalcWidget->findMathchingCompounds(g.meanMz,
                                                            massCutoff,
                                                            charge);
    if (compounds.size() > 0)
      Q_FOREACH (Compound *c, compounds) {
        g.tagString += " |" + c->name;
        break;
      }
  }
  updateTable();
}

void TableDockWidget::writeQEInclusionList(QString filename) {
  QFile file(filename);
  if (!file.open(QFile::WriteOnly)) {
    QErrorMessage errDialog(this);
    errDialog.showMessage("File open " + filename + " failed");
    return; // error
  }

  QList<PeakGroup *> selected = getSelectedGroups();

  float window = 1.5;
  int polarity = _mainwindow->mavenParameters->ionizationMode;
  QTextStream out(&file);
  for (int i = 0; i < selected.size(); i++) {
    PeakGroup *g = selected[i];
    out << g->meanMz << ",";
    polarity > 0 ? out << "Positive," : out << "Negative,";
    out << g->meanRt - window << ",";
    out << g->meanRt + window << ",";
    out << 25 << ","; // default CE set to 25
    out << 2 << ",";
    out << QString(g->getName().c_str());
    out << endl;
  }
  file.close();
}

void TableDockWidget::writeMascotGeneric(QString filename) {
  QFile file(filename);
  if (!file.open(QFile::WriteOnly)) {
    QErrorMessage errDialog(this);
    errDialog.showMessage("File open " + filename + " failed");
    return; // error
  }

  QList<PeakGroup *> selected = getSelectedGroups();
  QTextStream out(&file);
  for (int i = 0; i < selected.size(); i++) {
    PeakGroup *g = selected[i];
    Scan *cons =
        g->getAverageFragmenationScan(_mainwindow->getUserMassCutoff());

    if (cons) {
      string scandata = cons->toMGF();
      out << scandata.c_str();
    }
  }
  file.close();
}

void TableDockWidget::align() {
  if (allgroups.size() > 0) {
    vector<PeakGroup *> groups;
    for (int i = 0; i < allgroups.size(); i++)
      groups.push_back(&allgroups[i]);
    Aligner aligner;
    aligner.setMaxIterations(
        _mainwindow->alignmentDialog->maxIterations->value());
    aligner.setPolymialDegree(
        _mainwindow->alignmentDialog->polynomialDegree->value());
    aligner.doAlignment(groups);
    _mainwindow->getEicWidget()->replotForced();
    showSelectedGroup();
  }
}

void TableDockWidget::clearClusters() {

  for (unsigned int i = 0; i < allgroups.size(); i++)
    allgroups[i].clusterId = 0;
  showAllGroups();
}

void TableDockWidget::clusterGroups() {

  sort(allgroups.begin(), allgroups.end(), PeakGroup::compRt);
  qDebug() << "Clustering..";
  int clusterId = 0;

  QSettings *settings = _mainwindow->getSettings();
  double maxRtDiff = clusterDialog->maxRtDiff_2->value();
  double minSampleCorrelation = clusterDialog->minSampleCorr->value();
  double minRtCorrelation = clusterDialog->minRt->value();
  MassCutoff *massCutoff = _mainwindow->getUserMassCutoff();

  vector<mzSample *> samples = _mainwindow->getSamples();

  // clear cluster information
  for (unsigned int i = 0; i < allgroups.size(); i++)
    allgroups[i].clusterId = 0;
  map<int, PeakGroup *> parentGroups;

  for (unsigned int i = 0; i < allgroups.size(); i++) {
    PeakGroup &group1 = allgroups[i];

    if (group1.clusterId == 0) {
      // create new cluster
      group1.clusterId = ++clusterId;
      parentGroups[clusterId] = &group1;
    }

    // cluster parent
    PeakGroup *parent = parentGroups[clusterId];

    mzSample *largestSample = NULL;
    double maxIntensity = 0;
    int countCheckRaghu = 0;

    for (int i = 0; i < group1.peakCount(); i++) {
      mzSample *sample = group1.peaks[i].getSample();
      if (group1.peaks[i].peakIntensity > maxIntensity)
        largestSample = sample;
    }

    if (largestSample == NULL)
      continue;
    vector<float> peakIntensityA =
        group1.getOrderedIntensityVector(samples, PeakGroup::AreaTop);

    for (unsigned int j = i + 1; j < allgroups.size(); j++) {
      PeakGroup &group2 = allgroups[j];
      if (group2.clusterId > 0)
        continue;

      // retention time distance
      float rtdist = abs(parent->meanRt - group2.meanRt);
      if (rtdist > maxRtDiff * 2)
        continue;

      // retention time overlap
      float rtoverlap = mzUtils::checkOverlap(group1.minRt, group1.maxRt,
                                              group2.minRt, group2.maxRt);
      if (rtoverlap < 0.1)
        continue;

      // peak intensity correlation
      vector<float> peakIntensityB =
          group2.getOrderedIntensityVector(samples, PeakGroup::AreaTop);
      float cor = correlation(peakIntensityA, peakIntensityB);
      if (cor < minSampleCorrelation)
        continue;

      // peak shape correlation
      float cor2 = largestSample->correlation(group1.meanMz, group2.meanMz,
                                              massCutoff,
                                              group1.minRt, group1.maxRt,
                                              _mainwindow->mavenParameters->eicType,
                                              _mainwindow->mavenParameters->filterline);
      if (cor2 < minRtCorrelation)
        continue;

      // passed all the filters.. group1 and group2 into a single metagroup
      group2.clusterId = group1.clusterId;
    }
    if (i % 10 == 0)
      _mainwindow->setProgressBar("Clustering.,", i + 1, allgroups.size());
  }

  _mainwindow->setProgressBar("Clustering., done!",
                              allgroups.size(),
                              allgroups.size());
  showAllGroups();
}

void TableDockWidget::setupFiltersDialog() {

  filtersDialog = new QDialog(this);
  QVBoxLayout *layout = new QVBoxLayout(filtersDialog);

  sliders["PeakQuality"] = new QHistogramSlider(this);
  sliders["PeakIntensity"] = new QHistogramSlider(this);
  sliders["PeakWidth"] = new QHistogramSlider(this);
  sliders["GaussianFit"] = new QHistogramSlider(this);
  sliders["PeakAreaFractional"] = new QHistogramSlider(this);
  sliders["PeakAreaTop"] = new QHistogramSlider(this);
  sliders["S/N Ratio"] = new QHistogramSlider(this);
  sliders["GoodPeakCount"] = new QHistogramSlider(this);

  Q_FOREACH (QHistogramSlider *slider, sliders) {
    connect(slider, SIGNAL(minBoundChanged(double)), SLOT(filterPeakTable()));
    connect(slider, SIGNAL(maxBoundChanged(double)), SLOT(filterPeakTable()));
    layout->addWidget(slider);
  }

  filtersDialog->setLayout(layout);
}

void TableDockWidget::showFiltersDialog() {
  filtersDialog->setVisible(!filtersDialog->isVisible());
  if (filtersDialog->isVisible() == false)
    return;

  Q_FOREACH (QHistogramSlider *slider, sliders) { slider->clearData(); }

  for (int i = 0; i < 100; i++)
    sliders["PeakQuality"]->addDataPoint(QPointF((float)i / 100.00, i));
  for (int i = 0; i < 50; i++)
    sliders["GoodPeakCount"]->addDataPoint(QPointF(i, 5));
  for (int i = 0; i < 100; i++)
    sliders["PeakIntensity"]->addDataPoint(QPointF(i, i));
  sliders["PeakQuality"]->setPrecision(2);
  Q_FOREACH (QHistogramSlider *slider, sliders)
    slider->recalculatePlotBounds();
}

void TableDockWidget::filterPeakTable() { updateTable(); }

void TableDockWidget::showFocusedGroups() {
  int N = treeWidget->topLevelItemCount();
  for (int i = 0; i < N; i++) {
    QTreeWidgetItem *item = treeWidget->topLevelItem(i);
    QVariant v = item->data(0, Qt::UserRole);
    PeakGroup *group = v.value<PeakGroup *>();
    if (group && group->isFocused)
      item->setHidden(false);
    else
      item->setHidden(true);

    if (item->text(0).startsWith("Cluster")) {
      bool showParentFlag = false;
      for (int j = 0; j < item->childCount(); j++) {
        QVariant v = (item->child(j))->data(0, Qt::UserRole);
        PeakGroup *group = v.value<PeakGroup *>();
        if (group && group->isFocused) {
          item->setHidden(false);
          showParentFlag = true;
        } else
          item->setHidden(true);
      }
      if (showParentFlag)
        item->setHidden(false);
    }
  }
}

void TableDockWidget::clearFocusedGroups() {
  for (int i = 0; i < allgroups.size(); i++) {
    allgroups[i].isFocused = false;
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

void TableDockWidget::dragEnterEvent(QDragEnterEvent *event) {
  Q_FOREACH (QUrl url, event->mimeData()->urls()) {
    std::cerr << "dragEnterEvent:" << url.toString().toStdString() << endl;
    if (url.toString() == "ok") {
      event->acceptProposedAction();
      return;
    } else {
      return;
    }
  }
}

void TableDockWidget::dropEvent(QDropEvent *event) {
  Q_FOREACH (QUrl url, event->mimeData()->urls()) {
    std::cerr << "dropEvent:" << url.toString().toStdString() << endl;
  }
}

int TableDockWidget::loadSpreadsheet(QString fileName) {
  qDebug() << "Loading SpreadSheet   : " << fileName;

  if (fileName.endsWith(".txt", Qt::CaseInsensitive)) {
    loadCSVFile(fileName, "\t");
  } else if (fileName.endsWith(".csv", Qt::CaseInsensitive)) {
    loadCSVFile(fileName, ",");
  } else if (fileName.endsWith(".tsv", Qt::CaseInsensitive)) {
    loadCSVFile(fileName, "\t");
  } else if (fileName.endsWith(".tab", Qt::CaseInsensitive)) {
    loadCSVFile(fileName, "\t");
  }
}

int TableDockWidget::loadCSVFile(QString filename, QString sep = "\t") {

  if (filename.isEmpty())
    return 0;

  QFile myfile(filename);
  if (!myfile.open(QIODevice::ReadOnly | QIODevice::Text))
    return 0;

  QTextStream stream(&myfile);
  if (stream.atEnd())
    return 0;

  QString line;
  int lineCount = 0;
  QMap<QString, int> headerMap;
  QStringList header;

  do {
    line = stream.readLine();
    if (line.isEmpty() || line[0] == '#')
      continue;
    QStringList fields = line.split(sep);
    lineCount++;
    if (lineCount == 1) {
      // header line
      for (int i = 0; i < fields.size(); i++) {
        fields[i] = fields[i].toLower();
        fields[i].replace("\"", "");
        headerMap[fields[i]] = i;
        header << fields[i];
      }
      qDebug() << header << endl;
    } else {
      PeakGroup *g = new PeakGroup();
      if (headerMap.contains("name"))
        g->tagString = fields[headerMap["name"]].toStdString();
      if (headerMap.contains("mz"))
        g->meanMz = fields[headerMap["mz"]].toFloat();
      if (headerMap.contains("mzmed"))
        g->meanMz = fields[headerMap["mzmed"]].toFloat();
      if (headerMap.contains("mzmin"))
        g->minMz = fields[headerMap["mzmin"]].toFloat();
      if (headerMap.contains("mzmax"))
        g->maxMz = fields[headerMap["mzmax"]].toFloat();

      if (headerMap.contains("rt"))
        g->meanRt = fields[headerMap["rt"]].toFloat() / 60;
      if (headerMap.contains("rtmed"))
        g->meanRt = fields[headerMap["rtmed"]].toFloat() / 60;
      if (headerMap.contains("rtmin"))
        g->minRt = fields[headerMap["rtmin"]].toFloat() / 60;
      if (headerMap.contains("rtmax"))
        g->maxRt = fields[headerMap["rtmax"]].toFloat() / 60;

      if (headerMap.contains("fold"))
        g->changeFoldRatio = fields[headerMap["fold"]].toFloat();
      if (headerMap.contains("pvalue"))
        g->changePValue = fields[headerMap["pvalue"]].toFloat();

      for (unsigned int i = 14; i < header.size(); i++) {
        Peak p;
        p.peakIntensity = fields[i].toInt();
        p.rt = g->meanRt;
        p.rtmin = g->minRt;
        p.rtmax = g->maxRt;
        p.peakMz = g->meanMz;
        p.mzmin = g->minMz;
        p.mzmax = g->maxMz;

        g->addPeak(p);
      }

      if (g->meanMz > 0) {
        addPeakGroup(g);
      }
      delete (g);
    }
  } while (!line.isNull());

  showAllGroups();
  return lineCount;
}

void TableDockWidget::switchTableView() {
  viewType == groupView ? viewType = peakView : viewType = groupView;
  setupPeakTable();
  showAllGroups();
  updateTable();
}

QWidget *TableToolBarWidgetAction::createWidget(QWidget *parent) {
  if (btnName == "titlePeakTable") {

    td->titlePeakTable = new QLabel(parent);
    QFont font;
    font.setPointSize(14);
    td->titlePeakTable->setFont(font);
    td->setStyleSheet("QLabel { margin: 0px 6px; }");

    if (td->tableId == 0)
      td->titlePeakTable->setText("Bookmark Table");
    else
      td->titlePeakTable->setText("Peak Table "
                                  + QString::number(td->tableId));

    td->titlePeakTable->setStyleSheet("font-weight: bold; color: black");
    td->setWindowTitle(td->titlePeakTable->text());

    return td->titlePeakTable;
  } else if (btnName == "btnSwitchView") {

    QToolButton *btnSwitchView = new QToolButton(parent);
    btnSwitchView->setIcon(QIcon(rsrcPath + "/flip.png"));
    btnSwitchView->setToolTip("Switch between Group and Peak Views");
    connect(btnSwitchView, SIGNAL(clicked()), td, SLOT(switchTableView()));
    return btnSwitchView;
  } else if (btnName == "btnGroupCSV") {

    QToolButton *btnGroupCSV = new QToolButton(parent);

    btnGroupCSV->setIcon(QIcon(rsrcPath + "/exportcsv.png"));
    btnGroupCSV->setToolTip(tr("Export Groups To SpreadSheet (.csv) "));
    btnGroupCSV->setMenu(new QMenu("Export Groups"));
    btnGroupCSV->setPopupMode(QToolButton::InstantPopup);
    QAction *exportSelected =
        btnGroupCSV->menu()->addAction(tr("Export Selected"));
    QAction *exportAll =
        btnGroupCSV->menu()->addAction(tr("Export All Groups"));
    QAction *exportGood = btnGroupCSV->menu()->addAction(tr("Export Good"));
    QAction *exportBad = btnGroupCSV->menu()->addAction(tr("Export Bad"));

    connect(exportSelected, SIGNAL(triggered()), td, SLOT(selectedPeakSet()));
    connect(exportSelected,
            SIGNAL(triggered()),
            td,
            SLOT(exportGroupsToSpreadsheet()));
    connect(exportSelected, SIGNAL(triggered()), td, SLOT(showNotification()));

    connect(exportAll, SIGNAL(triggered()), td, SLOT(wholePeakSet()));
    connect(exportAll, SIGNAL(triggered()), td->treeWidget, SLOT(selectAll()));
    connect(exportAll,
            SIGNAL(triggered()),
            td,
            SLOT(exportGroupsToSpreadsheet()));
    connect(exportAll, SIGNAL(triggered()), td, SLOT(showNotification()));

    connect(exportGood, SIGNAL(triggered()), td, SLOT(goodPeakSet()));
    connect(exportGood, SIGNAL(triggered()), td->treeWidget, SLOT(selectAll()));
    connect(exportGood,
            SIGNAL(triggered()),
            td,
            SLOT(exportGroupsToSpreadsheet()));
    connect(exportGood, SIGNAL(triggered()), td, SLOT(showNotification()));

    connect(exportBad, SIGNAL(triggered()), td, SLOT(badPeakSet()));
    connect(exportBad, SIGNAL(triggered()), td->treeWidget, SLOT(selectAll()));
    connect(exportBad,
            SIGNAL(triggered()),
            td,
            SLOT(exportGroupsToSpreadsheet()));
    connect(exportBad, SIGNAL(triggered()), td, SLOT(showNotification()));
    return btnGroupCSV;
  } else if (btnName == "btnSaveJson") {

    QToolButton *btnSaveJson = new QToolButton(parent);
    btnSaveJson->setIcon(QIcon(rsrcPath + "/JSON.png"));
    btnSaveJson->setToolTip(tr("Export EICs to Json (.json)"));
    connect(btnSaveJson, SIGNAL(clicked()), td, SLOT(exportJson()));
    connect(btnSaveJson, SIGNAL(clicked()), td, SLOT(showNotification()));
    return btnSaveJson;
  } else if (btnName == "btnMlStatistics") {

    QToolButton *btnMlStatistics = new QToolButton(parent);
    btnMlStatistics->setIcon(QIcon(rsrcPath + "/train.png"));
    btnMlStatistics->setToolTip(tr("Get insights about the ML model"));
    connect(btnMlStatistics, SIGNAL(clicked()), td, SLOT(ShowStatistics()));
    return btnMlStatistics;
  }else if (btnName == "btnScatter") {

    QToolButton *btnScatter = new QToolButton(parent);
    btnScatter->setIcon(QIcon(rsrcPath + "/scatterplot.png"));
    btnScatter->setToolTip("Show ScatterPlot");
    connect(btnScatter, SIGNAL(clicked()), td, SLOT(showScatterPlot()));
    return btnScatter;
  } else if (btnName == "btnCluster") {

    QToolButton *btnCluster = new QToolButton(parent);
    btnCluster->setIcon(QIcon(rsrcPath + "/cluster.png"));
    btnCluster->setToolTip("Cluster Groups");
    connect(btnCluster, SIGNAL(clicked()), td, SLOT(showClusterDialog()));
    return btnCluster;
  } else if (btnName == "btnTrain") {

    QToolButton *btnTrain = new QToolButton(parent);
    btnTrain->setIcon(QIcon(rsrcPath + "/train.png"));
    btnTrain->setToolTip("Train Neural Net");
    connect(btnTrain, SIGNAL(clicked()), td, SLOT(showTrainDialog()));
    return btnTrain;
  } else if (btnName == "btnGood") {

    QToolButton *btnGood = new QToolButton(parent);
    btnGood->setIcon(QIcon(rsrcPath + "/markgood.png"));
    btnGood->setToolTip("Mark Group as Good");
    connect(btnGood, SIGNAL(clicked()), td, SLOT(markGroupGood()));
    return btnGood;
  } else if (btnName == "btnBad") {

    QToolButton *btnBad = new QToolButton(parent);
    btnBad->setIcon(QIcon(rsrcPath + "/markbad.png"));
    btnBad->setToolTip("Mark Good as Bad");
    connect(btnBad, SIGNAL(clicked()), td, SLOT(markGroupBad()));
    return btnBad;
  } else if (btnName == "btnHeatmapelete") {

    QToolButton *btnHeatmapelete = new QToolButton(parent);
    btnHeatmapelete->setIcon(QIcon(rsrcPath + "/delete.png"));
    btnHeatmapelete->setToolTip("Delete Group");
    connect(btnHeatmapelete, SIGNAL(clicked()), td, SLOT(deleteGroups()));
    return btnHeatmapelete;
  } else if (btnName == "btnPDF") {

    QToolButton *btnPDF = new QToolButton(parent);
    btnPDF->setIcon(QIcon(rsrcPath + "/PDF.png"));
    btnPDF->setToolTip("Generate PDF Report");
    connect(btnPDF, SIGNAL(clicked()), td, SLOT(printPdfReport()));
    connect(btnPDF, SIGNAL(clicked()), td, SLOT(showNotification()));
    return btnPDF;
  } else if (btnName == "btnX") {

    QToolButton *btnX = new QToolButton(parent);
    btnX->setIcon(td->style()->standardIcon(QStyle::SP_DockWidgetCloseButton));
    connect(btnX, SIGNAL(clicked()), td, SLOT(showDeletionDialog()));
    return btnX;
  } else if (btnName == "btnMin") {

    QToolButton *btnMin = new QToolButton(parent);
    btnMin->setIcon(td->style()->standardIcon(QStyle::SP_TitleBarMinButton));
    connect(btnMin, SIGNAL(clicked()), td, SLOT(hide()));
    return btnMin;
  } else {
    return NULL;
  }
}

PeakTableDockWidget::PeakTableDockWidget(MainWindow *mw, const int peakTableId)
  : TableDockWidget(mw) {

  _mainwindow = mw;

  if (peakTableId > mw->lastPeakTableId)
    tableId = mw->lastPeakTableId = peakTableId;
  else
    tableId = ++(mw->lastPeakTableId);

  toolBar = new QToolBar(this);
  toolBar->setFloatable(false);
  toolBar->setMovable(false);

  QWidgetAction *titlePeakTable =
      new TableToolBarWidgetAction(toolBar, this, "titlePeakTable");
  QWidgetAction *btnSwitchView =
      new TableToolBarWidgetAction(toolBar, this, "btnSwitchView");
  QWidgetAction *btnGroupCSV =
      new TableToolBarWidgetAction(toolBar, this, "btnGroupCSV");
  QWidgetAction *btnSaveJson =
      new TableToolBarWidgetAction(toolBar, this, "btnSaveJson");
  QWidgetAction *btnMlStatistics =
      new TableToolBarWidgetAction(toolBar, this, "btnMlStatistics");
  QWidgetAction *btnScatter =
      new TableToolBarWidgetAction(toolBar, this, "btnScatter");
  QWidgetAction *btnCluster =
      new TableToolBarWidgetAction(toolBar, this, "btnCluster");
  QWidgetAction *btnTrain =
      new TableToolBarWidgetAction(toolBar, this, "btnTrain");
  QWidgetAction *btnGood =
      new TableToolBarWidgetAction(toolBar, this, "btnGood");
  QWidgetAction *btnBad = new TableToolBarWidgetAction(toolBar, this, "btnBad");
  QWidgetAction *btnHeatmapelete =
      new TableToolBarWidgetAction(toolBar, this, "btnHeatmapelete");
  QWidgetAction *btnPDF = new TableToolBarWidgetAction(toolBar, this, "btnPDF");
  QWidgetAction *btnX = new TableToolBarWidgetAction(toolBar, this, "btnX");
  QWidgetAction *btnMin = new TableToolBarWidgetAction(toolBar, this, "btnMin");

  QWidget *spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  toolBar->addAction(titlePeakTable);
  toolBar->addAction(btnSwitchView);
  toolBar->addAction(btnGood);
  toolBar->addAction(btnBad);
  toolBar->addAction(btnTrain);
  toolBar->addAction(btnHeatmapelete);

  toolBar->addSeparator();
  toolBar->addAction(btnScatter);
  toolBar->addAction(btnCluster);

  toolBar->addSeparator();
  toolBar->addAction(btnPDF);
  toolBar->addAction(btnGroupCSV);
  toolBar->addAction(btnSaveJson);
  toolBar->addAction(btnMlStatistics);
  toolBar->addWidget(spacer);
  toolBar->addAction(btnMin);
  toolBar->addAction(btnX);

  setTitleBarWidget(toolBar);

  connect(this, &PeakTableDockWidget::unSetFromEicWidget, _mainwindow->getEicWidget(), &EicWidget::unSetPeakTableGroup);

  deletionDialog = new PeakTableDeletionDialog(this);
}

PeakTableDockWidget::~PeakTableDockWidget() {
  toolBar->clear();
  delete toolBar;
}

void PeakTableDockWidget::destroy() {

  cleanUp();
  deleteLater();
  _mainwindow->removePeaksTable(this);
}

void PeakTableDockWidget::cleanUp()
{
  if (treeWidget->currentItem())
    emit unSetFromEicWidget(treeWidget->currentItem()->data(0, Qt::UserRole).value<PeakGroup*>());
  _mainwindow->ligandWidget->resetColor();
}

void PeakTableDockWidget::showDeletionDialog() {
  deletionDialog->show();
}

BookmarkTableDockWidget::BookmarkTableDockWidget(MainWindow *mw) : TableDockWidget(mw) {
  _mainwindow = mw;
  tableId = 0;

  toolBar = new QToolBar(this);
  toolBar->setFloatable(false);
  toolBar->setMovable(false);
  btnMerge = new QToolButton(toolBar);
  btnMerge->setIcon(QIcon(rsrcPath + "/merge.png"));
  btnMerge->setToolTip("Merge Groups With");
  btnMergeMenu = new QMenu("Merge Groups");
  btnMerge->setMenu(btnMergeMenu);
  btnMerge->setPopupMode(QToolButton::InstantPopup);
  connect(btnMergeMenu,
          SIGNAL(aboutToShow()),
          SLOT(showMergeTableOptions()));
  connect(btnMergeMenu,
          SIGNAL(triggered(QAction *)),
          SLOT(mergeGroupsIntoPeakTable(QAction *)));

  QWidgetAction *titlePeakTable =
      new TableToolBarWidgetAction(toolBar, this, "titlePeakTable");
  QWidgetAction *btnSwitchView =
      new TableToolBarWidgetAction(toolBar, this, "btnSwitchView");
  QWidgetAction *btnGroupCSV =
      new TableToolBarWidgetAction(toolBar, this, "btnGroupCSV");
  QWidgetAction *btnSaveJson =
      new TableToolBarWidgetAction(toolBar, this, "btnSaveJson");
  QWidgetAction *btnMlStatistics =
      new TableToolBarWidgetAction(toolBar, this, "btnMlStatistics");
  QWidgetAction *btnScatter =
      new TableToolBarWidgetAction(toolBar, this, "btnScatter");
  QWidgetAction *btnCluster =
      new TableToolBarWidgetAction(toolBar, this, "btnCluster");
  QWidgetAction *btnTrain =
      new TableToolBarWidgetAction(toolBar, this, "btnTrain");
  QWidgetAction *btnGood =
      new TableToolBarWidgetAction(toolBar, this, "btnGood");
  QWidgetAction *btnBad = new TableToolBarWidgetAction(toolBar, this, "btnBad");
  QWidgetAction *btnHeatmapelete =
      new TableToolBarWidgetAction(toolBar, this, "btnHeatmapelete");
  QWidgetAction *btnPDF = new TableToolBarWidgetAction(toolBar, this, "btnPDF");
  QWidgetAction *btnX = new TableToolBarWidgetAction(toolBar, this, "btnX");
  QWidgetAction *btnMin = new TableToolBarWidgetAction(toolBar, this, "btnMin");

  QWidget *spacer = new QWidget();
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
  toolBar->addAction(btnCluster);

  toolBar->addSeparator();
  toolBar->addAction(btnPDF);
  toolBar->addAction(btnGroupCSV);
  toolBar->addAction(btnSaveJson);
  toolBar->addAction(btnMlStatistics);
  toolBar->addWidget(spacer);
  toolBar->addAction(btnMin);

  setTitleBarWidget(toolBar);

  promptDialog = new QDialog(this);
  promptDialogLayout = new QVBoxLayout();

  cancel = new QPushButton();
  cancel->setText("cancel");
  connect(cancel, SIGNAL(clicked()), this, SLOT(rejectGroup()));

  save = new QPushButton();
  save->setText("save");
  connect(save, SIGNAL(clicked()), this, SLOT(acceptGroup()));

  buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(cancel);
  buttonLayout->addWidget(save);

  upperLabel = new QLabel();
  upperLabel->setText(
      "Groups with same mz and rt.\nSelect and (ctrl+c) to copy");
  lowerLabel = new QLabel();
  lowerLabel->setText("Add this group too ?");

  listTextView = new ListView();
  stringModel = new QStringListModel(promptDialog);
}

BookmarkTableDockWidget::~BookmarkTableDockWidget() {
  toolBar->clear();
  delete toolBar;
}

void BookmarkTableDockWidget::showMergeTableOptions() {
  QList<QPointer<TableDockWidget>> peaksTableList =
      _mainwindow->getPeakTableList();
  int n = peaksTableList.size();
  btnMergeMenu->clear();
  mergeAction.clear();
  for (int i = 0; i < n; i++) {
    mergeAction.insert(
        btnMergeMenu->addAction(peaksTableList[i]->titlePeakTable->text()),
        peaksTableList[i]->tableId);
  }
}

void BookmarkTableDockWidget::showMsgBox(bool check, int tableNo) {

  QMessageBox *msgBox = new QMessageBox(this);
  msgBox->setAttribute(Qt::WA_DeleteOnClose);
  msgBox->setStandardButtons(QMessageBox::Ok);

  if (check) {
    msgBox->setIconPixmap(QPixmap(rsrcPath + "/success.png"));
    msgBox->setText("Successfully Merged from Bookmark Table to Table " +
                    QString::number(tableNo) + "   ");
  } else {
    msgBox->setIcon(QMessageBox::Warning);
    if (tableNo == -1)
      msgBox->setText("Error while merging");
    else
      msgBox->setText("Error while merging from Bookmark Table to Table " +
                      QString::number(tableNo) + "   ");
  }

  msgBox->open();
}

void BookmarkTableDockWidget::mergeGroupsIntoPeakTable(QAction *action)
{
    QList<QPointer<TableDockWidget>> peaksTableList = _mainwindow->getPeakTableList();
    int j = mergeAction.value(action, -1);

    //check if action exists
    if (j == -1) {
        showMsgBox(false, j);
        return;
    }

    //return if no bookmarked groups or peak tables
    if (allgroups.isEmpty() || peaksTableList.isEmpty()) {
        showMsgBox(true, j);
        return;
    }

    //find table to merge with
    TableDockWidget* peakTable;
    for (auto table : peaksTableList) {
        if (table->tableId == j) {
            peakTable = table;
            break;
        }
    }

    //return if peak table not found
    if (!peakTable) {
        showMsgBox(false, j);
        return;
    }

    int initialSize = peakTable->allgroups.size();
    int finalSize = allgroups.size() + initialSize;
    for (auto group : allgroups) {
        group.groupId = ++initialSize;
        peakTable->allgroups.append(group);
    }

    deleteAll();
    peakTable->showAllGroups();
    showAllGroups();

    bool merged = true;

    if (finalSize == peakTable->allgroups.size())
        merged = true;
    else
        merged = false;
    
    showMsgBox(merged, j);
    _mainwindow->getAnalytics()->hitEvent("Bookmark Table", "Merge Table", merged);
}

void BookmarkTableDockWidget::acceptGroup() {
  addSameMzRtGroup = true;
  promptDialog->close();
}

void BookmarkTableDockWidget::rejectGroup() {
  addSameMzRtGroup = false;
  promptDialog->close();
}

void BookmarkTableDockWidget::showSameGroup(QPair<int, int> sameMzRtGroupIndexHash) {

  QStringList list;

  for (int i = 0; i < sameMzRtGroups[sameMzRtGroupIndexHash].size(); ++i) {
    // saving all compound name of same rt and mz value to <list> variable
    list.append(sameMzRtGroups[sameMzRtGroupIndexHash][i]);
    qDebug() << sameMzRtGroups[sameMzRtGroupIndexHash][i];
  }

  stringModel->setStringList(list);

  listTextView->setModel(stringModel);
  listTextView->setData(list);
  listTextView->setSelectionMode(QAbstractItemView::MultiSelection);
  listTextView->setEditTriggers(QAbstractItemView::NoEditTriggers);

  // set all widget and labels to prompt dailog
  promptDialogLayout->insertWidget(0, upperLabel);
  promptDialogLayout->insertWidget(1, listTextView);
  promptDialogLayout->insertWidget(2, lowerLabel);
  promptDialogLayout->insertLayout(3, buttonLayout);
  promptDialog->setLayout(promptDialogLayout);
  promptDialog->exec();
}

bool BookmarkTableDockWidget::hasPeakGroup(PeakGroup *group) {

  int intMz = group->meanMz * 1e5;
  int intRt = group->meanRt * 1e5;
  QPair<int, int> sameMzRtGroupIndexHash(intMz, intRt);
  QString compoundName = QString::fromStdString(group->getName());

  if (allgroups.size() == 0 ||
      sameMzRtGroups[sameMzRtGroupIndexHash].size() == 0) {
    /**
     * add this group corresponding compound name to list of string which all
     * share same mz and rt value. Both mz and rt are hashed in
     * sameMzRtGroupIndexHash.
     */
    sameMzRtGroups[sameMzRtGroupIndexHash].append(compoundName);
  }

  for (int i = 0; i < allgroups.size(); i++) {
    if (&allgroups[i] == group)
      return true;
    if ((double)std::abs(group->meanMz - allgroups[i].meanMz) < 1e-5 &&
        (double)std::abs(group->meanRt - allgroups[i].meanRt) < 1e-5) {
      addSameMzRtGroup = false;
      if (!sameMzRtGroups[sameMzRtGroupIndexHash].contains(compoundName)) {

        showSameGroup(sameMzRtGroupIndexHash);
        /**
         * if bookmarked list has group with same mz and rt, loop will hold the
         * execution of this method after showing the prompt dialog to choose
         * whether to add this group by above method <showSameGroup>.
         */
        QEventLoop loop;
        connect(save, SIGNAL(clicked()), &loop, SLOT(quit()));
        connect(cancel, SIGNAL(clicked()), &loop, SLOT(quit()));
      }
      if (addSameMzRtGroup) {
        /**
         * if user pressed <save> button <addSameMzRtGroup> will be set to true
         * otherwise false. if it is true, this groups corresponding compound
         * name will we saved by an index of sameMzRtGroupIndexHash to show all
         * these string next time if a group with same rt and mz is encountered.
         */
        sameMzRtGroups[sameMzRtGroupIndexHash].append(compoundName);
        /**
         * return false such that calling method will add this group to
         * bookmarked group.
         */
        return false;
      }

      return true;
    }
  }

  return false;
}

void BookmarkTableDockWidget::deleteGroup(PeakGroup *groupX) {
  if (!groupX)
    return;

  int pos = -1;
  for (int i = 0; i < allgroups.size(); i++) {
    if (&allgroups[i] == groupX) {
      pos = i;
      break;
    }
  }
  if (pos == -1)
    return;

  QTreeWidgetItemIterator it(treeWidget);
  while (*it) {
    QTreeWidgetItem *item = (*it);
    if (item->isHidden()) {
      ++it;
      continue;
    }
    QVariant v = item->data(0, Qt::UserRole);
    PeakGroup *group = v.value<PeakGroup *>();
    if (group != NULL and group == groupX) {
        item->setHidden(true);

	    if (group->children.size() > 0)
            	labeledGroups--;

      	// Deleting
        int posTree = treeWidget->indexOfTopLevelItem(item);
        if (posTree != -1)
    		treeWidget->takeTopLevelItem(posTree);

        /**
         * delete name of compound associated with this group stored in
         * <sameMzRtGroups> with given mz and rt
        */
        int intMz = group->meanMz * 1e5;
        int intRt = group->meanRt * 1e5;
        QPair<int, int> sameMzRtGroupIndexHash(intMz, intRt);
        QString compoundName = QString::fromStdString(groupX->getName());
        if (sameMzRtGroups[sameMzRtGroupIndexHash].contains(compoundName)) {
        	for (int i = 0; i < sameMzRtGroups[sameMzRtGroupIndexHash].size();
            	 ++i)
			{
            	if (sameMzRtGroups[sameMzRtGroupIndexHash][i] == compoundName) {
            	sameMzRtGroups[sameMzRtGroupIndexHash].removeAt(i);
            	break;
          	}
        }
      }

      allgroups.erase(allgroups.begin() + pos);
      break;
    }
    ++it;
  }

  for (unsigned int i = 0; i < allgroups.size(); i++) {
    allgroups[i].groupId = i + 1;
  }
  updateTable();
  updateCompoundWidget();
}

void BookmarkTableDockWidget::markGroupGood() {
  setGroupLabel('g');
  showNextGroup();
  _mainwindow->peaksMarked++;
  if (checkLabeledGroups())
    _mainwindow->allPeaksMarked = true;
  _mainwindow->autoSaveSignal();
}

void BookmarkTableDockWidget::markGroupBad() {

  setGroupLabel('b');
  showNextGroup();
  _mainwindow->peaksMarked++;
  if (checkLabeledGroups())
    _mainwindow->allPeaksMarked = true;
  _mainwindow->autoSaveSignal();
}

ScatterplotTableDockWidget::ScatterplotTableDockWidget(MainWindow *mw) :
    TableDockWidget(mw) {
  _mainwindow = mw;

  toolBar = new QToolBar(this);
  toolBar->setFloatable(false);
  toolBar->setMovable(false);

  QWidgetAction *titlePeakTable =
      new TableToolBarWidgetAction(toolBar, this, "titlePeakTable");
  QWidgetAction *btnSwitchView =
      new TableToolBarWidgetAction(toolBar, this, "btnSwitchView");
  QWidgetAction *btnGroupCSV =
      new TableToolBarWidgetAction(toolBar, this, "btnGroupCSV");
  QWidgetAction *btnSaveJson =
      new TableToolBarWidgetAction(toolBar, this, "btnSaveJson");
  QWidgetAction *btnMlStatistics =
      new TableToolBarWidgetAction(toolBar, this, "btnMlStatistics");
  QWidgetAction *btnCluster =
      new TableToolBarWidgetAction(toolBar, this, "btnCluster");
  QWidgetAction *btnTrain =
      new TableToolBarWidgetAction(toolBar, this, "btnTrain");
  QWidgetAction *btnGood =
      new TableToolBarWidgetAction(toolBar, this, "btnGood");
  QWidgetAction *btnBad = new TableToolBarWidgetAction(toolBar, this, "btnBad");
  QWidgetAction *btnHeatmapelete =
      new TableToolBarWidgetAction(toolBar, this, "btnHeatmapelete");
  QWidgetAction *btnPDF = new TableToolBarWidgetAction(toolBar, this, "btnPDF");
  QWidgetAction *btnX = new TableToolBarWidgetAction(toolBar, this, "btnX");
  QWidgetAction *btnMin = new TableToolBarWidgetAction(toolBar, this, "btnMin");

  QWidget *spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  toolBar->addAction(titlePeakTable);
  toolBar->addAction(btnSwitchView);
  toolBar->addAction(btnGood);
  toolBar->addAction(btnBad);
  toolBar->addAction(btnTrain);
  toolBar->addAction(btnHeatmapelete);

  toolBar->addSeparator();
  toolBar->addAction(btnCluster);

  toolBar->addSeparator();
  toolBar->addAction(btnPDF);
  toolBar->addAction(btnGroupCSV);
  toolBar->addAction(btnSaveJson);
  toolBar->addAction(btnMlStatistics);
  toolBar->addWidget(spacer);
  toolBar->addAction(btnMin);

  setTitleBarWidget(toolBar);
}

ScatterplotTableDockWidget::~ScatterplotTableDockWidget() {
  toolBar->clear();
  delete toolBar;
}

void ScatterplotTableDockWidget::setupPeakTable() {

  QStringList colNames;

  // Add common columns to the table
  colNames << "#";
  colNames << "ID";
  colNames << "Observed m/z";
  colNames << "Expected m/z";
  colNames << "rt";

  if (viewType == groupView) {

    // Add group view columns to the table
    colNames << "rt delta";
    colNames << "#peaks";
    colNames << "#good";
    colNames << "Max Width";
    colNames << "Max AreaTop";
    colNames << "Max S/N";
    colNames << "Max Quality";
    colNames << "Rank";

    // add scatterplot table columns
    colNames << "Ratio Change";
    colNames << "P-value";
    colNames << "Avg Peak Quality";
    colNames << "Group Quality";
    colNames << "Weighted Peak Quality";
    colNames << "Predicted Label";
  } else if (viewType == peakView) {
    vector<mzSample *> vsamples = _mainwindow->getVisibleSamples();
    sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
    for (unsigned int i = 0; i < vsamples.size(); i++) {
      // Add peak view columns to the table
      colNames << QString(vsamples[i]->sampleName.c_str());
    }
  }

  treeWidget->setColumnCount(colNames.size());
  treeWidget->setHeaderLabels(colNames);
  treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  treeWidget->header()->adjustSize();
  treeWidget->setSortingEnabled(true);
}

//@Kailash: Put decision sequence/tree for automatic validation here
void TableDockWidget::validateGroup(PeakGroup* grp, QTreeWidgetItem* item)
{
    int mark=0;
    bool decisionConflict=false;
    if (grp != NULL)
    {
        //Disjoint Decision Tree
        //Require improvements

        //Decisions to mark group good
        if (grp->avgPeakQuality > 0.74 && grp->groupQuality > 0.69 && grp->weightedAvgPeakQuality > 0.73) {
            if (mark!=-1) mark=1;
            else decisionConflict=true;
        }
        else if (grp->groupQuality > 0.73) {
            if (mark!=-1) mark=1;
            else decisionConflict=true;
        }
        else if (grp->predictedLabel==1) {
            if (grp->avgPeakQuality > 0.76) {
                if (mark!=-1) mark=1;
                else decisionConflict=true;   
            }

            if (grp->weightedAvgPeakQuality > 0.69) {
                if (mark!=-1) mark=1;
                else decisionConflict=true;
            }
        }
        else if (grp->predictedLabel==0 && grp->avgPeakQuality > 0.72 && grp->weightedAvgPeakQuality > 0.76) {
            if (mark!=-1) mark=1;
            else decisionConflict=true;
        }

        //Decisions to mark group bad
        if (grp->avgPeakQuality < 0.25 && grp->weightedAvgPeakQuality < 0.35) {
            if (mark!=1) mark=-1;
            else decisionConflict=true;
        }

        if (grp->predictedLabel==-1) {
            if (grp->avgPeakQuality < 0.29) {
                if (mark!=1) mark=-1;
                else decisionConflict=true;
            }

            if (grp->weightedAvgPeakQuality < 0.31) {
                if (mark!=1) mark=-1;
                else decisionConflict=true;
            }
        }

        //Decisions to not mark group
        if (grp->peakCount() < int(maxPeaks/4) || grp->peakCount() == 1) decisionConflict=true; //Do not mark if number of peaks in the group is less

        if (abs(grp->avgPeakQuality - grp->weightedAvgPeakQuality) > 0.2) {
            decisionConflict=true;
        }

        if (grp->avgPeakQuality > 0.25 && abs(grp->avgPeakQuality - grp->maxQuality) > 0.3) {
            decisionConflict=true;
        }
        grp->markedGoodByCloudModel = 0;
        grp->markedBadByCloudModel = 0;
        //Call respected functions to mark the groups
        if (mark==1 && !decisionConflict) {
            // markGroupGood(grp, item);
            grp->markedGoodByCloudModel = 1;
        }
        else if (mark==-1 && !decisionConflict) {
            // markGroupBad(grp, item);
            grp->markedBadByCloudModel = 0;
        }
    }
}
 
void TableDockWidget::markGroupGood(PeakGroup* grp, QTreeWidgetItem* item)
{
    if(item && grp != NULL)
    {   if (!(grp->label=='g'||grp->label=='b')){
      numberOfGroupsMarked+=1;
      qDebug()<<"one more new group marked";
    }
      qDebug()<<"group already marked";
        grp->setLabel('g');
        updateItem(item);
    }
    updateStatus();
    if (_mainwindow->sessionCount==1){
    if (numberOfGroupsMarked ==10){
      _mainwindow->sessionCount+=1;
      Q_EMIT(tenPeaksMarked());
    }
  }
  else{
    qDebug()<<"session coiunt exceeds one.";
  }
}

void TableDockWidget::markGroupBad(PeakGroup* grp, QTreeWidgetItem* item)
{
    if(item && grp != NULL)
    {
        grp->setLabel('b');
        updateItem(item);
    }
    updateStatus();
}