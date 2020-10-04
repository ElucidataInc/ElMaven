#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <QDir>
#include <QCoreApplication>
#include <QProcess>
#include <QJsonObject>

#include "Compound.h"
#include "alignmentdialog.h"
#include "common/analytics.h"
#include "backgroundopsthread.h"
#include "database.h"
#include "groupFiltering.h"
#include "grouprtwidget.h"
#include "mainwindow.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "mzAligner.h"
#include "mzSample.h"
#include "obiwarp.h"
#include "peakdetector.h"
#include "samplertwidget.h"
#include "EIC.h"
#include "csvreports.h"
#include "pollyintegration.h"
#include "json.hpp"

using json = nlohmann::json;

BackgroundOpsThread::BackgroundOpsThread(QWidget*)
{
    mainwindow = NULL;
    setTerminationEnabled(true);
    runFunction = "";
    mavenParameters = nullptr;
    peakDetector = nullptr;
    setPeakDetector(new PeakDetector());

    _isotopeFormula = "";
    _isotopeCharge = 0;
    _parentGroup = nullptr;
    _performPolyFitAlignment = false;
    _dlManager = new DownloadManager();
    _pollyIntegration = new PollyIntegration(_dlManager);
}

BackgroundOpsThread::~BackgroundOpsThread()
{
    delete peakDetector;
}

/**
 * BackgroundOpsThread::run This function starts the thread. This function is
 * called by start() internally in QTThread. start() function will be called
 * where the thread starts.
 */
void BackgroundOpsThread::run(void)
{
    // Making sure that instance of the mainwindow is present so that the
    // peakdetection process can be run
    if (mainwindow == NULL) {
        quit();
        return;
    }

    connect(this,
            SIGNAL(alignmentError(QString)),
            mainwindow,
            SLOT(showAlignmentErrorDialog(QString)));
    if (mavenParameters->alignSamplesFlag) {
        connect(this,
                SIGNAL(alignmentComplete(QList<PeakGroup>)),
                mainwindow,
                SLOT(showAlignmentWidget()));
    }

    qRegisterMetaType<QList<PeakGroup>>("QList<PeakGroup>");

    connect(this,
            SIGNAL(alignmentComplete(QList<PeakGroup>)),
            mainwindow,
            SLOT(plotAlignmentVizAllGroupGraph(QList<PeakGroup>)));
    connect(this,
            SIGNAL(alignmentComplete(QList<PeakGroup>)),
            mainwindow->groupRtWidget,
            SLOT(setCurrentGroups(QList<PeakGroup>)));
    connect(this,
            SIGNAL(alignmentComplete(QList<PeakGroup>)),
            mainwindow->sampleRtWidget,
            SLOT(plotGraph()));

    mavenParameters->stop = false;

    if (runFunction == "alignWithObiWarp") {
        alignWithObiWarp();
    } else if (runFunction == "findFeatures") {
        findFeatures();
    } else if (runFunction == "computePeaks") {
        computePeaks();
    } else if (runFunction == "pullIsotopesForFormula") {
        pullIsotopesForFormula(_isotopeFormula,
                               _isotopeCharge);
    } else if (runFunction == "pullIsotopesForGroup") {
        pullIsotopesForGroup(_parentGroup.get());
    } else if (runFunction == "pullIsotopesForBarPlot") {
        pullIsotopesForBarPlot(_parentGroup.get());
    } else {
        qWarning() << QString("Unknown function: \"%1\"")
                          .arg(runFunction.c_str());
    }

    quit();
    return;
}

void BackgroundOpsThread::setPeakDetector(PeakDetector *pd)
{
    if (peakDetector != nullptr)
        delete peakDetector;

    peakDetector = pd;
    peakDetector->boostSignal.connect(boost::bind(&BackgroundOpsThread::qtSlot,
                                                  this,
                                                  _1,
                                                  _2,
                                                  _3));
}

void BackgroundOpsThread::qtSlot(const string& progressText,
                                  unsigned int progress,
                                  int totalSteps)
{
    emit updateProgressBar(QString::fromStdString(progressText),
                           progress,
                           totalSteps);
}

void BackgroundOpsThread::qtSignalSlot(const string& progressText,
                                        unsigned int completed_slices,
                                        int total_slices)
{
    emit updateProgressBar(QString::fromStdString(progressText),
                           completed_slices,
                           total_slices);
}

void BackgroundOpsThread::writeCSVRep(string setName)
{
    // int lastUniqueId = 0;
    // for (auto&group : mavenParameters->allgroups)
    //     group.setUniqueId(++lastUniqueId);

    // for(auto&group : mavenParameters->allgroups)
    //     for (auto& child : group.childIsotopes())
    //         child->setUniqueId(++lastUniqueId);

    if (mainwindow->mavenParameters->peakMl) 
        classifyGroups(mavenParameters->allgroups);

    emitGroups();

    Q_EMIT(updateProgressBar("Done", 1, 1));
}

void BackgroundOpsThread::align()
{
    // these if-else statements will take care of all corner cases of
    // "undo alignment"
    if (mavenParameters->alignSamplesFlag && mavenParameters->alignButton > 0) {
        ;
    } else if (mavenParameters->alignSamplesFlag
               && mavenParameters->alignButton == 0) {
        mavenParameters->alignButton++;
        mavenParameters->undoAlignmentGroups = mavenParameters->allgroups;
    } else if (mavenParameters->alignSamplesFlag
               && mavenParameters->alignButton == -1) {
        ;
    } else {
        mavenParameters->alignButton = -1;
        mavenParameters->undoAlignmentGroups = mavenParameters->allgroups;
    }

    if (mavenParameters->alignSamplesFlag && !mavenParameters->stop) {
        emit updateProgressBar("Aligning samples…", 0, 0);
        vector<PeakGroup*> groups(mavenParameters->allgroups.size());
        for (int i = 0; i < mavenParameters->allgroups.size(); i++)
            groups[i] = &mavenParameters->allgroups[i];
        Aligner aligner;
        int alignAlgo = mainwindow->alignmentDialog->alignAlgo->currentIndex();

        if (alignAlgo == 1) {
            mainwindow->getAnalytics()->hitEvent("Alignment", "PolyFit");
            aligner.setMaxIterations(
                mainwindow->alignmentDialog->maxIterations->value());
            aligner.setPolymialDegree(
                mainwindow->alignmentDialog->polynomialDegree->value());
            aligner.doAlignment(groups);
            mainwindow->sampleRtWidget->setDegreeMap(aligner.sampleDegree);
            mainwindow->sampleRtWidget->setCoefficientMap(
                aligner.sampleCoefficient);
        }

        mainwindow->deltaRt = aligner.getDeltaRt();
        mavenParameters->alignSamplesFlag = false;
    }
    QList<PeakGroup> listGroups;
    for (unsigned int i = 0; i < mavenParameters->allgroups.size(); i++) {
        listGroups.append(mavenParameters->allgroups.at(i));
    }

    emit alignmentComplete(listGroups);
    emit samplesAligned(true);
}

void BackgroundOpsThread::alignWithObiWarp()
{
    ObiParams* obiParams = new ObiParams(
        mainwindow->alignmentDialog->scoreObi->currentText().toStdString(),
        false,
        mainwindow->alignmentDialog->factorDiag->value(),
        mainwindow->alignmentDialog->factorGap->value(),
        mainwindow->alignmentDialog->gapInit->value(),
        mainwindow->alignmentDialog->gapExtend->value(),
        0.0f,
        mainwindow->alignmentDialog->responseObiWarp->value(),
        false,
        mainwindow->alignmentDialog->binSizeObiWarp->value());

    vector <mzSample*> samplesToAlign;
    int countMs2Samples = 0;
    for (auto sample : mavenParameters->samples) {
        if (sample->ms1ScanCount() == 0 && sample->ms2ScanCount()) {
            countMs2Samples++;
        } else {
            samplesToAlign.push_back(sample);
        }
    }

    if (countMs2Samples == mavenParameters->samples.size()) {
        auto htmlText = QString("The loaded samples contain purely MS2 data. "
                                "Alignment cannot be performed for such samples "
                                "and will therefore be aborted. Currently, El-MAVEN " 
                                "supports alignment of full-scan MS1 and DDA data only.");
        emit alignmentError(htmlText);
        return;
    }

    emit updateProgressBar("Aligning samples…", 0, 100);

    Aligner aligner;
    aligner.setAlignmentProgress.connect(
        boost::bind(&BackgroundOpsThread::qtSlot, this, _1, _2, _3));

    auto stopped = aligner.alignWithObiWarp(samplesToAlign,
                                            obiParams,
                                            mavenParameters);
    delete obiParams;

    if (stopped) {
        emit restoreAlignment();

        // restore previous RTs
        for (auto sample : samplesToAlign)
            sample->restorePreviousRetentionTimes();

        // stopped without user intervention
        if (!mavenParameters->stop) {
            emit alignmentError(
                QString("There was an error during alignment. "
                        "Please try again."));
        }

        mavenParameters->stop = false;
        return;
    }

    mainwindow->sampleRtWidget->plotGraph();
    emit samplesAligned(true);
}

void BackgroundOpsThread::emitGroups()
{
    for (PeakGroup& group : mavenParameters->allgroups) {
        if (mavenParameters->keepFoundGroups) {
            emit newPeakGroup(&group);
            QCoreApplication::processEvents();
        }
    }
}

void BackgroundOpsThread::computePeaks()
{
    vector<Compound*> set = mavenParameters->compounds;
    if (set.size() == 0)
        return;

    emit updateProgressBar("Processing Compounds", 0, 0);

    bool hadPullIsotopes = peakDetector->mavenParameters()->pullIsotopesFlag;
    bool hadSearchAdducts = peakDetector->mavenParameters()->searchAdducts;
    if (_performPolyFitAlignment) {
        peakDetector->mavenParameters()->pullIsotopesFlag = false;
        peakDetector->mavenParameters()->searchAdducts = false;
    }
    peakDetector->processCompounds(set);
    if (_performPolyFitAlignment) {
        align();
        peakDetector->mavenParameters()->pullIsotopesFlag = hadPullIsotopes;
        peakDetector->mavenParameters()->searchAdducts = hadSearchAdducts;
    }
    
    writeCSVRep("slices");

    emit updateProgressBar("Status", 0, 100);
}

void BackgroundOpsThread::findFeatures()
{
    emit updateProgressBar("Computing Mass Slices", 0, 0);
    mavenParameters->sig.connect(
        boost::bind(&BackgroundOpsThread::qtSignalSlot, this, _1, _2, _3));

    bool hadPullIsotopes = peakDetector->mavenParameters()->pullIsotopesFlag;
    bool hadSearchAdducts = peakDetector->mavenParameters()->searchAdducts;
    if (_performPolyFitAlignment) {
        peakDetector->mavenParameters()->pullIsotopesFlag = false;
        peakDetector->mavenParameters()->searchAdducts = false;
    }
    peakDetector->processFeatures(mavenParameters->compounds);
    if (_performPolyFitAlignment) {
        align();
        peakDetector->mavenParameters()->pullIsotopesFlag = hadPullIsotopes;
        peakDetector->mavenParameters()->searchAdducts = hadSearchAdducts;
    }
    
    writeCSVRep("allSlices");

    emit updateProgressBar("Status", 0, 100);
}

void BackgroundOpsThread::completeStop()
{
    peakDetector->resetProgressBar();
    mavenParameters->stop = true;
}

void BackgroundOpsThread::setRunFunction(QString functionName)
{
    runFunction = functionName.toStdString();
}

void BackgroundOpsThread::pullIsotopesForFormula(string formula, int charge)
{
    if (!mavenParameters->pullIsotopesFlag || formula.empty())
        return;

    Compound tempCompound("tmp_id", "tmp_name", formula, charge);
    peakDetector->processCompounds({&tempCompound}, false);
}

void BackgroundOpsThread::pullIsotopesForGroup(PeakGroup* parentGroup)
{
    peakDetector->detectIsotopesForParent(*parentGroup);
}

void BackgroundOpsThread::pullIsotopesForBarPlot(PeakGroup* parentGroup)
{
    peakDetector->detectIsotopesForParent(*parentGroup, true);
}

void BackgroundOpsThread::updateGroups(QList<shared_ptr<PeakGroup>>& groups,
                                       vector<mzSample*> samples)
{
    auto updateGroup = [samples](PeakGroup* group) {
        MavenParameters* mp = group->parameters().get();
        auto slice = group->getSlice();
        slice.rtmin = samples[0]->minRt;
        slice.rtmax = samples[0]->maxRt;

        auto eics  = PeakDetector::pullEICs(&slice, samples, mp);
        for(auto eic : eics) {
            for(Peak& peak :  group->peaks) {
                if (eic->getSample() == peak.getSample()) {
                    eic->adjustPeakBounds(peak, group->minRt, group->maxRt);
                    eic->getPeakDetails(peak);
                }
                if (mp->clsf->hasModel())
                    mp->clsf->scorePeak(peak);
            }
        }
        group->updateQuality();
        group->groupStatistics();
        delete_all(eics);
    };

    for(auto group : groups) {
        updateGroup(group.get());
        for (auto& child : group->childIsotopes())
            updateGroup(child.get());
        for (auto& child : group->childAdducts())
            updateGroup(child.get());
    }
}

void BackgroundOpsThread::classifyGroups(vector<PeakGroup>& groups)
{
    if (groups.empty())
        return;
    
    auto tempDir = QStandardPaths::writableLocation(
                       QStandardPaths::GenericConfigLocation)
                   + QDir::separator()
                   + "ElMaven";

    // TODO: binary name will keep changing and should not be hardcoded
    // TODO: model should not exist anywhere on the filesystem
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    auto mlBinary = tempDir + QDir::separator() + "moi";
#endif
#ifdef Q_OS_WIN
    auto mlBinary = tempDir + QDir::separator() + "moi.exe";
#endif
    QString mlModel;
    if(mainwindow->mavenParameters->peakMlModelType == "Global Model Elucidata"){
        mlModel = tempDir + QDir::separator() + "model.pickle.dat";
    }
    else{
        return;
    }

    if (!QFile::exists(mlBinary)) {
        bool downloadSuccess = downloadPeakMlFilesFromURL("moi");
        if(!downloadSuccess) {
            cerr << "Error: ML binary not found at path: "
                << mlBinary.toStdString()
                << endl;
            return;
        }
    }
    if (!QFile::exists(mlModel)) {
        bool downloadSuccess = downloadPeakMlFilesFromURL("model.pickle.dat");
        if(!downloadSuccess) {
            cerr << "Error: ML model not found at path: "
                << mlModel.toStdString()
                << endl;
            return;
        }
    }
    
    Q_EMIT(updateProgressBar("Classifying peaks…", 0, 0));
    
    QString peakAttributesFile = tempDir
                                 + QDir::separator()
                                 + "peak_ml_input.csv";
    QString classificationOutputFile = tempDir
                                       + QDir::separator()
                                       + "peak_ml_output.csv";

    // // have to enumerated and assign each group with an ID, because multiple
    // // groups at this point have the same ID
    int startId = 1;
    for (auto& group : groups) {
        group.setGroupId(startId++);
        for (auto& child : group.childIsotopes()) {
            child->setGroupId(startId++);
        }
    }

    CSVReports::writeDataForPeakMl(peakAttributesFile.toStdString(),
                                   groups);
    if (!QFile::exists(peakAttributesFile)) {
        cerr << "Error: peak attributes input file not found at path: "
             << peakAttributesFile.toStdString()
             << endl;
        return;
    }
    QString badGroupLimit = QString::fromStdString(
                                mzUtils::float2string(mavenParameters->badGroupLimit, 1));
    QString maybeGoodGroupLimit = QString::fromStdString(
                                    mzUtils::float2string(mavenParameters->maybeGoodGroupLimit, 1));
    QStringList mlArguments;
    mlArguments << "--input_attributes_file" << peakAttributesFile
                << "--output_moi_file" << classificationOutputFile
                << "--model_path" << mlModel
                << "--bad_group_limit" << badGroupLimit
                << "--maybeGood_group_limit" << maybeGoodGroupLimit;
    QProcess subProcess;
    subProcess.setWorkingDirectory(QFileInfo(mlBinary).path());
    subProcess.start(mlBinary, mlArguments);
    subProcess.waitForFinished(-1);
    if (!QFile::exists(classificationOutputFile)) {
        qDebug() << "MOI stdout:" << subProcess.readAllStandardOutput();
        qDebug() << "MOI stderr:" << subProcess.readAllStandardError();
        cerr << "Error: peak classification output file not found at path: "
             << peakAttributesFile.toStdString()
             << endl;
        return;
    }

    QFile file(classificationOutputFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        cerr << "Error: failed to open classification output file" << endl;
        return;
    }

    map<int, pair<int, float>> predictions;
    map<int, multimap<float, string>> inferences;
    map<int, map<int, float>> correlations;
    map<string, int> headerColumnMap;
    map<string, int> attributeHeaderColumnMap;
    vector<string> headers;
    vector<string> knownHeaders = {"groupId",
                                   "label",
                                   "probability",
                                   "correlations"};
    while (!file.atEnd()) {
        string line = file.readLine().trimmed().toStdString();
        if (line.empty())
            continue;

        auto fields = mzUtils::split(line, ",");
        for (auto& field : fields) {
            int n = field.length();
            if (n > 2 && field[0] == '"' && field[n-1] == '"') {
                field = field.substr(1, n-2);
            }
            if (n > 2 && field[0] == '\'' && field[n-1] == '\'') {
                field = field.substr(1, n-2);
            }
        }

        if (headers.empty()) {
            headers = fields;
            for(size_t i = 0; i < fields.size(); ++i) {
                if (find(begin(knownHeaders),
                         end(knownHeaders),
                         fields[i]) != knownHeaders.end()) {
                    headerColumnMap[fields[i]] = i;
                } else {
                    attributeHeaderColumnMap[fields[i]] = i;
                }
            }
            continue;
        }

        int groupId = -1;
        int label = -1;
        float probability = 0.0f;
        map<int, float> group_correlations;
        if (headerColumnMap.count("groupId"))
            groupId = string2integer(fields[headerColumnMap["groupId"]]);
        if (headerColumnMap.count("label"))
            label = string2integer(fields[headerColumnMap["label"]]);
        if (headerColumnMap.count("probability"))
            probability = string2float(fields[headerColumnMap["probability"]]);
        if (headerColumnMap.count("correlations")) {
            QString correlations_str =
                QString::fromStdString(fields[headerColumnMap["correlations"]]);
            if (correlations_str != "[]") {
                // trim brackets from start and end
                auto size = correlations_str.size();
                correlations_str = correlations_str.mid(2, size - 4);

                // obtain individual {group ID <-> correlation score} pair
                QStringList pair_strings = correlations_str.split(") (");
                for (auto& pair_str : pair_strings) {
                    QStringList pair = pair_str.split(" ");
                    auto corrGroupId = string2integer(pair.at(0).toStdString());
                    auto corrScore = string2float(pair.at(1).toStdString());
                    group_correlations[corrGroupId] = corrScore;
                }
            }
        }

        if (groupId != -1) {
            predictions[groupId] = make_pair(label, probability);

            // we use multimap for values mapping to attribute names because
            // later on sorted values are really helpful
            multimap<float, string> groupInference;
            for (auto& element : attributeHeaderColumnMap) {
                string attribute = element.first;
                float value = string2float(fields[element.second]);
                groupInference.insert(make_pair(value, attribute));
            }
            inferences[groupId] = groupInference;
            correlations[groupId] = group_correlations;
        }
    }

    auto assignPrediction = [&] (PeakGroup* group, 
                                 map<int, pair<int, float>> predictions,
                                 map<int, multimap<float, string>> inferences,
                                 map<int, map<int, float>> correlations) {
                                if (predictions.count(group->groupId())) {
                                    pair<int, float> prediction = predictions.at(group->groupId());
                                    group->setPredictedLabel(
                                        PeakGroup::classificationLabelForValue(prediction.first),
                                        prediction.second);
                                }   
                                if (inferences.count(group->groupId()))
                                    group->setPredictionInference(inferences.at(group->groupId()));

                                // add correlated groups
                                if (correlations.count(group->groupId()) == 0)
                                    return;

                                auto& group_correlations = correlations.at(group->groupId());
                                for (auto& elem : group_correlations)
                                    group->addCorrelatedGroup(elem.first, elem.second);
                                
                            };
    
    for (auto& group : groups) {
        assignPrediction(&group, 
                         predictions, 
                         inferences, 
                         correlations);
    }
    for (auto& group : groups) {
        for (auto& child : group.childIsotopes()) {
            assignPrediction(child.get(), 
                             predictions, 
                             inferences, 
                             correlations);
        } 
    }
    removeFiles();
}

bool BackgroundOpsThread::downloadPeakMlFilesFromURL(QString fileName) {
 
    QStringList args;

    Q_EMIT(updateProgressBar("Configuring PeakML...", 0, 0));
    auto tempDir = QStandardPaths::writableLocation(
                    QStandardPaths::GenericConfigLocation)
                    + QDir::separator()
                    + "ElMaven";

    if(fileName == "moi")
    {
        #if defined(Q_OS_MAC)
            args << "Mac/moi";
            tempDir = tempDir + QDir::separator() + "moi";
        #endif

        #if defined(Q_OS_LINUX)
            args << "Unix/moi";
            tempDir = tempDir + QDir::separator() + "moi";
        #endif
        
        #ifdef Q_OS_WIN
            args << "Windows/moi.exe";
            tempDir = tempDir + QDir::separator() + "moi.exe" ;
        #endif
    } else {
        args << "global_model_elucidata.dat";
        tempDir = tempDir + QDir::separator() + "model.pickle.dat";
    }

    auto cookieFile = QStandardPaths::writableLocation(
                    QStandardPaths::GenericConfigLocation)
                    + QDir::separator() 
                    + "cookie.json" ;
    
    ifstream readCookie(cookieFile.toStdString());
    json cookieInput = json::parse(readCookie);
    string refreshToken = cookieInput["refreshToken"];
    string idToken = cookieInput["idToken"];

    QString cookies;
    cookies += "refreshToken=";
    cookies += QString::fromStdString(refreshToken);
    cookies += ";";
    cookies += "idToken=";
    cookies += QString::fromStdString(idToken);
    
    args << cookies;

    auto res = _pollyIntegration->runQtProcess("downloadFilesForPeakML", args);
    QString str(res[0].constData());
    auto splitString = mzUtils::split(str.toStdString(), "\n");
    auto data = splitString[splitString.size() - 2];
    auto dataQstring = QString::fromStdString(data);
    QJsonObject dataObj;

    QJsonDocument doc = QJsonDocument::fromJson(dataQstring.toUtf8());
    dataObj = doc.object();  
    
    auto dataValue = dataObj["data"].toObject();
    auto error = dataValue["error"].toString();
    if (!error.isEmpty()){

    }
    
    auto url = dataValue["id"].toString();
    
    _dlManager->setRequest(url, this, false);
    if (!_dlManager->err) {
        auto downloadedData = _dlManager->getData();
        QFile file(tempDir);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(downloadedData);
        }
    } else {
        cerr <<  "\n\nError occured while downloading" << endl;
    }
   
    if (!QFile::exists(tempDir)) {
        return false;
    } else {
        if(fileName == "moi")
        {
            #if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
                changeMode(tempDir.toStdString());
            #endif 
        }
        return true;
    }
}

void BackgroundOpsThread::changeMode(string fileName)
{
    string command = "chmod 755 " + fileName;
    system(command.c_str());
}

void BackgroundOpsThread::removeFiles()
{
    auto tempDir = QStandardPaths::writableLocation(
                   QStandardPaths::GenericConfigLocation)
                   + QDir::separator()
                   + "ElMaven" 
                   + QDir::separator()
                   + "model.pickle.dat";
    
    string fileName = tempDir.toStdString();

    if (QFile::exists(tempDir))
        remove(fileName.c_str());

    tempDir.clear();
    tempDir = QStandardPaths::writableLocation(
                   QStandardPaths::GenericConfigLocation)
                   + QDir::separator()
                   + "ElMaven" 
                   + QDir::separator()
                   + "peak_ml_input.csv";
    fileName = tempDir.toStdString();

    if (QFile::exists(tempDir))
        remove(fileName.c_str());

    tempDir.clear();
    tempDir = QStandardPaths::writableLocation(
                   QStandardPaths::GenericConfigLocation)
                   + QDir::separator()
                   + "ElMaven" 
                   + QDir::separator()
                   + "peak_ml_output.csv";
    fileName = tempDir.toStdString();

    if (QFile::exists(tempDir))
        remove(fileName.c_str());
}