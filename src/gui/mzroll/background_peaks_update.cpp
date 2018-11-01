#include "background_peaks_update.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <QDir>
#include <QCoreApplication>

BackgroundPeakUpdate::BackgroundPeakUpdate(QWidget*) {
        mainwindow = NULL;
        _stopped = true;
        setTerminationEnabled(true);
        runFunction = "computeKnowsPeaks";
        peakDetector.boostSignal.connect(boost::bind(&BackgroundPeakUpdate::qtSlot, this, _1, _2, _3));

        /**
         * create one python program for running alignment
         */
        pythonProg = new QProcess();
        QString programPath;
        #if defined(Q_OS_LINUX)
                programPath =  QCoreApplication::applicationDirPath() + QDir::separator() + "linux" + QDir::separator() + "python_exe";
        #elif defined(Q_OS_WIN)
                programPath = QCoreApplication::applicationDirPath() + QDir::separator() + "windows" + QDir::separator() + "python_exe.exe";
        #endif
        pythonProg->setProgram(programPath);
        connect(pythonProg,SIGNAL(readyRead()),this,SLOT(readDataFromPython()));
}

QString BackgroundPeakUpdate::printSettings() {

    QString buffer;
    QTextStream summary( &buffer, QIODevice::ReadWrite);
    summary << "-------------------SETTINGS-------------------"<< "\n"<< "\n";
//     summary << "runFunction =" << runFunction<< "\n";
    summary << "alignSamplesFlag="  <<  mavenParameters->alignSamplesFlag<< "\n";
    summary << "alignMaxItterations="  <<  mavenParameters->alignMaxItterations << "\n";
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
    summary << "matchFragmentation=" << mavenParameters->matchFragmentation
            << "\n";
    summary << "fragmentMatchMassCutoffTolr=" << mavenParameters->fragmentMatchMassCutoffTolr->getMassCutoff()
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
    stream.writeAttribute( "matchFragmentation" ,QString::number( mavenParameters->matchFragmentation));
    stream.writeAttribute( "fragmentMatchMassCutoffTolr" ,QString::number( mavenParameters->fragmentMatchMassCutoffTolr->getMassCutoff()));

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
                // settings->setValue("minFragmentMatchScore",
                //                   minFragMatchScore->value());
                settings->setValue("matchFragmentation",
                        xml.attributes().value("matchFragmentation").toString().toInt());

                // Enabling feature detection or compound search
                // mavenParameters->runFunction =
                // xml.attributes().value("runFunction").toString();
                settings->setValue("limitGroupCount",
                        xml.attributes()
                        .value("limitGroupCount")
                        .toString()
                        .toInt());

                settings->setValue("fragmentMatchMassCutoffTolr",
                        xml.attributes()
                        .value("fragmentMatchMassCutoffTolr")
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
        connect(this,SIGNAL(alignmentError(QString)),mainwindow,SLOT(showAlignmetErrorDialog(QString)));
        if (mavenParameters->alignSamplesFlag) {
                connect(this, SIGNAL(alignmentComplete(QList<PeakGroup> )), mainwindow, SLOT(showAlignmentWidget()));
        }
	qRegisterMetaType<QList<PeakGroup> >("QList<PeakGroup>");
	connect(this, SIGNAL(alignmentComplete(QList<PeakGroup> )), mainwindow, SLOT(plotAlignmentVizAllGroupGraph(QList<PeakGroup>)));
	connect(this, SIGNAL(alignmentComplete(QList<PeakGroup> )), mainwindow->groupRtWidget, SLOT(setCurrentGroups(QList<PeakGroup>)));
        connect(this, SIGNAL(alignmentComplete(QList<PeakGroup> )), mainwindow->sampleRtWidget, SLOT(plotGraph()));
        mavenParameters->stop = false;
        //_stopped = false;

        //populating the maven setting insatnces with with the samples
        // if (mavenParameters->samples.size() == 0) {
        //         mavenParameters->samples = mainwindow->getSamples();
        // }
        //Getting the classification model
        //mavenParameters->clsf = mainwindow->getClassifier();

        //Setting the ionization mode if the user specifies the ionization mode
        //then its given the priority else the ionization mode is taken from the
        //sample
        //TODO: See how the ionization mode is effected if the user selects
        //Neutral or autodetect
        // if (mainwindow->getIonizationMode()) {
        //         mavenParameters->ionizationMode = mainwindow->getIonizationMode();
        // } else {
        //         mavenParameters->setIonizationMode();
        // }
        if (runFunction == "findPeaksQQQ") {
                findPeaksQQQ();
        } else if (runFunction == "alignUsingDatabase") {
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
void BackgroundPeakUpdate::alignWithObiWarp(){
        ObiParams *obiParams = new ObiParams(mainwindow->alignmentDialog->scoreObi->currentText().toStdString(),
                                        mainwindow->alignmentDialog->local->isChecked(),
                                        mainwindow->alignmentDialog->factorDiag->value(),
                                        mainwindow->alignmentDialog->factorGap->value(),
                                        mainwindow->alignmentDialog->gapInit->value(),
                                        mainwindow->alignmentDialog->gapExtend->value(),
                                        mainwindow->alignmentDialog->initPenalty->value(),
                                        mainwindow->alignmentDialog->responseObiWarp->value(),
                                        mainwindow->alignmentDialog->noStdNormal->isChecked(),
                                        mainwindow->alignmentDialog->binSizeObiWarp->value()
                                );
        Q_EMIT(updateProgressBar("Aligning Samples", 0, 0));

        Aligner aligner;
        aligner.alignWithObiWarp(mavenParameters->samples, obiParams);
        delete obiParams;

        mainwindow->sampleRtWidget->plotGraph();

}
void BackgroundPeakUpdate::writeCSVRep(string setName) {

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
                csvreports->openGroupReport(groupfilename,includeSetNamesLine);
        }

        peakDetector.pullAllIsotopes();

        for (int j = 0; j < mavenParameters->allgroups.size(); j++) {
			PeakGroup& group = mavenParameters->allgroups[j];

			if (csvreports != NULL) csvreports->addGroup(&group);


			if (mavenParameters->keepFoundGroups) {
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


void BackgroundPeakUpdate::sendDataToPython(QJsonObject& grpJson, QJsonObject& rtsJson)
{
    // prepare the data we have to send to python
    QJsonObject jObj;
    jObj.insert("groups", grpJson);
    jObj.insert("rts", rtsJson);


    QJsonDocument jDoc(jObj);
    QByteArray data = jDoc.toJson();

    writeToPythonProcess(data);

}

void BackgroundPeakUpdate::readDataFromPython()
{
        while(pythonProg->bytesAvailable())
                processedDataFromPython += pythonProg->readLine(1024*1024);
        
}
void BackgroundPeakUpdate::writeToPythonProcess(QByteArray data){

        if(pythonProg->state()!=QProcess::Running){
                qDebug()<<"Error in pipe- data sent to be written but python process is not running";
                return;
        }
        QTextStream stream(pythonProg);
        int quantumOfData=1024*1024;
        for(int i=0;i<data.size();i+=quantumOfData){
                stream<<data.mid(i,quantumOfData);
        }
        stream.flush();
        pythonProg->closeWriteChannel();
}

void BackgroundPeakUpdate::runPythonProg(Aligner* aligner)
{

    if(pythonProg->state() != QProcess::NotRunning)
        pythonProg->kill();

    pythonProg->start();

    /**
     * wait for python to start otherwise exit
     */
    if(pythonProg->waitForStarted(-1)) {
        qDebug()<<"python program is running...";
        sendDataToPython(aligner->groupsJson, aligner->rtsJson);
    }
    else{
        qDebug()<<"Python program did not start. Check availability of execcutable";
    }

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
                Q_EMIT(updateProgressBar("Aligning Samples", 0, 0));
                vector<PeakGroup*> groups(mavenParameters->allgroups.size());
                for (int i = 0; i < mavenParameters->allgroups.size(); i++)
                        groups[i] = &mavenParameters->allgroups[i];
                Aligner aligner;
                int alignAlgo = mainwindow->alignmentDialog->alignAlgo->currentIndex();

                if (alignAlgo == 0) {
                        aligner.setMaxItterations(mainwindow->alignmentDialog->maxItterations->value());
                        aligner.setPolymialDegree(mainwindow->alignmentDialog->polynomialDegree->value());
                        aligner.doAlignment(groups);
                        mainwindow->sampleRtWidget->setDegreeMap(aligner.sampleDegree);
                        mainwindow->sampleRtWidget->setCoefficientMap(aligner.sampleCoefficient);
                } else if (alignAlgo == 1) {
                        aligner.preProcessing(groups, mavenParameters->alignWrtExpectedRt);
                        // initialize processedDataFromPython with null 
                        processedDataFromPython="";
                         /**runPythonProg()
                         * sends the json of groups and samples rt to the python exe. for more look in sendDataToPython()
                         * python exe is going to correct the rts and send it back to us in json format
                         */
                        runPythonProg(&aligner);
                        // wait for processing of data by python program
                        pythonProg->waitForFinished(-1);                        

                        // convert the data to json
                        QJsonDocument jDoc;
                        QJsonObject parentObj;

                        // if jDoc is null that means the json returned from python is malformed
                        // in such a case our rts wont update with new values
                        jDoc = QJsonDocument::fromJson(processedDataFromPython);

                        QString errorMessage=QString::number(processedDataFromPython.size());

                        if(!jDoc.isNull()){
                                parentObj = jDoc.object();
                        }
                        else{
                                if(processedDataFromPython.size()==0){
                                        errorMessage=errorMessage + " good groups found." +"<br>"+"Relax parameters for better result";
                                }
                                else{
                                        errorMessage=errorMessage+"<br>"+"Incomplete data, re-run alignment";
                                }
                                qDebug()<<errorMessage;
                                Q_EMIT alignmentError(errorMessage);
                                return;
                        }

                        if(!parentObj.isEmpty()){
                                aligner.updateRts(parentObj);
                                qDebug()<<"Alignment complete";
                        }
                        else{
                                errorMessage=errorMessage+"<br>"+"Incomplete data, re-run alignment";
                                qDebug()<<errorMessage;
                                Q_EMIT alignmentError(errorMessage);
                                return;
                        }
                }

                mainwindow->deltaRt = aligner.getDeltaRt();
                mavenParameters->alignSamplesFlag = false;

        }
        QList<PeakGroup> listGroups;
        for (unsigned int i = 0; i<mavenParameters->allgroups.size(); i++) {
                listGroups.append(mavenParameters->allgroups.at(i));
        }	

        Q_EMIT(alignmentComplete(listGroups));
}

void BackgroundPeakUpdate::alignUsingDatabase() {

        vector<mzSlice*> slices = peakDetector.processCompounds(mavenParameters->compounds, "compounds");
        processSlices(slices, "compounds");


}

void BackgroundPeakUpdate::processSlices(vector<mzSlice*>&slices,
                                         string setName) {

        getProcessSlicesSettings();

        peakDetector.processSlices(slices, setName);

        if (runFunction == "alignUsingDatabase") align();

        if (mavenParameters->showProgressFlag
            && mavenParameters->pullIsotopesFlag) {
                Q_EMIT(updateProgressBar("Calculation Isotopes", 1, 100));
        }

        writeCSVRep(setName);
}

void BackgroundPeakUpdate::qtSlot(const string& progressText, unsigned int completed_slices, int total_slices)
{
        Q_EMIT(updateProgressBar(QString::fromStdString(progressText), completed_slices, total_slices));

}

void BackgroundPeakUpdate::processCompounds(vector<Compound*> set,
                                            string setName) {

        if (set.size() == 0)
                return;

        Q_EMIT(updateProgressBar("Processing Compounds", 0, 0));
        vector<mzSlice*> slices = peakDetector.processCompounds(set, setName);
        processSlices(slices, setName);
        delete_all(slices);
}

void BackgroundPeakUpdate::processMassSlices() {
        Q_EMIT (updateProgressBar("Computing Mass Slices", 0, 0));
        mavenParameters->sig.connect(boost::bind(&BackgroundPeakUpdate::qtSignalSlot, this, _1, _2, _3));
        peakDetector.processMassSlices();
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
        
        peakDetector.resetProgressBar();
        mavenParameters->stop = true;
        //stop();
}

void BackgroundPeakUpdate::computePeaks() {
        if (mavenParameters->compounds.size() == 0)
                return;

        processCompounds(mavenParameters->compounds, "compounds");
}

//TODO: Not being used anywhere right now - Sahil
void BackgroundPeakUpdate::findPeaksQQQ() {

        if(mainwindow == NULL) return;

        int userPolarity = 0;
	if (mainwindow->getIonizationMode()) userPolarity = mainwindow->getIonizationMode();
        
        bool associateCompoundNames = false;

        deque<Compound*> compoundsDB = DB.getCompoundsDB();

        double amuQ1 = mainwindow->getSettings()->value("amuQ1").toDouble();
        double amuQ3 = mainwindow->getSettings()->value("amuQ3").toDouble();

	vector<mzSlice*>slices = mainwindow->srmList->getSrmSlices(amuQ1, amuQ3, userPolarity, associateCompoundNames);

        processSlices(slices,"QQQ Peaks");
	delete_all(slices);
	slices.clear();
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
	bool C13Flag = mavenParameters->C13Labeled_IsoWidget;
	bool N15Flag = mavenParameters->N15Labeled_IsoWidget;
	bool S34Flag = mavenParameters->S34Labeled_IsoWidget;
	bool D2Flag = mavenParameters->D2Labeled_IsoWidget;

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
