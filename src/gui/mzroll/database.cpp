#include <QFile>

#include "datastructures/adduct.h"
#include "Compound.h"
#include "constants.h"
#include "database.h"
#include "masscutofftype.h"
#include "mgf/mgf.h"
#include "mzMassCalculator.h"
#include "mzSample.h"
#include "mzUtils.h"

bool Database::connect(string filename) {
    QString dbname(filename.c_str());
    ligandDB = QSqlDatabase::addDatabase("QSQLITE", "ligandDB");
    ligandDB.setDatabaseName(dbname);
    return ligandDB.open();
}

void Database::loadAll() {

	//compounds subsets
	const std::string EmptyString;
    loadSpecies(EmptyString);
    loadReactions(EmptyString);
	loadPathways();
	loadCategories();

    cerr << "compoundsDB="
         << compoundsDB.size()
         << " "
         << compoundIdNameDbMap.size()
         << endl;
    cerr << "reactionsDB=" << reactionsDB.size() << endl;
    cerr << "pathwaysDB=" <<  pathwayDB.size() << endl;
    cerr << "adductsDB=" << adductsDB.size() << endl;
    cerr << "fragmentsDB=" << fragmentsDB.size() << endl;
    cerr << endl;
}


void Database::closeAll() {
    mzUtils::delete_all(adductsDB);
    mzUtils::delete_all(compoundsDB);
    mzUtils::delete_all(fragmentsDB);
    mzUtils::delete_all(reactionsDB);
}

void Database::removeDatabase(string dbName)
{
    auto iter = begin(compoundsDB);
    while (iter < end(compoundsDB)) {
        auto compound = *iter;
        if (compound->db() == dbName) {
            compoundIdenticalCount.erase(compound->id() + compound->name() + dbName);
            compoundIdNameDbMap.erase(compound->id() + compound->name() + dbName);
            iter = compoundsDB.erase(iter);
            delete compound;
        } else {
            ++iter;
        }
    }
}

multimap<string,Compound*> Database::keywordSearch(string needle) {
    QSqlQuery query(ligandDB);
    query.prepare("SELECT compound_id, keyword from sets where keyword like '%?%'");
	query.addBindValue(needle.c_str());
 	if (!query.exec())   qDebug() << query.lastError();


    multimap<string,Compound*>matches;
    while (query.next()) {
        std::string id  = query.value(0).toString().toStdString();
        std::string keyword  = query.value(1).toString().toStdString();
        //Updated while merging with Maven776 - Kiran
        Compound* cmpd = findSpeciesByIdAndName(id, "", ANYDATABASE);
        if (cmpd != NULL ) matches.insert(pair<string,Compound*>(keyword,cmpd));
    }
	return matches;
}

void Database::loadCategories() {
    QSqlQuery query(ligandDB);
    query.prepare("SELECT * from sets");
	if(!query.exec()) qDebug() << query.lastError();

    while (query.next()) {
        std::string keyword  = query.value(0).toString().toStdString();
        std::string id  = query.value(1).toString().toStdString();
        //Updated while merging with Maven776 - Kiran
        Compound* cmpd = findSpeciesByIdAndName(id, "",ANYDATABASE);
        if (cmpd != NULL ) {
         //   cmpd->category.push_back(keyword);
        }
    }

    query.prepare("SELECT P.pathway_id, R.species_id from pathways P, reactions R where P.reaction_id = R.reaction_id");
	if(!query.exec()) qDebug() << query.lastError();

    while (query.next()) {
        std::string keyword  = query.value(0).toString().toStdString();
        std::string id  = query.value(1).toString().toStdString();
        //Updated while merging with Maven776 - Kiran
        Compound* cmpd = findSpeciesByIdAndName(id, "", ANYDATABASE);
        if (cmpd != NULL ) {
         //   cmpd->category.push_back(keyword);
        }
    }

	return;
}

bool Database::addCompound(Compound* newCompound)
{
    if(newCompound == nullptr)
        return false;

    // existing compound, change its name according to the number of
    // compounds with the same ID
    if (compoundIdenticalCount.count(newCompound->id()
                                     + newCompound->name()
                                     + newCompound->db())) {
        int loadOrder = compoundIdenticalCount.at(newCompound->id()
                                                  + newCompound->name()
                                                  + newCompound->db());

        // return false if any of the compounds having the same ID are the
        // exact same in all aspects.
        auto originalName = newCompound->name();
        for (int i = 0; i < loadOrder; ++i) {
            string nameWithSuffix = originalName;
            if (i != 0)
                nameWithSuffix = originalName + " (" + to_string(i) + ")";

            newCompound->setName (nameWithSuffix);
            Compound* possibleCopy = compoundIdNameDbMap[newCompound->id()
                                                         + nameWithSuffix
                                                         + newCompound->db()];
            if (possibleCopy != nullptr && *newCompound == *possibleCopy)
                return false;

            newCompound->setName (originalName);
        }

        newCompound->setName (originalName + " (" + to_string(loadOrder) + ")");
        compoundIdenticalCount[newCompound->id()
                               + originalName
                               + newCompound->db()] = ++loadOrder;
    } else {
        compoundIdenticalCount[newCompound->id()
                               + newCompound->name()
                               + newCompound->db()] = 1;
    }

    compoundIdNameDbMap[newCompound->id()
                        + newCompound->name()
                        + newCompound->db()] = newCompound;
    compoundsDB.push_back(newCompound);
    return true;
}

void Database::loadSpecies(string db) {
		map<string,Compound*> species;
		map<string, int> species_degree;
		QString sql = "SELECT * from species";
		if (db.length()) sql += " where database=?";

        QSqlQuery query(ligandDB);
        query.prepare(sql);
        if(db.length()) query.addBindValue(db.c_str());
		if(!query.exec()) qDebug() << query.lastError();
        while (query.next()) {
			string id =   query.value(0).toString().toStdString();
			string name = query.value(1).toString().toStdString();
			string formula = query.value(2).toString().toStdString();
			int charge = query.value(3).toInt();
			Compound* compound = new Compound(id,name,formula,charge);
                        compound->setDb (query.value(4).toString().toStdString());
			addCompound(compound);
		}
}

set<Compound*> Database::findSpeciesByMass(float mz, MassCutoff *massCutoff) {
	set<Compound*>uniqset;

    Compound x("find", "", "",0);
    x.setMz(mz-massCutoff->massCutoffValue(mz));
    deque<Compound*>::iterator itr = lower_bound(
            compoundsDB.begin(),compoundsDB.end(),
            &x, Compound::compMass );

    for(;itr != compoundsDB.end(); itr++ ) {
        Compound* c = *itr;
        if (c->mz() > mz+1) break;

        if ( mzUtils::massCutoffDist(c->mz(),mz,massCutoff) < massCutoff->getMassCutoff() ) {
			if (uniqset.count(c)) continue;
            uniqset.insert(c);
        }
    }
    return uniqset;
}

Compound* Database::findSpeciesByIdAndName(string id,
                                           string name,
                                           string dbName)
{
    if (compoundIdNameDbMap.count(id + name + dbName))
        return compoundIdNameDbMap[id + name + dbName];
    return NULL;
}

Molecule2D* Database::getMolecularCoordinates(QString id) {
    	if ( coordinatesMap.count(id.toStdString()) ) return coordinatesMap[id.toStdString()];

		QString sql = "select * from atoms";
		if (id.length()) sql += " where species_id=?";

		QSqlQuery query(ligandDB);
		query.prepare(sql);
		if(id.length()) query.addBindValue(id);
		if(!query.exec()) qDebug() << query.lastError();

		Molecule2D* mol = new Molecule2D();
		while (query.next()) {
				mol->id = query.value(0).toString();
				mol->atoms.push_back(query.value(2).toString());
				mol->coord.push_back(QPointF( query.value(3).toDouble(), query.value(4).toDouble()));
		}

		coordinatesDB.push_back(mol);
		coordinatesMap[id.toStdString()] = mol;
		return mol;
}

vector<Compound*> Database::findSpeciesByName(string name, string dbname) {
		vector<Compound*> set;
		for(unsigned int i=0; i < compoundsDB.size(); i++ ) {
                                if (compoundsDB[i]->name() == name && compoundsDB[i]->db() == dbname) {
					set.push_back(compoundsDB[i]);
				}
		}
		return set;
}

vector<Compound*> Database::findSpeciesById(string id, string dbName) {
    vector<Compound*> matches;
    for (auto compound : compoundsDB) {
        if (compound->id() == id && compound->db() == dbName) {
            matches.push_back(compound);
        }
    }

    return matches;
}

Adduct* Database::findAdductByName(string name)
{
    if(name == "[M+H]+") {
        return MassCalculator::PlusHAdduct;
    } else if(name == "[M-H]-") {
        return MassCalculator::MinusHAdduct;
    } else if(name == "[M]") {
        return MassCalculator::ZeroMassAdduct;
    }

    for(auto adduct : adductsDB) {
        if (adduct->getName() == name)
            return adduct;
    }
    return nullptr;
}

void Database::loadReactions(string db) {

		map<string, Reaction*> seenReactions;
		map<string, bool> seenSpeciesReactions;

		QString sql = "select * from reactions";
		if (db.length()) sql += " where database=?";

        QSqlQuery query(ligandDB);
        query.prepare(sql);
        if(db.length()) query.addBindValue(db.c_str());
		if(!query.exec()) qDebug() << query.lastError();

        while (query.next()) {
			string id = query.value(0).toString().toStdString();
			string species_id = query.value(1).toString().toStdString();
			string species_type = query.value(2).toString().toStdString();
			int stoch = query.value(3).toInt();
			bool reversable = query.value(4).toInt() > 0;
			string db = query.value(5).toString().toStdString();
			Reaction* r = NULL;
			if ( seenReactions.count(id) == 0 ) {
					r = new Reaction(db,id,id);
					r->setReversable(reversable);
					reactionsDB.push_back(r);
					reactionIdMap[r->id]=r;
					seenReactions[id] = r;
			} else {
					r = seenReactions[id];
			}

			//cerr << id << " " << species << " " << species_type << endl;
            //Updated while merging with Maven776 - Kiran
                        Compound* c = findSpeciesByIdAndName(species_id,
                                                             "",
                                                             ANYDATABASE);

                        vector<Reaction*> reactions;
                        if ( c != NULL && r != NULL && seenSpeciesReactions.count(c->id() + r->id) == 0 ) {
                                seenSpeciesReactions[ c->id() + r->id ]=true;
			}

			if ( c != NULL && r != NULL && species_type == "R" )  r->addReactant(c,stoch);
			if ( c != NULL && r != NULL && species_type == "P" )  r->addProduct(c,stoch);
		}
}

void Database::loadPathways() {
    QSqlQuery query(ligandDB);
    query.prepare("select pathway_id, pathway_name, reaction_id from pathways");
	if(!query.exec()) qDebug() << query.lastError();
    while (query.next()) {
		string id = query.value(0).toString().toStdString();
		string name = query.value(1).toString().toStdString();
		string rid= query.value(2).toString().toStdString();

		if (id.empty() || name.empty() || rid.empty() ) continue;

		Pathway* pathway=NULL;
		if (pathwayIdMap.count(id) > 0) {
			pathway = pathwayIdMap[id];
		}else {
			pathway = new Pathway(id,name);
			pathwayIdMap[id]=pathway;
			pathwayDB.push_back(pathway);
		}

		if (pathway && reactionIdMap.count(rid) > 0 ) {
			Reaction* r = reactionIdMap[rid];
			pathway->reactions.push_back(r);
		}
	}
}

void Database::saveRetentionTime(Compound* c, float rt, QString method) {
	if (!c) return;

	cerr << "setExpectedRetentionTime() " << rt << endl;
	//QSqlDatabase db = QSqlDatabase::database("ligand.db");
	 ligandDB.transaction();
     QSqlQuery query(ligandDB);

	query.prepare("insert into knowns_times values (?,?,?)");
        query.addBindValue(QString(c->id().c_str()));
	query.addBindValue(method);
	query.addBindValue(QString::number(rt,'f',4) );

	if(!query.exec()) qDebug() << query.lastError();

	ligandDB.commit();
	query.clear();
}


void Database::loadRetentionTimes(QString method) {
    QSqlQuery query(ligandDB);
    query.prepare("select compound_id, rt from knowns_times where method = ?");
    query.addBindValue(method);
	if(!query.exec()) qDebug() << query.lastError();


    while (query.next()) {
		std::string cid = query.value(0).toString().toStdString();
		float rt = query.value(1).toDouble();
        //Updated while merging with Maven776 - Kiran
                Compound* c = findSpeciesByIdAndName(cid, "", ANYDATABASE);
                if (c) c->setExpectedRt (rt);
	}
}

void Database::saveValidation(Peak* p) {
        QSqlQuery query(ligandDB);
	query.prepare("insert into validations (rt,mz,class) values(?,?,?)");
	query.addBindValue(QString::number(p->rt,'f',4) );
	query.addBindValue(QString::number(p->peakMz,'f',4) );
	query.addBindValue(QString(p->label));
	if(!query.exec()) qDebug() << query.lastError();
	query.clear();
}

vector<string> Database::getPathwayReactions(string pathway_id) {

    QSqlQuery query(ligandDB);
    query.prepare("select reaction_id from pathways where pathway_id = ?");
    //query.addBindValue(QString(pathway_id.c_str()));
    query.addBindValue(pathway_id.c_str());
	if(!query.exec()) qDebug() << query.lastError();

	vector<string> reactions;
    while (query.next()) {
		string reaction_id = query.value(0).toString().toStdString();
		if (! reaction_id.empty() ) {
			reactions.push_back(reaction_id);
		}
	}
	return reactions;
}


vector<string> Database::getCompoundReactions(string compound_id) {
    QSqlQuery query(ligandDB);
    query.prepare("select reaction_id from reactions where compound_id = ?");
    query.addBindValue(compound_id.c_str());
	 if (!query.exec())   qDebug() << query.lastError();

	vector<string> compounds;
    while (query.next()) {
		string id = query.value(0).toString().toStdString();
		if (! id.empty() ) {
			compounds.push_back(id);
		}
	}
	return compounds;
}

vector<Compound*> Database::getCompoundsSubset(string dbname) {
	vector<Compound*> subset;
	for (unsigned int i=0; i < compoundsDB.size(); i++ ) {
                        if (compoundsDB[i]->db() == dbname) {
					subset.push_back(compoundsDB[i]);
			}
	}
	return subset;
}

vector<Compound*> Database::getKnowns() {
    return getCompoundsSubset("KNOWNS");
}

map<string,int> Database::getDatabaseNames() {
	map<string,int>dbnames;
        for (unsigned int i=0; i < compoundsDB.size(); i++ ) dbnames[ compoundsDB[i]->db() ]++;
	return dbnames;
}

int Database::loadNISTLibrary(QString filepath,
                              bsignal::signal<void (string, int, int)>* signal)
{
    QString filename = QFileInfo(filepath).fileName();
    if (signal)
        (*signal)("Preprocessing database " + filename.toStdString(), 0, 0);

    qDebug() << "Counting number of lines in NIST Libary file…" << filepath;
    ifstream file(filepath.toStdString());
    file.unsetf(ios_base::skipws); // do not skip newlines
    unsigned lineCount = std::count(istream_iterator<char>(file),
                                    istream_iterator<char>(),
                                    '\n');

    qDebug() << "Loading NIST Libary: " << filepath;
    QFile data(filepath);
    if (!data.open(QFile::ReadOnly) ) {
        qDebug() << "Can't open " << filepath;
        return 0;
    }

    QRegExp whiteSpace("\\s+");
    QRegExp formulaMatch("Formula\\=(C\\d+H\\d+\\S*)");
    QRegExp retentionTimeMatch("AvgRt\\=(\\S+)");

    string dbName = mzUtils::cleanFilename(filepath.toStdString());
    Compound* currentCompound = nullptr;
    bool capturePeaks = false;
    int compoundCount = 0;
    int currentLine = 0;

    QTextStream stream(&data);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        if (line.startsWith("NAME:", Qt::CaseInsensitive) || stream.atEnd()) {
            // before reading the next record or ending stream, save the
            // compound created from last record
            if (currentCompound and !currentCompound->name().empty()) {
                if (!currentCompound->formula().empty()) {
                    auto formula = currentCompound->formula();
                    auto exactMass = MassCalculator::computeMass(formula, 0);
                    currentCompound->setMz(exactMass);
                }
                if (addCompound(currentCompound))
                    ++compoundCount;
            }

            // we need to check this again before creating a new compound,
            // otherwise it would create one at stream end as well
            if (line.startsWith("NAME:", Qt::CaseInsensitive)) {
                // new compound
                QString name = line.mid(5, line.length()).simplified();
                currentCompound = new Compound(name.toStdString(),
                                               name.toStdString(),
                                               "",
                                               0);
                currentCompound->setDb (dbName);
                capturePeaks = false;
            }
        }

        if(currentCompound == nullptr)
            continue;

        if (line.startsWith("MW:", Qt::CaseInsensitive)) {
            currentCompound->setMz(line.mid(3, line.length())
                                        .simplified()
                                        .toDouble());
        } else if (line.startsWith("CE:", Qt::CaseInsensitive)) {
            currentCompound->setCollisionEnergy (line.mid(3, line.length())
                                                   .simplified()
                                                   .toDouble());
        } else if (line.startsWith("ID:", Qt::CaseInsensitive)) {
            QString id = line.mid(3, line.length()).simplified();
            if (!id.isEmpty())
                currentCompound->setId(id.toStdString());
        } else if (line.startsWith("LOGP:", Qt::CaseInsensitive)) {
            currentCompound->setLogP (line.mid(5, line.length())
                                         .simplified()
                                         .toDouble());
        } else if (line.startsWith("RT:", Qt::CaseInsensitive)) {
            currentCompound->setExpectedRt (line.mid(3, line.length())
                                              .simplified()
                                              .toDouble());
        } else if (line.startsWith("SMILE:", Qt::CaseInsensitive)) {
            QString smileString = line.mid(7, line.length()).simplified();
            if (!smileString.isEmpty())
                currentCompound->setSmileString (smileString.toStdString());
        } else if (line.startsWith("SMILES:", Qt::CaseInsensitive)) {
            QString smileString = line.mid(8, line.length()).simplified();
            if (!smileString.isEmpty())
                currentCompound->setSmileString (smileString.toStdString());
        } else if (line.startsWith("PRECURSORMZ:", Qt::CaseInsensitive)) {
            currentCompound->setPrecursorMz ( line.mid(13, line.length())
                                               .simplified()
                                               .toDouble());
        } else if (line.startsWith("EXACTMASS:", Qt::CaseInsensitive)) {
            currentCompound->setMz( line.mid(10, line.length())
                                        .simplified()
                                        .toDouble());
        } else if (line.startsWith("FORMULA:", Qt::CaseInsensitive)) {
            QString formula = line.mid(9, line.length()).simplified();
            formula.replace("\"", "", Qt::CaseInsensitive);
            if (!formula.isEmpty())
                currentCompound->setFormula (formula.toStdString());
        } else if (line.startsWith("MOLECULE FORMULA:", Qt::CaseInsensitive)) {
            QString formula = line.mid(17, line.length()).simplified();
            formula.replace("\"", "", Qt::CaseInsensitive);
            if (!formula.isEmpty())
                currentCompound->setFormula(formula.toStdString());
        } else if (line.startsWith("CATEGORY:", Qt::CaseInsensitive)) {
            auto category = currentCompound->category();
            category.push_back(line.mid(10, line.length())
                                                    .simplified()
                                                    .toStdString());
            currentCompound->setCategory(category);
        } else if (line.startsWith("TAG:", Qt::CaseInsensitive)) {
            if (line.contains("VIRTUAL", Qt::CaseInsensitive))
                currentCompound->setVirtualFragmentation(true);
        } else if (line.startsWith("ION MODE:", Qt::CaseInsensitive)
                   || line.startsWith("IONMODE:", Qt::CaseInsensitive)
                   || line.startsWith("IONIZATION:", Qt::CaseInsensitive)) {
            if (line.contains("NEG", Qt::CaseInsensitive))
                currentCompound->ionizationMode = Compound::IonizationMode::Negative;
            if (line.contains("POS", Qt::CaseInsensitive))
                currentCompound->ionizationMode = Compound::IonizationMode::Positive;
        } else if (line.startsWith("COMMENT:", Qt::CaseInsensitive)) {
            QString comment = line.mid(8, line.length()).simplified();
            if (comment.contains(formulaMatch)) {
                currentCompound->setFormula (formulaMatch.capturedTexts()
                                                       .at(1)
                                                       .toStdString());
            }
            if (comment.contains(retentionTimeMatch)) {
                currentCompound->setExpectedRt (retentionTimeMatch.capturedTexts()
                                                                .at(1)
                                                                .simplified()
                                                                .toDouble());
            }
        } else if (line.startsWith("NUM PEAKS:", Qt::CaseInsensitive)
                   || line.startsWith("NUMPEAKS:", Qt::CaseInsensitive)) {
            capturePeaks = true;
        } else if (capturePeaks) {
            QStringList mzIntensityPair = line.split(whiteSpace);
            if (mzIntensityPair.size() >= 2) {
                double mz = mzIntensityPair.at(0).toDouble();
                double in = mzIntensityPair.at(1).toDouble();
                if (mz >= 0.0 && in >= 0.0) {
                    auto mzValues = currentCompound->fragmentMzValues();
                    mzValues.push_back(mz);
                    currentCompound->setFragmentMzValues(mzValues);
                    auto intensities = currentCompound->fragmentIntensities();
                    intensities.push_back(in);
                    currentCompound->setFragmentIntensities(intensities);

                    int fragIdx = currentCompound->fragmentMzValues().size() - 1;
                    if (mzIntensityPair.size() >= 3) {
                        auto ionTypes = currentCompound->fragmentIonTypes();
                        ionTypes[fragIdx] =
                            mzIntensityPair.at(2).toStdString();
                        currentCompound->setFragmentIonTypes(ionTypes);
                    }
                }
            }
        }
        ++currentLine;
        if (signal) {
            (*signal)("Loading spectral library: " + filename.toStdString(),
                      currentLine,
                      lineCount);
        }
    }

    return compoundCount;
}

int Database::loadMascotLibrary(QString filepath,
                                bsignal::signal<void (string, int, int)> *signal)
{
    mgf::MgfFile mgfFile;
    mgf::Driver driver(mgfFile);
    driver.trace_parsing = false;
    driver.trace_scanning = false;

    ifstream ifs(filepath.toStdString());
    bool result = driver.parse_stream(ifs);

    QString filename = QFileInfo(filepath).fileName();
    if (signal)
        (*signal)("Reading file " + filename.toStdString(), 0, 0);

    if (!result) {
        std::cerr << "Error parsing data stream"
                  << std::endl;
        return 0;
    }

    for (auto specIter = begin(mgfFile); specIter != end(mgfFile); ++specIter) {
        auto charges = specIter->getCHARGE();
        int charge = 1;
        if (!charges.empty())
            charge = charges.front();

        Compound* compound = new Compound(specIter->getTITLE(),
                                          specIter->getTITLE(),
                                          "",
                                          charge);
        compound->setExpectedRt ( specIter->getRTINSECONDS().first / 60.0f);
        compound->setMz(specIter->getPEPMASS().first);
        compound->setPrecursorMz(compound->mz());
        compound->setSmileString (specIter->getSMILES());
        compound->ionizationMode = specIter->getIONMODE() == "negative" ? Compound::IonizationMode::Negative
                                                                        : Compound::IonizationMode::Positive;

        // create spectra
        vector<float> fragmentMzValues;
        vector<float> fragmentInValues;
        for (auto fragPair = specIter->begin();
             fragPair != specIter->end();
             ++fragPair) {
            fragmentMzValues.push_back(fragPair->first);
            fragmentInValues.push_back(fragPair->second);
        }
        compound->setFragmentMzValues (fragmentMzValues);
        compound->setFragmentIntensities (fragmentInValues);

        compound->setDb( mzUtils::cleanFilename(filepath.toStdString()));
        addCompound(compound);

        if (signal) {
            (*signal)("Loading spectral library: " + filename.toStdString(),
                      (specIter - begin(mgfFile)),
                      mgfFile.size());
        }
    }
    if (signal)
        (*signal)("Finished loading " + filename.toStdString(), 0, 0);
    return mgfFile.size();
}

bool Database::isSpectralLibrary(string dbName) {
    auto compounds = getCompoundsSubset(dbName);
    if (compounds.size() > 0) {
        return compounds.at(0)->type() == Compound::Type::MS2;
    }
    return false;
}

//TODO Shubhra: column order should not affect loading
//not high priority since this is not a user generated file yet
void Database::loadAdducts(string filename)
{
    QFile myFile(QString(filename.c_str()));
    if (!myFile.open(QFile::ReadOnly)) return;

    int loadCount = 0;
    int lineCount = 0;
    while (!myFile.atEnd()) {
        QString tempLine = myFile.readLine().trimmed();
        if (tempLine.isEmpty()) continue;

        string line = tempLine.toStdString();
        if (!line.empty() && line[0] == '#')
            continue;
        lineCount++;

        if (lineCount == 1) 
            continue;

        vector<string> fields;
        mzUtils::split(line, ',', fields);

        if(fields.size() < 2 )
            continue;

        string name = fields[0];
        int nmol = string2float(fields[1]);
        int charge = string2float(fields[2]);
        float mass = string2float(fields[3]);

        if (name.empty() || nmol < 0)
            continue;
        Adduct* a = new Adduct(name, nmol, charge, mass);
        adductsDB.push_back(a);
        loadCount++;
    }
    cerr << "LOADCOUNT: " << loadCount;
    myFile.close();
}

void Database::loadFragments(string filename) {
    ifstream myfile(filename.c_str());
    if (! myfile.is_open()) return;

    string line;
    while ( getline(myfile,line) ) {
		if (!line.empty() && line[0] == '#') continue;
      	vector<string>fields;
        mzUtils::split(line,',', fields);

		if(fields.size() < 3 ) continue;
		string name=fields[0];
		float mass=string2float(fields[1]);
		float charge=string2float(fields[2]);
		if ( mass < 0 || name.empty() ) continue;
		Adduct* a = new Adduct(name, 1, charge, mass);
		//TODO Shubhra: Figure out the purpose of isParent
        //a->isParent = false;
		fragmentsDB.push_back(a);
	}
	myfile.close();
}

int Database::loadCompoundCSVFile(string filename)
{
    QFile myFile (QString(filename.c_str()));
    if(!myFile.open(QFile::ReadOnly))
        return 0;

    string line;
    string dbname = mzUtils::cleanFilename(filename);
    int loadCount=0;
    int lineCount=0;
    map<string, int>header;
    static const string allHeadersarr[] = {"mz", "mass", "rt", "expectedrt", "charge", "formula", "id", "name",
        "compound", "precursormz", "productmz", "collisionenergy", "Q1", "Q3", "CE", "category", "polarity", "note"};
    vector<string> allHeaders (allHeadersarr, allHeadersarr + sizeof(allHeadersarr) / sizeof(allHeadersarr[0]) );

    //assume that files are tab delimited, unless matched ".csv", then comma delimited
    string sep="\t";
    if(filename.find(".csv") != -1 || filename.find(".CSV") != -1) sep=",";
    notFoundColumns.resize(0);
    invalidRows.clear();
    //cerr << filename << " sep=" << sep << endl;
    while(!myFile.atEnd()) {
        //remove whitespace from the start and end
        QString tempLine = myFile.readLine().trimmed();
        if (tempLine.isEmpty()) continue;

        string line = tempLine.toStdString();
        if (!line.empty() && line[0] == '#') continue;
        lineCount++;

        vector<string>fields;
        mzUtils::splitNew(line, sep, fields);

        for(unsigned int i=0; i < fields.size(); i++ ) {
            int n = fields[i].length();
            if (n>2 && fields[i][0] == '"' && fields[i][n-1] == '"') {
                fields[i]= fields[i].substr(1,n-2);
            }
            if (n>2 && fields[i][0] == '\'' && fields[i][n-1] == '\'') {
                fields[i]= fields[i].substr(1,n-2);
            }
        }

        if (lineCount==1) {
            for(unsigned int i=0; i < fields.size(); i++ ) {
                fields[i] = makeLowerCase(fields[i]);
                if (find(allHeaders.begin(), allHeaders.end(), fields[i]) != allHeaders.end()) {
                    header[fields[i]] = i;
                } else {
                    notFoundColumns.push_back(fields[i]);
                }
            }
            continue;
        }

        string id, name, formula;
        string note;
        float rt=0;
        float mz=0;
        float mass = 0.0f;
        float charge=0;
        float collisionenergy=0;
        float precursormz=0;
        float productmz=0;
        int N=fields.size();
        vector<string>categorylist;


        if ( header.count("mz") && header["mz"]<N)  mz = string2float(fields[ header["mz"]]);
        if ( header.count("mass") && header["mass"]<N)
            mass = string2float(fields[ header["mass"]]);
        if ( header.count("rt") && header["rt"]<N)  rt = string2float(fields[ header["rt"]]);
        if ( header.count("expectedrt") && header["expectedrt"]<N) rt = string2float(fields[ header["expectedrt"]]);
        if ( header.count("charge")&& header["charge"]<N) charge = string2float(fields[ header["charge"]]);
        if ( header.count("formula")&& header["formula"]<N) formula = fields[header["formula"]];
        if ( header.count("id")&& header["id"]<N) 	 id = fields[ header["id"] ];
        if ( header.count("name")&& header["name"]<N) 	 name = fields[ header["name"] ];
        if ( header.count("compound")&& header["compound"]<N) 	 name = fields[ header["compound"] ];

        if ( header.count("precursormz") && header["precursormz"]<N) precursormz=string2float(fields[ header["precursormz"]]);
        if ( header.count("productmz") && header["productmz"]<N)  productmz = string2float(fields[header["productmz"]]);
        if ( header.count("collisionenergy") && header["collisionenergy"]<N) collisionenergy=string2float(fields[ header["collisionenergy"]]);

        if ( header.count("Q1") && header["Q1"]<N) precursormz=string2float(fields[ header["Q1"]]);
        if ( header.count("Q3") && header["Q3"]<N)  productmz = string2float(fields[header["Q3"]]);
        if ( header.count("CE") && header["CE"]<N) collisionenergy=string2float(fields[ header["CE"]]);

        if (header.count("note") && header["note"] < N)
            note = fields[header["note"]];

        if ( header.count("category") && header["category"]<N) {
            string catstring = fields[header["category"]];
            if (!catstring.empty()) {
                mzUtils::split(catstring,';', categorylist);
                if(categorylist.size() == 0) categorylist.push_back(catstring);
                //cerr << catstring << " ListSize=" << categorylist.size() << endl;
            }
         }

        if ( header.count("polarity") && header["polarity"] <N)  {
            string x = fields[ header["polarity"]];
            if ( x == "+" ) {
                charge = 1;
            } else if ( x == "-" ) {
                charge = -1;
            } else  {
                charge = string2float(x);
            }

        }


        if (id.empty()&& !name.empty()) id=name;
        if (id.empty() && name.empty()) id="cmpd:" + integer2string(loadCount);

        if ( mz > 0 || ! formula.empty() || precursormz > 0 || mass > 0.0f) {
            Compound* compound = new Compound(id,name,formula,charge);

            compound->setExpectedRt(rt);

            if(mass != 0)
                compound->setNeutralMass(mass);

            if (mz == 0 && !formula.empty())
                mz = MassCalculator::computeMass(formula,charge);
            else if (mass != 0.0f) {
                mz = MassCalculator::adjustMass(mass, charge);
            }

            compound->setMz(mz);
            compound->setDb(dbname);
            compound->setExpectedRt(rt);
            compound->setPrecursorMz (precursormz);
            compound->setProductMz(productmz);
            compound->setCollisionEnergy(collisionenergy);
            compound->setNote(note);
            auto category = compound->category();
            for(int i=0; i < categorylist.size(); i++)
                category.push_back(categorylist[i]);
            compound->setCategory(category);
            if (addCompound(compound))
                loadCount++;
        } else {
            if (!name.empty())
                id = name;
            invalidRows.push_back(id);
        }
    }
    sort(compoundsDB.begin(),compoundsDB.end(), Compound::compMass);
    //cerr << "Loading " << dbname << " " << loadCount << endl;
    myFile.close();
    return loadCount;
}
