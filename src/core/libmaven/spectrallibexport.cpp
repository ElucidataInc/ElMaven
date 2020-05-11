#include "datastructures/adduct.h"
#include "spectrallibexport.h"
#include "Fragment.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "PeakGroup.h"

using namespace std;

SpectralLibExport::SpectralLibExport(string filePath,
                                     Format format,
                                     int limitNumPeaks,
                                     float scaleToMax)
    : _filePath(filePath)
    , _format(format)
    , _limitNumPeaks(limitNumPeaks)
    , _scaleToMax(scaleToMax)
{
}

void SpectralLibExport::writePeakGroupData(PeakGroup* group)
{
    if (_format == Format::Nist)
        _writePeakGroupAsMsp(group);
    else if (_format == Format::Mascot) {
        _writePeakGroupAsMgf(group);
    }
}

void SpectralLibExport::_writePeakGroupAsMsp(PeakGroup *group)
{
    Fragment fragmentationProfile = group->fragmentationPattern;
    if (fragmentationProfile.nobs() == 0)
        return;

    // we never overwrite the contents of the file, only append to it
    ofstream out(_filePath, ios_base::app);

    if (group->hasCompoundLink()) {
        Compound* compound = group->getCompound();
        out << "NAME: " << compound->name() << "\n";
        out << "ID: " << compound->id() << "\n";
        out << "MW: " << compound->neutralMass() << "\n";
        out << "FORMULA: " << compound->formula() << "\n";
        out << "CATEGORY: "
            << mzUtils::join(compound->category(), ", ") << "\n";
        out << "SMILE: " << compound->smileString() << "\n";
        if (group->getAdduct() != nullptr) {
            out << "PRECURSORTYPE: " << group->getAdduct()->getName() << "\n";
        } else if (fragmentationProfile.polarity < 0) {
            out << "PRECURSORTYPE: [M-H]-\n";
        } else if (fragmentationProfile.polarity > 0) {
            out << "PRECURSORTYPE: [M+H]+\n";
        } else {
            out << "PRECURSORTYPE: [M]\n";
        }
    } else {
        out << "NAME: " << group->getName() << "\n";
        out << "ID: " << group->getName() << "\n";
    }

    out << "PRECURSORMZ: " << group->meanMz << "\n";
    out << "RETENTIONTIME: " << group->meanRt << "\n";

    if (group->sampleCount > 0) {
        mzSample* sample = group->samples.at(0);
        if (!sample->instrumentInfo.empty()) {
            out << "INSTRUMENTTYPE: "
                << sample->instrumentInfo.at("msIonisation") << "-"
                << sample->instrumentInfo.at("msMassAnalyzer") << "-"
                << sample->instrumentInfo.at("msDetector") << "\n";
            out << "INSTRUMENT: "
                << sample->instrumentInfo.at("msModel") << "\n";
        }
    }

    string ionizationMode = "";
    if (fragmentationProfile.polarity < 0) {
        ionizationMode = "Negative";
    } else if (fragmentationProfile.polarity > 0) {
        ionizationMode = "Positive";
    } else {
        ionizationMode = "Neutral"; // is this even possible?
    }
    out << "IONMODE: " << ionizationMode << "\n";
    out << "CE: " << fragmentationProfile.collisionEnergy << "\n";

    if (group->hasCompoundLink()) {
        Compound* compound = group->getCompound();
        out << "LOGP: " << compound->logP() << "\n";
        out << "COMMENT: " << compound->note() << "\n";
    }

    vector<float> mzValues = fragmentationProfile.mzValues;
    vector<float> intensityValues = fragmentationProfile.intensityValues;
    float maxIntensity = *(max_element(begin(intensityValues),
                                       end(intensityValues)));
    for (size_t i = 0; i < intensityValues.size(); ++i)
        intensityValues[i] = intensityValues[i] / maxIntensity * _scaleToMax;

    float cutoffIntensity = 1.0f;
    int numPeaks = intensityValues.size();
    if (numPeaks > _limitNumPeaks) {
        auto tempValues = intensityValues;
        std::sort(begin(tempValues), end(tempValues), std::greater<float>());
        cutoffIntensity = tempValues[_limitNumPeaks - 1];
        numPeaks = _limitNumPeaks;
    }

    out << "Num Peaks: " << numPeaks << "\n";
    for (int i = 0; i < mzValues.size(); ++i) {
        if (intensityValues[i] < cutoffIntensity)
            continue;
        out << mzValues[i] << "\t" << intensityValues[i] << "\n";
    }

    out << endl;
}

void SpectralLibExport::_writePeakGroupAsMgf(PeakGroup *group)
{
    // TODO: we can do this later when people want Mascot exports as well
}
