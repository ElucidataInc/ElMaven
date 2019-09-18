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
#include "analytics.h"
#include "csvreports.h"
#include "background_peaks_update.h"
#include "database.h"
#include "grouprtwidget.h"
#include "isotopeDetection.h"
#include "mainwindow.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "mzAligner.h"
#include "mzSample.h"
#include "obiwarp.h"
#include "PeakDetector.h"
#include "samplertwidget.h"

BackgroundPeakUpdate::BackgroundPeakUpdate(QWidget*) {
        mainwindow = NULL;
        _stopped = true;
        setTerminationEnabled(true);
        runFunction = "computeKnowsPeaks";
        peakDetector = nullptr;
        setPeakDetector(new PeakDetector());
}

QString BackgroundPeakUpdate::printSettings() {

    QString buffer;
    QTextStream summary( &buffer, QIODevice::ReadWrite);
    summary << "-------------------SETTINGS-------------------"<< "\n"<< "\n";
//     summary << "runFunction =" << runFunction<< "\n";
    summary << "alignSamplesFlag="  <<  mavenParameters->alignSamplesFlag<< "\n";
    summary << "alignMaxIterations="  <<  mavenParameters->alignMaxIterations << "\n";
    summary << "alignPolynomialDegree="  <<  mavenParameters->alignPolynomialDegree << "\n";

    summary << "--------------------------------MASS SLICING"<< "\n";
    summary << "rtStepSize=" << mavenParameters->rtStepSize<< "\n";
    summary << "massCutoffMerge=" << mavenParameters->massCutoffMerge->getMassCutoff()<< "\n";
    summary << "limitGroupCount=" << mavenParameters->limitGroupCount<< "\n";

    summary << "minMz=" << mavenParameters->minMz << "\n";
    summary << "maxMz=" << mavenParameters->maxMz << "\n";

    summary << "minRt=" << mavenParameters->minRt << "\n";
    summary << "maxRt=" << mavenParameters->maxRt << "\n";

    summary << "minIntensity=" << mavenParameters->minIntensity << "\n";
    summary << "maxIntensity=" << mavenParameters->maxIntensity << "\n";

    summary << "minCharge=" << mavenParameters->minCharge << "\n";
    summary << "maxCharge=" << mavenParameters->maxCharge << "\n";
    summary << "------------------------------COMPOUND MATCHING"
            << "\n";

    summary << "ionizationMode=" << mavenParameters->ionizationMode << "\n";
    summary << "matchRtFlag=" << mavenParameters->matchRtFlag << "\n";
    summary << "compoundMassCutoffWindow=" << mavenParameters->compoundMassCutoffWindow->getMassCutoff()
            << "\n";
    summary << "compoundRTWindow=" << mavenParameters->compoundRTWindow << "\n";
    summary << "matchFragmentationOptions=" << mavenParameters->matchFragmentationFlag
            << "\n";
    summary << "fragmentTolerance=" << mavenParameters->fragmentTolerance
            << "\n";

    summary << "------------------------------EIC CONSTRUCTION"
            << "\n";
    summary << "eic_smoothingWindow=" << mavenParameters->eic_smoothingWindow
            << "\n";
    summary << "eic_smoothingAlgorithm="
            << mavenParameters->eic_smoothingAlgorithm << "\n";
    summary << "baseline_smoothingWindow="
            << mavenParameters->baseline_smoothingWindow << "\n";
    summary << "baseline_dropTopX=" << mavenParameters->baseline_dropTopX
            << "\n";
    summary << "------------------------------PEAK GROUPING"
            << "\n";

    summary << "grouping_maxRtWindow=" << mavenParameters->grouping_maxRtWindow
            << "\n";
    summary << "eicMaxGroups=" << mavenParameters->eicMaxGroups << "\n";
    summary << "------------------------------GROUP FILTERING"
            << "\n";

    summary << "minGoodPeakCount=" << mavenParameters->minGoodGroupCount
            << "\n";
    summary << "minSignalBlankRatio=" << mavenParameters->minSignalBlankRatio
            << "\n";
    summary << "minSignalBlankRatio=" << mavenParameters->minSignalBlankRatio
            << "\n";
    summary << "minNoNoiseObs=" << mavenParameters->minNoNoiseObs << "\n";
    summary << "minSignalBaseLineRatio="
            << mavenParameters->minSignalBaseLineRatio << "\n";
    summary << "minGroupIntensity=" << mavenParameters->minGroupIntensity
            << "\n";

    summary << "-----------------------------------OUTPUT"
            << "\n";
    summary << "outputdir="
            << QString::fromStdString(mavenParameters->outputdir) << "\n";
    summary << "writeCSVFlag=" << mavenParameters->writeCSVFlag << "\n";
    summary << "keepFoundGroups=" << mavenParameters->keepFoundGroups << "\n";
    summary << "showProgressFlag=" << mavenParameters->showProgressFlag << "\n";

    QString x = summary.readAll();
    qDebug() << x;
    return x;
}

void BackgroundPeakUpdate::saveSettings(QString fileName) {
    QFile file(fileName);
    if ( !file.open(QFile::WriteOnly) )return; //error

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartElement("Settings");
    stream.writeStartElement("PeakDetectionSettings");

    //     stream.writeAttribute("runFunction",  mavenParameters->runFunction);
    stream.writeAttribute( "alignSamplesFlag"  ,   QString::number(mavenParameters->alignSamplesFlag));

    stream.writeAttribute( "rtStepSize" ,QString::number( mavenParameters->rtStepSize));
    stream.writeAttribute( "massCutoffMerge" ,QString::number( mavenParameters->massCutoffMerge->getMassCutoff()));
    stream.writeAttribute( "limitGroupCount" ,QString::number( mavenParameters->limitGroupCount));

    stream.writeAttribute( "minMz" ,QString::number( mavenParameters->minMz));
    stream.writeAttribute( "maxMz" ,QString::number( mavenParameters->maxMz));

    stream.writeAttribute( "minRt" ,QString::number( mavenParameters->minRt));
    stream.writeAttribute( "maxRt" ,QString::number( mavenParameters->maxRt));

    stream.writeAttribute( "minIntensity" ,QString::number( mavenParameters->minIntensity));
    stream.writeAttribute( "maxIntensity" ,QString::number( mavenParameters->maxIntensity));

    stream.writeAttribute( "minCharge" ,QString::number(mavenParameters->minCharge));
    stream.writeAttribute( "maxCharge" ,QString::number(mavenParameters->maxCharge));

    stream.writeAttribute( "ionizationMode" ,QString::number(mavenParameters->ionizationMode));
    stream.writeAttribute( "matchRtFlag" ,QString::number( mavenParameters->matchRtFlag));
    stream.writeAttribute( "compoundMassCutoffWindow" ,QString::number( mavenParameters->compoundMassCutoffWindow->getMassCutoff()));
    stream.writeAttribute( "compoundRTWindow" ,QString::number( mavenParameters->compoundRTWindow));

    stream.writeAttribute( "eic_smoothingWindow" ,QString::number( mavenParameters->eic_smoothingWindow));
    stream.writeAttribute( "eic_smoothingAlgorithm" ,QString::number( mavenParameters->eic_smoothingAlgorithm));
    stream.writeAttribute( "baseline_smoothingWindow" ,QString::number( mavenParameters->baseline_smoothingWindow));
    stream.writeAttribute( "baseline_dropTopX" ,QString::number( mavenParameters->baseline_dropTopX));

    stream.writeAttribute( "grouping_maxRtWindow" ,QString::number( mavenParameters->grouping_maxRtWindow));
    stream.writeAttribute( "eicMaxGroups" ,QString::number( mavenParameters->eicMaxGroups));

    //     stream.writeAttribute( "minGoodPeakCount" ,QString::number( mavenParameters->minGoodPeakCount));
    stream.writeAttribute( "minSignalBlankRatio" ,QString::number( mavenParameters->minSignalBlankRatio));
    stream.writeAttribute( "minNoNoiseObs" ,QString::number( mavenParameters->minNoNoiseObs));
    stream.writeAttribute( "minSignalBaseLineRatio" ,QString::number( mavenParameters->minSignalBaseLineRatio));
    stream.writeAttribute( "minGroupIntensity" ,QString::number( mavenParameters->minGroupIntensity));

    stream.writeAttribute( "outputdir" , QString::fromStdString(mavenParameters->outputdir));
    stream.writeAttribute( "writeCSVFlag" ,QString::number( mavenParameters->writeCSVFlag));
    stream.writeAttribute( "keepFoundGroups" ,QString::number( mavenParameters->keepFoundGroups));
    stream.writeAttribute( "showProgressFlag" ,QString::number( mavenParameters->showProgressFlag));

    stream.writeEndElement();
    stream.writeEndElement();

    file.close();
}

void BackgroundPeakUpdate::loadSettings(QString fileName) {
    QFile file(fileName);
    if ( !file.open(QFile::ReadOnly) )return; //error

    QXmlStreamReader xml(&file);

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            QSettings* settings = mainwindow->getSettings();
            if (xml.name() == "PeakDetectionSettings") {
                settings->setValue("alignSamplesFlag", xml.attributes().value("alignSamplesFlag").toString().toInt());
                // EIC Processing: Baseline calculation and Smoothing

                // Isotope detection in peakdetection dialogue box
                // settings->setValue("checkBox", checkBox->isChecked());  // C13
                // settings->setValue("checkBox_2",
                //                    checkBox_2->isChecked());  // N15
                // settings->setValue("checkBox_3",
                //                    checkBox_3->isChecked());  // D2
                // settings->setValue("checkBox_4",
                //                   checkBox_4->isChecked());  // S34

                // Fragment Score
                // settings->setValue("minFragMatchScore",
                //                   minFragMatchScore->value());
                //settings->setValue("matchFragmentationOptions",
                //        xml.attributes().value("matchFragmentationFlag").toString().toInt());

                // Enabling feature detection or compound search
                // mavenParameters->runFunction =
                // xml.attributes().value("runFunction").toString();
                settings->setValue("limitGroupCount",
                        xml.attributes()
                        .value("limitGroupCount")
                        .toString()
                        .toInt());

                settings->setValue("fragmentTolerance",
                        xml.attributes()
                        .value("fragmentTolerance")
                        .toString()
                        .toFloat());

                settings->setValue(
                    "outputdir",
                    xml.attributes().value("outputdir").toString());
                settings->setValue(
                        "writeCSVFlag",
                        xml.attributes().value("writeCSVFlag").toString().toInt());
                settings->setValue("keepFoundGroups",
                        xml.attributes()
                        .value("keepFoundGroups")
                        .toString()
                        .toInt());
                settings->setValue("showProgressFlag", xml.attributes()
                        .value("showProgressFlag")
                        .toString()
                        .toInt());
            }
        }
    }
    file.close();
}

BackgroundPeakUpdate::~BackgroundPeakUpdate() {
    delete peakDetector;
    mavenParameters->cleanup();  // remove allgroups
}

/**
 * BackgroundPeakUpdate::run This function starts the thread. This function is
 * called by start() internally in QTThread. start() function will be called
 * where the thread starts.
 */
void BackgroundPeakUpdate::run(void) {
        //Making sure that instance of the mainwindow is present so that the
        //peakdetection process can be ran
        if (mainwindow == NULL) {
                quit();
                return;
        }
        connect(this, SIGNAL(alignmentError(QString)), mainwindow, SLOT(showAlignmentErrorDialog(QString)));
        if (mavenParameters->alignSamplesFlag) {
                connect(this, SIGNAL(alignmentComplete(QList<PeakGroup> )), mainwindow, SLOT(showAlignmentWidget()));
        }
	qRegisterMetaType<QList<PeakGroup> >("QList<PeakGroup>");
	connect(this, SIGNAL(alignmentComplete(QList<PeakGroup> )), mainwindow, SLOT(plotAlignmentVizAllGroupGraph(QList<PeakGroup>)));
	connect(this, SIGNAL(alignmentComplete(QList<PeakGroup> )), mainwindow->groupRtWidget, SLOT(setCurrentGroups(QList<PeakGroup>)));
        connect(this, SIGNAL(alignmentComplete(QList<PeakGroup> )), mainwindow->sampleRtWidget, SLOT(plotGraph()));
        mavenParameters->stop = false;
        started();

        if (runFunction == "alignUsingDatabase") {
                alignUsingDatabase();
        } else if (runFunction == "processSlices") {
                processSlices();
        } else if (runFunction == "processMassSlices") {
                processMassSlices();
        } else if (runFunction == "pullIsotopes") {
                pullIsotopes(mavenParameters->_group);
        } else if (runFunction == "pullIsotopesIsoWidget") {
                pullIsotopesIsoWidget(mavenParameters->_group);
        } else if (runFunction == "pullIsotopesBarPlot") {
                pullIsotopesBarPlot(mavenParameters->_group);
        } else if (runFunction == "computePeaks") {
                computePeaks();
        } else if(runFunction == "alignWithObiWarp" ){
                alignWithObiWarp();
        } else {
                qDebug() << "Unknown Function " << runFunction.c_str();
        }

        quit();
        return;
}
void BackgroundPeakUpdate::alignWithObiWarp()
{
    ObiParams *obiParams = new ObiParams(mainwindow->alignmentDialog->scoreObi->currentText().toStdString(),
                                         mainwindow->alignmentDialog->local->isChecked(),
                                         mainwindow->alignmentDialog->factorDiag->value(),
                                         mainwindow->alignmentDialog->factorGap->value(),
                                         mainwindow->alignmentDialog->gapInit->value(),
                                         mainwindow->alignmentDialog->gapExtend->value(),
                                         mainwindow->alignmentDialog->initPenalty->value(),
                                         mainwindow->alignmentDialog->responseObiWarp->value(),
                                         mainwindow->alignmentDialog->noStdNormal->isChecked(),
                                         mainwindow->alignmentDialog->binSizeObiWarp->value());

    Q_EMIT(updateProgressBar("Aligning samples…", 0, 100));

    Aligner aligner;
    aligner.setAlignmentProgress.connect(boost::bind(&BackgroundPeakUpdate::qtSlot,
                                                     this, _1, _2, _3));

    _stopped = aligner.alignWithObiWarp(mavenParameters->samples, obiParams, mavenParameters);
    delete obiParams;

    if (_stopped) {
        Q_EMIT(restoreAlignment());
        //restore previous RTs
        for (auto sample : mavenParameters->samples) {
            sample->restorePreviousRetentionTimes();
        }

        //stopped without user intervention
        if (!mavenParameters->stop)
            Q_EMIT(alignmentError(
                   QString("There was an error during alignment. Please try again.")));

        mavenParameters->stop = false;
        return;
    }
        
    mainwindow->sampleRtWidget->plotGraph();
    Q_EMIT(samplesAligned(true));

}
void BackgroundPeakUpdate::writeCSVRep(string setName)
{
    auto prmGroupAt = find_if(begin(mavenParameters->allgroups),
                              end(mavenParameters->allgroups),
                              [] (PeakGroup& group) {
                                  if (group.compound)
                                    return (group.compound->type()
                                            == Compound::Type::PRM);
                                  return false;
                              });
    bool prmGroupExists = prmGroupAt != end(mavenParameters->allgroups);

    //write reports
    CSVReports* csvreports = NULL;
    if (mavenParameters->writeCSVFlag) {
        //Added to pass into csvreports file when merged with Maven776 - Kiran
        bool includeSetNamesLine=true;
        string groupfilename = mavenParameters->outputdir + setName + ".csv";
        csvreports = new CSVReports(mavenParameters->samples);
        csvreports->setMavenParameters(mavenParameters);
        csvreports->setUserQuantType(mainwindow->getUserQuantType());
        //Added to pass into csvreports file when merged with Maven776 - Kiran
        csvreports->openGroupReport(groupfilename,
                                    prmGroupExists,
                                    includeSetNamesLine);
    }

    peakDetector->pullAllIsotopes();

        for (int j = 0; j < mavenParameters->allgroups.size(); j++) {
            PeakGroup& group = mavenParameters->allgroups[j];

            if (csvreports != NULL)
                csvreports->addGroup(&group);

            if (mavenParameters->keepFoundGroups) {
                if (_untargetedMustHaveMs2
                    && mavenParameters->allgroups[j].ms2EventCount == 0)
                    continue;

                Q_EMIT(newPeakGroup(&(mavenParameters->allgroups[j])));
                QCoreApplication::processEvents();
            }
        }

        if (csvreports != NULL) {
                csvreports->closeFiles();
                delete (csvreports);
                csvreports = NULL;
        }
        Q_EMIT(updateProgressBar("Done", 1, 1));
}

void BackgroundPeakUpdate::setPeakDetector(PeakDetector *pd)
{
    if (peakDetector != nullptr)
        delete peakDetector;

    peakDetector = pd;
    peakDetector->boostSignal.connect(boost::bind(&BackgroundPeakUpdate::qtSlot,
                                                  this,
                                                  _1,
                                                  _2,
                                                  _3));
}

void BackgroundPeakUpdate::processSlices() {
        processSlices(mavenParameters->_slices, "sliceset");
}

void BackgroundPeakUpdate::processSlice(mzSlice& slice) {
        vector<mzSlice*> slices;
        slices.push_back(&slice);
        processSlices(slices, "sliceset");
}

//TODO: kiran Make a function which tell that its from option
//window and should be called where the settings are been called
void BackgroundPeakUpdate::getProcessSlicesSettings() {
        QSettings* settings = mainwindow->getSettings();

        // To Do: Are these lines required. The same is already being done in PeakDetectionDialog.cpp
        // mavenParameters->baseline_smoothingWindow = settings->value(
        //         "baseline_smoothing").toInt();
        // mavenParameters->baseline_dropTopX =
        //         settings->value("baseline_quantile").toInt();

}

void BackgroundPeakUpdate::align() {

        //These else if statements will take care of all corner cases of undoAlignment
        if (mavenParameters->alignSamplesFlag && mavenParameters->alignButton > 0) {
                ;
        } else if (mavenParameters->alignSamplesFlag && mavenParameters->alignButton ==0){
                mavenParameters->alignButton++;
                mavenParameters->undoAlignmentGroups = mavenParameters->allgroups;
        } else if (mavenParameters->alignSamplesFlag && mavenParameters->alignButton == -1) {
                ;
        } else {
                mavenParameters->alignButton = -1;
                mavenParameters->undoAlignmentGroups = mavenParameters->allgroups;
        }

        if (mavenParameters->alignSamplesFlag && !mavenParameters->stop) {
                Q_EMIT(updateProgressBar("Aligning samples…", 0, 0));
                vector<PeakGroup*> groups(mavenParameters->allgroups.size());
                for (int i = 0; i < mavenParameters->allgroups.size(); i++)
                        groups[i] = &mavenParameters->allgroups[i];
                Aligner aligner;
                int alignAlgo = mainwindow->alignmentDialog->alignAlgo->currentIndex();

                if (alignAlgo == 1) {
                        mainwindow->getAnalytics()->hitEvent("Alignment", "PolyFit");
                        aligner.setMaxIterations(mainwindow->alignmentDialog->maxIterations->value());
                        aligner.setPolymialDegree(mainwindow->alignmentDialog->polynomialDegree->value());
                        aligner.doAlignment(groups);
                        mainwindow->sampleRtWidget->setDegreeMap(aligner.sampleDegree);
                        mainwindow->sampleRtWidget->setCoefficientMap(aligner.sampleCoefficient);
                }

                mainwindow->deltaRt = aligner.getDeltaRt();
                mavenParameters->alignSamplesFlag = false;

        }
        QList<PeakGroup> listGroups;
        for (unsigned int i = 0; i<mavenParameters->allgroups.size(); i++) {
                listGroups.append(mavenParameters->allgroups.at(i));
        }	

        Q_EMIT(alignmentComplete(listGroups));
        Q_EMIT(samplesAligned(true));
}

void BackgroundPeakUpdate::alignUsingDatabase() {

    vector<mzSlice*> slices = peakDetector->processCompounds(mavenParameters->compounds, "compounds");
        processSlices(slices, "compounds");


}

void BackgroundPeakUpdate::processSlices(vector<mzSlice*>&slices,
                                         string setName) {

        getProcessSlicesSettings();

        peakDetector->processSlices(slices, setName);

        if (runFunction == "alignUsingDatabase") align();

        if (mavenParameters->showProgressFlag
            && mavenParameters->pullIsotopesFlag) {
                Q_EMIT(updateProgressBar("Calculating Isotopes", 1, 100));
        }

        writeCSVRep(setName);
}

void BackgroundPeakUpdate::qtSlot(const string& progressText, unsigned int progress, int totalSteps)
{
        Q_EMIT(updateProgressBar(QString::fromStdString(progressText), progress, totalSteps));

}

void BackgroundPeakUpdate::processCompounds(vector<Compound*> set,
                                            string setName) {

        if (set.size() == 0)
                return;

        Q_EMIT(updateProgressBar("Processing Compounds", 0, 0));
        vector<mzSlice*> slices = peakDetector->processCompounds(set, setName);
        processSlices(slices, setName);
        delete_all(slices);
}

void BackgroundPeakUpdate::processMassSlices() {
        Q_EMIT (updateProgressBar("Computing Mass Slices", 0, 0));
        mavenParameters->sig.connect(boost::bind(&BackgroundPeakUpdate::qtSignalSlot, this, _1, _2, _3));
        peakDetector->processMassSlices();
        //cerr << "BPU IS " << mavenParameters->allgroups.size() << endl;

        align();

        if (mavenParameters->showProgressFlag
            && mavenParameters->pullIsotopesFlag) {
                Q_EMIT(updateProgressBar("Calculation Isotopes", 1, 100));
        }

        writeCSVRep("allslices");
}

void BackgroundPeakUpdate::qtSignalSlot(const string& progressText, unsigned int completed_slices, int total_slices)
{
        Q_EMIT(updateProgressBar(QString::fromStdString(progressText), completed_slices, total_slices));

}

void BackgroundPeakUpdate::completeStop() {

    peakDetector->resetProgressBar();
        mavenParameters->stop = true;
        stop();
}

void BackgroundPeakUpdate::computePeaks() {
        if (mavenParameters->compounds.size() == 0)
                return;

        processCompounds(mavenParameters->compounds, "compounds");
}

/**
 * BackgroundPeakUpdate::setRunFunction Getting the function that has tobe ran
 * as a thread and updating it inside a variable
 * @param functionName [description]
 */
void BackgroundPeakUpdate::setRunFunction(QString functionName) {
        runFunction = functionName.toStdString();
}

void BackgroundPeakUpdate::pullIsotopes(PeakGroup* parentgroup) {

	bool isotopeFlag = mavenParameters->pullIsotopesFlag;

        if (!isotopeFlag) return;

	bool C13Flag = mavenParameters->C13Labeled_BPE;
	bool N15Flag = mavenParameters->N15Labeled_BPE;
	bool S34Flag = mavenParameters->S34Labeled_BPE;
	bool D2Flag = mavenParameters->D2Labeled_BPE;

        IsotopeDetection::IsotopeDetectionType isoType;
        isoType = IsotopeDetection::PeakDetection;

	IsotopeDetection isotopeDetection(
                mavenParameters,
                isoType,
                C13Flag,
                N15Flag,
                S34Flag,
                D2Flag);
	isotopeDetection.pullIsotopes(parentgroup);
}

void BackgroundPeakUpdate::pullIsotopesIsoWidget(PeakGroup* parentgroup) {
	bool C13Flag = mavenParameters->C13Labeled_BPE;
	bool N15Flag = mavenParameters->N15Labeled_BPE;
	bool S34Flag = mavenParameters->S34Labeled_BPE;
	bool D2Flag = mavenParameters->D2Labeled_BPE;

        IsotopeDetection::IsotopeDetectionType isoType;
        isoType = IsotopeDetection::IsoWidget;

	IsotopeDetection isotopeDetection(
                mavenParameters,
                isoType,
                C13Flag,
                N15Flag,
                S34Flag,
                D2Flag);
	isotopeDetection.pullIsotopes(parentgroup);
}

void BackgroundPeakUpdate::pullIsotopesBarPlot(PeakGroup* parentgroup) {

	bool C13Flag = mavenParameters->C13Labeled_Barplot;
	bool N15Flag = mavenParameters->N15Labeled_Barplot;
	bool S34Flag = mavenParameters->S34Labeled_Barplot;
	bool D2Flag = mavenParameters->D2Labeled_Barplot;

        IsotopeDetection::IsotopeDetectionType isoType;
        isoType = IsotopeDetection::BarPlot;

	IsotopeDetection isotopeDetection(
                mavenParameters,
                isoType,
                C13Flag,
                N15Flag,
                S34Flag,
                D2Flag);
        isotopeDetection.pullIsotopes(parentgroup);
}

bool BackgroundPeakUpdate::covertToMzXML(QString filename, QString outfile) {

        QFile test(outfile);
        if (test.exists())
                return true;

        QString command = QString("ReAdW.exe --centroid --mzXML \"%1\" \"%2\"").arg(
                filename).arg(outfile);

        qDebug() << command;

        QProcess *process = new QProcess();
        //connect(process, SIGNAL(finished(int)), this, SLOT(doVideoCreated(int)));
        process->start(command);

        if (!process->waitForStarted()) {
                process->kill();
                return false;
        }

        while (!process->waitForFinished()) {
        };
        QFile testOut(outfile);
        return testOut.exists();
}
