#ifndef DATABASES_H
#define DATABASES_H

#include "mzSample.h"
#include "mzUtils.h"

class Databases {

    public:
        void addCompound(Compound* c);
        int loadCompoundCSVFile(string filename);
        vector<Compound*> getCopoundsSubset(string dbname);
        void closeAll();
    private:
        map<string,Compound*> compoundIdMap;
        deque<Compound*> compoundsDB;
        //vector<Adduct*> adductsDB;
        //vector<Adduct*> fragmentsDB;
        //deque<Reaction*> reactionsDB;
        
};

#endif //DATABASES_H