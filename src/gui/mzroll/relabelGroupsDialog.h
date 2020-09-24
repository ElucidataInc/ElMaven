#ifndef RELABELGROUPSDIALOG_h
#define RELABELGROUPSDIALOG_h

#include "ui_relabelgroupsdialog.h"
#include "superSlider.h"
#include "tabledockwidget.h"
#include "stable.h"

class TableDockWidget;
class RangeSlider;
class MultiSelectComboBox;

class RelabelGroupsDialog : public QDialog, public Ui_Dialog
{   
    Q_OBJECT
    
    private:
        TableDockWidget* _tabledockWidget;
        RangeSlider* _slider;
        float badGroupLimit;
        float maybeGoodGroupLimit;

    public:
        RelabelGroupsDialog() {};
        RelabelGroupsDialog(TableDockWidget * tabledockWidget);
        ~RelabelGroupsDialog() {};

    public Q_SLOTS:
        void updateCurationParameter(int, int);
        void closeEvent(QCloseEvent* event);
        void handleRelabel();
};

#endif