#include "mzMassCalculator.h"
#include "constants.h"
#include "Compound.h"
#include "Peptide.hpp"
#include "elementMass.h"
#include "masscutofftype.h"
#include "mzSample.h"
#include "mzUtils.h"

using namespace mzUtils;
using namespace std;

MassCalculator::IonizationType MassCalculator::ionizationType = MassCalculator::ESI;

ElementMass MassCalculator::elementMass;

double MassCalculator::getElementMass(string elmnt) {
    double val_atome(0);
    if (elementMass.elementMassMap.count(elmnt)  == 1) {
        val_atome = elementMass.elementMassMap[elmnt];
    }
    return val_atome;
}



map<string, int> MassCalculator::getComposition(string formula) {

    /* define some variable */
    int SIZE = formula.length();
    map<string, int> atoms;

    /* parse the formula */
    for (int i = 0; i < SIZE; i++) {
        string bloc, coeff_txt;
        int coeff;

        /* start of symbol must be uppercase letter */
        if (CHE_FORMULA_ALPHA_UPP.find(formula[i]) != string::npos) {
            bloc = formula[i];
            if (CHE_FORMULA_ALPHA_LOW.find(formula[i + 1]) != string::npos) {
                bloc += formula[i + 1];
                i++;
            }
        }

        while (CHE_FORMULA_COFF.find(formula[i + 1]) != string::npos) {
            coeff_txt += formula[i + 1];
            i++;
        }

        if (coeff_txt.length() > 0) {
            coeff = string2integer(coeff_txt);
        } else {
            coeff = 1;
        }

        /* compute normally if there was no open bracket */
        // cout << bloc <<  " " << coeff << endl;
        atoms[bloc] += coeff;
    }

    return (atoms);
    /* send back value to main.cpp */
}

double MassCalculator::computeNeutralMass(string formula) {
    map<string, int> atoms = getComposition(formula);
    map<string, int>::iterator itr;

    double mass = 0;
    for (itr = atoms.begin(); itr != atoms.end(); itr++) {
        mass += getElementMass((*itr).first) * (*itr).second;
    }
    return mass;
}

double MassCalculator::adjustMass(double mass, int charge) {
    if (mass == 0) return 0;
    if (MassCalculator::ionizationType == EI and charge != 0) {
        return ((mass - charge * ELECTRON_MASS) /
                abs(charge));  // lost of electrons
    }

    if (charge == 0) {
        return mass;
    } else {
        return (mass + charge * PROTON_MASS) / abs(charge);
    }
}

double MassCalculator::computeMass(string formula, int charge) {
    double mass = computeNeutralMass(formula);
    return adjustMass(mass, charge);
}

vector<Isotope> MassCalculator::computeIsotopes(
    string formula,
    int charge,
    bool C13Flag,
    bool N15Flag,
    bool S34Flag,
    bool D2Flag
)
{
    map<string, int> atoms = getComposition(formula);
    int CatomCount = atoms[C_STRING_ID];
    int NatomCount = atoms[N_STRING_ID];
    int SatomCount = atoms[S_STRING_ID];
    int HatomCount = atoms[H_STRING_ID];

    vector<Isotope> isotopes;
    double parentMass = computeNeutralMass(formula);

    Isotope parent(C12_PARENT_LABEL, parentMass);
    isotopes.push_back(parent);

    if(C13Flag) {
        for (int i = 1; i <= CatomCount; i++) {
            Isotope x(C13_LABEL + integer2string(i),
                    parentMass + (i * C_MASS_DELTA), i, 0, 0, 0);
            isotopes.push_back(x);
        }
    }

    if(C13Flag && D2Flag) {
        for (int i = 1; i <= CatomCount; i++) {
            for (int j = 1; j <= HatomCount; j++) {
                string name = C13H2_LABEL + integer2string(i) + "-" + integer2string(j);
                double mass = parentMass + (j * D_MASS_DELTA) + (i * C_MASS_DELTA);
                Isotope x(name, mass, i, 0, 0, j);
                isotopes.push_back(x);
            }
        }
    }

    if(C13Flag && N15Flag) {
        for (int i = 1; i <= CatomCount; i++) {
            for (int j = 1; j <= NatomCount; j++) {
                string name = C13N15_LABEL + integer2string(i) + "-" + integer2string(j);
                double mass = parentMass + (j * N_MASS_DELTA) + (i * C_MASS_DELTA);
                Isotope x(name, mass, i, j, 0, 0);
                isotopes.push_back(x);
            }
        }
    }

    if(C13Flag && S34Flag) {
        for (int i = 1; i <= CatomCount; i++) {
            for (int j = 1; j <= SatomCount; j++) {
                string name = C13S34_LABEL + integer2string(i) + "-" + integer2string(j);
                double mass = parentMass + (j * S_MASS_DELTA) + (i * C_MASS_DELTA);
                Isotope x(name, mass, i, 0, j, 0);
                isotopes.push_back(x);
            }
        }
    }

    if(D2Flag) {
        for (int i = 1; i <= HatomCount; i++) {
            Isotope x(H2_LABEL + integer2string(i), parentMass + (i * D_MASS_DELTA),
                    0, 0, 0, i);
            isotopes.push_back(x);
        }
    }

    if(N15Flag) {
        for (int i = 1; i <= NatomCount; i++) {
            Isotope x(N15_LABEL + integer2string(i),
                    parentMass + (i * N_MASS_DELTA), 0, i, 0, 0);
            isotopes.push_back(x);
        }
    }

    if(S34Flag) {
        for (int i = 1; i <= SatomCount; i++) {
            Isotope x(S34_LABEL + integer2string(i),
                    parentMass + (i * S_MASS_DELTA), 0, 0, i, 0);
            isotopes.push_back(x);
        }
    }

    for (unsigned int i = 0; i < isotopes.size(); i++) {
        Isotope& x = isotopes[i];
        int c = x.C13;
        int n = x.N15;
        int s = x.S34;
        int d = x.H2;

        isotopes[i].mass = adjustMass(isotopes[i].mass,charge);

        isotopes[i].abundance =
            mzUtils::nchoosek(CatomCount, c) *
            pow(C12_ABUNDANCE, CatomCount - c) * pow(C13_ABUNDANCE, c) *
            mzUtils::nchoosek(NatomCount, n) *
            pow(N14_ABUNDANCE, NatomCount - n) * pow(N15_ABUNDANCE, n) *
            mzUtils::nchoosek(SatomCount, s) *
            pow(S32_ABUNDANCE, SatomCount - s) * pow(S34_ABUNDANCE, s) *
            mzUtils::nchoosek(HatomCount, d) *
            pow(H_ABUNDANCE, HatomCount - d) * pow(H2_ABUNDANCE, d);
    }

    return isotopes;
}

void MassCalculator::enumerateMasses(double inputMass, double charge,
    MassCutoff *massCutoff, vector<Match*>& matches) {
    if (charge > 0)
        inputMass = inputMass * abs(charge) - H_MASS * abs(charge);
    if (charge < 0)
        inputMass = inputMass * abs(charge) + H_MASS * abs(charge);

    for (int c = 0; c < 30; c++) {  // C
        if (c * 12 > inputMass) break;
        for (int n = 0; n < 30; n++) {  // N
            if (c * 12 + n * 14 > inputMass) break;
            for (int o = 0; o < 30; o++) {  // O
                if (c * 12 + n * 14 + o * 16 > inputMass) break;
                for (int p = 0; p < 6; p++) {	  // P
                    for (int s = 0; s < 6; s++) {  // S
                        int hmax = c * 4 + o * 2 +
                            n * 4 + p * 3 +
                            s * 3;
                        for (int h = 0; h < hmax;
                                h++) {  // H
                            double du =
                                ((c * 2 + n + p) +
                                 2 - h) /
                                2;
                            if (du < -0.5) continue;
                            if (round(du / 0.5) !=
                                    (du / 0.5))
                                continue;

                            double c12 =
                                c * C12_MASS +
                                o * O16_MASS +
                                n * N14_MASS +
                                p * P31_MASS +
                                h * H_MASS +
                                s * S32_MASS;
                            double diff = massCutoffDist(
                                    c12, inputMass,massCutoff);

                            if (diff < massCutoff->getMassCutoff()) {
                                string name =
                                    prettyName(
                                            c, h, n,
                                            o, p,
                                            s);
                                MassCalculator::
                                    Match* m =
                                    new MassCalculator::
                                    Match();
                                m->name = name;
                                m->mass = c12;
                                m->diff = diff;
                                m->compoundLink =
                                    NULL;
                                matches
                                    .push_back(
                                            m);
                            }
                        }
                    }
                }
            }
        }
    }
    std::sort(matches.begin(), matches.end(), compDiff);
}

std::string MassCalculator::prettyName(int c, int h, int n, int o, int p,
        int s) {
    char buf[1000];
    string name;
    if (c != 0) {
        name += "C";
        if (c > 1) {
            sprintf(buf, "%d", c);
            name += buf;
        }
    }
    if (h != 0) {
        name += "H";
        if (h > 1) {
            sprintf(buf, "%d", h);
            name += buf;
        }
    }
    if (n != 0) {
        name += "N";
        if (n > 1) {
            sprintf(buf, "%d", n);
            name += buf;
        }
    }
    if (o != 0) {
        name += "O";
        if (o > 1) {
            sprintf(buf, "%d", o);
            name += buf;
        }
    }
    if (p != 0) {
        name += "P";
        if (p > 1) {
            sprintf(buf, "%d", p);
            name += buf;
        }
    }
    if (s != 0) {
        name += "S";
        if (s > 1) {
            sprintf(buf, "%d", s);
            name += buf;
        }
    }
    return name;
}

/*
   @author: Sahil
   */
//TODO: Sahil, Added while merging point
string MassCalculator::peptideFormula(const string& peptideSeq) {
    int C=0; int H=0; int O=0; int N=0; int S=0; int P=0;
    bool noCmod=true;
    bool noNmod=true;

    for(unsigned int i=0; i<peptideSeq.length(); i++ ) {
        char aa = peptideSeq[i];
        switch (aa)  {
            case 'A': C+=3; H+=5;  N+=1;  O+=1; break;
            case 'R': C+=6; H+=12; N+=4;  O+=1; break;
            case 'N': C+=4; H+=6;  N+=2;  O+=2; break;
            case 'D': C+=4; H+=5;  N+=1;  O+=3; break;
            case 'C': C+=3; H+=5;  N+=1;  O+=1;  S+=1; break;
            case 'Q': C+=5; H+=8;  N+=2;  O+=2; break;
            case 'E': C+=5; H+=7;  N+=1;  O+=3; break;
            case 'G': C+=2; H+=3;  N+=1;  O+=1; break;
            case 'H': C+=6; H+=7;  N+=3;  O+=1; break;
            case 'I': C+=6; H+=11; N+=1;  O+=1; break;
            case 'L': C+=6; H+=11; N+=1;  O+=1; break;
            case 'K': C+=6; H+=12; N+=2;  O+=1; break;
            case 'M': C+=5; H+=9;  N+=1;  O+=1; S+=1 ;break;
            case 'F': C+=9; H+=9;  N+=1;  O+=1; break;
            case 'P': C+=5; H+=7;  N+=1;  O+=1; break;
            case 'S': C+=3; H+=5;  N+=1;  O+=2; break;
            case 'T': C+=4; H+=7;  N+=1;  O+=2; break;
            case 'W': C+=11; H+=10;  N+=2;  O+=1; break;
            case 'Y': C+=9; H+=9;  N+=1;  O+=2; break;
            case 'V': C+=5; H+=9;  N+=1;  O+=1; break;
            default: cerr << "peptideComposition() unknown aa: " << aa << endl;
        }
    }

    Peptide pept(peptideSeq,0,"");
    map<string, unsigned int> presentModTypes;
    pept.getAllPresentModTypes(presentModTypes);

    map<string, unsigned int>::iterator itr;
    for(itr =presentModTypes.begin(); itr != presentModTypes.end(); itr++ ){
        unsigned int c= itr->second;
        if ( itr->first  == "C,Carbamidomethyl" ) {
            C+=(2*c); H+=(3*c); N+=(1*c); O+=(1*c);
        } else if ( itr->first.find("Oxidation") != string::npos)  {
            O+=c;
        } else if ( itr->first.find("Phospho") != string::npos )  {
            P+=c; O+=(3*c); H+=(1*c);
        } else {
            cerr << "Unknown formulat for modification: " << itr->first;
        }
    }
    if(C and noNmod and noCmod) { H+=2; O+=1;}

    //cerr << "prettyName=" << prettyName(C,H,N,O,P,S) << endl;
    return prettyName(C,H,N,O,P,S);
}
