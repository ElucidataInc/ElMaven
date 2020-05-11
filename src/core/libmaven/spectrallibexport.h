#ifndef SPECTRALLIBEXPORT_H
#define SPECTRALLIBEXPORT_H

#include "Compound.h"
#include "standardincludes.h"

class PeakGroup;

class SpectralLibExport
{
public:
    enum Format {
        Nist,
        Mascot
    };

    SpectralLibExport(std::string filePath,
                      Format format,
                      int limitNumPeaks = 20,
                      float scaleToMax = 1000.0f);
    void writePeakGroupData(PeakGroup* group);

private:
    std::string _filePath;
    Format _format;
    int _scaleToMax;
    int _limitNumPeaks;

    void _writePeakGroupAsMsp(PeakGroup* group);
    void _writePeakGroupAsMgf(PeakGroup* group);
};

#endif // SPECTRALLIBEXPORT_H
