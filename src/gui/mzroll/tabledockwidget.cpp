#include <algorithm>

#include <qtconcurrentrun.h>

#include "Compound.h"
#include "EIC.h"
#include "PeakGroup.h"
#include "Scan.h"
#include "alignmentdialog.h"
#include "backgroundopsthread.h"
#include "classificationWidget.h"
#include "classifierNeuralNet.h"
#include "clusterdialog.h"
#include "common/analytics.h"
#include "controller.h"
#include "correlationtable.h"
#include "csvreports.h"
#include "eicwidget.h"
#include "globals.h"
#include "grouprtwidget.h"
#include "groupsettingslog.h"
#include "isotopeswidget.h"
#include "jsonReports.h"
#include "ligandwidget.h"
#include "mainwindow.h"
#include "masscalcgui.h"
#include "mavenparameters.h"
#include "multiselectcombobox.h"
#include "mzAligner.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "notificator.h"
#include "numeric_treewidgetitem.h"
#include "peakdetector.h"
#include "peakeditor.h"
#include "peaktabledeletiondialog.h"
#include "relabelGroupsDialog.h"
#include "saveJson.h"
#include "scatterplot.h"
#include "spectrallibexport.h"
#include "spectrawidget.h"
#include "tabledockwidget.h";

QMap<int, QString> TableDockWidget::_idTitleMap;

const QMap<PeakGroup::ClassifiedLabel, QString>
TableDockWidget::labelsForLegend()
{
    static QMap<PeakGroup::ClassifiedLabel, QString> labels = {
        {PeakGroup::ClassifiedLabel::CorrelationAndPattern,
         "Signal with correlation and cohort-variance"},
        {PeakGroup::ClassifiedLabel::Correlation,
         "Signal with only correlation"},
        {PeakGroup::ClassifiedLabel::Pattern,
         "Signal with only cohort-variance"},
        {PeakGroup::ClassifiedLabel::Signal, "Signal"},
        {PeakGroup::ClassifiedLabel::Noise, "Noise"},
        {PeakGroup::ClassifiedLabel::MaybeGood, "May be good signal"}};
    return labels;
}

const QMap<PeakGroup::ClassifiedLabel, QIcon> TableDockWidget::iconsForLegend()
{
    static QMap<PeakGroup::ClassifiedLabel, QIcon> icons{
        {PeakGroup::ClassifiedLabel::CorrelationAndPattern,
         QIcon(":/images/moi_pattern_correlated.png")},
        {PeakGroup::ClassifiedLabel::Correlation,
         QIcon(":/images/moi_correlated.png")},
        {PeakGroup::ClassifiedLabel::Pattern,
         QIcon(":/images/moi_pattern.png")},
        {PeakGroup::ClassifiedLabel::Signal, QIcon(":/images/good.png")},
        {PeakGroup::ClassifiedLabel::Noise, QIcon(":/images/bad.png")},
        {PeakGroup::ClassifiedLabel::MaybeGood,
         QIcon(":/images/maybeGood.png")}};
    return icons;
}

TableDockWidget::TableDockWidget(MainWindow* mw)
{
    QDateTime current_time;
    const QString format = "dd-MM-yyyy_hh_mm_ss";
    QString datetimestamp = current_time.currentDateTime().toString(format);
    datetimestamp.replace(" ", "_");
    datetimestamp.replace(":", "-");

    uploadId = datetimestamp + "_Peak_table_" + QString::number(lastTableId());

    writableTempS3Dir =
        QStandardPaths::writableLocation(
            QStandardPaths::QStandardPaths::GenericConfigLocation)
        + QDir::separator() + "tmp_Elmaven_s3_files_"
        + QString::number(lastTableId());
    if (!QDir(writableTempS3Dir).exists())
        QDir().mkdir(writableTempS3Dir);

    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFloating(false);
    _mainwindow = mw;
    _labeledGroups = 0;
    _targetedGroups = 0;
    _nextGroupId = 1;
    _legend = nullptr;
    pal = palette();
    setAutoFillBackground(true);
    pal.setColor(QPalette::Background, QColor(170, 170, 170, 100));
    setPalette(pal);
    setDefaultStyle();

    viewType = groupView;

    treeWidget = new PeakGroupTreeWidget(this);
    relabelDialog = new RelabelGroupsDialog(this);

    treeWidget->setSortingEnabled(false);
    treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeWidget->setObjectName("PeakGroupTable");
    treeWidget->setFocusPolicy(Qt::NoFocus);
    treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setFocusPolicy(Qt::ClickFocus);
    tableSelectionFlagUp = false;
    tableSelectionFlagDown = false;

    if (_mainwindow->mavenParameters->classifyUsingPeakMl)
        hasClassifiedGroups = true;
    else
        hasClassifiedGroups = false;

    badGroupLimit = _mainwindow->mavenParameters->badGroupUpperLimit;
    maybeGoodGroupLimit = _mainwindow->mavenParameters->goodGroupLowerLimit;

    setWidget(treeWidget);
    setupPeakTable();

    ctrlZ = new QShortcut(this);          // Initialize the object
    ctrlZ->setKey(Qt::CTRL + Qt::Key_Z);  // Set the key code
    connect(ctrlZ, SIGNAL(activated()), this, SLOT(undoLabel()));

    connect(treeWidget,
            &QTreeWidget::itemClicked,
            this,
            &TableDockWidget::showSelectedGroup);
    connect(treeWidget,
            &QTreeWidget::itemSelectionChanged,
            this,
            &TableDockWidget::_refreshCycleBuffer);

    connect(
        treeWidget, SIGNAL(itemSelectionChanged()), SLOT(showSelectedGroup()));

    connect(treeWidget,
            &QTreeWidget::itemExpanded,
            this,
            &TableDockWidget::sortChildrenAscending);

    clusterDialog = new ClusterDialog(this);
    connect(clusterDialog->clusterButton,
            SIGNAL(clicked(bool)),
            SLOT(clusterGroups()));
    connect(clusterDialog->clearButton,
            SIGNAL(clicked(bool)),
            SLOT(clearClusters()));

    connect(this,
            SIGNAL(updateProgressBar(QString, int, int, bool)),
            _mainwindow,
            SLOT(setProgressBar(QString, int, int, bool)));
    connect(
        this, SIGNAL(UploadPeakBatch()), this, SLOT(UploadPeakBatchToCloud()));
    connect(this, SIGNAL(renderedPdf()), this, SLOT(pdfReadyNotification()));

    // selections in correlation table will trigger selection in this table
    connect(mw->getCorrelationTable(),
            SIGNAL(groupNameSelected(string)),
            this,
            SLOT(displayNextGroupInCorrelationTable(string)));

    connect(this,
            SIGNAL(updateRelabelStatusBar(QString, int, int)),
            relabelDialog,
            SLOT(setProgressBar(QString, int, int)));

    setAcceptDrops(true);
    _cycleInProgress = false;
}
int TableDockWidget::noiseCount = 0;
int TableDockWidget::signalCount = 0;
int TableDockWidget::correlatedCount = 0;
int TableDockWidget::cohortCount = 0;
int TableDockWidget::correlatedCohortCount = 0;
int TableDockWidget::mayBeGroupsCount = 0;

TableDockWidget::~TableDockWidget()
{
    if (clusterDialog != NULL)
        delete clusterDialog;
    if (ctrlZ)
        delete ctrlZ;
}

void TableDockWidget::sortChildrenAscending(QTreeWidgetItem* item)
{
    item->sortChildren(1, Qt::AscendingOrder);
}

void TableDockWidget::showClusterDialog()
{
    clusterDialog->show();
}

void TableDockWidget::updateTableAfterAlignment()
{
    BackgroundOpsThread::updateGroups(_topLevelGroups, _mainwindow->samples);
    showAllGroups();
}

void TableDockWidget::setIntensityColName()
{
    QTreeWidgetItem* header = treeWidget->headerItem();
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
    header->setText(10, temp);
}

void TableDockWidget::undoLabel()
{
    if (!hasClassifiedGroups)
        return;

    auto groups = getSelectedGroups();
    for (auto group : groups) {
        auto id = undoBuffer[group->groupId()];
        group->setPredictedLabel(group->labelForString(id.first), id.second);
        saveLabelForUndo(group.get());

        auto item = itemForGroup(group.get());
        updateItem(item);
        updateLegend();
        updateStatus();
    }
}

void TableDockWidget::displayNextGroupInCorrelationTable(string groupName)
{
    if (_mainwindow->getCorrelationTable()->currentTable() == this) {
        auto wasBlocked = treeWidget->blockSignals(true);
        for (auto item : _cycleBuffer) {
            if (item->text(2).toStdString() == groupName) {
                treeWidget->setCurrentItem(item);
                treeWidget->scrollTo(treeWidget->currentIndex(),
                                     QAbstractItemView::PositionAtCenter);
                showSelectedGroup();
                break;
            }
        }
        treeWidget->blockSignals(wasBlocked);
    }
}

void TableDockWidget::setupPeakTable()
{
    QStringList colNames;

    // Add common coulmns to the Table
    if (hasClassifiedGroups) {
        colNames << "Label";
    }
    colNames << "#";
    colNames << "ID";
    colNames << "Observed m/z";
    colNames << "Expected m/z";
    colNames << "RT";

    if (viewType == groupView) {
        // Add group view columns to the peak table
        colNames << "Δ RT";
        colNames << "# peaks";
        colNames << "# good peaks";
        colNames << "Max width";
        colNames << "Max AreaTop";
        colNames << "Max S/N";
        colNames << "Max quality";
        colNames << "MS2 score";
        colNames << "#MS2 events";
        if (hasClassifiedGroups)
            colNames << "Classification Probability";
        colNames << "Rank";
    } else if (viewType == peakView) {
        vector<mzSample*> vsamples = _mainwindow->getVisibleSamples();
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

shared_ptr<PeakGroup> TableDockWidget::groupForItem(QTreeWidgetItem* item)
{
    if (item == nullptr)
        return nullptr;

    int index = 0;
    if (hasClassifiedGroups)
        index = 1;

    auto var = item->data(index, Qt::UserRole);

    auto rowData = var.value<RowData>();
    auto group = _topLevelGroups.at(rowData.parentIndex);
    if (rowData.childType == RowData::ChildType::Isotope) {
        group = group->childIsotopes().at(rowData.childIndex);
    } else if (rowData.childType == RowData::ChildType::Adduct) {
        group = group->childAdducts().at(rowData.childIndex);
    }
    return group;
}

QTreeWidgetItem* TableDockWidget::itemForGroup(PeakGroup* group)
{
    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        auto grp = groupForItem(*it).get();
        if (grp == group)
            return *it;
        ++it;
    }
    return nullptr;
}

void TableDockWidget::refreshParentItem(QTreeWidgetItem* item)
{
    if (item == nullptr)
        return;

    if (item->parent() != nullptr)
        item = item->parent();

    foreach (auto i, item->takeChildren())
        delete i;

    auto var = item->data(0, Qt::UserRole);
    auto parentRowData = var.value<RowData>();
    auto parent = groupForItem(item);
    if (parent->childIsotopeCount() > 0) {
        for (size_t i = 0; i < parent->childIsotopeCount(); ++i) {
            RowData rowData = _rowDataForThisTable(
                parentRowData.parentIndex, RowData::ChildType::Isotope, i);
            addRow(rowData, item);
        }
    }
    if (parent->childAdductsCount() > 0) {
        for (size_t i = 0; i < parent->childAdductsCount(); ++i) {
            RowData rowData = _rowDataForThisTable(
                parentRowData.parentIndex, RowData::ChildType::Adduct, i);
            addRow(rowData, item);
        }
    }
}

void TableDockWidget::updateLegend()
{
    noiseCount = 0;
    signalCount = 0;
    correlatedCount = 0;
    cohortCount = 0;
    correlatedCohortCount = 0;
    mayBeGroupsCount = 0;
    _legend->clear();

    auto incrementClassification = [&](PeakGroup* group) {
        if (group->labelToString(group->predictedLabel())
                == "ClassifiedLabel::Noise"
            || group->userLabel() == 'b')
            noiseCount++;
        if (group->labelToString(group->predictedLabel())
                == "ClassifiedLabel::Signal"
            || group->userLabel() == 'g')
            signalCount++;
        if (group->labelToString(group->predictedLabel())
            == "ClassifiedLabel::Correlation")
            correlatedCount++;
        if (group->labelToString(group->predictedLabel())
            == "ClassifiedLabel::Pattern")
            cohortCount++;
        if (group->labelToString(group->predictedLabel())
            == "ClassifiedLabel::CorrelationAndPattern")
            correlatedCohortCount++;
        if (group->labelToString(group->predictedLabel())
            == "ClassifiedLabel::MaybeGood")
            mayBeGroupsCount++;
    };
    for (auto group : _topLevelGroups) {
        incrementClassification(group.get());
        for (auto childIsotope : group->childIsotopes())
            incrementClassification(childIsotope.get());
    }

    _legend->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    auto labelsForLegend = TableDockWidget::labelsForLegend();
    auto iconsForLegend = TableDockWidget::iconsForLegend();

    int totalCount = 0;
    totalCount += noiseCount;
    totalCount += signalCount;
    totalCount += correlatedCount;
    totalCount += cohortCount;
    totalCount += correlatedCohortCount;
    totalCount += mayBeGroupsCount;

    int count = 0;
    vector<float> countPercentage;

    if (!totalCount) {
        for (int i = 0; i < 6; i++) {
            countPercentage.push_back(0.0f);
        }
    } else {
        countPercentage.push_back((noiseCount / static_cast<float>(totalCount))
                                  * 100);
        countPercentage.push_back((signalCount / static_cast<float>(totalCount))
                                  * 100);
        countPercentage.push_back(
            (correlatedCount / static_cast<float>(totalCount)) * 100);
        countPercentage.push_back((cohortCount / static_cast<float>(totalCount))
                                  * 100);
        countPercentage.push_back(
            (correlatedCohortCount / static_cast<float>(totalCount)) * 100);
        countPercentage.push_back(
            (mayBeGroupsCount / static_cast<float>(totalCount)) * 100);
    }

    for (const auto& label : labelsForLegend) {
        auto type = labelsForLegend.key(label);
        auto icon = iconsForLegend.value(type);

        string labelString = label.toStdString();
        labelString += " (";
        labelString += mzUtils::float2string(countPercentage[count++], 4);
        labelString += "%)";

        auto labelConverted = QString::fromStdString(labelString);
        _legend->addItem(icon, labelConverted);
    }
    _legend->addItem(QIcon(":/images/relabel.png"), "Re-Label");
}

void TableDockWidget::updateTable()
{
    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        updateItem(*it);
        ++it;
    }
    updateStatus();
    if (hasClassifiedGroups) {
        updateLegend();
        _legend->selectAll();
    }
}

void TableDockWidget::_paintClassificationDisagreement(QTreeWidgetItem* item)
{
    auto group = groupForItem(item);
    int numGood = 0;
    int numBad = 0;
    int total = group->peakCount();
    for (int i = 0; i < group->peakCount(); i++) {
        group->peaks[i].quality > _mainwindow->mavenParameters->minQuality
            ? numGood++
            : numBad++;
    }

    float incorrectFraction = 0.0f;
    if (numGood > 0 && group->userLabel() == 'b') {
        incorrectFraction = static_cast<float>(numGood) / total;
    } else if (numBad > 0 && group->userLabel() == 'g') {
        incorrectFraction = static_cast<float>(numBad) / total;
    }
    QLinearGradient gradient(0, 6, 42, 6);
    gradient.setColorAt(0, QColor::fromRgbF(1.0, 1.0, 1.0, 0.0));
    gradient.setColorAt(1, QColor::fromRgbF(1.0, 0.0, 0.13, incorrectFraction));
    QBrush brush(gradient);
    item->setBackground(0, brush);
}

void TableDockWidget::updateItem(QTreeWidgetItem* item, bool updateChildren)
{
    shared_ptr<PeakGroup> group = groupForItem(item);
    if (group == nullptr)
        return;

    for (int i = 0; i < treeWidget->columnCount(); ++i)
        item->setTextAlignment(i, Qt::AlignRight);

    if (hasClassifiedGroups)
        item->setTextAlignment(
            2, Qt::AlignLeft);  // compound name for peakML tables.
    else
        item->setTextAlignment(
            1, Qt::AlignLeft);  // compound name for non-peakML tables.

    if (group->isGhost()) {
        if (hasClassifiedGroups) {
            item->setText(1, QString::number(group->groupId()));
            item->setText(2, QString(group->getName().c_str()));
        } else {
            item->setText(0, QString::number(group->groupId()));
            item->setText(1, QString(group->getName().c_str()));
        }

        if (updateChildren) {
            for (int i = 0; i < item->childCount(); ++i)
                updateItem(item->child(i));
        }
        return;
    }

    int index = 0;
    if (hasClassifiedGroups)
        index = 1;
    // Find maximum number of peaks
    if (maxPeaks < group->peakCount())
        maxPeaks = group->peakCount();

    // Updating the peakid
    item->setText(index++, QString::number(group->groupId()));
    item->setText(index++, QString(group->getName().c_str()));
    item->setText(index++, QString::number(group->meanMz, 'f', 4));

    int charge = group->parameters()->getCharge(group->getCompound());
    if (group->getExpectedMz(charge) != -1) {
        float mz = group->getExpectedMz(charge);
        item->setText(index++, QString::number(mz, 'f', 4));
    } else {
        item->setText(index++, "NA");
    }

    item->setText(index++, QString::number(group->meanRt, 'f', 2));

    if (viewType == groupView) {
        auto expectedRtDiff = group->expectedRtDiff();
        if (expectedRtDiff == -1.0f) {
            item->setText(index++, "NA");
        } else {
            item->setText(index++, QString::number(expectedRtDiff, 'f', 2));
        }
        item->setText(
            index++,
            QString::number(group->sampleCount + group->blankSampleCount));
        item->setText(index++, QString::number(group->goodPeakCount));
        item->setText(index++, QString::number(group->maxNoNoiseObs));
        item->setText(
            index++, QString::number(extractMaxIntensity(group.get()), 'g', 3));
        item->setText(index++,
                      QString::number(group->maxSignalBaselineRatio, 'f', 0));
        item->setText(index++, QString::number(group->maxQuality, 'f', 2));
        item->setText(
            index++,
            QString::number(group->fragMatchScore.mergedScore, 'f', 2));
        item->setText(index++, QString::number(group->ms2EventCount));
        item->setText(index++,
                      QString::number(group->predictionProbability(), 'f', 2));
        item->setText(index++, QString::number(group->groupRank, 'e', 6));

        if (fabs(group->changeFoldRatio) >= 0) {
            item->setText(index++,
                          QString::number(group->changeFoldRatio, 'f', 3));
            item->setText(index++,
                          QString::number(group->changePValue, 'f', 6));
        }
    } else if (viewType == peakView) {
        vector<mzSample*> vsamples = _mainwindow->getVisibleSamples();
        sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
        vector<float> yvalues = group->getOrderedIntensityVector(
            vsamples, _mainwindow->getUserQuantType());
        for (unsigned int i = 0; i < yvalues.size(); i++) {
            if (hasClassifiedGroups)
                item->setText(6 + i, QString::number(yvalues[i], 'f', 2));
            else
                item->setText(5 + i, QString::number(yvalues[i], 'f', 2));
        }
        heatmapBackground(item);
    }

    if (hasClassifiedGroups) {
        item->setIcon(0, iconsForLegend()[group->predictedLabel()]);
        QString castLabel = QString::fromStdString(
            PeakGroup::labelToString(group->predictedLabel()));
        item->setData(0, Qt::UserRole, QVariant::fromValue(castLabel));

        if (group->predictedLabel()
            == PeakGroup::ClassifiedLabel::Correlation) {
            QString castLabel = "PeakGroup::ClassifiedLabel::Correlation";
            item->setData(0, Qt::UserRole, QVariant::fromValue(castLabel));
        } else if (group->predictedLabel()
                   == PeakGroup::ClassifiedLabel::Pattern) {
            QString castLabel = "PeakGroup::ClassifiedLabel::Pattern";
            item->setData(0, Qt::UserRole, QVariant::fromValue(castLabel));
        } else if (group->predictedLabel()
                   == PeakGroup::ClassifiedLabel::CorrelationAndPattern) {
            QString castLabel =
                "PeakGroup::ClassifiedLabel::CorrelationAndPattern";
            item->setData(0, Qt::UserRole, QVariant::fromValue(castLabel));
        } else if (group->predictedLabel()
                   == PeakGroup::ClassifiedLabel::Signal) {
            // we have to store stringified classifier labels because QVariant
            // has issues with standard enum classes
            QString castLabel = "PeakGroup::ClassifiedLabel::Signal";
            item->setData(0, Qt::UserRole, QVariant::fromValue(castLabel));
        } else if (group->predictedLabel()
                   == PeakGroup::ClassifiedLabel::Noise) {
            QString castLabel = "PeakGroup::ClassifiedLabel::Noise";
            item->setData(0, Qt::UserRole, QVariant::fromValue(castLabel));
        } else if (group->predictedLabel()
                   == PeakGroup::ClassifiedLabel::MaybeGood) {
            QString castLabel = "PeakGroup::ClassifiedLabel::MaybeGood";
            item->setData(0, Qt::UserRole, QVariant::fromValue(castLabel));
        } else {
            QString castLabel = "PeakGroup::ClassifiedLabel::None";
            item->setData(0, Qt::UserRole, QVariant::fromValue(castLabel));
        }
    } else {
        if (group->userLabel() == 'g') {
            item->setIcon(0, QIcon(":/images/good.png"));
        } else if (group->userLabel() == 'b') {
            item->setIcon(0, QIcon(":/images/bad.png"));
        }
    }
    if (!hasClassifiedGroups)
        _paintClassificationDisagreement(item);

    if (updateChildren) {
        for (int i = 0; i < item->childCount(); ++i)
            updateItem(item->child(i));
    }
}

void TableDockWidget::updateCompoundWidget()
{
    _mainwindow->ligandWidget->resetColor();
    QMap<Compound*, bool> parentCompounds;
    QMap<Compound*, set<Isotope>> compoundIsotopeForms;
    QMap<Compound*, set<Adduct*>> compoundAdductForms;
    for (auto& group : _topLevelGroups) {
        if (group == nullptr || !group->hasCompoundLink())
            continue;

        Compound* compound = group->getCompound();
        if (parentCompounds.contains(compound)) {
            parentCompounds[compound] =
                parentCompounds[compound] || !group->isGhost();
        } else {
            parentCompounds[compound] = !group->isGhost();
        }

        if (!compoundIsotopeForms.contains(compound))
            compoundIsotopeForms.insert(compound, {});
        for (auto& child : group->childIsotopes())
            compoundIsotopeForms[compound].insert(child->isotope());

        if (!compoundAdductForms.contains(compound))
            compoundAdductForms.insert(compound, {});
        for (auto& child : group->childAdducts())
            compoundAdductForms[compound].insert(child->adduct());
    }

    for (auto compound : parentCompounds.keys())
        _mainwindow->ligandWidget->markAsDone(compound,
                                              !parentCompounds[compound]);

    for (auto compound : compoundIsotopeForms.keys()) {
        auto isotopes = compoundIsotopeForms[compound];
        for (auto& isotope : isotopes)
            _mainwindow->ligandWidget->markAsDone(compound, isotope);
    }
    for (auto compound : compoundAdductForms.keys()) {
        auto adducts = compoundAdductForms[compound];
        for (auto& adduct : adducts)
            _mainwindow->ligandWidget->markAsDone(compound, adduct);
    }
}

void TableDockWidget::heatmapBackground(QTreeWidgetItem* item)
{
    if (viewType != peakView)
        return;

    int firstColumn = 5;
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
                values[i] = abs((max - values[i]) / max);  // Z-score
        }

        QColor color = 0xfe7400;
        for (int i = 0; i < values.size(); i++) {
            float value = values[i];
            float prob = value;
            if (prob < 0)
                prob = 0;
            color.setAlpha((1.0f - prob) * 210);

            item->setBackgroundColor(firstColumn + i, color);
        }
    }
}

RowData TableDockWidget::_rowDataForThisTable(size_t parentIndex,
                                              RowData::ChildType childType,
                                              size_t childIndex)
{
    RowData rowData;
    rowData.tableId = tableId;
    rowData.parentIndex = parentIndex;
    rowData.childType = childType;
    rowData.childIndex = childIndex;
    return rowData;
}

void TableDockWidget::addRow(RowData& indexData, QTreeWidgetItem* root)
{
    shared_ptr<PeakGroup> group = _topLevelGroups.at(indexData.parentIndex);
    if (root != nullptr) {
        if (indexData.childType == RowData::ChildType::Isotope) {
            group = group->childIsotopes().at(indexData.childIndex);
        } else if (indexData.childType == RowData::ChildType::Adduct) {
            group = group->childAdducts().at(indexData.childIndex);
        }
    }

    if (group == nullptr)
        return;

    NumericTreeWidgetItem* item = new NumericTreeWidgetItem(root, 0);
    if (root == nullptr) {
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled
                       | Qt::ItemIsDropEnabled);
    } else {
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled
                       | Qt::ItemIsDragEnabled);
    }

    if (hasClassifiedGroups)
        item->setData(1, Qt::UserRole, QVariant::fromValue(indexData));
    else
        item->setData(0, Qt::UserRole, QVariant::fromValue(indexData));

    if (root == nullptr)
        treeWidget->addTopLevelItem(item);

    updateItem(item, false);

    if (group->childIsotopeCount() > 0) {
        for (size_t i = 0; i < group->childIsotopeCount(); ++i) {
            RowData rowData = _rowDataForThisTable(
                indexData.parentIndex, RowData::ChildType::Isotope, i);
            addRow(rowData, item);
        }
    }
    if (group->childAdductsCount() > 0) {
        for (size_t i = 0; i < group->childAdductsCount(); ++i) {
            RowData rowData = _rowDataForThisTable(
                indexData.parentIndex, RowData::ChildType::Adduct, i);
            addRow(rowData, item);
        }
    }
}

void ListView::keyPressEvent(QKeyEvent* event)
{
    if (event->matches(QKeySequence::Copy)) {
        // set all selected compound name to clipboard
        QApplication::clipboard()->setText(strings.join("\n"));
    }
}

PeakGroup createGhostParent(Compound* compound, MavenParameters* mp)
{
    PeakGroup parentGroup(make_shared<MavenParameters>(*mp),
                          PeakGroup::IntegrationType::Ghost);

    // set an appropriate slice for ghost parent
    mzSlice slice;
    slice.compound = compound;
    slice.calculateMzMinMax(mp->compoundMassCutoffWindow, mp->getCharge());
    slice.calculateRTMinMax(false, 0.0f);
    parentGroup.setSlice(slice);

    return parentGroup;
}

shared_ptr<PeakGroup> TableDockWidget::addPeakGroup(PeakGroup* group)
{
    if (group == nullptr)
        return nullptr;

    auto label = group->labelToString(group->predictedLabel());
    auto probability = group->predictionProbability();
    pair<string, float> groupLabel = make_pair(label, probability);
    undoBuffer[group->groupId()] = groupLabel;

    auto newTopLevelGroup = [this](PeakGroup* topLevelGroup) {
        shared_ptr<PeakGroup> sharedGroup =
            make_shared<PeakGroup>(*topLevelGroup);
        _topLevelGroups.push_back(sharedGroup);
        if (sharedGroup->childIsotopeCount() > 0
            || sharedGroup->tagString == C12_PARENT_LABEL) {
            _labeledGroups++;
        }
        if (sharedGroup->hasCompoundLink())
            _targetedGroups++;

        int parentGroupId = _nextGroupId++;
        for (auto& child : _topLevelGroups.back()->childIsotopes())
            child->setGroupId(_nextGroupId++);
        for (auto& child : _topLevelGroups.back()->childAdducts())
            child->setGroupId(_nextGroupId++);
        _topLevelGroups.back()->setGroupId(parentGroupId);
        _topLevelGroups.back()->setTableName(
            titlePeakTable->text().toStdString());
        return _topLevelGroups.back();
    };

    shared_ptr<PeakGroup> insertedGroup = nullptr;
    if (group->isIsotope() || group->isAdduct()) {
        shared_ptr<PeakGroup> topLevelParent = nullptr;
        for (auto& parentGroup : _topLevelGroups) {
            if (parentGroup->isGhost()
                && parentGroup->getCompound() == group->getCompound()) {
                topLevelParent = parentGroup;
                if (group->isIsotope()) {
                    insertedGroup = parentGroup->addIsotopeChild(*group);
                } else if (group->isAdduct()) {
                    insertedGroup = parentGroup->addAdductChild(*group);
                }
            }
        }

        if (insertedGroup == nullptr || topLevelParent == nullptr) {
            PeakGroup parentGroup = createGhostParent(
                group->getCompound(), group->parameters().get());
            topLevelParent = newTopLevelGroup(&parentGroup);
            if (topLevelParent != nullptr && group->isIsotope()) {
                insertedGroup = topLevelParent->addIsotopeChild(*group);
            } else if (topLevelParent != nullptr && group->isAdduct()) {
                insertedGroup = topLevelParent->addAdductChild(*group);
            }
        }
        insertedGroup->setTableName(this->titlePeakTable->text().toStdString());
        insertedGroup->setGroupId(_nextGroupId++);

        // NOTE: top-level group's ID must be set after setting the IDs of its
        // children, such that each child's `metaGroupId` is overwritten
        topLevelParent->setGroupId(topLevelParent->groupId());
    } else {
        insertedGroup = newTopLevelGroup(group);
    }

    return insertedGroup;
}

QList<shared_ptr<PeakGroup>> TableDockWidget::getGroups()
{
    return _topLevelGroups;
}

bool TableDockWidget::deleteAll(bool askConfirmation)
{
    if (!topLevelGroupCount())
        return false;

    auto continueDeletion = true;
    if (askConfirmation)
        continueDeletion = deleteAllgroupsWarning();
    if (!continueDeletion)
        return false;

    if (treeWidget->currentItem()) {
        _mainwindow->getEicWidget()->unSetPeakTableGroup(
            groupForItem(treeWidget->currentItem()));
    }

    disconnect(treeWidget,
               &QTreeWidget::itemSelectionChanged,
               this,
               &TableDockWidget::showSelectedGroup);
    treeWidget->clear();
    _topLevelGroups.clear();
    connect(treeWidget,
            &QTreeWidget::itemSelectionChanged,
            this,
            &TableDockWidget::showSelectedGroup);

    _mainwindow->getEicWidget()->replotForced();

    this->hide();
    return true;
}

void TableDockWidget::noPeakFound()
{
    QMessageBox* warning = new QMessageBox(this);
    auto htmlText = QString(
        "No peaks were detected. Either the search was "
        "prematurely cancelled or the settings were too "
        "strict for this data. To get a better idea of what "
        "settings to use, try manually browsing over your "
        "samples using a compound database.");

    warning->setText(htmlText);
    warning->setIcon(QMessageBox::Icon::Information);
    warning->exec();

    QCoreApplication::processEvents();
}

void TableDockWidget::showAllGroups()
{
    treeWidget->clear();

    setFocus();
    treeWidget->setSortingEnabled(false);

    setupPeakTable();
    if (hasClassifiedGroups) {
        updateLegend();
        _legend->selectAll();
    }

    if (viewType == groupView)
        setIntensityColName();

    QMap<int, QTreeWidgetItem*> parents;
    for (size_t i = 0; i < _topLevelGroups.size(); ++i) {
        auto group = _topLevelGroups[i];

        // Setting curation parameters;
        if (hasClassifiedGroups) {
            group->isClassified = true;
            for (auto childIsotope : group->childIsotopes()) {
                childIsotope->isClassified = true;
            }
            for (auto childAdducts : group->childAdducts()) {
                childAdducts->isClassified = true;
            }
        }

        RowData rowData = _rowDataForThisTable(i);

        int clusterId = group->clusterId;
        if (clusterId && group->meanMz > 0 && group->peakCount() > 0) {
            if (!parents.contains(clusterId)) {
                parents[clusterId] = new QTreeWidgetItem(treeWidget);
                if (hasClassifiedGroups) {
                    parents[clusterId]->setText(
                        0, QString("Cluster ") + QString::number(clusterId));
                    parents[clusterId]->setText(
                        6, QString::number(allgroups[i].meanRt, 'f', 2));
                    parents[clusterId]->setExpanded(true);
                } else {
                    parents[clusterId]->setText(
                        0, QString("Cluster ") + QString::number(clusterId));
                    parents[clusterId]->setText(
                        5, QString::number(allgroups[i].meanRt, 'f', 2));
                    parents[clusterId]->setExpanded(true);
                }
            }
            QTreeWidgetItem* parent = parents[clusterId];
            addRow(rowData, parent);
        } else {
            addRow(rowData, nullptr);
        }
    }
    QScrollBar* vScroll = treeWidget->verticalScrollBar();
    if (vScroll) {
        vScroll->setSliderPosition(vScroll->maximum());
    }
    // sort by group id.
    if (hasClassifiedGroups)
        treeWidget->sortByColumn(1, Qt::AscendingOrder);
    else
        treeWidget->sortByColumn(0, Qt::AscendingOrder);

    treeWidget->setSortingEnabled(true);
    treeWidget->header()->setStretchLastSection(false);
    updateStatus();
    updateCompoundWidget();

    if (hasClassifiedGroups) {
        treeWidget->setColumnWidth(2, 250);  // Compound name for peakML tables.
        treeWidget->header()->setSectionResizeMode(2, QHeaderView::Interactive);
    } else {
        treeWidget->header()->setSectionResizeMode(1, QHeaderView::Interactive);
        treeWidget->setColumnWidth(
            1, 250);  // compound name for non-peakML tables.
    }
}

QMap<TableDockWidget::PeakTableSubsetType, int>
TableDockWidget::countBySubsets()
{
    auto itemsBySubset = _peakTableGroupedBySubsets();
    QMap<PeakTableSubsetType, int> numItemsPerSubset;

    auto insertCountForSubset = [&](PeakTableSubsetType subset) {
        numItemsPerSubset.insert(subset, itemsBySubset.value(subset).size());
    };

    insertCountForSubset(PeakTableSubsetType::Selected);
    insertCountForSubset(PeakTableSubsetType::Whole);
    insertCountForSubset(PeakTableSubsetType::Good);
    insertCountForSubset(PeakTableSubsetType::Bad);
    insertCountForSubset(PeakTableSubsetType::ExcludeBad);
    insertCountForSubset(PeakTableSubsetType::Unmarked);
    insertCountForSubset(PeakTableSubsetType::Correlated);
    insertCountForSubset(PeakTableSubsetType::Variance);
    insertCountForSubset(PeakTableSubsetType::CorrelatedVariance);
    insertCountForSubset(PeakTableSubsetType::MaybeGood);

    return numItemsPerSubset;
}

void TableDockWidget::showOnlySubsets(QList<PeakTableSubsetType> visibleSubsets)
{
    auto itemsBySubset = _peakTableGroupedBySubsets();

    // first, we hide all items from subsets that are not in the inclusion list
    QMapIterator<PeakTableSubsetType, QList<QTreeWidgetItem*>> hideItr(
        itemsBySubset);
    while (hideItr.hasNext()) {
        hideItr.next();
        auto subset = hideItr.key();
        auto& itemsForSubset = hideItr.value();
        if (!visibleSubsets.contains(subset)) {
            for (auto item : itemsForSubset) {
                item->setHidden(true);
                auto group = groupForItem(item);
                group->setIsGrooupHidden(true);
            }
        }
    }

    // next, we separately show visible subset because some of the items that
    // were hidden in the last iteration might have to be shown again
    QMapIterator<PeakTableSubsetType, QList<QTreeWidgetItem*>> showItr(
        itemsBySubset);
    while (showItr.hasNext()) {
        showItr.next();
        auto subset = showItr.key();
        auto& itemsForSubset = showItr.value();
        if (visibleSubsets.contains(subset)) {
            for (auto item : itemsForSubset) {
                item->setHidden(false);
                auto group = groupForItem(item);
                group->setIsGrooupHidden(false);
            }
        }
    }
}

void TableDockWidget::filterForSelectedLabels()
{
    QList<PeakTableSubsetType> selectedSubsets;
    auto selectedLabels = _legend->selectedTexts();

    if (selectedLabels.contains("Re-Label")) {
        showRelabelWidget();
        return;
    }

    for (auto& label : selectedLabels) {
        auto removedPercentage = mzUtils::split(label.toStdString(), " (");
        auto originalLabel = QString::fromStdString(removedPercentage[0]);
        auto predictionLabel = labelsForLegend().key(originalLabel);
        if (predictionLabel == PeakGroup::ClassifiedLabel::Noise)
            selectedSubsets.append(PeakTableSubsetType::Bad);
        if (predictionLabel == PeakGroup::ClassifiedLabel::Signal)
            selectedSubsets.append(PeakTableSubsetType::Good);
        if (predictionLabel == PeakGroup::ClassifiedLabel::Correlation)
            selectedSubsets.append(PeakTableSubsetType::Correlated);
        if (predictionLabel == PeakGroup::ClassifiedLabel::Pattern)
            selectedSubsets.append(PeakTableSubsetType::Variance);
        if (predictionLabel
            == PeakGroup::ClassifiedLabel::CorrelationAndPattern)
            selectedSubsets.append(PeakTableSubsetType::CorrelatedVariance);
        if (predictionLabel == PeakGroup::ClassifiedLabel::MaybeGood)
            selectedSubsets.append(PeakTableSubsetType::MaybeGood);
    }

    selectedSubsets.append(PeakTableSubsetType::Unmarked);
    showOnlySubsets(selectedSubsets);
}

float TableDockWidget::extractMaxIntensity(PeakGroup* group)
{
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

void TableDockWidget::exportGroupsToSpreadsheet()
{
    vector<mzSample*> samples = _mainwindow->getSamples();

    if (topLevelGroupCount() == 0) {
        QString msg = "Peaks Table is Empty";
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();

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
        this,
        tr("Export Groups"),
        dir,
        groupsCSV + ";;" + groupsSCSV + ";;" + groupsTAB + ";;" + groupsSTAB
            + ";;" + peaksCSV + ";;" + peaksTAB + ";;" + peaksListQE + ";;"
            + mascotMGF,
        &sFilterSel);

    if (fileName.isEmpty())
        return;

    if (sFilterSel == groupsSCSV || sFilterSel == peaksCSV
        || sFilterSel == groupsCSV) {
        if (!fileName.endsWith(".csv", Qt::CaseInsensitive))
            fileName = fileName + ".csv";
    }

    if (sFilterSel == groupsSTAB || sFilterSel == peaksTAB
        || sFilterSel == groupsTAB) {
        if (!fileName.endsWith(".tab", Qt::CaseInsensitive))
            fileName = fileName + ".tab";
    }

    if (samples.size() == 0)
        return;

    if (sFilterSel == groupsSCSV || sFilterSel == groupsSTAB
        || sFilterSel == groupsCSV || sFilterSel == groupsTAB)
        _mainwindow->getAnalytics()->hitEvent("Exports", "CSV", "Groups");
    if (sFilterSel == peaksCSV || sFilterSel == peaksTAB)
        _mainwindow->getAnalytics()->hitEvent("Exports", "CSV", "Peaks");

    if (sFilterSel == peaksListQE) {
        _mainwindow->getAnalytics()->hitEvent("Exports", "CSV", "Peaks List");
        writeQEInclusionList(fileName);
        return;
    } else if (sFilterSel == mascotMGF) {
        _mainwindow->getAnalytics()->hitEvent("Exports", "CSV", "Mascot");
        writeMascotGeneric(fileName);
        return;
    }

    auto ms2GroupAt = find_if(
        begin(_topLevelGroups),
        end(_topLevelGroups),
        [](shared_ptr<PeakGroup> group) {
            if (!group->hasCompoundLink())
                return false;
            return (group->getCompound()->type() == Compound::Type::MS2);
        });
    bool ms2GroupExists = ms2GroupAt != end(_topLevelGroups);
    bool includeSetNamesLines = false;

    auto reportType = CSVReports::ReportType::GroupReport;
    if (sFilterSel == groupsSCSV) {
        reportType = CSVReports::ReportType::GroupReport;
        includeSetNamesLines = true;
    } else if (sFilterSel == groupsSTAB) {
        reportType = CSVReports::ReportType::GroupReport;
        includeSetNamesLines = true;
    } else if (sFilterSel == peaksCSV) {
        reportType = CSVReports::ReportType::PeakReport;
    } else if (sFilterSel == peaksTAB) {
        reportType = CSVReports::ReportType::PeakReport;
    } else {
        reportType = CSVReports::ReportType::GroupReport;
    }

    CSVReports csvreports(fileName.toStdString(),
                          reportType,
                          samples,
                          _mainwindow->getUserQuantType(),
                          ms2GroupExists,
                          includeSetNamesLines,
                          _mainwindow->mavenParameters,
                          false,
                          hasClassifiedGroups);

    QList<shared_ptr<PeakGroup>> selectedGroups;

    if (peakTableSelection == PeakTableSubsetType::Displayed)
        selectedGroups = getDisplayedGroups();
    else
        selectedGroups = getSelectedGroups();

    csvreports.setSelectionFlag(static_cast<int>(peakTableSelection));

    for (auto group : _topLevelGroups) {
        if (selectedGroups.contains(group)) {
            csvreports.addGroup(group.get());
        }
    }

    if (csvreports.getErrorReport() != "") {
        QMessageBox msgBox(_mainwindow);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText(csvreports.getErrorReport());
        msgBox.exec();
    }
}

void TableDockWidget::prepareDataForPolly(QString writableTempDir,
                                          QString exportFormat,
                                          QString userFilename)
{
    vector<mzSample*> samples = _mainwindow->getSamples();

    if (topLevelGroupCount() == 0) {
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

    if (sFilterSel == groupsSCSV || sFilterSel == peaksCSV
        || sFilterSel == groupsCSV) {
        if (!fileName.endsWith(".csv", Qt::CaseInsensitive))
            fileName = fileName + ".csv";
    }

    if (sFilterSel == groupsSTAB || sFilterSel == peaksTAB
        || sFilterSel == groupsTAB) {
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

    auto ms2GroupAt = find_if(
        begin(_topLevelGroups),
        end(_topLevelGroups),
        [](shared_ptr<PeakGroup> group) {
            if (!group->hasCompoundLink())
                return false;
            return (group->getCompound()->type() == Compound::Type::MS2);
        });
    bool ms2GroupExists = ms2GroupAt != end(_topLevelGroups);
    bool includeSetNamesLines = false;

    auto reportType = CSVReports::ReportType::GroupReport;
    if (sFilterSel == groupsSCSV) {
        reportType = CSVReports::ReportType::GroupReport;
    } else if (sFilterSel == groupsSTAB) {
        reportType = CSVReports::ReportType::GroupReport;
    } else if (sFilterSel == peaksCSV) {
        reportType = CSVReports::ReportType::PeakReport;
    } else if (sFilterSel == peaksTAB) {
        reportType = CSVReports::ReportType::PeakReport;
    }
    CSVReports csvreports(fileName.toStdString(),
                          reportType,
                          samples,
                          _mainwindow->getUserQuantType(),
                          ms2GroupExists,
                          includeSetNamesLines,
                          _mainwindow->mavenParameters,
                          true);

    QList<shared_ptr<PeakGroup>> selectedGroups = getSelectedGroups();
    csvreports.setSelectionFlag(static_cast<int>(peakTableSelection));

    for (auto group : _topLevelGroups) {
        // we do not set untargeted groups to Polly yet, remove this when we
        // can.
        if (selectedGroups.contains(group) && group->hasCompoundLink()) {
            csvreports.addGroup(group.get());
        }
    }

    if (csvreports.getErrorReport() != "") {
        QMessageBox msgBox(_mainwindow);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText(csvreports.getErrorReport());
        msgBox.exec();
    }
}

void TableDockWidget::exportJsonToPolly(QString writableTempDir,
                                        QString jsonfileName,
                                        bool addMLInfo)
{
    if (topLevelGroupCount() == 0) {
        QString msg = "Peaks Table is Empty";
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    // copy all groups from <_topLevelGroups> to <vallgroups> which is used by
    // <libmaven/jsonReports.cpp>
    vallgroups.clear();
    for (auto group : _topLevelGroups)
        vallgroups.push_back(*(group.get()));

    jsonReports = new JSONReports(_mainwindow->mavenParameters, addMLInfo);
    jsonReports->save(jsonfileName.toStdString(),
                      vallgroups,
                      _mainwindow->getVisibleSamples());
}

UploadPeaksToCloudThread::UploadPeaksToCloudThread(PollyIntegration* iPolly)
{
    _pollyintegration = iPolly;
};

void UploadPeaksToCloudThread::run()
{
    qDebug() << "Checking for active internet connection..";
    QString status;
    ErrorStatus response = _pollyintegration->activeInternet();
    if (response == ErrorStatus::Failure || response == ErrorStatus::Error) {
        qDebug() << "No internet connection..aborting upload";
        return;
    }
    ErrorStatus uploadStatus =
        _pollyintegration->UploadPeaksToCloud(sessionId, fileName, filePath);
    if (uploadStatus == ErrorStatus::Failure
        || uploadStatus == ErrorStatus::Error) {
        qDebug() << "Peaks upload failed...";
        return;
    }
    return;
}

UploadPeaksToCloudThread::~UploadPeaksToCloudThread() {}

void TableDockWidget::UploadPeakBatchToCloud()
{
    jsonReports = new JSONReports(_mainwindow->mavenParameters);
    QString filePath = writableTempS3Dir + QDir::separator() + uploadId + "_"
                       + QString::number(uploadCount) + ".json";
    jsonReports->save(filePath.toStdString(),
                      subsetPeakGroups,
                      _mainwindow->getVisibleSamples());

    PollyIntegration* iPolly = _mainwindow->getController()->iPolly;
    UploadPeaksToCloudThread* uploadPeaksToCloudThread =
        new UploadPeaksToCloudThread(iPolly);
    connect(uploadPeaksToCloudThread,
            &UploadPeaksToCloudThread::finished,
            uploadPeaksToCloudThread,
            &QObject::deleteLater);
    uploadPeaksToCloudThread->sessionId = uploadId;
    uploadPeaksToCloudThread->fileName =
        uploadId + "_" + QString::number(uploadCount);
    uploadPeaksToCloudThread->filePath = filePath;
    uploadPeaksToCloudThread->start();
}

void TableDockWidget::exportJson()
{
    if (topLevelGroupCount() == 0) {
        QString msg = "Peaks Table is Empty";
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    _mainwindow->getAnalytics()->hitEvent("Exports", "JSON");

    // copy all groups from <_topLevelGroups> to <vallgroups> which is used by
    // <libmaven/jsonReports.cpp>
    vallgroups.clear();
    for (auto group : _topLevelGroups)
        vallgroups.push_back(*(group.get()));

    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();
    if (settings->contains("lastDir"))
        dir = settings->value("lastDir").value<QString>();

    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save EICs to Json File"), dir, tr("*.json"));
    if (fileName.isEmpty())
        return;
    if (!fileName.endsWith(".json", Qt::CaseInsensitive))
        fileName = fileName + ".json";

    saveJson* jsonSaveThread = new saveJson();
    jsonSaveThread->setMainwindow(_mainwindow);
    jsonSaveThread->setPeakTable(this);
    jsonSaveThread->setfileName(fileName.toStdString());
    jsonSaveThread->start();
}

void TableDockWidget::exportSpectralLib()
{
    bool ok;
    int limitNumPeaks = QInputDialog::getInt(this,
                                             "",
                                             "Limit number of fragments per "
                                             "compound",
                                             20,
                                             1,
                                             100,
                                             1,
                                             &ok);
    if (!ok)
        return;

    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();
    if (settings->contains("lastDir"))
        dir = settings->value("lastDir").value<QString>();

    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Export table as spectral library"), dir, tr("*.msp"));
    if (fileName.isEmpty())
        return;
    if (!fileName.endsWith(".msp", Qt::CaseInsensitive))
        fileName = fileName + ".msp";
    if (QFile::exists(fileName))
        QFile::remove(fileName);

    SpectralLibExport library(
        fileName.toStdString(), SpectralLibExport::Format::Nist, limitNumPeaks);
    for (auto group : _topLevelGroups) {
        library.writePeakGroupData(group.get());
        for (auto& child : group->childIsotopes())
            library.writePeakGroupData(child.get());
        for (auto& child : group->childAdducts())
            library.writePeakGroupData(child.get());
    }
}

void TableDockWidget::markDuplicatesAsBad()
{
    QStringList selectionItems;
    selectionItems << "Parent compounds only"
                   << "Parent compounds and their isotopes"
                   << "Parent compounds and their adducts"
                   << "All compounds";

    bool ok;
    QString item =
        QInputDialog::getItem(this,
                              "Mark duplicates as bad",
                              "This operation will automatically mark "
                              "duplicates of all compounds as \"bad\"\n"
                              "peak-groups. The original for each "
                              "compound is decided from rows with the\n"
                              "same name but with the highest rank among "
                              "them. The rest of them are regarded\n"
                              "as duplicates.\n\n"
                              "If isotopes or adducts are also considered "
                              "then they will be compared with\n"
                              "their siblings only (i.e., along with the "
                              "name they must share the same parent).",
                              selectionItems,
                              0,
                              false,
                              &ok);

    // lambda: given a map of vectors and a peak-group, adds the peak-group to
    // the vector mapped against its name
    auto indexGroup = [](map<string, vector<shared_ptr<PeakGroup>>>& duplicates,
                         const shared_ptr<PeakGroup>& group) {
        auto name = group->getName();
        if (duplicates.count(name) == 0) {
            duplicates[name] = {};
        }
        duplicates[name].push_back(group);
    };

    // lambda: given a map of vector of peak-groups, within each vector, mark
    // all but the best peak-group as a "bad" one
    auto markAllButTheBest =
        [](map<string, vector<shared_ptr<PeakGroup>>>& duplicates) {
            for (auto& elem : duplicates) {
                auto& groups = elem.second;
                if (groups.size() <= 1)
                    continue;

                sort(begin(groups),
                     end(groups),
                     [](shared_ptr<PeakGroup>& a, shared_ptr<PeakGroup>& b) {
                         return a->groupRank < b->groupRank;
                     });
                for (size_t i = 1; i < groups.size(); ++i) {
                    auto& group = groups[i];
                    group->setLabel('b');
                }
            }
        };

    if (ok && !item.isEmpty()) {
        map<string, vector<shared_ptr<PeakGroup>>> duplicateParents;
        if (item == selectionItems.at(0)) {
            // only parent items
            for (auto& group : _topLevelGroups)
                indexGroup(duplicateParents, group);
        } else if (item == selectionItems.at(1)) {
            // parent items and their child isotopes
            for (auto& group : _topLevelGroups) {
                indexGroup(duplicateParents, group);

                map<string, vector<shared_ptr<PeakGroup>>> duplicateChildren;
                for (auto& child : group->childIsotopes())
                    indexGroup(duplicateChildren, child);
                markAllButTheBest(duplicateChildren);
            }
        } else if (item == selectionItems.at(2)) {
            // parent items and their child adducts
            for (auto& group : _topLevelGroups) {
                indexGroup(duplicateParents, group);

                map<string, vector<shared_ptr<PeakGroup>>> duplicateChildren;
                for (auto& child : group->childAdducts())
                    indexGroup(duplicateChildren, child);
                markAllButTheBest(duplicateChildren);
            }
        } else if (item == selectionItems.at(3)) {
            // parent items and their child isotopes as well adducts
            for (auto& group : _topLevelGroups) {
                indexGroup(duplicateParents, group);

                map<string, vector<shared_ptr<PeakGroup>>> duplicateChildren;
                for (auto& child : group->childIsotopes())
                    indexGroup(duplicateChildren, child);
                for (auto& child : group->childAdducts())
                    indexGroup(duplicateChildren, child);
                markAllButTheBest(duplicateChildren);
            }
        }

        markAllButTheBest(duplicateParents);
        updateTable();
    }
}

vector<EIC*> TableDockWidget::getEICs(float rtmin, float rtmax, PeakGroup& grp)
{
    vector<EIC*> eics;
    for (int i = 0; i < grp.peaks.size(); i++) {
        float mzmin = grp.meanMz - 0.2;
        float mzmax = grp.meanMz + 0.2;
        vector<mzSample*> samples = _mainwindow->getSamples();
        for (unsigned int j = 0; j < samples.size(); j++) {
            if (!grp.srmId.empty()) {
                EIC* eic = samples[j]->getEIC(
                    grp.srmId, _mainwindow->mavenParameters->eicType);
                eics.push_back(eic);
            } else {
                EIC* eic = samples[j]->getEIC(
                    mzmin,
                    mzmax,
                    rtmin,
                    rtmax,
                    1,
                    _mainwindow->mavenParameters->eicType,
                    _mainwindow->mavenParameters->filterline);
                eics.push_back(eic);
            }
        }
    }
    return (eics);
}

bool TableDockWidget::selectPeakGroup(shared_ptr<PeakGroup> group)
{
    if (group == nullptr)
        return false;

    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        QTreeWidgetItem* item = (*it);
        shared_ptr<PeakGroup> currentGroup = groupForItem(item);
        if (currentGroup == group) {
            treeWidget->setCurrentItem(item);
            return true;
        }
        ++it;
    }
    return false;
}

void TableDockWidget::showSelectedGroup()
{
    QTreeWidgetItem* item = treeWidget->currentItem();
    if (!item)
        return;

    shared_ptr<PeakGroup> group = groupForItem(item);

    if (group == nullptr)
        return;

    if (group->isGhost()) {
        emit ghostPeakGroupSelected(true);
    } else {
        emit ghostPeakGroupSelected(false);
    }

    _mainwindow->setPeakGroup(group);
}

QList<shared_ptr<PeakGroup>> TableDockWidget::getSelectedGroups()
{
    QList<shared_ptr<PeakGroup>> selectedGroups;
    Q_FOREACH (QTreeWidgetItem* item, treeWidget->selectedItems()) {
        if (item) {
            shared_ptr<PeakGroup> group = groupForItem(item);
            if (group != nullptr) {
                selectedGroups.append(group);
            }
        }
    }
    return selectedGroups;
}

QList<shared_ptr<PeakGroup>> TableDockWidget::getDisplayedGroups()
{
    QList<shared_ptr<PeakGroup>> displayedGroups;

    Q_FOREACH (QTreeWidgetItem* item, treeWidget->selectedItems()) {
        if (item) {
            if (item->isHidden())
                continue;

            shared_ptr<PeakGroup> group = groupForItem(item);
            if (group != nullptr) {
                displayedGroups.append(group);
            }
        }
    }

    return displayedGroups;
}

QList<PeakGroup*> TableDockWidget::getCustomGroups(
    PeakTableSubsetType peakSelection)
{
    QList<PeakGroup*> selectedGroups;
    PeakTableSubsetType temppeakSelection = peakSelection;
    Q_FOREACH (QTreeWidgetItem* item, treeWidget->selectedItems()) {
        if (item) {
            QVariant v = item->data(1, Qt::UserRole);
            PeakGroup* group = v.value<PeakGroup*>();
            if (group != NULL) {
                if (temppeakSelection == PeakTableSubsetType::Good) {
                    if (group->userLabel() == 'g') {
                        selectedGroups.append(group);
                    }
                } else if (temppeakSelection == PeakTableSubsetType::Bad) {
                    if (group->userLabel() == 'b') {
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

QMap<TableDockWidget::PeakTableSubsetType, QList<QTreeWidgetItem*>>
TableDockWidget::_peakTableGroupedBySubsets()
{
    QList<QTreeWidgetItem*> allItems;
    QList<QTreeWidgetItem*> selectedItems;
    QList<QTreeWidgetItem*> goodItems;
    QList<QTreeWidgetItem*> badItems;
    QList<QTreeWidgetItem*> nonBadItems;
    QList<QTreeWidgetItem*> unmarkedItems;
    QList<QTreeWidgetItem*> correlatedItems;
    QList<QTreeWidgetItem*> varianceItems;
    QList<QTreeWidgetItem*> correlatedVarianceItems;
    QList<QTreeWidgetItem*> maybeGoodItems;

    QTreeWidgetItemIterator itr(treeWidget);
    while (*itr) {
        QTreeWidgetItem* item = (*itr);
        if (item) {
            auto group = groupForItem(item).get();

            if (group == nullptr)
                continue;

            // all groups are inserted into this list
            allItems.append(item);

            // all selected groups are inserted into this list
            if (item->isSelected())
                selectedItems.append(item);

            auto label = group->predictedLabel();
            if (label == PeakGroup::ClassifiedLabel::Noise) {
                badItems.append(item);
            } else {
                nonBadItems.append(item);
            }
            if (label == PeakGroup::ClassifiedLabel::Signal)
                goodItems.append(item);
            if (label == PeakGroup::ClassifiedLabel::None)
                unmarkedItems.append(item);
            if (label == PeakGroup::ClassifiedLabel::Correlation)
                correlatedItems.append(item);
            if (label == PeakGroup::ClassifiedLabel::Pattern)
                varianceItems.append(item);
            if (label == PeakGroup::ClassifiedLabel::CorrelationAndPattern)
                correlatedVarianceItems.append(item);
            if (label == PeakGroup::ClassifiedLabel::MaybeGood)
                maybeGoodItems.append(item);
        }
        ++itr;
    }

    QMap<PeakTableSubsetType, QList<QTreeWidgetItem*>> itemsBySubset;
    itemsBySubset.insert(PeakTableSubsetType::Selected, selectedItems);
    itemsBySubset.insert(PeakTableSubsetType::Whole, allItems);
    itemsBySubset.insert(PeakTableSubsetType::Good, goodItems);
    itemsBySubset.insert(PeakTableSubsetType::Bad, badItems);
    itemsBySubset.insert(PeakTableSubsetType::ExcludeBad, nonBadItems);
    itemsBySubset.insert(PeakTableSubsetType::Unmarked, unmarkedItems);
    itemsBySubset.insert(PeakTableSubsetType::Correlated, correlatedItems);
    itemsBySubset.insert(PeakTableSubsetType::Variance, varianceItems);
    itemsBySubset.insert(PeakTableSubsetType::CorrelatedVariance,
                         correlatedVarianceItems);
    itemsBySubset.insert(PeakTableSubsetType::MaybeGood, maybeGoodItems);

    return itemsBySubset;
}

void TableDockWidget::hideEvent(QHideEvent* event)
{
    QDockWidget::hideEvent(event);
    if (_mainwindow->getCorrelationTable()->currentTable() == this)
        _mainwindow->getCorrelationTable()->setVisible(false);
}

void TableDockWidget::showEvent(QShowEvent* event)
{
    QDockWidget::showEvent(event);
    _refreshCycleBuffer();
}

void TableDockWidget::_refreshCycleBuffer()
{
    if (treeWidget->topLevelItemCount() == 0)
        return;

    if (_cycleInProgress) {
        int currentGroupId = treeWidget->currentItem()->text(1).toInt();
        _mainwindow->getCorrelationTable()->selectGroupId(currentGroupId);
        return;
    }

    auto selectedGroup = getSelectedGroup();
    if (selectedGroup == nullptr)
        return;

    _cycleBuffer.clear();

    auto correlatedGroups = selectedGroup->getCorrelatedGroups();

    if (correlatedGroups.empty()) {
        _mainwindow->getCorrelationTable()->setVisible(false);
        return;
    }
    _mainwindow->getCorrelationTable()->setVisible(true);
    _mainwindow->getCorrelationTable()->setReferencePeakGroup(
        selectedGroup.get());
    _mainwindow->getCorrelationTable()->setCurrentTable(this);

    auto processGroupForCorrelation =
        [&](PeakGroup* group, PeakGroup* selectedGroup, TableDockWidget* tb) {
            auto item = itemForGroup(group);

            if (group == selectedGroup) {
                tb->_cycleBuffer.append(item);
            }

            if (correlatedGroups.count(group->groupId())) {
                tb->_cycleBuffer.append(item);
                auto corr = correlatedGroups.at(group->groupId());
                tb->_mainwindow->getCorrelationTable()->addCorrelatedPeakGroup(
                    group, corr);
            }
        };

    for (int i = 0; i < _topLevelGroups.size(); i++) {
        auto group = _topLevelGroups[i];
        if (group == nullptr)
            continue;
        processGroupForCorrelation(group.get(), selectedGroup.get(), this);

        for (auto& child : group->childIsotopes())
            processGroupForCorrelation(child.get(), selectedGroup.get(), this);
    }

    int currentGroupId = treeWidget->currentItem()->text(1).toInt();
    _mainwindow->getCorrelationTable()->selectGroupId(currentGroupId);
}

shared_ptr<PeakGroup> TableDockWidget::getSelectedGroup()
{
    QTreeWidgetItem* item = treeWidget->currentItem();
    if (!item)
        return NULL;
    shared_ptr<PeakGroup> group = groupForItem(item);
    if (group != nullptr) {
        return group;
    } else
        return shared_ptr<PeakGroup>(nullptr);
}

void TableDockWidget::setGroupLabel(char label)
{
    Q_FOREACH (QTreeWidgetItem* item, treeWidget->selectedItems()) {
        if (item) {
            shared_ptr<PeakGroup> group = groupForItem(item);

            multimap<float, string> predictionInference;
            if (hasClassifiedGroups) {
                predictionInference = group->predictionInference();
            }

            if (group != nullptr) {
                if (group->userLabel() != 'g' && group->userLabel() != 'b') {
                    numberOfGroupsMarked += 1;
                    subsetPeakGroups.push_back(*(group.get()));
                }
                group->setUserLabel(label);
                updateItem(item);
                if (item->parent() != nullptr)
                    updateItem(item->parent(), false);

                if (hasClassifiedGroups) {
                    group->setPredictionInference(predictionInference);
                }
            }
        }
        updateStatus();
    }
}

void TableDockWidget::_deleteItemsAndGroups(QSet<QTreeWidgetItem*>& items)
{
    if (items.isEmpty())
        return;

    // temporarily disconnect selection trigger
    disconnect(treeWidget,
               &QTreeWidget::itemSelectionChanged,
               this,
               &TableDockWidget::showSelectedGroup);

    QVector<shared_ptr<PeakGroup>> parentGroupsToDelete;
    QMap<PeakGroup*, PeakGroup*> childGroupsToDelete;
    for (auto item : items) {
        if (item == nullptr)
            continue;
        if (item->parent() != nullptr && items.contains(item->parent()))
            continue;

        auto group = groupForItem(item);
        if (group->parent != nullptr) {
            childGroupsToDelete.insert(group.get(), group->parent);
        } else {
            parentGroupsToDelete.append(group);
        }
    }

    for (auto& parentGroup : parentGroupsToDelete) {
        if (parentGroup->childIsotopeCount() > 0)
            _labeledGroups--;
        if (parentGroup->hasCompoundLink())
            _targetedGroups--;
        _topLevelGroups.removeOne(parentGroup);
    }

    for (auto& childGroup : childGroupsToDelete.keys()) {
        auto parentGroup = childGroupsToDelete.value(childGroup);
        if (childGroup != nullptr && parentGroup != nullptr)
            parentGroup->removeChild(childGroup);
    }

    // TODO: delete peak-groups from any autosaved emDB as well

    // possibly the most expensive call in this whole method
    showAllGroups();

    // reconnect selection trigger
    connect(treeWidget,
            &QTreeWidget::itemSelectionChanged,
            this,
            &TableDockWidget::showSelectedGroup);
}

void TableDockWidget::deleteGroup(PeakGroup* group)
{
    if (group == nullptr)
        return;

    QSet<QTreeWidgetItem*> itemsToDelete;
    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        QTreeWidgetItem* item = (*it);
        if (item->isHidden()) {
            ++it;
            continue;
        }
        shared_ptr<PeakGroup> itemGroup = groupForItem(item);
        if (itemGroup != nullptr and itemGroup.get() == group) {
            itemsToDelete.insert(item);
            break;
        }
        ++it;
    }
    _deleteItemsAndGroups(itemsToDelete);
    updateCompoundWidget();
}

bool TableDockWidget::deleteAllgroupsWarning()
{
    QMessageBox* warning = new QMessageBox(this);
    bool selectedOption;

    auto htmlText = QString(
        "<p><b>Are you sure you want to permanently erase all the "
        "groups from this table?</b></p>");
    htmlText += "<p>You can not undo this action.</p>";
    warning->setText(htmlText);
    warning->setIcon(QMessageBox::Icon::Warning);

    auto noButton = warning->addButton(tr("No"), QMessageBox::RejectRole);
    auto yesButton = warning->addButton(tr("Yes"), QMessageBox::AcceptRole);
    warning->exec();

    QCoreApplication::processEvents();

    if (warning->clickedButton() == yesButton)
        return true;

    return false;
}

void TableDockWidget::deleteSelectedItems(bool groupsMovedToAnotherTable)
{
    QSet<QTreeWidgetItem*> selectedItems;
    int topLevelItemsBeingDeleted = 0;
    for (auto item : treeWidget->selectedItems()) {
        selectedItems.insert(item);
        if (item->parent() == nullptr)
            ++topLevelItemsBeingDeleted;
    }

    // checks if the selected item count is same as the no. of top-level
    // groups in the table.
    if (topLevelItemsBeingDeleted == topLevelGroupCount()
        && !groupsMovedToAnotherTable) {
        deleteAll();
        return;
    }

    QSet<PeakGroup*> expandedParentGroups;
    QTreeWidgetItem* itemBelow = nullptr;
    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        auto item = *it;
        if (item->isSelected()) {
            // this branch tries to find the next item to be selected
            itemBelow = treeWidget->itemBelow(item);
            if (item->parent() != nullptr
                && selectedItems.contains(item->parent())) {
                while (itemBelow != nullptr && itemBelow->parent() != nullptr)
                    itemBelow = treeWidget->itemBelow(itemBelow);
            }
        } else if (item->parent() == nullptr && item->isExpanded()) {
            // this helps us store all parent items that were in expanded state
            auto expandedGroup = groupForItem(item);
            expandedParentGroups.insert(expandedGroup.get());
        }
        ++it;
    }
    if (itemBelow == nullptr) {
        int lastItemIndex = treeWidget->topLevelItemCount() - 1;
        auto lastItem = treeWidget->topLevelItem(lastItemIndex);
        while (lastItem != nullptr && selectedItems.contains(lastItem))
            lastItem = treeWidget->topLevelItem(--lastItemIndex);
        itemBelow = lastItem;
    }

    shared_ptr<PeakGroup> nextGroup = nullptr;
    if (itemBelow != nullptr)
        nextGroup = groupForItem(itemBelow);

    // store sort state
    int sortColumn = treeWidget->sortColumn();
    auto sortOrder = treeWidget->sortOrder();

    _deleteItemsAndGroups(selectedItems);

    // restore sort state (because table was cleared and repopulated)
    treeWidget->sortByColumn(sortColumn, sortOrder);

    if (_topLevelGroups.empty()) {
        _mainwindow->getEicWidget()->replot(nullptr);
        _mainwindow->ligandWidget->resetColor();
        _mainwindow->removePeaksTable(this);
    } else if (nextGroup != nullptr) {
        // if a next group was available, we select it and bring it in view
        selectPeakGroup(nextGroup);
        auto currentItem = treeWidget->currentItem();
        if (currentItem != nullptr) {
            treeWidget->scrollToItem(currentItem,
                                     QAbstractItemView::PositionAtCenter);
        }
    }

    // re-expand items after table was cleared and repopulated
    if (!_topLevelGroups.empty() && expandedParentGroups.size() > 0) {
        QTreeWidgetItemIterator it2(treeWidget);
        while (*it2) {
            auto item = *(it2++);
            if (item->parent() != nullptr)
                continue;

            auto group = groupForItem(item);
            if (expandedParentGroups.contains(group.get()))
                item->setExpanded(true);
        }
    }

    updateCompoundWidget();
}

void TableDockWidget::setClipboard()
{
    _mainwindow->getAnalytics()->hitEvent(
        "Exports", "Clipboard", "From Peak Table Menu");
    QList<shared_ptr<PeakGroup>> groups = getSelectedGroups();
    if (groups.size() > 0) {
        _mainwindow->isotopeWidget->setClipboard(groups);
    }
}

void TableDockWidget::showConsensusSpectra()
{
    shared_ptr<PeakGroup> group = getSelectedGroup();
    if (group != nullptr) {
        _mainwindow->fragSpectraDockWidget->setVisible(true);
        _mainwindow->fragSpectraWidget->overlayPeakGroup(group);
    }
}

void TableDockWidget::saveLabelForUndo(PeakGroup* group)
{
    auto label = group->labelToString(group->predictedLabel());
    auto probability = group->predictionProbability();
    pair<string, float> groupLabel = make_pair(label, probability);
    undoBuffer[group->groupId()] = groupLabel;
}

void TableDockWidget::markGroupGood()
{
    auto currentGroups = getSelectedGroups();

    if (currentGroups.isEmpty())
        return;

    treeWidget->setSortingEnabled(false);
    QStringList selectedLabels;
    if (hasClassifiedGroups) {
        for (auto group : currentGroups) {
            saveLabelForUndo(group.get());
        }
        selectedLabels = _legend->selectedTexts();
    }
    setGroupLabel('g');
    showNextGroup();
    if (hasClassifiedGroups) {
        updateLegend();
        _legend->setCurrentTexts(selectedLabels);
    }

    treeWidget->sortByColumn(-1, Qt::AscendingOrder);
    treeWidget->setSortingEnabled(true);
    _mainwindow->getAnalytics()->hitEvent("Peak Group Curation", "Mark Good");
    _mainwindow->autoSaveSignal(currentGroups);
}

void TableDockWidget::markGroupBad()
{
    auto currentGroups = getSelectedGroups();

    if (currentGroups.isEmpty())
        return;

    treeWidget->setSortingEnabled(false);
    QStringList selectedLabels;
    if (hasClassifiedGroups) {
        for (auto group : currentGroups) {
            saveLabelForUndo(group.get());
        }
        selectedLabels = _legend->selectedTexts();
    }
    setGroupLabel('b');
    showNextGroup();
    if (hasClassifiedGroups) {
        updateLegend();
        _legend->setCurrentTexts(selectedLabels);
    }

    _mainwindow->getAnalytics()->hitEvent("Peak Group Curation", "Mark Bad");
    treeWidget->sortByColumn(-1, Qt::AscendingOrder);
    treeWidget->setSortingEnabled(true);
    _mainwindow->autoSaveSignal(currentGroups);
}

void TableDockWidget::unmarkGroup()
{
    // TODO: Add a button for unmarking peak-groups?
    setGroupLabel('\0');
    auto currentGroups = getSelectedGroups();
    if (currentGroups.isEmpty())
        return;

    _mainwindow->getAnalytics()->hitEvent("Peak Group Curation", "Unmark");
    _mainwindow->autoSaveSignal(currentGroups);
}

void TableDockWidget::markGroupIgnored()
{
    setGroupLabel('i');
    showNextGroup();
}

void TableDockWidget::showLastGroup()
{
    QTreeWidgetItem* item = treeWidget->currentItem();
    if (item != NULL) {
        treeWidget->setCurrentItem(treeWidget->itemAbove(item));
    }
}

void TableDockWidget::showNextGroup()
{
    QTreeWidgetItem* item = treeWidget->currentItem();
    if (item == NULL)
        return;

    // get next item
    QTreeWidgetItem* nextitem = treeWidget->itemBelow(item);
    if (nextitem != NULL)
        treeWidget->setCurrentItem(nextitem);
}

void TableDockWidget::keyPressEvent(QKeyEvent* e)
{
    QTreeWidgetItem* item = treeWidget->currentItem();
    if (e->key() == Qt::Key_Delete) {
        QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
        if (items.size() > 0) {
            deleteSelectedItems();
        }
    } else if (e->key() == Qt::Key_G) {
        if (item) {
            markGroupGood();
        }
    } else if (e->key() == Qt::Key_B) {
        if (item) {
            markGroupBad();
        }
    } else if (e->key() == Qt::Key_U) {
        if (item) {
            unmarkGroup();
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
                    treeWidget->collapseItem(
                        treeWidget->currentItem()->parent());
                    treeWidget->setCurrentItem(
                        treeWidget->currentItem()->parent());
                } else {
                    treeWidget->collapseItem(treeWidget->currentItem());
                }
            } else {
                treeWidget->expandItem(treeWidget->currentItem());
            }
        }
    } else if (e->key() == Qt::Key_Down
               && e->modifiers() == Qt::ShiftModifier) {
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
    } else if (e->key() == Qt::Key_E) {
        editSelectedPeakGroup();
    } else if (e->key() == Qt::Key_L) {
        if (item && _cycleBuffer.size() > 1) {
            int currentIndex = _cycleBuffer.indexOf(item);
            if (currentIndex != -1) {
                int nextIndex = 0;
                if (currentIndex < _cycleBuffer.size() - 1)
                    nextIndex = currentIndex + 1;

                _cycleInProgress = true;
                QTreeWidgetItem* newItem = _cycleBuffer.at(nextIndex);
                treeWidget->clearSelection();
                treeWidget->setCurrentItem(newItem);
                treeWidget->scrollTo(treeWidget->currentIndex(),
                                     QAbstractItemView::PositionAtCenter);
                _cycleInProgress = false;
            }
        }
    }
    QDockWidget::keyPressEvent(e);
    updateStatus();
}

void TableDockWidget::updateStatus()
{
    int totalCount = 0;
    int goodCount = 0;
    int badCount = 0;
    for (auto group : _topLevelGroups) {
        char groupLabel = group->userLabel();
        if (groupLabel == 'g' && !group->isGhost()) {
            goodCount++;
        } else if (groupLabel == 'b' && !group->isGhost()) {
            badCount++;
        }
        totalCount++;
    }
    QString title = tr("Group Validation Status: Good=%2 Bad=%3 Total=%1")
                        .arg(QString::number(totalCount),
                             QString::number(goodCount),
                             QString::number(badCount));
    _mainwindow->setStatusText(title);
}

void TableDockWidget::showScatterPlot()
{
    if (topLevelGroupCount() == 0)
        return;
    _mainwindow->scatterDockWidget->setVisible(true);
    _mainwindow->scatterDockWidget->setTable(this);
    _mainwindow->scatterDockWidget->replot();
    _mainwindow->scatterDockWidget->contrastGroups();
}

void TableDockWidget::printPdfReport()
{
    _mainwindow->getAnalytics()->hitEvent("Exports", "PDF", "From Table");
    QString dir = ".";
    QSettings* settings = _mainwindow->getSettings();
    if (settings->contains("lastDir"))
        dir = settings->value("lastDir").value<QString>();

    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save Group Report a PDF File"), dir, tr("*.pdf"));
    if (fileName.isEmpty())
        return;
    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive))
        fileName = fileName + ".pdf";

    auto res = QtConcurrent::run(this, &TableDockWidget::renderPdf, fileName);
}

void TableDockWidget::renderPdf(QString fileName)
{
    // Setting printer.
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOrientation(QPrinter::Landscape);
    printer.setCreator("El-MAVEN");
    printer.setOutputFileName(fileName);
    printer.setResolution(50);

    QPainter painter;

    if (!painter.begin(&printer)) {
        // failed to open file
        qWarning("failed to open file, is it writable?");
        return;
    }

    if (printer.printerState() != QPrinter::Active) {
        qDebug() << "PrinterState:" << printer.printerState();
    }

    QList<shared_ptr<PeakGroup>> selected;

    // PDF report only for selected groups
    if (peakTableSelection == PeakTableSubsetType::Selected) {
        selected = getSelectedGroups();
    } else if (peakTableSelection == PeakTableSubsetType::Whole
               || peakTableSelection == PeakTableSubsetType::Good) {
        selected = getGroups();
    } else if (peakTableSelection == PeakTableSubsetType::Displayed) {
        selected = getDisplayedGroups();
    }

    for (int i = 0; i < selected.size(); i++) {
        shared_ptr<PeakGroup> group = selected[i];
        emit updateProgressBar(
            "Saving PDF export for table…", i + 1, selected.size(), true);
        if (!group->isGhost()) {
            _mainwindow->getEicWidget()->renderPdf(group, &painter);
            if (!printer.newPage()) {
                qWarning("failed in flushing page to disk, disk full?");
                return;
            }
        }
        for (auto& child : group->childIsotopes()) {
            if (peakTableSelection == PeakTableSubsetType::Good
                && child->userLabel() != 'g')
                continue;
            if (peakTableSelection == PeakTableSubsetType::Displayed
                && child->isGroupHidden())
                continue;

            _mainwindow->getEicWidget()->renderPdf(child, &painter);
            if (!printer.newPage()) {
                qWarning("failed in flushing page to disk, disk full?");
                return;
            }
        }
        for (auto& child : group->childAdducts()) {
            _mainwindow->getEicWidget()->renderPdf(child, &painter);
            if (!printer.newPage()) {
                qWarning("failed in flushing page to disk, disk full?");
                return;
            }
        }
    }
    painter.end();
    emit updateProgressBar("", selected.size(), selected.size());
    emit renderedPdf();
}

void TableDockWidget::pdfReadyNotification()
{
    QIcon icon = QIcon("");
    QString title("");
    QString message("Your PDF has been saved successfully.");

    Notificator* fluxomicsPrompt =
        Notificator::showMessage(icon, title, message, this);
    title = "Your PDF has been saved successfully.";
    _mainwindow->setStatusText(title);
}

void TableDockWidget::editSelectedPeakGroup()
{
    if (treeWidget->selectedItems().size() != 1)
        return;

    shared_ptr<PeakGroup> group = getSelectedGroup();
    PeakEditor* editor = _mainwindow->peakEditor();
    if (editor == nullptr || group == nullptr || group->isGhost())
        return;

    editor->setPeakGroup(group);
    editor->exec();

    auto currentItem = treeWidget->currentItem();
    if (currentItem->parent() != nullptr) {
        updateItem(currentItem->parent(), true);
    } else {
        updateItem(currentItem, true);
    }

    auto groupToSave = group;
    if (group->parent != nullptr) {
        QTreeWidgetItemIterator it(treeWidget);
        while (*it) {
            QTreeWidgetItem* item = (*it);
            shared_ptr<PeakGroup> currentGroup = groupForItem(item);
            if (currentGroup.get() == group->parent) {
                groupToSave = currentGroup;
                break;
            }
            it++;
        }
    }
    _mainwindow->autoSaveSignal({groupToSave});
}

void TableDockWidget::showIntegrationSettings()
{
    if (treeWidget->selectedItems().size() != 1)
        return;

    shared_ptr<PeakGroup> group = getSelectedGroup();
    GroupSettingsLog log(this, group);
    log.exec();
}

void TableDockWidget::moveSelectedRows(QString destinationTableName)
{
    QString sourceTableName = this->titlePeakTable->text();
    QList<QPointer<TableDockWidget>> peaksTableList =
        _mainwindow->getPeakTableList();
    QList<shared_ptr<PeakGroup>> selected = getSelectedGroups();
    QString bookmarkTableName =
        _mainwindow->bookmarkedPeaks->titlePeakTable->text();

    // Adding the rows to the Destination Table
    if (bookmarkTableName == destinationTableName) {
        BookmarkTableDockWidget* destinationPeakTable =
            _mainwindow->bookmarkedPeaks;
        for (auto group : selected) {
            destinationPeakTable->addPeakGroup(group.get());
        }
        destinationPeakTable->showAllGroups();
        destinationPeakTable->setVisible(true);
    } else {
        TableDockWidget* destinationPeakTable = nullptr;
        for (auto table : peaksTableList) {
            if (table->titlePeakTable->text() == destinationTableName) {
                destinationPeakTable = table;
                break;
            }
        }

        if (!destinationPeakTable)
            return;
        for (auto group : selected) {
            destinationPeakTable->addPeakGroup(group.get());
        }
        destinationPeakTable->showAllGroups();
    }

    // Deleting the rows from the Source Table
    deleteSelectedItems(true);
}

void TableDockWidget::contextMenuEvent(QContextMenuEvent* event)
{
    if (treeWidget->topLevelItemCount() < 1)
        return;

    QMenu menu;
    QAction* z0 = menu.addAction("Copy to clipboard");
    connect(z0, SIGNAL(triggered()), this, SLOT(setClipboard()));

    QAction* z1 = menu.addAction("Edit peak-group");
    connect(
        z1, &QAction::triggered, this, &TableDockWidget::editSelectedPeakGroup);

    QAction* z2 = menu.addAction("Show integration settings");
    connect(z2,
            &QAction::triggered,
            this,
            &TableDockWidget::showIntegrationSettings);

    QAction* z4 = menu.addAction("Mark duplicates as bad");
    connect(
        z4, &QAction::triggered, this, &TableDockWidget::markDuplicatesAsBad);

    QAction* z5 = menu.addAction("Delete all groups from this table");
    connect(z5, SIGNAL(triggered()), SLOT(deleteAll()));

    QList<QPointer<TableDockWidget>> peaksTableList =
        _mainwindow->getPeakTableList();
    int n = peaksTableList.count();

    QMenu* subMenu = menu.addMenu(tr("Move to another table…"));

    QMap<QAction*, QString> actionsAndTables;
    QString tableName = _mainwindow->bookmarkedPeaks->titlePeakTable->text();
    if (this->titlePeakTable->text() != tableName) {
        QAction* z5 = subMenu->addAction(tableName);
        actionsAndTables[z5] = tableName;
    }

    for (int i = 0; i < n; i++) {
        tableName = peaksTableList[i]->titlePeakTable->text();
        if (this->titlePeakTable->text() != tableName) {
            QAction* z5 = subMenu->addAction(tableName);
            actionsAndTables[z5] = tableName;
        }
    }

    QList<shared_ptr<PeakGroup>> selected = getSelectedGroups();

    QMapIterator<QAction*, QString> iter(actionsAndTables);
    while (iter.hasNext()) {
        iter.next();
        QAction* action = iter.key();
        QString tableName = iter.value();
        connect(action, &QAction::triggered, this, [this, tableName]() {
            this->moveSelectedRows(tableName);
        });
    }

    if (selected.count() == 0) {
        // disable actions not relevant when nothing is selected
        subMenu->setEnabled(false);
    }

    if (treeWidget->selectedItems().empty()) {
        // disable actions not relevant when nothing is selected
        z0->setEnabled(false);
    }
    if (treeWidget->selectedItems().size() != 1) {
        // disable actions not relevant to individual peak-groups
        z1->setEnabled(false);
        z2->setEnabled(false);
    }

    if (hasClassifiedGroups) {
        QAction* z8 = menu.addAction("Explain classification");
        classificationWidget = new ClassificationWidget(this);
        connect(z8,
                &QAction::triggered,
                classificationWidget,
                &ClassificationWidget::showClassification);
        if (treeWidget->selectedItems().size() != 1) {
            // disable actions not relevant to individual peak-groups
            z8->setEnabled(false);
        }
    }

    QAction* selectedAction = menu.exec(event->globalPos());
}

void TableDockWidget::focusInEvent(QFocusEvent* event)
{
    if (event->gotFocus()) {
        pal.setColor(QPalette::Background, QColor(255, 255, 255, 100));
        setPalette(pal);
        _mainwindow->setActiveTable(this);
    }
}

void TableDockWidget::filterPeakTable()
{
    updateTable();
}

void TableDockWidget::focusOutEvent(QFocusEvent* event)
{
    if (event->lostFocus()) {
        pal.setColor(QPalette::Background, QColor(170, 170, 170, 100));
        setPalette(pal);
    }
}

void TableDockWidget::writeQEInclusionList(QString filename)
{
    QFile file(filename);
    if (!file.open(QFile::WriteOnly)) {
        QErrorMessage errDialog(this);
        errDialog.showMessage("File open " + filename + " failed");
        return;  // error
    }

    QList<shared_ptr<PeakGroup>> selected = getSelectedGroups();

    float window = 1.5;
    int polarity = _mainwindow->mavenParameters->ionizationMode;
    QTextStream out(&file);
    for (auto g : selected) {
        out << g->meanMz << ",";
        polarity > 0 ? out << "Positive," : out << "Negative,";
        out << g->meanRt - window << ",";
        out << g->meanRt + window << ",";
        out << 25 << ",";  // default CE set to 25
        out << 2 << ",";
        out << QString(g->getName().c_str());
        out << endl;
    }
    file.close();
}

void TableDockWidget::writeMascotGeneric(QString filename)
{
    QFile file(filename);
    if (!file.open(QFile::WriteOnly)) {
        QErrorMessage errDialog(this);
        errDialog.showMessage("File open " + filename + " failed");
        return;  // error
    }

    QList<shared_ptr<PeakGroup>> selected = getSelectedGroups();
    QTextStream out(&file);
    for (auto g : selected) {
        Scan* cons = g->getAverageFragmentationScan(
            _mainwindow->mavenParameters->fragmentTolerance);

        if (cons) {
            string scandata = cons->toMGF();
            out << scandata.c_str();
        }
    }
    file.close();
}

void TableDockWidget::showRelabelWidget()
{
    relabelDialog->showDialog();
}

void TableDockWidget::relabelGroups(float changedBadLimit,
                                    float changedMaybeGoodLimit)
{
    disconnect(_legend,
               &MultiSelectComboBox::selectionChanged,
               this,
               &TableDockWidget::filterForSelectedLabels);

    if (changedBadLimit == badGroupLimit
        && changedMaybeGoodLimit == maybeGoodGroupLimit) {
        QMessageBox::warning(
            this,
            "No Change!",
            "The curation parameters you entered are same as "
            "they were in the classified peak-table.\nYou may change the range"
            " and try again.\n");
        _legend->uncheckRelabel();
        connect(_legend,
                &MultiSelectComboBox::selectionChanged,
                this,
                &TableDockWidget::filterForSelectedLabels);
        return;
    }
    badGroupLimit = changedBadLimit;
    maybeGoodGroupLimit = changedMaybeGoodLimit;
    auto changePrediction = [&](PeakGroup* group,
                                float changedBadLimit,
                                float changedMaybeGoodLimit) {
        if (group->predictionProbability() < changedBadLimit) {
            group->setPredictedLabel(PeakGroup::ClassifiedLabel::Noise,
                                     group->predictionProbability());
            auto correlatedGroups = group->getCorrelatedGroups();
            correlatedGroups.clear();
            group->setCorrelatedGroups(correlatedGroups);
        } else if (group->predictionProbability() >= changedBadLimit
                   && group->predictionProbability() < changedMaybeGoodLimit) {
            group->setPredictedLabel(PeakGroup::ClassifiedLabel::MaybeGood,
                                     group->predictionProbability());
            auto correlatedGroups = group->getCorrelatedGroups();
            correlatedGroups.clear();
            group->setCorrelatedGroups(correlatedGroups);
        } else {
            if (group->predictedLabel() == PeakGroup::ClassifiedLabel::Noise
                || group->predictedLabel()
                       == PeakGroup::ClassifiedLabel::MaybeGood) {
                group->setPredictedLabel(PeakGroup::ClassifiedLabel::Signal,
                                         group->predictionProbability());
            }
        }

        auto parameters = group->parameters().get();
        parameters->badGroupUpperLimit = changedBadLimit;
        parameters->goodGroupLowerLimit = changedMaybeGoodLimit;
        group->setParameters(make_shared<MavenParameters>(*parameters));
    };

    auto groups = _topLevelGroups;
    int totalSteps = groups.size();
    int steps = 1;
    for (auto& group : groups) {
        Q_EMIT(updateRelabelStatusBar("Relabeling...", steps, totalSteps));
        steps++;
        changePrediction(group.get(), changedBadLimit, changedMaybeGoodLimit);
        for (auto child : group->childIsotopes())
            changePrediction(
                child.get(), changedBadLimit, changedMaybeGoodLimit);

        for (auto child : group->childAdducts())
            changePrediction(
                child.get(), changedBadLimit, changedMaybeGoodLimit);
    }
    _legend->selectAll();
    showAllGroups();
    connect(_legend,
            &MultiSelectComboBox::selectionChanged,
            this,
            &TableDockWidget::filterForSelectedLabels);
}

void TableDockWidget::clearClusters()
{
    for (auto group : _topLevelGroups)
        group->clusterId = 0;
    showAllGroups();
}

void TableDockWidget::clusterGroups()
{
    sort(_topLevelGroups.begin(),
         _topLevelGroups.end(),
         [](shared_ptr<PeakGroup> a, shared_ptr<PeakGroup> b) {
             return a->meanRt < b->meanRt;
         });
    qDebug() << "Clustering…";
    int clusterId = 0;

    double maxRtDiff = clusterDialog->maxRtDiff_2->value();
    double minSampleCorrelation = clusterDialog->minSampleCorr->value();
    double minRtCorrelation = clusterDialog->minRt->value();
    MassCutoff* massCutoff = _mainwindow->getUserMassCutoff();

    vector<mzSample*> samples = _mainwindow->getSamples();

    // clear cluster information
    for (auto group : _topLevelGroups)
        group->clusterId = 0;

    map<int, shared_ptr<PeakGroup>> parentGroups;
    for (int i = 0; i < _topLevelGroups.size(); ++i) {
        auto group1 = _topLevelGroups[i];
        if (group1->clusterId == 0) {
            // create new cluster
            group1->clusterId = ++clusterId;
            parentGroups[clusterId] = group1;
        }

        // cluster parent
        shared_ptr<PeakGroup> parent = parentGroups[clusterId];

        mzSample* largestSample = NULL;
        double maxIntensity = 0;

        for (int i = 0; i < group1->peakCount(); i++) {
            mzSample* sample = group1->peaks[i].getSample();
            if (group1->peaks[i].peakIntensity > maxIntensity)
                largestSample = sample;
        }

        if (largestSample == NULL)
            continue;
        vector<float> peakIntensityA =
            group1->getOrderedIntensityVector(samples, PeakGroup::AreaTop);

        for (auto group2 : _topLevelGroups) {
            if (group2->clusterId > 0)
                continue;

            // retention time distance
            float rtdist = abs(parent->meanRt - group2->meanRt);
            if (rtdist > maxRtDiff * 2)
                continue;

            // retention time overlap
            float rtoverlap = mzUtils::checkOverlap(
                group1->minRt, group1->maxRt, group2->minRt, group2->maxRt);
            if (rtoverlap < 0.1)
                continue;

            // peak intensity correlation
            vector<float> peakIntensityB =
                group2->getOrderedIntensityVector(samples, PeakGroup::AreaTop);
            float cor = correlation(peakIntensityA, peakIntensityB);
            if (cor < minSampleCorrelation)
                continue;

            // peak shape correlation
            float cor2 = largestSample->correlation(
                group1->meanMz,
                group2->meanMz,
                massCutoff,
                group1->minRt,
                group1->maxRt,
                _mainwindow->mavenParameters->eicType,
                _mainwindow->mavenParameters->filterline);
            if (cor2 < minRtCorrelation)
                continue;

            // passed all the filters, group1 and group2 into a single metagroup
            group2->clusterId = group1->clusterId;
        }
        if (i % 10 == 0)
            _mainwindow->setProgressBar(
                "Clustering…", i + 1, _topLevelGroups.size());
    }

    _mainwindow->setProgressBar(
        "Clustering done!", _topLevelGroups.size(), _topLevelGroups.size());
    showAllGroups();
}

void TableDockWidget::showFocusedGroups()
{
    int N = treeWidget->topLevelItemCount();
    for (int i = 0; i < N; i++) {
        QTreeWidgetItem* item = treeWidget->topLevelItem(i);
        shared_ptr<PeakGroup> group = groupForItem(item);
        if (group != nullptr && group->isFocused)
            item->setHidden(false);
        else
            item->setHidden(true);

        if (item->text(0).startsWith("Cluster")) {
            bool showParentFlag = false;
            for (int j = 0; j < item->childCount(); j++) {
                shared_ptr<PeakGroup> group = groupForItem(item->child(j));
                if (group != nullptr && group->isFocused) {
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

void TableDockWidget::clearFocusedGroups()
{
    for (auto group : _topLevelGroups)
        group->isFocused = false;
}

void TableDockWidget::switchTableView()
{
    viewType == groupView ? viewType = peakView : viewType = groupView;
    setupPeakTable();
    showAllGroups();
    updateTable();
}

int TableDockWidget::getTargetedGroupCount()
{
    return _targetedGroups;
}

int TableDockWidget::getLabeledGroupCount()
{
    return _labeledGroups;
}

QString TableDockWidget::getTitleForId(int tableId)
{
    return _idTitleMap.value(tableId, QString(""));
}

void TableDockWidget::setTitleForId(int tableId, const QString& tableTitle)
{
    if (_idTitleMap.contains(tableId))
        return;

    QString title = tableTitle;
    if (tableId == -1) {
        title = "Scatterplot Peak Table";
    } else if (tableId == 0) {
        title = "Bookmark Table";
    } else if (title.isEmpty()) {
        title = QString("Peak Table ") + QString::number(tableId);
    } else {
        QString expression("^(%1) (?:\\((\\d+)\\)$)");
        QRegularExpression re(expression.arg(tableTitle));
        bool titleExists = false;
        int highestCounter = 0;
        for (auto& existingTitle : _idTitleMap.values()) {
            QRegularExpressionMatch match = re.match(existingTitle);
            if (match.hasMatch()) {
                titleExists = true;
                int currentCounter = match.captured(2).toInt();
                highestCounter = currentCounter > highestCounter
                                     ? currentCounter
                                     : highestCounter;
            } else if (existingTitle == tableTitle) {
                titleExists = true;
            }
        }
        if (titleExists)
            title = QString("%1 (%2)").arg(tableTitle).arg(highestCounter + 1);
    }
    _idTitleMap.insert(tableId, title);
}

int TableDockWidget::lastTableId()
{
    if (!_idTitleMap.isEmpty())
        return _idTitleMap.lastKey();
    return -1;
}

void TableDockWidget::setDefaultStyle(bool isActive)
{
    QString style = "";
    style += "QToolBar QLabel { margin: 0 2px 0 6px; }";
    style += "QToolBar { background: %1; }";
    style += "QTreeView { border: none; }";
    style += "QToolBar QToolButton { margin: 2px; }";
    QPalette themePalette = namedColorSchemePalette(ElMavenLight);
    if (isActive) {
        QColor mid = themePalette.mid().color().lighter(125);
        style = style.arg(mid.name(QColor::HexRgb));
    } else {
        QColor base = themePalette.base().color();
        style = style.arg(base.name(QColor::HexRgb));
    }
    setStyleSheet(style);
}

QWidget* TableToolBarWidgetAction::createWidget(QWidget* parent)
{
    if (btnName == "titlePeakTable") {
        td->titlePeakTable = new QLabel(parent);
        QFont font;
        font.setPixelSize(16);
        td->titlePeakTable->setFont(font);
        td->titlePeakTable->setText(
            TableDockWidget::getTitleForId(td->tableId));
        td->titlePeakTable->setStyleSheet("font-weight: bold; color: black");
        td->setWindowTitle(td->titlePeakTable->text());
        return td->titlePeakTable;
    } else if (btnName == "btnSwitchView") {
        QToolButton* btnSwitchView = new QToolButton(parent);
        btnSwitchView->setIcon(QIcon(rsrcPath + "/flip.png"));
        btnSwitchView->setToolTip(
            "Switch between \"group\" and \"peak\" views");
        connect(btnSwitchView, SIGNAL(clicked()), td, SLOT(switchTableView()));
        return btnSwitchView;
    } else if (btnName == "btnGroupCSV") {
        QToolButton* btnGroupCSV = new QToolButton(parent);
        btnGroupCSV->setIcon(QIcon(rsrcPath + "/exportcsv.png"));
        btnGroupCSV->setToolTip(tr("Export groups to spreadSheet (.csv) "));
        btnGroupCSV->setMenu(new QMenu("Export groups"));
        btnGroupCSV->setPopupMode(QToolButton::InstantPopup);

        QAction* exportSelected =
            btnGroupCSV->menu()->addAction(tr("Export selected groups"));
        connect(
            exportSelected, SIGNAL(triggered()), td, SLOT(selectedPeakSet()));
        connect(exportSelected,
                SIGNAL(triggered()),
                td,
                SLOT(exportGroupsToSpreadsheet()));

        if (!td->hasClassifiedGroups) {
            QAction* exportAll =
                btnGroupCSV->menu()->addAction(tr("Export all groups"));
            QAction* exportGood =
                btnGroupCSV->menu()->addAction(tr("Export good groups"));
            QAction* excludeBad = btnGroupCSV->menu()->addAction(
                tr("Export excluding bad groups"));
            QAction* exportBad =
                btnGroupCSV->menu()->addAction(tr("Export bad groups"));
            connect(exportAll, SIGNAL(triggered()), td, SLOT(wholePeakSet()));
            connect(exportAll,
                    SIGNAL(triggered()),
                    td->treeWidget,
                    SLOT(selectAll()));
            connect(exportAll,
                    SIGNAL(triggered()),
                    td,
                    SLOT(exportGroupsToSpreadsheet()));
            connect(exportGood, SIGNAL(triggered()), td, SLOT(goodPeaks()));
            connect(exportGood,
                    SIGNAL(triggered()),
                    td->treeWidget,
                    SLOT(selectAll()));
            connect(exportGood,
                    SIGNAL(triggered()),
                    td,
                    SLOT(exportGroupsToSpreadsheet()));

            connect(
                excludeBad, SIGNAL(triggered()), td, SLOT(excludeBadPeaks()));
            connect(excludeBad,
                    SIGNAL(triggered()),
                    td->treeWidget,
                    SLOT(selectAll()));
            connect(excludeBad,
                    SIGNAL(triggered()),
                    td,
                    SLOT(exportGroupsToSpreadsheet()));

            connect(exportBad, SIGNAL(triggered()), td, SLOT(badPeaksSet()));
            connect(exportBad,
                    SIGNAL(triggered()),
                    td->treeWidget,
                    SLOT(selectAll()));
            connect(exportBad,
                    SIGNAL(triggered()),
                    td,
                    SLOT(exportGroupsToSpreadsheet()));
        } else {
            QAction* exportAllDisplayed =
                btnGroupCSV->menu()->addAction(tr("Export displayed groups"));
            connect(exportAllDisplayed,
                    SIGNAL(triggered()),
                    td,
                    SLOT(showNotification()));
            connect(exportAllDisplayed,
                    SIGNAL(triggered()),
                    td,
                    SLOT(displayedPeakSet()));
            connect(exportAllDisplayed,
                    SIGNAL(triggered()),
                    td->treeWidget,
                    SLOT(selectAll()));
            connect(exportAllDisplayed,
                    SIGNAL(triggered()),
                    td,
                    SLOT(exportGroupsToSpreadsheet()));

            QAction* exportAllSignals =
                btnGroupCSV->menu()->addAction(tr("Export all signals"));
            connect(
                exportAllSignals, SIGNAL(triggered()), td, SLOT(goodPeakSet()));
            connect(exportAllSignals,
                    SIGNAL(triggered()),
                    td->treeWidget,
                    SLOT(selectAll()));
            connect(exportAllSignals,
                    SIGNAL(triggered()),
                    td,
                    SLOT(exportGroupsToSpreadsheet()));
        }
        return btnGroupCSV;

    } else if (btnName == "btnSaveJson") {
        QToolButton* btnSaveJson = new QToolButton(parent);
        btnSaveJson->setIcon(QIcon(rsrcPath + "/JSON.png"));
        btnSaveJson->setToolTip(tr("Export EICs to JSON (.json)"));
        connect(btnSaveJson, SIGNAL(clicked()), td, SLOT(exportJson()));
        return btnSaveJson;
    } else if (btnName == "btnScatter") {
        QToolButton* btnScatter = new QToolButton(parent);
        btnScatter->setIcon(QIcon(rsrcPath + "/scatterplot.png"));
        btnScatter->setToolTip("Show scatter-plot");
        connect(btnScatter, SIGNAL(clicked()), td, SLOT(showScatterPlot()));
        return btnScatter;
    } else if (btnName == "btnCluster") {
        QToolButton* btnCluster = new QToolButton(parent);
        btnCluster->setIcon(QIcon(rsrcPath + "/cluster.png"));
        btnCluster->setToolTip("Cluster groups");
        connect(btnCluster, SIGNAL(clicked()), td, SLOT(showClusterDialog()));
        return btnCluster;
    } else if (btnName == "btnGood") {
        QToolButton* btnGood = new QToolButton(parent);
        btnGood->setIcon(QIcon(rsrcPath + "/markgood.png"));
        btnGood->setToolTip("Mark selected group as good");
        connect(btnGood, SIGNAL(clicked()), td, SLOT(markGroupGood()));
        connect(td,
                &TableDockWidget::ghostPeakGroupSelected,
                btnGood,
                &QToolButton::setDisabled);
        return btnGood;
    } else if (btnName == "btnBad") {
        QToolButton* btnBad = new QToolButton(parent);
        btnBad->setIcon(QIcon(rsrcPath + "/markbad.png"));
        btnBad->setToolTip("Mark selected group as bad");
        connect(btnBad, SIGNAL(clicked()), td, SLOT(markGroupBad()));
        connect(td,
                &TableDockWidget::ghostPeakGroupSelected,
                btnBad,
                &QToolButton::setDisabled);
        return btnBad;
    } else if (btnName == "btnUnmark") {
        QToolButton* btnUnmark = new QToolButton(parent);
        btnUnmark->setIcon(QIcon(rsrcPath + "/unmark.png"));
        btnUnmark->setToolTip("Unmark selected group from good/bad");
        connect(btnUnmark, SIGNAL(clicked()), td, SLOT(unmarkGroup()));
        connect(td,
                &TableDockWidget::ghostPeakGroupSelected,
                btnUnmark,
                &QToolButton::setDisabled);
        return btnUnmark;
    } else if (btnName == "btnDelete") {
        QToolButton* btnDelete = new QToolButton(parent);
        btnDelete->setIcon(QIcon(rsrcPath + "/Delete Group.png"));
        btnDelete->setToolTip("Delete group");
        connect(btnDelete, SIGNAL(clicked()), td, SLOT(deleteSelectedItems()));
        return btnDelete;
    } else if (btnName == "btnPDF") {
        QToolButton* btnPDF = new QToolButton(parent);
        btnPDF->setIcon(QIcon(rsrcPath + "/PDF.png"));
        btnPDF->setToolTip("Generate PDF report");
        btnPDF->setMenu(new QMenu("Export Groups"));
        btnPDF->setPopupMode(QToolButton::InstantPopup);

        QAction* exportSelected =
            btnPDF->menu()->addAction(tr("Export selected groups"));

        connect(
            exportSelected, SIGNAL(triggered()), td, SLOT(selectedPeakSet()));
        connect(
            exportSelected, SIGNAL(triggered()), td, SLOT(printPdfReport()));

        if (!td->hasClassifiedGroups) {
            QAction* exportAll =
                btnPDF->menu()->addAction(tr("Export all groups"));
            connect(exportAll, SIGNAL(triggered()), td, SLOT(wholePeakSet()));
            connect(exportAll, SIGNAL(triggered()), td, SLOT(printPdfReport()));
        } else {
            QAction* exportDisplayedGroups =
                btnPDF->menu()->addAction(tr("Export displayed groups"));
            connect(exportDisplayedGroups,
                    SIGNAL(triggered()),
                    td->treeWidget,
                    SLOT(selectAll()));
            connect(exportDisplayedGroups,
                    SIGNAL(triggered()),
                    td,
                    SLOT(displayedPeakSet()));
            connect(exportDisplayedGroups,
                    SIGNAL(triggered()),
                    td,
                    SLOT(printPdfReport()));

            QAction* exportGoodSignals =
                btnPDF->menu()->addAction(tr("Export all signals"));
            connect(exportGoodSignals,
                    SIGNAL(triggered()),
                    td,
                    SLOT(goodPeakSet()));
            connect(exportGoodSignals,
                    SIGNAL(triggered()),
                    td,
                    SLOT(printPdfReport()));
        }
        return btnPDF;
    } else if (btnName == "btnX") {
        QToolButton* btnX = new QToolButton(parent);
        btnX->setIcon(QIcon(rsrcPath + "/closeWidget.png"));
        connect(btnX, SIGNAL(clicked()), td, SLOT(showDeletionDialog()));
        return btnX;
    } else if (btnName == "btnMin") {
        QToolButton* btnMin = new QToolButton(parent);
        btnMin->setIcon(QIcon(rsrcPath + "/minimizeWidget.png"));
        connect(btnMin, SIGNAL(clicked()), td, SLOT(hide()));
        return btnMin;
    } else if (btnName == "btnSaveSpectral") {
        QToolButton* btnSaveSpectral = new QToolButton(parent);
        btnSaveSpectral->setIcon(QIcon(rsrcPath + "/exportmsp.png"));
        btnSaveSpectral->setToolTip(
            tr("Export table as spectral library (.msp)"));
        connect(btnSaveSpectral,
                &QToolButton::clicked,
                td,
                &TableDockWidget::exportSpectralLib);
        return btnSaveSpectral;
    } else if (btnName == "btnEditPeakGroup") {
        QToolButton* btnEditPeakGroup = new QToolButton(parent);
        btnEditPeakGroup->setIcon(QIcon(rsrcPath + "/editPeakGroup.png"));
        btnEditPeakGroup->setToolTip(
            "Edit the peak area or baseline for "
            "individual peaks of the selected peak-group.");
        connect(btnEditPeakGroup,
                &QToolButton::clicked,
                td,
                &TableDockWidget::editSelectedPeakGroup);
        connect(td,
                &TableDockWidget::ghostPeakGroupSelected,
                btnEditPeakGroup,
                &QToolButton::setDisabled);
        return btnEditPeakGroup;
    } else if (btnName == "btnSettingsLog") {
        QToolButton* btnSettingsLog = new QToolButton(parent);
        btnSettingsLog->setIcon(QIcon(rsrcPath + "/settingsLog.png"));
        btnSettingsLog->setToolTip(
            "Show a log of settings that were used while "
            "integrating the selected peak-group.");
        connect(btnSettingsLog,
                &QToolButton::clicked,
                td,
                &TableDockWidget::showIntegrationSettings);
        connect(td,
                &TableDockWidget::ghostPeakGroupSelected,
                btnSettingsLog,
                &QToolButton::setDisabled);
        return btnSettingsLog;
    } else {
        return nullptr;
    }
}

PeakTableDockWidget::PeakTableDockWidget(MainWindow* mw,
                                         const QString& tableTitle,
                                         bool classifiedGroups)
    : TableDockWidget(mw)
{
    _mainwindow = mw;
    this->hasClassifiedGroups = classifiedGroups;
    auto lastId = lastTableId();
    tableId = ++lastId;
    setTitleForId(tableId, tableTitle);

    toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24));

    QWidgetAction* titlePeakTable =
        new TableToolBarWidgetAction(toolBar, this, "titlePeakTable");
    QWidgetAction* btnSwitchView =
        new TableToolBarWidgetAction(toolBar, this, "btnSwitchView");
    QWidgetAction* btnGroupCSV =
        new TableToolBarWidgetAction(toolBar, this, "btnGroupCSV");
    QWidgetAction* btnSaveJson =
        new TableToolBarWidgetAction(toolBar, this, "btnSaveJson");
    QWidgetAction* btnSaveSpectral =
        new TableToolBarWidgetAction(toolBar, this, "btnSaveSpectral");
    QWidgetAction* btnScatter =
        new TableToolBarWidgetAction(toolBar, this, "btnScatter");
    QWidgetAction* btnGood =
        new TableToolBarWidgetAction(toolBar, this, "btnGood");
    QWidgetAction* btnBad =
        new TableToolBarWidgetAction(toolBar, this, "btnBad");
    QWidgetAction* btnUnmark =
        new TableToolBarWidgetAction(toolBar, this, "btnUnmark");
    QWidgetAction* btnDelete =
        new TableToolBarWidgetAction(toolBar, this, "btnDelete");
    QWidgetAction* btnEditPeakGroup =
        new TableToolBarWidgetAction(toolBar, this, "btnEditPeakGroup");
    QWidgetAction* btnSettingsLog =
        new TableToolBarWidgetAction(toolBar, this, "btnSettingsLog");
    QWidgetAction* btnPDF =
        new TableToolBarWidgetAction(toolBar, this, "btnPDF");
    QWidgetAction* btnX = new TableToolBarWidgetAction(toolBar, this, "btnX");
    QWidgetAction* btnMin =
        new TableToolBarWidgetAction(toolBar, this, "btnMin");

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    toolBar->addAction(titlePeakTable);
    toolBar->addSeparator();

    if (hasClassifiedGroups) {
        MultiSelectComboBox* legend = new MultiSelectComboBox(this);
        setLegend(legend);
        connect(legend,
                &MultiSelectComboBox::selectionChanged,
                this,
                &TableDockWidget::filterForSelectedLabels);

        toolBar->addWidget(new QLabel("Labels"));
        toolBar->addWidget(legend);
        toolBar->addSeparator();
    }
    toolBar->addAction(btnSwitchView);
    toolBar->addSeparator();

    toolBar->addAction(btnGood);
    toolBar->addAction(btnBad);
    toolBar->addAction(btnUnmark);
    toolBar->addAction(btnDelete);

    toolBar->addSeparator();
    toolBar->addAction(btnEditPeakGroup);
    toolBar->addAction(btnSettingsLog);

    toolBar->addSeparator();
    toolBar->addAction(btnScatter);

    toolBar->addSeparator();
    toolBar->addAction(btnPDF);
    toolBar->addAction(btnGroupCSV);
    toolBar->addAction(btnSaveJson);
    toolBar->addAction(btnSaveSpectral);
    toolBar->addWidget(spacer);
    toolBar->addAction(btnMin);
    toolBar->addAction(btnX);

    setTitleBarWidget(toolBar);

    connect(this,
            &PeakTableDockWidget::unSetFromEicWidget,
            _mainwindow->getEicWidget(),
            &EicWidget::unSetPeakTableGroup);

    deletionDialog = new PeakTableDeletionDialog(this);
}

PeakTableDockWidget::~PeakTableDockWidget()
{
    toolBar->clear();
    delete toolBar;
}

void PeakTableDockWidget::destroy()
{
    TableDockWidget::deleteAll(false);
    if (_mainwindow->getCorrelationTable()->currentTable() == this) {
        _mainwindow->getCorrelationTable()->clearCorrelation();
        _mainwindow->getCorrelationTable()->setVisible(false);
        _mainwindow->getCorrelationTable()->setCurrentTable(nullptr);
    }
    cleanUp();
    deleteLater();
    _mainwindow->removePeaksTable(this);
}

void PeakTableDockWidget::deleteAll()
{
    auto allDeleted = TableDockWidget::deleteAll();
    if (allDeleted) {
        cleanUp();
        deleteLater();
        _mainwindow->removePeaksTable(this);
    }
}

void PeakTableDockWidget::cleanUp()
{
    if (treeWidget->currentItem())
        emit unSetFromEicWidget(groupForItem(treeWidget->currentItem()));
    _mainwindow->ligandWidget->resetColor();
}

void PeakTableDockWidget::showDeletionDialog()
{
    deletionDialog->show();
}

void PeakTableDockWidget::showAllGroups()
{
    if (topLevelGroupCount() == 0) {
        setVisible(false);
        _mainwindow->ligandWidget->resetColor();
        _mainwindow->removePeaksTable(this);
        noPeakFound();
        this->deleteLater();
        return;
    }
    TableDockWidget::showAllGroups();
}

BookmarkTableDockWidget::BookmarkTableDockWidget(MainWindow* mw)
    : TableDockWidget(mw)
{
    _mainwindow = mw;
    tableId = 0;
    setTitleForId(0);

    toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24));
    btnMerge = new QToolButton(toolBar);
    btnMerge->setIcon(QIcon(rsrcPath + "/merge.png"));
    btnMerge->setToolTip("Merge bookmarks with peak-table");
    btnMergeMenu = new QMenu("Merge groups");
    btnMerge->setMenu(btnMergeMenu);
    btnMerge->setPopupMode(QToolButton::InstantPopup);
    connect(btnMergeMenu, SIGNAL(aboutToShow()), SLOT(showMergeTableOptions()));
    connect(btnMergeMenu,
            SIGNAL(triggered(QAction*)),
            SLOT(mergeGroupsIntoPeakTable(QAction*)));

    QWidgetAction* titlePeakTable =
        new TableToolBarWidgetAction(toolBar, this, "titlePeakTable");
    QWidgetAction* btnSwitchView =
        new TableToolBarWidgetAction(toolBar, this, "btnSwitchView");
    QWidgetAction* btnGroupCSV =
        new TableToolBarWidgetAction(toolBar, this, "btnGroupCSV");
    QWidgetAction* btnSaveJson =
        new TableToolBarWidgetAction(toolBar, this, "btnSaveJson");
    QWidgetAction* btnSaveSpectral =
        new TableToolBarWidgetAction(toolBar, this, "btnSaveSpectral");
    QWidgetAction* btnScatter =
        new TableToolBarWidgetAction(toolBar, this, "btnScatter");
    QWidgetAction* btnGood =
        new TableToolBarWidgetAction(toolBar, this, "btnGood");
    QWidgetAction* btnBad =
        new TableToolBarWidgetAction(toolBar, this, "btnBad");
    QWidgetAction* btnUnmark =
        new TableToolBarWidgetAction(toolBar, this, "btnUnmark");
    QWidgetAction* btnDelete =
        new TableToolBarWidgetAction(toolBar, this, "btnDelete");
    QWidgetAction* btnEditPeakGroup =
        new TableToolBarWidgetAction(toolBar, this, "btnEditPeakGroup");
    QWidgetAction* btnSettingsLog =
        new TableToolBarWidgetAction(toolBar, this, "btnSettingsLog");
    QWidgetAction* btnPDF =
        new TableToolBarWidgetAction(toolBar, this, "btnPDF");
    QWidgetAction* btnMin =
        new TableToolBarWidgetAction(toolBar, this, "btnMin");

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    toolBar->addAction(titlePeakTable);

    toolBar->addSeparator();
    toolBar->addAction(btnSwitchView);

    toolBar->addSeparator();
    toolBar->addWidget(btnMerge);

    toolBar->addSeparator();
    toolBar->addAction(btnGood);
    toolBar->addAction(btnBad);
    toolBar->addAction(btnUnmark);
    toolBar->addAction(btnDelete);

    toolBar->addSeparator();
    toolBar->addAction(btnEditPeakGroup);
    toolBar->addAction(btnSettingsLog);

    toolBar->addSeparator();
    toolBar->addAction(btnScatter);

    toolBar->addSeparator();
    toolBar->addAction(btnPDF);
    toolBar->addAction(btnGroupCSV);
    toolBar->addAction(btnSaveJson);
    toolBar->addAction(btnSaveSpectral);
    toolBar->addWidget(spacer);
    toolBar->addAction(btnMin);

    setTitleBarWidget(toolBar);
}

BookmarkTableDockWidget::~BookmarkTableDockWidget()
{
    toolBar->clear();
    delete toolBar;
}

void BookmarkTableDockWidget::showMergeTableOptions()
{
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

void BookmarkTableDockWidget::showMsgBox(bool check, int tableNo)
{
    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setStandardButtons(QMessageBox::Ok);

    if (check) {
        msgBox->setIconPixmap(QPixmap(rsrcPath + "/success.png"));
        msgBox->setText("Successfully Merged from Bookmark Table to Table "
                        + QString::number(tableNo) + "   ");
    } else {
        msgBox->setIcon(QMessageBox::Warning);
        if (tableNo == -1)
            msgBox->setText("Error while merging");
        else
            msgBox->setText("Error while merging from Bookmark Table to Table "
                            + QString::number(tableNo) + "   ");
    }

    msgBox->open();
}

void BookmarkTableDockWidget::mergeGroupsIntoPeakTable(QAction* action)
{
    QList<QPointer<TableDockWidget>> peaksTableList =
        _mainwindow->getPeakTableList();
    int j = mergeAction.value(action, -1);

    // check if action exists
    if (j == -1) {
        showMsgBox(false, j);
        return;
    }

    // return if no bookmarked groups or peak tables
    if (_topLevelGroups.isEmpty() || peaksTableList.isEmpty()) {
        showMsgBox(true, j);
        return;
    }

    // find table to merge with
    TableDockWidget* peakTable = nullptr;
    for (auto table : peaksTableList) {
        if (table->tableId == j) {
            peakTable = table;
            break;
        }
    }

    // return if peak table not found
    if (peakTable == nullptr) {
        showMsgBox(false, j);
        return;
    }

    int initialSize = peakTable->topLevelGroupCount();
    int finalSize = _topLevelGroups.size() + initialSize;
    for (auto group : _topLevelGroups)
        peakTable->addPeakGroup(group.get());

    deleteAll(false);
    peakTable->showAllGroups();
    showAllGroups();

    bool merged = true;

    if (finalSize == peakTable->topLevelGroupCount())
        merged = true;
    else
        merged = false;

    showMsgBox(merged, j);
    QString status = merged ? "Success" : "Failure";
    _mainwindow->getAnalytics()->hitEvent(
        "Bookmark Table", "Merge Table", status);
}

void BookmarkTableDockWidget::showSameGroup(
    QPair<int, int> sameMzRtGroupIndexHash)
{
    QMessageBox prompt(_mainwindow);
    prompt.setWindowTitle("Bookmarking possible duplicate");

    QString compoundList = "";
    for (QString groupName : sameMzRtGroups[sameMzRtGroupIndexHash])
        compoundList += "<li>" + groupName;
    auto htmlText = QString(
                        "<p>The table already contains one or more "
                        "peak-groups having m/z and RT values same as the "
                        "group being currently bookmarked:</p>"
                        "<ul>%1</ul>")
                        .arg(compoundList);
    htmlText += "<p>Do you want to add it anyway?</p>";
    prompt.setText(htmlText);

    auto discardButton = prompt.addButton("Discard", QMessageBox::RejectRole);
    auto addButton = prompt.addButton("Add", QMessageBox::AcceptRole);
    prompt.setDefaultButton(discardButton);

    prompt.exec();
    if (prompt.clickedButton() == addButton) {
        addSameMzRtGroup = true;
    } else {
        addSameMzRtGroup = false;
    }
}

bool BookmarkTableDockWidget::hasPeakGroup(PeakGroup* group)
{
    int intMz = group->meanMz * 1e5;
    int intRt = group->meanRt * 1e5;
    QPair<int, int> sameMzRtGroupIndexHash(intMz, intRt);
    QString compoundName = QString::fromStdString(group->getName());

    if (topLevelGroupCount() == 0
        || sameMzRtGroups[sameMzRtGroupIndexHash].size() == 0) {
        /**
         * add this group corresponding compound name to list of string which
         * all share same mz and rt value. Both mz and rt are hashed in
         * sameMzRtGroupIndexHash.
         */
        sameMzRtGroups[sameMzRtGroupIndexHash].append(compoundName);
    }

    for (auto g : _topLevelGroups) {
        if (mzUtils::almostEqual(group->meanMz, g->meanMz)
            && mzUtils::almostEqual(group->meanRt, g->meanRt)) {
            addSameMzRtGroup = false;
            showSameGroup(sameMzRtGroupIndexHash);

            if (addSameMzRtGroup) {
                /**
                 * if user pressed <save> button <addSameMzRtGroup> will be set
                 * to true otherwise false. if it is true, this groups
                 * corresponding compound name will we saved by an index of
                 * sameMzRtGroupIndexHash to show all these string next time if
                 * a group with same rt and mz is encountered.
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

void BookmarkTableDockWidget::_removeGroupHash(PeakGroup* group)
{
    /**
     * delete name of compound associated with this group stored in
     * <sameMzRtGroups> with given mz and rt
     */
    int intMz = group->meanMz * 1e5;
    int intRt = group->meanRt * 1e5;
    QPair<int, int> sameMzRtGroupIndexHash(intMz, intRt);
    QString compoundName = QString::fromStdString(group->getName());
    if (sameMzRtGroups[sameMzRtGroupIndexHash].contains(compoundName)) {
        for (int i = 0; i < sameMzRtGroups[sameMzRtGroupIndexHash].size();
             ++i) {
            if (sameMzRtGroups[sameMzRtGroupIndexHash][i] == compoundName) {
                sameMzRtGroups[sameMzRtGroupIndexHash].removeAt(i);
                break;
            }
        }
    }
}

void BookmarkTableDockWidget::deleteGroup(PeakGroup* group)
{
    _removeGroupHash(group);
    TableDockWidget::deleteGroup(group);
}

void BookmarkTableDockWidget::deleteSelectedItems()
{
    int topLevelItemsBeingDeleted = 0;
    for (auto item : treeWidget->selectedItems()) {
        if (item->parent() == nullptr)
            ++topLevelItemsBeingDeleted;
    }

    // checks if the selected item count is same as the no. of top-level
    // groups in the table.
    if (topLevelItemsBeingDeleted == topLevelGroupCount()) {
        deleteAll();
        return;
    }

    for (auto item : treeWidget->selectedItems()) {
        auto group = groupForItem(item);
        _removeGroupHash(group.get());
    }
    TableDockWidget::deleteSelectedItems();
}

void BookmarkTableDockWidget::deleteAll(bool askConfirmation)
{
    bool allDeleted = TableDockWidget::deleteAll(askConfirmation);
    if (allDeleted)
        sameMzRtGroups.clear();
}

void BookmarkTableDockWidget::markGroupGood()
{
    setGroupLabel('g');
    auto currentGroups = getSelectedGroups();
    if (currentGroups.empty())
        return;
    showNextGroup();
    _mainwindow->autoSaveSignal(currentGroups);
}

void BookmarkTableDockWidget::markGroupBad()
{
    setGroupLabel('b');
    auto currentGroups = getSelectedGroups();
    if (currentGroups.empty())
        return;
    showNextGroup();
    _mainwindow->autoSaveSignal(currentGroups);
}

ScatterplotTableDockWidget::ScatterplotTableDockWidget(MainWindow* mw)
    : TableDockWidget(mw)
{
    _mainwindow = mw;
    tableId = -1;
    setTitleForId(tableId);

    toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24));

    QWidgetAction* titlePeakTable =
        new TableToolBarWidgetAction(toolBar, this, "titlePeakTable");
    QWidgetAction* btnSwitchView =
        new TableToolBarWidgetAction(toolBar, this, "btnSwitchView");
    QWidgetAction* btnGroupCSV =
        new TableToolBarWidgetAction(toolBar, this, "btnGroupCSV");
    QWidgetAction* btnSaveJson =
        new TableToolBarWidgetAction(toolBar, this, "btnSaveJson");
    QWidgetAction* btnSaveSpectral =
        new TableToolBarWidgetAction(toolBar, this, "btnSaveSpectral");
    QWidgetAction* btnGood =
        new TableToolBarWidgetAction(toolBar, this, "btnGood");
    QWidgetAction* btnBad =
        new TableToolBarWidgetAction(toolBar, this, "btnBad");
    QWidgetAction* btnUnmark =
        new TableToolBarWidgetAction(toolBar, this, "btnUnmark");
    QWidgetAction* btnDelete =
        new TableToolBarWidgetAction(toolBar, this, "btnDelete");
    QWidgetAction* btnEditPeakGroup =
        new TableToolBarWidgetAction(toolBar, this, "btnEditPeakGroup");
    QWidgetAction* btnSettingsLog =
        new TableToolBarWidgetAction(toolBar, this, "btnSettingsLog");
    QWidgetAction* btnPDF =
        new TableToolBarWidgetAction(toolBar, this, "btnPDF");
    QWidgetAction* btnMin =
        new TableToolBarWidgetAction(toolBar, this, "btnMin");

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    toolBar->addAction(titlePeakTable);

    toolBar->addSeparator();
    toolBar->addAction(btnSwitchView);

    toolBar->addSeparator();
    toolBar->addAction(btnGood);
    toolBar->addAction(btnBad);
    toolBar->addAction(btnUnmark);
    toolBar->addAction(btnDelete);

    toolBar->addSeparator();
    toolBar->addAction(btnEditPeakGroup);
    toolBar->addAction(btnSettingsLog);

    toolBar->addSeparator();

    toolBar->addSeparator();
    toolBar->addAction(btnPDF);
    toolBar->addAction(btnGroupCSV);
    toolBar->addAction(btnSaveJson);
    toolBar->addAction(btnSaveSpectral);
    toolBar->addWidget(spacer);
    toolBar->addAction(btnMin);

    setTitleBarWidget(toolBar);
}

ScatterplotTableDockWidget::~ScatterplotTableDockWidget()
{
    toolBar->clear();
    delete toolBar;
}

void ScatterplotTableDockWidget::setupPeakTable()
{
    QStringList colNames;

    // Add common columns to the table
    colNames << "Label";
    colNames << "#";
    colNames << "ID";
    colNames << "Observed m/z";
    colNames << "Expected m/z";
    colNames << "RT";

    if (viewType == groupView) {
        // Add group view columns to the table
        colNames << "Δ RT";
        colNames << "# peaks";
        colNames << "# good";
        colNames << "Max width";
        colNames << "Max AreaTop";
        colNames << "Max S/N";
        colNames << "Max quality";
        colNames << "MS2 score";
        colNames << "#MS2 events";
        colNames << "Classification Probability";  // TODO: add this column
                                                   // conditionally
        colNames << "Rank";

        // add scatterplot table columns
        colNames << "Ratio change";
        colNames << "p-value";
    } else if (viewType == peakView) {
        vector<mzSample*> vsamples = _mainwindow->getVisibleSamples();
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

RowData::RowData()
{
    tableId = 0;
    parentIndex = 0;
    childType = ChildType::None;
    childIndex = 0;
}

bool RowData::operator==(const RowData& b) const
{
    return (tableId == b.tableId && parentIndex == b.parentIndex
            && childType == b.childType && childIndex == b.childIndex);
}

QDataStream& operator<<(QDataStream& stream, const RowData& rowData)
{
    int childType = static_cast<int>(rowData.childType);
    stream << rowData.tableId;
    stream << rowData.parentIndex;
    stream << childType;
    stream << rowData.childIndex;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, RowData& rowData)
{
    int childType = 0;
    stream >> rowData.tableId;
    stream >> rowData.parentIndex;
    stream >> childType;
    stream >> rowData.childIndex;
    rowData.childType = static_cast<RowData::ChildType>(childType);
    return stream;
}

RowData PeakGroupTreeWidget::dragData = RowData();
bool PeakGroupTreeWidget::moveInProgress = false;

PeakGroupTreeWidget::PeakGroupTreeWidget(TableDockWidget* parent)
    : QTreeWidget(parent)
{
    _sortOrder = Qt::AscendingOrder;
    table = parent;
    setDragEnabled(true);
    viewport()->setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    qRegisterMetaTypeStreamOperators<RowData>("RowData");
}

void PeakGroupTreeWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (!moveInProgress) {
        if (selectedItems().size() != 1)
            return;

        auto var = currentItem()->data(0, Qt::UserRole);
        dragData = var.value<RowData>();
        moveInProgress = true;
    }
    QTreeWidget::dragEnterEvent(event);
}

void PeakGroupTreeWidget::dropEvent(QDropEvent* event)
{
    QModelIndex droppedIndex = indexAt(event->pos());
    if (!droppedIndex.isValid()) {
        moveInProgress = false;
        return;
    }

    auto dropPosition = dropIndicatorPosition();
    if (dropPosition == QAbstractItemView::BelowItem
        || dropPosition == QAbstractItemView::AboveItem) {
        // exit since we do not care for these types of drop events
        moveInProgress = false;
        return;
    }

    // we do not allow dragging of parent items
    if (dragData.childType == RowData::ChildType::None) {
        moveInProgress = false;
        return;
    }

    MainWindow* mw = table->getMainWindow();
    TableDockWidget* sourceTable = mw->tableForTableId(dragData.tableId);
    if (sourceTable == nullptr) {
        moveInProgress = false;
        return;
    }

    auto tableGroups = sourceTable->getGroups();
    shared_ptr<PeakGroup> originalParent = tableGroups.at(dragData.parentIndex);
    if (originalParent == nullptr) {
        moveInProgress = false;
        return;
    }

    shared_ptr<PeakGroup> child = nullptr;
    if (dragData.childType == RowData::ChildType::Isotope) {
        child = originalParent->childIsotopes().at(dragData.childIndex);
    } else if (dragData.childType == RowData::ChildType::Adduct) {
        child = originalParent->childAdducts().at(dragData.childIndex);
    }
    if (child == nullptr) {
        moveInProgress = false;
        return;
    }

    QTreeWidgetItem* item = itemFromIndex(droppedIndex);
    if (item->parent() != nullptr)
        item = item->parent();

    shared_ptr<PeakGroup> newParent = table->groupForItem(item);
    if (child->getCompound() != newParent->getCompound()) {
        moveInProgress = false;
        return;
    }

    QTreeWidget::dropEvent(event);
    originalParent->removeChild(child.get());
    shared_ptr<PeakGroup> newChild = nullptr;
    if (child->isIsotope()) {
        newChild = newParent->addIsotopeChild(*child);
    } else if (child->isAdduct()) {
        newChild = newParent->addAdductChild(*child);
    }
    newChild->setGroupId(table->_nextGroupId++);
    newParent->setGroupId(newParent->groupId());

    // update the new parent
    table->refreshParentItem(item);

    // update the old parent
    QTreeWidgetItemIterator it(sourceTable->treeWidget);
    while (*it) {
        QTreeWidgetItem* parentItem = *(it++);
        if (parentItem->parent() != nullptr)
            continue;

        shared_ptr<PeakGroup> group = sourceTable->groupForItem(parentItem);
        if (group == originalParent) {
            if (group->childIsotopeCount() == 0
                && group->childAdductsCount() == 0) {
                QSet<QTreeWidgetItem*> itemsToDelete;
                itemsToDelete.insert(parentItem);
                sourceTable->_deleteItemsAndGroups(itemsToDelete);
                sourceTable->updateCompoundWidget();
            } else {
                sourceTable->refreshParentItem(parentItem);
            }
            break;
        }
    }

    QTreeWidgetItemIterator it2(table->treeWidget);
    while (*it2) {
        item = *(it2++);

        auto currentGroup = table->groupForItem(item);
        if (currentGroup == newParent && !item->isExpanded())
            item->setExpanded(true);

        if (currentGroup == newChild) {
            table->treeWidget->scrollToItem(
                item, QAbstractItemView::PositionAtCenter);

            // TODO: should also be selected, but for some reason the dropped
            // item disappears instead when selected, Qt bug?
            // table->treeWidget->setCurrentItem(item);

            break;
        }
    }

    mw->autoSaveSignal({originalParent, newParent});
    QApplication::processEvents();
    moveInProgress = false;
}

Qt::DropActions PeakGroupTreeWidget::supportedDropActions() const
{
    return Qt::MoveAction;
}

void PeakGroupTreeWidget::paintEvent(QPaintEvent* event)
{
    auto dropPosition = dropIndicatorPosition();
    setDropIndicatorShown(dropPosition != QAbstractItemView::BelowItem
                          && dropPosition != QAbstractItemView::AboveItem);
    QTreeWidget::paintEvent(event);
    setDropIndicatorShown(true);
}

void PeakGroupTreeWidget::sortByColumn(int column, Qt::SortOrder order)
{
    _sortOrder = order;
    QTreeWidget::sortItems(column, order);
}
