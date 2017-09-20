#include "databases.h"

int Databases::loadCompoundCSVFile(string filename) {

    ifstream myfile(filename.c_str());
    if (! myfile.is_open()) return 0;

    string line;
    int loadCount = 0, lineCount = 0;
    map<string, int> header;
    vector<string> headers;

    //assume that files are tab delimited, unless matched ".csv", then comma delimited
    string sep="\t";
    if(filename.find(".csv") != -1 || filename.find(".CSV") != -1) sep=",";

    while (getline(myfile,line)) {
        //This is used to write commands
        if (!line.empty() && line[0] == '#') continue;
        
        //trim spaces on the left
        line.erase(line.find_last_not_of(" \n\r\t")+1);
        lineCount++;

        vector<string> fields;
        mzUtils::splitNew(line, sep, fields);

        mzUtils::removeSpecialcharFromStartEnd(fields);

        //Getting the heading from the csv File
        if (lineCount == 1) {
            headers = fields;
            for(unsigned int i = 0; i < fields.size(); i++ ) {
                fields[i] = makeLowerCase(fields[i]);
                header[ fields[i] ] = i;
            }
            continue;
        }

        Compound* compound = extractCompoundfromEachLine(fields, header, loadCount, filename);

        if (compound) {
            if (addCompound(compound)) {
                loadCount++;
            }
        }
    }
    sort(compoundsDB.begin(),compoundsDB.end(), Compound::compMass);
    myfile.close();
    return loadCount;
}

Compound* Databases::extractCompoundfromEachLine(vector<string>& fields, map<string, int> & header, int loadCount, string filename) {
    string id, name, formula, polarityString;
    float rt = 0, mz = 0, charge = 0, collisionenergy = 0, precursormz = 0, productmz = 0;
    int NumOfFields = fields.size();
    vector<string> categorylist;

    string dbname = mzUtils::cleanFilename(filename);

    if (header.count("mz") && header["mz"] < NumOfFields)  
        mz = string2float(fields[header["mz"]]);

    //Expected RT is given importance over RT. So if Expected RT is
    //present in the DB that will be taken to the RT field in compounds
    if (header.count("rt") && header["rt"] < NumOfFields)  
        rt = string2float(fields[header["rt"]]);

    if (header.count("expectedrt") && header["expectedrt"] < NumOfFields) 
        rt = string2float(fields[header["expectedrt"]]);
    
    //TODO: Not really a todo, just marking that I fixed a typo here
    //make sure we merge it in
    if (header.count("formula") && header["formula"] < NumOfFields)
        formula = fields[header["formula"]];
    
    if (header.count("id") && header["id"] < NumOfFields)
        id = fields[header["id"]];
    
    // Compound Field is given importance than the names field
    // compound is a better field to keep
    if (header.count("name") && header["name"] < NumOfFields)
        name = fields[header["name"]];

    if (header.count("compound") && header["compound"] < NumOfFields)
        name = fields[header["compound"]];

    if (header.count("precursormz") && header["precursormz"] < NumOfFields)
        precursormz = string2float(fields[ header["precursormz"]]);

    if (header.count("productmz") && header["productmz"] < NumOfFields)  
        productmz = string2float(fields[header["productmz"]]);

    if (header.count("collisionenergy") && header["collisionenergy"] < NumOfFields)
        collisionenergy = string2float(fields[ header["collisionenergy"]]);

    if (header.count("Q1") && header["Q1"] < NumOfFields) 
        precursormz = string2float(fields[ header["Q1"]]);

    if (header.count("Q3") && header["Q3"] < NumOfFields)  
        productmz = string2float(fields[header["Q3"]]);

    if (header.count("CE") && header["CE"] < NumOfFields) 
        collisionenergy=string2float(fields[header["CE"]]);

    categorylist = getCategoryFromDB(fields, header);

    charge = getChargeFromDB(fields, header);
    //If Some of the imp fields are not present here is th way it will be
    //assigned
    if (mz == 0) 
        mz = precursormz;

    if (id.empty() && !name.empty()) 
        id = name;

    if (id.empty() && name.empty()) 
        id = "cmpd:" + integer2string(loadCount);

    //The compound should atleast have formula so that
    //mass can be calculated from the formula
    if ( mz > 0 || !formula.empty() ) {
        Compound* compound = new Compound(id,name,formula,charge);

        compound->expectedRt = rt;

        if (mz == 0)
            mz = MassCalculator::computeMass(formula, charge);
        
        
        compound->mass = mz;
        compound->db = dbname;
        compound->expectedRt = rt;
        compound->precursorMz = precursormz;
        compound->productMz = productmz;
        compound->collisionEnergy = collisionenergy;

        for(unsigned int i=0; i < categorylist.size(); i++) 
            compound->category.push_back(categorylist[i]);
        
        return compound;
    }

    return NULL;
    
}

float Databases::getChargeFromDB(vector<string>& fields, map<string, int> & header) {
    float charge = 0;
    int NumOfFields = fields.size();
    if (header.count("charge") && header["charge"] < NumOfFields)
        charge = string2float(fields[header["charge"]]);

    if ( header.count("polarity") && header["polarity"] < NumOfFields) {
        string polarityString = fields[header["polarity"]];
        if ( polarityString == "+" ) {
            charge = 1;
        } else if ( polarityString == "-" ) {
            charge = -1;
        } else  {
            charge = string2float(polarityString);
        }
    }
    return charge;
}

vector<string> Databases::getCategoryFromDB(vector<string>& fields, map<string, int> & header) {
    vector<string> categorylist;
    int NumOfFields = fields.size();
    //What is category?
    if ( header.count("category") && header["category"] < NumOfFields) {
        string catstring = fields[header["category"]];
        if (!catstring.empty()) {
            mzUtils::split(catstring,';', categorylist);
            if(categorylist.size() == 0) categorylist.push_back(catstring);
        }
    }
    return categorylist;
}


bool Databases::addCompound(Compound* c) {
    if(c == NULL) return false;
    bool compoundAdded = false;

    //new compound id .. insert into compound list
    if (!compoundIdMap.count(c->id)) {
        compoundIdMap[c->id] = c;
        compoundsDB.push_back(c);
        compoundAdded = true;
    } else { 
        //compound exists with the same name, match database
        bool matched = false;
        for(unsigned int i = 0; i < compoundsDB.size(); i++) {
            Compound* currentCompound = compoundsDB[i];
            if ( currentCompound->db == c->db && currentCompound->id == c->id) { 
                //compound from the same database
                currentCompound->id = c->id;
                currentCompound->name = c->name;
                currentCompound->formula = c->formula;
                currentCompound->srmId = c->srmId;
                currentCompound->expectedRt = c->expectedRt;
                currentCompound->charge = c->charge;
                currentCompound->mass = c->mass;
                currentCompound->precursorMz = c->precursorMz;
                currentCompound->productMz = c->productMz;
                currentCompound->collisionEnergy = c->collisionEnergy;
                currentCompound->category = c->category;
                matched = true;
            }
        }

        if(!matched) {
            compoundsDB.push_back(c);
            compoundAdded = true;
        }
    }
    return compoundAdded;
}

vector<Compound*> Databases::getCopoundsSubset(string dbname) {
	vector<Compound*> subset;
	for (unsigned int i=0; i < compoundsDB.size(); i++ ) {
	    if (compoundsDB[i]->db == dbname) {
		    subset.push_back(compoundsDB[i]);
		}
	}
	return subset;
}

void Databases::closeAll() {
    //mzUtils::delete_all(adductsDB);
    mzUtils::delete_all(compoundsDB);
    //mzUtils::delete_all(fragmentsDB);
    //mzUtils::delete_all(reactionsDB);
}
