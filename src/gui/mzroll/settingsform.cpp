#include "settingsform.h"

OptionsDialogSettings::OptionsDialogSettings(SettingsForm* dialog): sf(dialog)
{

    settings.insert("ionizationMode", QVariant::fromValue(sf->ionizationMode));
    settings.insert("amuQ1", QVariant::fromValue(sf->amuQ1));
    settings.insert("amuQ3", QVariant::fromValue(sf->amuQ3));
    settings.insert("eic_smoothingAlgorithm", QVariant::fromValue(sf->eic_smoothingAlgorithm));
    settings.insert("eic_smoothingWindow", QVariant::fromValue(sf->eic_smoothingWindow));
    settings.insert("grouping_maxRtWindow", QVariant::fromValue(sf->grouping_maxRtWindow));

    settings.insert("baseline_quantile", QVariant::fromValue(sf->baseline_quantile));
    settings.insert("baseline_smoothing", QVariant::fromValue(sf->baseline_smoothing));

    settings.insert("isIsotopeEqualPeakFilter", QVariant::fromValue(sf->isIsotopeEqualPeakFilter));
    settings.insert("minSignalBaselineDifference", QVariant::fromValue(sf->minSignalBaselineDifference));
    settings.insert("isotopicMinSignalBaselineDifference", QVariant::fromValue(sf->isotopicMinSignalBaselineDifference));
    settings.insert("minPeakQuality", QVariant::fromValue(sf->minPeakQuality));
    settings.insert("minIsotopicPeakQuality", QVariant::fromValue(sf->minIsotopicPeakQuality));

    settings.insert("D2Labeled_BPE", QVariant::fromValue(sf->D2Labeled_BPE));
    settings.insert("C13Labeled_BPE", QVariant::fromValue(sf->C13Labeled_BPE));
    settings.insert("N15Labeled_BPE", QVariant::fromValue(sf->N15Labeled_BPE));
    settings.insert("S34Labeled_BPE", QVariant::fromValue(sf->S34Labeled_BPE));

    settings.insert("D2Labeled_Barplot", QVariant::fromValue(sf->D2Labeled_Barplot));
    settings.insert("C13Labeled_Barplot", QVariant::fromValue(sf->C13Labeled_Barplot));
    settings.insert("N15Labeled_Barplot", QVariant::fromValue(sf->N15Labeled_Barplot));
    settings.insert("S34Labeled_Barplot", QVariant::fromValue(sf->S34Labeled_Barplot));

    settings.insert("D2Labeled_IsoWidget", QVariant::fromValue(sf->D2Labeled_IsoWidget));
    settings.insert("C13Labeled_IsoWidget", QVariant::fromValue(sf->C13Labeled_IsoWidget));
    settings.insert("N15Labeled_IsoWidget", QVariant::fromValue(sf->N15Labeled_IsoWidget));
    settings.insert("S34Labeled_IsoWidget", QVariant::fromValue(sf->S34Labeled_IsoWidget));

    settings.insert("isotopeC13Correction", QVariant::fromValue(sf->isotopeC13Correction));
    settings.insert("minIsotopicCorrelation", QVariant::fromValue(sf->minIsotopicCorrelation));
    settings.insert("maxIsotopeScanDiff", QVariant::fromValue(sf->maxIsotopeScanDiff));
    settings.insert("maxNaturalAbundanceErr", QVariant::fromValue(sf->maxNaturalAbundanceErr));

    settings.insert("eicType", QVariant::fromValue(sf->eicTypeComboBox));
    settings.insert("useOverlap", QVariant::fromValue(sf->useOverlap));
    settings.insert("distXWeight", QVariant::fromValue(sf->distXSlider));
    settings.insert("distYWeight", QVariant::fromValue(sf->distYSlider));

    settings.insert("overlapWeight", QVariant::fromValue(sf->overlapSlider));
    settings.insert("qualityWeight", QVariant::fromValue(sf->qualityWeight));
    settings.insert("intensityWeight", QVariant::fromValue(sf->intensityWeight));
    settings.insert("deltaRTWeight", QVariant::fromValue(sf->deltaRTWeight));
    settings.insert("deltaRTCheck", QVariant::fromValue(sf->deltaRTCheck));

}

void OptionsDialogSettings::updateOptionsDialog(string key, string value)
{

        if(settings.find(QString(key.c_str())) != settings.end() && !value.empty()) {

        const QVariant& v = settings[QString(key.c_str())];
        // convert the val to proper type;
        if(QString(v.typeName()).contains("QDoubleSpinBox"))
            v.value<QDoubleSpinBox*>()->setValue(std::stod(value));

        if(QString(v.typeName()).contains("QGroupBox"))
            v.value<QGroupBox*>()->setChecked(std::stod(value));

        if(QString(v.typeName()).contains("QCheckBox"))
            v.value<QCheckBox*>()->setChecked(std::stod(value));

        if(QString(v.typeName()).contains("QSpinBox"))
            v.value<QSpinBox*>()->setValue(std::stod(value));

        if(QString(v.typeName()).contains("QSlider"))
            v.value<QSlider*>()->setValue(std::stod(value));

        if(QString(v.typeName()).contains("QComboBox"))
            v.value<QComboBox*>()->setCurrentIndex(std::stoi(value));
    }
}



SettingsForm::SettingsForm(QSettings* s, MainWindow *w): QDialog(w) { 
    setupUi(this);
    settings = s;
    mainwindow = w;
    deltaRtCheckFlag = false;
    updateSettingFormGUI();
    setMavenParameters();

    odSettings = new OptionsDialogSettings(this);

    connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(getFormValues()));

    connect(eic_smoothingWindow, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));
    connect(eic_smoothingAlgorithm, SIGNAL(currentIndexChanged(int)), SLOT(recomputeEIC()));
    connect(grouping_maxRtWindow, SIGNAL(valueChanged(double)), SLOT(recomputeEIC()));
    connect(baseline_smoothing, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));
    connect(baseline_quantile, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));

    connect(isIsotopeEqualPeakFilter, SIGNAL(toggled(bool)), SLOT(setIsotopicPeakFiltering()));
    connect(isIsotopeEqualPeakFilter, SIGNAL(toggled(bool)), SLOT(recomputeEIC()));
    connect(minSignalBaselineDifference, SIGNAL(valueChanged(double)), SLOT(setIsotopicPeakFiltering()));
    connect(minSignalBaselineDifference, SIGNAL(valueChanged(double)), SLOT(recomputeEIC()));
    connect(isotopicMinSignalBaselineDifference, SIGNAL(valueChanged(double)), SLOT(setIsotopicPeakFiltering()));
    connect(isotopicMinSignalBaselineDifference, SIGNAL(valueChanged(double)), SLOT(recomputeEIC()));
    connect(minPeakQuality, SIGNAL(valueChanged(double)), SLOT(setIsotopicPeakFiltering()));
    connect(minPeakQuality, SIGNAL(valueChanged(double)), SLOT(recomputeEIC()));
    connect(minIsotopicPeakQuality, SIGNAL(valueChanged(double)), SLOT(setIsotopicPeakFiltering()));
    connect(minIsotopicPeakQuality, SIGNAL(valueChanged(double)), SLOT(recomputeEIC()));

    connect(ionizationMode, SIGNAL(currentIndexChanged(int)), SLOT(getFormValues()));
    connect(ionizationMode, SIGNAL(currentIndexChanged(QString)), mainwindow, SLOT(setIonizationModeLabel()));
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
    connect(overlapSlider, SIGNAL(valueChanged(int)), SLOT(setWeightStatus()));
    connect(overlapSlider, SIGNAL(valueChanged(int)), SLOT(getFormValues()));
    connect(overlapSlider, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));
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

    connect(this,&SettingsForm::settingsChanged, odSettings, &OptionsDialogSettings::updateOptionsDialog);
    connect(this, &QDialog::rejected, this, &SettingsForm::dialogRejected);
}

void SettingsForm::setIsotopicPeakFiltering()
{
    if(isIsotopeEqualPeakFilter->isChecked())
    {
        isotopicMinSignalBaselineDifference->setValue(minSignalBaselineDifference->value());
        minIsotopicPeakQuality->setValue(minPeakQuality->value());
    }
}

void SettingsForm::closeEvent(QCloseEvent* event)
{
     getFormValues();
    emit updateSettings(odSettings);
     QDialog::closeEvent(event);
}

void SettingsForm::dialogRejected()
{
  // happens when user presses 'esc' key
  emit updateSettings(odSettings);
}

void SettingsForm::setSettingsIonizationMode(QString ionMode) {

    if      (ionMode.contains("Neutral"))   ionizationMode->setCurrentIndex(1);
    else if (ionMode.contains("Positive"))  ionizationMode->setCurrentIndex(2);
    else if (ionMode.contains("Negative"))  ionizationMode->setCurrentIndex(3);
    else                                    ionizationMode->setCurrentIndex(0);
}

void SettingsForm::setWeightStatus() {

    // slider int values to double
    double distX = distXSlider->value()*1.0;
    double distY = distYSlider->value()*1.0;
    double overlap = overlapSlider->value()*1.0;

    // normalizing slider values from 0-100 (int) to 0-10 (double) 
    distX = distX/10;
    distY = distY/10;
    overlap = overlap/10;

    // updating slider status
    distXStatus->setText(QString::number(distX));
    distYStatus->setText(QString::number(distY));
    overlapStatus->setText(QString::number(overlap));

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
    overlapSlider->setEnabled(statusOverlap);
    label_30->setEnabled(statusOverlap);
    overlapStatus->setEnabled(statusOverlap);
}

void SettingsForm::recomputeIsotopes() { 
    getFormValues();
    if (!mainwindow) return;

    //update isotope plot in EICview
    if (mainwindow->getEicWidget()->isVisible()) {
        PeakGroup* group = mainwindow->getEicWidget()->getParameters()->getSelectedGroup();
        if (group)
        {
            mainwindow->isotopeWidget->updateIsotopicBarplot(group);
            mainwindow->isotopeWidget->setPeakGroupAndMore(group, false);
        }
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

void SettingsForm::updateSmoothingWindowValue(int value) {

    eic_smoothingWindow->setValue(value);
    recomputeEIC();
}


void SettingsForm::updateSettingFormGUI() {
   // qDebug() << "SettingsForm::updateSettingFormGUI()";

    if (settings == NULL) return;
    //Upload Multiprocessing
    checkBoxMultiprocessing->setCheckState( (Qt::CheckState) settings->value("uploadMultiprocessing").toInt() );


    doubleSpinBoxAbThresh->setValue(settings->value("AbthresholdBarplot").toDouble());

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
    
    if (settings == NULL) return;
    //qDebug() << "SettingsForm::getFormValues() ";



    /*Isotopic settings for barplot*/
    settings->setValue("AbthresholdBarplot",  doubleSpinBoxAbThresh->value());




    settings->setValue("filterlineComboBox", filterlineComboBox->currentText());


    settings->setValue("centroid_scan_flag", centroid_scan_flag->checkState() );
    settings->setValue("scan_filter_min_intensity", scan_filter_min_intensity->value());
    settings->setValue("scan_filter_min_quantile", scan_filter_min_quantile->value());

    settings->setValue("data_server_url", data_server_url->text());

    settings->setValue("centroid_scan_flag", centroid_scan_flag->checkState());
    settings->setValue("scan_filter_min_intensity", scan_filter_min_intensity->value());
    settings->setValue("scan_filter_min_quantile", scan_filter_min_quantile->value());



    //change ionization mode

    //change ionization type

    if (ionizationType->currentText() == "EI")  MassCalculator::ionizationType = MassCalculator::EI;
    else MassCalculator::ionizationType = MassCalculator::ESI;

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

