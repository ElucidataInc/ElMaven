#ifndef DATABASES_H
#define DATABASES_H

#include "Compound.h"
#include "mzSample.h"
#include "mzUtils.h"

class Databases {

    public:
        void addCompound(Compound* c);
        int loadCompoundCSVFile(string filename);
        vector<Compound*> getCopoundsSubset(string dbname);
        Compound* extractCompoundfromEachLine(vector<string>& fields, map<string, int> & header, int loadCount, string filename);
        float getChargeFromDB(vector<string>& fields, map<string, int> & header);
        vector<string> getCategoryFromDB(vector<string>& fields, map<string, int> & header);
        void closeAll();

    private:
        map<string,Compound*> compoundIdMap;
        deque<Compound*> compoundsDB;

        //vector<Adduct*> adductsDB;
        //vector<Adduct*> fragmentsDB;
        //deque<Reaction*> reactionsDB;
        
};

#endif //DATABASES_H
