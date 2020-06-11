#include "classifierNeuralNet.h"
#include "Compound.h"
#include "EIC.h"
#include "gallerywidget.h"
#include "globals.h"
#include "mainwindow.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "numeric_treewidgetitem.h"
#include "PeakDetector.h"
#include "peakeditor.h"
#include "Scan.h"
#include "ui_peakeditor.h"

PeakEditor::PeakEditor(MainWindow *parent,
                       ClassifierNeuralNet* clsf) :
      QDialog(parent),
      ui(new Ui::PeakEditor),
      _mw(parent),
      _clsf(clsf)
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
            &GalleryWidget::peakRegionSet,
            [this](mzSample* sample, float rtMin, float rtMax) {
                // in case the user joined both markers, attempting to remove
                if (rtMin == rtMax) {
                    rtMin = -1.0f;
                    rtMax = -1.0f;
                }
                _setPeakRegions[sample] = make_pair(rtMin, rtMax);
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
    connect(ui->dropTopSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            [this] {
                if (ui->baselineTabWidget->currentIndex() == 0) {
                    _gallery->recomputeBaselinesThresh(
                        ui->dropTopSpinBox->value(),
                        ui->smoothingSpinBox->value());
                }
            });
    connect(ui->smoothingSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            [this] {
                if (ui->baselineTabWidget->currentIndex() == 0) {
                    _gallery->recomputeBaselinesThresh(
                        ui->dropTopSpinBox->value(),
                        ui->smoothingSpinBox->value());
                }
            });
    connect(ui->smoothnessSlider,
            &QSlider::valueChanged,
            [this](int value) {
                ui->smoothnessLabel->setText(tr("%1").arg(value));
                if (ui->baselineTabWidget->currentIndex() == 1) {
                    _gallery->recomputeBaselinesAsLS(
                        ui->smoothnessSlider->value(),
                        ui->asymmetrySlider->value());
                }
            });
    connect(ui->asymmetrySlider,
            &QSlider::valueChanged,
            [this](int value) {
                ui->asymmetryLabel->setText(tr("%1").arg(value));
                if (ui->baselineTabWidget->currentIndex() == 1) {
                    _gallery->recomputeBaselinesAsLS(
                        ui->smoothnessSlider->value(),
                        ui->asymmetrySlider->value());
                }
            });
    connect(ui->baselineTabWidget,
            &QTabWidget::currentChanged,
            [this](int index) {
                if (index == 0) {
                    _gallery->recomputeBaselinesThresh(
                        ui->dropTopSpinBox->value(),
                        ui->smoothingSpinBox->value());
                } else {
                    _gallery->recomputeBaselinesAsLS(
                        ui->smoothnessSlider->value(),
                        ui->asymmetrySlider->value());
                }
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

void PeakEditor::setPeakGroup(shared_ptr<PeakGroup> group)
{
    if (group == nullptr || group->samples.empty())
        return;

    setWindowTitle(tr("Peak Editor: %1").arg(group->getName().c_str()));

    _setPeakRegions.clear();
    _group = group;

    _gallery->addEicPlots(_group.get());
    _setBaselineParameters();
    _setRtRangeAndValues();
    _setSyncRtCheckbox();
    _populateSampleList();
}

void PeakEditor::_setBaselineParameters()
{
    MavenParameters* parameters = _group->parameters().get();
    if (parameters->aslsBaselineMode) {
        ui->baselineTabWidget->setCurrentIndex(1);
        ui->smoothnessSlider->setValue(parameters->aslsSmoothness);
        ui->asymmetrySlider->setValue(parameters->aslsAsymmetry);
    } else {
        ui->baselineTabWidget->setCurrentIndex(0);
        ui->dropTopSpinBox->setValue(parameters->baseline_dropTopX);
        ui->smoothingSpinBox->setValue(parameters->baseline_smoothingWindow);
    }
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
        for (auto child : _group->children) {
            if (child->peakCount() == 0)
                continue;
            minRt = min(minRt, child->minRt - rtBuffer);
            maxRt = max(maxRt, child->maxRt + rtBuffer);
        }
    } else if (_group->isIsotope() && _group->parent != nullptr) {
        PeakGroup* parentGroup = _group->parent;
        minRt = parentGroup->minRt - rtBuffer;
        maxRt = parentGroup->maxRt + rtBuffer;
        for (auto child : parentGroup->children) {
            if (child->peakCount() == 0)
                continue;
            minRt = min(minRt, child->minRt - rtBuffer);
            maxRt = max(maxRt, child->maxRt + rtBuffer);
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

void PeakEditor::_setSyncRtCheckbox()
{
    if (_group->childCount() == 0 && !_group->isIsotope()) {
        ui->syncRtCheckBox->setChecked(false);
        ui->syncRtCheckBox->setEnabled(false);
        return;
    }

    ui->syncRtCheckBox->setEnabled(true);

    MavenParameters* parameters = _group->parameters().get();
    if (parameters->linkIsotopeRtRange
        && (_group->childCount() > 0 || _group->tagString == "C12 PARENT")) {
        ui->syncRtCheckBox->setChecked(true);
    } else {
        ui->syncRtCheckBox->setChecked(false);
    }
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

void PeakEditor::_editPeakRegionForSample(PeakGroup *group,
                                          mzSample* peakSample,
                                          vector<EIC*>& eics,
                                          float rtMin,
                                          float rtMax)
{
    // lambda: deletes the peak for `peakSample` in `group` if it exists
    auto deletePeakIfExists = [group, peakSample] {
        group->peaks.erase(remove_if(begin(group->peaks),
                                  end(group->peaks),
                                  [peakSample](Peak& peak) {
                                      return peak.getSample() == peakSample;
                                  }),
                            end(group->peaks));
    };

    if (rtMin < 0.0f && rtMax < 0.0f) {
        deletePeakIfExists();
        return;
    }

    auto eicFoundAt = find_if(begin(eics), end(eics), [peakSample](EIC* eic) {
        return eic->sample == peakSample;
    });
    if (eicFoundAt == end(eics))
        return;

    int peakIndexForSample = -1;
    EIC* eic = *eicFoundAt;
    bool deletePeak = false;
    for (int i = 0; i < group->peaks.size(); ++i) {
        Peak& peak = group->peaks[i];
        if (peak.getSample() != peakSample)
            continue;
        peakIndexForSample = i;
    }

    Peak newPeak = eic->peakForRegion(rtMin, rtMax);
    newPeak.mzmin = group->getSlice().mzmin;
    newPeak.mzmax = group->getSlice().mzmax;
    eic->getPeakDetails(newPeak);
    if (newPeak.pos > 0) {
        if (_clsf != nullptr)
            newPeak.quality = _clsf->scorePeak(newPeak);

        PeakFiltering peakFilter(group->parameters().get(), group->isIsotope());
        if (!peakFilter.filter(newPeak)) {
            if (peakIndexForSample < 0) {
                group->addPeak(newPeak);
            } else {
                group->peaks[peakIndexForSample] = newPeak;
            }
        } else {
            deletePeak = true;
        }
    } else {
        deletePeak = true;
    }

    if (deletePeak)
        deletePeakIfExists();

    mzSlice slice = group->getSlice();
    slice.rtmin = min(slice.rtmin, rtMin);
    slice.rtmax = max(slice.rtmax, rtMax);
    slice.rt = (slice.rtmin + slice.rtmax) / 2.0f;
    group->setSlice(slice);
}

void PeakEditor::_applyEdits()
{
    _setBusyState();

    MavenParameters* mp = _group->parameters().get();
    if (ui->baselineTabWidget->currentIndex() == 0) {
        mp->aslsBaselineMode = false;
        mp->baseline_dropTopX = ui->dropTopSpinBox->value();
        mp->baseline_smoothingWindow = ui->smoothingSpinBox->value();
    } else {
        mp->aslsBaselineMode = true;
        mp->aslsSmoothness = ui->smoothnessSlider->value();
        mp->aslsAsymmetry = ui->asymmetrySlider->value();
    }

    // lambda: edits peak regions and recalculates a group's statistics
    auto editGroup = [this](PeakGroup* group, vector<EIC*>& eics) {
        for (auto& elem : _setPeakRegions) {
            auto sample = elem.first;
            auto rtMin = elem.second.first;
            auto rtMax = elem.second.second;
            _editPeakRegionForSample(group, sample, eics, rtMin, rtMax);
        }
        group->groupStatistics();
    };

    // lambda: obtain full range EICs for the given peak-group
    auto getEicsForGroup = [] (PeakGroup* group) {
        mzSlice slice = group->getSlice();
        slice.rtmin = numeric_limits<float>::max();
        slice.rtmax = numeric_limits<float>::min();
        for (mzSample* sample : group->samples) {
            slice.rtmin = min(slice.rtmin, sample->minRt);
            slice.rtmax = max(slice.rtmax, sample->maxRt);
        }
        auto eics = PeakDetector::pullEICs(&slice,
                                           group->samples,
                                           group->parameters().get());
        return eics;
    };

    // if checked, edit regions for all related isotopologues
    if (ui->syncRtCheckBox->isChecked()) {
        mp->linkIsotopeRtRange = true;

        PeakGroup* parentGroup = nullptr;
        if (_group->childCount() > 0) {
            parentGroup = _group.get();
        } else if (_group->isIsotope()) {
            parentGroup = _group->parent;
        }

        if (parentGroup == nullptr) {
            close();
            _setActiveState();
            return;
        }

        auto eics = getEicsForGroup(parentGroup);
        editGroup(parentGroup, eics);
        for (auto child : parentGroup->children) {
            eics = getEicsForGroup(child.get());
            editGroup(child.get(), eics);
        }
    } else {
        mp->linkIsotopeRtRange = false;
        auto eics = _gallery->eics();
        editGroup(_group.get(), eics);
    }

    _mw->setPeakGroup(_group);
    _setActiveState();
    close();
}

void PeakEditor::_setBusyState()
{
    ui->sampleList->setEnabled(false);
    _gallery->setEnabled(false);
    ui->baselineSettings->setEnabled(false);
    ui->editorSettings->setEnabled(false);
    ui->cancelButton->setEnabled(false);
    ui->applyButton->setEnabled(false);
    ui->statusLabel->setText("Applying edits…");
}

void PeakEditor::_setActiveState()
{
    ui->sampleList->setEnabled(true);
    _gallery->setEnabled(true);
    ui->baselineSettings->setEnabled(true);
    ui->editorSettings->setEnabled(true);
    ui->cancelButton->setEnabled(true);
    ui->applyButton->setEnabled(true);
    ui->statusLabel->setText("");
}
