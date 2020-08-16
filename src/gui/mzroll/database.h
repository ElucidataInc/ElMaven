#ifndef DATABASE_H
#define DATABASE_H

#include <boost/signals2.hpp>

#include "stable.h"
#include "standardincludes.h"

class Adduct;
class Compound;
class MassCutoff;
class Peak;

namespace bsignal = boost::signals2;

class Molecule2D {
       public:
	Molecule2D() {}
	QString id;
	QVector<QPointF> coord;
	QVector<QString> atoms;
};

class Database {
       public:
	Database() { _connected = false; };
	Database(string filename) {
		connect(filename);
		loadAll();
	}
	~Database() { closeAll(); }

	QSqlDatabase& getLigandDB() { return ligandDB; }
	void loadAll();  // loads all tables
	void closeAll();

    /**
     * @brief Remove an already loaded database (and all compounds part of it)
     * from the DB store.
     * @param dbName Name of the database to be removed.
     */
    void removeDatabase(string dbName);

    bool connect(string filename);
	bool isConnected() { return _connected; }

	void loadKnowns();
	void loadAdducts(string filename);
	void loadFragments(string filename);
	int loadCompoundCSVFile(string filename);

	multimap<string, Compound*> keywordSearch(string needle);

	bool addCompound(Compound* c);

    /**
     * @brief Set the charge values for all compounds that have a charge of 0.
     * @param charge The integral charge value to be set.
     */
    void updateChargesForZeroCharges(int charge);

    vector<Compound*> getCompoundsSubset(string database);
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
        int loadNISTLibrary(QString filepath,
                            bsignal::signal<void (string, int, int)>* signal=nullptr);

        int loadMascotLibrary(QString filepath,
                              bsignal::signal<void (string, int, int)>* signal=nullptr);

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

        map<string, int> getDatabaseNames();
	map<string, int> getChromotographyMethods();

	Molecule2D* getMolecularCoordinates(QString id);
    //Added while merging with Maven776 - Kiran
        Compound* findSpeciesByIdAndName(string id, string name, string dbName);

	deque<Compound*> getCompoundsDB(){ 	return compoundsDB;}
	set<Compound*> findSpeciesByMass(float mz, MassCutoff *massCutoff);
	vector<Compound*> findSpeciesByName(string name, string dbname);
	vector<Compound*> findSpeciesById(string id, string dbName);
    Adduct* findAdductByName(string name);

	void loadRetentionTimes(QString method);
	void saveRetentionTime(Compound* c, float rt, QString method);

	void saveValidation(Peak* p);

	vector<Adduct*> adductsDB;
	vector<Adduct*> fragmentsDB;

    vector<Compound*> _compoundsWithZeroCharge;
    deque<Compound*> compoundsDB;
	deque<Molecule2D*> coordinatesDB;

        map<string, Compound*> compoundIdNameDbMap;
	map<string, Molecule2D*> coordinatesMap;
    vector<string> notFoundColumns;
    vector<string> invalidRows;
    map<string, int> compoundIdenticalCount;
    //Added while merging with Maven776 - Kiran
    const std::string ANYDATABASE;
       private:
	QSqlDatabase ligandDB;
	bool _connected;
};

extern Database DB;

#endif
