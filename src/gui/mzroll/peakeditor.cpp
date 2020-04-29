#include "Compound.h"
#include "gallerywidget.h"
#include "globals.h"
#include "mainwindow.h"
#include "mzSample.h"
#include "numeric_treewidgetitem.h"
#include "peakeditor.h"
#include "Scan.h"
#include "ui_peakeditor.h"

PeakEditor::PeakEditor(MainWindow *parent) :
      QDialog(parent),
      ui(new Ui::PeakEditor),
      _mw(parent)
{
    ui->setupUi(this);

    _gallery = new GalleryWidget(this);
    ui->graphicsLayout->addWidget(_gallery);

    connect(_gallery,
            &GalleryWidget::plotIndexChanged,
            [this](int index) {
                disconnect(ui->sampleList,
                           &QTreeWidget::currentItemChanged,
                           this,
                           &PeakEditor::_selectionChanged);
                if (ui->sampleList->topLevelItemCount() == 0)
                    return;

                auto itemAtIndex = ui->sampleList->topLevelItem(index);
                if (itemAtIndex != nullptr)
                    ui->sampleList->setCurrentItem(itemAtIndex);

                connect(ui->sampleList,
                        &QTreeWidget::currentItemChanged,
                        this,
                        &PeakEditor::_selectionChanged);
            });
}

PeakEditor::~PeakEditor()
{
    delete ui;
}

void PeakEditor::setPeakGroup(PeakGroup *group)
{
    if (group->samples.empty())
        return;

    _populateSampleList(group);
    _gallery->addEicPlots(group, _mw->mavenParameters);
}

void PeakEditor::_populateSampleList(PeakGroup* group)
{
    ui->sampleList->clear();

    auto samples = group->samples;
    sort(begin(samples), end(samples), mzSample::compSampleOrder);
    for (mzSample* sample : samples) {
        auto item = new NumericTreeWidgetItem(ui->sampleList, 0);
        item->setText(0, QString::fromStdString(sample->getSampleName()));
        item->setData(0, Qt::UserRole, QVariant::fromValue(sample));
        ui->sampleList->addTopLevelItem(item);
    }
}

void PeakEditor::_selectionChanged(QTreeWidgetItem* current,
                                   QTreeWidgetItem* previous)
{
    Q_UNUSED(previous);
    if (current == nullptr)
        return;

    _gallery->showPlotFor(ui->sampleList->indexOfTopLevelItem(current));
}
