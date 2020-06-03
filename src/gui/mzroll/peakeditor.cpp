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
    ui->sampleList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _gallery = new GalleryWidget(this);
    ui->graphicsLayout->addWidget(_gallery);

    connect(ui->sampleList,
            &QTreeWidget::itemSelectionChanged,
            this,
            &PeakEditor::_selectionChanged);
    connect(_gallery,
            &GalleryWidget::peakRegionChanged,
            [this](mzSample* sample, float rtMin, float rtMax) {
                _editedPeakRegions[sample] = make_pair(rtMin, rtMax);
            });
    connect(ui->rtMinSpinBox,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this] {
                _gallery->setRtBounds(ui->rtMinSpinBox->value(),
                                      ui->rtMaxSpinBox->value());
            });
    connect(ui->rtMaxSpinBox,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this] {
                _gallery->setRtBounds(ui->rtMinSpinBox->value(),
                                      ui->rtMaxSpinBox->value());
            });
    connect(ui->cancelButton, &QPushButton::clicked, this, &PeakEditor::hide);
    connect(ui->applyButton,
            &QPushButton::clicked,
            this,
            &PeakEditor::_applyEdits);
}

PeakEditor::~PeakEditor()
{
    delete _gallery;
    delete ui;
}

void PeakEditor::setPeakGroup(PeakGroup *group)
{
    if (group == nullptr || group->samples.empty())
        return;

    setWindowTitle(tr("Peak Editor: %1").arg(group->getName().c_str()));

    _editedPeakRegions.clear();
    _group = group;

    _gallery->addEicPlots(_group);
    _setRtRangeAndValues();
    _populateSampleList();
}

void PeakEditor::_setRtRangeAndValues()
{
    auto rtBounds = _gallery->rtBounds();
    float rtBuffer = _gallery->rtBuffer();

    // find absolute min/max RT ranges
    float minRtBound = numeric_limits<float>::max();
    float maxRtBound = numeric_limits<float>::min();
    for (mzSample* sample : _group->samples) {
        minRtBound = min(minRtBound, sample->minRt);
        maxRtBound = max(maxRtBound, sample->maxRt);
    }

    // find min/max RT limit across isotopologues
    float minRt = rtBounds.first;
    float maxRt = rtBounds.second;
    if (_group->childCount() > 0) {
        for (PeakGroup& child : _group->children) {
            if (child.peakCount() == 0)
                continue;
            minRt = min(minRt, child.minRt - rtBuffer);
            maxRt = max(maxRt, child.maxRt + rtBuffer);
        }
    } else if (_group->type() == PeakGroup::GroupType::Isotope
               && _group->parent != nullptr) {
        PeakGroup* parentGroup = _group->parent;
        minRt = parentGroup->minRt - rtBuffer;
        maxRt = parentGroup->maxRt + rtBuffer;
        for (PeakGroup& child : parentGroup->children) {
            if (child.peakCount() == 0)
                continue;
            minRt = min(minRt, child.minRt - rtBuffer);
            maxRt = max(maxRt, child.maxRt + rtBuffer);
        }
    }

    // set allowed and visible RT ranges
    ui->rtMinSpinBox->setRange(minRtBound, minRt);
    ui->rtMaxSpinBox->setRange(maxRt, maxRtBound);
    ui->rtMinSpinBox->setValue(minRt);
    ui->rtMaxSpinBox->setValue(maxRt);

    // have to call this separately (even though slots are connected), because
    // for isotopologues the `valueChanged` signal will not be re-emitted, as we
    // are constructing a range for them which does not change
    _gallery->setRtBounds(minRt, maxRt);
}

void PeakEditor::_populateSampleList()
{
    ui->sampleList->clear();

    auto samples = _group->samples;
    sort(begin(samples), end(samples), mzSample::compSampleOrder);
    for (mzSample* sample : samples) {
        auto item = new NumericTreeWidgetItem(ui->sampleList, 0);
        item->setText(0, QString::fromStdString(sample->getSampleName()));
        item->setData(0, Qt::UserRole, QVariant::fromValue(sample));
        ui->sampleList->addTopLevelItem(item);
    }
    ui->sampleList->setCurrentItem(ui->sampleList->topLevelItem(0));
}

void PeakEditor::_editPeakRegionForSample(mzSample* peakSample,
                                          float rtMin,
                                          float rtMax)
{
    // lambda: deletes the peak for `peakSample` in `_group` if it exists
    auto deletePeakIfExists = [this, peakSample] {
        _group->peaks.erase(remove_if(begin(_group->peaks),
                                  end(_group->peaks),
                                  [peakSample](Peak& peak) {
                                      return peak.getSample() == peakSample;
                                  }),
                            end(_group->peaks));
    };

    if (rtMin < 0.0f && rtMax < 0.0f) {
        deletePeakIfExists();
        return;
    }

    vector<EIC*> eics = _gallery->eics();
    auto eicFoundAt = find_if(begin(eics), end(eics), [peakSample](EIC* eic) {
        return eic->sample == peakSample;
    });
    if (eicFoundAt == end(eics))
        return;

    int peakIndexForSample = -1;
    EIC* eic = *eicFoundAt;
    bool deletePeak = false;
    for (int i = 0; i < _group->peaks.size(); ++i) {
        Peak& peak = _group->peaks[i];
        if (peak.getSample() != peakSample)
            continue;
        peakIndexForSample = i;
    }

    Peak newPeak = eic->peakForRegion(rtMin, rtMax);
    newPeak.mzmin = _group->getSlice().mzmin;
    newPeak.mzmax = _group->getSlice().mzmax;
    eic->getPeakDetails(newPeak);
    if (newPeak.pos > 0) {
        if (_clsf != nullptr)
            newPeak.quality = _clsf->scorePeak(newPeak);

        if (!_peakFilter.filter(newPeak)) {
            if (peakIndexForSample < 0) {
                _group->addPeak(newPeak);
            } else {
                _group->peaks[peakIndexForSample] = newPeak;
            }
        } else {
            deletePeak = true;
        }
    } else {
        deletePeak = true;
    }

    if (deletePeak)
        deletePeakIfExists();

    mzSlice slice = _group->getSlice();
    slice.rtmin = min(slice.rtmin, rtMin);
    slice.rtmax = max(slice.rtmax, rtMax);
    slice.rt = (slice.rtmin + slice.rtmax) / 2.0f;
    _group->setSlice(slice);
}

void PeakEditor::_selectionChanged()
{
    auto selectedItems = ui->sampleList->selectedItems();
    if (selectedItems.empty())
        return;

    vector<int> selectedIndexes;
    for (auto item : selectedItems)
        selectedIndexes.push_back(ui->sampleList->indexOfTopLevelItem(item));
    _gallery->showPlotFor(selectedIndexes);
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
