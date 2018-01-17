#include "peakFiltering.h"

PeakFiltering::PeakFiltering(MavenParameters *mavenParameters)
{
    _mavenParameters = mavenParameters;
}

void PeakFiltering::filter(vector<Peak> &peaks)
{

    unsigned int i = 0;
    while (i < peaks.size())
    {
        if (filter(peaks[i]))
        {
            peaks.erase(peaks.begin() + i);
        }
        else
        {
            ++i;
        }
    }
}

bool PeakFiltering::filter(Peak &peak)
{

    if (_mavenParameters->minPeakQuality > peak.quality)
    {
        return true;
    }

    return false;
}
