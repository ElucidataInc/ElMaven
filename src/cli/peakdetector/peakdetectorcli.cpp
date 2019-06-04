#include "common/downloadmanager.h"
#include "Compound.h"
#include "csvparser.h"
#include "mavenparameters.h"
#include "masscutofftype.h"
#include "mzUtils.h"
#include "peakdetectorcli.h"

PeakDetectorCLI::PeakDetectorCLI()
{
    status = true;
    textStatus = "";
    mavenParameters = new MavenParameters();
    peakDetector = new PeakDetector();
    saveJsonEIC = false;
    saveMzrollFile = true;
    quantitationType = PeakGroup::AreaTop;
    clsfModelFilename = "default.model";
    alignMode = AlignmentMode::None;
    _reduceGroupsFlag = true;
    _parseOptions = new ParseOptions();
    _dlManager = new DownloadManager;
    _pollyIntegration = new PollyIntegration(_dlManager);
    _redirectTo = "gsheet_sym_polly_elmaven";
    _currentPollyApp = PollyApp::None;
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

        case 's':
            saveMzrollFile = true;
            if (atoi(optarg) == 0)
                saveMzrollFile = false;
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

    cout << "\n\nCommand:  ";

    for (int i = 0; i < argc; i++)
        cout << argv[i] << " ";

    cout << "\n\n\n";

    if (iter.index() < argc) {
        for (int i = iter.index(); i < argc; i++)
            filenames.push_back(argv[i]);
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
        cout << endl << "Found " << fileName << endl;
        cout << endl << "Processing…" << endl;

        xml_document doc;
        doc.load_file(fileName, pugi::parse_minimal);
        xml_node argsNode = doc.child("Arguments");

        xml_node optionsArgs = argsNode.child("OptionsDialogArguments");
        xml_node peaksArgs = argsNode.child("PeaksDialogArguments");
        xml_node generalArgs = argsNode.child("GeneralArguments");

        _processOptionsArgsXML(optionsArgs);
        _processPeaksArgsXML(peaksArgs);
        _processGeneralArgsXML(generalArgs);

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
            cout << endl << "Unknown node : " << node.name() << endl;
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
            cout << endl << "Unknown node : " << node.name() << endl;
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

        } else if (strcmp(node.name(), "savemzroll") == 0) {
            saveMzrollFile = true;
            if (atoi(node.attribute("value").value()) == 0)
                saveMzrollFile = false;

        } else if (strcmp(node.name(), "pollyExtra") == 0) {
            _pollyExtraInfo = QString(node.attribute("value").value());

        } else if (strcmp(node.name(), "samples") == 0) {
            string sampleStr = node.attribute("value").value();
            filenames.push_back(sampleStr);

        } else {
            cout << endl << "Unknown node : " << node.name() << endl;
        }
    }
}

void PeakDetectorCLI::loadClassificationModel(string clsfModelFilename)
{
    cout << "Loading classifiation model" << endl;
    cout << "clsfModelFilename " << clsfModelFilename << endl;
    mavenParameters->clsf = new ClassifierNeuralNet();
    mavenParameters->clsf->loadModel(clsfModelFilename);
}

void PeakDetectorCLI::loadCompoundsFile()
{
    // exit if no db file has been provided
    if (mavenParameters->ligandDbFilename.empty()) {
        cerr << "\nPlease provide a compound database file to proceed with "
                "targeted analysis."
             << "Use the '-h' argument to see all available options." << endl;
        exit(0);
    }

    // load compound list
    mavenParameters->processAllSlices = false;
    cout << "\nLoading ligand database" << endl;
    int loadCount = _db.loadCompoundCSVFile(mavenParameters->ligandDbFilename);
    mavenParameters->compounds = _db.compoundsDB;

    // exit if db is empty
    if (loadCount == 0) {
        cerr << "Warning: Given compound database is empty!" << endl;
        exit(1);
    }

    // check for invalid compounds
    if (_db.invalidRows.size() > 0) {
        cout << "The following compounds had insufficient information for peak "
                "detection, and were not loaded:"
             << endl;
        for (auto compoundID : _db.invalidRows) {
            cout << " - " << compoundID << endl;
        }
    }

    cout << "Total Compounds Loaded : " << loadCount << endl;
}

void PeakDetectorCLI::loadSamples(vector<string>& filenames)
{
#ifndef __APPLE__
    double startLoadingTime = getTime();
#endif
    cout << "\nLoading samples" << endl;

    for (unsigned int i = 0; i < filenames.size(); i++) {
        mzSample* sample = new mzSample();
        sample->loadSample(filenames[i].c_str());
        sample->sampleName = mzUtils::cleanFilename(filenames[i]);
        sample->isSelected = true;
        if (sample->scans.size() >= 1) {
            mavenParameters->samples.push_back(sample);
            cout << endl
                 << "Loaded Sample : " << sample->getSampleName() << endl;
        } else {
            if (sample != NULL) {
                delete sample;
                sample = NULL;
            }
        }
    }

    if (mavenParameters->samples.size() == 0) {
        cout << "Nothing to process. Exiting…" << endl;
        exit(1);
    }

    sort(mavenParameters->samples.begin(),
         mavenParameters->samples.end(),
         mzSample::compSampleSort);

    cout << "LoadSamples done: loaded " << mavenParameters->samples.size()
         << " samples";

#ifndef __APPLE__
    cout << "\nExecution time (Sample loading) : "
         << getTime() - startLoadingTime << " seconds \n";
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

    qDebug()
        << "Sample cohort file prepared. Moving on to gsheet interface now…";
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
        qDebug() << "JSON file is required for using Polly applications. "
                    "Overriding existing settings to save JSON data file…";
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
                if (compound->charge > 0)
                    charpolarity = "+";
                if (compound->charge < 0)
                    charpolarity = "-";

                QStringList category;

                for (int i = 0; i < compound->category.size(); i++) {
                    category << QString(compound->category[i].c_str());
                }

                out << charpolarity << SEP;
                out << QString(compound->name.c_str()) << SEP;
                out << compound->mass << SEP;
                out << compound->charge << SEP;
                out << compound->precursorMz << SEP;
                out << compound->collisionEnergy << SEP;
                out << compound->productMz << SEP;
                out << compound->expectedRt << SEP;
                out << compound->id.c_str() << SEP;
                out << compound->formula.c_str() << SEP;
                out << compound->srmId.c_str() << SEP;
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
        cerr << "PollyPhi currently does not support purely MS2 data. "
             << "Please use an MS1 or DDA dataset."
             << endl;
        return true;
    }
    
    //Untargeted data is not compatible with any app atm
    if (mavenParameters->processAllSlices) {
        cerr << "Untargeted data is not supported on Polly. Please switch off"
             << " mass slicing and provide a compound database."
             << endl;
        return true;
    }

    //Unlabelled data is not the desired data for PollyPhi
    if (!mavenParameters->pullIsotopesFlag && 
        _currentPollyApp == PollyApp::PollyPhi) {
        cerr << "PollyPhi is used for the analysis of labeled data. Please "
             << "switch on isotope detection using paramater -f."
             << endl;
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
    cout << "\nwriteReport " << mavenParameters->allgroups.size() << " groups ";

    // reduce groups
    _groupReduction();

    // save files in the output dir if Polly arguments have not been provided
    if (_currentPollyApp == PollyApp::None || pollyArgs.isEmpty()) {
        // create an output folder
        mzUtils::createDir(mavenParameters->outputdir.c_str());
        string fileName = mavenParameters->outputdir + setName;

        // save Eic Json
        saveJson(fileName);

        // save Mzroll File
        saveMzRoll(fileName);

        // save output CSV
        saveCSV(fileName, false);
    } else {
        if (_incompatibleWithPollyApp())
            exit(0);

        // try uploading to Polly
        QMap<QString, QString> creds = _readCredentialsFromXml(pollyArgs);
        cout << "uploading to Polly now…" << endl;
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
        qDebug() << "CSV filename:" << csvFilename;
        int compoundDbStatus =
            prepareCompoundDbForPolly(compoundDbFilename + ".csv");
        QString readyStatus = _isReadyForPolly();
        if (!(readyStatus == "ready") || compoundDbStatus == 0) {
            qDebug() << "Error while preparing files for Polly:"
                     << readyStatus;
            return;
        }

        // save Eic Json
        saveJson(jsonFilename.toStdString());

        // save output CSV
        saveCSV(csvFilename.toStdString(), true);

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
                    qDebug() << "Sample cohort copy status:"
                             << statusSampleCopy;
                    _redirectTo = "relative_lcms_elmaven";
                } else {
                    qDebug() << "There was some problem with the sample cohort "
                                "file, you will be redirected to an interface "
                                "on Polly where you can make the cohort file "
                                "again.";
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
                cerr << "Unable to upload data to Polly." << endl;
            }

            // NOTE: Adding this to help clients of this CLI to be able to
            // access the redirection URL.
            QString fname = "polly_redirection_url.txt";
            QString fpath = QStandardPaths::writableLocation(
                                QStandardPaths::GenericConfigLocation)
                            + QDir::separator()
                            + fname;
            ofstream ofs(fpath.toStdString());
            ofs << redirectionUrl.toStdString() << endl;
            ofs.close();

            // if redirection URL is not empty then we can print it on console
            // and send the user an email with that URL
            if (!redirectionUrl.isEmpty()) {
                cerr << "Redirection URL: \""
                     << redirectionUrl.toStdString()
                     << "\""
                     << endl;

                bool response = _sendUserEmail(creds, redirectionUrl);
                string status = response == 1 ? "success"
                                              : "failure";
                cerr << "Emailer status: " << status << "!" << endl;
            } else {
                cerr << "Unable to obtain a redirection URL." << endl;
            }
        } catch (...) {
            cerr << "Unable to upload data to Polly. "
                    "Please check the CLI arguments."
                 << endl;
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
            cerr << "Unable to create workflow request id. "
                    "Please try again."
                 << endl;
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
        cout << "\tExecution time (Group reduction) : "
             << getTime() - startGroupReduction << " seconds \n";
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
        _jsonReports->saveMzEICJson(
            fileName, mavenParameters->allgroups, mavenParameters->samples);
#ifndef __APPLE__
        cout << "\tExecution time (Saving Eic Json) : "
             << getTime() - startSavingJson << " seconds \n";
#endif
    }
}

QMap<QString, QString> PeakDetectorCLI::_readCredentialsFromXml(QString filename)
{
    QMap<QString, QString> creds;
    QDomDocument doc;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file)) {
        qDebug() << "Something is wrong with your credentials file. Please try "
                    "again with a valid xml file.";
        return creds;
    }

    // Get the root element
    QDomElement docElem = doc.documentElement();

    // you could check the root tag name here if it matters
    QString rootTag = docElem.tagName();  // == credentials
    if (rootTag != "credentials") {
        qDebug() << "The root tag of your credentials file is not correct. "
                    "Please try again with a valid xml file.";
        return creds;
    }
    QDomNodeList polly_creds = doc.elementsByTagName("pollyaccountdetails");
    for (int i = 0; i < polly_creds.size(); i++) {
        QDomNode n = polly_creds.item(i);
        QDomElement username = n.firstChildElement("username");
        QDomElement password = n.firstChildElement("password");
        if (username.isNull() || password.isNull()) {
            qDebug() << "Both username and password must be provided in the "
                        "credentials file.Please try again";
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

    // In case of CLI, we don't want persistent login as of now, so deleting
    // existing Token everytime and starting afresh. In future if persistent
    // login is required in CLI, just delete the two lines below this comment.
    QFile pollyCredFile(_pollyIntegration->getCredFile());
    pollyCredFile.remove();

    ErrorStatus response = _pollyIntegration->activeInternet();
    if (response == ErrorStatus::Failure ||
        response == ErrorStatus::Error) {
        cerr << "No internet access. Please connect to the internet and try "
                "again"
             << endl;
        return uploadProjectId;
    }

    ErrorStatus loginResponse = _pollyIntegration->authenticateLogin(
        creds["polly_username"], creds["polly_password"]);
    if (loginResponse == ErrorStatus::Failure) {
        cerr << "Incorrect credentials. Please check." << endl;
        return uploadProjectId;
    } else if (loginResponse == ErrorStatus::Error) {
        cerr << "A server error was encountered. Please contact tech support "
                "at elmaven@elucidata.io if the problem persists."
             << endl;
        return uploadProjectId;
    }

    // User is logged in, now proceeding to upload…

    // This will list all the project corresponding to the user on polly…
    QVariantMap projectNamesId = _pollyIntegration->getUserProjects();
    QStringList keys = projectNamesId.keys();
    QString projectId;
    QString defaultprojectId;
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
        cerr << "Creating new project with name: \""
             << _pollyProject.toStdString()
             << "\""
             << endl;
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

void PeakDetectorCLI::saveMzRoll(string setName)
{
    if (saveMzrollFile == true) {
#ifndef __APPLE__
        double startSavingMzroll = getTime();
#endif

        writePeakTableXML(setName + ".mzroll");

#ifndef __APPLE__
        cout << "\tExecution time (Saving mzroll)   : "
             << getTime() - startSavingMzroll << " seconds \n";
#endif
    }
}

void PeakDetectorCLI::saveCSV(string setName, bool pollyExport)
{
#ifndef __APPLE__
    double startSavingCSV = getTime();
#endif

    string fileName = setName + ".csv";

    CSVReports* csvreports = new CSVReports(mavenParameters->samples, pollyExport);
    csvreports->setMavenParameters(mavenParameters);

    if (mavenParameters->allgroups.size() == 0) {
        cout << "Writing to CSV Failed: No Groups found" << endl;
        return;
    }

    if (fileName.empty())
        return;

    if (mavenParameters->samples.size() == 0)
        return;

    csvreports->setUserQuantType(quantitationType);

    auto ddaGroupAt =
        find_if(begin(mavenParameters->allgroups),
                end(mavenParameters->allgroups),
                [](PeakGroup& group) {
                    if (!group.compound)
                        return false;
                    return group.compound->type() == Compound::Type::PRM;
                });
    bool ddaGroupExists = ddaGroupAt != end(mavenParameters->allgroups);

    // Added to pass into csvreports file when merged with Maven776 - Kiran
    // CLI exports the default Group Summary Matrix Format (without set Names)
    csvreports->openGroupReport(fileName, ddaGroupExists);

    for (int i = 0; i < mavenParameters->allgroups.size(); i++) {
        PeakGroup& group = mavenParameters->allgroups[i];
        csvreports->addGroup(&group);
    }
    csvreports->closeFiles();

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
        csvreports->openGroupReport(fileName, ddaGroupExists);
        csvreports->groupId = 0;

        sort(groupsWithMissingLabels.begin(), groupsWithMissingLabels.end());
        for (int i = 0; i < mavenParameters->allgroups.size(); i++) {
            PeakGroup& group = mavenParameters->allgroups[i];
            // `csvreports->groupId` is incremented with each group added, we
            // can use this to check if the group to be added next is present
            // within groupsWithMissingLabels or not.
            if (binary_search(groupsWithMissingLabels.begin(),
                              groupsWithMissingLabels.end(),
                              csvreports->groupId + 1)) {
                PeakGroup newGroup = group;
                Compound* compound = group.getCompound();
                float compoundMz = MassCalculator::computeMass(
                    compound->formula, mavenParameters->getCharge(compound));
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
        csvreports->closeFiles();
    }

    if (csvreports->getErrorReport() != "") {
        cout << endl
             << "Writing to CSV Failed : "
             << csvreports->getErrorReport().toStdString() << endl;
    }

#ifndef __APPLE__
    cout << "\tExecution time (Saving CSV)      : "
         << getTime() - startSavingCSV << " seconds \n";
#endif
}

void PeakDetectorCLI::reduceGroups()
{
    sort(mavenParameters->allgroups.begin(),
         mavenParameters->allgroups.end(),
         PeakGroup::compMz);
    cout << "\nreduceGroups(): " << mavenParameters->allgroups.size();
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
    cout << "\nReduced count of groups : " << reducedGroupCount << " \n";
    mavenParameters->allgroups = allgroups_;
    cout << "Done final group count(): " << mavenParameters->allgroups.size()
         << endl;
}

void PeakDetectorCLI::writeSampleListXML(xml_node& parent)
{
    xml_node samplesset = parent.append_child();
    samplesset.set_name("samples");

    for (int i = 0; i < _samples.size(); i++) {
        xml_node _sample = samplesset.append_child();
        _sample.set_name("sample");
        _sample.append_attribute("name") = _samples[i]->sampleName.c_str();
        _sample.append_attribute("filename") = _samples[i]->fileName.c_str();
        _sample.append_attribute("sampleOrder") = i;
        _sample.append_attribute("setName") = "";
        _sample.append_attribute("sampleName") = _samples[i]->sampleName.c_str();
    }
}

void PeakDetectorCLI::writePeakTableXML(std::string filename)
{
    xml_document doc;
    doc.append_child().set_name("project");
    xml_node peak = doc.child("project");
    peak.append_attribute("Program") = "peakdetector";
    peak.append_attribute("Version") = "Mar112012";

    writeSampleListXML(peak);
    writeParametersXML(peak);

    if (mavenParameters->allgroups.size()) {
        peak.append_child().set_name("PeakGroups");
        xml_node peakgroups = peak.child("PeakGroups");
        for (int i = 0; i < mavenParameters->allgroups.size(); i++) {
            writeGroupXML(peakgroups, &mavenParameters->allgroups[i]);
        }
    }

    doc.save_file(filename.c_str());
}

void PeakDetectorCLI::writeParametersXML(xml_node& parent)
{
    xml_node p = parent.append_child();
    p.set_name("PeakDetectionParameters");
    p.append_attribute("alignSamples") = mavenParameters->alignSamplesFlag;
    p.append_attribute("matchRt") = mavenParameters->matchRtFlag;
    p.append_attribute("ligandDbFilename") =
        mavenParameters->ligandDbFilename.c_str();
    p.append_attribute("clsfModelFilename") = clsfModelFilename.c_str();
    p.append_attribute("rtStepSize") = mavenParameters->rtStepSize;
    p.append_attribute("massCutoffMerge") =
        mavenParameters->massCutoffMerge->getMassCutoff();
    p.append_attribute("eic_smoothingWindow") =
        mavenParameters->eic_smoothingWindow;
    p.append_attribute("grouping_maxRtWindow") =
        mavenParameters->grouping_maxRtWindow;
    p.append_attribute("minGoodGroupCount") =
        mavenParameters->minGoodGroupCount;
    p.append_attribute("minSignalBlankRatio") =
        mavenParameters->minSignalBlankRatio;
    p.append_attribute("minPeakWidth") = mavenParameters->minNoNoiseObs;
    p.append_attribute("minSignalBaseLineRatio") =
        mavenParameters->minSignalBaseLineRatio;
    p.append_attribute("minGroupIntensity") =
        mavenParameters->minGroupIntensity;
    p.append_attribute("minQuality") = mavenParameters->minQuality;
}

void PeakDetectorCLI::writeGroupXML(xml_node& parent, PeakGroup* g)
{
    if (!g)
        return;

    xml_node group = parent.append_child();
    group.set_name("PeakGroup");

    group.append_attribute("groupId") = g->groupId;
    group.append_attribute("tagString") = g->tagString.c_str();
    group.append_attribute("metaGroupId") = g->metaGroupId;
    group.append_attribute("clusterId") = g->clusterId;
    group.append_attribute("expectedRtDiff") = g->expectedRtDiff;
    group.append_attribute("groupRank") = (float)g->groupRank;
    group.append_attribute("label") = g->label;
    group.append_attribute("type") = (int)g->type();
    group.append_attribute("changeFoldRatio") = (int)g->changeFoldRatio;
    group.append_attribute("changePValue") = (int)g->changePValue;
    if (g->srmId.length())
        group.append_attribute("srmId") = g->srmId.c_str();

    if (g->hasCompoundLink()) {
        Compound* c = g->compound;
        group.append_attribute("compoundId") = c->id.c_str();
        group.append_attribute("compoundDB") = c->db.c_str();
    }

    for (int j = 0; j < g->peaks.size(); j++) {
        Peak& p = g->peaks[j];
        xml_node peak = group.append_child();
        peak.set_name("Peak");
        peak.append_attribute("pos") = ((int)p.pos);
        peak.append_attribute("minpos") = (int)p.minpos;
        peak.append_attribute("maxpos") = (int)p.maxpos;
        peak.append_attribute("splineminpos") = (int)p.splineminpos;
        peak.append_attribute("splinemaxpos") = (int)p.splinemaxpos;
        peak.append_attribute("rt") = p.rt;
        peak.append_attribute("rtmin") = p.rtmin;
        peak.append_attribute("rtmax") = p.rtmax;
        peak.append_attribute("mzmin") = p.mzmin;
        peak.append_attribute("mzmax") = p.mzmax;
        peak.append_attribute("scan") = (int)p.scan;
        peak.append_attribute("minscan") = (int)p.minscan;
        peak.append_attribute("maxscan") = (int)p.maxscan;
        peak.append_attribute("peakArea") = p.peakArea;
        peak.append_attribute("peakSplineArea") = p.peakSplineArea;
        peak.append_attribute("peakAreaCorrected") = p.peakAreaCorrected;
        peak.append_attribute("peakAreaTop") = p.peakAreaTop;
        peak.append_attribute("peakAreaTopCorrected") = p.peakAreaTopCorrected;
        peak.append_attribute("peakAreaFractional") = p.peakAreaFractional;
        peak.append_attribute("peakRank") = p.peakRank;
        peak.append_attribute("peakIntensity") = p.peakIntensity;
        peak.append_attribute("peakBaseLineLevel") = p.peakBaseLineLevel;
        peak.append_attribute("peakMz") = p.peakMz;
        peak.append_attribute("medianMz") = p.medianMz;
        peak.append_attribute("baseMz") = p.baseMz;
        peak.append_attribute("quality") = p.quality;
        peak.append_attribute("width") = (int)p.width;
        peak.append_attribute("gaussFitSigma") = p.gaussFitSigma;
        peak.append_attribute("gaussFitR2") = p.gaussFitR2;
        peak.append_attribute("groupNum") = p.groupNum;
        peak.append_attribute("noNoiseObs") = (int)p.noNoiseObs;
        peak.append_attribute("noNoiseFraction") = p.noNoiseFraction;
        peak.append_attribute("symmetry") = p.symmetry;
        peak.append_attribute("signalBaselineRatio") = p.signalBaselineRatio;
        peak.append_attribute("groupOverlap") = p.groupOverlap;
        peak.append_attribute("groupOverlapFrac") = p.groupOverlapFrac;
        peak.append_attribute("localMaxFlag") = p.localMaxFlag;
        peak.append_attribute("fromBlankSample") = p.fromBlankSample;
        peak.append_attribute("label") = p.label;
        peak.append_attribute("sample") = p.getSample()->sampleName.c_str();
    }

    if (g->childCount()) {
        for (int i = 0; i < g->children.size(); i++) {
            PeakGroup* child = &(g->children[i]);
            writeGroupXML(group, child);
        }
    }
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
