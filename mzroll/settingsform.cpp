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
    connect(minRtCorrelation, SIGNAL(valueChanged(double)), SLOT(getFormValues()));
    connect(minSampleCorrelation, SIGNAL(valueChanged(double)), SLOT(getFormValues()));
    connect(maxRtDiff, SIGNAL(valueChanged(double)), SLOT(getFormValues()));
    connect(isotopeC13Correction, SIGNAL(toggled(bool)), SLOT(getFormValues()));
    connect(amuQ1, SIGNAL(valueChanged(double)), SLOT(getFormValues()));
    connect(amuQ3, SIGNAL(valueChanged(double)), SLOT(getFormValues()));

    //isotope detection setting
    connect(C13Labeled,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(N15Labeled,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(S34Labeled,SIGNAL(toggled(bool)),SLOT(recomputeIsotopes()));
    connect(D2Labeled,SIGNAL (toggled(bool)),SLOT(recomputeIsotopes()));
    connect(isotopeC13Correction, SIGNAL(toggled(bool)), SLOT(recomputeIsotopes()));

    connect(maxNaturalAbundanceErr, SIGNAL(valueChanged(double)), SLOT(recomputeIsotopes()));
    connect(minIsotopicCorrelation, SIGNAL(valueChanged(double)), SLOT(recomputeIsotopes()));
    connect(maxIsotopeScanDiff, SIGNAL(valueChanged(int)), SLOT(recomputeIsotopes()));

    connect(notes_server_url, SIGNAL(textChanged(QString)), SLOT(getFormValues()));
    connect(search_server_url, SIGNAL(textChanged(QString)), SLOT(getFormValues()));
    connect(pathway_server_url, SIGNAL(textChanged(QString)), SLOT(getFormValues()));

    connect(centroid_scan_flag,SIGNAL(toggled(bool)), SLOT(getFormValues()));
    connect(scan_filter_min_quantile, SIGNAL(valueChanged(int)), SLOT(getFormValues()));
    connect(scan_filter_min_intensity, SIGNAL(valueChanged(int)), SLOT(getFormValues()));

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
    qDebug() << "SettingsForm::setFormValues()";

    if (settings == NULL) return;
    eic_smoothingAlgorithm->setCurrentIndex(settings->value("eic_smoothingAlgorithm").toInt());
    eic_smoothingWindow->setValue(settings->value("eic_smoothingWindow").toDouble());
    grouping_maxRtWindow->setValue(settings->value("grouping_maxRtWindow").toDouble());
    maxNaturalAbundanceErr->setValue(settings->value("maxNaturalAbundanceErr").toDouble());
    maxIsotopeScanDiff->setValue(settings->value("maxIsotopeScanDiff").toDouble());
    minIsotopicCorrelation->setValue(settings->value("minIsotopicCorrelation").toDouble());
    minRtCorrelation->setValue(settings->value("minRtCorrelation").toDouble());
    minSampleCorrelation->setValue(settings->value("minSampleCorrelation").toDouble());
    maxRtDiff->setValue(settings->value("maxRtDiff").toDouble());
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

    if(settings->contains("notes_server_url"))
        notes_server_url->setText( settings->value("notes_server_url").toString());

    if(settings->contains("pathway_server_url"))
        pathway_server_url->setText( settings->value("pathway_server_url").toString());

    if(settings->contains("search_server_url"))
        search_server_url->setText( settings->value("search_server_url").toString());


    if(settings->contains("centroid_scan_flag"))
        centroid_scan_flag->setCheckState( (Qt::CheckState) settings->value("centroid_scan_flag").toInt());

    if(settings->contains("scan_filter_min_intensity"))
        scan_filter_min_intensity->setValue( settings->value("scan_filter_min_intensity").toInt());

    if(settings->contains("scan_filter_min_quantile"))
        scan_filter_min_quantile->setValue( settings->value("scan_filter_min_quantile").toInt());
}


void SettingsForm::getFormValues() {
    if (settings == NULL) return;
    qDebug() << "SettingsForm::getFormValues() ";


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
    settings->setValue("minRtCorrelation", minRtCorrelation->value());
    settings->setValue("minSampleCorrelation", minSampleCorrelation->value());
    settings->setValue("maxRtDiff", maxRtDiff->value());
    settings->setValue("amuQ1", amuQ1->value());
    settings->setValue("amuQ3", amuQ3->value());
    settings->setValue("baseline_quantile", baseline_quantile->value());
    settings->setValue("baseline_smoothing", baseline_smoothing->value());

    settings->setValue("centroid_scan_flag", centroid_scan_flag->checkState() );
    settings->setValue("scan_filter_min_intensity", scan_filter_min_intensity->value());
    settings->setValue("scan_filter_min_quantile", scan_filter_min_quantile->value());


    settings->setValue("notes_server_url", notes_server_url->text());
    settings->setValue("pathway_server_url", pathway_server_url->text());
    settings->setValue("search_server_url", search_server_url->text());

    settings->setValue("centroid_scan_flag", centroid_scan_flag->checkState());
    settings->setValue("scan_filter_min_intensity", scan_filter_min_intensity->value());
    settings->setValue("scan_filter_min_quantile", scan_filter_min_quantile->value());



    int mode=0;
    if( ionizationMode->currentText().contains("+1") ) {
        mode=1;
    } else if ( ionizationMode->currentText().contains("-1") ) {
        mode=-1;
    } else {
        mode=0;
    }

    if(mainwindow) mainwindow->setIonizationMode(mode);

    settings->setValue("ionizationMode",mode);

    mzSample::setFilter_centroidScans( centroid_scan_flag->checkState() == Qt::Checked );
    mzSample::setFilter_minIntensity( scan_filter_min_intensity->value() );
    mzSample::setFilter_intensityQuantile( scan_filter_min_quantile->value());

}
