#include <QGroupBox>
#include <QLabel>

#include "Compound.h"
#include "datastructures/adduct.h"
#include "EIC.h"
#include "groupsettingslog.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "mzUtils.h"
#include "PeakGroup.h"
#include "ui_groupsettingslog.h"

GroupSettingsLog::GroupSettingsLog(QWidget *parent, shared_ptr<PeakGroup> group)
    : QDialog(parent)
    , _group(group)
    , ui(new Ui::GroupSettingsLog)
{
    setModal(true);
    ui->setupUi(this);
    setFixedSize(size());
    _displayGroupSettings();

    if (_group != nullptr)
        setWindowTitle(tr("Settings Log: %1").arg(_group->getName().c_str()));
}

GroupSettingsLog::~GroupSettingsLog()
{
    delete ui;
}

void GroupSettingsLog::_displayGroupSettings()
{
    if (_group == nullptr)
        return;

    MavenParameters* mp = _group->parameters().get();

    // lambda: returns a suitable ionization mode string
    auto ionizationMode = [mp] {
        if (mp->ionizationMode < 0)
            return "Negative (-1)";
        if (mp->ionizationMode > 0)
            return "Positive (+1)";
        return "Neutral";
    };

    // lambda: returns a "X ppm" or "Y mDa" type of string
    auto massToleranceAndType = [this, mp] {
        auto integrationType = _group->integrationType();
        if (integrationType == PeakGroup::IntegrationType::Manual
            || (integrationType == PeakGroup::IntegrationType::Automated
                && _group->hasCompoundLink())) {
            return tr("%1 %2")
                .arg(mp->compoundMassCutoffWindow->getMassCutoff())
                .arg(mp->compoundMassCutoffWindow->getMassCutoffType().c_str());
        }
        return tr("%1 %2")
            .arg(mp->massCutoffMerge->getMassCutoff())
            .arg(mp->massCutoffMerge->getMassCutoffType().c_str());
    };

    // lambda: returns name of the smoothing algorithm
    auto smoothingAlgorithm = [mp] {
        auto smootherType = static_cast<EIC::SmootherType>(
            mp->eic_smoothingAlgorithm);
        if (smootherType == EIC::SmootherType::SAVGOL)
            return "Savitzky-Golay";
        if (smootherType == EIC::SmootherType::GAUSSIAN)
            return "Gaussian";
        return "Moving average";
    };

    // lambda: returns name of the baseline estimation algorithm
    auto baselineEstimator = [mp] {
        if (mp->aslsBaselineMode)
            return "AsLS Smoothing";
        return "Thresholding";
    };

    // lambda: returns name and value for first baseline esitmation parameter
    auto firstBaselineParamAndValue = [mp] {
        if (mp->aslsBaselineMode) {
            return make_pair(tr("Smoothness"),
                             tr("%1").arg(mp->aslsSmoothness));
        }
        return make_pair(tr("Allow top [x] percent intensities as part of the real signal"),
                         tr("%1 %").arg(mp->baseline_dropTopX));
    };

    // lambda: returns name and value for second baseline esitmation parameter
    auto secondBaselineParamAndValue = [mp] {
        if (mp->aslsBaselineMode) {
            return make_pair(tr("Asymmetry"),
                             tr("%1").arg(static_cast<float>(mp->aslsAsymmetry)
                                          / 100.0f));
        }
        return make_pair(tr("Baseline smoothing"),
                         tr("%1 scans").arg(mp->baseline_smoothingWindow));
    };

    // lambda: returns a comma-separated string of selected tracer names
    auto isotopicTracers = [mp] {
        QStringList tracerNames;
        if (mp->C13Labeled_BPE)
            tracerNames << "C13";
        if (mp->D2Labeled_BPE)
            tracerNames << "D2";
        if (mp->N15Labeled_BPE)
            tracerNames << "N15";
        if (mp->S34Labeled_BPE)
            tracerNames << "S34";
        if (tracerNames.isEmpty())
            return tr("None");
        return tracerNames.join(", ");
    };

    // lambda: returns a newline separated string of selected adduct forms
    auto adductForms = [mp] {
        QStringList adductNames;
        for (auto adduct : mp->getChosenAdductList())
            adductNames << adduct->getName().c_str();
        if (adductNames.isEmpty()) {
            auto defaultAdducts = mp->getDefaultAdductList();
            for (auto adduct : defaultAdducts) {
                if (SIGN(adduct->getCharge()) == SIGN(mp->getCharge()))
                    adductNames << adduct->getName().c_str();
            }
        }
        if (adductNames.size() < 4)
            return adductNames.join(", ");
        return adductNames.join("\n");
    };

    // lambda: returns a string for the set EIC type
    auto eicType = [mp] {
        auto type = static_cast<EIC::EicType>(mp->eicType);
        if (type == EIC::EicType::MAX)
            return "Max";
        return "Sum";
    };

    vector<pair<QString, vector<pair<QString, QString>>>> parameterGroups = {
        {
            "Instrument-level settings",
            {
                {"Polarity / Ionization mode",
                 ionizationMode()},
                {"Q1 accuracy",
                 tr("%1 amu").arg(mp->amuQ1)},
                {"Q3 accuracy",
                 tr("%1 amu").arg(mp->amuQ3)},
                {"Mass tolerance",
                 massToleranceAndType()}
            }
        },
        {
            "Peak-detection settings",
            {
                {"Smoothing algorithm",
                 smoothingAlgorithm()},
                {"Smoothing window",
                 tr("%1 scans").arg(mp->eic_smoothingWindow)},
                {"Max RT devialtion among peaks in a group",
                 tr("%1 minutes").arg(mp->grouping_maxRtWindow)},
                {"Baseline estimation algorithm",
                 baselineEstimator()},
                firstBaselineParamAndValue(),
                secondBaselineParamAndValue()
            }
        },
        {
            "Peak filtering settings",
            {
                {"Min signal-baseline difference",
                 tr("%1").arg(mp->minSignalBaselineDifference)},
                {"Min peak quality",
                 tr("%1").arg(mp->minPeakQuality)},
                {"Min isotope signal-baseline difference",
                 tr("%1").arg(mp->isotopicMinSignalBaselineDifference)},
                {"Min isotope peak quality",
                 tr("%1").arg(mp->minIsotopicPeakQuality)},
            }
        },
        {
            "Peak-grouping settings",
            {
                {"RT weight",
                 tr("%1").arg(mp->distXWeight/10)},
                {"Intensity weight",
                 tr("%1").arg(mp->distYWeight/10)},
                {"Overlap weight",
                 tr("%1").arg(mp->overlapWeight/10)},
                {"Consider overlap",
                 tr("%1").arg(mp->useOverlap ? "Yes" : "No")},
            }
        },
        {
            "Miscellaneous settings",
            {
                {"EIC type",
                 tr("%1").arg(eicType())},
                {"Report isotopes",
                 tr("%1").arg(mp->pullIsotopesFlag ? "Yes" : "No")},
                {"Match fragmentation",
                 tr("%1").arg(mp->matchFragmentationFlag ? "Yes" : "No")}
            }
        },
    };

    if (mp->pullIsotopesFlag
        && _group->hasCompoundLink()) {
        vector<pair<QString, QString>> isotopeParams = {
            {"Isotopic tracers",
             tr("%1").arg(isotopicTracers())},
            {"Parent ion (unlabelled form) must be present",
             tr("%1").arg(mp->parentIsotopeRequired ? "Yes" : "No")},
            {"Link isotope peak RT range with parent peak",
             tr("%1").arg(mp->linkIsotopeRtRange ? "Yes" : "No")},
        };
        if (mp->filterIsotopesAgainstParent) {
            isotopeParams.insert(
                begin(isotopeParams) + 1,
                {"Max RT deviation from parent isotopologue",
                 tr("%1 seconds").arg(mp->maxIsotopeScanDiff)});
            isotopeParams.insert(
                begin(isotopeParams) + 2,
                {"Min signal correlation with parent isotopologue",
                 tr("%1 %").arg(mp->minIsotopicCorrelation)});
        }
        parameterGroups.push_back(make_pair("Isotope detection settings",
                                            isotopeParams));
    }

    if (mp->searchAdducts
        && _group->hasCompoundLink()) {
        vector<pair<QString, QString>> adductParams = {
            {"Adduct forms",
             tr("%1").arg(adductForms())},
            {"Parent ion (default adduct form) must be present",
             tr("%1").arg(mp->parentAdductRequired ? "Yes" : "No")},
        };
        if (mp->filterAdductsAgainstParent) {
            adductParams.insert(
                begin(adductParams) + 1,
                {"Max RT deviation from parent adduct",
                 tr("%1 seconds").arg(mp->adductSearchWindow)});
            adductParams.insert(
                begin(adductParams) + 2,
                {"Min signal correlation with parent adduct",
                 tr("%1 %").arg(mp->adductPercentCorrelation)});
        }
        parameterGroups.push_back(make_pair("Adduct detection settings",
                                            adductParams));
    }

    if (mp->matchFragmentationFlag
        && _group->hasCompoundLink()
        && _group->getCompound()->type() == Compound::Type::MS2) {
        vector<pair<QString, QString>> fragParams = {
            {"Fragment mass tolerance",
             tr("%1 ppm").arg(mp->fragmentTolerance)},
            {"Fragmentation matching algorithm",
             tr("%1").arg(mp->scoringAlgo.c_str())}
        };
        parameterGroups.push_back(make_pair("Fragmentation matching settings",
                                            fragParams));
    }

    if (_group->isClassified) {
        auto label = _group->labelToString(_group->predictedLabel());
        auto model = mp->peakMlModelType;
        auto noiseRange = "0.0 - " + mzUtils::float2string(mp->badGroupUpperLimit, 2);
        string maybeRange = "";
        if (mp->badGroupUpperLimit != mp->goodGroupLowerLimit) {
            maybeRange = mzUtils::float2string(mp->badGroupUpperLimit, 2)
                                + " - " + mzUtils::float2string(mp->goodGroupLowerLimit, 2);
        }
        auto signalRange = mzUtils::float2string(mp->goodGroupLowerLimit, 2)
                            + " - 1.0";
        vector<pair<QString, QString>> curationParams = {
            {"Classified label",
             tr("%1").arg(QString::fromStdString(label))},
            {"Classification model",
             tr("%1").arg(QString::fromStdString(model))},
            {"Noise range",
             tr("%1").arg(QString::fromStdString(noiseRange))}, 
        };
        if (mp->badGroupUpperLimit != mp->goodGroupLowerLimit) {
            curationParams.push_back({"May be good range",
             tr("%1").arg(QString::fromStdString(maybeRange))});
        }
        curationParams.push_back({"Signal range",
             tr("%1").arg(QString::fromStdString(signalRange))});
        parameterGroups.push_back(make_pair("Peak Curation",
                                            curationParams));
    }

    QString spacerStyle = "QWidget { "
                          "margin-top: 10px; "
                          "border-top: 1px dotted; "
                          "}";
    for (auto& elem : parameterGroups) {
        QString parameterGroupName = elem.first;
        vector<pair<QString, QString>> parameters = elem.second;

        QVBoxLayout* vbox = new QVBoxLayout;
        for (auto& parameter : parameters) {
            QHBoxLayout* hbox = new QHBoxLayout;

            QLabel* parameterLabel = new QLabel(parameter.first);
            parameterLabel->setAlignment(Qt::AlignTop);
            parameterLabel->setAlignment(Qt::AlignLeft);
            QLabel* valueLabel = new QLabel(parameter.second);
            valueLabel->setAlignment(Qt::AlignTop);
            valueLabel->setAlignment(Qt::AlignRight);

            QWidget* spacer = new QWidget(nullptr);
            spacer->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::Minimum);
            spacer->setStyleSheet(spacerStyle);

            hbox->addWidget(parameterLabel);
            hbox->addWidget(spacer);
            hbox->addWidget(valueLabel);
            vbox->addLayout(hbox);
        }

        QGroupBox* groupBox = new QGroupBox(parameterGroupName);
        groupBox->setLayout(vbox);

        ui->vLayout->addWidget(groupBox);
    }
}
