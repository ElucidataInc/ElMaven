#include "classifierNeuralNet.h"
#include "Compound.h"
#include "EIC.h"
#include "gallerywidget.h"
#include "globals.h"
#include "mainwindow.h"
#include "mzSample.h"
#include "numeric_treewidgetitem.h"
#include "peakeditor.h"
#include "Scan.h"
#include "ui_peakeditor.h"

PeakEditor::PeakEditor(MainWindow *parent,
                       ClassifierNeuralNet* clsf) :
      QDialog(parent),
      ui(new Ui::PeakEditor),
      _mw(parent),
      _clsf(clsf),
      _peakFilter(PeakFiltering(_mw->mavenParameters, false))
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
    connect(_gallery,
            &GalleryWidget::peakRegionChanged,
            [this](mzSample* sample, float rtMin, float rtMax) {
                _editedPeakRegions[sample] = make_pair(rtMin, rtMax);
            });
    connect(ui->cancelButton, &QPushButton::clicked, this, &PeakEditor::hide);
    connect(ui->applyButton,
            &QPushButton::clicked,
            this,
            &PeakEditor::_applyEdits);
}

PeakEditor::~PeakEditor()
{
    delete ui;
}

void PeakEditor::setPeakGroup(PeakGroup *group)
{
    if (group == nullptr || group->samples.empty())
        return;

    _editedPeakRegions.clear();
    _group = group;

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

void PeakEditor::_editPeakRegionForSample(mzSample* peakSample,
                                          float rtMin,
                                          float rtMax)
{
    vector<EIC*> eics = _gallery->eics();
    auto eicFoundAt = find_if(begin(eics), end(eics), [peakSample](EIC* eic) {
        return eic->sample == peakSample;
    });
    if (eicFoundAt == end(eics))
        return;

    EIC* eic = *eicFoundAt;
    bool deletePeak = false;
    for (Peak& peak : _group->peaks) {
        if (peak.getSample() != peakSample)
            continue;

        Peak newPeak = eic->peakForRegion(rtMin, rtMax);
        newPeak.mzmin = _group->getSlice().mzmin;
        newPeak.mzmax = _group->getSlice().mzmax;
        eic->getPeakDetails(newPeak);
        if (newPeak.pos > 0) {
            if (_clsf != nullptr)
                newPeak.quality = _clsf->scorePeak(newPeak);

            if (!_peakFilter.filter(newPeak)) {
                peak = newPeak;
            } else {
                deletePeak = true;
            }
        } else {
            deletePeak = true;
        }
        break;
    }

    if (deletePeak) {
        _group->peaks.erase(remove_if(begin(_group->peaks),
                                      end(_group->peaks),
                                      [peakSample](Peak& peak) {
                                          return peak.getSample() == peakSample;
                                      }),
                            end(_group->peaks));
    }

    mzSlice slice = _group->getSlice();
    slice.rtmin = min(slice.rtmin, rtMin);
    slice.rtmax = max(slice.rtmax, rtMax);
    slice.rt = (slice.rtmin + slice.rtmax) / 2.0f;
    _group->setSlice(slice);
}

void PeakEditor::_selectionChanged(QTreeWidgetItem* current,
                                   QTreeWidgetItem* previous)
{
    Q_UNUSED(previous);
    if (current == nullptr)
        return;

    _gallery->showPlotFor(ui->sampleList->indexOfTopLevelItem(current));
}

void PeakEditor::_applyEdits()
{
    for (auto& elem : _editedPeakRegions) {
        auto sample = elem.first;
        auto rtMin = elem.second.first;
        auto rtMax = elem.second.second;
        _editPeakRegionForSample(sample, rtMin, rtMax);
    }
    _group->groupStatistics();
    _mw->setPeakGroup(_group);
    hide();
}
