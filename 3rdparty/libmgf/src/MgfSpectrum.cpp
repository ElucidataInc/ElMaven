#include "MgfSpectrum.h"

#include <algorithm>
#include <cstdlib> // for std::abs(int)
#include <iostream>
#include <sstream>
#include <utility>

namespace mgf {

MgfSpectrum::MgfSpectrum() : Collection<MassAbundancePair>() {
    this->clear();
    scans_ = std::make_pair(-1, -1);
    rtinseconds_ = std::make_pair(-1.0, -1.0);
}

std::vector<int> MgfSpectrum::getCHARGE(void) const {
    std::vector<int> trueCharges;
    std::transform(std::begin(charges_),
                   std::end(charges_),
                   std::back_inserter(trueCharges),
                   [this](const int value) {
                       int ionization = ionmode_ == "negative" ? -1 : 1;
                       return (ionization * abs(value));
                   });
    return trueCharges;
}
void MgfSpectrum::setCHARGE(const std::vector<int>& charges) {
    charges_ = charges;
}

std::string MgfSpectrum::getCOMP(void) const {
    return comp_;
}
void MgfSpectrum::setCOMP(const std::string& comp) {
    comp_ = comp;
}

std::string MgfSpectrum::getETAG(void) const {
    return etag_;
}
void MgfSpectrum::setETAG(const std::string& etag) {
    etag_ = etag;
}

std::string MgfSpectrum::getINSTRUMENT(void) const {
    return instrument_;
}
void MgfSpectrum::setINSTRUMENT(const std::string& instrument) {
    instrument_ = instrument;
}

void MgfSpectrum::getIONS(std::vector<MassAbundancePair>& ions) const {
    ions = c_;
}
void MgfSpectrum::setIONS(const std::vector<MassAbundancePair>& ions) {
    c_ = ions;
}

std::string MgfSpectrum::getIT_MODS(void) const {
    return it_mods_;
}
void MgfSpectrum::setIT_MODS(const std::string& it_mods) {
    it_mods_ = it_mods;
}

std::pair<double, double> MgfSpectrum::getPEPMASS(void) const {
    return pepmass_;
}
void MgfSpectrum::setPEPMASS(const std::pair<double, double> pepmass) {
    pepmass_ = pepmass;
}

std::pair<double, double> MgfSpectrum::getRTINSECONDS(void) const {
    return rtinseconds_;
}
void MgfSpectrum::setRTINSECONDS(const std::pair<double,double>& rtinseconds) {
    rtinseconds_ = rtinseconds;
}
void MgfSpectrum::setRTINSECONDS(const double rtinseconds) {
    setRTINSECONDS(std::make_pair(rtinseconds, -1.0));
}

std::pair<int, int> MgfSpectrum::getSCANS(void) const {
    return scans_;  
}
void MgfSpectrum::setSCANS(const std::pair<int, int>& scans) {
    scans_ = scans;
}
void MgfSpectrum::setSCANS(const int scan) {
    setSCANS(std::make_pair(scan, -1));
}

std::string MgfSpectrum::getSEQ(void) const {
    return seq_ ;
}
void MgfSpectrum::setSEQ(const std::string& seq) {
    seq_ = seq;
}

std::string MgfSpectrum::getTAG(void) const {
    return tag_;
}
void MgfSpectrum::setTAG(const std::string& tag) {
    tag_ = tag;
}

std::string MgfSpectrum::getTITLE(void) const {
    return title_;
}
void MgfSpectrum::setTITLE(const std::string& title) {
    title_ = title;
}

double MgfSpectrum::getTOL(void) const {
    return tol_;
}
void MgfSpectrum::setTOL(const double tol) {
    tol_ = tol;
}

std::string MgfSpectrum::getTOLU(void) const {
    return tolu_;
}
void MgfSpectrum::setTOLU(const std::string& tolu) {
    tolu_ = tolu;
}

void MgfSpectrum::clear() {
    charges_.clear();
    scans_ = std::make_pair(-1, -1);
    comp_ = etag_ = instrument_ = it_mods_;
    seq_ = tag_ = title_ = tolu_ = "";
    pepmass_ = std::make_pair(0.0, 0.0);
    rtinseconds_ = std::make_pair(-1.0, -1.0);
    tol_ = 0.0;
    Collection<MassAbundancePair>::clear();
}

int MgfSpectrum::getMSLEVEL(void) const
{
    return mslevel_;
}

void MgfSpectrum::setMSLEVEL(const int mslevel)
{
    mslevel_ = mslevel;
}

std::string MgfSpectrum::getFILENAME(void) const {
    return filename_;
}

void MgfSpectrum::setFILENAME(const std::string& filename) {
    filename_ = filename;
}

std::string MgfSpectrum::getIONMODE(void) const {
    return ionmode_;
}

void MgfSpectrum::setIONMODE(const std::string& ionmode) {
    ionmode_ = ionmode;
}

std::string MgfSpectrum::getORGANISM(void) const {
    return organism_;
}

void MgfSpectrum::setORGANISM(const std::string& organism) {
    organism_ = organism;
}

std::string MgfSpectrum::getSMILES(void) const {
    return smiles_;
}

void MgfSpectrum::setSMILES(const std::string& smileString) {
    smiles_ = smileString;
}

std::string MgfSpectrum::getINCHI(void) const {
    return inchi_;
}

void MgfSpectrum::setINCHI(const std::string& inchi) {
    inchi_ = inchi;
}

std::ostream& operator<<(std::ostream& os, const MgfSpectrum& mgf) {
    // start with title, then A-Z
    os << "BEGIN IONS" << '\n';
    if (!mgf.title_.empty())
        os << "TITLE=" << mgf.title_ << '\n';
    if (!mgf.charges_.empty()) {
        os << "CHARGE=";
        typedef std::vector<int>::const_iterator VICI;
        for (VICI i = mgf.charges_.begin(); i != mgf.charges_.end(); ++i) {
            if (i != mgf.charges_.begin()) {
                os << ',';
            }
            os << std::abs(*i);
            if (*i > 0) {
                os << '+';
            } else {
                os << '-';
            }
        }
        os << '\n';
    }
    if (!mgf.comp_.empty())
        os << "COMP=" << mgf.comp_ << '\n';
    if (!mgf.etag_.empty())
        os << "ETAG=" << mgf.etag_ << '\n';
    if (!mgf.instrument_.empty())
        os << "INSTRUMENT=" << mgf.instrument_ << '\n';
    if (!mgf.it_mods_.empty())
        os << "IT_MODS=" << mgf.it_mods_ << '\n';
    if (mgf.pepmass_.first > 0.0) {
        os << "PEPMASS=" << mgf.pepmass_.first;
        if (mgf.pepmass_.second > 0.0) {
            os << " " << mgf.pepmass_.second;
        }
        os << '\n';
    }
    if (mgf.rtinseconds_.first != -1.0) {
        os << "RTINSECONDS=" << mgf.rtinseconds_.first;
        if (mgf.rtinseconds_.second != -1.0) {
            os << "-" << mgf.rtinseconds_.second;
        }
        os << '\n';
    }
    if (mgf.scans_.first != -1) {
        os << "SCANS=" << mgf.scans_.first;
        if (mgf.scans_.second != -1) {
            os << "-" << mgf.scans_.second;
        }
        os << '\n';
    }
    if (!mgf.seq_.empty())
        os << "SEQ=" << mgf.seq_ << '\n';
    if (!mgf.tag_.empty())
        os << "TAG=" << mgf.tag_ << '\n';
    if (mgf.tol_ > 0.0)
        os << "TOL=" << mgf.tol_ << '\n';
    if (!mgf.tolu_.empty())
        os << "TOLU=" << mgf.tolu_ << '\n';
    if (mgf.mslevel_ > 0)
        os << "MSLEVEL=" << mgf.mslevel_ << '\n';
    if (!mgf.filename_.empty())
        os << "FILENAME=" << mgf.filename_ << '\n';
    if (!mgf.ionmode_.empty())
        os << "IONMODE=" << mgf.ionmode_ << '\n';
    if (!mgf.organism_.empty())
        os << "ORGANISM=" << mgf.organism_ << '\n';
    if (!mgf.smiles_.empty())
        os << "SMILES=" << mgf.smiles_ << '\n';
    if (!mgf.inchi_.empty())
        os << "INCHI=" << mgf.inchi_ << '\n';
    for (MgfSpectrum::const_iterator i = mgf.begin(); i != mgf.end(); ++i) {
        os << i->first << " " << i->second << '\n';
    }
    os << "END IONS" << '\n';
    return os;
}

} // namespace mgf

