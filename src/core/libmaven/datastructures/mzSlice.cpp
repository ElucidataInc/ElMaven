#include "mzSlice.h"
#include "Compound.h"

bool mzSlice::calculateMzMinMax(MassCutoff *compoundMassCutoffWindow, int charge)
{
	
	//Calculating the mzmin and mzmax
	if (!this->compound->formula.empty())
	{
		//Computing the mass if the formula is given
		double mass = MassCalculator::computeMass(this->compound->formula, charge);
		this->mzmin = mass - compoundMassCutoffWindow->massCutoffValue(mass);
		this->mzmax = mass + compoundMassCutoffWindow->massCutoffValue(mass);
	}
	else if (this->compound->mass > 0)
	{
		// Mass already present in the compound DB then using
		// it to find the mzmin and mzmax
		double mass = this->compound->mass;
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
	if (matchRtFlag && this->compound->expectedRt > 0)
	{
		this->rtmin = this->compound->expectedRt - compoundRTWindow;
		this->rtmax = this->compound->expectedRt + compoundRTWindow;
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
	if (!this->compound->srmId.empty())
	{
		this->srmId = this->compound->srmId;
	}
}
