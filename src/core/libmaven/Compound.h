#ifndef COMPOUND_H
#define COMPOUND_H

#include "constants.h"
#include "standardincludes.h"
#include "PeakGroup.h"

class FragmentationMatchScore;
class Fragment;

using namespace std;

/**
 *@brief  -   class to represent a compound
 *@detail  -  class Compound representing a compound. It will hold variable to
 *discribe a compound and also PeakGroup object it will belong to in addition
 *to other required functions for other operation
 */
class Compound{

    private:

        /**
         *@param - compund id
        */
        string _id;

        /**
         *@param -  name of compound
        */
        string _name;

        string _originalName;

        /**
         *@param -  formula of compound
        */
        string _formula;

        /**
         *@param   -  alias name of compound
        */
        string _alias;

        /**
        *@param -  srmId will hold filterLine string from mzxml file which represent type of
        *mass spec and ionization is used and other info as well
        */
        string _srmId;

        float _expectedRt;

        /**
         *@param  -   number of charge of compound (electron loss or gain)
        */
        int _charge;

        /**
         *@param   -  mass of this compund
        */
        float _mz;

        /**
         *@param  -  QQQ parent ion  mz value
        */
        float _precursorMz;

        /**
         *@param  -  QQQ child ion   mz value
        */
        float _productMz;

        /**
         *@param  -   QQQ collision energy of this compound
        */
        float _collisionEnergy;


        /**
         * @brief Name of database this compound belongs to for example KEGG,
         * ECOCYC, etc.
         */
        string _db;

        float _neutralMass;

        /**
         * @brief Categories of this compound. For e.g., amino acids, nucleic
         * acids, peptide, etc.
         */
        vector<string> _category;

        /**
         * @brief Vector of m/z values of fragments generated from this compund.
         */
        vector<float> _fragmentMzValues;

        /**
         * @brief Vector of intensities of fragments generated from this
         * compund.
         */
        vector<float> _fragmentIntensities;

        /**
         * @brief Collection of indices of fragment values mapping to its
         * ionisation type information.
         */
        map<int, string> _fragmentIonTypes;

        /**
         * @brief A simple string in form of a line notation for describing the
         * structure of chemical species using short ASCII strings.
         */
        string _smileString;

        float _logP; // TODO: find out what value this is.

        // TODO: from MAVEN (upstream), find out what this is
        bool _virtualFragmentation;

        /**
         * @brief A note containing any miscellaneous details for this compound.
         */
        string _note;

        // TODO: from MAVEN (upstream) decoy compound?
        bool _isDecoy;

        /** QQQ mapping */
        string _method_id;   /**@param  -  TODO*/

        /**
         * TODO
         */
        int _transition_id;

        /**
         *@param -  kegg_id-    Kyoto Encyclopedia of
         *Genes and Genomes id
         */
        string _kegg_id;

        /**
         *@param  -  pubchem_id -    PubChem id
         */
        string _pubchem_id;

        /**
         *@param  -  hmdb_id-    Human Metabolome Database id
         */
        string _hmdb_id;


    public:
        enum class Type {
            MS1,
            MRM,
            MS2,
            UNKNOWN
        };

        enum class IonizationMode{
            Positive,
            Negative,
            Neutral
        };

        /**
        *@brief  -   constructor for this compound
        */
        Compound(string id, string name, string formula, int charge,
                 float expectedRt = -1,  float mass = 0, string db="", float precursorMz = 0,
                   float productMz = 0 ,float collisionEnergy =0, string note="");


        IonizationMode ionizationMode;

        /**
         * @brief Getters and Setters for 
         * private data members.
         */
        void setId(string id);

        string id() const;

        void setName(string name);

        string name() const;

        /**
         * Original name of this compound from the DB. This does not change once
         * assigned during construction.
         */
        string originalName() const;

        void setFormula(string formula);

        /**
         * @brief Filters an arbitrary string and gets rid of any characters
         * that are not allowed to be part of a small molecule formula.
         * @details It should be noted that this is only a filter and not a
         * parser of any sort. This function can be used to strip a possible
         * formula string to a form which is equivalent to how El-MAVEN
         * currently understands a checmical formula. The parsing scheme in
         * `MassCalculator::getComposition` can be used as a guide.
         * @param formulaString Any arbitrary string.
         * @return A string stripped of all non-chemical characters.
         */
        static string filterFormula(string formulaString);

        string formula() const;

        void setAlias(string alias);

        string alias() const;

        void setExpectedRt(float expectedRt);

        float expectedRt() const;

        void setCharge(int charge);

        int charge() const;

        void setMz(float mass);

        float mz() const;

        void setPrecursorMz(float precursorMz);

        float precursorMz() const;

        void setProductMz(float productMz);

        float productMz() const;

        void setCollisionEnergy(float  collisionEnergy);

        float collisionEnergy() const;

        void setDb(string db);

        string db() const;

        void setSrmId(string srmId);

        string srmId() const;

        void setNeutralMass(float mass);

        float neutralMass() const;

        void setCategory(vector<string> category);

        vector<string> category() const;

        void setFragmentMzValues(vector<float> mzValues);

        vector<float> fragmentMzValues() const;

        void setFragmentIntensities(vector<float> intensities);

        vector<float> fragmentIntensities() const;

        void setFragmentIonTypes(map<int, string> types);

        map<int, string> fragmentIonTypes() const;

        void setSmileString(string smileString);

        string smileString() const;

        void setLogP(float logP);

        float logP() const;

        void setVirtualFragmentation(bool isVirtual);

        bool virtualFragmentation() const;

        void setNote(string note);

        string note() const;

        void setIsDecoy(bool isDecoy);

        bool isDecoy() const;

        void setMethod_id(string id);

        string method_id() const;

        void setTransition_id(int id);

        int transition_id() const;

        void setKegg_id(string id);

        string kegg_id() const;

        void setPubchem_id(string id);

        string pubchem_id() const;

        void setHmdb_id(string id);

        string hmdb_id() const;

        bool operator == (const Compound& rhs) const;

        Compound* operator = (const Compound& rhs);

        /**
         * @brief Get the type of this compound, whether it can be used for
         * targeted analysis of MS (level 1), SRM, MRM or MS2 datasets.
         * @return Type of the compound as a `Compound::Type` enum.
         */
        Type type() const;

        FragmentationMatchScore scoreCompoundHit(Fragment* expFrag,
                                                 float productPpmTolr = 20,
                                                 bool searchProton = false);

        /**
         *@brief total mass by formula minus loss of electrons' mass
         */
        float adjustedMass(int charge);

        /**
         *@brief   -  utility function to compare compound by mass
         */
        static bool compMass(const Compound* a, const Compound* b )      { return(a->_mz < b->_mz);       }

        /**
         *@brief  -   utility function to compare compound by name for sorting purpose
         */
        static bool compName(const Compound* a, const Compound* b )    { return(a->_name < b->_name);       }

        /**
         *@brief  -  utility function to compare compound by formula for sorting purpose
         */
        static bool compFormula(const Compound* a, const Compound* b ) { return(a->_formula < b->_formula); }
};

ostream& operator<<(ostream& os, const Compound& compound);

#endif
