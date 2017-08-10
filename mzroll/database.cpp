#include "database.h"


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

    cerr << "compoundsDB=" << compoundsDB.size() << " " << compoundIdMap.size() << endl;
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
        Compound* cmpd = findSpeciesById(id,ANYDATABASE);
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
        Compound* cmpd = findSpeciesById(id,ANYDATABASE);
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
        Compound* cmpd = findSpeciesById(id,ANYDATABASE);
        if (cmpd != NULL ) {
         //   cmpd->category.push_back(keyword);
        }
    }

	return;
}

bool Database::addCompound(Compound* c) {
    if(c == NULL) return false;
    bool compoundAdded = false;

    //new compound id .. insert into compound list
    if (!compoundIdMap.count(c->id)) {
        compoundIdMap[c->id] = c;
        compoundsDB.push_back(c);
        compoundAdded = true;
    } else { //compound exists with the same name, match database
        bool matched=false;
        for(int i=0; i < compoundsDB.size();i++) {
            Compound* currentCompound = compoundsDB[i];
            if ( currentCompound->db == c->db && currentCompound->id==c->id) { //compound from the same database
                currentCompound->id=c->id;
                currentCompound->name=c->name;
                currentCompound->formula = c->formula;
                currentCompound->srmId = c->srmId;
                currentCompound->expectedRt = c->expectedRt;
                currentCompound->charge = c->charge;
                currentCompound->mass = c->mass;
                currentCompound->precursorMz = c->precursorMz;
                currentCompound->productMz = c->productMz;
                currentCompound->collisionEnergy = c->collisionEnergy;
                currentCompound->category = c->category;
                matched=true;
            }
        }
        if(!matched) {
            compoundsDB.push_back(c);
            compoundAdded = true;
        }
    }
    return compoundAdded;
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
			compound->db =  query.value(4).toString().toStdString();
			addCompound(compound);
		}
}

set<Compound*> Database::findSpeciesByMass(float mz, float ppm) {
	set<Compound*>uniqset;

    Compound x("find", "", "",0);
    x.mass = mz-(mz/1e6*ppm);;
    deque<Compound*>::iterator itr = lower_bound(
            compoundsDB.begin(),compoundsDB.end(),
            &x, Compound::compMass );

    for(;itr != compoundsDB.end(); itr++ ) {
        Compound* c = *itr;
        if (c->mass > mz+1) break;

        if ( mzUtils::ppmDist(c->mass,mz) < ppm ) {
			if (uniqset.count(c)) continue;
            uniqset.insert(c);
        }
    }
    return uniqset;
}

        //Updated while merging with Maven776 - Kiran
Compound* Database::findSpeciesById(string id, string dbName) {
    if ( compoundIdMap.count(id) ) return compoundIdMap[id];
    return NULL;

    Compound* c = NULL;
    for(int i=0; i < compoundsDB.size(); i++ ) {
        //Updated while merging with Maven776 - Kiran
        if (!dbName.empty() and compoundsDB[i]->db != dbName) continue;
        if (compoundsDB[i]->id == id ) { c = compoundsDB[i]; break; }
    }
    return c;
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
				if (compoundsDB[i]->name == name && compoundsDB[i]->db == dbname) {
					set.push_back(compoundsDB[i]);
				}
		}
		return set;
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
            Compound* c = findSpeciesById(species_id,ANYDATABASE);

			if ( c != NULL && r != NULL && seenSpeciesReactions.count(c->id + r->id) == 0 ) {
				seenSpeciesReactions[ c->id + r->id ]=true;
				c->reactions.push_back(r);
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
	query.addBindValue(QString(c->id.c_str()));
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
        Compound* c = findSpeciesById(cid,ANYDATABASE);
		if (c) c->expectedRt = rt;
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

vector<Compound*> Database::getCopoundsSubset(string dbname) {
	vector<Compound*> subset;
	for (unsigned int i=0; i < compoundsDB.size(); i++ ) {
			if (compoundsDB[i]->db == dbname) {
					subset.push_back(compoundsDB[i]);
			}
	}
	return subset;
}

vector<Compound*> Database::getKnowns() {
	return getCopoundsSubset("KNOWNS");
}

map<string,int> Database::getDatabaseNames() {
	map<string,int>dbnames;
	for (unsigned int i=0; i < compoundsDB.size(); i++ ) dbnames[ compoundsDB[i]->db ]++;
	return dbnames;
}


void Database::loadAdducts(string filename) {
    ifstream myfile(filename.c_str());
    if (! myfile.is_open()) return;

    string line;
    while ( getline(myfile,line) ) {
		if (!line.empty() && line[0] == '#') continue;
      	vector<string>fields;
        mzUtils::split(line,',', fields);

		if(fields.size() < 2 ) continue;
		string name=fields[0];
		int nmol=string2float(fields[1]);
		int charge=string2float(fields[2]);
		float mass=string2float(fields[3]);

		if ( name.empty() || nmol < 0 ) continue;
		Adduct* a = new Adduct();
		a->name = name;
		a->mass = mass;
		a->nmol = nmol;
		a->charge = charge;
		a->isParent = false;
		if (abs(abs(a->mass)-HMASS)< 0.01) a->isParent=true;
		adductsDB.push_back(a);
	}
	myfile.close();
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
		Adduct* a = new Adduct();
		a->name = name;
		a->mass = mass;
		a->nmol = 1;
		a->charge = charge;
		a->isParent = false;
		fragmentsDB.push_back(a);
	}
	myfile.close();
}



int Database::loadCompoundCSVFile(string filename){

    ifstream myfile(filename.c_str());
    if (! myfile.is_open()) return 0;

    string line;
    string dbname = mzUtils::cleanFilename(filename);
    int loadCount=0;
    int lineCount=0;
    map<string, int>header;
    vector<string> headers;
    static const string allHeadersarr[] = {"mz", "rt", "expectedrt", "charge", "formula", "id", "name", 
    "compound", "precursormz", "productmz", "collisionenergy", "Q1", "Q3", "CE", "category", "polarity"};
    vector<string> allHeaders (allHeadersarr, allHeadersarr + sizeof(allHeadersarr) / sizeof(allHeadersarr[0]) );

    //assume that files are tab delimited, unless matched ".csv", then comma delimited
    string sep="\t";
    if(filename.find(".csv") != -1 || filename.find(".CSV") != -1) sep=",";
    notFoundColumns.resize(0);
    //cerr << filename << " sep=" << sep << endl;
    while ( getline(myfile,line) ) {
        if (!line.empty() && line[0] == '#') continue;
        //trim spaces on the left
        line.erase(line.find_last_not_of(" \n\r\t")+1);
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
            headers = fields;
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
        float rt=0;
        float mz=0;
        float charge=0;
        float collisionenergy=0;
        float precursormz=0;
        float productmz=0;
        int N=fields.size();
        vector<string>categorylist;


        if ( header.count("mz") && header["mz"]<N)  mz = string2float(fields[ header["mz"]]);
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

        //cerr << lineCount << " " << endl;
        //for(int i=0; i<headers.size(); i++) cerr << headers[i] << ", ";
        //cerr << "   -> category=" << header.count("category") << endl;
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

        //cerr << "Loading: " << id << " " << formula << "mz=" << mz << " rt=" << rt << " charge=" << charge << endl;

        if (mz == 0) mz=precursormz;
        if (id.empty()&& !name.empty()) id=name;
        if (id.empty() && name.empty()) id="cmpd:" + integer2string(loadCount);

        if ( mz > 0 || ! formula.empty() ) {
            Compound* compound = new Compound(id,name,formula,charge);

            compound->expectedRt = rt;

            if (mz == 0) mz = MassCalculator::computeMass(formula,charge);
            compound->mass = mz;



            compound->db = dbname;
            compound->expectedRt=rt;
            compound->precursorMz=precursormz;
            compound->productMz=productmz;
            compound->collisionEnergy=collisionenergy;
            for(int i=0; i < categorylist.size(); i++) compound->category.push_back(categorylist[i]);
            if (addCompound(compound))
                loadCount++;
        }
    }
    sort(compoundsDB.begin(),compoundsDB.end(), Compound::compMass);
    //cerr << "Loading " << dbname << " " << loadCount << endl;
    myfile.close();
    return loadCount;
}
