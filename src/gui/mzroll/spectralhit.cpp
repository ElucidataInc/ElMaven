#include "spectralhit.h"
#include "mzUtils.h"

SpectralHit::SpectralHit()
{
    scan = NULL;
    precursorMz = 0;
    matchCount = 0;
    productPPM = 0.0;
    score = 0;
    xcorr = 0;
    massdiff = 0;
    mvh = 0;
    rank = 0;
    decoy = false;
    charge = 0;
    scannum = 0;
    isFocused = false;
    rt = 0;
    id = 0;
}

double SpectralHit::getMaxIntensity()
{
    double maxIntensity = 0;
    Q_FOREACH (double x, intensityList)
        if (x > maxIntensity)
            maxIntensity = x;
    return maxIntensity;
}

double SpectralHit::getMinMz()
{
    double min = mzList.first();
    Q_FOREACH (double x, mzList)
        if (x < min)
            min = x;
    return min;
}

double SpectralHit::getMaxMz()
{
    double max = mzList.last();
    Q_FOREACH (double x, mzList)
        if (x > max)
            max = x;
    return max;
}
