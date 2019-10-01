#include "MgfHeader.h"
#include <cstdlib> // for std::abs(int)

namespace mgf {

std::string MgfHeader::getCLE(void) const {
    return existsOrEmpty(strings, "CLE");
}
void MgfHeader::setCLE(const std::string& cle) {
    strings["CLE"] = cle;
}

std::string MgfHeader::getCOM(void) const {
    return existsOrEmpty(strings, "COM");
}
void MgfHeader::setCOM(const std::string& com) {
    strings["COM"] = com;
}

std::string MgfHeader::getDB(void) const {
    return existsOrEmpty(strings, "DB");
}
void MgfHeader::setDB(const std::string& db) {
    strings["DB"] = db;
}

std::string MgfHeader::getFORMAT(void) const {
    return existsOrEmpty(strings, "FORMAT");
}
void MgfHeader::setFORMAT(const std::string& format) {
    strings["FORMAT"] = format;
}

std::string MgfHeader::getINSTRUMENT(void) const {
    return existsOrEmpty(strings, "INSTRUMENT");
}
void MgfHeader::setINSTRUMENT(const std::string& instrument) {
    strings["INSTRUMENT"] = instrument;
}

std::string MgfHeader::getIT_MODS(void) const {
    return existsOrEmpty(strings, "IT_MODS");
}
void MgfHeader::setIT_MODS(const std::string& it_mods) {
    strings["IT_MODS"] = it_mods;
}

std::string MgfHeader::getITOLU(void) const {
    return existsOrEmpty(strings, "ITOLU");
}
void MgfHeader::setITOLU(const std::string& itolu) {
    strings["ITOLU"] = itolu;
}

std::string MgfHeader::getMASS(void) const {
    return existsOrEmpty(strings, "MASS");
}
void MgfHeader::setMASS(const std::string& mass) {
    strings["MASS"] = mass;
}

std::string MgfHeader::getMODS(void) const {
    return existsOrEmpty(strings, "MODS");
}
void MgfHeader::setMODS(const std::string& mods) {
    strings["MODS"] = mods;
}

std::string MgfHeader::getQUANTITATION(void) const {
    return existsOrEmpty(strings, "QUANTITATION");
}
void MgfHeader::setQUANTITATION(const std::string& quantitation) {
    strings["QUANTITATION"] = quantitation;
}

std::string MgfHeader::getREPORT(void) const {
    return existsOrEmpty(strings, "REPORT");
}
void MgfHeader::setREPORT(const std::string& report) {
    strings["REPORT"] = report;
}

std::string MgfHeader::getREPTYPE(void) const {
    return existsOrEmpty(strings, "REPTYPE");
}
void MgfHeader::setREPTYPE(const std::string& reptype) {
    strings["REPTYPE"] = reptype;
}

std::string MgfHeader::getSEARCH(void) const {
    return existsOrEmpty(strings, "SEARCH");
}
void MgfHeader::setSEARCH(const std::string& search) {
    strings["SEARCH"] = search;
}

std::string MgfHeader::getTAXONOMY(void) const {
    return existsOrEmpty(strings, "TAXONOMY");
}
void MgfHeader::setTAXONOMY(const std::string& taxonomy) {
    strings["TAXONOMY"] = taxonomy;
}

std::string MgfHeader::getTOLU(void) const {
    return existsOrEmpty(strings, "TOLU");
}
void MgfHeader::setTOLU(const std::string& tolu) {
    strings["TOLU"] = tolu;
}

std::string MgfHeader::getUSEREMAIL(void) const {
    return existsOrEmpty(strings, "USEREMAIL");
}
void MgfHeader::setUSEREMAIL(const std::string& useremail) {
    strings["USEREMAIL"] = useremail;
}

std::string MgfHeader::getUSERNAME(void) const {
    return existsOrEmpty(strings, "USERNAME");
}
void MgfHeader::setUSERNAME(const std::string& username) {
    strings["USERNAME"] = username;
}

int MgfHeader::getPFA(void) const {
    return existsOrEmpty(ints, "PFA");
}
void MgfHeader::setPFA(const int pfa) {
    ints["PFA"] = pfa;
}

int MgfHeader::getDECOY(void) const {
    return existsOrEmpty(ints, "DECOY");
}
void MgfHeader::setDECOY(const int decoy) {
    ints["DECOY"] = decoy;
}

int MgfHeader::getERRORTOLERANT(void) const {
    return existsOrEmpty(ints, "ERRORTOLERANT");
}
void MgfHeader::setERRORTOLERANT(const int errortolerant) {
    ints["ERRORTOLERANT"] = errortolerant;
}

double MgfHeader::getPEP_ISOTOPE_ERROR(void) const {
    return existsOrEmpty(doubles, "PEP_ISOTOPE_ERROR");
}
void MgfHeader::setPEP_ISOTOPE_ERROR(const double pep_isotope_error) {
    doubles["PEP_ISOTOPE_ERROR"] = pep_isotope_error;
}

double MgfHeader::getITOL(void) const {
    return existsOrEmpty(doubles, "ITOL");
}
void MgfHeader::setITOL(const double itol) {
    doubles["ITOL"] = itol;
}

double MgfHeader::getPRECURSOR(void) const {
    return existsOrEmpty(doubles, "PRECURSOR");
}
void MgfHeader::setPRECURSOR(const double precursor) {
    doubles["PRECURSOR"] = precursor;
}

double MgfHeader::getSEG(void) const {
    return existsOrEmpty(doubles, "SEG");
}
void MgfHeader::setSEG(const double seg) {
    doubles["SEG"] = seg;
}

double MgfHeader::getTOL(void) const {
    return existsOrEmpty(doubles, "TOL");
}
void MgfHeader::setTOL(const double tol) {
    doubles["TOL"] = tol;
}

void MgfHeader::getCHARGE(std::vector<int>& charges) const {
    charges = charges_;
}
void MgfHeader::setCHARGE(const std::vector<int>& charges) {
    charges_ = charges;
}

void MgfHeader::getFRAMES(std::vector<int>& frames) const {
    frames = frames_;
}
void MgfHeader::setFRAMES(const std::vector<int>& frames) {
    frames_ = frames;
}

void MgfHeader::clear() {
    strings.clear();
    ints.clear();
    doubles.clear();
    charges_.clear();
    frames_.clear();
}

std::ostream& operator<<(std::ostream& os, const MgfHeader& mgf) {
    typedef std::map<std::string, std::string>::const_iterator MSSCI;
    for (MSSCI i = mgf.strings.begin(); i != mgf.strings.end() ; ++i) {
        os << i->first << '=' << i->second << '\n';
    }
    typedef std::map<std::string, int>::const_iterator MSICI;
    for (MSICI i = mgf.ints.begin(); i != mgf.ints.end() ; ++i) {
        os << i->first << '=' << i->second << '\n';
    }
    typedef std::map<std::string, double>::const_iterator MSDCI;
    for (MSDCI i = mgf.doubles.begin(); i != mgf.doubles.end() ; ++i) {
        os << i->first << '=' << i->second << '\n';
    }
    typedef std::vector<int>::const_iterator VICI;
    if (!mgf.charges_.empty()) {
        os << "CHARGE=";
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
    if (!mgf.frames_.empty()) {
        os << "FRAMES=";
        for (VICI i = mgf.frames_.begin(); i != mgf.frames_.end(); ++i) {
            if (i != mgf.frames_.begin()) {
                os << ',';
            }
            os << *i;
        }
        os << '\n';
    }
    return os;
}

} // namespace mgf

