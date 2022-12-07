#include "mzSample.h"
#include "base64.h"
#include "datastructures/mzSlice.h"
#include "statistics.h"
#include "mzUtils.h"
#include "mzPatterns.h"
#include "mzFit.h"
#include "masscutofftype.h"
#include "mzMassCalculator.h"
#include "Matrix.h"
#include "EIC.h"
#include "Scan.h"

#include <MavenException.h>

// global options
int mzSample::filter_minIntensity = -1;
bool mzSample::filter_centroidScans = false;
int mzSample::filter_intensityQuantile = 0;
int mzSample::filter_polarity = 0;
int mzSample::filter_mslevel = 0;

mzSample::mzSample() : _setName(""), injectionOrder(0)
{
    _id = -1;
    _numMS1Scans = 0;
    _numMS2Scans = 0;
    _msMsType = MsMsType::None;
    maxMz = maxRt = 0;
    minMz = minRt = 0;
    isBlank = false;
    isSelected = true;
    maxIntensity = 0;
    minIntensity = 0;
    totalIntensity = 0;
    _normalizationConstant = 1;  // TODO: Sahil Not being used anywhere
    injectionTime = 0;
    _sampleOrder = -1;
    sampleNumber = -1;
    _C13Labeled = false;
    _N15Labeled = false;
    _S34Labeled = false;  // Feng note: added to track S34 labeling state
    _D2Labeled = false;   // Feng note: added to track D2 labeling state
    // _setName =  "A"; //naman     Variable '_setName' is assigned in
    // constructor body. Consider performing initialization in initialization
    // list.
    color[0] = color[1] = color[2] = 0;
    color[3] = 1.0;
}

mzSample::~mzSample()
{
    for (unsigned int i = 0; i < scans.size(); i++)
        if (scans[i] != NULL)
            delete (scans[i]);
    scans.clear();
}

void mzSample::addScan(Scan* s)
{
    if (!s)
        return;

    // skip scans that do not match mslevel
    if (mzSample::filter_mslevel and s->mslevel != mzSample::filter_mslevel) {
        delete s;
        return;
    }

    // skip scans that do not match polarity
    if (mzSample::filter_polarity
        and s->getPolarity() != mzSample::filter_polarity) {
        delete s;
        return;
    }

    // unsigned int sizeBefore = s->intensity.size();
    if (mzSample::filter_centroidScans == true) {
        s->simpleCentroid();
    }

    // unsigned int sizeAfter1 = s->intensity.size();

    if (mzSample::filter_intensityQuantile > 0) {
        s->quantileFilter(mzSample::filter_intensityQuantile);
    }
    // unsigned int sizeAfter2 = s->intensity.size();

    if (mzSample::filter_minIntensity > 0) {
        s->intensityFilter(mzSample::filter_minIntensity);
    }
    // unsigned int sizeAfter3 = s->intensity.size();
    // cerr << "addScan " << sizeBefore <<  " " << sizeAfter1 << " " <<
    // sizeAfter2 << " " << sizeAfter3 << endl;

    if (s->mslevel == 1)
        ++_numMS1Scans;
    if (s->mslevel == 2)
        ++_numMS2Scans;

    scans.push_back(s);
    s->scannum = scans.size() - 1;

    //recalculate precursorMz of MS2 scans
    if (s->mslevel == 2 && _numMS1Scans > 0) {
        float ppm = 10;
        s->recalculatePrecursorMz(ppm);
    }
}

string mzSample::getFileName(const string& filename)
{
    char sep = '/';

    size_t i = filename.rfind(sep, filename.length());

    if (i != string::npos) {
        string fullname = filename.substr(i + 1, filename.length() - i);
        return (fullname.substr(0, fullname.find_last_of(".")));
    }

    return ("");
}

void mzSample::loadAnySample(string filename)
{
    if (contains(filename, "mzCSV")) {
        parseMzCSV(filename.c_str());
    } else if (contains(filename, "mzdata")) {
        parseMzData(filename.c_str());
    } else if (contains(filename, "mzxml")) {
        parseMzXML(filename.c_str());
    } else if (contains(filename, "mzml")) {
        parseMzML(filename.c_str());
    } else if (contains(filename, "cdf")) {
        parseCDF(filename.c_str(), 1);
    } else {
        parseMzData(filename.c_str());
    }
}

void mzSample::sampleNaming(const char* filename)
{
    string filenameString = string(filename);

    // This is the complete file name
    this->sampleName = getFileName(filenameString);

    // This is the sample name
    this->fileName = filenameString;
}

void mzSample::checkSampleBlank(const char* filename)
{
    string filenameString = string(filename);
    this->isBlank = false;

    makeLowerCase(filenameString);
    if (filenameString.find("blank") != string::npos) {
        this->isBlank = true;
    }
}

void mzSample::loadSample(string filename)
{
    // Loading and Decoding the file
    // catch any error while parsing
    try {
        loadAnySample(filename);
    } catch (MavenException& excp) {
        cerr << endl << "Error: " << excp.what() << endl;
    }

    // getting the SRM scan type
    enumerateSRMScans();

    // set min and max values for rt and mz
    calculateMzRtRange();

    // Setting Sample name
    sampleNaming(filename.c_str());

    // Checking if a sample is blank or not
    checkSampleBlank(filename.c_str());
}

void mzSample::parseMzCSV(const char* filename)
{
    // file structure:
    // scannum,rt,mz,intensity,mslevel,precursorMz,polarity,srmid
    int lineNum = 0;

    ifstream myfile(filename);
    if (!myfile.is_open())
        throw(MavenException(ErrorMsg::FileNotFound));

    std::stringstream ss;
    std::string line;
    std::string polarity;

    int lastScanNum = -1;
    int scannum = 0;
    float rt = 0;
    float intensity = 0;
    float mz = 0;
    float precursorMz = 0;
    int mslevel = 0;

    Scan* scan = NULL;
    int newscannum = 0;

    while (getline(myfile, line)) {
        lineNum++;
        vector<string> fields;
        fields = mzUtils::split(line, ",");
        if (fields.size() >= 5 && lineNum > 1) {
            ss.clear();

            ss << fields[0] << " " << fields[1] << " " << fields[2] << " "
               << fields[3] << " " << fields[4] << " " << fields[5] << " "
               << fields[6];

            ss >> scannum >> rt >> mz >> intensity >> mslevel >> precursorMz
                >> polarity;

            if (scannum != lastScanNum) {
                newscannum++;
                if (mslevel <= 0)
                    mslevel = 1;
                int scanpolarity = 0;
                if (polarity.empty() && fields.size() > 7)
                    polarity = fields[7];
                if (!polarity.empty() && polarity[0] == '+')
                    scanpolarity = 1;
                if (!polarity.empty() && polarity[0] == '-')
                    scanpolarity = -1;
                scan = new Scan(this,
                                newscannum,
                                mslevel,
                                rt / 60,
                                precursorMz,
                                scanpolarity);
                if (mslevel > 1)
                    scan->productMz = mz;

                addScan(scan);
                if (fields.size() > 7)
                    scan->filterLine = fields[7];  // last field is srmId
            }

            scan->mz.push_back(mz);
            scan->intensity.push_back(intensity);
            lastScanNum = scannum;
        }
    }
}

// void mzSample::writeMzCSV(const char* filename) const {
void mzSample::writeMzCSV(const char* filename)
{
    // TODO naman unused function

    ofstream mzCSV;
    mzCSV.open(filename);
    if (!mzCSV.is_open()) {
        cerr << "Unable to write to a file" << filename;
        return;
    }

    mzCSV << "scannum,rt,mz,intensity,mslevel,precursorMz,polarity,srmid"
          << endl;
    for (unsigned int i = 0; i < scans.size(); i++) {
        Scan* scan = scans[i];
        for (unsigned int j = 0; j < scan->nobs(); j++) {
            mzCSV << scan->scannum + 1 << "," << scan->rt * 60 << ","
                  << scan->mz[j] << "," << scan->intensity[j] << ","
                  << scan->mslevel << "," << scan->precursorMz << ","
                  << (scan->getPolarity() > 0 ? "+" : "-") << ","
                  << scan->filterLine << endl;
        }
    }
}

int mzSample::getPolarity()
{
    if (scans.size() > 0)
        return scans[0]->getPolarity();
    return 0;
}
void mzSample::parseMzML(const char* filename)
{
    xml_document doc;

    const unsigned int parse_options = parse_minimal;

    pugi::xml_parse_result parseResult = doc.load_file(filename, parse_options);
    if (parseResult.status != pugi::xml_parse_status::status_ok) {
        throw MavenException(ErrorMsg::ParsemzMl);
    }

    // Get injection time stamp
    xml_node experimentRun =
        doc.first_child().first_element_by_path("mzML/run");
    if (!experimentRun.empty()) {
        parseMzMLInjectionTimeStamp(experimentRun.attribute("startTimeStamp"));
    }

    // Get a spectrumstore node
    xml_node chromatogramList =
        doc.first_child().first_element_by_path("mzML/run/chromatogramList");
    xml_node spectrumList =
        doc.first_child().first_element_by_path("mzML/run/spectrumList");

    if (!spectrumList.empty()) {
        parseMzMLSpectrumList(spectrumList);
    } else if (!chromatogramList.empty()) {
        parseMzMLChromatogramList(chromatogramList);
    }
}

void mzSample::parseMzMLInjectionTimeStamp(
    const xml_attribute& injectionTimeStamp)
{
    if (!injectionTimeStamp.empty()) {
        using namespace date;
        string time_details = injectionTimeStamp.value();
        istringstream in;
        in.str(time_details);
        sys_seconds timeStamp;
        in >> parse("%Y-%m-%dT%T", timeStamp);
        injectionTime = timeStamp.time_since_epoch().count();
    }
}

void mzSample::parseMzMLChromatogramList(const xml_node& chromatogramList)
{
    int scannum = 0;
    for (xml_node chromatogram = chromatogramList.child("chromatogram");
         chromatogram;
         chromatogram = chromatogram.next_sibling("chromatogram")) {
        string chromatogramId = chromatogram.attribute("id").value();
        int sampleNo = getSampleNoChromatogram(chromatogramId);

        cleanFilterLine(chromatogramId);

        int scanPolarity = -1;
        map<string, string> chromatogramParams = mzML_cvParams(chromatogram);
        if (chromatogramParams.count("positive scan")) {
            scanPolarity = 1;
        } else if (chromatogramParams.count("negative scan")) {
            scanPolarity = -1;
        }

        vector<float> timeVector;
        vector<float> intsVector;

        xml_node binaryDataArrayList =
            chromatogram.child("binaryDataArrayList");
        string precursorMzStr =
            chromatogram
                .first_element_by_path("precursor/isolationWindow/cvParam")
                .attribute("value")
                .value();
        string productMzStr =
            chromatogram
                .first_element_by_path("product/isolationWindow/cvParam")
                .attribute("value")
                .value();
        float precursorMz = string2float(precursorMzStr);
        float productMz = string2float(productMzStr);

        xml_node activationNode =
            chromatogram.first_element_by_path("precursor/activation");
        map<string, string> activationParams = mzML_cvParams(activationNode);
        float collisionEnergy = 0.0f;
        if (activationParams.count("collision energy"))
            collisionEnergy = string2float(activationParams["collision energy"]);

        for (xml_node binaryDataArray = binaryDataArrayList.child("binaryDataArray");
             binaryDataArray;
             binaryDataArray =
                 binaryDataArray.next_sibling("binaryDataArray")) {

            map<string, string> attr = mzML_cvParams(binaryDataArray);

            int precision = 64;
            if (attr.count("32-bit float"))
                precision = 32;

            bool decompress = false;
            if(attr.count("zlib compression"))
                decompress=true;

            if (attr.count("positive scan")) {
                scanPolarity = 1;
            } else if (attr.count("negative scan")) {
                scanPolarity = -1;
            }

            string binaryDataStr =
                binaryDataArray.child("binary").child_value();
            vector<float> binaryData = base64::decodeBase64(binaryDataStr,
                                                             precision / 8,
                                                             false,
                                                             decompress);

            if (attr.count("time array")) {
                timeVector = binaryData;
            }
            if (attr.count("intensity array")) {
                intsVector = binaryData;
            }
        }

        // NOTE: if precursor and product m/z values are present then we treat
        // this as a MS2 scan (MRM), but if only precursor m/z is present then
        // we treat this as a SIM (selected ion monitoring) MS1 scan.
        if (precursorMz) {
            int mslevel = 1;
            if (productMz > 0.0f)
                mslevel = 2;

            // FIXME: a scan created for each data point! This is extremely
            // wasteful - maybe we should directly create EICs and store them
            // within the sample object for MRM data.
            for (unsigned int i = 0; i < timeVector.size(); i++) {
                Scan* scan = new Scan(this,
                                      scannum++,
                                      mslevel,
                                      timeVector[i],
                                      precursorMz,
                                      scanPolarity);

                scan->collisionEnergy = collisionEnergy;
                if (scan->collisionEnergy > 0.0f) {
                    scan->filterLine = chromatogramId
                                       + " CE: "
                                       + to_string(collisionEnergy);
                } else {
                    scan->filterLine = chromatogramId;
                }

                scan->productMz = productMz;
                if (productMz > 0.0f) {
                    scan->mz.push_back(productMz);
                } else {
                    scan->mz.push_back(precursorMz);
                }

                scan->intensity.push_back(intsVector[i]);
                sampleNumber = sampleNo;
                addScan(scan);
            }
        }
    }

    // renumber scans based on retention time
    std::sort(scans.begin(), scans.end(), Scan::compRt);
    for (unsigned int i = 0; i < scans.size(); i++) {
        scans[i]->scannum = i;
    }
}

int mzSample::getSampleNoChromatogram(const string& chromatogramId)
{
    std::regex rxSampleNumber("sample\ *\=\ *([0-9]+)\ ");
    std::vector<int> results;
    for (std::sregex_iterator i = std::sregex_iterator(
             chromatogramId.begin(), chromatogramId.end(), rxSampleNumber);
         i != std::sregex_iterator();
         ++i) {
        std::smatch m = *i;
        results.push_back(std::stoi(m[1].str().c_str()));
    }

    if (results.size() > 0)
        return results[0];
    else
        return -1;
}

void mzSample::cleanFilterLine(string &filterline) {

    string filterRegex;
    regex rx;
    for (const string& filterId : filterChromatogram) {
        filterRegex = filterId + "\ *\=\ *[0-9]*\.?[0-9]+";
        rx = filterRegex;
        filterline = std::regex_replace(filterline, rx, "");
	}
}

void mzSample::parseMzMLSpectrumList(const xml_node& spectrumList)
{
    // Iterate through spectrums
    int scannum = 0;

    for (xml_node spectrum = spectrumList.child("spectrum"); spectrum;
         spectrum = spectrum.next_sibling("spectrum")) {
        string spectrumId = spectrum.attribute("id").value();

        if (spectrum.empty())
            continue;
        map<string, string> cvParams = mzML_cvParams(spectrum);

        int mslevel = 1;
        int scanpolarity = 0;
        float rt = 0;
        vector<float> mzVector;
        vector<float> intsVector;

        if (cvParams.count("ms level")) {
            string msLevelStr = cvParams["ms level"];
            mslevel = (int)string2float(msLevelStr);
        }

        if (cvParams.count("positive scan"))
            scanpolarity = 1;
        else if (cvParams.count("negative scan"))
            scanpolarity = -1;
        else
            scanpolarity = 0;

        xml_node scanNode = spectrum.first_element_by_path("scanList/scan");
        map<string, string> scanAttr = mzML_cvParams(scanNode);
        if (scanAttr.count("scan start time minute")) {
            string rtStr = scanAttr["scan start time minute"];
            rt = string2float(rtStr);
        } else if (scanAttr.count("scan start time second")) {
            string rtStr = scanAttr["scan start time second"];
            rt = string2float(rtStr) / 60.0f;
        }

        if (scanAttr.count("filter string")) {
            spectrumId = scanAttr["filter string"];
        }
        cleanFilterLine(spectrumId);

        map<string, string> isolationWindow =
            mzML_cvParams(spectrum.first_element_by_path(
                "precursorList/precursor/isolationWindow"));
        string precursorMzStr = isolationWindow["isolation window target m/z"];
        float precursorMz = 0;
        if (string2float(precursorMzStr) > 0)
            precursorMz = string2float(precursorMzStr);

        string precursorIsolationStrLower =
            isolationWindow["isolation window lower offset"];
        string precursorIsolationStrUpper =
            isolationWindow["isolation window upper offset"];

        float precursorIsolationWindow = 0.0f;
        if (string2float(precursorIsolationStrLower) > 0.0f)
            precursorIsolationWindow +=
                string2float(precursorIsolationStrLower);
        if (string2float(precursorIsolationStrUpper) > 0.0f)
            precursorIsolationWindow +=
                string2float(precursorIsolationStrUpper);
        if (precursorIsolationWindow <= 0.0f)
            precursorIsolationWindow = 1.0f;

        string productMzStr =
            spectrum.first_element_by_path("product/isolationWindow/cvParam")
                .attribute("value")
                .value();
        float productMz = 0;
        if (string2float(productMzStr) > 0)
            productMz = string2float(productMzStr);

        xml_node binaryDataArrayList = spectrum.child("binaryDataArrayList");
        if (!binaryDataArrayList or binaryDataArrayList.empty())
            continue;

        for (xml_node binaryDataArray =
                 binaryDataArrayList.child("binaryDataArray");
             binaryDataArray;
             binaryDataArray =
                 binaryDataArray.next_sibling("binaryDataArray")) {
            if (!binaryDataArray or binaryDataArray.empty())
                continue;

            map<string, string> attr = mzML_cvParams(binaryDataArray);

            int precision = 64;
            if (attr.count("32-bit float"))
                precision = 32;

            bool decompress = false;
            if(attr.count("zlib compression"))
                decompress=true;

            string binaryDataStr =
                binaryDataArray.child("binary").child_value();
            if (!binaryDataStr.empty()) {
                vector<float> binaryData = base64::decodeBase64(binaryDataStr,
                                                                 precision / 8,
                                                                 false,
                                                                 decompress);
                if (attr.count("m/z array")) {
                    mzVector = binaryData;
                }
                if (attr.count("intensity array")) {
                    intsVector = binaryData;
                }
            }
        }

        Scan* scan =
            new Scan(this, scannum++, mslevel, rt, precursorMz, scanpolarity);
        scan->isolationWindow = precursorIsolationWindow;
        scan->productMz = productMz;
        scan->filterLine = spectrumId;
        scan->intensity = intsVector;
        scan->mz = mzVector;
        addScan(scan);
    }
}

map<string, string> mzSample::mzML_cvParams(xml_node node)
{
    map<string, string> attr;
    if (!node || node.empty())
        return attr;
    for (xml_node cv = node.child("cvParam"); cv;
         cv = cv.next_sibling("cvParam")) {
        string name = cv.attribute("name").value();
        string value = cv.attribute("value").value();
        if (name == "scan start time") {
            string unit = cv.attribute("unitName").value();
            name += " " + unit;
        }
        attr[name] = value;
        // cerr << name << "->" << value << endl;
    }
    return (attr);
}

void mzSample::parseMzData(const char* filename)
{
    xml_document doc;

    const unsigned int parse_options = parse_minimal;

    pugi::xml_parse_result parseResult = doc.load_file(filename, parse_options);
    if (parseResult.status != pugi::xml_parse_status::status_ok) {
        throw(MavenException(ErrorMsg::ParsemzData));
    }

    // Get a spectrumstore node
    xml_node spectrumstore = doc.first_child().child("spectrumList");

    // Iterate through spectrums
    int scannum = 0;

    for (xml_node spectrum = spectrumstore.child("spectrum"); spectrum;
         spectrum = spectrum.next_sibling("spectrum")) {
        scannum++;
        float rt = 0;
        float precursorMz = 0;
        char scanpolarity = 0;  // default case

        xml_node spectrumInstrument = spectrum.first_element_by_path(
            "spectrumDesc/spectrumSettings/spectrumInstrument");
        int mslevel = spectrumInstrument.attribute("msLevel").as_int();
        // cerr << mslevel << " " << spectrum.attribute("msLevel").value() <<
        // endl;

        for (xml_node cvParam = spectrumInstrument.child("cvParam"); cvParam;
             cvParam = cvParam.next_sibling("cvParam")) {
            //	cout << "cvParam=" << cvParam.attribute("name").value() << endl;
            //
            if (strncasecmp(
                    cvParam.attribute("name").value(), "TimeInMinutes", 10)
                == 0) {
                rt = cvParam.attribute("value").as_float();
                // cout << "rt=" << rt << endl;
            }

            if (strncasecmp(
                    cvParam.attribute("name").value(), "time in seconds", 10)
                == 0) {
                rt = cvParam.attribute("value").as_float() / 60;
                // cout << "rt=" << rt << endl;
            }

            if (strncasecmp(cvParam.attribute("name").value(), "polarity", 5)
                == 0) {
                if (cvParam.attribute("value").value()[0] == 'p'
                    || cvParam.attribute("value").value()[0] == 'P') {
                    scanpolarity = +1;
                } else {
                    scanpolarity = -1;
                }
            }
        }

        // cout <<
        // spectrum.first_element_by_path("spectrumDesc/spectrumSettings/spectrumInstrument").child_value()
        // << endl
        if (mslevel <= 0)
            mslevel = 1;
        Scan* scan =
            new Scan(this, scannum, mslevel, rt, precursorMz, scanpolarity);
        addScan(scan);

        int precision1 = spectrum.child("intenArrayBinary")
                             .child("data")
                             .attribute("precision")
                             .as_int();
        string b64intensity =
            spectrum.child("intenArrayBinary").child("data").child_value();
        scan->intensity =
            base64::decodeBase64(b64intensity, precision1 / 8, false, false);

        // cout << "mz" << endl;
        int precision2 = spectrum.child("mzArrayBinary")
                             .child("data")
                             .attribute("precision")
                             .as_int();
        string b64mz =
            spectrum.child("mzArrayBinary").child("data").child_value();
        scan->mz = base64::decodeBase64(b64mz, precision2 / 8, false, false);

        // cout << "spectrum " << spectrum.attribute("title").value() << endl;
    }
}

xml_node mzSample::getmzXMLSpectrumData(xml_document& doc, const char* filename)
{
    // parse_minimal has all options turned off. This option mask means
    // that pugixml does not add declaration nodes, document type declaration
    // nodes, PI nodes, CDATA sections and comments to the resulting tree and
    // does not perform any conversion for input data, so theoretically it is
    // the fastest mode
    bool loadok = doc.load_file(filename, pugi::parse_minimal);

    // Checking if the file can be loaded if it can be loaded
    if (!loadok) {
        cerr << "Failed to load " << filename << endl;
        // returning an empty node
        return xml_node();
    }

    //"msRun" is the first child of the parent child "mzXML"
    xml_node spectrumstore = doc.first_child().child("msRun");

    // Checking if the node is empty which means that msRun is not there
    // but then it might have scan which contain the data
    if (spectrumstore.empty()) {
        // Getting the first child named "scan"
        xml_node scan = doc.first_child().child("scan");
        // If scan is not present then there is no data
        // which means that there is no information in the
        // mzXML file
        if (!scan.empty()) {
            spectrumstore = doc.first_child();
        } else {
            // if the above condition is not satisfied return a empty
            // spectrumstore
            cerr << "parseMzXML: can't find <msRun> or <scan> section" << endl;
            return xml_node();
        }
    }
    return spectrumstore;
}

void mzSample::setInstrumentSettigs(xml_document& doc, xml_node spectrumstore)
{
    // Getting the instrument related information
    xml_node msInstrument = spectrumstore.child("msInstrument");
    if (!msInstrument.empty()) {
        xml_node msManufacturer = msInstrument.child("msManufacturer");
        xml_node msModel = msInstrument.child("msModel");
        xml_node msIonisation = msInstrument.child("msIonisation");
        xml_node msMassAnalyzer = msInstrument.child("msMassAnalyzer");
        xml_node msDetector = msInstrument.child("msDetector");
        instrumentInfo["msManufacturer"] =
            msManufacturer.attribute("value").value();
        instrumentInfo["msModel"] = msModel.attribute("value").value();
        instrumentInfo["msIonisation"] =
            msIonisation.attribute("value").value();
        instrumentInfo["msMassAnalyzer"] =
            msMassAnalyzer.attribute("value").value();
        instrumentInfo["msDetector"] = msDetector.attribute("value").value();
    }
}

void mzSample::parseMzXMLData(const xml_node& spectrumstore)
{
    // Iterate through spectrums
    int scannum = 0;

    for (xml_node scan = spectrumstore.child("scan"); scan;
         scan = scan.next_sibling("scan")) {
        scannum++;
        if (strncasecmp(scan.name(), "scan", 4) == 0) {
            parseMzXMLScan(scan, scannum);
        }

        for (xml_node child = scan.first_child(); child;
             child = child.next_sibling()) {
            scannum++;
            if (strncasecmp(child.name(), "scan", 4) == 0) {
                parseMzXMLScan(child, scannum);
            }
        }
    }
}

void mzSample::parseMzXML(const char* filename)
{
    xml_document doc;

    xml_node spectrumstore = getmzXMLSpectrumData(doc, filename);

    if (!spectrumstore.empty()) {
        // Setting the instrument related information
        setInstrumentSettigs(doc, spectrumstore);
        // parse mzXML information from the scan
        parseMzXMLData(spectrumstore);
    } else
        throw MavenException(ErrorMsg::ParsemzXml);
}

/**
 * calculate the RT in minutes
 **/
float mzSample::parseRTFromMzXML(xml_attribute& attr)
{
    float rt = 0.0;
    if (strncasecmp(attr.value(), "PT", 2) == 0) {
        rt = string2float(string(attr.value() + 2));
    } else if (strncasecmp(attr.value(), "P", 1) == 0) {
        rt = string2float(string(attr.value() + 1));
    } else {
        rt = string2float(attr.value());
    }
    rt /= 60.0;
    return rt;
}

int mzSample::parsePolarityFromMzXML(xml_attribute& attr)
{
    char p = attr.value()[0];
    // For polarity 0 is the default value
    int scanpolarity = 0;
    switch (p) {
    case '+':
        scanpolarity = 1;
        break;
    case '-':
        scanpolarity = -1;
        break;
    default:
        scanpolarity = 0;
        break;
    }

    return scanpolarity;
}

int mzSample::getPolarityFromfilterLine(string filterLine)
{
    int MIN_FILTER_LINE_LENGTH = 13;
    int scanpolarity = 0;

    if (filterLine.size() > MIN_FILTER_LINE_LENGTH) {
        if (filterLine.find(" + ") != string::npos) {
            scanpolarity = 1;
        } else {
            scanpolarity = -1;
        }
    }

    return scanpolarity;
}

vector<float> mzSample::parsePeaksFromMzXML(const xml_node& scan)
{
    xml_node peaks = scan.child("peaks");
    vector<float> mzint;

    if (!peaks.empty()) {
        string b64String(peaks.child_value());

        // no m/z intensity values
        if (b64String.empty())
            return mzint;

        // if the data is been compressed in zlib format this part will
        // take care.
        bool decompress = false;
        if (strncasecmp(peaks.attribute("compressionType").value(), "zlib", 4) == 0) {
            decompress = true;
        }

        bool networkorder = false;  // naman The scope of the variable
                                    // 'networkorder' can be reduced.

        if (peaks.attribute("byteOrder").empty()
            || strncasecmp(peaks.attribute("byteOrder").value(), "network", 5)
                   == 0) {
            networkorder = true;
        }

        int precision =
            32;  // naman The scope of the variable 'precision' can be reduced.

        if (!peaks.attribute("precision").empty()) {
            precision = peaks.attribute("precision").as_int();
        }

        // cerr << "new scan=" << scannum << " msL=" << msLevel << " rt=" << rt
        // << " precMz=" << precursorMz << " polar=" << scanpolarity
        //    << " prec=" << precision << endl;

        mzint = base64::decodeBase64(
            b64String, precision / 8, networkorder, decompress);

        return mzint;
    }

    return mzint;
}

void mzSample::populateMzAndIntensity(const vector<float>& mzint, Scan* _scan)
{
    int j = 0, count = 0, size = mzint.size() / 2;

    // sizing the mz variable and the intensity variable
    _scan->mz.resize(size);
    _scan->intensity.resize(size);

    for (int i = 0; i < size; i++) {
        float mzValue = mzint[j++];
        float intensityValue = mzint[j++];
        // cerr << mzValue << " " << intensityValue << endl;
        if (mzValue > 0 && intensityValue > 0) {
            _scan->mz[count] = mzValue;
            _scan->intensity[count] = intensityValue;
            count++;
        }
    }

    _scan->mz.resize(count);
    _scan->intensity.resize(count);
    _scan->mz.shrink_to_fit();
    _scan->intensity.shrink_to_fit();
}

void mzSample::populateFilterline(const string& filterLine, Scan* _scan)
{
    if (!filterLine.empty())
        _scan->filterLine = filterLine;

    // TODO: why is this logic like this is
    if (filterLine.empty() && _scan->precursorMz > 0) {
        _scan->filterLine = _scan->scanType + ":"
                            + float2string(_scan->precursorMz, 4) + " ["
                            + float2string(_scan->productMz, 4) + "]";
    }
}

void mzSample::parseMzXMLScan(const xml_node& scan, const int& scannum)
{
    float rt = 0.0, precursorMz = 0.0f, productMz = 0, collisionEnergy = 0;
    int scanpolarity = 0, msLevel = 1;
    string filterLine, scanType;
    vector<float> mzint;

    for (xml_attribute attr = scan.first_attribute(); attr;
         attr = attr.next_attribute()) {
        if (strncasecmp(attr.name(), "retentionTime", 10) == 0) {
            rt = parseRTFromMzXML(attr);
        }

        if (strncasecmp(attr.name(), "polarity", 5) == 0) {
            scanpolarity = parsePolarityFromMzXML(attr);
        }

        if (strncasecmp(attr.name(), "filterLine", 9) == 0) {
            filterLine = attr.value();
        }

        if (strncasecmp(attr.name(), "mslevel", 5) == 0) {
            msLevel = string2integer(attr.value());
            // If ms Level is less than Zero then its hardcoded
            // to one
            if (msLevel <= 0)
                msLevel = 1;
        }

        if (strncasecmp(attr.name(), "basePeakMz", 9) == 0) {
            productMz = string2float(attr.value());
        }

        if (strncasecmp(attr.name(), "collisionEnergy", 12) == 0) {
            collisionEnergy = string2float(attr.value());
        }

        if (strncasecmp(attr.name(), "scanType", 8) == 0) {
            scanType = attr.value();
        }
    }

    // To get the polarity from filterline if polarity turns out to be neurtal
    if (scanpolarity == 0) {
        scanpolarity = getPolarityFromfilterLine(filterLine);
    }

    // no m/z intensity values
    mzint = parsePeaksFromMzXML(scan);
    if (mzint.empty()) {
        return;
    }

    Scan* _scan =
        new Scan(this, scannum, msLevel, rt, precursorMz, scanpolarity);

    xml_node precursor = scan.child("precursorMz");
    if (precursor) {
        _scan->precursorMz = string2float(scan.child_value("precursorMz"));
        _scan->isolationWindow = 1.0f;

        for (xml_attribute attr = precursor.first_attribute(); attr;
             attr = attr.next_attribute()) {
            if (strncasecmp(attr.name(), "precursorIntensity", 15) == 0)
                _scan->precursorIntensity = string2float(attr.value());
            else if (strncasecmp(attr.name(), "precursorCharge", 15) == 0)
                _scan->precursorCharge = string2integer(attr.value());
            else if (strncasecmp(attr.name(), "precursorScanNum", 15) == 0)
                _scan->precursorScanNum = string2integer(attr.value());
            else if (strncasecmp(attr.name(), "windowWideness", 13) == 0)
                _scan->isolationWindow = string2float(attr.value());
        }
    }

    if (!scanType.empty())
        _scan->scanType = scanType;

    _scan->productMz = productMz;

    _scan->collisionEnergy = collisionEnergy;

    populateMzAndIntensity(mzint, _scan);

    populateFilterline(filterLine, _scan);

    addScan(_scan);
}

void mzSample::summary()
{
    // void mzSample::summary() const {
    cerr << "Num of obs:" << this->scans.size() << endl;
    cerr << "Rt range:" << this->minRt << " " << this->maxRt << endl;
    cerr << "Mz range:" << this->minMz << " " << this->maxMz << endl;
}

void mzSample::calculateMzRtRange()
{
    if (scans.size() == 0) {
        cerr << "sample has no data" << endl;
        return;
    }

    minRt = scans[0]->rt;
    maxRt = scans[scans.size() - 1]->rt;
    minMz = FLT_MAX;
    maxMz = 0;
    minIntensity = FLT_MAX;
    maxIntensity = 0;
    totalIntensity = 0;
    int nobs = 0;

    unsigned int numOfScans = scans.size();
    for (unsigned int j = 0; j < numOfScans; j++) {
        Scan* currentScan = scans[j];
        unsigned int mzSize = currentScan->mz.size();
        for (unsigned int i = 0; i < mzSize; i++) {
            float intensity = currentScan->intensity[i];
            float mz = currentScan->mz[i];
            totalIntensity += intensity;
            if (mz < minMz && mz > 0)
                minMz = mz; //sanity check must be greater > 0
            if (mz > maxMz && mz < 1e9)
                maxMz = mz; //sanity check m/z over a billion
            if (intensity < minIntensity)
                minIntensity = intensity;
            if (intensity > maxIntensity)
                maxIntensity = intensity;
            nobs++;
        }
    }
    //sanity check
    if (minRt <= 0)
        minRt = 0;
    if (maxRt >= 1e4)
        maxRt = 1e4;
}

float mzSample::getMaxRt(const vector<mzSample*>& samples)
{
    // TODO naman unused function
    float maxRt = 0;
    for (unsigned int i = 0; i < samples.size(); i++)
        if (samples[i]->maxRt > maxRt)
            maxRt = samples[i]->maxRt;

    return maxRt;
}

float mzSample::getAverageFullScanTime()
{
    // float mzSample::getAverageFullScanTime() const {
    float s = 0;
    int n = 0;
    Scan* lscan = NULL;
    Scan* tscan = NULL;
    if (scans.size() == 0)
        return 0;

    for (unsigned int i = 1; i < scans.size(); i++) {
        if (scans[i]->mslevel == 1) {
            tscan = scans[i];
            if (lscan) {
                s += tscan->rt - lscan->rt;
                n++;
            }
            lscan = tscan;
        }
    }
    if (n > 0)
        return s / n;
    return 0;
}

void mzSample::enumerateSRMScans()
{
    srmScans.clear();
    for (unsigned int i = 0; i < scans.size(); i++) {
        if (scans[i]->filterLine.length() > 0) {
            srmScans[scans[i]->filterLine].push_back(i);
        }
    }
}

Scan* mzSample::getScan(unsigned int scanNum)
{
    if (scanNum >= scans.size())
        scanNum = scans.size() - 1;
    if (scanNum < scans.size()) {
        return (scans[scanNum]);
    } else {
        cerr << "Warning bad scan number " << scanNum << endl;
        return NULL;
    }
}

EIC* mzSample::getEIC(float precursorMz,
                      float collisionEnergy,
                      float productMz,
                      int eicType,
                      string filterline,
                      float amuQ1 = 0.5,
                      float amuQ3 = 0.5)
{
    // lambda: constructs a fresh EIC object with some initializations
    auto newEic = [this]() {
        EIC* e = new EIC();
        e->sampleName = sampleName;
        e->sample = this;
        e->totalIntensity = 0;
        e->maxIntensity = 0;
        e->mzmin = 0;
        e->mzmax = 0;
        return e;
    };

    map<string, EIC*> filterlineEicMap;
    map<string, vector<float>> filterlinePrecursorDeltas;
    map<string, vector<float>> filterlineProductDeltas;
    for (unsigned int i = 0; i < scans.size(); i++) {
        Scan* scan = scans[i];
        if (filterline != "" && scan->filterLine != filterline)
            continue;
        if (scan->mslevel != 2)
            continue;
        if (precursorMz != 0.0f && abs(scan->precursorMz - precursorMz) > amuQ1)
            continue;
        if (collisionEnergy != 0.0f
            && scan->collisionEnergy != 0.0f
            && abs(scan->collisionEnergy - collisionEnergy) > 0.5) {
            continue;
        }

        // NOTE: we check for product m/z later while iterating over individual
        // observations because, for mzXML files, the scans may actually contain
        // multiple m/z and intensity points (just like a spectra should),
        // unlike chromatogram-extracted scans from mzML files (which contain
        // a single mz-intensity observation). This way, we can maintain
        // compatibility with both the formats.

        float eicMz = 0;
        float eicIntensity = -0.01;
        string eicFilterline = "";

        switch ((EIC::EicType)eicType) {
        case EIC::MAX: {
            for (unsigned int k = 0; k < scan->nobs(); k++) {
                if (productMz != 0.0f && abs(scan->mz[k] - productMz) > amuQ3)
                    continue;

                if (scan->intensity[k] > eicIntensity) {
                    eicIntensity = scan->intensity[k];
                    eicMz = scan->mz[k];
                    eicFilterline = scan->filterLine;
                }
            }
            break;
        }

        // calculate the weighted average(with intensities as weights)
        // while finding the eicMz for the whole EIC.
        case EIC::SUM: {
            double sumMz = 0.0;
            double sumIntensity = 0.0;
            vector<float> mzValues;
            for (unsigned int k = 0; k < scan->nobs(); k++) {
                if (productMz != 0.0f && abs(scan->mz[k] - productMz) > amuQ3)
                    continue;

                double intensity = static_cast<double>(scan->intensity[k]);
                sumIntensity += intensity;
                sumMz += static_cast<double>(scan->mz[k]) * intensity;
                mzValues.push_back(scan->mz[k]);
            }
            if (sumIntensity != 0.0) {
                eicMz = static_cast<float>(sumMz / sumIntensity);
                eicIntensity = static_cast<float>(sumIntensity);
                eicFilterline = scan->filterLine;
            } else {
                eicMz = accumulate(begin(mzValues),
                                   end(mzValues),
                                   0.0) / mzValues.size();
                eicIntensity = 0.0f;
                eicFilterline = scan->filterLine;
            }
            break;
        }

        default: {
            for (unsigned int k = 0; k < scan->nobs(); k++) {
                if (productMz != 0.0f && abs(scan->mz[k] - productMz) > amuQ3)
                    continue;

                if (scan->intensity[k] > eicIntensity) {
                    eicIntensity = scan->intensity[k];
                    eicMz = scan->mz[k];
                    eicFilterline = scan->filterLine;
                }
            }
            break;
        }
        }

        EIC* e = nullptr;
        vector<float> precursorDeltas;
        vector<float> productDeltas;
        if (filterlineEicMap.count(eicFilterline) > 0) {
            e = filterlineEicMap[eicFilterline];
            precursorDeltas = filterlinePrecursorDeltas[eicFilterline];
            productDeltas = filterlineProductDeltas[eicFilterline];
        } else {
            e = newEic();
            filterlineEicMap[eicFilterline] = e;
        }

        // Have to do this since we started with a default negative value,
        // which itself makes sure that zero observations are not ignored.
        if (eicIntensity < 0.0f)
            eicIntensity = 0.0f;

        // if rt is already present save the higher intensity for that rt
        // this can happen when there are multiple product m/z for the same
        // precursor
        if (!e->rt.empty() && e->rt.back() == scan->rt) {
            if (eicIntensity <= e->intensity.back())
                continue;
            else {
                // replace old values for the rt
                e->scannum.back() = scan->scannum;
                e->intensity.back() = eicIntensity;
                e->mz.back() = eicMz;
            }
        }
        // save values for new rt
        else {
            e->scannum.push_back(scan->scannum);
            e->rt.push_back(scan->rt);
            e->intensity.push_back(eicIntensity);
            e->mz.push_back(eicMz);
        }
        e->totalIntensity += eicIntensity;
        if (eicIntensity > e->maxIntensity) {
            e->maxIntensity = eicIntensity;
            e->rtAtMaxIntensity = scan->rt;
            e->mzAtMaxIntensity = eicMz;
        }

        precursorDeltas.push_back(abs(scan->precursorMz - precursorMz));
        filterlinePrecursorDeltas[eicFilterline] = precursorDeltas;
        productDeltas.push_back(abs(eicMz - productMz));
        filterlineProductDeltas[eicFilterline] = productDeltas;
    }

    // lambda: make some adjustments to the generated EIC before returning
    auto adjustEic = [this](EIC* eic) {
        if (eic->rt.size() > 0) {
            eic->rtmin = eic->rt[0];
            eic->rtmax = eic->rt[eic->size() - 1];
        }

        float scale = getNormalizationConstant();
        if (scale != 1.0) {
            for (size_t j = 0; j < eic->size(); j++) {
                eic->intensity[j] *= scale;
            }
        }
    };

    if (filterlineEicMap.size() == 1) {
        // just one EIC, must be the right one
        EIC* eic = (*begin(filterlineEicMap)).second;
        adjustEic(eic);
        return eic;
    }

    // In case there were multiple chromatograms for the same precursor-product
    // pair, we check which of them is the closer one and return that one. For
    // SRM experiments, the chromatogram ID is stored in the filterline string,
    // which can be used to differentiate between product chromatograms.
    EIC* eic = nullptr;
    float bestDeltaScore = numeric_limits<float>::max();
    for (auto& elem : filterlineEicMap) {
        string filterline = elem.first;
        EIC* eicAlt = elem.second;
        if (eicAlt->size() == 0 || eicAlt->maxIntensity == 0.0f)
            continue;

        auto precursorDeltas = filterlinePrecursorDeltas.at(filterline);
        auto productDeltas = filterlineProductDeltas.at(filterline);
        if (precursorDeltas.size() > 0
            && productDeltas.size() > 0) {
            auto meanPrecursorDelta = accumulate(begin(precursorDeltas),
                                                 end(precursorDeltas),
                                                 0.0) / precursorDeltas.size();
            auto meanProductDelta = accumulate(begin(precursorDeltas),
                                               end(precursorDeltas),
                                               0.0) / productDeltas.size();
            float deltaScore = hypotf(meanPrecursorDelta, meanProductDelta);
            if (deltaScore < bestDeltaScore) {
                eic = eicAlt;
                bestDeltaScore = deltaScore;
            }
        }
    }

    // Now a separate loop to clear the unused EICs
    for (auto& elem : filterlineEicMap) {
        EIC* e = elem.second;
        if (e != eic)
            delete e;
    }

    if (eic == nullptr)
        eic = newEic();

    adjustEic(eic);
    return eic;
}

EIC* mzSample::getEIC(string srm, int eicType)
{
    EIC* e = new EIC();
    e->sampleName = sampleName;
    e->sample = this;
    e->totalIntensity = 0;
    e->maxIntensity = 0;
    e->mzmin = 0;
    e->mzmax = 0;

    // if (srmScans.size() == 0 )

    // naman Checking for ‘List’ emptiness might be inefficient. Using
    // List.empty() instead of List.size() can be faster. List.size() can take
    // linear time but List.empty() is guaranteed to take constant time. src:
    // https://kmdarshan.wordpress.com/2011/08/15/static-analysis-of-cc-code-using-cppcheck/
    if (srmScans.empty()) {
        enumerateSRMScans();
    }

    if (srmScans.count(srm) > 0) {
        vector<int> srmscans = srmScans[srm];
        for (unsigned int i = 0; i < srmscans.size(); i++) {
            Scan* scan = scans[srmscans[i]];
            float eicMz = 0;
            float eicIntensity = -0.01f;

            switch ((EIC::EicType)eicType) {
            case EIC::MAX: {
                for (unsigned int k = 0; k < scan->nobs(); k++) {
                    if (scan->intensity[k] > eicIntensity) {
                        eicIntensity = scan->intensity[k];
                        eicMz = scan->mz[k];
                    }
                }
                break;
            }

            // calculate the weighted average(with intensities as weights)
            // while finding the eicMz for the whole EIC.
            case EIC::SUM: {
                double sumMz = 0.0;
                double sumIntensity = 0.0;
                vector<float> mzValues;
                for (unsigned int k = 0; k < scan->nobs(); k++) {
                    double intensity = static_cast<double>(scan->intensity[k]);
                    sumIntensity += intensity;
                    sumMz += static_cast<double>(scan->mz[k]) * intensity;
                    mzValues.push_back(scan->mz[k]);
                }
                if (sumIntensity != 0.0) {
                    eicMz = static_cast<float>(sumMz / sumIntensity);
                    eicIntensity = static_cast<float>(sumIntensity);
                } else {
                    eicMz = accumulate(begin(mzValues),
                                       end(mzValues),
                                       0.0) / mzValues.size();
                    eicIntensity = 0.0f;
                }
                break;
            }

            default: {
                for (unsigned int k = 0; k < scan->nobs(); k++) {
                    if (scan->intensity[k] > eicIntensity) {
                        eicIntensity = scan->intensity[k];
                        eicMz = scan->mz[k];
                    }
                }
                break;
            }
            }

            // Have to do this since we started with a default negative value,
            // which itself makes sure that zero observations are not ignored.
            if (eicIntensity < 0.0f)
                eicIntensity = 0.0f;

            e->scannum.push_back(scan->scannum);
            e->rt.push_back(scan->rt);
            e->intensity.push_back(eicIntensity);
            e->mz.push_back(eicMz);
            e->totalIntensity += eicIntensity;

            if (eicIntensity > e->maxIntensity) {
                e->maxIntensity = eicIntensity;
                e->rtAtMaxIntensity = scan->rt;
                e->mzAtMaxIntensity = eicMz;
            }
        }
    }

    if (e->rt.size() > 0) {
        e->rtmin = e->rt[0];
        e->rtmax = e->rt[e->size() - 1];
    }

    float scale = getNormalizationConstant();
    if (scale != 1.0)
        for (unsigned int j = 0; j < e->size(); j++) {
            e->intensity[j] *= scale;
        }
    // if (e->size() == 0)
    //     cerr << "getEIC(SRM STRING): is empty" << srm << endl;
    // std::cerr << "getEIC: srm" << srm << " " << e->intensity.size() << endl;

    return e;
}

/**
 * mzSample::getEIC This is the function which computes the EICs
 * @param  mzmin   mz min that is for a given slice of interest
 * @param  mzmax   mz max that is for a given slice of interest
 * @param  rtmin   rt min that is for a given slice of interest
 * @param  rtmax   rt max that is for a given slice of interest
 * @param  mslevel This is to select the MS level this can be normal MS
 * MS/MS
 * @return         [description]
 */
EIC* mzSample::getEIC(float mzmin,
                      float mzmax,
                      float rtmin,
                      float rtmax,
                      int mslevel,
                      int eicType,
                      string filterline)
{
    // Adjusting the Retension Time so that it matches with the sample
    // retension time
    if (rtmin < this->minRt)
        rtmin = this->minRt;
    if (rtmax > this->maxRt && this->maxRt > rtmin)
        rtmax = this->maxRt;
    if (mzmin < this->minMz)
        mzmin = this->minMz;
    if (mzmax > this->maxMz && this->maxMz > mzmin)
        mzmax = this->maxMz;

    EIC* e = new EIC();
    e->sampleName = sampleName;
    e->sample = this;
    e->mzmin = mzmin;
    e->mzmax = mzmax;
    e->totalIntensity = 0;
    e->maxIntensity = 0;

    int scanCount = scans.size();
    if (scanCount == 0)
        return e;

    if (mzmin < minMz && mzmax < maxMz) {
        cerr << "getEIC(): mzmin and mzmax are out of range" << endl;
        return e;
    }

    bool success = e->makeEICSlice(
        this, mzmin, mzmax, rtmin, rtmax, mslevel, eicType, filterline);

    if (!success) {
        return e;
    }

    e->getRTMinMaxPerScan();

    // scale EIC by normalization constant
    float scale = getNormalizationConstant();
    e->normalizeIntensityPerScan(scale);

    // if (e->size() == 0)
    //     cerr << "getEIC(mzrange,rtrange,mslevel): is empty" << mzmin << " "
    //          << mzmax << " " << rtmin << " " << rtmax << endl;

    return (e);
}

EIC* mzSample::getTIC(float rtmin, float rtmax, int mslevel)
{
    // TODO naman unused function

    // ajust EIC retention time window to match sample retentention times
    if (rtmin < this->minRt)
        rtmin = this->minRt;
    if (rtmax > this->maxRt)
        rtmax = this->maxRt;

    // cerr << "getEIC()" << setprecision(7) << mzmin << " " << mzmax << " " <<
    // rtmin << " " << rtmax << endl;

    EIC* e = new EIC();
    e->sampleName = sampleName;
    e->sample = this;
    e->mzmin = 0;
    e->mzmax = 0;
    e->totalIntensity = 0;
    e->maxIntensity = 0;

    int scanCount = scans.size();
    if (scanCount == 0)
        return e;

    for (int i = 0; i < scanCount; i++) {
        if (scans[i]->mslevel == mslevel) {
            Scan* scan = scans[i];
            float y = scan->totalIntensity();
            e->mz.push_back(0);
            e->scannum.push_back(i);
            e->rt.push_back(scan->rt);
            e->intensity.push_back(y);
            e->totalIntensity += y;
            if (y > e->maxIntensity) {
                e->maxIntensity = y;
                e->rtAtMaxIntensity = scan->rt;
                e->mzAtMaxIntensity = 0;
            }
        }
    }
    if (e->rt.size() > 0) {
        e->rtmin = e->rt[0];
        e->rtmax = e->rt[e->size() - 1];
    }
    return (e);
}

// TODO: Sahil Added this function because of merging of eicwidget
EIC* mzSample::getBIC(float rtmin, float rtmax, int mslevel)
{
    // ajust EIC retention time window to match sample retentention times
    if (rtmin < this->minRt)
        rtmin = this->minRt;
    if (rtmax > this->maxRt)
        rtmax = this->maxRt;

    // cerr << "getEIC()" << setprecision(7) << mzmin << " " << mzmax << " " <<
    // rtmin << " " << rtmax << endl;

    EIC* e = new EIC();
    e->sampleName = sampleName;
    e->sample = this;
    e->mzmin = 0;
    e->mzmax = 0;
    e->totalIntensity = 0;
    e->maxIntensity = 0;

    int scanCount = scans.size();
    if (scanCount == 0)
        return e;

    for (int i = 0; i < scanCount; i++) {
        if (scans[i]->mslevel == mslevel) {
            Scan* scan = scans[i];
            float maxMz = 0;
            float maxIntensity = 0;
            for (unsigned int i = 0; i < scan->intensity.size(); i++) {
                if (scan->intensity[i] > maxIntensity) {
                    maxIntensity = scan->intensity[i];
                    maxMz = scan->mz[i];
                }
            }
            e->mz.push_back(maxMz);
            e->scannum.push_back(i);
            e->rt.push_back(scan->rt);
            e->intensity.push_back(maxIntensity);
            e->totalIntensity += maxIntensity;
            if (maxIntensity > e->maxIntensity) {
                e->maxIntensity = maxIntensity;
                e->rtAtMaxIntensity = scan->rt;
                e->mzAtMaxIntensity = maxMz;
            }
        }
    }
    if (e->rt.size() > 0) {
        e->rtmin = e->rt[0];
        e->rtmax = e->rt[e->size() - 1];
    }
    return (e);
}

// compute correlation between two mzs within some retention time window
float mzSample::correlation(float mz1,
                            float mz2,
                            MassCutoff* massCutoff,
                            float rt1,
                            float rt2,
                            int eicType,
                            string filterline)
{
    float ppm1 = massCutoff->massCutoffValue(mz1);
    float ppm2 = massCutoff->massCutoffValue(mz2);
    int msLevel = 1;
    return correlation(mz1 - ppm1,
                       mz1 + ppm1,
                       mz2 - ppm2,
                       mz2 + ppm2,
                       rt1,
                       rt2,
                       msLevel,
                       eicType,
                       filterline);
}

float mzSample::correlation(float mzMin1,
                            float mzMax1,
                            float mzMin2,
                            float mzMax2,
                            float rtMin,
                            float rtMax,
                            int msLevel,
                            int eicType,
                            string filterline)
{
    EIC* e1 = getEIC(mzMin1,
                     mzMax1,
                     rtMin,
                     rtMax,
                     msLevel,
                     eicType,
                     filterline);
    EIC* e2 = getEIC(mzMin2,
                     mzMax2,
                     rtMin,
                     rtMax,
                     msLevel,
                     eicType,
                     filterline);
    float correlation = mzUtils::correlation(e1->intensity, e2->intensity);
    delete (e1);
    delete (e2);
    return correlation;
}

// TODO: is_verbose not being used
int mzSample::parseCDF(const char* filename, int is_verbose)
{
#ifdef CDFPARSER
    int cdf = 0;
    int errflag = 0;
    long nscans = 0;
    long ninst = 0;

    extern int ncopts; /* from "netcdf.h" */
    ncopts = 0;

    static MS_Admin_Data admin_data;
    static MS_Sample_Data sample_data;
    static MS_Test_Data test_data;
    // static MS_Instrument_Data inst_data; //naman unused
    static MS_Raw_Data_Global raw_global_data;
    static MS_Raw_Per_Scan raw_data;
    // double mass_pt=0;
    // double inty_pt=0;
    // double inty=0;

    cdf = ms_open_read(const_cast<char*>(filename));
    if (-1 == cdf) {
        fprintf(stderr, "\nopen_cdf_ms: ms_open_read failed!");
        return 0;
    }

    /* Initialize attribute data structures */

    ms_init_global(
        FALSE, &admin_data, &sample_data, &test_data, &raw_global_data);

    /* Read global information */

    if (MS_ERROR
        == ms_read_global(
               cdf, &admin_data, &sample_data, &test_data, &raw_global_data)) {
        fprintf(stderr, "\nopen_cdf_ms: ms_read_global failed!");
        ms_init_global(
            TRUE, &admin_data, &sample_data, &test_data, &raw_global_data);
        ms_close(cdf);
        return 0;
    }

    nscans = raw_global_data.nscans;

    switch (admin_data.experiment_type) {
    case 0:
        printf("Centroid");
        break;
    case 1:
        printf("Continuum");
        break;
    case 2:
        printf("Library");
        break;
    default:
        printf("Unknown: '%d'", admin_data.experiment_type);
        break;
    }

    printf("\n\n-- Instrument Information --");
    ninst = admin_data.number_instrument_components;
    printf("\nNumber_inst_comp\t%ld", ninst);

    printf("\n\n-- Raw Data Information --");
    printf("\nNumber of scans\t\t%ld", nscans);

    printf("\nMass Range\t\t%.2f > %.2f",
           raw_global_data.mass_axis_global_min,
           raw_global_data.mass_axis_global_max);

    printf("\nInty Range\t\t%.2f > %.2f",
           raw_global_data.intensity_axis_global_min,
           raw_global_data.intensity_axis_global_max);

    printf("\nTime Range\t\t%.2f > %.2f",
           raw_global_data.time_axis_global_min,
           raw_global_data.time_axis_global_max);

    printf("\nActual Run Time\t\t%.2f (%.2f min)",
           raw_global_data.run_time,
           raw_global_data.run_time / 60.0);
    printf("\nComments \t\t%s", raw_global_data.comments);

    if (errflag) /* if error occurred, clean up and leave */
    {
        ms_init_global(
            TRUE, &admin_data, &sample_data, &test_data, &raw_global_data);
        ms_close(cdf);
        return 0;
    }

    /* Check to see if scale factors and offsets are set to "NULL"
        values; if so, correct them for use below */

    if ((int)MS_NULL_FLT == (int)raw_global_data.mass_factor)
        raw_global_data.mass_factor = 1.0;

    if ((int)MS_NULL_FLT == (int)raw_global_data.time_factor)
        raw_global_data.time_factor = 1.0;

    if ((int)MS_NULL_FLT == (int)raw_global_data.intensity_factor)
        raw_global_data.intensity_factor = 1.0;

    if ((int)MS_NULL_FLT == (int)raw_global_data.intensity_offset)
        raw_global_data.intensity_offset = 0.0;

    if ((raw_global_data.mass_axis_global_min < 0)
        || (raw_global_data.mass_axis_global_max < 0)) {
        /* this bug is frequently observed with files from HP/Agilent
         * ChemStation */
        fprintf(
            stderr,
            "\n*** WARNING: Negative mass reported! Use '-v' for details.\n\n");
    }

    for (int scan = 0; scan < nscans; scan++) {
        ms_init_per_scan(FALSE, &raw_data, NULL);
        raw_data.scan_no = (long)scan;
        double mass_pt, inty_pt = 0.0;
            /* init */  // naman The scope can be reduced.

        if (MS_ERROR
            == ms_read_per_scan(
                   cdf,
                   &raw_data,
                   NULL)) { /* free allocated memory before leaving */
            fprintf(
                stderr, "\nreadchro: ms_read_per_scan failed (scan %d)!", scan);
            ms_init_per_scan(TRUE, &raw_data, NULL);
            return 0;
        }

        if (!raw_data.points) { /* empty scan? */
            break;
        } else { /* there are data points */

            int polarity = 0;
            if (test_data.ionization_mode == polarity_plus)
                polarity = +1;
            else
                polarity = -1;

            Scan* myscan =
                new Scan(this,
                         raw_data.actual_scan_no,
                         test_data.scan_function - (int)resolution_proportional,
                         raw_data.scan_acq_time / 60,
                         0,
                         polarity);

            myscan->intensity.resize(raw_data.points);
            myscan->mz.resize(raw_data.points);

            if (admin_data.experiment_type == 0)
                myscan->centroided = true;
            else
                myscan->centroided = false;

            for (int i = 0; i < raw_data.points; i++) {
                switch (raw_global_data.mass_format) {
                case data_short:
                    mass_pt = (double)((short*)raw_data.masses)[i];
                    break;

                case data_long:
                    mass_pt = (double)((long*)raw_data.masses)[i];
                    break;

                case data_float:
                    mass_pt = (double)((float*)raw_data.masses)[i];
                    break;

                case data_double:
                    mass_pt = ((double*)raw_data.masses)[i];
                    break;
                }

                mass_pt *= raw_global_data.mass_factor;

                switch (raw_global_data.intensity_format) {
                case data_short:
                    inty_pt = (double)((short*)raw_data.intensities)[i];
                    break;

                case data_long:
                    inty_pt = (double)((long*)raw_data.intensities)[i];
                    break;

                case data_float:
                    inty_pt = (double)((float*)raw_data.intensities)[i];
                    break;

                case data_double:
                    inty_pt = (double)((double*)raw_data.intensities)[i];
                    break;
                }

                inty_pt = inty_pt * raw_global_data.intensity_factor
                          + raw_global_data.intensity_offset;
                // cerr << "mz/int" << mass_pt << " " << inty_pt << endl;
                myscan->intensity[i] = inty_pt;
                myscan->mz[i] = mass_pt;

                if (raw_data.flags > 0)
                    printf("\nWarning: There are flags in scan %ld (ignored).",
                           scan);
            } /* i loop */

            addScan(myscan);
        }

        ms_init_per_scan(TRUE, &raw_data, NULL);

    } /* scan loop */
#endif
    return 1;
}

Scan* mzSample::getAverageScan(float rtmin,
                               float rtmax,
                               int mslevel,
                               int polarity,
                               float sd)
{
    // TODO naman unused function

    float rt = rtmin + (rtmax - rtmin) / 2;
    int scanCount = 0;
    int scannum = 0;

    map<float, double> mz_intensity_map;
    map<float, double> mz_bin_map;
    map<float, int> mz_count;

    for (unsigned int s = 0; s < scans.size(); s++) {
        if (scans[s]->getPolarity() != polarity || scans[s]->mslevel != mslevel
            || scans[s]->rt < rtmin || scans[s]->rt > rtmax)
            continue;

        Scan* scan = scans[s];
        scanCount++;
        for (unsigned int i = 0; i < scan->mz.size(); i++) {
            float bin = FLOATROUND(scan->mz[i], sd);
            mz_intensity_map[bin] += ((double)scan->intensity[i]);
            mz_bin_map[bin] += ((double)(scan->intensity[i]) * (scan->mz[i]));
            mz_count[bin]++;
        }
    }

    Scan* avgScan =
        new Scan(this, scannum, mslevel, rt / scanCount, 0, polarity);

    map<float, double>::iterator itr;
    for (itr = mz_intensity_map.begin(); itr != mz_intensity_map.end(); ++itr) {
        float bin = (*itr).first;
        double totalIntensity = (*itr).second;
        double avgMz = mz_bin_map[bin] / totalIntensity;
        avgScan->mz.push_back((float)avgMz);
        avgScan->intensity.push_back((float)totalIntensity / mz_count[bin]);
    }
    // cout << "getAverageScan() from:" << from << " to:" << to << " scanCount:"
    // << scanCount << "scans. mzs=" << avgScan->nobs() << endl;
    return avgScan;
}

void mzSample::saveCurrentRetentionTimes()
{
    lastSavedRTs.resize(scans.size());

    for (unsigned int ii = 0; ii < scans.size(); ii++) {
        lastSavedRTs[ii] = scans[ii]->rt;
    }
}

void mzSample::restorePreviousRetentionTimes()
{
    if (lastSavedRTs.size() == 0) {
        // restore original RTs if no alignment has been performed
        for (auto scan : scans) {
            scan->rt = scan->originalRt;
        }
    } else {
        for (unsigned int ii = 0; ii < scans.size(); ii++) {
            scans[ii]->rt = lastSavedRTs[ii];
        }
    }
}

vector<Scan*> mzSample::getFragmentationEvents(mzSlice* slice)
{
    vector<Scan*> matchedScans;
    for (auto scan : scans) {
        if (scan->mslevel != 2)
            continue;  // ms2 + scans only
        if (scan->rt < slice->rtmin)
            continue;
        if (scan->rt > slice->rtmax)
            break;
        if (scan->precursorMz >= slice->mzmin
            && scan->precursorMz <= slice->mzmax) {
            matchedScans.push_back(scan);
        }
    }
    return matchedScans;
}

vector<float> mzSample::getIntensityDistribution(int mslevel)
{
    vector<float> allintensities;
    for (unsigned int s = 0; s < this->scans.size(); s++) {
        Scan* scan = this->scans[s];
        if (scan->mslevel != mslevel)
            continue;

        for (unsigned int i = 0; i < scan->mz.size(); i++) {
            allintensities.push_back(scan->intensity[i]);
        }
    }

    return (quantileDistribution(allintensities));
}

/*
@author: Sahil
*/
// TODO: Sahil, Added while merging projectdockwidget
void mzSample::applyPolynomialTransform()
{
    int poly_align_degree = polynomialAlignmentTransformation.size() - 1;
    if (poly_align_degree <= 0)
        return;

    double* transform = &polynomialAlignmentTransformation.front();
    for (unsigned int i = 0; i < scans.size(); i++) {
        float newrt = leasev(transform, poly_align_degree, scans[i]->rt);
        // cerr << "applyPolynomialTransform() " << scans[i]->rt << "\t" <<
        // newrt << endl;
        scans[i]->rt = newrt;
    }
}

mzLink::mzLink()
{
    mz1 = mz2 = 0.0;
    value1 = value2 = 0.0;
    data1 = data2 = NULL;
    correlation = 0;
}

mzLink::mzLink(int a, int b, string n) : value1(a), value2(b), note(n)
{
    mz1 = mz2 = 0.0;
    value1 = value2 = 0.0;
    data1 = data2 = NULL;
    correlation = 0;
}
mzLink::mzLink(float a, float b, string n) : mz1(a), mz2(b), note(n)
{
    mz1 = mz2 = 0.0;
    value1 = value2 = 0.0;
    data1 = data2 = NULL;
    correlation = 0;
}
