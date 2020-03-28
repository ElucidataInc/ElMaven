#include "common/analytics.h"
#include "common/downloadmanager.h"
#include "Compound.h"
#include "csvparser.h"
#include "common/logger.h"
#include "mavenparameters.h"
#include "masscutofftype.h"
#include "mzUtils.h"
#include "peakdetectorcli.h"

PeakDetectorCLI::PeakDetectorCLI(Logger* log, Analytics* analytics)
{
    _log = log;
    _analytics = analytics;
    status = true;
    textStatus = "";
    mavenParameters = new MavenParameters();
    peakDetector = new PeakDetector();
    saveJsonEIC = false;
    quantitationType = PeakGroup::AreaTop;
    clsfModelFilename = "default.model";
    alignMode = AlignmentMode::None;
    _reduceGroupsFlag = true;
    _parseOptions = new ParseOptions();
    _dlManager = new DownloadManager;
    _pollyIntegration = new PollyIntegration(_dlManager);
    _redirectTo = "gsheet_sym_polly_elmaven";
    _currentPollyApp = PollyApp::None;

    analytics->hitScreenView("CLI");
}

PeakDetectorCLI::~PeakDetectorCLI()
{
    delete _dlManager;
    delete _pollyIntegration;
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

        case 'A':
            if (atoi(optarg) == 1) {
                _currentPollyApp = PollyApp::PollyPhi;
            } else if (atoi(optarg) == 2) {
                _currentPollyApp = PollyApp::QuantFit;
            } else {
                _currentPollyApp = PollyApp::None;
            }

        case 'b':
            mavenParameters->minGoodGroupCount = atoi(optarg);
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

        case 'E':
            _pollyExtraInfo = QString(optarg);
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

        case 'P':
            // parse polly specific arguments here
            pollyArgs = QString(optarg);
            break;

        case 'N':
            // parse polly project argument here
            _pollyProject = QString(optarg);
            break;

        case 'S':
            // parse sample cohort filename here
            _sampleCohortFile = QString(optarg);
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
                _log->debug() << "Unable to locate sample file \""
                              << file
                              << "\", passed as parameter. Skipping."
                              << std::flush;
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
            _log->info() << "Found config file "
                         << fileName
                         << ". Processing…"
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
        errorMsg = errorMsg
                   + "To create a default file pass argument --defaultXml.\n";
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
            _log->error() << "Unknown config node: "
                          << node.name()
                          << std::flush;
        }
    }
}

void PeakDetectorCLI::_processPeaksArgsXML(xml_node& peaksArgs)
{
    for (xml_node node = peaksArgs.first_child(); node;
         node = node.next_sibling()) {
        if (strcmp(node.name(), "minGoodGroupCount") == 0) {
            mavenParameters->minGoodGroupCount =
                atoi(node.attribute("value").value());

        } else if (strcmp(node.name(), "matchRtFlag") == 0) {
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
            _log->error() << "Unknown config node: "
                          << node.name()
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

        } else if (strcmp(node.name(), "pollyExtra") == 0) {
            _pollyExtraInfo = QString(node.attribute("value").value());

        } else if (strcmp(node.name(), "samples") == 0) {
            string sampleStr = node.attribute("value").value();
            if (QFile::exists(QString::fromStdString(sampleStr))) {
                filenames.push_back(sampleStr);
            } else {
                _log->debug() << "Unable to locate sample file \""
                              << sampleStr
                              << "\", specified in config file. Skipping."
                              << std::flush;
            }
        } else {
            _log->error() << "Unknown config node: "
                          << node.name()
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
    mavenParameters->compounds = _db.compoundsDB;

    // exit if db is empty
    if (loadCount == 0) {
        _log->error() << "Warning: Given compound database is empty!"
                      << std::flush;
        exit(1);
    }

    // check for invalid compounds
    if (_db.invalidRows.size() > 0) {
        string debugStr = "The following compounds had insufficient information "
                          "for peak detection, and were not loaded:\n";
        for (auto compoundID : _db.invalidRows)
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
        sample->loadSample(filenames[i].c_str());
        sample->sampleName = mzUtils::cleanFilename(filenames[i]);
        sample->isSelected = true;
        if (sample->scans.size() >= 1) {
            mavenParameters->samples.push_back(sample);
            _log->info() << "Loaded Sample: "
                         << sample->getSampleName()
                         << std::flush;
        } else {
            if (sample != NULL) {
                delete sample;
                sample = NULL;
            }
        }
    }

    if (mavenParameters->samples.size() == 0) {
        _log->error() << "Nothing to process. Exiting…" << std::flush;
        exit(1);
    }

    sort(mavenParameters->samples.begin(),
         mavenParameters->samples.end(),
         mzSample::compSampleSort);

    _log->info() << "Loaded "
                 << mavenParameters->samples.size()
                 << " samples"
                 << std::flush;
    cout << endl;

#ifndef __APPLE__
    cout << "Execution time (sample loading): "
         << getTime() - startLoadingTime
         << " seconds.\n";
#endif
}

void PeakDetectorCLI::_makeSampleCohortFile(QString sampleCohortFilename,
                                           QStringList loadedSamples)
{
    QFile file(sampleCohortFilename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "Sample"
        << ",Cohort"
        << "\n";
    for (const auto& sampleName : loadedSamples) {
        out << sampleName << ",\n";
    }

    _log->debug() << "Sample cohort file prepared. Moving on to gsheet "
                     "interface now…"
                  << std::flush;
    file.close();
}

QString PeakDetectorCLI::_isReadyForPolly()
{
    QString message = "ready";
    if (mavenParameters->ligandDbFilename == "") {
        message =
            "Please provide compound database file. It is required for using "
            "Polly applications.";
        return message;
    }
    if (!saveJsonEIC) {
        _log->debug() << "JSON file is required for using Polly applications. "
                         "Overriding existing settings to save JSON data file…"
                      << std::flush;
        saveJsonEIC = true;
    }
    return message;
}

int PeakDetectorCLI::prepareCompoundDbForPolly(QString fileName)
{
    try {
        if (fileName.isEmpty())
            return 0;

        QString SEP = "\t";
        if (fileName.endsWith(".csv", Qt::CaseInsensitive)) {
            SEP = ",";
        } else if (!fileName.endsWith(".tab", Qt::CaseInsensitive)) {
            fileName = fileName + ".tab";
            SEP = "\t";
        }

        QFile data(fileName);
        if (data.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream out(&data);

            // header
            out << "polarity" << SEP;
            out << "compound" << SEP;
            // Addiditional headers added when merging with Maven776 - Kiran
            out << "mz" << SEP;
            out << "charge" << SEP;
            out << "precursorMz" << SEP;
            out << "collisionEnergy" << SEP;
            out << "productMz" << SEP;
            out << "expectedRt" << SEP;
            out << "id" << SEP;
            out << "formula" << SEP;
            out << "srmId" << SEP;
            out << "category" << endl;

            for (unsigned int i = 0; i < mavenParameters->compounds.size();
                 i++) {
                Compound* compound = mavenParameters->compounds[i];

                QString charpolarity;
                if (compound->charge() > 0)
                    charpolarity = "+";
                if (compound->charge() < 0)
                    charpolarity = "-";

                QStringList category;
                auto categoryVect = compound->category();
                for (int i = 0; i < categoryVect.size(); i++) {
                    category << QString(categoryVect[i].c_str());
                }

                out << charpolarity << SEP;
                out << QString(compound->name().c_str()) << SEP;
                out << compound->mz() << SEP;
                out << compound->charge() << SEP;
                out << compound->precursorMz() << SEP;
                out << compound->collisionEnergy() << SEP;
                out << compound->productMz() << SEP;
                out << compound->expectedRt() << SEP;
                out << compound->id().c_str() << SEP;
                out << compound->formula().c_str() << SEP;
                out << compound->srmId().c_str() << SEP;
                out << category.join(";") << SEP;
                out << "\n";
            }
        }
        return 1;
    } catch (...) {
        return 0;
    }
}

bool PeakDetectorCLI::_incompatibleWithPollyApp() 
{
    //MRM data is not compatible with PollyPhi
    bool onlyMS2 = true;
    for (auto sample : mavenParameters->samples) {
        if (sample->ms1ScanCount())
            onlyMS2 = false;
    }
    if (onlyMS2 && _currentPollyApp == PollyApp::PollyPhi) {
        _log->debug() << "PollyPhi currently does not support purely MS2 data. "
                      << "Please use an MS1 or DDA dataset."
                      << std::flush;
        return true;
    }
    
    //Untargeted data is not compatible with any app atm
    if (mavenParameters->processAllSlices) {
        _log->debug() << "Untargeted data is not supported on Polly. Please switch off"
                      << " mass slicing and provide a compound database."
                      << std::flush;
        return true;
    }

    //Unlabelled data is not the desired data for PollyPhi
    if (!mavenParameters->pullIsotopesFlag && 
        _currentPollyApp == PollyApp::PollyPhi) {
        _log->debug() << "PollyPhi is used for the analysis of labeled data. Please "
                      << "switch on isotope detection using paramater -f."
                     << std::flush;
        return true;
    }

    return false;
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

    // save files in the output dir if Polly arguments have not been provided
    if (_currentPollyApp == PollyApp::None || pollyArgs.isEmpty()) {
        // create an output folder
        mzUtils::createDir(mavenParameters->outputdir.c_str());
        string fileName = mavenParameters->outputdir + setName;

        _log->info() << "Saving data reports…" << std::flush;
        saveJson(fileName);
        saveCSV(fileName, false);
    } else {
        if (_incompatibleWithPollyApp())
            exit(0);

        // try uploading to Polly
        QMap<QString, QString> creds = _readCredentialsFromXml(pollyArgs);
        if (creds.empty()) {
            _log->error() << "Error in credentials file. Exiting.." << std::flush;
            exit(1);
        }

        QDateTime currentTime;
        const QString format = "dd-MM-yyyy_hh_mm_ss";
        QString datetimestamp = currentTime.currentDateTime().toString(format);
        datetimestamp.replace(" ", "_");
        datetimestamp.replace(":", "-");

        QString writableTempDir =
            QStandardPaths::writableLocation(
                QStandardPaths::QStandardPaths::GenericConfigLocation)
            + QDir::separator() + "tmp_Elmaven_Polly_files";
        QDir qdir(writableTempDir);
        if (!qdir.exists()) {
            QDir().mkdir(writableTempDir);
            QDir qdir(writableTempDir);
        }

        QString csvFilename =
            writableTempDir + QDir::separator() + datetimestamp
            + "_Peak_table_all_Elmaven_Polly";  // uploading the csv file
        QString jsonFilename =
            writableTempDir + QDir::separator() + datetimestamp
            + "_Peaks_information_json_Elmaven_Polly";  //  uploading the json
                                                        //  file
        QString compoundDbFilename =
            writableTempDir + QDir::separator() + datetimestamp
            + "_Compound_DB_Elmaven_Polly";  //  uploading the compound DB file
        QString sampleCohortFilename =
            writableTempDir + QDir::separator() + datetimestamp
            + "_Cohort_Mapping_Elmaven";  //  uploading the sample cohort file
        int compoundDbStatus =
            prepareCompoundDbForPolly(compoundDbFilename + ".csv");
        QString readyStatus = _isReadyForPolly();
        if (!(readyStatus == "ready") || compoundDbStatus == 0) {
            _log->debug() << "Error while preparing files for Polly: "
                          << readyStatus.toStdString()
                          << std::flush;
            return;
        }

        _log->info() << "Storing temporary data files…" << std::flush;
        saveJson(jsonFilename.toStdString());
        saveCSV(csvFilename.toStdString(), true);
        cout << endl;

        _log->info() << "Uploading data files to Polly…" << std::flush;
        try {
            // add more files to upload, if desired…
            QStringList filesToBeUploaded =
                QStringList() << csvFilename + ".csv"
                              << jsonFilename + ".json"
                              << compoundDbFilename + ".csv";

            if (_currentPollyApp == PollyApp::PollyPhi) {
                QStringList loadedSamples = _getSampleList();

                // check validity of sample cohort file if present
                bool validSampleCohort = false;

                if (!_sampleCohortFile.isEmpty()) {
                    validSampleCohort = _pollyIntegration->validSampleCohort(
                        _sampleCohortFile, loadedSamples);
                }

                if (validSampleCohort) {
                    bool statusSampleCopy = QFile::copy(
                        _sampleCohortFile, sampleCohortFilename + ".csv");
                    _log->debug() << "Sample cohort copy status: "
                                  << statusSampleCopy
                                  << std::flush;
                    _redirectTo = "relative_lcms_elmaven";
                } else {
                    _log->debug() << "There was some problem with the sample cohort "
                                     "file, you will be redirected to an interface "
                                     "on Polly where you can make the cohort file "
                                     "again."
                                  << std::flush;
                    _makeSampleCohortFile(sampleCohortFilename + ".csv",
                                         loadedSamples);
                }

                filesToBeUploaded << sampleCohortFilename + ".csv";
            }

            // jspath and nodepath are very important here. Node executable will
            // be used to connect to Polly, with the help of index.js script.
            QString uploadProjectId = uploadToPolly(jsPath,
                                                    nodePath,
                                                    filesToBeUploaded,
                                                    creds);

            QString redirectionUrl = "";

            // upload was successful if non empty project ID,
            // redirect the user to Polly…
            if (!uploadProjectId.isEmpty()) {
                redirectionUrl = _getRedirectionUrl(datetimestamp,
                                                    uploadProjectId);
            } else {
                _log->error() << "Unable to upload data to Polly."
                              << std::flush;
            }

            _log->debug() << "Redirection URL: \""
                          << redirectionUrl.toStdString()
                          << "\""
                          << std::flush;

            // if redirection URL is not empty then we send the user an email
            // with that URL
            if (!redirectionUrl.isEmpty()) {
                bool response = _sendUserEmail(creds, redirectionUrl);
                string status = response == 1 ? "success"
                                              : "failure";
                _log->info() << "Emailer status: "
                             << status
                             << "!"
                             << std::flush;
            } else {
                _log->error() << "Failed to obtain a redirection URL."
                              << std::flush;
            }
        } catch (...) {
            _log->error() << "Something went wrong when trying to upload data "
                             "to Polly. Please check the CLI arguments."
                          << std::flush;
        }
        bool status = qdir.removeRecursively();
    }
}

QString PeakDetectorCLI::_getRedirectionUrl(QString datetimestamp,
                                            QString uploadProjectId)
{
    QString redirectionUrl = "";
    switch (_currentPollyApp) {
    case PollyApp::PollyPhi: {
        QString workflowId = 
            _pollyIntegration->obtainWorkflowId(PollyApp::PollyPhi);

        QString workflowName = 
            _pollyIntegration->obtainComponentName(PollyApp::PollyPhi);

        QString workflowRequestId =
            _pollyIntegration->createWorkflowRequest(uploadProjectId,
                                                     workflowName,
                                                     workflowId);
        if (!workflowRequestId.isEmpty()) {
            redirectionUrl =
                _pollyIntegration->getWorkflowEndpoint(workflowId,
                                                        workflowRequestId,
                                                        _redirectTo,
                                                        uploadProjectId,
                                                        datetimestamp);
        } else {
            _log->error() << "Unable to create workflow request id. Please try "
                             "again."
                          << std::flush;
        }
        break;
    } case PollyApp::QuantFit: {
        QString componentId =
            _pollyIntegration->obtainComponentId(PollyApp::QuantFit);
        QString runRequestId =
            _pollyIntegration->createRunRequest(componentId,
                                                uploadProjectId,
                                                _pollyExtraInfo);
        if (!runRequestId.isEmpty()) {
            redirectionUrl =
                _pollyIntegration->getComponentEndpoint(componentId,
                                                        runRequestId,
                                                        datetimestamp);
        }
        break;
    } default: break;
    }
    return redirectionUrl;
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

QMap<QString, QString> PeakDetectorCLI::_readCredentialsFromXml(QString filename)
{
    QMap<QString, QString> creds;
    QDomDocument doc;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file)) {
        _log->error() << "Something is wrong with your credentials file. "
                         "Please try again with a valid xml file."
                      << std::flush;
        return creds;
    }

    // Get the root element
    QDomElement docElem = doc.documentElement();

    // you could check the root tag name here if it matters
    QString rootTag = docElem.tagName();  // == credentials
    if (rootTag != "credentials") {
        _log->error() << "The root tag of your credentials file is not "
                         "correct. Please try again with a valid xml file."
                      << std::flush;
        return creds;
    }
    QDomNodeList polly_creds = doc.elementsByTagName("pollyaccountdetails");
    for (int i = 0; i < polly_creds.size(); i++) {
        QDomNode n = polly_creds.item(i);
        QDomElement username = n.firstChildElement("username");
        QDomElement password = n.firstChildElement("password");
        if (username.isNull() || password.isNull()) {
            _log->error() << "Both username and password must be provided in "
                             "the credentials file.Please try again"
                          << std::flush;
            return creds;
        }
        creds["polly_username"] = username.text();
        creds["polly_password"] = password.text();
        // Only considering the first occurance
        break;
    }
    return creds;
}

QString PeakDetectorCLI::uploadToPolly(QString jsPath,
                                       QString nodePath,
                                       QStringList filenames,
                                       QMap<QString, QString> creds)
{
    QString uploadProjectId;

    // set jspath and nodepath for _pollyIntegration library .
    _pollyIntegration->jsPath = jsPath;
    _pollyIntegration->nodePath = nodePath;
    if (!_pollyIntegration->checkNodeExecutable()) {
        #ifdef Q_OS_MAC
            _log->error() << "`node_bin` was not found in your system path. "
                          << "Please contact the technical team at elmaven@elucidata.io"
                          << std::flush;
        #else
            _log->error() << "NodeJS was not found on this system. "
                          "Please install NodeJS and try again."
                          << std::flush;
        #endif
        exit(1);
    }

    // In case of CLI, we don't want persistent login as of now, so deleting
    // existing Token everytime and starting afresh. In future if persistent
    // login is required in CLI, just delete the two lines below this comment.
    QFile pollyCredFile(_pollyIntegration->getCredFile());
    pollyCredFile.remove();

    ErrorStatus response = _pollyIntegration->activeInternet();
    if (response == ErrorStatus::Failure ||
        response == ErrorStatus::Error) {
        _log->error() << "No internet access. Please connect to the internet "
                         "and try again."
                      << std::flush;
        return uploadProjectId;
    }

    ErrorStatus loginResponse = _pollyIntegration->authenticateLogin(
        creds["polly_username"], creds["polly_password"]);
    if (loginResponse == ErrorStatus::Failure) {
        _log->error() << "Incorrect credentials. Please recheck." << std::flush;
        return uploadProjectId;
    } else if (loginResponse == ErrorStatus::Error) {
        _log->error() << "A server error was encountered. Please contact tech "
                         "support at elmaven@elucidata.io if the problem "
                         "persists."
                      << std::flush;
        return uploadProjectId;
    }


    _analytics->hitEvent("Exports", "Polly");
    if (_currentPollyApp == PollyApp::PollyPhi) {
        _analytics->hitEvent("Polly upload", "PollyPhi");
    } else if (_currentPollyApp == PollyApp::QuantFit) {
        _analytics->hitEvent("Polly upload", "QuantFit");
    }

    // User is logged in, now proceeding to upload…

    // This will list all the project corresponding to the user on polly…
    QVariantMap projectNamesId = _pollyIntegration->getUserProjects();
    QStringList keys = projectNamesId.keys();
    QString projectId;
    if (_pollyProject.isEmpty()) {
        _pollyProject = "Default_Elmaven_Polly_Project";
    }
    for (const auto& key : keys) {
        if (projectNamesId[key].toString() == _pollyProject) {
            // that means the name provided by the user matches a project.
            projectId = key;
        }
    }
    if (projectId.isEmpty()) {
        // In case no project matches with the user defined name,
        // Create the project and upload to it. This makes the project name to
        // be mandatory.
        _log->error() << "Creating new project with name: \""
                      << _pollyProject.toStdString()
                      << "\""
                      << std::flush;
        QString newProjectId =
            _pollyIntegration->createProjectOnPolly(_pollyProject);
        uploadProjectId = newProjectId;
    } else {
        uploadProjectId = projectId;
    }

    _pollyIntegration->exportData(filenames, uploadProjectId);

    return uploadProjectId;
}

bool PeakDetectorCLI::_sendUserEmail(QMap<QString, QString> creds,
                                      QString redirectionUrl)
{
    QString userEmail = creds["polly_username"];
    QString emailMessage = QString("Data Successfully uploaded to Polly "
                               "project - \"%1\"").arg(_pollyProject);
    QString emailUrl = QString("<a href='%1'></a>").arg(redirectionUrl);
    QString appname = "";
    switch (_currentPollyApp) {
    case PollyApp::PollyPhi: {
        appname = "pollyphi";
        break;
    } case PollyApp::QuantFit: {
        appname = "quantfit";
        break;
    } default:
        break;
    }

    ErrorStatus errorstatus = _pollyIntegration->sendEmail(userEmail,
                                          emailMessage,
                                          emailUrl,
                                          appname);

    if (errorstatus == ErrorStatus::Success)
        status = true;
    else {
        status = false;
    } 


    return status;
}

void PeakDetectorCLI::saveCSV(string setName, bool pollyExport)
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


    auto ddaGroupAt =
        find_if(begin(mavenParameters->allgroups),
                end(mavenParameters->allgroups),
                [](PeakGroup& group) {
                    if (!group.getCompound())
                        return false;
                    return group.getCompound()->type() == Compound::Type::MS2;
                });
    bool ddaGroupExists = ddaGroupAt != end(mavenParameters->allgroups);

    // Added to pass into csvreports file when merged with Maven776 - Kiran
    // CLI exports the default Group Summary Matrix Format (without set Names)

    bool includeSetNamesLine = false;
    
    csvreports = new CSVReports(fileName, CSVReports::ReportType::GroupReport,
                                mavenParameters->samples, quantitationType,  
                                ddaGroupExists, includeSetNamesLine,
                                mavenParameters, pollyExport);

    for (int i = 0; i < mavenParameters->allgroups.size(); i++) {
        PeakGroup& group = mavenParameters->allgroups[i];
        csvreports->addGroup(&group);
    }


    // NOTE: The following validation is being done to prevent a workflow
    // breaking issue (SYP-24) caused by missing C12 PARENT labels for labelled
    // data. This is only being done for the CLI.

    // Check whether there's a missing C12 PARENT label in the written CSV file.
    bool labeledDataPresent = false;
    bool fileNeedsCorrection = false;
    vector<int> groupsWithMissingLabels;
    CsvParser* csvParser = CsvParser_new(fileName.c_str(), ",", 1);
    CsvRow* currentRow = NULL;
    // Iterates over each row, continuously assigning a pointer to an unread
    // CsvRow.
    while (currentRow = CsvParser_getRow(csvParser)) {
        // eighth field is the isotope label.
        string isotopeLabel = string(currentRow->fields_[7]);
        if (isotopeLabel.empty()) {
            int groupId = atoi(currentRow->fields_[2]);
            groupsWithMissingLabels.push_back(groupId);
            fileNeedsCorrection = true;
        } else {
            labeledDataPresent = true;
        }
    }

    if (!labeledDataPresent)
        fileNeedsCorrection = false;

    if (fileNeedsCorrection) {
        // rewrite file if needed
        csvreports->setGroupId(0);

        sort(groupsWithMissingLabels.begin(), groupsWithMissingLabels.end());
        for (int i = 0; i < mavenParameters->allgroups.size(); i++) {
            PeakGroup& group = mavenParameters->allgroups[i];
            // `csvreports->groupId` is incremented with each group added, we
            // can use this to check if the group to be added next is present
            // within groupsWithMissingLabels or not.
            if (binary_search(groupsWithMissingLabels.begin(),
                              groupsWithMissingLabels.end(),
                              csvreports->groupId() + 1)) {
                PeakGroup newGroup = group;
                Compound* compound = group.getCompound();
                float compoundMz = MassCalculator::computeMass(
                    compound->formula(), mavenParameters->getCharge(compound));
                float cutoffDist = massCutoffDist(
                    group.meanMz, compoundMz, mavenParameters->massCutoffMerge);
                if (cutoffDist
                    < mavenParameters->massCutoffMerge->getMassCutoff()) {
                    PeakGroup childGroup = group;
                    childGroup.tagString = "C12 PARENT";
                    newGroup.children.push_back(childGroup);
                    csvreports->addGroup(&newGroup);
                    continue;
                }
            }
            csvreports->addGroup(&group);
        }
    }

    if (csvreports->getErrorReport() != "") {
        _log->info() << "Writing to CSV failed with error - "
                     << csvreports->getErrorReport().toStdString()
                     << "."
                     << std::flush;
        return;
    }

    _log->info() << "CSV output file: " << fileName << std::flush;

#ifndef __APPLE__
    cout << "\tExecution time (Saving CSV): "
         << getTime() - startSavingCSV << " seconds.\n";
#endif
}

void PeakDetectorCLI::reduceGroups()
{
    sort(mavenParameters->allgroups.begin(),
         mavenParameters->allgroups.end(),
         PeakGroup::compMz);
    _log->info() << "Reducing "
                 << mavenParameters->allgroups.size()
                 << " groups…"
                 << std::flush;

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
                 << mavenParameters->allgroups.size()
                 << std::flush;
    cout << endl;
}

double get_wall_time()
{
    struct timeval time;
    if (gettimeofday(&time, nullptr)){
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
