#ifndef CORRELATIONTABLE_H
#define CORRELATIONTABLE_H

#include <QDockWidget>

class PeakGroup;

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

private:
    Ui::CorrelationTable *ui;
    PeakGroup* _referenceGroup;
    map<PeakGroup*, float> _correlatedGroups;

    void _populateTable();
};

#endif // CORRELATIONTABLE_H
