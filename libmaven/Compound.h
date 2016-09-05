#ifndef COMPOUND_H
#define COMPOUND_H

#include <vector>
#include "PeakGroup.h"

class Reaction;
class PeakGroup;
using namespace std;
class Compound{
    //static MassCalculator* mcalc;

    private:
        PeakGroup _group;			//link to peak group
        bool      _groupUnlinked;

    public:
        Compound(string id, string name, string formula, int charge );
        ~Compound(){}; //empty destructor

        PeakGroup* getPeakGroup() { return &_group; }
        void setPeakGroup(const PeakGroup& group ) { _group = group; _group.compound = this; }
        bool hasGroup()  const   { if(_group.meanMz != 0 ) return true; return false; }
        void clearGroup()  { _group.clear(); }
        void unlinkGroup() { _group.clear(); _groupUnlinked = true; }
        bool groupUnlinked() const { return _groupUnlinked; }

        vector<Reaction*>reactions;
        string id;

        string name;
        string formula;
        string kegg_id;
        string pubchem_id;
        string hmdb_id;
        string alias;

        string srmId;
        float expectedRt;

        int charge;
        float mass;

        //QQQ mapping
        string method_id;
        float precursorMz;	//QQQ parent ion
        float productMz;    // QQQ child ion
        float collisionEnergy; //QQQ collision energy

        string db;			//name of database for example KEGG, ECOCYC.. etc..

        int transition_id;
        vector<float>fragment_mzs;
        vector<float>fragment_intensity;
        vector<string> category;

        float ajustedMass(int charge);
        void addReaction(Reaction* r) { reactions.push_back(r); }
        static bool compMass(const Compound* a, const Compound* b )      { return(a->mass < b->mass);       }
        static bool compName(const Compound* a, const Compound* b )    { return(a->name < b->name);       }
        static bool compFormula(const Compound* a, const Compound* b ) { return(a->formula < b->formula); }
        static bool compReactionCount(const Compound* a, const Compound* b ) { return(a->reactions.size() < b->reactions.size()); }
};
#endif
