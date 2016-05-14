#ifndef DATABASE_H
#define DATABASE_H

#include "stable.h"
#include "mzSample.h"
#include "mzUtils.h"

/**
 * \class Molecule2D
 *
 * \ingroup mzroll
 *
 * \brief Class for Molecule2D.
 *
 * This class is used for Molecule2D.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class Molecule2D {
public:
    Molecule2D() {}
    QString id;
    QVector<QPointF> coord;
    QVector<QString> atoms;
};

/**
 * \class Database
 *
 * \ingroup mzroll
 *
 * \brief Class for Database.
 *
 * This class is used for Database.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class Database {

public:
    Database() {
        _connected = false;
    };
    Database(string filename) {
        connect(filename);
        loadAll();
    }
    ~Database() {
        closeAll();
    }

    QSqlDatabase& getLigandDB() {
        return ligandDB;
    }
    void loadAll();  //loads all tables
    void closeAll();
    bool connect(string filename);
    bool isConnected() {
        return _connected;
    }

    void loadKnowns();
    void loadAdducts(string filename);
    void loadFragments(string filename);
    void loadSpecies(string modelName);
    int  loadCompoundCSVFile(string filename);
    void loadCategories();
    void loadPathways();

    multimap<string, Compound*> keywordSearch(string needle);
    vector<string>   getCompoundReactions(string compound_id);

    void addCompound(Compound*c);
    void loadReactions(string modelName);

    vector<Compound*> getCopoundsSubset(string database);
    vector<Pathway*>  getPathways();
    vector<Compound*> getKnowns();

    vector<string> getPathwayReactions(string pathway_id);

    map<string, int>   getDatabaseNames();
    map<string, int>   getChromotographyMethods();

    Molecule2D* getMolecularCoordinates(QString id);
    Compound* findSpeciesById(string id);
    Compound* findSpeciesByPrecursor(float precursorMz, float productMz, int polarity );
    set<Compound*> findSpeciesByMass(float mz, float ppm);
    vector<Compound*> findSpeciesByName(string name, string dbname);

    void loadRetentionTimes(QString method);
    void saveRetentionTime(Compound* c, float rt, QString method);

    void saveValidation(Peak* p);

    vector<Adduct*> adductsDB;
    vector<Adduct*> fragmentsDB;

    deque<Compound*> compoundsDB;
    deque<Reaction*> reactionsDB;
    deque<Pathway*>  pathwayDB;
    deque<Molecule2D*> coordinatesDB;

    map<string, Compound*> compoundIdMap;
    map<string, Reaction*> reactionIdMap;
    map<string, Pathway*>  pathwayIdMap;
    map<string, Molecule2D*> coordinatesMap;

private:
    QSqlDatabase ligandDB;
    bool _connected;

};


#endif

