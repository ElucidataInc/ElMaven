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
        if (_mavenParameters->minPeakQuality > peaks[i].quality)
        {
            peaks.erase(peaks.begin() + i);
        }
        else
        {
            ++i;
        }
    }
}