#include "settingsform.h"

SettingsForm::SettingsForm(QSettings* s, MainWindow *w): QDialog(w) { 
    setupUi(this);
    settings = s;
    mainwindow = w;
    setFormValues();


    connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(getFormValues()));

    connect(recomputeEICButton, SIGNAL(clicked(bool)), SLOT(recomputeEIC()));
    connect(eic_smoothingWindow, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));
    connect(eic_smoothingAlgorithm, SIGNAL(currentIndexChanged(int)), SLOT(recomputeEIC()));
    connect(grouping_maxRtWindow, SIGNAL(valueChanged(double)), SLOT(recomputeEIC()));
    connect(baseline_smoothing, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));
    connect(baseline_quantile, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));

    connect(ionizationMode, SIGNAL(currentIndexChanged(int)), SLOT(getFormValues()));
    connect(isotopeC13Correction, SIGNAL(toggled(bool)), SLOT(getFormValues()));
    connect(amuQ1, SIGNAL(valueChanged(double)), SLOT(getFormValues()));
    connect(amuQ3, SIGNAL(valueChanged(double)), SLOT(getFormValues()));

    //isotope detection setting
    connect(C13Labeled,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(N15Labeled,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(S34Labeled,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(D2Labeled, SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
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

    connect(centroid_scan_flag,SIGNAL(toggled(bool)), SLOT(getFormValues()));
    connect(scan_filter_min_quantile, SIGNAL(valueChanged(int)), SLOT(getFormValues()));
    connect(scan_filter_min_intensity, SIGNAL(valueChanged(int)), SLOT(getFormValues()));
    connect(ionizationType,SIGNAL(currentIndexChanged(int)),SLOT(getFormValues()));

}

void SettingsForm::recomputeIsotopes() { 
    getFormValues();
    if (!mainwindow) return;

    //update isotope plot in EICview
    if (mainwindow->getEicWidget()->isVisible()) {
        PeakGroup* group =mainwindow->getEicWidget()->getSelectedGroup();
        cerr << "recomputeIsotopes() " << group << endl;
        mainwindow->isotopeWidget->setPeakGroup(group);
    }

    //update isotopes in pathwayview
    if (mainwindow->pathwayWidget ) {
        if ( mainwindow->pathwayWidget->isVisible()) {
            mainwindow->pathwayWidget->recalculateConcentrations();
        }
    }
}

void SettingsForm::recomputeEIC() { 
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


void SettingsForm::setFormValues() {
   // qDebug() << "SettingsForm::setFormValues()";

    if (settings == NULL) return;
    eic_smoothingAlgorithm->setCurrentIndex(settings->value("eic_smoothingAlgorithm").toInt());
    eic_smoothingWindow->setValue(settings->value("eic_smoothingWindow").toDouble());
    grouping_maxRtWindow->setValue(settings->value("grouping_maxRtWindow").toDouble());
    maxNaturalAbundanceErr->setValue(settings->value("maxNaturalAbundanceErr").toDouble());
    maxIsotopeScanDiff->setValue(settings->value("maxIsotopeScanDiff").toDouble());
    minIsotopicCorrelation->setValue(settings->value("minIsotopicCorrelation").toDouble());
    baseline_smoothing->setValue(settings->value("baseline_smoothing").toDouble());
    baseline_quantile->setValue(settings->value("baseline_quantile").toDouble());


    C13Labeled->setCheckState( (Qt::CheckState) settings->value("C13Labeled").toInt() );
    N15Labeled->setCheckState( (Qt::CheckState) settings->value("N15Labeled").toInt()  );
    S34Labeled->setCheckState( (Qt::CheckState) settings->value("S34Labeled").toInt() );
    D2Labeled->setCheckState(  (Qt::CheckState) settings->value("D2Labeled").toInt()  );
    isotopeC13Correction->setCheckState(  (Qt::CheckState) settings->value("isotopeC13Correction").toInt()  );

    centroid_scan_flag->setCheckState( (Qt::CheckState) settings->value("centroid_scan_flag").toInt());
    scan_filter_min_intensity->setValue( settings->value("scan_filter_min_intensity").toInt());
    scan_filter_min_quantile->setValue(  settings->value("scan_filter_min_quantile").toInt());



   QStringList folders;       folders << "scriptsFolder" << "methodsFolder" << "pathwaysFolder" << "Rprogram";
   QList<QLineEdit*> items;    items  << scriptsFolder << methodsFolder << pathwaysFolder << Rprogram;

   unsigned int itemCount=0;
    foreach(QString itemName, folders) {
        if(settings->contains(itemName)) items[itemCount]->setText( settings->value(itemName).toString());
        itemCount++;
    }

    if(settings->contains("remote_server_url"))
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


    settings->setValue("eic_smoothingAlgorithm",eic_smoothingAlgorithm->currentIndex());
    settings->setValue("eic_smoothingWindow",eic_smoothingWindow->value());
    settings->setValue("grouping_maxRtWindow",grouping_maxRtWindow->value());
    settings->setValue("maxNaturalAbundanceErr",maxNaturalAbundanceErr->value());
    settings->setValue("maxIsotopeScanDiff",maxIsotopeScanDiff->value());
    settings->setValue("minIsotopicCorrelation",minIsotopicCorrelation->value());
    settings->setValue("C13Labeled",C13Labeled->checkState() );
    settings->setValue("N15Labeled",N15Labeled->checkState() );
    settings->setValue("S34Labeled",S34Labeled->checkState() );
    settings->setValue("D2Labeled", D2Labeled->checkState()  );
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
        setFormValues();
    }
}

void SettingsForm::selectFile(QString key) {
    QString oFile = ".";
    if(settings->contains(key)) oFile =  settings->value(key).toString();
    QString newFile = QFileDialog::getOpenFileName(this,"Select file",".","*.exe");
    if (! newFile.isEmpty()) {
        settings->setValue(key,newFile);
        setFormValues();
    }
}


