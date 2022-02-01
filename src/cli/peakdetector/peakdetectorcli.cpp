#define _STR(X) #X
#define STR(X) _STR(X)

#include "peakdetectorcli.h"
#include "Compound.h"
#include "common/analytics.h"
#include "common/downloadmanager.h"
#include "common/logger.h"
#include "csvparser.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "mzAligner.h"
#include "mzUtils.h"
#include "obiwarp.h"
#include "projectDB/projectdatabase.h"

PeakDetectorCLI::PeakDetectorCLI(Logger* log,
                                 Analytics* analytics,
                                 int argc,
                                 char* argv[])
{
    _log = log;
    _analytics = analytics;
    status = true;
    textStatus = "";
    mavenParameters = new MavenParameters();
    peakDetector = new PeakDetector();
    saveJsonEIC = false;
    quantitationType = PeakGroup::AreaTop;
    alignMode = AlignmentMode::None;
    _reduceGroupsFlag = true;
    _parseOptions = new ParseOptions();
    _dlManager = new DownloadManager;

    analytics->hitScreenView("CLI");

    if (argc <= 0)
        return;

    QString execDirPath = "";
    QFileInfo execFileInfo(argv[0]);
    QDir execDir = execFileInfo.absoluteDir();
#ifdef Q_OS_WIN
    execDirPath = execDir.absolutePath();
#endif

#ifdef Q_OS_LINUX
    execDirPath = execDir.absolutePath();
#endif

#ifdef Q_OS_MAC
    execDir.cdUp();  // <install_dir>/El-MAVEN/bin/peakdetector.app/Contents/
    execDir.cdUp();  // <install_dir>/El-MAVEN/bin/peakdetector.app/
    execDir.cdUp();  // <install_dir>/El-MAVEN/bin/
    execDirPath = execDir.absolutePath();
#endif

    clsfModelFilename =
        QString(execDirPath + QDir::separator() + QString("default.model"))
            .toStdString();
}

PeakDetectorCLI::~PeakDetectorCLI()
{
    delete _dlManager;
}

void PeakDetectorCLI::processOptions(int argc, char* argv[])
{
    auto options = getOptions();
    const char** optionsArray = new const char*[options.size()];
    copy(begin(options), end(options), optionsArray);

    // parse input options
    Options opts(*argv, optionsArray);
    OptArgvIter iter(--argc, ++argv);
    const char* optarg;

    while (const char optchar = opts(iter, optarg)) {
        switch (optchar) {
        case 'a':
            mavenParameters->alignSamplesFlag = true;

            if (atoi(optarg) == 1)
                alignMode = AlignmentMode::ObiWarp;

            else if (atoi(optarg) == 2)
                alignMode = AlignmentMode::PolyFit;

            else {
                alignMode = AlignmentMode::None;
                mavenParameters->alignSamplesFlag = false;
            }

            break;

        case 'c':
            mavenParameters->compoundRTWindow = atof(optarg);
            mavenParameters->matchRtFlag = true;
            if (mavenParameters->compoundRTWindow == 0)
                mavenParameters->matchRtFlag = false;
            break;

        case 'C':
            mavenParameters->compoundMassCutoffWindow->setMassCutoffAndType(
                atof(optarg), "ppm");
            break;

        case 'd':
            mavenParameters->ligandDbFilename = optarg;
            break;

        case 'e':
            mavenParameters->processAllSlices = true;
            if (atoi(optarg) == 0)
                mavenParameters->processAllSlices = false;
            break;

        case 'f': {
            mavenParameters->pullIsotopesFlag = 0;
            int label = 0;
            label = atoi(optarg);
            if (label > 0) {
                mavenParameters->pullIsotopesFlag = 1;
                if (label & 1)
                    mavenParameters->C13Labeled_BPE = true;
                else
                    mavenParameters->C13Labeled_BPE = false;
                if (label & 2)
                    mavenParameters->S34Labeled_BPE = true;
                else
                    mavenParameters->S34Labeled_BPE = false;
                if (label & 4)
                    mavenParameters->N15Labeled_BPE = true;
                else
                    mavenParameters->N15Labeled_BPE = false;
                if (label & 8)
                    mavenParameters->D2Labeled_BPE = true;
                else
                    mavenParameters->D2Labeled_BPE = false;
            }
        } break;

        case 'g':
            mavenParameters->grouping_maxRtWindow = atof(optarg);
            break;

        case 'h':
            opts.usage(cout, "files --Enter full path to each sample file");
            exit(0);
            break;

        case 'i':
            mavenParameters->minGroupIntensity = atof(optarg);
            break;

        case 'I':
            mavenParameters->quantileIntensity = atof(optarg);
            break;

        case 'j':
            saveJsonEIC = true;
            if (atoi(optarg) == 0)
                saveJsonEIC = false;
            break;

        case 'k':
            mavenParameters->charge = atoi(optarg);
            break;

        case 'm':
            clsfModelFilename = optarg;
            break;

        case 'n':
            mavenParameters->eicMaxGroups = atoi(optarg);
            break;

        case 'o':
            mavenParameters->outputdir = optarg + string(DIR_SEPARATOR_STR);
            break;

        case 'p':
            mavenParameters->massCutoffMerge->setMassCutoffAndType(atof(optarg),
                                                                   "ppm");
            break;

        case 'q':
            mavenParameters->minQuality = atof(optarg);
            break;

        case 'Q':
            mavenParameters->quantileQuality = atof(optarg);
            break;

        case 'r':
            mavenParameters->rtStepSize = atoi(optarg);
            break;

        case 's':
            _projectName = QString(optarg);
            break;

        case 'v':
            mavenParameters->ionizationMode = atoi(optarg);
            break;

        case 'w':
            mavenParameters->minNoNoiseObs = atoi(optarg);
            break;

        case 'x':
            if (!optarg) {
                processXML("config.xml");
            } else {
                processXML((char*)optarg);
            }
            break;

        case 'X':
            createXMLFile("config.xml");
            break;

        case 'y':
            mavenParameters->eic_smoothingWindow = atoi(optarg);
            break;

        case 'z':
            mavenParameters->minSignalBaseLineRatio = atof(optarg);
            break;

        default:
            break;
        }
    }

    string argsInfo = "Running with arguments: ";
    for (int i = 0; i < argc; i++)
        argsInfo += string(argv[i]) + " ";
    _log->info() << argsInfo << std::flush;
    cout << endl;

    if (iter.index() < argc) {
        for (int i = iter.index(); i < argc; i++) {
            string file = argv[i];
            if (QFile::exists(QString::fromStdString(file))) {
                filenames.push_back(file);
            } else {
                _log->debug()
                    << "Unable to locate sample file \"" << file
                    << "\", passed as parameter. Skipping." << std::flush;
            }
        }
    }

    delete[] optionsArray;
}

void PeakDetectorCLI::processXML(const char* fileName)
{
    ifstream xmlFile(fileName);

    string file = string(fileName);

    if (file.substr(file.find_last_of(".") + 1) != "xml") {
        status = false;
        textStatus += "Not a xml file.\n";
        return;
    }

    if (xmlFile) {
        xml_document doc;
        doc.load_file(fileName, pugi::parse_minimal);
        if (string(doc.first_child().name()) == "Arguments") {
            _log->info() << "Found config file " << fileName << ". Processing…"
                         << std::flush;
            xml_node argsNode = doc.child("Arguments");
            xml_node optionsArgs = argsNode.child("OptionsDialogArguments");
            xml_node peaksArgs = argsNode.child("PeaksDialogArguments");
            xml_node generalArgs = argsNode.child("GeneralArguments");
            _processOptionsArgsXML(optionsArgs);
            _processPeaksArgsXML(peaksArgs);
            _processGeneralArgsXML(generalArgs);
        } else {
            _log->info() << "Provided XML file is not an arguments file. "
                            "Trying to read it as El-MAVEN settings file…"
                         << endl;
            processSettingsFromGui(fileName);
        }
    } else {
        status = false;
        string errorMsg = "Error Loading file " + (string)fileName
                          + ". File not found. Can't process further.\n";
        errorMsg =
            errorMsg + "To create a default file pass argument --defaultXml.\n";
        errorMsg = errorMsg
                   + "This will create a default file config.xml into the root "
                     "folder.\n";

        textStatus += errorMsg;
    }
    cout << endl;
}

void PeakDetectorCLI::createXMLFile(const char* fileName)
{
    Arguments arguments;

    arguments.populateArgs();
    QStringList optionsDialog = arguments.optionsDialogArgs;
    QStringList peakDialog = arguments.peakDialogArgs;
    QStringList general = arguments.generalArgs;

    xml_document doc;
    xml_node args = doc.append_child("Arguments");

    _parseOptions->addChildren(args, "OptionsDialogArguments", optionsDialog);
    _parseOptions->addChildren(args, "PeaksDialogArguments", peakDialog);
    _parseOptions->addChildren(args, "GeneralArguments", general);

    doc.save_file(fileName);

    status = false;

    textStatus = textStatus + "Default file \"" + fileName + "\" created.\n";
}

void PeakDetectorCLI::_processOptionsArgsXML(xml_node& optionsArgs)
{
    for (xml_node node = optionsArgs.first_child(); node;
         node = node.next_sibling()) {
        if (strcmp(node.name(), "ionizationMode") == 0) {
            mavenParameters->ionizationMode =
                atoi(node.attribute("value").value());
        } else if (strcmp(node.name(), "charge") == 0) {
            mavenParameters->charge = atoi(node.attribute("value").value());
        } else if (strcmp(node.name(), "compoundPPMWindow") == 0) {
            mavenParameters->compoundMassCutoffWindow->setMassCutoffAndType(
                atof(node.attribute("value").value()), "ppm");
        } else {
            _log->error() << "Unknown config node: " << node.name()
                          << std::flush;
        }
    }
}

void PeakDetectorCLI::_processPeaksArgsXML(xml_node& peaksArgs)
{
    for (xml_node node = peaksArgs.first_child(); node;
         node = node.next_sibling()) {
        if (strcmp(node.name(), "matchRtFlag") == 0) {
            mavenParameters->compoundRTWindow =
                atof(node.attribute("value").value());
            mavenParameters->matchRtFlag = true;
            if (mavenParameters->compoundRTWindow == 0)
                mavenParameters->matchRtFlag = false;

        } else if (strcmp(node.name(), "Db") == 0) {
            mavenParameters->ligandDbFilename = node.attribute("value").value();

        } else if (strcmp(node.name(), "processAllSlices") == 0) {
            mavenParameters->processAllSlices = true;
            if (atoi(node.attribute("value").value()) == 0)
                mavenParameters->processAllSlices = false;

        } else if (strcmp(node.name(), "pullIsotopes") == 0) {
            mavenParameters->pullIsotopesFlag = 0;
            int label = 0;
            label = atoi(node.attribute("value").value());
            if (label > 0) {
                mavenParameters->pullIsotopesFlag = 1;
                if (label & 1)
                    mavenParameters->C13Labeled_BPE = true;
                else
                    mavenParameters->C13Labeled_BPE = false;
                if (label & 2)
                    mavenParameters->S34Labeled_BPE = true;
                else
                    mavenParameters->S34Labeled_BPE = false;
                if (label & 4)
                    mavenParameters->N15Labeled_BPE = true;
                else
                    mavenParameters->N15Labeled_BPE = false;
                if (label & 8)
                    mavenParameters->D2Labeled_BPE = true;
                else
                    mavenParameters->D2Labeled_BPE = false;
            }

        } else if (strcmp(node.name(), "grouping_maxRtWindow") == 0) {
            mavenParameters->grouping_maxRtWindow =
                atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "minGroupIntensity") == 0) {
            mavenParameters->minGroupIntensity =
                atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "quantileIntensity") == 0) {
            mavenParameters->quantileIntensity =
                atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "model") == 0) {
            clsfModelFilename = node.attribute("value").value();

        } else if (strcmp(node.name(), "eicMaxGroups") == 0) {
            mavenParameters->eicMaxGroups =
                atoi(node.attribute("value").value());

        } else if (strcmp(node.name(), "ppmMerge") == 0) {
            mavenParameters->massCutoffMerge->setMassCutoffAndType(
                atof(node.attribute("value").value()), "ppm");
            mavenParameters->compoundMassCutoffWindow->setMassCutoffType("ppm");

            // TODO -
            // mavenParameters->compoundMassCutoffWindow
            //                ->setMassCutoffAndType(atof(node.attribute("value")
            //                                                .value()), "ppm");
            // has to be removed later when <compoundMassCutoffWindow> attribute
            // is added in test.xml file.

        } else if (strcmp(node.name(), "minQuality") == 0) {
            mavenParameters->minQuality = atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "quantileQuality") == 0) {
            mavenParameters->quantileQuality =
                atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "rtStepSize") == 0) {
            mavenParameters->rtStepSize = atoi(node.attribute("value").value());

        } else if (strcmp(node.name(), "minPeakWidth") == 0) {
            mavenParameters->minNoNoiseObs =
                atoi(node.attribute("value").value());

        } else if (strcmp(node.name(), "eicSmoothingWindow") == 0) {
            mavenParameters->eic_smoothingWindow =
                atoi(node.attribute("value").value());

        } else if (strcmp(node.name(), "minSignalBaseLineRatio") == 0) {
            mavenParameters->minSignalBaseLineRatio =
                atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "quantitationType") == 0) {
            mavenParameters->peakQuantitation = (PeakGroup::QType)atoi(
                node.attribute("value")
                    .value());  // AreaTop=0, Area=1, Height=2,
                                // AreaNotCorrected=3
            quantitationType =
                (PeakGroup::QType)atoi(node.attribute("value").value());

        } else if (strcmp(node.name(), "minScanMz") == 0) {
            mavenParameters->minMz = atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "maxScanMz") == 0) {
            mavenParameters->maxMz = atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "minScanRt") == 0) {
            mavenParameters->minRt = atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "maxScanRt") == 0) {
            mavenParameters->maxRt = atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "minScanIntensity") == 0) {
            mavenParameters->minIntensity =
                atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "maxScanIntensity") == 0) {
            mavenParameters->maxIntensity =
                atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "minSignalBaselineDifference") == 0) {
            mavenParameters->minSignalBaselineDifference =
                atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "minPeakQuality") == 0) {
            mavenParameters->minPeakQuality =
                atof(node.attribute("value").value());

        } else if (strcmp(node.name(), "minIsotopicPeakQuality") == 0) {
            mavenParameters->minIsotopicPeakQuality =
                atof(node.attribute("value").value());

        } else {
            _log->error() << "Unknown config node: " << node.name()
                          << std::flush;
        }
    }
}

void PeakDetectorCLI::_processGeneralArgsXML(xml_node& generalArgs)
{
    for (xml_node node = generalArgs.first_child(); node;
         node = node.next_sibling()) {
        if (strcmp(node.name(), "alignSamples") == 0) {
            mavenParameters->alignSamplesFlag = true;
            switch (atoi(node.attribute("value").value())) {
            case 1:
                alignMode = AlignmentMode::ObiWarp;
                break;

            case 2:
                alignMode = AlignmentMode::PolyFit;
                break;

            default:
                mavenParameters->alignSamplesFlag = false;
                alignMode = AlignmentMode::None;
                break;
            }
        } else if (strcmp(node.name(), "saveEicJson") == 0) {
            saveJsonEIC = true;
            if (atoi(node.attribute("value").value()) == 0)
                saveJsonEIC = false;

        } else if (strcmp(node.name(), "outputdir") == 0) {
            mavenParameters->outputdir =
                node.attribute("value").value() + string(DIR_SEPARATOR_STR);

        } else if (strcmp(node.name(), "samples") == 0) {
            string sampleStr = node.attribute("value").value();
            if (QFile::exists(QString::fromStdString(sampleStr))) {
                filenames.push_back(sampleStr);
            } else {
                _log->debug()
                    << "Unable to locate sample file \"" << sampleStr
                    << "\", specified in config file. Skipping." << std::flush;
            }
        } else {
            _log->error() << "Unknown config node: " << node.name()
                          << std::flush;
        }
    }
}

void PeakDetectorCLI::processSettingsFromGui(const string& settingsFilepath)
{
    bool fileLoaded = false;
    QFile file(settingsFilepath.c_str());
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        if (mavenParameters->loadSettings(data.data()))
            fileLoaded = true;
    }

    if (!fileLoaded) {
        _log->error() << "Failed to load settings file from El-MAVEN"
                      << std::flush;
    }
}

void PeakDetectorCLI::loadClassificationModel(string clsfModelFilename)
{
    _log->info() << "Loading classifiation model…" << std::flush;
    mavenParameters->clsf = new ClassifierNeuralNet();
    mavenParameters->clsf->loadModel(clsfModelFilename);
    cout << endl;
}

void PeakDetectorCLI::loadCompoundsFile()
{
    // exit if no db file has been provided
    if (mavenParameters->ligandDbFilename.empty()) {
        _log->error() << "Please provide a compound database file to proceed "
                         "with targeted analysis. Use the '-h' argument to see "
                         "all available options."
                      << std::flush;
        exit(0);
    }

    // load compound list
    mavenParameters->processAllSlices = false;
    _log->info() << "Loading compound database…" << std::flush;
    int loadCount = _db.loadCompoundCSVFile(mavenParameters->ligandDbFilename);
    auto compoundsDB = _db.compoundsDB();
    vector<Compound*> v(compoundsDB.begin(), compoundsDB.end());
    mavenParameters->compounds = v;

    // exit if db is empty
    if (loadCount == 0) {
        _log->error() << "Warning: Given compound database is empty!"
                      << std::flush;
        exit(1);
    }

    // check for invalid compounds
    vector<string> invalidRows = _db.invalidRows();
    if (invalidRows.size() > 0) {
        string debugStr =
            "The following compounds had insufficient information "
            "for peak detection, and were not loaded:\n";
        for (auto compoundID : invalidRows)
            debugStr += " - " + compoundID + "\n";
        _log->debug() << debugStr << std::flush;
    }

    _log->info() << "Loaded " << loadCount << " compounds" << std::flush;
    cout << endl;
}

void PeakDetectorCLI::loadSamples(vector<string>& filenames)
{
#ifndef __APPLE__
    double startLoadingTime = getTime();
#endif
    _log->info() << "Loading samples…" << std::flush;

    for (unsigned int i = 0; i < filenames.size(); i++) {
        mzSample* sample = new mzSample();
        try {
            sample->loadSample(filenames[i].c_str());
        } catch (const std::bad_alloc&) {
            cerr << "MemoryError: "
                 << "ran out of memory" << endl;
            mzUtils::delete_all(sample->scans);
        }
        if (!sample->scans.empty()) {
            sample->sampleName = mzUtils::cleanFilename(filenames[i]);
            sample->isSelected = true;
            mavenParameters->samples.push_back(sample);
            _log->info() << "Loaded sample: " << sample->getSampleName()
                         << std::flush;
        } else {
            if (sample != NULL) {
                delete sample;
                sample = NULL;
            }
            _log->info() << "Failed to load file: " << filenames[i]
                         << std::flush;
        }
    }

    if (mavenParameters->samples.size() == 0) {
        _log->error() << "Nothing to process. Exiting…" << std::flush;
        exit(1);
    }

    sort(mavenParameters->samples.begin(),
         mavenParameters->samples.end(),
         mzSample::compSampleSort);

    _log->info() << "Loaded " << mavenParameters->samples.size() << " samples"
                 << std::flush;
    cout << endl;

#ifndef __APPLE__
    cout << "Execution time (sample loading): " << getTime() - startLoadingTime
         << " seconds.\n";
#endif
}

void PeakDetectorCLI::alignSamples(const int& method)
{
    if (mavenParameters->samples.size() > 1) {
        switch (method) {
        case 1: {
            cerr << "Starting OBI-WARP alignment" << std::endl;
            // TODO: move the hard coded values in  default_settings.xml and
            // instead of using obi params make use mavenParameters to access
            // all the values.
            ObiParams params(
                "cor", false, 2.0, 1.0, 0.20, 3.40, 0.0, 20.0, false, 0.60);
            Aligner mzAligner;
            mzAligner.alignWithObiWarp(
                mavenParameters->samples, &params, mavenParameters);
        } break;

        case 2: {
            mavenParameters->writeCSVFlag = false;
            peakDetector->processFeatures();

            cerr << "Starting PolyFit alignment using "
                 << mavenParameters->allgroups.size() << " groups" << endl;

            vector<PeakGroup*> agroups(mavenParameters->allgroups.size());
            for (unsigned int i = 0; i < mavenParameters->allgroups.size(); i++)
                agroups[i] = &mavenParameters->allgroups[i];

            // init aligner
            Aligner aligner;
            aligner.doAlignment(agroups);
            mavenParameters->writeCSVFlag = true;
        } break;

        default:
            break;
        }
    }
}

void PeakDetectorCLI::saveEmdb()
{
    if (_projectName.isEmpty())
        return;

    if (!_projectName.endsWith(".emDB", Qt::CaseInsensitive))
        _projectName = _projectName + ".emDB";
    if (QFile::exists(_projectName))
        QFile::remove(_projectName);

    bool saveRawData = false;
    if (_projectName.contains(".raw", Qt::CaseInsensitive))
        saveRawData = true;

    _log->info() << "\nSaving project as \"" << _projectName.toStdString()
                 << "\"…" << std::flush;

    if (mavenParameters->samples.size() == 0)
        return;

    auto version = string(STR(APPVERSION));
    auto sessionDb =
        new ProjectDatabase(_projectName.toStdString(), version, saveRawData);
    if (sessionDb) {
        shared_ptr<MavenParameters> mp(mavenParameters);
        auto settingsMap = sessionDb->fromParametersToMap(mp);
        settingsMap["activeTableName"] = mavenParameters->ligandDbFilename;
        sessionDb->saveGlobalSettings(settingsMap);

        sessionDb->saveSamples(mavenParameters->samples);
        sessionDb->saveAlignment(mavenParameters->samples);

        vector<PeakGroup*> groupVector;
        for (auto& group : mavenParameters->allgroups)
            groupVector.push_back(&group);
        sessionDb->saveGroups(groupVector, mavenParameters->ligandDbFilename);

        set<Compound*> compoundSet(mavenParameters->compounds.begin(),
                                   mavenParameters->compounds.end());
        sessionDb->saveCompounds(compoundSet);
        delete sessionDb;

        _log->info() << "Finished saving emDB project." << std::flush;
        return;
    }
    _log->error() << "Unable to open SQLite DB." << _projectName.toStdString()
                  << std::flush;
}

void PeakDetectorCLI::writeReport(string setName,
                                  QString jsPath,
                                  QString nodePath)
{
    // TODO kailash, this function should not have jsPath and nodePath as its
    // arguments…
    _log->info() << "Writing report for groups…" << std::flush;

    // reduce groups
    _groupReduction();

    // create an output folder
    mzUtils::createDir(mavenParameters->outputdir);
    string fileName = mavenParameters->outputdir + setName;

    _log->info() << "Saving data reports…" << std::flush;
    saveJson(fileName);
    saveCSV(fileName);
}

QStringList PeakDetectorCLI::_getSampleList()
{
    QStringList loadedSamples;
    vector<mzSample*> visibleSamples = mavenParameters->getVisibleSamples();

    for (const auto& sample : visibleSamples) {
        loadedSamples.append(QString::fromStdString(sample->getSampleName()));
    }

    return loadedSamples;
}

void PeakDetectorCLI::_groupReduction()
{
    if (_reduceGroupsFlag) {
#ifndef __APPLE__
        double startGroupReduction = getTime();
#endif

        reduceGroups();

#ifndef __APPLE__
        cout << "Execution time (group reduction): "
             << getTime() - startGroupReduction << " seconds.\n";
#endif
    }
}

void PeakDetectorCLI::saveJson(string setName)
{
    if (saveJsonEIC) {
#ifndef __APPLE__
        double startSavingJson = getTime();
#endif

        _jsonReports = new JSONReports(mavenParameters);
        string fileName = setName + ".json";
        _jsonReports->save(
            fileName, mavenParameters->allgroups, mavenParameters->samples);
        _log->info() << "JSON output file: " << fileName << std::flush;
#ifndef __APPLE__
        cout << "Execution time (saving EIC in JSON) : "
             << getTime() - startSavingJson << " seconds.\n";
#endif
    }
}

void PeakDetectorCLI::saveCSV(string setName)
{
#ifndef __APPLE__
    double startSavingCSV = getTime();
#endif

    string fileName = setName + ".csv";

    CSVReports* csvreports;

    if (mavenParameters->allgroups.size() == 0) {
        _log->info() << "Writing to CSV failed: no groups found." << std::flush;
        return;
    }

    if (fileName.empty())
        return;

    if (mavenParameters->samples.size() == 0)
        return;

    bool includeSetNamesLine = false;

    vector<PeakGroup*> groupVector;
    for (auto& group : mavenParameters->allgroups)
        groupVector.push_back(&group);
    auto reportMode = CSVReports::guessAcquisitionMode(groupVector);

    auto ms2GroupAt =
        find_if(begin(mavenParameters->allgroups),
                end(mavenParameters->allgroups),
                [](PeakGroup group) {
                    if (!group.hasCompoundLink())
                        return false;
                    return (group.getCompound()->type() == Compound::Type::MS2);
                });
    bool ms2GroupExists = ms2GroupAt != end(mavenParameters->allgroups);

    csvreports = new CSVReports(fileName,
                                CSVReports::ReportType::GroupReport,
                                mavenParameters->samples,
                                quantitationType,
                                ms2GroupExists,
                                reportMode,
                                includeSetNamesLine,
                                mavenParameters);

    for (int i = 0; i < mavenParameters->allgroups.size(); i++) {
        PeakGroup& group = mavenParameters->allgroups[i];
        csvreports->addGroup(&group);
    }

    if (csvreports->getErrorReport() != "") {
        _log->info() << "Writing to CSV failed with error - "
                     << csvreports->getErrorReport().toStdString() << "."
                     << std::flush;
        return;
    }

    _log->info() << "CSV output file: " << fileName << std::flush;

#ifndef __APPLE__
    cout << "\tExecution time (Saving CSV): " << getTime() - startSavingCSV
         << " seconds.\n";
#endif
}

void PeakDetectorCLI::reduceGroups()
{
    sort(mavenParameters->allgroups.begin(),
         mavenParameters->allgroups.end(),
         PeakGroup::compMz);
    _log->info() << "Reducing " << mavenParameters->allgroups.size()
                 << " groups…" << std::flush;

    // init deleteFlag
    for (unsigned int i = 0; i < mavenParameters->allgroups.size(); i++) {
        mavenParameters->allgroups[i].deletedFlag = false;
    }

    for (unsigned int i = 0; i < mavenParameters->allgroups.size(); i++) {
        PeakGroup& grup1 = mavenParameters->allgroups[i];
        if (grup1.deletedFlag)
            continue;
        for (unsigned int j = i + 1; j < mavenParameters->allgroups.size();
             j++) {
            PeakGroup& grup2 = mavenParameters->allgroups[j];
            if (grup2.deletedFlag)
                continue;

            float rtoverlap = mzUtils::checkOverlap(
                grup1.minRt, grup1.maxRt, grup2.minRt, grup2.maxRt);
            float masscutoffdist = massCutoffDist(
                grup2.meanMz, grup1.meanMz, mavenParameters->massCutoffMerge);
            if (masscutoffdist
                > mavenParameters->massCutoffMerge->getMassCutoff())
                break;

            if (rtoverlap > 0.8
                && masscutoffdist
                       < mavenParameters->massCutoffMerge->getMassCutoff()) {
                if (grup1.maxIntensity <= grup2.maxIntensity) {
                    grup1.deletedFlag = true;
                    // allgroups.erase(allgroups.begin()+i);
                    // i--;
                    break;
                } else if (grup1.maxIntensity > grup2.maxIntensity) {
                    grup2.deletedFlag = true;
                    // allgroups.erase(allgroups.begin()+j);
                    // i--;
                    // break;
                }
            }
        }
    }
    int reducedGroupCount = 0;
    vector<PeakGroup> allgroups_;
    for (int i = 0; i < mavenParameters->allgroups.size(); i++) {
        PeakGroup& grup1 = mavenParameters->allgroups[i];
        if (grup1.deletedFlag == false) {
            allgroups_.push_back(grup1);
            reducedGroupCount++;
        }
    }
    mavenParameters->allgroups = allgroups_;
    _log->info() << "Done. Final group count: "
                 << mavenParameters->allgroups.size() << std::flush;
    cout << endl;
}

double get_wall_time()
{
    struct timeval time;
    if (gettimeofday(&time, nullptr)) {
        //  Handle error
        return 0;
    }
    return double(time.tv_sec) + double(time.tv_usec * .000001);
}

double get_cpu_time()
{
#ifndef __APPLE__
    return (double)getTime() / CLOCKS_PER_SEC;
#endif
    return 0;
}
