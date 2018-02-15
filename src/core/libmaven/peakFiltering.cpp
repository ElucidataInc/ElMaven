#include "peakFiltering.h"

PeakFiltering::PeakFiltering(MavenParameters *mavenParameters, bool isIsotope)
{
    _isIsotope = isIsotope; 
    _mavenParameters = mavenParameters;
}

void PeakFiltering::filter(vector<EIC*> &eics)
{

    for (unsigned int i = 0; i < eics.size(); i++)
    {
        filter(eics[i]->peaks);
    }

}

void PeakFiltering::filter(EIC *eic)
{
    filter(eic->peaks);
}

// TODO: Erasing takes linear time. We can make copy
// of filtered peaks and return these to caller method
// or should do something else.
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

    if (_isIsotope)
    {
        if (_mavenParameters->minIsotopicPeakQuality > peak.quality)
        {
            return true;
        }
    }
    else
    {
        if (_mavenParameters->minPeakQuality > peak.quality)
        {
            return true;
        }
    }

    return false;
}
