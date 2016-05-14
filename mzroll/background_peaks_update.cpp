#include "background_peaks_update.h"

BackgroundPeakUpdate::BackgroundPeakUpdate(QWidget*) {
    clsf = NULL;    //initially classifier is not loaded
    mainwindow = NULL;
    _stopped = true;
//  setTerminationEnabled(false);

    runFunction = "computeKnowsPeaks";
    writePdfReportFlag = false;
    alignSamplesFlag = false;
    processKeggFlag = false;
    processMassSlicesFlag = false;
    pullIsotopesFlag = false;
    matchRtFlag = false;
    checkConvergance = false;

    outputdir = "reports" + string(DIR_SEPARATOR_STR);

    writeCSVFlag = false;
    ionizationMode = -1;
    keepFoundGroups = true;
    showProgressFlag = true;

    mzBinStep = 0.01;
    rtStepSize = 20;
    ppmMerge = 30;
    qcut = 0.05;
    avgScanTime = 0.2;

    limitGroupCount = INT_MAX;

    //peak detection
    eic_smoothingWindow = 10;
    eic_smoothingAlgorithm = 0;

    //peak grouping across samples
    grouping_maxRtWindow = 0.5;

    //peak filtering criteria
    minGoodPeakCount = 1;
    minSignalBlankRatio = 2;
    minNoNoiseObs = 1;
    minSignalBaseLineRatio = 2;
    minGroupIntensity = 500;

    //compound detection setting
    compoundPPMWindow = 10;
    compoundRTWindow = 2;
    eicMaxGroups = INT_MAX;

}

BackgroundPeakUpdate::~BackgroundPeakUpdate() {
    cleanup();
}

void BackgroundPeakUpdate::run(void) {

    if (mainwindow == NULL)   {
        quit();
        return;
    }
    _stopped = false;

    if ( samples.size() == 0) samples = mainwindow->getSamples(); //get samples
    clsf = mainwindow->getClassifier(); //get classification modej

    if (samples.size() > 0 && samples[0]->getPolarity() > 0 ) ionizationMode = +1;
    else ionizationMode = -1; //set ionization mode for compound matching
    if (mainwindow->getIonizationMode()) ionizationMode = mainwindow->getIonizationMode(); //user specified ionization mode

    if ( runFunction == "findPeaksQQQ" ) {
        findPeaksQQQ();
    } else if ( runFunction == "processSlices" ) {
        processSlices();
    } else if ( runFunction == "processMassSlices" ) {
        processMassSlices();
    } else if  ( runFunction == "computeKnowsPeaks" ) {
        computeKnowsPeaks();
    } else if  (runFunction == "pullIsotopes" ) {
        pullIsotopes(_group);
    } else if  ( runFunction == "computePeaks" ) {
        computePeaks();
    } else {
        qDebug() << "Unknown Function " << runFunction.c_str();
    }

    quit();
    return;
}

void BackgroundPeakUpdate::processSlices() {
    processSlices(_slices, "sliceset");
}

void BackgroundPeakUpdate::processSlice(mzSlice& slice) {
    vector<mzSlice*>slices;
    slices.push_back(&slice);
    processSlices(slices, "sliceset");
}

void BackgroundPeakUpdate::processSlices(vector<mzSlice*>&slices, string setName) {
    if (slices.size() == 0 ) return;
    allgroups.clear();
    sort(slices.begin(), slices.end(), mzSlice::compIntensity );

    //process KNOWNS
    QTime timer;
    timer.start();
    qDebug() << "Proessing slices: setName=" << setName.c_str() << " slices=" << slices.size();

    int converged = 0;
    int foundGroups = 0;

    int eicCount = 0;
    int groupCount = 0;
    int peakCount = 0;

    QSettings* settings = mainwindow->getSettings();
    float amuQ1 = settings->value("amuQ1").toDouble();
    float amuQ3 = settings->value("amuQ3").toDouble();

    for (unsigned int s = 0; s < slices.size();  s++ ) {
        mzSlice* slice = slices[s];
        double mzmin = slice->mzmin;
        double mzmax = slice->mzmax;
        double rtmin = slice->rtmin;
        double rtmax = slice->rtmax;;

        Compound* compound = slice->compound;

        if ( compound != NULL && compound->hasGroup() )
            compound->unlinkGroup();

        if (checkConvergance ) {
            allgroups.size() - foundGroups > 0 ? converged = 0 : converged++;
            if ( converged > 1000 ) break;   //exit main loop
            foundGroups = allgroups.size();
        }

        vector<EIC*>eics = pullEICs(slice, samples, EicLoader::PeakDetection, eic_smoothingWindow, eic_smoothingAlgorithm, amuQ1, amuQ3);
        float eicMaxIntensity = 0;

        for ( unsigned int j = 0; j < eics.size(); j++ ) {
            eicCount++;
            if (eics[j]->maxIntensity > eicMaxIntensity) eicMaxIntensity = eics[j]->maxIntensity;
        }
        if (eicMaxIntensity < minGroupIntensity) {
            delete_all(eics);
            continue;
        }

        //for ( unsigned int j=0; j < eics.size(); j++ )  eics[j]->getPeakPositions(eic_smoothingWindow);
        vector<PeakGroup> peakgroups = EIC::groupPeaks(eics, eic_smoothingWindow, grouping_maxRtWindow);

        //score quality of each group
        vector<PeakGroup*> groupsToAppend;
        for (int j = 0; j < peakgroups.size(); j++ ) {
            PeakGroup& group = peakgroups[j];
            group.computeAvgBlankArea(eics);
            group.groupStatistics();
            groupCount++;
            peakCount += group.peakCount();

            if (clsf->hasModel()) {
                clsf->classify(&group);
                group.groupStatistics();
            }
            if (clsf->hasModel() && group.goodPeakCount < minGoodPeakCount) continue;
            // if (group.blankMean*minBlankRatio > group.sampleMean ) continue;
            if (group.blankMax * minSignalBlankRatio > group.maxIntensity) continue;
            if (group.maxNoNoiseObs < minNoNoiseObs) continue;
            if (group.maxSignalBaselineRatio < minSignalBaseLineRatio) continue;
            if (group.maxIntensity < minGroupIntensity ) continue;

            if (compound) group.compound = compound;
            if (!slice->srmId.empty()) group.srmId = slice->srmId;

            if (matchRtFlag && compound != NULL && compound->expectedRt > 0) {
                float rtDiff =  abs(compound->expectedRt - (group.meanRt));
                group.expectedRtDiff = rtDiff;
                group.groupRank = rtDiff * rtDiff * (1.1 - group.maxQuality) * (1 / log(group.maxIntensity + 1));
                if (group.expectedRtDiff > compoundRTWindow ) continue;
            } else {
                group.groupRank = (1.1 - group.maxQuality) * (1 / log(group.maxIntensity + 1));
            }

            groupsToAppend.push_back(&group);
        }


        std::sort(groupsToAppend.begin(), groupsToAppend.end(), PeakGroup::compRankPtr);

        for (int j = 0; j < groupsToAppend.size(); j++) {
            //check for duplicates  and append group
            if (j >= eicMaxGroups) break;

            PeakGroup* group = groupsToAppend[j];
            bool ok = addPeakGroup(*group);

            //force insert when processing compounds.. even if duplicated
            if (ok == false && compound != NULL ) allgroups.push_back(*group);
        }

        delete_all(eics);

        if (allgroups.size() > limitGroupCount ) break;
        if ( stopped() ) break;

        if (showProgressFlag && s % 10 == 0) {
            QString progressText = "Found " + QString::number(allgroups.size()) + " groups";
            emit(updateProgressBar( progressText , s + 1, std::min((int)slices.size(), limitGroupCount)));
        }
    }

    if (alignSamplesFlag) {
        emit(updateProgressBar("Aligning Samples" , 1, 100));
        vector<PeakGroup*> groups(allgroups.size());
        for (int i = 0; i < allgroups.size(); i++ ) groups[i] = &allgroups[i];
        Aligner aligner;
        aligner.setMaxItterations(mainwindow->alignmentDialog->maxItterations->value());
        aligner.setPolymialDegree(mainwindow->alignmentDialog->polynomialDegree->value());
        aligner.doAlignment(groups);
    }

    if (showProgressFlag && pullIsotopesFlag ) {
        emit(updateProgressBar("Calculation Isotopes" , 1, 100));
    }

    //write reports
    CSVReports* csvreports = NULL;
    if (writeCSVFlag) {
        string groupfilename = outputdir + setName + ".csv";
        csvreports = new CSVReports(samples);
        csvreports->setUserQuantType(mainwindow->getUserQuantType());
        csvreports->openGroupReport(groupfilename);
    }

    for (int j = 0; j < allgroups.size(); j++ ) {
        PeakGroup& group = allgroups[j];
        Compound* compound = group.compound;

        if (pullIsotopesFlag && !group.isIsotope()) pullIsotopes(&group);
        if (csvreports != NULL) csvreports->addGroup(&group);

        if (compound) {
            if (!compound->hasGroup() || group.groupRank < compound->getPeakGroup()->groupRank )
                compound->setPeakGroup(group);
        }

        if (keepFoundGroups) {
            emit(newPeakGroup(&allgroups[j]));
            QCoreApplication::processEvents();
        }

        if (showProgressFlag && pullIsotopesFlag && j % 10 == 0) {
            emit(updateProgressBar("Calculating Isotopes", j , allgroups.size()));
        }
    }

    if (csvreports != NULL) {
        csvreports->closeFiles();
        delete(csvreports);
        csvreports = NULL;
    }
    emit(updateProgressBar("Done" , 1, 1));

    qDebug() << "processSlices() Slices=" << slices.size();
    qDebug() << "processSlices() EICs="   << eicCount;
    qDebug() << "processSlices() Groups="   << groupCount;
    qDebug() << "processSlices() Peaks="   << peakCount;
    qDebug() << "BackgroundPeakUpdate:processSlices() done. " << timer.elapsed() << " sec.";
    //cleanup();
}

bool BackgroundPeakUpdate::addPeakGroup(PeakGroup& grp1) {

    for (int i = 0; i < allgroups.size(); i++) {
        PeakGroup& grp2 = allgroups[i];
        float rtoverlap = mzUtils::checkOverlap(grp1.minRt, grp1.maxRt, grp2.minRt, grp2.maxRt );
        if ( rtoverlap > 0.9 && ppmDist(grp2.meanMz, grp1.meanMz) < ppmMerge  ) {
            return false;
        }
    }

    allgroups.push_back(grp1);
    return true;
}

void BackgroundPeakUpdate::cleanup() {
    allgroups.clear();
}

void BackgroundPeakUpdate::processCompounds(vector<Compound*> set, string setName) {
    if (set.size() == 0 ) return;

    limitGroupCount = INT_MAX; //must not be active when processing compounds

    vector<mzSlice*>slices;
    for (unsigned int i = 0; i < set.size();  i++ ) {
        Compound* c = set[i];
        if ( c == NULL ) continue;

        mzSlice* slice = new mzSlice();
        slice->compound = c;
        if (!c->srmId.empty()) slice->srmId = c->srmId;

        if (!c->formula.empty()) {
            double mass = mcalc.computeMass(c->formula, ionizationMode);
            slice->mzmin = mass - compoundPPMWindow * mass / 1e6;
            slice->mzmax = mass + compoundPPMWindow * mass / 1e6;
        } else if (c->mass > 0 ) {
            double mass = c->mass;
            slice->mzmin = mass - compoundPPMWindow * mass / 1e6;
            slice->mzmax = mass + compoundPPMWindow * mass / 1e6;
        } else {
            continue;
        }


        if (matchRtFlag && c->expectedRt > 0 ) {
            slice->rtmin = c->expectedRt - compoundRTWindow;
            slice->rtmax = c->expectedRt + compoundRTWindow;
        } else {
            slice->rtmin = 0;
            slice->rtmax = 1e9;
        }
        slices.push_back(slice);
    }

    //printSettings();
    processSlices(slices, setName);
    delete_all(slices);
}

void BackgroundPeakUpdate::setSamples(vector<mzSample*>&set) {
    samples = set;
    if ( samples.size() > 0 ) avgScanTime = samples[0]->getAverageFullScanTime();
}

void BackgroundPeakUpdate::processMassSlices() {

    showProgressFlag = true;
    checkConvergance = true;
    QTime timer;
    timer.start();

    if ( samples.size() > 0 ) avgScanTime = samples[0]->getAverageFullScanTime();

    emit (updateProgressBar( "Computing Mass Slices", 2, 10 ));
    MassSlices massSlices;
    massSlices.setSamples(samples);
    massSlices.algorithmB(ppmMerge, minGroupIntensity, rtStepSize);

    if (massSlices.slices.size() == 0) massSlices.algorithmA();
    sort(massSlices.slices.begin(), massSlices.slices.end(), mzSlice::compIntensity );

    emit (updateProgressBar( "Computing Mass Slices", 0, 10 ));

    vector<mzSlice*>goodslices;
    goodslices.resize(massSlices.slices.size());
    for (int i = 0; i < massSlices.slices.size(); i++ ) goodslices[i] = massSlices.slices[i];

    if (goodslices.size() == 0 ) {
        emit (updateProgressBar( "Quting! No good mass slices found", 1, 1 ));
        return;
    }

    string setName = "allslices";
    processSlices(goodslices, setName);
    delete_all(massSlices.slices);
    massSlices.slices.clear();
    goodslices.clear();
    qDebug() << tr("processMassSlices() Done. ElepsTime=%1 msec").arg(timer.elapsed());
}

void BackgroundPeakUpdate::computePeaks() {
    if (compounds.size() == 0 ) {
        return;
    }
    processCompounds(compounds, "compounds");
}

void BackgroundPeakUpdate::computeKnowsPeaks() {

    //peak filtering criteria
    vector<Compound*> knowns = DB.getKnowns();
    minGoodPeakCount = 1;
    minSignalBlankRatio = 2;
    minSignalBaseLineRatio = 2;
    minNoNoiseObs = 2;
    minGroupIntensity = 0;
    matchRtFlag = false;
    showProgressFlag = true;
    checkConvergance = false;

    if (mainwindow == NULL) {
        QSettings* settings = mainwindow->getSettings();
        if ( settings != NULL ) {
            grouping_maxRtWindow = settings->value("grouping_maxRtWindow").toDouble();
            eic_smoothingWindow =  settings->value("eic_smoothingWindow").toDouble();
        }
    }
    processCompounds(knowns, "knowns");
}

void BackgroundPeakUpdate::findPeaksQQQ() {
    if (mainwindow == NULL) return;
    vector<mzSlice*>slices = mainwindow->getSrmSlices();
    processSlices(slices, "QQQ Peaks");
    delete_all(slices);
    slices.clear();
}

void BackgroundPeakUpdate::setRunFunction(QString functionName) {
    runFunction = functionName.toStdString();
}




vector<EIC*> BackgroundPeakUpdate::pullEICs(mzSlice* slice, std::vector<mzSample*>&samples, int peakDetect, int smoothingWindow, int smoothingAlgorithm, float amuQ1, float amuQ3) {
    vector<EIC*> eics;
    vector<mzSample*>vsamples;

    for ( unsigned int i = 0; i < samples.size(); i++ ) {
        if (samples[i] == NULL) continue;
        if (samples[i]->isSelected == false) continue;
        vsamples.push_back(samples[i]);
    }



    if (vsamples.size()) {
        /*EicLoader::PeakDetectionFlag pdetect = (EicLoader::PeakDetectionFlag) peakDetect;
        QFuture<EIC*>future = QtConcurrent::mapped(vsamples, EicLoader(slice, pdetect,smoothingWindow, smoothingAlgorithm, amuQ1,amuQ3));

        //wait for async operations to finish
        future.waitForFinished();

        QFutureIterator<EIC*> itr(future);

        while(itr.hasNext()) {
            EIC* eic = itr.next();
            if ( eic && eic->size() > 0) eics.push_back(eic);
        }
        */

        /*
        QList<EIC*> _eics = result.results();
        for(int i=0; i < _eics.size(); i++ )  {
            if ( _eics[i] && _eics[i]->size() > 0) {
                eics.push_back(_eics[i]);
            }
        }*/
    }


    // single threaded version

    for ( unsigned int i = 0; i < vsamples.size(); i++ ) {
        mzSample* sample = vsamples[i];
        Compound* c =  slice->compound;

        EIC* e = NULL;

        if ( ! slice->srmId.empty() ) {
            //cout << "computeEIC srm:" << slice->srmId << endl;
            e = sample->getEIC(slice->srmId);
        } else if ( c && c->precursorMz > 0 && c->productMz > 0 ) {
            //cout << "computeEIC qqq: " << c->precursorMz << "->" << c->productMz << endl;
            e = sample->getEIC(c->precursorMz, c->collisionEnergy, c->productMz, amuQ1, amuQ3);
        } else {
            //cout << "computeEIC mzrange" << setprecision(7) << slice->mzmin  << " " << slice->mzmax << slice->rtmin  << " " << slice->rtmax << endl;
            e = sample->getEIC(slice->mzmin, slice->mzmax, slice->rtmin, slice->rtmax, 1);
        }

        if (e) {
            EIC::SmootherType smootherType = (EIC::SmootherType) smoothingAlgorithm;
            e->setSmootherType(smootherType);
            e->getPeakPositions(smoothingWindow);
            eics.push_back(e);
        }
    }
    return eics;
}

void BackgroundPeakUpdate::pullIsotopes(PeakGroup* parentgroup) {

    if (parentgroup == NULL) return;
    if (parentgroup->compound == NULL ) return;
    if (parentgroup->compound->formula.empty() == true) return;
    if ( samples.size() == 0 ) return;

    float ppm = compoundPPMWindow;
    double maxIsotopeScanDiff = 10;
    double maxNaturalAbundanceErr = 100;
    double minIsotopicCorrelation = 0;
    bool   C13Labeled = false;
    bool   N15Labeled = false;
    bool   S34Labeled = false;
    bool   D2Labeled = false;
    int eic_smoothingAlgorithm = 0;


    if (mainwindow) {
        QSettings* settings = mainwindow->getSettings();
        if (settings) {
            maxIsotopeScanDiff  = settings->value("maxIsotopeScanDiff").toDouble();
            minIsotopicCorrelation  = settings->value("minIsotopicCorrelation").toDouble();
            maxNaturalAbundanceErr  = settings->value("maxNaturalAbundanceErr").toDouble();
            C13Labeled   =  settings->value("C13Labeled").toBool();
            N15Labeled   =  settings->value("N15Labeled").toBool();
            S34Labeled   =  settings->value("S34Labeled").toBool();
            D2Labeled   =   settings->value("D2Labeled").toBool();
            QSettings* settings = mainwindow->getSettings();
            eic_smoothingAlgorithm = settings->value("eic_smoothingAlgorithm").toInt();

            //Feng note: assign labeling state to sample
            samples[0]->_C13Labeled = C13Labeled;
            samples[0]->_N15Labeled = N15Labeled;
            samples[0]->_S34Labeled = S34Labeled;
            samples[0]->_D2Labeled = D2Labeled;
            //End Feng addition
        }
    }

    string formula = parentgroup->compound->formula;
    vector<Isotope> masslist = mcalc.computeIsotopes(formula, ionizationMode);

    map<string, PeakGroup>isotopes;
    map<string, PeakGroup>::iterator itr2;
    for ( unsigned int s = 0; s < samples.size(); s++ ) {
        mzSample* sample = samples[s];
        for (int k = 0; k < masslist.size(); k++) {
            if ( stopped() ) break;
            Isotope& x = masslist[k];
            string isotopeName = x.name;
            double isotopeMass = x.mass;
            double expectedAbundance = x.abundance;
            float mzmin = isotopeMass - isotopeMass / 1e6 * ppm;
            float mzmax = isotopeMass + isotopeMass / 1e6 * ppm;
            float rt =  parentgroup->medianRt();
            float rtmin = parentgroup->minRt;
            float rtmax = parentgroup->maxRt;


            Peak* parentPeak =  parentgroup->getPeak(sample);
            if ( parentPeak ) rt  =   parentPeak->rt;
            if ( parentPeak ) rtmin = parentPeak->rtmin;
            if ( parentPeak ) rtmax = parentPeak->rtmax;

            float isotopePeakIntensity = 0;
            float parentPeakIntensity = 0;

            if ( parentPeak ) {
                parentPeakIntensity = parentPeak->peakIntensity;
                int scannum = parentPeak->getScan()->scannum;
                for (int i = scannum - 3; i < scannum + 3; i++ ) {
                    Scan* s = sample->getScan(i);

                    //look for isotopic mass in the same spectrum
                    vector<int> matches = s->findMatchingMzs(mzmin, mzmax);

                    for (int i = 0; i < matches.size(); i++ ) {
                        int pos = matches[i];
                        if ( s->intensity[pos] > isotopePeakIntensity ) {
                            isotopePeakIntensity = s->intensity[pos];
                            rt = s->rt;
                        }
                    }
                }

            }
            //if(isotopePeakIntensity==0) continue;

            //natural abundance check
            if (    (x.C13 > 0    && C13Labeled == false)
                    || (x.N15 > 0 && N15Labeled == false)
                    || (x.S34 > 0 && S34Labeled == false )
                    || (x.H2 > 0  && D2Labeled == false )

               ) {
                if (expectedAbundance < 1e-8) continue;
                if (expectedAbundance * parentPeakIntensity < 1) continue;
                float observedAbundance = isotopePeakIntensity / (parentPeakIntensity + isotopePeakIntensity);
                float naturalAbundanceError = abs(observedAbundance - expectedAbundance) / expectedAbundance * 100;

                //cerr << isotopeName << endl;
                //cerr << "Expected isotopeAbundance=" << expectedAbundance;
                //cerr << " Observed isotopeAbundance=" << observedAbundance;
                //cerr << " Error="     << naturalAbundanceError << endl;

                if (naturalAbundanceError > maxNaturalAbundanceErr )  continue;
            }


            float w = maxIsotopeScanDiff * avgScanTime;
            double c = sample->correlation(isotopeMass, parentgroup->meanMz, ppm, rtmin - w, rtmax + w);
            if (c < minIsotopicCorrelation)  continue;

            //cerr << "pullIsotopes: " << isotopeMass << " " << rtmin-w << " " <<  rtmin+w << " c=" << c << endl;

            EIC* eic = NULL;
            for ( int i = 0; i < maxIsotopeScanDiff; i++ ) {
                float window = i * avgScanTime;
                eic = sample->getEIC(mzmin, mzmax, rtmin - window, rtmax + window, 1);
                eic->setSmootherType((EIC::SmootherType) eic_smoothingAlgorithm);
                eic->getPeakPositions(eic_smoothingWindow);
                if ( eic->peaks.size() == 0 ) continue;
                if ( eic->peaks.size() > 1 ) break;
            }
            if (!eic) continue;

            Peak* nearestPeak = NULL;
            float d = FLT_MAX;
            for (int i = 0; i < eic->peaks.size(); i++ ) {
                Peak& x = eic->peaks[i];
                float dist = abs(x.rt - rt);
                if ( dist > maxIsotopeScanDiff * avgScanTime) continue;
                if ( dist < d ) {
                    d = dist;
                    nearestPeak = &x;
                }
            }

            if (nearestPeak) {
                if (isotopes.count(isotopeName) == 0) {
                    PeakGroup g;
                    g.meanMz = isotopeMass;
                    g.tagString = isotopeName;
                    g.expectedAbundance = expectedAbundance;
                    g.isotopeC13count = x.C13;
                    isotopes[isotopeName] = g;
                }
                isotopes[isotopeName].addPeak(*nearestPeak);
            }
            delete(eic);
        }
    }

    parentgroup->children.clear();
    for (itr2 = isotopes.begin(); itr2 != isotopes.end(); itr2++ ) {
        string isotopeName = (*itr2).first;
        PeakGroup& child = (*itr2).second;
        child.tagString = isotopeName;
        child.metaGroupId = parentgroup->metaGroupId;
        child.groupId = parentgroup->groupId;
        child.compound = parentgroup->compound;
        child.parent = parentgroup;
        child.setType(PeakGroup::Isotope);
        child.groupStatistics();
        if (clsf->hasModel()) {
            clsf->classify(&child);
            child.groupStatistics();
        }
        parentgroup->addChild(child);
        //cerr << " add: " << isotopeName << " " <<  child.peaks.size() << " " << isotopes.size() << endl;
    }
    //cerr << "Done: " << parentgroup->children.size();
    /*
            //if ((float) isotope.maxIntensity/parentgroup->maxIntensity > 3*ab[isotopeName]/ab["C12 PARENT"]) continue;
        */
}

void BackgroundPeakUpdate::printSettings() {
    cerr << "#Output folder=" <<  outputdir << endl;
    cerr << "#ionizationMode=" << ionizationMode << endl;
    cerr << "#keepFoundGroups=" << keepFoundGroups << endl;
    cerr << "#showProgressFlag=" << showProgressFlag << endl;

    cerr << "#rtStepSize=" << rtStepSize << endl;
    cerr << "#ppmMerge="  << ppmMerge << endl;
    cerr << "#qcut="      << qcut << endl;
    cerr << "#avgScanTime=" << avgScanTime << endl;

    //peak detection
    cerr << "#eic_smoothingWindow=" << eic_smoothingWindow << endl;

    //peak grouping across samples
    cerr << "#grouping_maxRtWindow=" << grouping_maxRtWindow << endl;

    //peak filtering criteria
    cerr << "#minGoodPeakCount=" << minGoodPeakCount << endl;
    cerr << "#minSignalBlankRatio=" << minSignalBlankRatio << endl;
    cerr << "#minNoNoiseObs=" << minNoNoiseObs << endl;
    cerr << "#minSignalBaseLineRatio=" << minSignalBaseLineRatio << endl;
    cerr << "#minGroupIntensity=" << minGroupIntensity << endl;

    //compound detection setting
    cerr << "#compoundPPMWindow=" << compoundPPMWindow << endl;
    cerr << "#compoundRTWindow=" << compoundRTWindow << endl;
}

bool BackgroundPeakUpdate::covertToMzXML(QString filename, QString outfile) {

    QFile test(outfile);
    if (test.exists()) return true;

    QString command = QString("ReAdW.exe --centroid --mzXML \"%1\" \"%2\"").
                      arg(filename).
                      arg(outfile);

    qDebug() << command;

    QProcess *process = new QProcess();
    //connect(process, SIGNAL(finished(int)), this, SLOT(doVideoCreated(int)));
    process->start(command);

    if (!process->waitForStarted()) {
        process->kill();
        return false;
    }

    while (!process->waitForFinished()) {};
    QFile testOut(outfile);
    return testOut.exists();
}

