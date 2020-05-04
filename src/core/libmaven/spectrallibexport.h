#ifndef SPECTRALLIBEXPORT_H
#define SPECTRALLIBEXPORT_H

#include "Compound.h"
#include "standardincludes.h"

class PeakGroup;

class SpectralLibExport
{
public:
    enum Format {
        NIST,
        Mascot
    };

    SpectralLibExport(std::string filePath, Format format);
    void writePeakGroupData(PeakGroup* group);

private:
    std::string _filePath;
    Format _format;

    void _writePeakGroupAsMsp(PeakGroup* group);
    void _writePeakGroupAsMgf(PeakGroup* group);
};

#endif // SPECTRALLIBEXPORT_H
