#include "datastructures/adduct.h"
#include "spectrallibexport.h"
#include "Fragment.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "PeakGroup.h"

using namespace std;

SpectralLibExport::SpectralLibExport(string filePath, Format format)
    : _filePath(filePath)
    , _format(format)
{
}

void SpectralLibExport::writePeakGroupData(PeakGroup* group)
{
    if (_format == Format::NIST)
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
        out << "PRECURSORTYPE: " << group->getAdduct()->getName() << "\n";
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

    string ionizationMode = fragmentationProfile.polarity < 0 ? "Negative"
                                                              : "Positive";
    out << "IONMODE: " << ionizationMode << "\n";
    out << "CE: " << fragmentationProfile.collisionEnergy << "\n";

    if (group->hasCompoundLink()) {
        Compound* compound = group->getCompound();
        out << "LOGP: " << compound->logP() << "\n";
        out << "COMMENT: " << compound->note() << "\n";
    }

    out << "Num Peaks: " << fragmentationProfile.nobs() << "\n";
    for (int i = 0; i < fragmentationProfile.nobs(); ++i) {
        out << fragmentationProfile.mzValues.at(i) << "\t"
            << fragmentationProfile.intensityValues.at(i) << "\n";
    }

    out << endl;
}

void SpectralLibExport::_writePeakGroupAsMgf(PeakGroup *group)
{
    // TODO: we can do this later when people want Mascot exports as well
}
