#include "mainwindow.h"
#include "mzSample.h"
#include "numeric_treewidgetitem.h"
#include "peakeditor.h"
#include "ui_peakeditor.h"

PeakEditor::PeakEditor(MainWindow *parent) :
      QDialog(parent),
      ui(new Ui::PeakEditor)
{
    ui->setupUi(this);
}

PeakEditor::~PeakEditor()
{
    delete ui;
}

void PeakEditor::setPeakGroup(PeakGroup *group)
{
    _populateSampleList(group);
}

void PeakEditor::_populateSampleList(PeakGroup* group)
{
    ui->sampleList->clear();
    for (mzSample* sample : group->samples) {
        auto item = new NumericTreeWidgetItem(ui->sampleList, 0);
        item->setText(0, QString::fromStdString(sample->getSampleName()));
        ui->sampleList->addTopLevelItem(item);
    }
}
