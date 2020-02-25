#ifndef CORRELATIONTABLE_H
#define CORRELATIONTABLE_H

#include <QDockWidget>

class PeakGroup;
class TableDockWidget;

namespace Ui {
class CorrelationTable;
}

class CorrelationTable : public QDockWidget
{
    Q_OBJECT

public:
    explicit CorrelationTable(QWidget *parent = nullptr);
    ~CorrelationTable();

    void setReferencePeakGroup(PeakGroup* group);
    void addCorrelatedPeakGroup(PeakGroup* group, float correlation);
    void clearCorrelation();
    void setCurrentTable(TableDockWidget* table) { _table = table; }
    TableDockWidget* currentTable() { return _table; }
    void selectGroupId(int groupId);

signals:
    void groupIdSelected(int);

private:
    Ui::CorrelationTable *ui;
    PeakGroup* _referenceGroup;
    map<PeakGroup*, float> _correlatedGroups;
    TableDockWidget* _table;

    void _populateTable();
};

#endif // CORRELATIONTABLE_H
