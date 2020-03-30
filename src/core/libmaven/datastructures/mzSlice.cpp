#include "adduct.h"
#include "mzSlice.h"
#include "Compound.h"
#include "masscutofftype.h"
#include "mzMassCalculator.h"

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

bool mzSlice::calculateMzMinMax(MassCutoff *compoundMassCutoffWindow, int charge)
{
    // calculating the mzmin and mzmax
    if (this->adduct != nullptr && !this->compound->formula().empty()) {
        auto mass = MassCalculator::computeNeutralMass(compound->formula());
        auto adjustedMass = adduct->computeAdductMz(mass);
        mzmin = adjustedMass - compoundMassCutoffWindow->massCutoffValue(adjustedMass);
        mzmax = adjustedMass + compoundMassCutoffWindow->massCutoffValue(adjustedMass);
    } else if (!this->compound->formula().empty() || this->compound->neutralMass() != 0.0f) {
        //Computing the mass if the formula is given
        double mass = this->compound->adjustedMass(charge);
		this->mzmin = mass - compoundMassCutoffWindow->massCutoffValue(mass);
		this->mzmax = mass + compoundMassCutoffWindow->massCutoffValue(mass);
	}
    else if (this->compound->mz() > 0)
	{
		// Mass already present in the compound DB then using
		// it to find the mzmin and mzmax
        double mass = this->compound->mz();
		this->mzmin = mass - compoundMassCutoffWindow->massCutoffValue(mass);
		this->mzmax = mass + compoundMassCutoffWindow->massCutoffValue(mass);
	}
	else
	{
		// Not adding the compound if the formula is not given
		// and if the mass is also not given
		return false;
	}

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
}

void mzSlice::setSRMId()
{
	//TODO: Why is SRM id used for
    if (!this->compound->srmId().empty())
	{
        this->srmId = this->compound->srmId();
	}
}
