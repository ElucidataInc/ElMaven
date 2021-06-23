#include "datastructures/adduct.h"
#include "datastructures/isotope.h"
#include "mzMassCalculator.h"
#include "constants.h"
#include "Compound.h"
#include "elementMass.h"
#include "masscutofftype.h"
#include "mzSample.h"
#include "mzUtils.h"

using namespace mzUtils;
using namespace std;

MassCalculator::IonizationType MassCalculator::ionizationType = MassCalculator::ESI;

ElementMass MassCalculator::elementMass;

Adduct* MassCalculator::PlusHAdduct = new Adduct("[M+H]+", 1, 1, PROTON_MASS);
Adduct* MassCalculator::MinusHAdduct = new Adduct("[M-H]-", 1, -1, -PROTON_MASS);
Adduct* MassCalculator::ZeroMassAdduct = new Adduct("[M]", 1, 1, 0.0f);

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

vector<Isotope> MassCalculator::computeIsotopes(string formula,
                                                int charge,
                                                bool C13Flag,
                                                bool N15Flag,
                                                bool S34Flag,
                                                bool D2Flag,
                                                bool O18Flag,
                                                Adduct* adduct)
{
    map<string, int> atoms = getComposition(formula);
    int CatomCount = atoms[C_STRING_ID];
    int NatomCount = atoms[N_STRING_ID];
    int SatomCount = atoms[S_STRING_ID];
    int HatomCount = atoms[H_STRING_ID];
    int OatomCount = atoms[O_STRING_ID];

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

    if (C13Flag && O18Flag) {
        for (int i = 1; i <= CatomCount; i++) {
            for (int j = 1; j <= OatomCount; j++) {
                string name = C13O18_LABEL + integer2string(i) + "-" + integer2string(j);
                double mass = parentMass + (j * O_MASS_DELTA) + (i * C_MASS_DELTA);
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

    if (O18Flag) {
        for (int i = 1; i <= OatomCount; i++) {
            Isotope x(O18_LABEL + integer2string(i),
                    parentMass + (i * O_MASS_DELTA), 0, 0, i, 0);
            isotopes.push_back(x);
        }
    }

    for (unsigned int i = 0; i < isotopes.size(); i++) {
        Isotope& x = isotopes[i];
        int c = x.C13;
        int n = x.N15;
        int s = x.S34;
        int d = x.H2;
        int o = x.O18;

        if (adduct != nullptr) {
            isotopes[i].mass = adduct->computeAdductMz(isotopes[i].mass);
        } else {
            isotopes[i].mass = adjustMass(isotopes[i].mass,charge);
        }

        isotopes[i].abundance =
            mzUtils::nchoosek(CatomCount, c) *
            pow(C12_ABUNDANCE, CatomCount - c) * pow(C13_ABUNDANCE, c) *
            mzUtils::nchoosek(NatomCount, n) *
            pow(N14_ABUNDANCE, NatomCount - n) * pow(N15_ABUNDANCE, n) *
            mzUtils::nchoosek(SatomCount, s) *
            pow(S32_ABUNDANCE, SatomCount - s) * pow(S34_ABUNDANCE, s) *
            mzUtils::nchoosek(HatomCount, d) *
            pow(H_ABUNDANCE, HatomCount - d) * pow(H2_ABUNDANCE, d) *
            mzUtils::nchoosek(OatomCount, o) *
            pow(O18_ABUNDANCE, OatomCount - o) * pow(O18_ABUNDANCE, o);
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
