#include "correlationtable.h"
#include "PeakGroup.h"
#include "numeric_treewidgetitem.h"
#include "ui_correlationtable.h"

CorrelationTable::CorrelationTable(QWidget *parent) :
      QDockWidget(parent),
      ui(new Ui::CorrelationTable)
{
    ui->setupUi(this);
    _referenceGroup = nullptr;

    connect(ui->treeWidget,
            &QTreeWidget::itemSelectionChanged,
            [this] {
                int currentGroupId =
                    ui->treeWidget->currentItem()->text(0).toInt();
                emit groupIdSelected(currentGroupId);
            });
}

CorrelationTable::~CorrelationTable()
{
    if (_referenceGroup != nullptr)
        _referenceGroup = nullptr;
    for (auto elem : _correlatedGroups)
        delete elem.first;

    delete ui;
}

void CorrelationTable::setReferencePeakGroup(PeakGroup* group)
{
    if (group == nullptr)
        return;

    clearCorrelation();
    _referenceGroup = new PeakGroup(*group);
}

void CorrelationTable::addCorrelatedPeakGroup(PeakGroup* group,
                                              float correlation)
{
    if (group == nullptr)
        return;

    auto groupCopy = new PeakGroup(*group);
    _correlatedGroups[groupCopy] = correlation;
    _populateTable();
}

void CorrelationTable::clearCorrelation()
{
    if (_referenceGroup != nullptr)
        _referenceGroup = nullptr;
    for (auto elem : _correlatedGroups)
        delete elem.first;

    _correlatedGroups.clear();
    _populateTable();
}

void CorrelationTable::selectGroupId(int groupId)
{
    auto wasBlocked = ui->treeWidget->blockSignals(true);
    QTreeWidgetItemIterator itr(ui->treeWidget);
    while (*itr) {
        auto *item = (*itr);
        if (item->text(0).toInt() == groupId) {
            ui->treeWidget->setCurrentItem(item);
            ui->treeWidget->scrollTo(ui->treeWidget->currentIndex(),
                                     QAbstractItemView::PositionAtCenter);
            break;
        }
        ++itr;
    }
    ui->treeWidget->blockSignals(wasBlocked);
}

void CorrelationTable::_populateTable()
{
    ui->treeWidget->clear();
    if (_correlatedGroups.empty() || _referenceGroup == nullptr)
        return;

    setWindowTitle(QString("Correlated Metabolites: %1")
                       .arg(_referenceGroup->getName().c_str()));
    ui->treeWidget->setUpdatesEnabled(false);
    ui->treeWidget->setSortingEnabled(false);

    ui->treeWidget->setHeaderLabels(QStringList() << "Group ID"
                                                  << "Name"
                                                  << "Δ m/z"
                                                  << "% Correlation");

    // lambda that adds items to the table based on the values provided
    auto addItemToTable = [this](int groupId,
                                 string name,
                                 float delMz,
                                 float correlation,
                                 bool isReference = false) {
        QString item1 = QString::number(groupId);
        QString item2 = QString::fromStdString(name);
        QString item3 = QString::number(delMz, 'f', 4);
        QString item4 = QString::number(correlation * 100.0f, 'f', 2);
        QStringList rowItems = QStringList() << item1
                                             << item2
                                             << item3
                                             << item4;
        auto item = new NumericTreeWidgetItem(ui->treeWidget, rowItems);
        item->setTextAlignment(0, Qt::AlignRight);
        item->setTextAlignment(2, Qt::AlignRight);
        item->setTextAlignment(3, Qt::AlignRight);
        if (isReference) {
            item->setBackground(0, QBrush(QColor(255, 200, 0, 100)));
            item->setBackground(1, QBrush(QColor(255, 200, 0, 100)));
            item->setBackground(2, QBrush(QColor(255, 200, 0, 100)));
            item->setBackground(3, QBrush(QColor(255, 200, 0, 100)));
        }
        ui->treeWidget->addTopLevelItem(item);
    };

    addItemToTable(_referenceGroup->groupId,
                   _referenceGroup->getName(),
                   0.0f,
                   1.0f,
                   true);

    for (auto& elem : _correlatedGroups) {
        auto& group = elem.first;
        auto correlation = elem.second;
        auto delMz = abs(_referenceGroup->meanMz - group->meanMz);
        addItemToTable(group->groupId, group->getName(), delMz, correlation);
    }

    ui->treeWidget->sortByColumn(0, Qt::AscendingOrder);
    ui->treeWidget->setSortingEnabled(true);
    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->header()->setStretchLastSection(true);
    ui->treeWidget->setUpdatesEnabled(true);
    ui->treeWidget->update();
}
