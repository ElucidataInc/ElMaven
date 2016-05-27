#include "mzMassCalculator.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "mzUtils.h"

using namespace mzUtils;
using namespace std;

/*---------- function to get molar weight of an element or group --------*/
double MassCalculator::getElementMass(string elmnt) {

	/* default behavior is to ignore string */
	double val_atome(0);

	/* Check for atoms */
	if (elmnt == "H") {
		val_atome = 1.0078250321;
	} else if (elmnt == "D") {
		val_atome = 2.01410178;
	} else if (elmnt == "C") {
		val_atome = 12.00000000;
	} else if (elmnt == "N") {
		val_atome = 14.0030740052;
	} else if (elmnt == "O") {
		val_atome = 15.9949146221;
	} else if (elmnt == "S") {
		val_atome = 31.97207069;
	} else if (elmnt == "P") {
		val_atome = 30.97376151;
	} else if (elmnt == "F") {
		val_atome = 18.9984032;
	} else if (elmnt == "Na") {
		val_atome = 22.98976967;
	} else if (elmnt == "Mg") {
		val_atome = 24.98583702;
	} else if (elmnt == "Cl") {
		val_atome = 34.96885271;
	} else if (elmnt == "K") {
		val_atome = 38.96399867;
	} else if (elmnt == "Ca") {
		val_atome = 39.9625912;
	} else if (elmnt == "Se") {
		val_atome = 79.916521;
	} else if (elmnt == "As") {
		val_atome = 74.921596;
	}
	return (val_atome);
}
/*-----------------------------------------------------------------------*/

/*-------------- parsing function ---------------------------------------*/

map<string, int> MassCalculator::getComposition(string formula) {

	/* define allowed characters for formula */
	const string UPP("ABCDEFGHIKLMNOPRSTUVWXYZ");
	const string LOW("abcdefghiklmnoprstuy");
	const string COEFF("0123456789");

	/* define some variable */
	int SIZE = formula.length();
	map<string, int> atoms;

	/* parse the formula */
	for (int i = 0; i < SIZE; i++) {
		string bloc, coeff_txt;
		int coeff;

		/* start of symbol must be uppercase letter */
		if (UPP.find(formula[i]) != string::npos) {
			bloc = formula[i];
			if (LOW.find(formula[i + 1]) != string::npos) {
				bloc += formula[i + 1];
				i++;
			}
		}

		while (COEFF.find(formula[i + 1]) != string::npos) {
			coeff_txt += formula[i + 1];
			i++;
		}

		if (coeff_txt.length() > 0) {
			coeff = string2integer(coeff_txt);
		} else {
			coeff = 1;
		}

		/* compute normally if there was no open bracket */
		//cout << bloc <<  " " << coeff << endl;
		atoms[bloc] += coeff;
	}

	return (atoms);
	/* send back value to main.cpp */
}

double MassCalculator::computeMass(string formula) {
	map<string, int> atoms = getComposition(formula);
	map<string, int>::iterator itr;

	double mass = 0;
	for (itr = atoms.begin(); itr != atoms.end(); itr++) {
		mass += getElementMass((*itr).first) * (*itr).second;
	}
	return mass;
}

//input neutral formala with all the hydrogens and charge state of molecule
//output expected mass of the molecule after loss/gain of protons
double MassCalculator::computeMass(string formula, int charge) {
	double mass = computeMass(formula);
	return adjustMass(mass, charge);
}

double MassCalculator::adjustMass(double mass, int charge) {
	if (charge < 0) {
		return ((mass - -charge * HMASS + -charge * HMASS / 1836) / -charge);
	} else if (charge > 0) {
		return ((mass + charge * HMASS - charge * HMASS / 1836) / charge);
	} else if (charge == 0) {
		return mass;
	}
}

vector<Isotope> MassCalculator::computeIsotopes(string formula, int charge) {
	map<string, int> atoms = getComposition(formula);
	int CatomCount = atoms["C"];
	int NatomCount = atoms["N"];
	int SatomCount = atoms["S"];
	int HatomCount = atoms["H"];

	double parentMass = computeMass(formula, charge);
	charge = abs(charge);
	if (charge == 0)
		charge = 1;

	const double abC12 = 0.988930;
	const double abC13 = 0.011070;
	const double abN14 = 0.996337;
	const double abN15 = 0.003663;
	const double abS32 = 0.9502;
	const double abS34 = 0.0421;
	const double abH = 0.999885;
	const double abH2 = 0.00115;

	const double D_Delta = 2.01410178 - 1.0078250321;
	const double C_Delta = 13.0033548378 - 12.0;
	const double N_Delta = 15.0001088984 - 14.003074052;
	const double S_Delta = 33.96786690 - 31.97207100;

	vector<Isotope> isotopes;

	Isotope parent("C12 PARENT", parentMass);
	isotopes.push_back(parent);

	for (int i = 1; i <= CatomCount; i++) {
		Isotope x("C13-label-" + integer2string(i),
				parentMass + (i * C_Delta) / charge, i, 0, 0, 0);
		isotopes.push_back(x);
	}

	for (int i = 1; i <= NatomCount; i++) {
		Isotope x("N15-label-" + integer2string(i),
				parentMass + (i * N_Delta) / charge, 0, i, 0, 0);
		isotopes.push_back(x);
	}

	for (int i = 1; i <= SatomCount; i++) {
		Isotope x("S34-label-" + integer2string(i),
				parentMass + (i * S_Delta) / charge, 0, 0, i, 0);
		isotopes.push_back(x);
	}

	for (int i = 1; i <= HatomCount; i++) {
		Isotope x("D-label-" + integer2string(i),
				parentMass + (i * D_Delta) / charge, 0, 0, 0, i);
		isotopes.push_back(x);
	}

	for (int i = 1; i <= CatomCount; i++) {
		for (int j = 1; j <= NatomCount; j++) {
			string name = "C13N15-label-" + integer2string(i) + "-"
					+ integer2string(j);
			double mass = parentMass + (j * N_Delta) / charge
					+ (i * C_Delta) / charge;
			Isotope x(name, mass, i, j, 0, 0);
			isotopes.push_back(x);
		}
	}

	for (int i = 1; i <= CatomCount; i++) {
		for (int j = 1; j <= SatomCount; j++) {
			string name = "C13S34-label-" + integer2string(i) + "-"
					+ integer2string(j);
			double mass = parentMass + (j * S_Delta) / charge
					+ (i * C_Delta) / charge;
			Isotope x(name, mass, i, 0, j, 0);
			isotopes.push_back(x);
		}
	}

	for (int i = 0; i < isotopes.size(); i++) {
		Isotope& x = isotopes[i];
		int c = x.C13;
		int n = x.N15;
		int s = x.S34;
		int d = x.H2;

		isotopes[i].abundance = mzUtils::nchoosek(CatomCount, c)
				* pow(abC12, CatomCount - c) * pow(abC13, c)
				* mzUtils::nchoosek(NatomCount, n) * pow(abN14, NatomCount - n)
				* pow(abN15, n) * mzUtils::nchoosek(SatomCount, s)
				* pow(abS32, SatomCount - s) * pow(abS34, s)
				* mzUtils::nchoosek(HatomCount, d) * pow(abH2, HatomCount - d)
				* pow(abH2, d);
	}

	return isotopes;
}

void MassCalculator::enumerateMasses(double inputMass, double charge,
		double maxdiff, vector<Match*>& matches) {

	if (charge > 0)
		inputMass = inputMass * abs(charge) + HMASS * abs(charge);
	if (charge < 0)
		inputMass = inputMass * abs(charge) + HMASS * abs(charge);

	for (int c = 0; c < 30; c++) { //C
		if (c * 12 > inputMass)
			break;
		for (int n = 0; n < 30; n++) { //N
			if (c * 12 + n * 14 > inputMass)
				break;
			for (int o = 0; o < 30; o++) { //O
				if (c * 12 + n * 14 + o * 16 > inputMass)
					break;
				for (int p = 0; p < 6; p++) { //P
					for (int s = 0; s < 6; s++) { //S
						int hmax = c * 4 + o * 2 + n * 4 + p * 3 + s * 3;
						for (int h = 0; h < hmax; h++) { //H
							double du = ((c * 2 + n + p) + 2 - h) / 2;
							if (du < -0.5)
								continue;
							if (round(du / 0.5) != (du / 0.5))
								continue;

							double c12 = c * 12.0 + o * 15.9949146221
									+ n * 14.0030740052 + p * 30.97376151
									+ h * 1.0078250321 + s * 31.97207069;
							double diff = ppmDist(c12, inputMass);

							if (diff < maxdiff) {
								string name = prettyName(c, h, n, o, p, s);
								MassCalculator::Match* m =
										new MassCalculator::Match();
								m->name = name;
								m->mass = c12;
								m->diff = diff;
								m->compoundLink = NULL;
								matches.push_back(m);
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

/*-----------------------------------------------------------------------*/
