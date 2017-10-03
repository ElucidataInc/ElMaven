#include "settingsform.h"

SettingsForm::SettingsForm(QSettings* s, MainWindow *w): QDialog(w) { 
    setupUi(this);
    settings = s;
    mainwindow = w;
    deltaRtCheckFlag = false;
    updateSettingFormGUI();
    setIsotopeAtom();
    setMavenParameters();

    connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(getFormValues()));

    connect(eic_smoothingWindow, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));
    connect(eic_smoothingAlgorithm, SIGNAL(currentIndexChanged(int)), SLOT(recomputeEIC()));
    connect(grouping_maxRtWindow, SIGNAL(valueChanged(double)), SLOT(recomputeEIC()));
    connect(baseline_smoothing, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));
    connect(baseline_quantile, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));
    connect(minSignalBaselineDifference, SIGNAL(valueChanged(double)), SLOT(recomputeEIC()));
    connect(isotopicMinSignalBaselineDifference, SIGNAL(valueChanged(double)), SLOT(recomputeEIC()));

    connect(ionizationMode, SIGNAL(currentIndexChanged(int)), SLOT(getFormValues()));
    connect(ionizationMode, SIGNAL(currentIndexChanged(QString)), mainwindow, SLOT(setQComboBox()));
    connect(isotopeC13Correction, SIGNAL(toggled(bool)), SLOT(getFormValues()));
    connect(amuQ1, SIGNAL(valueChanged(double)), SLOT(getFormValues()));
    connect(amuQ3, SIGNAL(valueChanged(double)), SLOT(getFormValues()));
    connect(filterlineComboBox, SIGNAL(activated(QString)), SLOT(getFormValues()));

    //isotope detection setting
    connect(C13Labeled_BPE,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(N15Labeled_BPE,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(S34Labeled_BPE,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(D2Labeled_BPE, SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
        //isotope detection setting
    connect(C13Labeled_Barplot,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(N15Labeled_Barplot,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(S34Labeled_Barplot,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(D2Labeled_Barplot, SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(doubleSpinBoxAbThresh, SIGNAL(valueChanged(double)),SLOT(recomputeIsotopes()));
    connect(noOfIsotopes, SIGNAL(valueChanged(int)),SLOT(recomputeIsotopes()));
        //isotope detection setting
    connect(C13Labeled_IsoWidget,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(N15Labeled_IsoWidget,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(S34Labeled_IsoWidget,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(D2Labeled_IsoWidget, SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));

    connect(isotopeC13Correction, SIGNAL(toggled(bool)), SLOT(recomputeIsotopes()));

    connect(maxNaturalAbundanceErr, SIGNAL(valueChanged(double)), SLOT(recomputeIsotopes()));
    connect(minIsotopicCorrelation, SIGNAL(valueChanged(double)), SLOT(recomputeIsotopes()));
    connect(maxIsotopeScanDiff, SIGNAL(valueChanged(int)), SLOT(recomputeIsotopes()));

    connect(eicTypeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(recomputeEIC()));

    //Peak Grouping Tab
    setWeightStatus();
    connect(distXSlider, SIGNAL(valueChanged(int)), SLOT(setWeightStatus()));
    connect(distXSlider, SIGNAL(valueChanged(int)), SLOT(getFormValues()));
    connect(distXSlider, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));
    connect(distYSlider, SIGNAL(valueChanged(int)), SLOT(setWeightStatus()));
    connect(distYSlider, SIGNAL(valueChanged(int)), SLOT(getFormValues()));
    connect(distYSlider, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));
    connect(minPeakDiffComboBox, SIGNAL(valueChanged(double)), SLOT(getFormValues()));
    connect(minPeakDiffComboBox, SIGNAL(valueChanged(double)), SLOT(recomputeEIC()));
    connect(useOverlap, SIGNAL(stateChanged(int)), SLOT(getFormValues()));
    connect(useOverlap, SIGNAL(stateChanged(int)), SLOT(recomputeEIC()));
    connect(useOverlap, SIGNAL(stateChanged(int)), SLOT(toggleOverlap()));
    toggleOverlap();

    //remote url used to fetch compound lists, pathways, and notes
    connect(data_server_url, SIGNAL(textChanged(QString)), SLOT(getFormValues()));
    connect(scriptsFolderSelect, SIGNAL(clicked()), SLOT(selectScriptsFolder()));
    connect(pathwaysFolderSelect, SIGNAL(clicked()), SLOT(selectPathwaysFolder()));
    connect(methodsFolderSelect, SIGNAL(clicked()), SLOT(selectMethodsFolder()));
    connect(RProgramSelect, SIGNAL(clicked()), SLOT(selectRProgram()));
    connect(rawExtractSelect, SIGNAL(clicked()), SLOT(selectRawExtractor()));


    connect(centroid_scan_flag,SIGNAL(toggled(bool)), SLOT(getFormValues()));
    connect(scan_filter_min_quantile, SIGNAL(valueChanged(int)), SLOT(getFormValues()));
    connect(scan_filter_min_intensity, SIGNAL(valueChanged(int)), SLOT(getFormValues()));
    connect(ionizationType,SIGNAL(currentIndexChanged(int)),SLOT(getFormValues()));

    //Multiprocessing
    connect(checkBoxMultiprocessing,SIGNAL(toggled(bool)),SLOT(updateMultiprocessing()));

    //Group Rank
    setGroupRankStatus();
    connect(qualityWeight, SIGNAL(valueChanged(int)), this,SLOT(setGroupRankStatus()));
    connect(qualityWeight, SIGNAL(valueChanged(int)), this,SLOT(getFormValues()));
    connect(qualityWeight, SIGNAL(valueChanged(int)), this,SLOT(recomputeEIC()));
    connect(intensityWeight,SIGNAL(valueChanged(int)), this,SLOT(setGroupRankStatus()));
    connect(intensityWeight, SIGNAL(valueChanged(int)), this,SLOT(getFormValues()));
    connect(intensityWeight, SIGNAL(valueChanged(int)), this,SLOT(recomputeEIC()));
    connect(deltaRTWeight,SIGNAL(valueChanged(int)), this,SLOT(setGroupRankStatus()));
    connect(deltaRTWeight, SIGNAL(valueChanged(int)), this,SLOT(getFormValues()));
    connect(deltaRTWeight, SIGNAL(valueChanged(int)), this,SLOT(recomputeEIC()));
    connect(deltaRTCheck, SIGNAL(toggled(bool)), SLOT(toggleDeltaRtWeight()));
    connect(deltaRTCheck, SIGNAL(toggled(bool)), this,SLOT(getFormValues()));
    toggleDeltaRtWeight();
}

void SettingsForm::setSettingsIonizationMode(QString ionMode) {

    if      (ionMode.contains("Neutral"))   ionizationMode->setCurrentIndex(1);
    else if (ionMode.contains("Positive"))  ionizationMode->setCurrentIndex(2);
    else if (ionMode.contains("Negative"))  ionizationMode->setCurrentIndex(3);
    else                                    ionizationMode->setCurrentIndex(0);
}

void SettingsForm::setIsotopeAtom() {

    if(!mainwindow) return;
    mainwindow->mavenParameters->isotopeAtom.clear();

    if(D2Labeled_BPE->isChecked()) mainwindow->mavenParameters->isotopeAtom["D2Labeled_BPE"] = true;
    else mainwindow->mavenParameters->isotopeAtom["D2Labeled_BPE"] = false;
    
    if(C13Labeled_BPE->isChecked()) mainwindow->mavenParameters->isotopeAtom["C13Labeled_BPE"] = true;
    else mainwindow->mavenParameters->isotopeAtom["C13Labeled_BPE"] = false;

    if(N15Labeled_BPE->isChecked()) mainwindow->mavenParameters->isotopeAtom["N15Labeled_BPE"] = true;
    else mainwindow->mavenParameters->isotopeAtom["N15Labeled_BPE"] = false;

    if(S34Labeled_BPE->isChecked()) mainwindow->mavenParameters->isotopeAtom["S34Labeled_BPE"] = true;
    else mainwindow->mavenParameters->isotopeAtom["S34Labeled_BPE"] = false;

    if(mainwindow->mavenParameters->pullIsotopesFlag) mainwindow->mavenParameters->isotopeAtom["ShowIsotopes"] = true;
    else mainwindow->mavenParameters->isotopeAtom["ShowIsotopes"] = false;
    
}

void SettingsForm::setWeightStatus() {

    // slider int values to double
    double distX = distXSlider->value()*1.0;
    double distY = distYSlider->value()*1.0;

    // normalizing slider values from 0-100 (int) to 0-10 (double) 
    distX = distX/10;
    distY = distY/10;

    // updating slider status
    distXStatus->setText(QString::number(distX));
    distYStatus->setText(QString::number(distY));

}

void SettingsForm::toggleOverlap() {
    bool statusOverlap;
    if (useOverlap->checkState() > 0) {
        statusOverlap = true;
    }
    else {
        statusOverlap = false;
    }
    formulaWithOverlap->setVisible(statusOverlap);
    formulaWithoutOverlap->setVisible(!statusOverlap);
}

void SettingsForm::recomputeIsotopes() { 
    getFormValues();
    if (!mainwindow) return;
    setIsotopeAtom();

    //update isotope plot in EICview
    if (mainwindow->getEicWidget()->isVisible()) {
        PeakGroup* group = mainwindow->getEicWidget()->getParameters()->getSelectedGroup();
        mainwindow->isotopeWidget->updateIsotopicBarplot(group);
        mainwindow->isotopeWidget->setPeakGroupAndMore(group, false);
    }

    //update isotopes in pathwayview
    if (mainwindow->pathwayWidget ) {
        if ( mainwindow->pathwayWidget->isVisible()) {
            mainwindow->pathwayWidget->recalculateConcentrations();
        }
    }
}

void SettingsForm::updateMultiprocessing() { 
    settings->setValue("uploadMultiprocessing", checkBoxMultiprocessing->checkState());
}

void SettingsForm::recomputeEIC() {
    LOGD; 
    getFormValues();

    PeakGroup* previousGroup = mainwindow->getEicWidget()->getParameters()->getSelectedGroup();
    float rt = 0;

    if (previousGroup) {
        rt = previousGroup->meanRt;
    }

    if (mainwindow != NULL && mainwindow->getEicWidget() != NULL) {
        mainwindow->getEicWidget()->recompute();
        mainwindow->getEicWidget()->replot();
        if (rt != 0) {
            mainwindow->getEicWidget()->selectGroupNearRt(rt);
        }
    }
}

void SettingsForm::updateSmoothingWindowValue(double value) {
    settings->setValue("eic_smoothingWindow",value);
    eic_smoothingWindow->setValue(value);
    recomputeEIC();
}


void SettingsForm::updateSettingFormGUI() {
   // qDebug() << "SettingsForm::updateSettingFormGUI()";

    if (settings == NULL) return;
    eic_smoothingAlgorithm->setCurrentIndex(settings->value("eic_smoothingAlgorithm").toInt());
    eic_smoothingWindow->setValue(settings->value("eic_smoothingWindow").toDouble());
    grouping_maxRtWindow->setValue(settings->value("grouping_maxRtWindow").toDouble());
    maxNaturalAbundanceErr->setValue(settings->value("maxNaturalAbundanceErr").toDouble());
    maxIsotopeScanDiff->setValue(settings->value("maxIsotopeScanDiff").toDouble());
    minIsotopicCorrelation->setValue(settings->value("minIsotopicCorrelation").toDouble());
    baseline_smoothing->setValue(settings->value("baseline_smoothing").toInt());
    baseline_quantile->setValue(settings->value("baseline_quantile").toInt());
    minSignalBaselineDifference->setValue(settings->value("minSignalBaselineDifference").toInt());
    isotopicMinSignalBaselineDifference->setValue(settings->value("isotopicMinSignalBaselineDifference").toInt());
    //Upload Multiprocessing
    checkBoxMultiprocessing->setCheckState( (Qt::CheckState) settings->value("uploadMultiprocessing").toInt() );

    C13Labeled_BPE->setCheckState( (Qt::CheckState) settings->value("C13Labeled_BPE").toInt() );
    N15Labeled_BPE->setCheckState( (Qt::CheckState) settings->value("N15Labeled_BPE").toInt()  );
    S34Labeled_BPE->setCheckState( (Qt::CheckState) settings->value("S34Labeled_BPE").toInt() );
    D2Labeled_BPE->setCheckState(  (Qt::CheckState) settings->value("D2Labeled_BPE").toInt()  );
    
    C13Labeled_Barplot->setCheckState( (Qt::CheckState) settings->value("C13Labeled_Barplot").toInt() );
    N15Labeled_Barplot->setCheckState( (Qt::CheckState) settings->value("N15Labeled_Barplot").toInt()  );
    S34Labeled_Barplot->setCheckState( (Qt::CheckState) settings->value("S34Labeled_Barplot").toInt() );
    D2Labeled_Barplot->setCheckState(  (Qt::CheckState) settings->value("D2Labeled_Barplot").toInt()  );
    doubleSpinBoxAbThresh->setValue(settings->value("AbthresholdBarplot").toDouble());
    noOfIsotopes->setValue(settings->value("noOfIsotopes").toInt());

    C13Labeled_IsoWidget->setCheckState( (Qt::CheckState) settings->value("C13Labeled_IsoWidget").toInt() );
    N15Labeled_IsoWidget->setCheckState( (Qt::CheckState) settings->value("N15Labeled_IsoWidget").toInt()  );
    S34Labeled_IsoWidget->setCheckState( (Qt::CheckState) settings->value("S34Labeled_IsoWidget").toInt() );
    D2Labeled_IsoWidget->setCheckState(  (Qt::CheckState) settings->value("D2Labeled_IsoWidget").toInt()  );


    isotopeC13Correction->setCheckState(  (Qt::CheckState) settings->value("isotopeC13Correction").toInt()  );

    eicTypeComboBox->setCurrentIndex(settings->value("eicTypeComboBox").toInt());

    //peak grouping tab
    if (settings->contains("distXWeight"))
    distXSlider->setValue(settings->value("distXWeight").toFloat()*10);

    if (settings->contains("distYWeight"))
    distYSlider->setValue(settings->value("distYWeight").toFloat()*10);

    if (settings->contains("minPeakRtDiff"))
    minPeakDiffComboBox->setValue(settings->value("minPeakRtDiff").toFloat());

    if (settings->contains("useOverlap"))
    useOverlap->setCheckState( (Qt::CheckState) settings->value("useOverlap").toInt());

    //group rank tab
    if (settings->contains("qualityWeight"))
    qualityWeight->setValue(settings->value("qualityWeight").toInt());

    if (settings->contains("intensityWeight"))
    intensityWeight->setValue(settings->value("intensityWeight").toInt());

    if (settings->contains("deltaRTWeight"))
    deltaRTWeight->setValue(settings->value("deltaRTWeight").toInt());

    centroid_scan_flag->setCheckState( (Qt::CheckState) settings->value("centroid_scan_flag").toInt());
    scan_filter_min_intensity->setValue( settings->value("scan_filter_min_intensity").toInt());
    scan_filter_min_quantile->setValue(  settings->value("scan_filter_min_quantile").toInt());

    QList<QLineEdit*> items;    items  << scriptsFolder << methodsFolder << pathwaysFolder << Rprogram << RawExtractProgram;
    QStringList pathlist;        pathlist << "scriptsFolder" << "methodsFolder" << "pathwaysFolder" << "Rprogram" << "RawExtractProgram";

   unsigned int itemCount=0;
    Q_FOREACH(QString itemName, pathlist) {
        if(settings->contains(itemName)) items[itemCount]->setText( settings->value(itemName).toString());
        itemCount++;
    }

    if(settings->contains("data_server_url"))
        data_server_url->setText( settings->value("data_server_url").toString());

    if(settings->contains("centroid_scan_flag"))
        centroid_scan_flag->setCheckState( (Qt::CheckState) settings->value("centroid_scan_flag").toInt());

    if(settings->contains("scan_filter_min_intensity"))
        scan_filter_min_intensity->setValue( settings->value("scan_filter_min_intensity").toInt());

    if(settings->contains("scan_filter_min_quantile"))
        scan_filter_min_quantile->setValue( settings->value("scan_filter_min_quantile").toInt());
}


void SettingsForm::getFormValues() {
    LOGD;
    if (settings == NULL) return;
    //qDebug() << "SettingsForm::getFormValues() ";


    settings->setValue("eic_smoothingAlgorithm",eic_smoothingAlgorithm->currentIndex());
    settings->setValue("eic_smoothingWindow",eic_smoothingWindow->value());
    settings->setValue("grouping_maxRtWindow",grouping_maxRtWindow->value());
    settings->setValue("maxNaturalAbundanceErr",maxNaturalAbundanceErr->value());
    settings->setValue("baseline_quantile", baseline_quantile->value());
    settings->setValue("baseline_smoothing", baseline_smoothing->value());
    settings->setValue("maxIsotopeScanDiff",maxIsotopeScanDiff->value());
    settings->setValue("minIsotopicCorrelation",minIsotopicCorrelation->value());
    settings->setValue("minSignalBaselineDifference", minSignalBaselineDifference->value());
    settings->setValue("isotopicMinSignalBaselineDifference",isotopicMinSignalBaselineDifference->value());

    /*Isotopic settings for barplot*/
    settings->setValue("C13Labeled_Barplot", C13Labeled_Barplot->checkState());
    settings->setValue("N15Labeled_Barplot", N15Labeled_Barplot->checkState());
    settings->setValue("S34Labeled_Barplot", S34Labeled_Barplot->checkState());
    settings->setValue("D2Labeled_Barplot", D2Labeled_Barplot->checkState());
    settings->setValue("AbthresholdBarplot",  doubleSpinBoxAbThresh->value());
    settings->setValue("noOfIsotopes", noOfIsotopes->value());

    /*Isotopic settings for bookmark, peak detection and save csv*/
    settings->setValue("C13Labeled_BPE", C13Labeled_BPE->checkState());
    settings->setValue("N15Labeled_BPE", N15Labeled_BPE->checkState());
    settings->setValue("S34Labeled_BPE", S34Labeled_BPE->checkState());
    settings->setValue("D2Labeled_BPE", D2Labeled_BPE->checkState());

    /*Isotopic settings for bookmark, peak detection and save csv*/
    settings->setValue("C13Labeled_IsoWidget", C13Labeled_IsoWidget->checkState());
    settings->setValue("N15Labeled_IsoWidget", N15Labeled_IsoWidget->checkState());
    settings->setValue("S34Labeled_IsoWidget", S34Labeled_IsoWidget->checkState());
    settings->setValue("D2Labeled_IsoWidget", D2Labeled_IsoWidget->checkState());

    settings->setValue("isotopeC13Correction", isotopeC13Correction->checkState()  );
    settings->setValue("amuQ1", amuQ1->value());
    settings->setValue("amuQ3", amuQ3->value());
    settings->setValue("filterlineComboBox", filterlineComboBox->currentText());

    settings->setValue("eicTypeComboBox",eicTypeComboBox->currentIndex());

    settings->setValue("centroid_scan_flag", centroid_scan_flag->checkState() );
    settings->setValue("scan_filter_min_intensity", scan_filter_min_intensity->value());
    settings->setValue("scan_filter_min_quantile", scan_filter_min_quantile->value());

    settings->setValue("data_server_url", data_server_url->text());

    settings->setValue("centroid_scan_flag", centroid_scan_flag->checkState());
    settings->setValue("scan_filter_min_intensity", scan_filter_min_intensity->value());
    settings->setValue("scan_filter_min_quantile", scan_filter_min_quantile->value());



    //change ionization mode
    int mode=0;
    if (ionizationMode->currentText().contains("+1") )     mode=1;
    else if ( ionizationMode->currentText().contains("-1") ) mode=-1;
    else if (ionizationMode->currentText().contains("Auto Detect") && !mainwindow->samples.empty()) mode=mainwindow->samples[0]->getPolarity();
    if(mainwindow) mainwindow->setIonizationMode(mode);

    //change ionization type

    if (ionizationType->currentText() == "EI")  MassCalculator::ionizationType = MassCalculator::EI;
    else MassCalculator::ionizationType = MassCalculator::ESI;

    //peak grouping tab
    settings->setValue("distXWeight", (distXSlider->value()*1.0)/10);
    settings->setValue("distYWeight", (distYSlider->value()*1.0)/10);
    settings->setValue("minPeakRtDiff", minPeakDiffComboBox->value());
    settings->setValue("useOverlap", useOverlap->checkState());

    mzSample::setFilter_centroidScans( centroid_scan_flag->checkState() == Qt::Checked );
    mzSample::setFilter_minIntensity( scan_filter_min_intensity->value() );
    mzSample::setFilter_intensityQuantile( scan_filter_min_quantile->value());

    if( scan_filter_polarity->currentText().contains("Positive") ) {
    	mzSample::setFilter_polarity(+1);
    } else if ( scan_filter_polarity->currentText().contains("Negative")  ) {
    	mzSample::setFilter_polarity(-1);
    } else {
    	mzSample::setFilter_polarity(0);
    }

    if( scan_filter_mslevel->currentText().contains("MS1") ) {
    	mzSample::setFilter_mslevel(1);
    } else if ( scan_filter_mslevel->currentText().contains("MS2")  ) {
    	mzSample::setFilter_mslevel(2);
    } else {
    	mzSample::setFilter_mslevel(0);
    }

    //group rank tab
    settings->setValue("qualityWeight", qualityWeight->value());
    settings->setValue("intensityWeight", intensityWeight->value());
    settings->setValue("deltaRTWeight", deltaRTWeight->value());
    settings->setValue("deltaRTCheckFlag", deltaRTCheck->isChecked());
    setMavenParameters();
}

void SettingsForm::show() {
    if (mainwindow == NULL) return;

    if (mainwindow->ligandWidget) {
        connect(qualityWeight, SIGNAL(valueChanged(int)), mainwindow->ligandWidget, SLOT(showLigand()));
        connect(intensityWeight, SIGNAL(valueChanged(int)), mainwindow->ligandWidget, SLOT(showLigand()));
        connect(deltaRTWeight, SIGNAL(valueChanged(int)), mainwindow->ligandWidget, SLOT(showLigand()));
    }
}

void SettingsForm::setGroupRankStatus() {
    qualityWeightStatus->setText(QString::number((double) qualityWeight->value()/10, 'f', 1));
    intensityWeightStatus->setText(QString::number((double) intensityWeight->value()/10, 'f', 1));
    deltaRTWeightStatus->setText(QString::number((double) deltaRTWeight->value()/10, 'f', 1));
}

void SettingsForm::setInitialGroupRank() {
    qualityWeight->setSliderPosition(mainwindow->mavenParameters->qualityWeight);
    intensityWeight->setSliderPosition(mainwindow->mavenParameters->intensityWeight);
    deltaRTWeight->setSliderPosition(mainwindow->mavenParameters->deltaRTWeight);
    setGroupRankStatus();
    toggleDeltaRtWeight();
}

void SettingsForm::toggleDeltaRtWeight() {
    if (deltaRTCheck->isChecked()) {
        deltaRtCheckFlag = true;
    }
    else {
        deltaRtCheckFlag = false;
    }
    settings->setValue("deltaRtCheckFlag", deltaRtCheckFlag);
    deltaRTWeight->setEnabled(deltaRtCheckFlag);
    deltaRTWeightStatus->setEnabled(deltaRtCheckFlag);
    label_drtWeight->setEnabled(deltaRtCheckFlag);
    formulaWithRt->setVisible(deltaRtCheckFlag);
    formulaWithoutRt->setVisible(!deltaRtCheckFlag);
}

void SettingsForm::setMavenParameters() {

    MavenParameters* mavenParameters = mainwindow->mavenParameters;

    if (settings != NULL) {

        if (settings->value("filterlineComboBox") == "All") {
            mavenParameters->filterline = "";
        } else {
            mavenParameters->filterline = settings->value("filterlineComboBox").toString().toStdString();
        }

        mavenParameters->eicType = settings->value("eicTypeComboBox").toInt();
        //peak grouping tab
        mavenParameters->distXWeight = settings->value("distXWeight").toFloat();
        mavenParameters->distYWeight = settings->value("distYWeight").toFloat();
        mavenParameters->minPeakRtDiff = settings->value("minPeakRtDiff").toFloat();
        mavenParameters->useOverlap = false;
        if (settings->value("useOverlap").toInt() > 0) mavenParameters->useOverlap = true;

        //group rank tab
        mavenParameters->qualityWeight = settings->value("qualityWeight").toInt();
        mavenParameters->intensityWeight = settings->value("intensityWeight").toInt();
        mavenParameters->deltaRTWeight = settings->value("deltaRTWeight").toInt();
        mavenParameters->deltaRtCheckFlag = settings->value("deltaRtCheckFlag").toBool();

        //EIC Processing: Baseline Calculation and Smoothing
        mavenParameters->eic_smoothingAlgorithm = settings->value(
                "eic_smoothingAlgorithm").toInt();
        mavenParameters->eic_smoothingWindow = settings->value("eic_smoothingWindow").toDouble();

        mavenParameters->grouping_maxRtWindow = settings->value("grouping_maxRtWindow").toDouble();
        mavenParameters->baseline_smoothingWindow = settings->value("baseline_smoothing").toDouble();
        mavenParameters->baseline_dropTopX = settings->value("baseline_quantile").toDouble();
        mavenParameters->minSignalBaselineDifference = settings->value("minSignalBaselineDifference").toDouble();
        mavenParameters->isotopicMinSignalBaselineDifference=settings->value("isotopicMinSignalBaselineDifference").toDouble();

    }
}

void SettingsForm::selectFolder(QString key) {
    QString oFolder = ".";
    if(settings->contains(key)) oFolder =  settings->value(key).toString();
    QString newFolder = QFileDialog::getExistingDirectory(this,oFolder);
    if (! newFolder.isEmpty()) {
        settings->setValue(key,newFolder);
        updateSettingFormGUI();
    }
}

void SettingsForm::selectFile(QString key) {
    QString oFile = ".";
    if(settings->contains(key)) oFile =  settings->value(key).toString();
    QString newFile = QFileDialog::getOpenFileName(this,"Select file",".","*.exe");
    if (!newFile.isEmpty()) {
        settings->setValue(key,newFile);
        updateSettingFormGUI();
    }
}

void SettingsForm::setNumericValue(QString key, double value) {
    if(settings->contains(key)) qDebug() << "Changing " << key << " value to" << value;
     settings->setValue(key,value);
}

void SettingsForm::setStringValue(QString key, QString value) {
      if(settings->contains(key)) qDebug() << "Changing " << key << " value to" << value;
     settings->setValue(key,value);
}

