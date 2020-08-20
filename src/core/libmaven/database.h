#ifndef DATABASE_H
#define DATABASE_H

#include <boost/signals2.hpp>
#include "standardincludes.h"
#include "mzUtils.h"

using namespace std;
class Adduct;
class Compound;
class MassCutoff;
class Pathway;
class Peak;
class Reaction;

namespace bsignal = boost::signals2;

class Database
{
    public:
        Database(){};
        
        /**
         * @brief Remove an already loaded database (and all compounds part of it)
         * from the DB store.
         * @param dbName Name of the database to be removed.
         */
        void removeDatabase(string dbName);

        /**
         * @brief addCompound Reads CSV file and adds compound to
         * the compoundDB vector.
         * @param c Compound object.
         * @return
         */
        bool addCompound(Compound* c);

        /**
         * @brief getCompoundsSubset Gives the compounds from specified db
         * from the loaded compound database.
         * @param database
         * @return
         */
        vector<Compound*> getCompoundsSubset(string database);

        /**
         * @brief getKnowns Returns compounds from the Known database.
         * @return
         */
        vector<Compound*> getKnowns();

        /**
         * @brief Load metabolites from a file at a given path by treating it as
         * having NIST library format.
         * @param filepath The absolute path of the NIST library file.
         * @param signal Pointer to a boost signal object that can be called
         * with a string for update message, an integer for current steps of
         * progress and another integer for total steps to completion.
         * @return The number of compounds that were loaded into the database.
         */
        int loadNISTLibrary(
            string fileName,
            bsignal::signal<void(string, int, int)>* signal = nullptr);
        /**
         * @brief loadMascotLibrary Loads compounds from mgf file.
         * @param filepath  The absolute path of the Mascot library file.
         * @param signal Pointer to a boost signal object that can be called
         * with a string for update message, an integer for current steps of
         * progress and another integer for total steps to completion.
         * @return The number of compounds that were loaded into the database.
         */
        int loadMascotLibrary(
            string filepath,
            bsignal::signal<void(string, int, int)>* signal = nullptr);

        /**
         * @brief loadAdducts Loads Adducts from the file specified.
         * @param filename The path of the file.
         */
        void loadAdducts(string filename);
        
        /**
         * @brief loadCompoundCSVFile   Loads the compounds from CSV file.
         * @param filename  The path to csv file.
         * @return Number of compounds loaded.
         */
        int loadCompoundCSVFile(string filename);

        /**
         * @brief Checks whether the library with the given name is an NIST
         * library or not.
         * @details The first compound in the database (if it has any compounds)
         * is checked for PRM information and if found, the database is
         * regarded as an NIST library.
         * @param dbName String name of the database to be checked.
         * @return True if the database with given name is an NIST library,
         * false otherwise.
         */
        bool isSpectralLibrary(string dbName);

        /**
         * @brief getDatabaseNames Returns the names of the databases present, along
         * with the number of compounds loaded.
         * @return
         */
        map<string, int> getDatabaseNames();

        /**
         * @brief findSpeciesByIdAndName Finds species based on id, name and
         * dbname
         * @param id Id of compound.
         * @param name Name of compound.
         * @param dbName Db to which compound belong.
         * @return
         */
        Compound* findSpeciesByIdAndName(string id, string name, string dbName);

        /**
         * @brief findSpeciesByMass Finds compound on the basis of mass
         * @param mz Mass by charge ratio of the compound.
         * @param massCutoff
         * @return
         */
        set<Compound*> findSpeciesByMass(float mz, MassCutoff* massCutoff);

        /**
         * @brief findSpeciesByName Finds compound on basis of name and dbname.
         * @param name Name of compound.
         * @param dbname DB to which compound belongs.
         * @return
         */
        vector<Compound*> findSpeciesByName(string name, string dbname);

        /**
         * @brief findSpeciesById Finds compound on basis of id and dbname.
         * @param id Id of compound.
         * @param dbName DB to which compound belongs.
         * @return
         */
        vector<Compound*> findSpeciesById(string id, string dbName);

        /**
         * @brief findAdductByName Finds adduct by name in its db.
         * @param name Name of adduct.
         * @return
         */
        Adduct* findAdductByName(string name);

        Compound* extractCompoundfromEachLine(vector<string>& fields,
                                              map<string, int>& header,
                                              int loadCount,
                                              string filename);
        float getChargeFromDB(vector<string>& fields, map<string, int>& header);
        vector<string> getCategoryFromDB(vector<string>& fields,
                                         map<string, int>& header);
        
        deque<Compound*> getCompoundsDB(){ 	return compoundsDB;}

        deque<Compound*> compoundsDB;

        vector<Adduct*> adductsDB()
        {
            return _adductsDB;
        }

        vector<string> invalidRows()
        {
            return _invalidRows;
        }

        void updateChargesForZeroCharges(int charge);

        vector<string> notFoundColumns(){
            return _notFoundColumns;
        }
        // Added while merging with Maven776 - Kiran
        const std::string ANYDATABASE;

    private:

        vector<Adduct*> _adductsDB;
        vector<Adduct*> _fragmentsDB;
        map<string, Compound*> _compoundIdNameDbMap;
        vector<Compound*> _compoundsWithZeroCharge;
        vector<string> _invalidRows;
        vector<string> _notFoundColumns;
        map<string, int> _compoundIdenticalCount;
        bool _startsWith(string line, string text);
        bool _contain(string line, string text);
        map<string, Compound*> _compoundIdMap;
};

extern Database DB;

#endif
