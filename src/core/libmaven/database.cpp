#include "doctest.h"
#include "Compound.h"
#include "constants.h"
#include "database.h"
#include "masscutofftype.h"
#include "mgf/mgf.h"
#include "mzMassCalculator.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "boost/algorithm/string.hpp"
#include "datastructures/adduct.h"
using namespace mzUtils;

void Database::removeDatabase(string dbName)
{
    auto iter = begin(_compoundsDB);
    while (iter < end(_compoundsDB)) {
        auto compound = *iter;
        if (compound->db() == dbName) {
            _compoundIdenticalCount.erase(compound->id() + compound->name() + dbName);
            _compoundIdNameDbMap.erase(compound->id() + compound->name() + dbName);
            iter = _compoundsDB.erase(iter);
            delete compound;
        } else {
            ++iter;
        }
    }
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

    for(auto adduct : adductsDB()) {
        if (adduct->getName() == name)
            return adduct;
    }
    return nullptr;
}

void Database::updateChargesForZeroCharges(int charge)
{
    for (auto compound : _compoundsWithZeroCharge)
        compound->setCharge(charge);
}


bool Database::addCompound(Compound* newCompound)
{
    if(newCompound == nullptr)
        return false;

    // existing compound, change its name according to the number of
    // compounds with the same ID
    if (_compoundIdenticalCount.count(newCompound->id()
                                     + newCompound->originalName()
                                     + newCompound->db())) {
        int loadOrder = _compoundIdenticalCount.at(newCompound->id()
                                                  + newCompound->originalName()
                                                  + newCompound->db());

        // return false if any of the compounds having the same ID are the
        // exact same in all aspects.
        auto originalName = newCompound->originalName();
        for (int i = 0; i < loadOrder; ++i) {
            string nameWithSuffix = originalName;
            if (i != 0)
                nameWithSuffix = originalName + " (" + to_string(i) + ")";

            newCompound->setName(nameWithSuffix);
            Compound* possibleCopy = _compoundIdNameDbMap[newCompound->id()
                                                         + nameWithSuffix
                                                         + newCompound->db()];
            if (possibleCopy != nullptr && *newCompound == *possibleCopy)
                return false;

            newCompound->setName(originalName);
        }

        newCompound->setName(originalName + " (" + to_string(loadOrder) + ")");
        _compoundIdenticalCount[newCompound->id()
                               + originalName
                               + newCompound->db()] = ++loadOrder;
    } else {
        _compoundIdenticalCount[newCompound->id()
                               + newCompound->name()
                               + newCompound->db()] = 1;
    }

    _compoundIdNameDbMap[newCompound->id()
                        + newCompound->name()
                        + newCompound->db()] = newCompound;
    _compoundsDB.push_back(newCompound);
    if (newCompound->charge() == 0)
        _compoundsWithZeroCharge.push_back(newCompound);
    return true;
}

Compound* Database::findSpeciesByIdAndName(string id,
                                           string name,
                                           string dbName)
{
    if (_compoundIdNameDbMap.count(id + name + dbName))
        return _compoundIdNameDbMap[id + name + dbName];
    return NULL;
}

vector<Compound*> Database::findSpeciesById(string id, string dbName) {
    vector<Compound*> matches;
    for (auto compound : _compoundsDB) {
        if (compound->id() == id && compound->db() == dbName) {
            matches.push_back(compound);
        }
    }

    return matches;
}

vector<Compound*> Database::findSpeciesByName(string name, string dbname)
{
    vector<Compound*> set;
    for (unsigned int i = 0; i < _compoundsDB.size(); i++) {
        if (_compoundsDB[i]->name() == name && _compoundsDB[i]->db() == dbname) {
            set.push_back(_compoundsDB[i]);
        }
    }
    return set;
}

vector<Compound*> Database::getCompoundsSubset(string dbname) {
	vector<Compound*> subset;
	for (unsigned int i=0; i < _compoundsDB.size(); i++ ) {
        if (_compoundsDB[i]->db() == dbname) {
		    subset.push_back(_compoundsDB[i]);
		}
	}
	return subset;
}

vector<Compound*> Database::getKnowns() {
    return getCompoundsSubset("KNOWNS");
}

map<string,int> Database::getDatabaseNames() {
	map<string,int>dbnames;
        for (unsigned int i=0; i < _compoundsDB.size(); i++ )
            dbnames[ _compoundsDB[i]->db() ]++;
	return dbnames;
}


bool Database::_startsWith(string line, string text)
{
    transform(line.begin(), line.end(), line.begin(), ::tolower);
    transform(text.begin(), text.end(), text.begin(), ::tolower);
    for( size_t i = 0; i < text.size(); i++){
        if(line[i] != text[i])
            return false;
    }
    return true;
}

bool Database::_contain(string line, string text)
{
    transform(line.begin(), line.end(), line.begin(), ::tolower);
    transform(text.begin(), text.end(), text.begin(), ::tolower);
    if(boost::algorithm::contains(line, text))
        return true;
    else
        return false;
}

bool Database::isSpectralLibrary(string dbName) {
    auto compounds = getCompoundsSubset(dbName);
    if (compounds.size() > 0) {
        return compounds.at(0)->type() == Compound::Type::MS2;
    }
    return false;
}

int Database::loadMascotLibrary(string filepath,
                                bsignal::signal<void (string, int, int)> *signal)
{
    mgf::MgfFile mgfFile;
    mgf::Driver driver(mgfFile);
    driver.trace_parsing = false;
    driver.trace_scanning = false;

    ifstream ifs(filepath);
    bool result = driver.parse_stream(ifs);

    
    if (signal)
        (*signal)("Reading file " + filepath, 0, 0);

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

        compound->setDb( mzUtils::cleanFilename(filepath));
        addCompound(compound);

        if (signal) {
            (*signal)("Loading spectral library: " + filepath,
                      (specIter - begin(mgfFile)),
                      mgfFile.size());
        }
    }
    if (signal)
        (*signal)("Finished loading " + filepath, 0, 0);
    return mgfFile.size();
}

void Database::loadAdducts(string filename)
{
    ifstream file(filename);
    if(!file.is_open()) 
        return;

    int loadCount = 0;
    int lineCount = 0;

    while (!file.eof()) {
        string line;
        getline(file,line);

        if (!line.size())
            continue;

        if (!line.empty() && line[0] == '#')
            continue;
        
        lineCount++;

        if (lineCount == 1) 
            continue;

        vector<string> fields;
        fields = mzUtils::split(line, ",");

        if(fields.size() < 2 ) 
            continue;

        string name = fields[0];
        int nmol = string2float(fields[1]);
        int charge = string2float(fields[2]);
        float mass = string2float(fields[3]);

        if (name.empty() || nmol < 0) 
            continue;
    
        Adduct* a = new Adduct(name, nmol, charge, mass);
        _adductsDB.push_back(a);
        loadCount++;
    }
    cerr << "LOADCOUNT: " << loadCount;
    file.close();
}

void Database::loadAdducts(string line, int lineCount)
{
    if (lineCount == 1)
        return;

    if (!line.empty() && line[0] == '#')
        return;
    
    vector<string> fields;
    fields = mzUtils::split(line, ",");

    if(fields.size() < 2 ) 
        return;

    string name = fields[0];
    int nmol = string2float(fields[1]);
    int charge = string2float(fields[2]);
    float mass = string2float(fields[3]);

    if (name.empty() || nmol < 0) 
        return;

    Adduct* a = new Adduct(name, nmol, charge, mass);
    _adductsDB.push_back(a);
    
}

int Database::loadNISTLibrary(string fileName,
                              bsignal::signal<void (string, int, int)>* signal)
{
    if (signal)
        (*signal)("Preprocessing database " + fileName, 0, 0);

    cerr << "Counting number of lines in NIST Libary file…" << fileName << endl;
    ifstream file(fileName);
    if(!file.is_open())
        return 0; 

    file.unsetf(ios_base::skipws); // do not skip newlines
    unsigned lineCount = std::count(istream_iterator<char>(file),
                                    istream_iterator<char>(),
                                    '\n');
    
    file.close();
    
    cerr << "Loading NIST Libary: " << fileName << endl;
    
    regex whiteSpace("\\s+");
    regex formulaMatch("Formula\\=(C\\d+H\\d+\\S*)");
    regex retentionTimeMatch("AvgRt\\=(\\S+)");
    regex keyValuePattern("\\\"([^=\\\"]*)=([^\\\"]*)\\\"");

    string dbName = mzUtils::cleanFilename(fileName);
    Compound* currentCompound = nullptr;
    bool capturePeaks = false;
    int compoundCount = 0;
    int currentLine = 0;

    file.open(fileName, ios::in);

    while (!file.eof()) {

        string line;
        getline(file,line);

        if (_startsWith(line, "NAME:")) {
            // before reading the next record or ending stream, save the
            // compound created from last record
            if (currentCompound and !currentCompound->name().empty()) {
                if (!currentCompound->formula().empty()) {
                    auto formula = currentCompound->formula();
                    auto exactMass = MassCalculator::computeMass(formula, 0);
                    currentCompound->setMz(exactMass);
                }
                if (addCompound(currentCompound)){
                    ++compoundCount;
                }
            }

            // we need to check this again before creating a new compound,
            // otherwise it would create one at stream end as well
            if (_startsWith(line, "NAME:")) {
                // new compound
                string name = line.substr(6, line.length());
                currentCompound = new Compound(name, name, "", 0);
                currentCompound->setDb (dbName);
                capturePeaks = false;
            }
        }

        if(currentCompound == nullptr)
            continue;

        if (_startsWith(line, "MW:")) {
            currentCompound->setMz(string2float(line.substr(3, line.length())));
        
        } else if (_startsWith(line, "CE:")
                   || _startsWith(line, "COLLISION ENERGY:")
                   || _startsWith(line, "COLLISION_ENERGY:")) {
            currentCompound->setCollisionEnergy(string2float(line.substr(3, line.length())));

        } else if (_startsWith(line, "ID:")) {
            string id = line.substr(4, line.length());
            if (id.size() > 0)
                currentCompound->setId(id);

        } else if (_startsWith(line, "LOGP:")) {
            currentCompound->setLogP(string2float(line.substr(5, line.length())));

        } else if (_startsWith(line, "RT:")) {
            currentCompound->setExpectedRt(string2float(line.substr(3, line.length())));

        } else if (_startsWith(line, "SMILE:")) {
            string smileString = line.substr(7, line.length());
            if (smileString.size() > 0)
                currentCompound->setSmileString (smileString);

        } else if (_startsWith(line, "SMILES:")) {
             string smileString = line.substr(8, line.length());
            if (smileString.size() > 0)
                currentCompound->setSmileString (smileString);

        } else if (_startsWith(line, "PRECURSORMZ:")) {
            currentCompound->setPrecursorMz(string2float(line.substr(13, line.length())));

        } else if (_startsWith(line, "EXACTMASS:")) {
            currentCompound->setMz(string2float(line.substr(10, line.length())));

        } else if (_startsWith(line, "FORMULA:")) {
            string formula = line.substr(9, line.length());
            boost::replace_all_copy(formula,"\"", "");
            if (formula.size() > 0)
                currentCompound->setFormula (formula);

        } else if (_startsWith(line, "MOLECULE FORMULA:")) {
           string formula = line.substr(17, line.length());
            boost::replace_all_copy(formula, "\"", "");
            if (formula.size() > 0)
                currentCompound->setFormula(formula);

        } else if (_startsWith(line, "CATEGORY:")) {
            auto category = currentCompound->category();
            category.push_back(line.substr(10, line.length()));
            currentCompound->setCategory(category);

        } else if (_startsWith(line, "TAG:")) {
            if (_contain(line, "VIRTUAL"))
                currentCompound->setVirtualFragmentation(true);

        } else if (_startsWith(line, "ION MODE:")
                   || _startsWith(line, "ION_MODE:")
                   || _startsWith(line, "IONMODE:")
                   || _startsWith(line, "IONIZATION:")) {
            if (_contain(line, "N"))
                currentCompound->ionizationMode = Compound::IonizationMode::Negative;
            if (_contain(line, "P"))
                currentCompound->ionizationMode = Compound::IonizationMode::Positive;

        } else if (_startsWith(line, "COMMENT:")
                   || _startsWith(line, "COMMENTS:")) {
            smatch match;
            string comment = line.substr(8, line.length());
            if (regex_search(comment, match, formulaMatch) && match.size() > 1) {
                currentCompound->setFormula (match.str(1));
            }
            if (regex_search(comment, match, retentionTimeMatch) && match.size() > 1) {
                currentCompound->setExpectedRt(string2float(match.str(1)));
            }

            // the following pattern logic extracts some useful information
            // available as comments in the MoNA public library available at:
            // https://mona.fiehnlab.ucdavis.edu/downloads
            string keggId = "";
            string hmdbId = "";
            string pubchemId = "";
            string chebi = "";
            string note = "";
            auto comment_begin = sregex_iterator( 
                                    comment.begin(), 
                                    comment.end(), 
                                    keyValuePattern); 
  
            auto comment_end = sregex_iterator();

            for (sregex_iterator k = comment_begin; 
                k != comment_end; 
                ++k) { 
  
                smatch match = *k; 
                string key = match.str(1); 
                string value = match.str(2);
                
                // replace SMILE if available and not already set
                if (_contain(key, "SMILE")
                    && currentCompound->smileString().empty()) {
                    currentCompound->setSmileString(value);
                }

                // replace category if available and not already set
                if (_contain(key, "compound class")
                    && currentCompound->category().empty()) {
                    string categories = value;
                    currentCompound->setCategory(mzUtils::split(categories,
                                                                "; "));
                }

                if (_contain(key, "kegg"))
                    keggId = value;
                if (_contain(key, "hmdb"))
                    hmdbId = value;
                if (_contain(key, "pubchem"))
                    pubchemId = value;
                if (_contain(key, "chebi"))
                    chebi = value;

                note += key;
                note += value;
            }

            if (!keggId.size()) {
                // KEGG gets precendence over HMDB
                currentCompound->setId(keggId);
            } else if (!hmdbId.size()) {
                // HMDB gets precendence over PubChem
                currentCompound->setId(hmdbId);
            } else if (!pubchemId.size()) {
                // PubChem gets precendence over ChEBI
                currentCompound->setId(pubchemId);
            } else if (!chebi.size()) {
                currentCompound->setId(chebi);
            }

            // comments are added as a note for the compound
            if (note.size()) {
                currentCompound->setNote(comment);
            } else {
                currentCompound->setNote(note);
            }

        } else if (_startsWith(line, "NUM PEAKS:")
                   || _startsWith(line, "NUMPEAKS:")) {
            capturePeaks = true;
        } else if (capturePeaks) {
            vector<string> mzIntensityPair;
            boost::split(mzIntensityPair, line, boost::is_any_of(" "));
            if (mzIntensityPair.size() >= 2) {
                double mz = string2float(mzIntensityPair[0]);
                double in = string2float(mzIntensityPair[1]);
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
                            mzIntensityPair.at(2);
                        currentCompound->setFragmentIonTypes(ionTypes);
                    }
                }
            }
        }
        ++currentLine;
        if (signal) {
            (*signal)("Loading spectral library: " + fileName,
                      currentLine,
                      lineCount);
        }
    }
    if (currentCompound and !currentCompound->name().empty()) {
        if (!currentCompound->formula().empty()) {
            auto formula = currentCompound->formula();
            auto exactMass = MassCalculator::computeMass(formula, 0);
            currentCompound->setMz(exactMass);
        }
        if (addCompound(currentCompound)){
            ++compoundCount;
        }
    }

    return compoundCount;
}

int Database::loadCompoundCSVFile(string file,
                                  bool isFileContent,
                                  string dbName,
                                  string sep)
{
    string line;
    int loadCount = 0, lineCount = 0;
    map<string, int> header;
    vector<string> headers;

    // lambda: depending on `lineCount`, will treat the incoming line as a
    // header or a row item
    auto processHeaderOrLine = [this,
                                &lineCount,
                                &headers,
                                &header,
                                &loadCount,
                                &dbName](string& line, string sep) {
        if (lineCount == 1) {
            auto fields = mzUtils::split(line, sep);
            mzUtils::removeSpecialCharFromStartEnd(fields);
            headers = fields;
            for(unsigned int i = 0; i < fields.size(); i++ ) {
                fields[i] = makeLowerCase(fields[i]);
                header[fields[i]] = i;
            }
        } else {
            // trim spaces on the left
            size_t found = line.find_last_not_of(" \n\t");
            if (found != string::npos) {
                line.erase(found + 1);
            } else {
                return;
            }

            vector<string> fields = mzUtils::splitCSVFields(line, sep);
            mzUtils::removeSpecialCharFromStartEnd(fields);
            Compound* compound = extractCompoundfromEachLine(fields,
                                                             header,
                                                             loadCount,
                                                             dbName);

            if (compound && addCompound(compound))
                ++loadCount;
        }
    };

    // reset the contents of the vector containing the names of invalid rows
    _invalidRows.clear();

    if (isFileContent) {
        auto allContent = mzUtils::split(file, "\n");
        for (auto line : allContent) {
            //This is used to write commands
            if (!line.empty() && line[0] == '#') 
                continue;

            ++lineCount;
            processHeaderOrLine(line, sep);
        }
    } else {
        ifstream filestream(file.c_str());
        if (!filestream.is_open())
            return 0;

        dbName = mzUtils::cleanFilename(file);

        // assume that files are tab delimited, unless matched ".csv",
        // then comma delimited
        string sep = "\t";
        if(file.find(".csv") != -1 || file.find(".CSV") != -1)
            sep = ",";

        while (getline(filestream, line)) {
            if (line.empty() || line[0] == '#')
                continue;

            // getting the heading from the csv File
            if (boost::algorithm::contains(line, "\r")) {
                vector<string> carriageSeparated;
                carriageSeparated = mzUtils::split(line, "\r");
                for (auto& crLine : carriageSeparated) {
                    if (crLine.empty() || crLine[0] == '#')
                        continue;

                    ++lineCount;
                    processHeaderOrLine(crLine, sep);
                }
            } else {
                ++lineCount;
                processHeaderOrLine(line, sep);
            }
        }
        filestream.close();
    }

    sort(_compoundsDB.begin(),_compoundsDB.end(), Compound::compMass);
    return loadCount;
}

Compound* Database::extractCompoundfromEachLine(vector<string>& fields, map<string, int> & header, int loadCount, string filename) {
    string id, name, formula, polarityString;
    string note;
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

    if (header.count("note") && header["note"] < NumOfFields)
        note = fields[header["note"]];

    categorylist = getCategoryFromDB(fields, header);

    charge = getChargeFromDB(fields, header);
    //If Some of the imp fields are not present here is th way it will be
    //assigned

    if (id.empty() && !name.empty()) 
        id = name;

    if (id.empty() && name.empty()) 
        id = "cmpd:" + integer2string(loadCount);

    //The compound should atleast have formula so that
    //mass can be calculated from the formula
    if ( mz > 0 || !formula.empty() || precursormz > 0) {
        Compound* compound = new Compound(id,name,formula,charge);

        compound->setExpectedRt (rt);

        if (mz == 0)
            mz = MassCalculator::computeMass(formula, charge);
        
        
        compound->setMz(mz);
        compound->setDb  (dbname);
        compound->setExpectedRt(rt);
        compound->setPrecursorMz (precursormz);
        compound->setProductMz( productmz);
        compound->setCollisionEnergy (collisionenergy);
        compound->setNote(note);

        vector<string> category;
        for(unsigned int i=0; i < categorylist.size(); i++) 
            category.push_back(categorylist[i]);
        compound->setCategory(category);
        return compound;
    }

    if (!name.empty())
        id = name;
    _invalidRows.push_back(id);

    return NULL;
    
}

float Database::getChargeFromDB(vector<string>& fields, map<string, int> & header) {
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

vector<string> Database::getCategoryFromDB(vector<string>& fields, map<string, int> & header) {
    vector<string> categorylist;
    int NumOfFields = fields.size();
    //What is category?
    if ( header.count("category") && header["category"] < NumOfFields) {
        string catstring = fields[header["category"]];
        if (!catstring.empty()) {
            categorylist = mzUtils::split(catstring,";");
            if(categorylist.size() == 0) categorylist.push_back(catstring);
        }
    }
    return categorylist;
}



////////////////////////Test cases////////////////////

TEST_CASE("Testing database class")
{
    SUBCASE("Testing CSV Loading File")
    {
        Database db;
        string csvFile = "tests/test-libmaven/test_loadCSV.csv";
        int rescsv = db.loadCompoundCSVFile(csvFile);
        REQUIRE(rescsv == 10);
        vector<Compound*> compounds = db.getCompoundsSubset("test_loadCSV");

        vector<Compound> compoundInput;
        Compound c1("HMDB00653", "cholesteryl sulfate",
                    "C27H46O4S", 0);
        c1.setExpectedRt(17.25);
        c1.setDb("test_loadCSV");
        compoundInput.push_back(c1);

        Compound c2("C05464", "Deoxycholic acid",
                    "C26H43NO5", 0);
        c2.setExpectedRt(16.79);
        c2.setDb("test_loadCSV");
        compoundInput.push_back(c2);
        
        Compound c3("C15H28O7P2","trans_trans-farnesyl diphosphate",
                    "C00448",0);
        c3.setExpectedRt(16.74);
        c3.setDb("test_loadCSV");
        compoundInput.push_back(c3);
        
        Compound c4("HMDB00619", "Cholic acid",
                    "C24H40O5", 0);
        c4.setExpectedRt(16.69);
        c4.setDb("test_loadCSV");
        compoundInput.push_back(c4);

        Compound c5("C00341", "Geranyl-PP",
                    "C10H20O7P2", 0) ;
        c5.setExpectedRt(16.46);
        c5.setDb("test_loadCSV");
        compoundInput.push_back(c5);

        Compound c6("C05463", "Taurodeoxycholic acid",
                    "C26H45NO6S", 0);
        c6.setExpectedRt(16.23);
        c6.setDb("test_loadCSV");
        compoundInput.push_back(c6);

        Compound c7("C00725", "lipoate",
                    "C8H14O2S2", 0);
        c7.setExpectedRt(15.97);
        c7.setDb("test_loadCSV");
        compoundInput.push_back(c7);

        Compound c8("C00356", "3-hydroxy-3-methylglutaryl-CoA-nega",
                    "C27H44N7O20P3S", 0);
        c8.setExpectedRt(15.72);
        c8.setDb("test_loadCSV");
        compoundInput.push_back(c8);

        Compound c9("C00630", "butyryl-CoA",
                    "C25H42N7O17P3S", 0);
        c9.setExpectedRt(15.72);
        c9.setDb("test_loadCSV");
        compoundInput.push_back(c9);

        Compound c10("C00083", "malonyl-CoA",
                    "C24H38N7O19P3S", 0);
        c10.setExpectedRt(15.7);
        c10.setDb("test_loadCSV");
        compoundInput.push_back(c10);

        for(size_t i = 0; i < compoundInput.size(); i++ ){
            for(size_t j = 0; j < compounds.size(); j++){
                if(compoundInput[i].id() == compounds[j]->id())
                {
                    Compound input = compoundInput[i];
                    Compound saved = *compounds[j];
                    REQUIRE(input == saved);
                }
            }
        }

        vector<Compound*> compoundId;
        compoundId = db.findSpeciesById("C00083",
                                       "test_loadCSV");
        for(size_t i = 0; i < compoundId.size(); i++){
            REQUIRE(compoundId[i]->id() == "C00083");
        }

        vector<Compound*> compoundName;
        compoundName = db.findSpeciesByName("malonyl-CoA",
                                           "test_loadCSV");
        for(size_t i = 0; i < compoundName.size(); i++){
            REQUIRE(compoundId[i]->name() == "malonyl-CoA");
        }

        vector<Compound*> subset;
        subset = db.getCompoundsSubset("test_loadCSV");
        for(size_t i = 0; i < subset.size(); i++ )
            REQUIRE(subset[i]->db() == "test_loadCSV");

        vector<Compound*> knowns;
        knowns = db.getKnowns();
        for(size_t i = 0; i < knowns.size(); i++)
            REQUIRE(knowns[i]->db() == "KNOWNS");

        auto isSpectral = db.isSpectralLibrary("test_loadCSV");
        REQUIRE(isSpectral == false);

    }

    SUBCASE("Testing MSP File")
    {
        Database db;
        string mspFile = "tests/test-libmaven/test_NISTLibrary.msp";
        int resMsp = db.loadNISTLibrary(mspFile);
        REQUIRE(resMsp == 10);
        vector<Compound*> compounds = db.getCompoundsSubset("test_NISTLibrary");

        Compound* c1 = new Compound("HMDB00902", "NAD-20.0,50.0,100.0",
                                   "C21H27N7O14P2", 0);
        c1->setMz(663.109121804);
        vector<string> category;
        category.push_back("None");
        c1->setCategory(category);
        c1->setSmileString("NC(=O)c1ccc[n+](C2OC(COP(=O)([O-])OP(=O)(O)OCC3OC(n4cnc5c(N)ncnc54)C(O)C3O)C(O)C2O)c1");
        c1->ionizationMode = Compound::IonizationMode::Negative;
        c1->setCollisionEnergy(200);
        c1->setLogP(-3.6479);
        c1->setDb("test_NISTLibrary");
        vector<float> mz1 = {62.964214325, 78.9593391418,
                                92.0254847209, 96.9695861816,
                                107.036354065, 134.047064209,
                                138.980453491, 146.98580424,
                                150.980529785, 158.925656128,
                                174.980461121, 176.936406453,
                                192.991004944, 211.001739502,
                                254.946289062, 272.956855774,
                                328.045959473, 346.056945801,
                                408.012435913, 426.022750854,
                                540.051066081, 540.052429199};

        c1->setFragmentMzValues(mz1);

        vector<float> ion1 = {248, 8816,97,821, 289, 2205, 56, 107,
                                56, 3351, 539, 95, 179, 130, 54, 1175,
                                523, 488, 427, 614, 7137, 7118};
        
        c1->setFragmentIntensities(ion1);

        map<int, string> fragmentIonTypes;
        fragmentIonTypes[0] = "2";
        fragmentIonTypes[1] = "4";
        fragmentIonTypes[2] = "3";
        fragmentIonTypes[3] = "5";
        fragmentIonTypes[4] = "3";
        fragmentIonTypes[5] = "5";
        fragmentIonTypes[6] = "2";
        fragmentIonTypes[7] = "3";
        fragmentIonTypes[8] = "2";
        fragmentIonTypes[9] = "5";
        fragmentIonTypes[10] = "4";
        fragmentIonTypes[11] = "3";
        fragmentIonTypes[12] = "4";
        fragmentIonTypes[13] = "4";
        fragmentIonTypes[14] = "2";
        fragmentIonTypes[15] = "4";
        fragmentIonTypes[16] = "4";
        fragmentIonTypes[17] = "4";
        fragmentIonTypes[18] = "4";
        fragmentIonTypes[19] = "4";
        fragmentIonTypes[20] = "3";
        fragmentIonTypes[21] = "2";
        c1->setFragmentIonTypes(fragmentIonTypes);

        REQUIRE(*c1 == *(compounds[0]));
        delete(c1);

        c1 = new Compound("HMDB00641","L-GLUTAMINE-20.0,50.0,100.0",
                          "C5H10N2O3", 0);
        c1->setMz(146.06914218);
        category.clear();
        category.push_back("None");
        c1->setCategory(category);
        c1->setSmileString("NC(=O)CCC(N)C(=O)O");
        c1->ionizationMode = Compound::IonizationMode::Negative;
        c1->setCollisionEnergy(200);
        c1->setLogP(-1.3362);
        c1->setDb("test_NISTLibrary");
        vector<float> mz2 = {52.0191116333, 58.0298690796,
                            66.0349807739, 67.0301628113,
                            70.0298519135, 71.0140228271,
                            71.0251178741, 72.0091705322,
                            72.0455932617, 72.9931793213,
                            74.0247955322, 81.0460205078,
                            82.0252304077, 82.0298728943,
                            84.0404510498, 84.0455093384,
                            86.0247936249, 97.0407562256,
                            98.0247980754, 99.0564365387,
                            101.071949005, 102.056114197,
                            107.02511851, 109.040693283,
                            125.035812378, 125.390640259,
                            127.042938232, 127.051460266,
                            128.035438538, 138.631530762,
                            142.038706462, 145.062143962 };
        
        c1->setFragmentMzValues(mz2);
        
        vector<float> ion2 = {51, 2961,57,225, 140, 34, 134,
                            254, 28, 42, 1813, 33,50, 925,
                            61, 1891, 253, 110, 117, 185,
                            548, 17, 108, 768, 397, 26, 42,
                            2527, 1147, 23, 34, 3244 };
        
        c1->setFragmentIntensities(ion2);

        fragmentIonTypes.clear();
        fragmentIonTypes[0] = "2";
        fragmentIonTypes[1] = "4";
        fragmentIonTypes[2] = "2";
        fragmentIonTypes[3] = "4";
        fragmentIonTypes[4] = "4";
        fragmentIonTypes[5] = "4";
        fragmentIonTypes[6] = "4";
        fragmentIonTypes[7] = "4";
        fragmentIonTypes[8] = "3";
        fragmentIonTypes[9] = "3";
        fragmentIonTypes[10] = "4";
        fragmentIonTypes[11] = "3";
        fragmentIonTypes[12] = "2";
        fragmentIonTypes[13] = "4";
        fragmentIonTypes[14] = "2";
        fragmentIonTypes[15] = "4";
        fragmentIonTypes[16] = "4";
        fragmentIonTypes[17] = "4";
        fragmentIonTypes[18] = "3";
        fragmentIonTypes[19] = "4";
        fragmentIonTypes[20] = "4";
        fragmentIonTypes[21] = "3";
        fragmentIonTypes[22] = "3";
        fragmentIonTypes[23] = "4";
        fragmentIonTypes[24] = "3";
        fragmentIonTypes[25] = "2";
        fragmentIonTypes[26] = "2";
        fragmentIonTypes[27] = "4";
        fragmentIonTypes[28] = "4";
        fragmentIonTypes[29] = "2";
        fragmentIonTypes[30] = "3";
        fragmentIonTypes[31] = "3";
        c1->setFragmentIonTypes(fragmentIonTypes);
    
        REQUIRE(*c1 == *compounds[1]);
        delete(c1);

        c1 = new Compound("HMDB00965","HYPOTAURINE-20.0,50.0,100.0",
                          "C2H7NO2S",0);
        c1->setMz(109.019749464);
        category.clear();
        category.push_back("None");
        c1->setCategory(category);
        c1->setSmileString("NCCS(=O)O");
        c1->setDb("test_NISTLibrary");
        c1->ionizationMode = Compound::IonizationMode::Negative;
        c1->setCollisionEnergy(200);
        c1->setLogP(-0.8332);
        vector<float> mz3 = {63.9624799093, 64.9702987671, 108.01247406};
        c1->setFragmentMzValues(mz3);
        vector<float> ion3 = {9645, 981, 671};
        c1->setFragmentIntensities(ion3);
        fragmentIonTypes.clear();
        fragmentIonTypes[0] = "3";
        fragmentIonTypes[1] = "3";
        fragmentIonTypes[2] = "2";
        c1->setFragmentIonTypes(fragmentIonTypes);
        REQUIRE(*c1 == *compounds[2]);
        delete(c1);

        c1 = new Compound("HMDB00175","INOSINE 5'-PHOSPHATE-20.0,50.0,100.0",
                          "C10H13N4O8P", 0);
        c1->setMz(348.047100006);
        c1->setDb("test_NISTLibrary");
        category.clear();
        category.push_back("None");
        c1->setCategory(category);
        c1->setSmileString("O=c1nc[nH]c2c1ncn2C1OC(COP(=O)(O)O)C(O)C1O");
        c1->ionizationMode = Compound::IonizationMode::Negative;
        c1->setCollisionEnergy(200);
        c1->setLogP(-2.1519);
        vector<float> mz4= {62.964050293, 65.0145721436, 66.0098419189,
                            78.9590606689, 92.0253245036, 96.9695968628,
                            135.031138102, 150.980484009, 192.991226196,
                            211.001693726, 347.040802002};
       
        c1->setFragmentMzValues(mz4);
        vector<float> ion4 = {61, 355, 67, 7527, 1374,
                            871, 230, 113, 69, 523,
                            6304};
        
        c1->setFragmentIntensities(ion4);

        fragmentIonTypes.clear();
        fragmentIonTypes[0] = "3";
        fragmentIonTypes[1] = "2";
        fragmentIonTypes[2] = "2";
        fragmentIonTypes[3] = "3";
        fragmentIonTypes[4] = "3";
        fragmentIonTypes[5] = "3";
        fragmentIonTypes[6] = "3";
        fragmentIonTypes[7] = "2";
        fragmentIonTypes[8] = "2";
        fragmentIonTypes[9] = "2";
        fragmentIonTypes[10] = "2";
        c1->setFragmentIonTypes(fragmentIonTypes);

        REQUIRE(*c1 == *compounds[3]);
        delete(c1);

        c1 = new Compound("HMDB00094","CITRATE-20.0,50.0,100.0",
                          "C6H8O7",0);
        c1->setDb("test_NISTLibrary");
        c1->setMz(192.027002596);
        c1->setCategory(category);
        c1->setSmileString("O=C(O)CC(O)(CC(=O)O)C(=O)O");
        c1->ionizationMode = Compound::IonizationMode::Negative;
        c1->setCollisionEnergy(200);
        c1->setLogP(-1.2485);
        vector<float> mz5= {57.0346002579, 58.9589700699,
                            59.0139074326, 67.0189723969,
                            72.9933039347, 73.029571533,
                            83.0139490763, 85.0295581818,
                            87.0087833405, 87.9253025055,
                            102.948770523, 103.040133158,
                            103.920129776, 105.93572998,
                            111.008729935, 123.94648234,
                            129.019475301, 130.998474121,
                            146.939005534, 147.030131022,
                            154.998906453, 173.009328206,
                            191.020187378};
        
        c1->setFragmentMzValues(mz5);
        vector<float> ion5={2598, 96, 337, 971, 27,
                            30, 33, 1730, 2943, 377,
                            540, 26, 48, 41, 4779, 29,
                            384, 40, 291, 33, 36, 198,
                            1634};
        
        c1->setFragmentIntensities(ion5);
        fragmentIonTypes.clear();
        fragmentIonTypes[0] = "4";
        fragmentIonTypes[1] = "4";
        fragmentIonTypes[2] = "4";
        fragmentIonTypes[3] = "4";
        fragmentIonTypes[4] = "3";
        fragmentIonTypes[5] = "3";
        fragmentIonTypes[6] = "3";
        fragmentIonTypes[7] = "4";
        fragmentIonTypes[8] = "4";
        fragmentIonTypes[9] = "4";
        fragmentIonTypes[10] = "4";
        fragmentIonTypes[11] = "3";
        fragmentIonTypes[12] = "4";
        fragmentIonTypes[13] = "4";
        fragmentIonTypes[14] = "4";
        fragmentIonTypes[15] = "3";
        fragmentIonTypes[16] = "3";
        fragmentIonTypes[17] = "3";
        fragmentIonTypes[18] = "3";
        fragmentIonTypes[19] = "3";
        fragmentIonTypes[20] = "3";
        fragmentIonTypes[21] = "3";
        fragmentIonTypes[22] = "3";
        c1->setFragmentIonTypes(fragmentIonTypes);
        REQUIRE(*c1 == *compounds[4]);
        delete(c1);

        c1 = new Compound("HMDB00094", "CITRATE-20.0,50.0,100.0",
                          "C6H8O7", 0);
        c1->setMz(192.027002596);
        c1->setDb("test_NISTLibrary");
        c1->setName("CITRATE-20.0,50.0,100.0 (1)");
        c1->setCategory(category);
        c1->setSmileString("O=C(O)CC(O)(CC(=O)O)C(=O)O");
        c1->ionizationMode = Compound::IonizationMode::Negative;
        c1->setCollisionEnergy(200);
        c1->setLogP(-1.2485);
        vector<float> mz6 = {57.0345865885, 58.9589468638,
                            59.0138645172, 67.0189628601,
                            83.0137527466, 85.0295346578,
                            87.0087547302, 87.9252548218,
                            102.948786418, 102.997827148,
                            103.919984182, 105.935714722,
                            106.943557739, 111.008686066,
                            123.94655482,129.019424438,
                            130.998524984, 146.938732147,
                            147.029987335, 154.998645782,
                            173.008839925, 191.020000458};
        
        c1->setFragmentMzValues(mz6);
        vector<float> ion6 = {2419, 265, 295, 509, 26,
                                1641, 2912, 1013, 703, 45,
                                218, 175, 48, 4798, 615,362,
                                31,296, 27,40,140, 1087};
      
        c1->setFragmentIntensities(ion6);
        fragmentIonTypes.clear();
        fragmentIonTypes[0] = "6";
        fragmentIonTypes[1] = "6";
        fragmentIonTypes[2] = "6";
        fragmentIonTypes[3] = "6";
        fragmentIonTypes[4] = "5";
        fragmentIonTypes[5] = "6";
        fragmentIonTypes[6] = "6";
        fragmentIonTypes[7] = "6";
        fragmentIonTypes[8] = "6";
        fragmentIonTypes[9] = "5";
        fragmentIonTypes[10] = "6";
        fragmentIonTypes[11] = "6";
        fragmentIonTypes[12] = "5";
        fragmentIonTypes[13] = "6";
        fragmentIonTypes[14] = "6";
        fragmentIonTypes[15] = "4";
        fragmentIonTypes[16] = "3";
        fragmentIonTypes[17] = "4";
        fragmentIonTypes[18] = "4";
        fragmentIonTypes[19] = "4";
        fragmentIonTypes[20] = "3";
        fragmentIonTypes[21] = "4";
        c1->setFragmentIonTypes(fragmentIonTypes);
        REQUIRE(*c1 == *compounds[5]);
        delete(c1);

        c1 = new Compound("HMDB00094","CITRATE-20.0,50.0,100.0",
                          "C6H8O7",0);
        c1->setDb("test_NISTLibrary");
        c1->setName("CITRATE-20.0,50.0,100.0 (2)");
        c1->setMz(192.027002596);
        c1->setCategory(category);
        c1->setSmileString("O=C(O)CC(O)(CC(=O)O)C(=O)O");
        c1->ionizationMode = Compound::IonizationMode::Negative;
        c1->setCollisionEnergy(200);
        c1->setLogP(-1.2485);
        vector<float> mz7 = {57.0345850405, 59.0138343464,
                            59.9852591621, 67.0189613674,
                            67.8221041361, 68.8053665161,
                            69.5959014893, 83.0137751653,
                            85.0295320594, 87.0087479301,
                            92.4431991577, 101.024281979,
                            102.948895264, 103.040128371,
                            108.914337158, 111.00138855,
                            111.008674622, 126.157676697,
                            129.019415463, 130.998455048,
                            147.030051736, 154.998764038,
                            173.009250641, 190.186141968,
                            191.020112879, 209.673370361};
        
        c1->setFragmentMzValues(mz7);
        vector<float> ion7 = {3297, 372, 14, 1177, 12,
                                37, 39, 47, 1748, 3150,
                                41, 20, 9,  29,  38, 77,
                                5432, 37, 450, 46, 42,
                                46, 173, 36,1360, 36};
        
        c1->setFragmentIntensities(ion7);
        fragmentIonTypes.clear();
        fragmentIonTypes[0] = "23";
        fragmentIonTypes[1] = "22";
        fragmentIonTypes[2] = "9";
        fragmentIonTypes[3] = "23";
        fragmentIonTypes[4] = "6";
        fragmentIonTypes[5] = "2";
        fragmentIonTypes[6] = "2";
        fragmentIonTypes[7] = "13";
        fragmentIonTypes[8] = "23";
        fragmentIonTypes[9] = "23";
        fragmentIonTypes[10] = "2";
        fragmentIonTypes[11] = "16";
        fragmentIonTypes[12] = "15";
        fragmentIonTypes[13] = "17";
        fragmentIonTypes[14] = "2";
        fragmentIonTypes[15] = "2";
        fragmentIonTypes[16] = "23";
        fragmentIonTypes[17] = "2";
        fragmentIonTypes[18] = "17";
        fragmentIonTypes[19] = "16";
        fragmentIonTypes[20] = "17";
        fragmentIonTypes[21] = "17";
        fragmentIonTypes[22] = "16";
        fragmentIonTypes[23] = "2";
        fragmentIonTypes[24] = "17";
        fragmentIonTypes[25] = "2";
        c1->setFragmentIonTypes(fragmentIonTypes);
        REQUIRE(*c1 == *compounds[6]);
        delete(c1);

        c1 = new Compound("HMDB00167","L-THREONINE-20.0,50.0,100.0",
                          "C4H9NO3",0);
        c1->setDb("test_NISTLibrary");
        c1->setMz(119.058243148);
        c1->setCategory(category);
        c1->setSmileString("CC(O)C(N)C(=O)O");
        c1->ionizationMode = Compound::IonizationMode::Negative;
        c1->setCollisionEnergy (200);
        c1->setLogP(-1.2209);
        vector<float> mz8 = {72.0091552734, 74.0247751872, 118.050979614};
        c1->setFragmentMzValues(mz8);
        vector<float> ion8 = {3389, 8436, 1811};
        c1->setFragmentIntensities(ion8);
        fragmentIonTypes.clear();
        fragmentIonTypes[0] = "3";
        fragmentIonTypes[1] = "3";
        fragmentIonTypes[2] = "2";
        c1->setFragmentIonTypes(fragmentIonTypes);
        REQUIRE(*c1 == *compounds[7]);
        delete(c1);

        c1 = new Compound("HMDB01366","PURINE-20.0,50.0,100.0",
                          "C5H4N4",0);

        c1->setDb("test_NISTLibrary");
        c1->setMz(120.043596128);
        c1->setCollisionEnergy(200);
        c1->ionizationMode = Compound::IonizationMode::Negative;
        c1->setCategory(category);
        c1->setSmileString("c1ncc2[nH]cnc2n1");
        c1->setLogP(0.3529);
        vector<float> mz9 = {64.0066299438, 65.0145670573,
                            67.0302454631,68.0254770915,
                            90.0099182129, 92.0254681905,
                            119.036338806};
        c1->setFragmentMzValues(mz9);
        vector<float> ion9 = {62, 1786, 515, 204, 257,
                                1913,9786};
        c1->setFragmentIntensities(ion9);
        fragmentIonTypes.clear();
        fragmentIonTypes[0] = "6";
        fragmentIonTypes[1] = "6";
        fragmentIonTypes[2] = "6";
        fragmentIonTypes[3] = "6";
        fragmentIonTypes[4] = "6";
        fragmentIonTypes[5] = "6";
        fragmentIonTypes[6] = "6";
        c1->setFragmentIonTypes(fragmentIonTypes);
        REQUIRE(*c1 == *compounds[8]);
        delete(c1);

        Compound* comp = db.findSpeciesByIdAndName("HMDB01366",
                                                   "PURINE-20.0,50.0,100.0",
                                                   "test_NISTLibrary");
        REQUIRE(comp);

    }

    SUBCASE("Testing Mascot Library")
    {
        Database db;
        string mgfFile = "tests/test-libmaven/test_Mascot.mgf";
        int resMgf = db.loadMascotLibrary(mgfFile);
        REQUIRE(resMgf == 10);
        vector<Compound*> compounds = db.getCompoundsSubset("test_Mascot");

        Compound* c1 = new Compound("HMDB:HMDB04095-2361 5-Methoxytryptamine M-H",
                                    "HMDB:HMDB04095-2361 5-Methoxytryptamine M-H",
                                    "", 1);
        c1->setMz(189.103);
        c1->setExpectedRt(-1/60.0f);
        c1->ionizationMode = Compound::IonizationMode::Positive;
        c1->setSmileString("COC1=CC2=C(NC=C2CCN)C=C1");
        c1->setPrecursorMz(189.103);
        c1->setDb("test_Mascot");
        vector<float> mz1 = {143.0,144.0,173.0, 175.0,
                            188.0, 190.0};
        c1->setFragmentMzValues(mz1);

        vector<float> ion1 = {7.605, 2.369, 100.0,
                            3.178, 87.625, 2.304};
        
        c1->setFragmentIntensities(ion1);
        REQUIRE(*c1 == *compounds[0]);
        delete (c1);


        c1 = new Compound("HMDB:HMDB00099-156 L-Cystathionine M+H",
                          "HMDB:HMDB00099-156 L-Cystathionine M+H",
                          "", 1);
        c1->setMz(223.075);
        c1->setExpectedRt(-1/60.0f);
        c1->ionizationMode = Compound::IonizationMode::Positive;
        c1->setSmileString("N[C@@H](CCSC[C@H](N)C(O)=O)C(O)=O");
        c1->setPrecursorMz(223.075);
        c1->setDb("test_Mascot");
        vector<float> mz2 = {56.261, 88.097,
                        133.997, 177.012,
                        222.983};
        
        c1->setFragmentMzValues(mz2);
        vector<float> ion2 = {5.478,	9.877, 100.0,
                         3.318,91.975};
        
        c1->setFragmentIntensities(ion2);
        REQUIRE(*c1 == *compounds[1]);
        delete (c1);


        c1 = new Compound("HMDB:HMDB00021-38 Iodotyrosine M+H",
                          "HMDB:HMDB00021-38 Iodotyrosine M+H",
                          "", 1);
        c1->setMz(307.978);
        c1->setExpectedRt(-1/60.0f);
        c1->ionizationMode = Compound::IonizationMode::Positive;
        c1->setSmileString("N[C@@H](CC1=CC=C(O)C(I)=C1)C(O)=O");
        c1->setPrecursorMz(307.978);
        c1->setDb("test_Mascot");
        vector<float> mz3 = {89.975, 94.035, 106.854,
                            108.236, 118.003, 119.876,
                            134.711, 248.732, 261.798};
        
        c1->setFragmentMzValues(mz3);
        vector<float> ion3 = {5.266, 47.917, 28.935, 11.285,
                                21.296, 14.525, 100.0, 8.912,
                                11.69};
        
        c1->setFragmentIntensities(ion3);
        REQUIRE(*c1 == *compounds[2]);
        delete (c1);

        c1 = new Compound("HMDB:HMDB00145-218 Estrone M+H",
                          "HMDB:HMDB00145-218 Estrone M+H",
                          "", 1);
        c1->setMz(271.17);
        c1->setExpectedRt(-1/60.0f);
        c1->ionizationMode = Compound::IonizationMode::Positive;
        c1->setSmileString("[H][C@@]12CCC(=O)[C@@]1(C)CC[C@]1([H])C3=C(CC[C@@]21[H])C=C(O)C=C3");
        c1->setPrecursorMz(271.17);
        c1->setDb("test_Mascot");
        vector<float> mz4 = {199.0, 271.0, 272.0};
        c1->setFragmentMzValues(mz4);
        vector<float> ion4 = {15.265, 100.0, 23.44};
        c1->setFragmentIntensities(ion4);
        REQUIRE(*c1 == *compounds[3]);
        delete (c1);

        c1 = new Compound("HMDB:HMDB00258-449 Sucrose M+H",
                          "HMDB:HMDB00258-449 Sucrose M+H",
                          "", 1);
        c1->setMz(343.124);
        c1->setExpectedRt(-1/60.0f);
        c1->ionizationMode = Compound::IonizationMode::Positive;
        c1->setSmileString("OC[C@H]1O[C@@](CO)(O[C@H]2O[C@H](CO)[C@@H](O)[C@H](O)[C@H]2O)[C@@H](O)[C@@H]1O");
        c1->setPrecursorMz(343.124);
        c1->setDb("test_Mascot");
        vector<float> mz5 = {55.239, 69.118, 73.123,
                            85.059, 97.026, 99.025,
                            108.994, 114.982, 126.974,
                            144.97, 162.881};
        c1->setFragmentMzValues(mz5);
        vector<float> ion5 = {6.379, 11.724, 5.172,
                            100.0, 19.138, 6.983,
                            5.302, 5.129, 55.172,
                            31.552, 13.103};
       
        c1->setFragmentIntensities(ion5);
        REQUIRE(*c1 == *compounds[4]);
        delete (c1);

        c1 = new Compound("HMDB:HMDB00032-52 7-Dehydrocholesterol M+H",
                          "HMDB:HMDB00032-52 7-Dehydrocholesterol M+H",
                          "", 1);
        c1->setMz(385.347);
        c1->setExpectedRt(-1/60.0f);
        c1->ionizationMode = Compound::IonizationMode::Positive;
        c1->setSmileString("[H][C@@]12CC[C@H]([C@H](C)CCCC(C)C)[C@@]1(C)CC[C@@]1([H])C2=CC=C2C[C@@H](O)CC[C@]12C");
        c1->setPrecursorMz(385.347);
        c1->setDb("test_Mascot");
        vector<float> mz6 = {43.315, 55.297,
                            57.193, 69.167,
                            71.228, 81.111,
                            83.101, 85.037,
                            93.281, 95.201,
                            97.106, 99.081,
                            105.068, 107.083,
                            109.136, 110.869,
                            119.285, 121.033,
                            122.954,
                            131.261, 132.947,
                            135.008, 142.691,
                            145.095, 146.953,
                            148.851, 157.174,
                            158.173, 159.094,
                            160.211, 161.179,
                            163.264, 169.01,
                            170.939, 172.821,
                            175.124, 176.756,
                            182.901, 185.072,
                            187.18, 188.937,
                            195.309, 196.41,
                            197.409, 198.768,
                            199.65, 201.102,
                            203.109, 209.27,
                            211.246, 212.027,
                            212.855, 213.628,
                            214.932, 217.048,
                            218.774, 223.077,
                            225.154, 226.021,
                            226.934, 228.957,
                            231.354, 237.11,
                            239.015, 241.1,
                            254.22, 255.048,
                            325.076, 339.315,
                            349.47, 367.233,
                            367.999, 385.161,
                            386.021};
        
        c1->setFragmentMzValues(mz6);

        vector<float> ion6 = {18.02, 21.827, 45.685,
                            70.558, 62.437, 83.756,
                            63.959, 38.579, 26.904,
                            70.558, 38.579, 17.893,
                            27.284, 40.102, 56.853,
                            25.127, 25.761, 43.655,
                            32.995, 19.543, 35.533,
                            31.091, 21.32, 36.041,
                            42.132, 26.904, 40.102,
                            22.589, 58.376, 23.223,
                            54.315, 25.888, 22.843,
                            38.071, 47.716, 30.838,
                            22.589, 22.462, 35.025,
                            36.041, 24.492, 23.604,
                            20.685, 26.777, 57.868,
                            28.68, 32.995, 20.051,
                            21.32, 34.518, 18.02,
                            56.853, 20.685, 32.995,
                            17.64, 19.67, 19.67,
                            17.513, 17.893, 51.777,
                            24.619, 22.97, 34.518,
                            22.97, 28.299, 22.462,
                            45.178, 19.797, 19.162,
                            22.081, 62.944, 17.893,
                            100.0, 32.995};
        
        c1->setFragmentIntensities(ion6);
        REQUIRE(*c1 == *compounds[5]);
        delete (c1);

        c1 = new Compound("HMDB:HMDB00536-758 Adenylsuccinic acid M+H",
                          "HMDB:HMDB00536-758 Adenylsuccinic acid M+H",
                          "", 1);
        c1->setMz(464.082);
        c1->setExpectedRt(-1/60.0f);
        c1->ionizationMode = Compound::IonizationMode::Positive;
        c1->setSmileString("O[C@@H]1[C@@H](COP(O)(O)=O)O[C@H]([C@@H]1O)N1C=NC2=C1N=CN=C2NC(CC(O)=O)C(O)=O");
        c1->setPrecursorMz(464.082);
        c1->setDb("test_Mascot");
        vector<float> mz7 = {251.0, 252.0,
                            253.0, 386.0,
                            387.0, 431.0,
                            463.0, 464.0,
                            465.0};
       
        c1->setFragmentMzValues(mz7);
        vector<float> ion7 = {65.439, 36.631, 4.831,
                                3.04, 3.697, 2.281,
                                83.455, 100.0, 38.559};
       
        c1->setFragmentIntensities(ion7);
        REQUIRE(*c1 == *compounds[6]);
        delete (c1);

        c1 = new Compound("HMDB:HMDB00620-837 Glutaconic acid M-H",
                          "HMDB:HMDB00620-837 Glutaconic acid M-H",
                          "", 1);
        c1->setMz(129.019);
        c1->setExpectedRt(-1/60.0f);
        c1->ionizationMode = Compound::IonizationMode::Positive;
        c1->setSmileString("OC(=O)C\\C=C\\C(O)=O");
        c1->setPrecursorMz(129.019);
        c1->setDb("test_Mascot");
        vector<float> mz8 = {41.317, 85.036, 128.921};
        c1->setFragmentMzValues(mz8);
        vector<float> ion8 = {16.447, 100.0, 2.237};
        c1->setFragmentIntensities(ion8);
        REQUIRE(*c1 == *compounds[7]);
        delete (c1);

        c1 = new Compound("HMDB:HMDB00700-976 Hydroxypropionic acid M+H",
                          "HMDB:HMDB00700-976 Hydroxypropionic acid M+H",
                          "", 1);
        c1->setMz(91.0395);
        c1->setExpectedRt(-1/60.0f);
        c1->ionizationMode = Compound::IonizationMode::Positive;
        c1->setSmileString("OCCC(O)=O");
        c1->setPrecursorMz(91.0395);
        c1->setDb("test_Mascot");
        vector<float> mz9 = {54.152, 55.397,
                            55.522, 63.364,
                            72.451, 73.322,
                            73.571, 89.878,
                            91.496, 91.87};
        
        c1->setFragmentMzValues(mz9);
        vector<float> ion9 = {10.812, 28.713, 9.206,
                                13.339, 25.173, 100.0,
                                88.755, 23.212, 52.287,
                                10.032};
        
        c1->setFragmentIntensities(ion9);
        REQUIRE(*c1 == *compounds[8]);
        delete (c1);

        c1 = new Compound("HMDB:HMDB00705-984 Hexanoylcarnitine M+H",
                          "HMDB:HMDB00705-984 Hexanoylcarnitine M+H",
                          "", 1);
        c1->setMz(260.186);
        c1->setExpectedRt(-1/60.0f);
        c1->ionizationMode = Compound::IonizationMode::Positive;
        c1->setSmileString("CCCCCC(=O)OC(CC([O-])=O)C[N+](C)(C)C");
        c1->setPrecursorMz(260.186);
        c1->setDb("test_Mascot");
        vector<float> mz10 = {60.437, 85.247,
                            99.242, 144.142,
                            201.043, 260.119};
        
        c1->setFragmentMzValues(mz10);
        vector<float> ion10 = {17.549, 74.118, 5.221,
                            1.863, 37.647, 100.0};
        
        c1->setFragmentIntensities(ion10);
        REQUIRE(*c1 == *compounds[9]);
        delete (c1);

    }

    SUBCASE("Testing load Adducts")
    {
        Database db;
        string adductFile = "tests/test-libmaven/test_adducts.csv";
        db.loadAdducts(adductFile);
        vector<Adduct*> adductDb = db.adductsDB();
        REQUIRE(adductDb.size() == 10);
        vector<Adduct> adductInput;

        Adduct a1("[M+H]+", 1, 1, 1.0072277);
        adductInput.push_back(a1);
        
        Adduct a2("[M+NH4]+", 1, 1, 18.033823);
        adductInput.push_back(a2);
        
        Adduct a3("[M+H3O]+", 1, 1, 19.01839);
        adductInput.push_back(a3);
       
        Adduct a4("[M+Na]+", 1, 1, 22.98992);
        adductInput.push_back(a4);
        
        Adduct a5("[M+CH3OH+H]+", 1, 1, 33.0335);
        adductInput.push_back(a5);
       
        Adduct a6("[M+K]+", 1, 1, 38.963158);
        adductInput.push_back(a6);
        
        Adduct a7("[M+ACN+H]+", 1, 1, 42.033777);
        adductInput.push_back(a7);
       
        Adduct a8("[M+2Na-H]+", 1, 1, 44.972616);
        adductInput.push_back(a8);
       
        Adduct a9("[M+ACN+Na]+", 1, 1, 64.016471);
        adductInput.push_back(a9);
        
        Adduct a10("[M+NaHO2CH]+", 1, 1, 68.995402);
        adductInput.push_back(a10);
        
        for(size_t i = 0; i < adductInput.size(); i++ ){
            for(size_t j = 0; j < adductDb.size(); j++){
                if(adductInput[i].getName() == adductDb[j]->getName())
                {
                    REQUIRE(adductInput[i].getMass() == adductDb[j]->getMass());
                    REQUIRE(adductInput[i].getCharge() == adductDb[j]->getCharge());
                    REQUIRE(adductInput[i].getNmol() == adductDb[j]->getNmol());
                    REQUIRE(adductInput[i].getName() == adductDb[j]->getName());
                }
            }
        }

        auto adduct = db.findAdductByName("[M+NaHO2CH]+");
        REQUIRE(adduct->getName() == "[M+NaHO2CH]+");
    }

    SUBCASE("Testing miscellaneous function") {
        Database db;
        string mgfFile = "tests/test-libmaven/test_Mascot.mgf";
        int resMgf = db.loadMascotLibrary(mgfFile);
        string mspFile = "tests/test-libmaven/test_NISTLibrary.msp";
        int resMsp = db.loadNISTLibrary(mspFile);
        string csvFile = "tests/test-libmaven/test_loadCSV.csv";
        int rescsv = db.loadCompoundCSVFile(csvFile);

        REQUIRE(db.compoundsDB().size() == 30);
        // Remove Database.
        db.removeDatabase("test_Mascot");
        REQUIRE(db.compoundsDB().size() == 20);


        // Get database name
        auto names = db.getDatabaseNames();

        auto itr = names.begin();

        REQUIRE(itr->first == "test_NISTLibrary");
        REQUIRE(itr->second == 10);
        itr++;
        REQUIRE(itr->first == "test_loadCSV");
        REQUIRE(itr->second == 10);

    }

}
