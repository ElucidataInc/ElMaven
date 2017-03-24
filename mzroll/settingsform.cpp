#include "settingsform.h"

SettingsForm::SettingsForm(QSettings* s, MainWindow *w): QDialog(w) { 
    setupUi(this);
    settings = s;
    mainwindow = w;
    updateSettingFormGUI();
    setIsotopeAtom();

    connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(getFormValues()));

    connect(recomputeEICButton, SIGNAL(clicked(bool)), SLOT(recomputeEIC()));
    connect(eic_smoothingWindow, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));
    connect(eic_smoothingAlgorithm, SIGNAL(currentIndexChanged(int)), SLOT(recomputeEIC()));
    connect(grouping_maxRtWindow, SIGNAL(valueChanged(double)), SLOT(recomputeEIC()));
    connect(baseline_smoothing, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));
    connect(baseline_quantile, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));

    connect(ionizationMode, SIGNAL(currentIndexChanged(int)), SLOT(getFormValues()));
    connect(ionizationMode, SIGNAL(currentIndexChanged(QString)), mainwindow, SLOT(setQComboBox()));
    connect(isotopeC13Correction, SIGNAL(toggled(bool)), SLOT(getFormValues()));
    connect(amuQ1, SIGNAL(valueChanged(double)), SLOT(getFormValues()));
    connect(amuQ3, SIGNAL(valueChanged(double)), SLOT(getFormValues()));

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

    connect(showBookmarkIsotopes, SIGNAL(toggled(bool)),SLOT(showSetIsotopeAtom()));

    connect(isotopeC13Correction, SIGNAL(toggled(bool)), SLOT(recomputeIsotopes()));

    connect(maxNaturalAbundanceErr, SIGNAL(valueChanged(double)), SLOT(recomputeIsotopes()));
    connect(minIsotopicCorrelation, SIGNAL(valueChanged(double)), SLOT(recomputeIsotopes()));
    connect(maxIsotopeScanDiff, SIGNAL(valueChanged(int)), SLOT(recomputeIsotopes()));

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
}

void SettingsForm::setSettingsIonizationMode(QString ionMode) {

    if      (ionMode.contains("Neutral"))   ionizationMode->setCurrentIndex(1);
    else if (ionMode.contains("Positive"))  ionizationMode->setCurrentIndex(2);
    else if (ionMode.contains("Negative"))  ionizationMode->setCurrentIndex(3);
    else                                    ionizationMode->setCurrentIndex(0);
}

void SettingsForm::showSetIsotopeAtom() {
    setIsotopeAtom();
    cerr << mainwindow->mavenParameters->isotopeAtom["IsotopeWidget"] << " hello \n\n";
}

void SettingsForm::setIsotopeAtom() {

    if(!mainwindow) return;
    bool b = mainwindow->mavenParameters->isotopeAtom["IsotopeWidget"];
    mainwindow->mavenParameters->isotopeAtom.clear();

    if(D2Labeled_BPE->isChecked()) mainwindow->mavenParameters->isotopeAtom["D2Labeled_BPE"] = true;
    else mainwindow->mavenParameters->isotopeAtom["D2Labeled_BPE"] = false;
    
    if(C13Labeled_BPE->isChecked()) mainwindow->mavenParameters->isotopeAtom["C13Labeled_BPE"] = true;
    else mainwindow->mavenParameters->isotopeAtom["C13Labeled_BPE"] = false;

    if(N15Labeled_BPE->isChecked()) mainwindow->mavenParameters->isotopeAtom["N15Labeled_BPE"] = true;
    else mainwindow->mavenParameters->isotopeAtom["N15Labeled_BPE"] = false;

    if(S34Labeled_BPE->isChecked()) mainwindow->mavenParameters->isotopeAtom["S34Labeled_BPE"] = true;
    else mainwindow->mavenParameters->isotopeAtom["S34Labeled_BPE"] = false;

    if(showBookmarkIsotopes->isChecked()) mainwindow->mavenParameters->isotopeAtom["showBookmarkIsotopes"] = true;
    else mainwindow->mavenParameters->isotopeAtom["showBookmarkIsotopes"] = false;

    if(mainwindow->mavenParameters->pullIsotopesFlag) mainwindow->mavenParameters->isotopeAtom["ShowIsotopes"] = true;
    else mainwindow->mavenParameters->isotopeAtom["ShowIsotopes"] = false;

    mainwindow->mavenParameters->isotopeAtom["IsotopeWidget"] = b;
    
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
    if (mainwindow != NULL && mainwindow->getEicWidget() != NULL) {
        mainwindow->getEicWidget()->recompute();
        mainwindow->getEicWidget()->replot();
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
    baseline_smoothing->setValue(settings->value("baseline_smoothing").toDouble());
    baseline_quantile->setValue(settings->value("baseline_quantile").toDouble());

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
    settings->setValue("maxIsotopeScanDiff",maxIsotopeScanDiff->value());
    settings->setValue("minIsotopicCorrelation",minIsotopicCorrelation->value());
    
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
    settings->setValue("baseline_quantile", baseline_quantile->value());
    settings->setValue("baseline_smoothing", baseline_smoothing->value());

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
    settings->setValue("ionizationMode",mode);

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

