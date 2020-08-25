#ifndef SPECTRALHIT
#define SPECTRALHIT

#include "stable.h"

class Scan;

class SpectralHit
{
    public:
    double score;
    Scan* scan;
    QString sampleName;
    QVector<double> mzList;
    QVector<double> intensityList;
    float productPPM;
    int matchCount;
    float precursorMz;
    float xcorr;
    float mvh;
    int rank;
    bool decoy;
    int charge;
    double massdiff;
    int scannum;
    QString compoundId;
    QString fragmentId;
    bool isFocused;
    float rt;
    int id;
    QMap<int, float> mods;

    SpectralHit();

    double getMaxIntensity();
    double getMinMz();
    double getMaxMz();

    static bool compScan(SpectralHit* a, SpectralHit* b)
    {
        return a->scan < b->scan;
    }

    static bool compPrecursorMz(SpectralHit* a, SpectralHit* b)
    {
        return a->precursorMz < b->precursorMz;
    }

    static bool compScore(SpectralHit* a, SpectralHit* b)
    {
        return a->score > b->score;
    }

    static bool compMVH(SpectralHit* a, SpectralHit* b)
    {
        return a->mvh > b->mvh;
    }
};

Q_DECLARE_METATYPE(SpectralHit*)

#endif
