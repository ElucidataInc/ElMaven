#ifndef SPECTRALHIT
#define SPECTRALHIT

#include "stable.h"

class Scan;

class ProteinHit {
    public:
        int id;
        QString accession;
        bool isDecoy;
        int cluster;
        int proteinGroup;
        int length;
        QString description;
        QString sequence;
        QString geneSymbol;

        ProteinHit() {  id=-1; isDecoy=false; cluster=-1; proteinGroup=-1; length=-1;}
};

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
    QString unmodPeptideSeq;
    bool isFocused;
    float rt;
    int id;
    QMap<QString, int> proteins;
    QMap<int, float> mods;

    SpectralHit();

    double getMaxIntensity();
    double getMinMz();
    double getMaxMz();

    QString getProteinIds();
    QStringList getProteins();
    QString getUnchargedPeptideString();
    QString getModPeptideString();

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
Q_DECLARE_METATYPE(ProteinHit*)

#endif
