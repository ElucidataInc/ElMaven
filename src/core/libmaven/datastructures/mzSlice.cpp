#include "adduct.h"
#include "mzSlice.h"
#include "Compound.h"
#include "masscutofftype.h"
#include "mzMassCalculator.h"
#include "mzUtils.h"

mzSlice::mzSlice(float minMz, float maxMz, float minRt, float maxRt)
{
	mzmin = minMz;
	mzmax = maxMz;
	rtmin = minRt;
	rtmax = maxRt;
    mz = (maxMz + minMz) / 2.0f;
    rt = (maxRt + minRt) / 2.0f;
    compound = nullptr;
    adduct = nullptr;
    isotope = Isotope();
	ionCount = 0;
}

mzSlice::mzSlice(string filterLine)
{
	mzmin = mzmax = rtmin = rtmax = mz = rt = ionCount = 0;
	compound = NULL;
    srmId = filterLine;
    adduct = nullptr;
}

mzSlice::mzSlice()
{
	mzmin = mzmax = rtmin = rtmax = mz = rt = ionCount = 0;
    compound = NULL;
    adduct = nullptr;
}

mzSlice& mzSlice::operator=(const mzSlice& b)
{
    mzmin = b.mzmin;
    mzmax = b.mzmax;
    rtmin = b.rtmin;
    rtmax = b.rtmax;
    ionCount = b.ionCount;
    compound = b.compound;
    adduct = b.adduct;
    isotope = b.isotope;
    srmId = b.srmId;
    mz = b.mz;
    rt = b.rt;
    return *this;
}

bool mzSlice::operator==(const mzSlice &b) const
{
    return (mzmin == b.mzmin
            && mzmax == b.mzmax
            && rtmin == b.rtmin
            && rtmax == b.rtmax
            && ionCount == b.ionCount
            && compound == b.compound
            && adduct == b.adduct
            && isotope == b.isotope
            && srmId == b.srmId
            && mz == b.mz
            && rt == b.rt);
}

bool mzSlice::calculateMzMinMax(MassCutoff *compoundMassCutoffWindow, int charge)
{
    float adjustedMass = 0.0f;
    if (!mzUtils::almostEqual(isotope.mass, 0.0)) {
        // computing the mass (and bounds) based on isotopologue mass
        adjustedMass = static_cast<float>(isotope.mass);
    } else if (adduct != nullptr && !compound->formula().empty()) {
        // computing the mass (and bounds) adjusted for adduct's mass
        auto mass = MassCalculator::computeNeutralMass(compound->formula());
        adjustedMass = adduct->computeAdductMz(mass);
    } else if (adduct != nullptr && compound->neutralMass() != 0.0f) {
        // computing the mass (and bounds) adjusted for adduct's mass
        auto mass = compound->neutralMass();
        adjustedMass = adduct->computeAdductMz(mass);
    } else if (!compound->formula().empty()
               || compound->neutralMass() != 0.0f) {
        // regular adjusted mass if the formula or neutral mass is given
        adjustedMass = compound->adjustedMass(charge);
    } else if (compound->mz() > 0) {
        // m/z already present in the compound DB then just use that
        adjustedMass = compound->mz();
    } else {
        // cannot find bounds if formula, neutral mass and m/z are all absent
		return false;
	}

    float mzDelta = compoundMassCutoffWindow->massCutoffValue(adjustedMass);
    mzmin = adjustedMass - mzDelta;
    mzmax = adjustedMass + mzDelta;
    mz = (mzmax + mzmin) / 2.0f;
    return true;
}

void mzSlice::calculateRTMinMax(bool matchRtFlag, float compoundRTWindow)
{
	//If the compound database has the expected RT information and
	//if RT matching flag has been enabled in the peakdetection
	//window then only calculate the rt min and max else set
	//it in such a way that it will look in all the rt values
	//possible
    if (matchRtFlag && this->compound->expectedRt() > 0)
	{
        this->rtmin = this->compound->expectedRt() - compoundRTWindow;
        this->rtmax = this->compound->expectedRt() + compoundRTWindow;
	}
	else
	{
		// As its time min value will be 0
		this->rtmin = 0;
		//TODO: max value shoould be set as the max of the
		//double
		this->rtmax = 1e9;
	}
    rt = (rtmax + rtmin) / 2.0f;
}

void mzSlice::setSRMId()
{
	//TODO: Why is SRM id used for
    if (!this->compound->srmId().empty())
	{
        this->srmId = this->compound->srmId();
	}
}
