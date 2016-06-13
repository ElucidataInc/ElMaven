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

// EI    M+ M-
// Cl    M+H M+X
// ACPI  M+H M+X M-X
// FI    M+H  M+X
// ESI    [M+nH]^n+  [M-nX]^n-
// FAB   M+X M+N
// ACPI  M+H  M+X

/**
 * @class MassCalculator
 * @ingroup libmaven
 * @brief Mass Calculator class.
 * @author Elucidata
 */
class MassCalculator {


    public:
        enum IonizationType { ESI=0, EI=1};

        static IonizationType ionizationType;

    typedef struct {
        std::string name;
        double mass;
        double diff;
		Compound* compoundLink;
    } Match;

    MassCalculator(){};

    /**
     * [computeNeutralMass description]
     * @method computeNeutralMass
     * @param  formula            [description]
     * @return                    [description]
     */
    double computeNeutralMass(string formula);

    /**
     * [input is neutral formala with all the hydrogens and charge state of molecule.]
     * @method computeMass
     * @param  formula     [description]
     * @param  polarity    [description]
     * @return             [expected mass of the molecule after loss/gain of protons]
     */
    double computeMass(string formula, int polarity);
    /**
     * [computeC13IsotopeMass description]
     * @method computeC13IsotopeMass
     * @param  formula               [description]
     * @return                       [description]
     */
    double computeC13IsotopeMass(string formula);

    map<string,double>computeLabeledMasses(string formula, int polarity);
    map<string,double>computeLabeledAbundances(string formula);


    /**
     * [getComposition parsing function]
     * @method getComposition
     * @param  elmnt          string formula
     * @return                map of string and int.
     */
    map<string,int> getComposition(string formula);

    /**
     * [matchMass description]
     * @method matchMass
     * @param  mass      [description]
     * @param  ppm       [description]
     */
    void matchMass(double mass, double ppm);

    /**
     * [prettyName description]
     * @method prettyName
     * @param  c          [description]
     * @param  h          [description]
     * @param  n          [description]
     * @param  o          [description]
     * @param  p          [description]
     * @param  s          [description]
     * @return            [description]
     */
    string prettyName(int c, int h, int n, int o, int p, int s);

    /**
     * [enumerateMasses description]
     * @method enumerateMasses
     * @param  inputMass       [description]
     * @param  charge          [description]
     * @param  maxdiff         [description]
     * @param  matches         [description]
     */
    void enumerateMasses(double inputMass, double charge, double maxdiff, vector<Match*>& matches);


    vector<Isotope> computeIsotopes(string formula, int polarity);

    /**
     * [adjustMass description]
     * @method adjustMass
     * @param  mass       [description]
     * @param  charge     [description]
     * @return            [description]
     */
    double adjustMass(double mass,int charge);

    /**
     * [compDiff description]
     * @method compDiff
     * @param  a        [description]
     * @param  b        [description]
     * @return          [description]
     */
    static bool compDiff(const Match* a, const Match* b ) { return a->diff < b->diff; }


    private:
        /**
         * [getElementMass function to get molar weight of an element or group]
         * @method getElementMass
         * @param  elmnt          string element
         * @return                double mass of element.
         */
        double getElementMass(string elmnt);

};

#endif
