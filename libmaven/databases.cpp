#include "databases.h"

int Databases::loadCompoundCSVFile(string filename) {

    ifstream myfile(filename.c_str());
    if (! myfile.is_open()) return 0;

    string line;
    string dbname = mzUtils::cleanFilename(filename);
    int loadCount=0;
    int lineCount=0;
    map<string, int> header;
    vector<string> headers;

    //assume that files are tab delimited, unless matched ".csv", then comma delimited
    char sep='\t';
    if(filename.find(".csv") != -1 || filename.find(".CSV") != -1) sep=',';

    while ( getline(myfile,line) ) {
        if (!line.empty() && line[0] == '#') continue;
        //trim spaces on the left
        line.erase(line.find_last_not_of(" \n\r\t")+1);
        lineCount++;

        vector<string> fields;
        mzUtils::split(line, sep, fields);

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
                fields[i]=makeLowerCase(fields[i]);
                header[ fields[i] ] = i;
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
        
        if ( header.count("formula")&& header["formala"]<N) formula = fields[ header["formula"] ];
        if ( header.count("id")&& header["id"]<N) 	 id = fields[ header["id"] ];
        if ( header.count("name")&& header["name"]<N) 	 name = fields[ header["name"] ];
        if ( header.count("compound")&& header["compound"]<N) 	 name = fields[ header["compound"] ];

        if ( header.count("precursormz") && header["precursormz"]<N) precursormz=string2float(fields[ header["precursormz"]]);
        if ( header.count("productmz") && header["productmz"]<N)  productmz = string2float(fields[header["productmz"]]);
        if ( header.count("collisionenergy") && header["collisionenergy"]<N) collisionenergy=string2float(fields[ header["collisionenergy"]]);

        if ( header.count("Q1") && header["Q1"]<N) precursormz=string2float(fields[ header["Q1"]]);
        if ( header.count("Q3") && header["Q3"]<N)  productmz = string2float(fields[header["Q3"]]);
        if ( header.count("CE") && header["CE"]<N) collisionenergy=string2float(fields[ header["CE"]]);

        if ( header.count("category") && header["category"]<N) {
            string catstring = fields[header["category"]];
            if (!catstring.empty()) {
                mzUtils::split(catstring,';', categorylist);
                if(categorylist.size() == 0) categorylist.push_back(catstring);
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
            addCompound(compound);
            loadCount++;
        }
    }
    sort(compoundsDB.begin(),compoundsDB.end(), Compound::compMass);
    myfile.close();
    return loadCount;
}

void Databases::addCompound(Compound* c) {
    if(c == NULL) return;

    //new compound id .. insert into compound list
    if (!compoundIdMap.count(c->id)) {
        compoundIdMap[c->id] = c;
        compoundsDB.push_back(c);
    } else { //compound exists with the same name, match database
        bool matched=false;
        for(int i=0; i < compoundsDB.size();i++) {
            Compound* currentCompound = compoundsDB[i];
            if ( currentCompound->db == c->db && currentCompound->id==c->id) { 
                //compound from the same database
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
        if(!matched) compoundsDB.push_back(c);
    }
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
