#ifndef DATABASE_H
#define DATABASE_H

#include "Compound.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "stable.h"
#define HMASS 1.007825032
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
	bool connect(string filename);
	bool isConnected() { return _connected; }

	void loadKnowns();
	void loadAdducts(string filename);
	void loadFragments(string filename);
	void loadSpecies(string modelName);
	int loadCompoundCSVFile(string filename);
	void loadCategories();
	void loadPathways();

	multimap<string, Compound*> keywordSearch(string needle);
	vector<string> getCompoundReactions(string compound_id);

	bool addCompound(Compound* c);
	void loadReactions(string modelName);

	vector<Compound*> getCopoundsSubset(string database);
	vector<Pathway*> getPathways();
	vector<Compound*> getKnowns();

	vector<string> getPathwayReactions(string pathway_id);

	map<string, int> getDatabaseNames();
	map<string, int> getChromotographyMethods();

	Molecule2D* getMolecularCoordinates(QString id);
    //Added while merging with Maven776 - Kiran
	Compound* findSpeciesById(string id, string dbName);

	deque<Compound*> getCompoundsDB(){ 	return compoundsDB;}
	set<Compound*> findSpeciesByMass(float mz, float ppm);
	vector<Compound*> findSpeciesByName(string name, string dbname);

	void loadRetentionTimes(QString method);
	void saveRetentionTime(Compound* c, float rt, QString method);

	void saveValidation(Peak* p);

	vector<Adduct*> adductsDB;
	vector<Adduct*> fragmentsDB;

	deque<Compound*> compoundsDB;
	deque<Reaction*> reactionsDB;
	deque<Pathway*> pathwayDB;
	deque<Molecule2D*> coordinatesDB;

	map<string, Compound*> compoundIdMap;
	map<string, Reaction*> reactionIdMap;
	map<string, Pathway*> pathwayIdMap;
	map<string, Molecule2D*> coordinatesMap;
    vector<string> notFoundColumns;
    //Added while merging with Maven776 - Kiran
    const std::string ANYDATABASE;
       private:
	QSqlDatabase ligandDB;
	bool _connected;
};

#endif
