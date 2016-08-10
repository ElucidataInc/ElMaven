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
     * [computeNeutralMass ]
     * @method computeNeutralMass
     * @param  formula            []
     * @return                    []
     */
    double computeNeutralMass(string formula);

    /**
     * [input is neutral formala with all the hydrogens and charge state of molecule.]
     * @method computeMass
     * @param  formula     []
     * @param  polarity    []
     * @return             [expected mass of the molecule after loss/gain of protons]
     */
    double computeMass(string formula, int polarity);
    /**
     * [computeC13IsotopeMass ]
     * @method computeC13IsotopeMass
     * @param  formula               []
     * @return                       []
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
    static map<string,int> getComposition(string formula);

    /**
     * [matchMass ]
     * @method matchMass
     * @param  mass      []
     * @param  ppm       []
     */
    void matchMass(double mass, double ppm);

    /**
     * [prettyName ]
     * @method prettyName
     * @param  c          []
     * @param  h          []
     * @param  n          []
     * @param  o          []
     * @param  p          []
     * @param  s          []
     * @return            []
     */
    string prettyName(int c, int h, int n, int o, int p, int s);

    /**
     * [enumerateMasses ]
     * @method enumerateMasses
     * @param  inputMass       []
     * @param  charge          []
     * @param  maxdiff         []
     * @param  matches         []
     */
    void enumerateMasses(double inputMass, double charge, double maxdiff, vector<Match*>& matches);


    vector<Isotope> computeIsotopes(string formula, int polarity);

    /**
     * [adjustMass ]
     * @method adjustMass
     * @param  mass       []
     * @param  charge     []
     * @return            []
     */
    double adjustMass(double mass,int charge);

    /**
     * [compDiff ]
     * @method compDiff
     * @param  a        []
     * @param  b        []
     * @return          []
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
