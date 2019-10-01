#include "MgfFile.h"

namespace mgf {

MgfFile::MgfFile() : Collection<MgfSpectrum>() {
    this-> clear();
}
void MgfFile::getHeader(MgfHeader& header) {
    header = header_;
}
MgfHeader& MgfFile::getHeaderRef() {
    return header_;
}
void MgfFile::setHeader(const MgfHeader& header) {
    header_ = header;
}
void MgfFile::clear() {
    header_.clear();
    Collection<MgfSpectrum>::clear();
}

std::ostream& operator<<(std::ostream& os, const MgfFile& mgf) {
    os << mgf.header_;
    for (MgfFile::const_iterator i = mgf.begin(); i != mgf.end(); ++i) {
        os << '\n' << *i;
    }
    return os;
}

}

