#ifndef COMPOUND_H
#define COMPOUND_H

#include "standardincludes.h"
#include "PeakGroup.h"

class Reaction;
class FragmentationMatchScore;
class Fragment;

using namespace std;

class Compound{
    /*
    *@brief  -   class to represent a compound
    *@detail  -  class Compound representing a compound. It will hold variable to
    *discribe a compound and also PeakGroup object it will belong to in addition
    *to other required functions for other operation
    */

    private:
        /** 
        *@param - One group represent one compund and
        * _group will present a link to that compund
         */
        PeakGroup _group;
        /**
        *@param - _groupUnlinked will check  wether Compound and PeakGroup are linked or not
        */
        bool      _groupUnlinked;

    public:
        enum class Type {
            MS1,
            MRM,
            PRM,
            UNKNOWN
        };

        /**
        *@brief  -   constructor for this compound
        */
        Compound(string id, string name, string formula, int charge );
        ~Compound(){};

        bool operator == (const Compound& rhs) const;

        PeakGroup* getPeakGroup() { return &_group; }
        /**@brief   -   set this compound to a group and vice-versa   */
        void setPeakGroup(const PeakGroup& group ) { _group = group; _group.setCompound(this); }
        /**@brief    -  check wether this compound belong to a group or not   */
        bool hasGroup()  const   { if(_group.meanMz != 0 ) return true; return false; }

        void clearGroup()  { _group.clear(); }      /**@brief  -   delete group children, peaks etc   */

        void unlinkGroup() { _group.clear(); _groupUnlinked = true; }   /**@brief  -   clear its group and mark it unlink to any group  */

        bool groupUnlinked() const { return _groupUnlinked; }

        /**
        *@param  -   Reaction represents simple compound reaction
        *Check Reaction class in mzSample.h  
        */
        vector<Reaction*>reactions;
        string id;          /**@param - compund id */

        string name;        /**@param -  name of compound */
        string formula;         /**@param -  formula of compound */
        string kegg_id;         /**@param -  kegg_id-    Kyoto Encyclopedia of Genes and Genomes id*/
        string pubchem_id;      /**@param  -  pubchem_id -    PubChem id*/
        string hmdb_id;         /**@param  -  hmdb_id-    Human Metabolome Database id */
        string alias;       /**@param   -  alias name of compound   */

        /**
         * @brief A simple string in form of a line notation for describing the
         * structure of chemical species using short ASCII strings.
         */
        string smileString;

        // TODO: from MAVEN (upstream), find out what this is
        string adductString;

        /**
        *@param -  srmId will hold filterLine string from mzxml file which represent type of
        *mass spec and ionization is used and other info as well
        */
        string srmId;
        float expectedRt;

        int charge;     /**@param  -   number of charge of compound (electron loss or gain)   */
        float mass;     /**@param   -  mass of this compund   */

        /** QQQ mapping */
        string method_id;   /**@param  -  TODO*/
        float precursorMz;	/**@param  -  QQQ parent ion  mz value   */
        float productMz;    /**@param  -  QQQ child ion   mz value */
        float collisionEnergy; /**@param  -   QQQ collision energy of this compound   */
        float logP; // TODO: find out what value this is.

        // TODO: from MAVEN (upstream), find out what this is
        bool virtualFragmentation;

        // TODO: from MAVEN (upstream) decoy compound?
        bool isDecoy;

        // TODO: from MAVEN (upstream). Can this be derived somehow.
        // Also maybe use an enum.
        int ionizationMode;

        /**
         * @brief Name of database this compound belongs to for example KEGG,
         * ECOCYC, etc.
         */
        string db;

        /**
         * TODO
         */
        int transition_id;

        /**
         * @brief Vector of m/z values of fragments generated from this compund.
         */
        vector<float>fragmentMzValues;

        /**
         * @brief Vector of intensities of fragments generated from this
         * compund.
         */
        vector<float>fragmentIntensities;

        /**
         * @brief Collection of indices of fragment values mapping to its
         * ionisation type information.
         */
        map<int, string>fragmentIonTypes;

        /**
         * @brief Categories of this compound. For e.g., amino acids, nucleic
         * acids, peptide, etc.
         */
        vector<string> category;

        /**
         * @brief Get the type of this compound, whether it can be used for
         * targeted analysis of MS (level 1), SRM, MRM or PRM datasets.
         * @return Type of the compound as a `Compound::Type` enum.
         */
        Type type() const;

        /**
         * @brief A note containing any miscellaneous details for this compound.
         */
        string note;

        FragmentationMatchScore scoreCompoundHit(Fragment* expFrag,
                                                 float productPpmTolr = 20,
                                                 bool searchProton = false);

        float adjustedMass(int charge);  /**   total mass by formula minus loss of electrons' mass  */
        void addReaction(Reaction* r) { reactions.push_back(r); }   /**  add reaction of this compound   */
        /**
        *@brief   -  utility function to compare compound by mass
        */
        static bool compMass(const Compound* a, const Compound* b )      { return(a->mass < b->mass);       }
        /**
        *@brief  -   utility function to compare compound by name for sorting purpose
        */
        static bool compName(const Compound* a, const Compound* b )    { return(a->name < b->name);       }
        /**
        *@brief  -  utility function to compare compound by formula for sorting purpose
        */
        static bool compFormula(const Compound* a, const Compound* b ) { return(a->formula < b->formula); }
        /**
        *@brief   -  utility function to compare compound by number of reactions
        *it is involved in
        */
        static bool compReactionCount(const Compound* a, const Compound* b ) { return(a->reactions.size() < b->reactions.size()); }
};
#endif
