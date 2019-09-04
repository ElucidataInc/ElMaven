#ifndef MASSCALC_H
#define MASSCALC_H

#include "elementMass.h"
#include "standardincludes.h"
#include "Fragment.h"

class Adduct;
class Compound;
class Isotope;
class MassCutoff;

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

        // TODO: We only need these declarations so as to be compatible with
        // MAVEN's projectDB exports. Once our "Database" and "Databases"
        // classes are merged we will be able to use all "findSpeciesBy_X" type
        // of methods from there itself, and remove this.
        static Adduct* PlusHAdduct;
        static Adduct* MinusHAdduct;
        static Adduct* ZeroMassAdduct;

        typedef struct {
            std::string name;
            double mass;
            double diff;
            double rtDiff;
            FragmentationMatchScore fragScore;
            Compound* compoundLink;
        } Match;


        /**
         * [computeNeutralMass ]
         * @method computeNeutralMass
         * @param  formula            []
         * @return                    []
         */
        static double computeNeutralMass(string formula);

        /**
         * [input is neutral formala with all the hydrogens and charge state of molecule.]
         * @method computeMass
         * @param  formula     []
         * @param  polarity    []
         * @return             [expected mass of the molecule after loss/gain of protons]
         */
        static double computeMass(string formula, int polarity);

        /**
         * [getComposition parsing function]
         * @method getComposition
         * @param  elmnt          string formula
         * @return                map of string and int.
         */
        static map<string,int> getComposition(string formula);


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
        static string prettyName(int c, int h, int n, int o, int p, int s);

        /**
         * [enumerateMasses ]
         * @method enumerateMasses
         * @param  inputMass       []
         * @param  charge          []
         * @param  maxdiff         []
         * @param  matches         []
         */
        void enumerateMasses(double inputMass, double charge, MassCutoff *massCutoff, vector<Match*>& matches);


        static vector<Isotope> computeIsotopes(string formula,
                                               int charge,
                                               bool C13Flag,
                                               bool N15Flag,
                                               bool S34Flag,
                                               bool D2Flag,
                                               Adduct *adduct = nullptr);

        /**
         * [adjustMass ]
         * @method adjustMass
         * @param  mass       []
         * @param  charge     []
         * @return            []
         */
        static double adjustMass(double mass,int charge);

        /**
         * [compDiff ]
         * @method compDiff
         * @param  a        []
         * @param  b        []
         * @return          []
         */
        static bool compDiff(const Match* a, const Match* b ) { return a->diff < b->diff; }

        string peptideFormula(const string& peptideSeq); //TODO: Sahil, Added while merging point


    private:
        /**
         * [getElementMass function to get molar weight of an element or group]
         * @method getElementMass
         * @param  elmnt          string element
         * @return                double mass of element.
         */
        static ElementMass elementMass;
        static double getElementMass(string elmnt);
        static void generateElementMassMap(string filename);

};

#endif
