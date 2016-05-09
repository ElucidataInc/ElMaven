#ifndef MASSCALC_H
#define MASSCALC_H

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <map>
#include "mzSample.h" 
#include "mzUtils.h"

class Compound;
class Isotope;

using namespace std;


class MassCalculator { 

    public:
    typedef struct {
        std::string name;
        double mass;
        double diff;
		Compound* compoundLink;
    } Match;


    MassCalculator(){};
    double computeMass(string formula);
    double computeMass(string formula, int polarity);
    double computeC13IsotopeMass(string formula);
    map<string,double>computeLabeledMasses(string formula, int polarity);
	map<string,double>computeLabeledAbundances(string formula);
    map<string,int> getComposition(string formula);
    void matchMass(double mass, double ppm);
    string prettyName(int c, int h, int n, int o, int p, int s);
    void enumerateMasses(double inputMass, double charge, double maxdiff, vector<Match*>& matches);
    static bool compDiff(const Match* a, const Match* b ) { return a->diff < b->diff; }
    vector<Isotope> computeIsotopes(string formula, int polarity);
	double adjustMass(double mass,int charge);

    private:
    double getElementMass(string elmnt);

};

#endif
