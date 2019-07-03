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

QString SpectralHit::getProteinIds()
{
    QStringList ids = proteins.uniqueKeys();
    return (ids.join(";"));
}
QStringList SpectralHit::getProteins() { return proteins.uniqueKeys(); }

QString SpectralHit::getUnchargedPeptideString()
{
    int slashIndx = fragmentId.lastIndexOf('/');
    if (slashIndx != 0) {
        return fragmentId.left(slashIndx);
    } else {
        return fragmentId;
    }
}

QString SpectralHit::getModPeptideString()
{
    QString peptide = this->unmodPeptideSeq;
    QString modPeptideSeq;
    for (int i = 0; i < peptide.length(); i++) {
        modPeptideSeq += peptide[i];
        QString sign;
        if (mods.count(i)) {
            if (mods[i] > 0)
                sign = "+";
            modPeptideSeq +=
                ("[" + sign + QString::number(mzUtils::round(mods[i]))
                 + "]");
        }
    }
    modPeptideSeq = modPeptideSeq + "/" + QString::number(this->charge);
    return modPeptideSeq;
}
