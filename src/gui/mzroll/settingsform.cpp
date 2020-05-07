#include "common/analytics.h"
#include "eiclogic.h"
#include "eicwidget.h"
#include "isotopeswidget.h"
#include "ligandwidget.h"
#include "mainwindow.h"
#include "mavenparameters.h"
#include "mzMassCalculator.h"
#include "mzSample.h"
#include "pathwaywidget.h"
#include "settingsform.h"

OptionsDialogSettings::OptionsDialogSettings(SettingsForm* dialog): sf(dialog)
{
    settings.insert("ionizationMode", QVariant::fromValue(sf->ionizationMode));
    settings.insert("ionizationType", QVariant::fromValue(sf->ionizationType));
    settings.insert("instrumentType", QVariant::fromValue(sf->instrumentType));
    settings.insert("q1Accuracy", QVariant::fromValue(sf->amuQ1));
    settings.insert("q3Accuracy", QVariant::fromValue(sf->amuQ3));
    settings.insert("filterline", QVariant::fromValue(sf->filterlineComboBox));

    settings.insert("centroidScans", QVariant::fromValue(sf->centroid_scan_flag));
    settings.insert("scanFilterPolarity", QVariant::fromValue(sf->scan_filter_polarity));
    settings.insert("scanFilterMsLevel", QVariant::fromValue(sf->scan_filter_mslevel));
    settings.insert("scanFilterMinQuantile", QVariant::fromValue(sf->scan_filter_min_quantile));
    settings.insert("scanFilterMinIntensity", QVariant::fromValue(sf->scan_filter_min_intensity));
    settings.insert("uploadMultiprocessing", QVariant::fromValue(sf->checkBoxMultiprocessing));

    settings.insert("eicSmoothingAlgorithm", QVariant::fromValue(sf->eic_smoothingAlgorithm));
    settings.insert("eicSmoothingWindow", QVariant::fromValue(sf->eic_smoothingWindow));
    settings.insert("maxRtDiffBetweenPeaks", QVariant::fromValue(sf->grouping_maxRtWindow));

    settings.insert("aslsBaselineMode", QVariant::fromValue(sf->baselineModeTab));
    settings.insert("baselineQuantile", QVariant::fromValue(sf->baseline_quantile));
    settings.insert("baselineSmoothing", QVariant::fromValue(sf->baseline_smoothing));
    settings.insert("aslsSmoothness", QVariant::fromValue(sf->smoothnessSlider));
    settings.insert("aslsAsymmetry", QVariant::fromValue(sf->asymmetrySlider));

    settings.insert("isotopeFilterEqualPeak", QVariant::fromValue(sf->isIsotopeEqualPeakFilter));
    settings.insert("minSignalBaselineDifference", QVariant::fromValue(sf->minSignalBaselineDifference));
    settings.insert("isotopeMinSignalBaselineDifference", QVariant::fromValue(sf->isotopicMinSignalBaselineDifference));
    settings.insert("minPeakQuality", QVariant::fromValue(sf->minPeakQuality));
    settings.insert("isotopeMinPeakQuality", QVariant::fromValue(sf->minIsotopicPeakQuality));

    settings.insert("eicType", QVariant::fromValue(sf->eicTypeComboBox));

    settings.insert("useOverlap", QVariant::fromValue(sf->useOverlap));
    settings.insert("distXWeight", QVariant::fromValue(sf->distXSlider));
    settings.insert("distYWeight", QVariant::fromValue(sf->distYSlider));
    settings.insert("overlapWeight", QVariant::fromValue(sf->overlapSlider));

    settings.insert("considerDeltaRT", QVariant::fromValue(sf->deltaRTCheck));
    settings.insert("qualityWeight", QVariant::fromValue(sf->qualityWeight));
    settings.insert("intensityWeight", QVariant::fromValue(sf->intensityWeight));
    settings.insert("deltaRTWeight", QVariant::fromValue(sf->deltaRTWeight));
}

void OptionsDialogSettings::updateOptionsDialog(string key, string value)
{
    QString k(QString(key.c_str()));
    if (settings.find(k) != settings.end()
        && !value.empty()) {
        const QVariant& v = settings[k];

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

        if (QString(v.typeName()).contains("QTabWidget"))
            v.value<QTabWidget*>()->setCurrentIndex(std::stoi(value));

        emit sf->settingsUpdated(k, v);
    }
}

SettingsForm::SettingsForm(QSettings* s, MainWindow *w): QDialog(w) { 
    setupUi(this);
    settings = s;
    mainwindow = w;
    deltaRtCheckFlag = false;
    updateSettingFormGUI();
    setMavenParameters();

    optionSettings = new OptionsDialogSettings(this);

    resetButton->setDefault(false);
    resetButton->setAutoDefault(false);

    connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(getFormValues()));
    connect(resetButton, &QPushButton::clicked, this, &SettingsForm::onReset);

    connect(eic_smoothingWindow, SIGNAL(valueChanged(int)), SLOT(recomputeEIC()));
    connect(eic_smoothingAlgorithm, SIGNAL(currentIndexChanged(int)), SLOT(recomputeEIC()));
    connect(grouping_maxRtWindow, SIGNAL(valueChanged(double)), SLOT(recomputeEIC()));

    connect(baselineModeTab,
            SIGNAL(currentChanged(int)),
            SLOT(recomputeEIC()));
    connect(baseline_smoothing,
            SIGNAL(valueChanged(int)),
            SLOT(recomputeEIC()));
    connect(baseline_quantile,
            SIGNAL(valueChanged(int)),
            SLOT(recomputeEIC()));
    connect(smoothnessSlider,
            SIGNAL(valueChanged(int)),
            SLOT(_smoothnessChanged(int)));
    connect(asymmetrySlider,
            SIGNAL(valueChanged(int)),
            SLOT(_asymmetryChanged(int)));

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

    // instrumentation settings
    connect(ionizationMode,
            SIGNAL(currentIndexChanged(int)),
            SLOT(getFormValues()));
    connect(ionizationMode,
            SIGNAL(currentIndexChanged(QString)),
            mainwindow,
            SLOT(setIonizationModeLabel()));
    connect(ionizationType,
            SIGNAL(currentIndexChanged(int)),
            SLOT(getFormValues()));
    connect(ionizationType,
            SIGNAL(currentIndexChanged(int)),
            SLOT(setAppropriatePolarity()));
    connect(amuQ1,
            SIGNAL(valueChanged(double)),
            SLOT(getFormValues()));
    connect(amuQ3,
            SIGNAL(valueChanged(double)),
            SLOT(getFormValues()));
    connect(filterlineComboBox,
            SIGNAL(activated(QString)),
            SLOT(getFormValues()));

    // file import settings
    connect(centroid_scan_flag, SIGNAL(toggled(bool)), SLOT(getFormValues()));
    connect(scan_filter_polarity,
            SIGNAL(currentIndexChanged(int)),
            SLOT(getFormValues()));
    connect(scan_filter_mslevel,
            SIGNAL(currentIndexChanged(int)),
            SLOT(getFormValues()));
    connect(scan_filter_min_quantile,
            SIGNAL(valueChanged(int)),
            SLOT(getFormValues()));
    connect(scan_filter_min_intensity,
            SIGNAL(valueChanged(int)),
            SLOT(getFormValues()));
    connect(checkBoxMultiprocessing,
            SIGNAL(toggled(bool)),
            SLOT(updateMultiprocessing()));

    _connectAnalytics();

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

    connect(this,&SettingsForm::settingsChanged, optionSettings, &OptionsDialogSettings::updateOptionsDialog);
    connect(this, &QDialog::rejected, this, &SettingsForm::triggerSettingsUpdate);
}

void SettingsForm::setFilterline(string filterline)
{
    QString filterlineString = QString::fromStdString(filterline);
    filterlineComboBox->setCurrentText(filterlineString);
}

void SettingsForm::onReset()
{
    _disconnectAnalytics();
    emit resetSettings(optionSettings->getSettings().keys());
    QCoreApplication::processEvents();
    _connectAnalytics();
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
    emit updateSettings(optionSettings);
     QDialog::closeEvent(event);
}

void SettingsForm::triggerSettingsUpdate()
{
  emit updateSettings(optionSettings);
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

void SettingsForm::updateMultiprocessing() { 
    settings->setValue("uploadMultiprocessing", checkBoxMultiprocessing->checkState());
}

void SettingsForm::recomputeEIC() {
     
    getFormValues();

    PeakGroup* previousGroup = mainwindow->getEicWidget()->getParameters()->displayedGroup();
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

    centroid_scan_flag->setCheckState( (Qt::CheckState) settings->value("centroidScans").toInt());
    scan_filter_min_intensity->setValue( settings->value("scanFilterMinIntensity").toInt());
    scan_filter_min_quantile->setValue(  settings->value("scanFilterMinQuantile").toInt());

    QList<QLineEdit*> items;    items  << scriptsFolder << methodsFolder << pathwaysFolder << Rprogram << RawExtractProgram;
    QStringList pathlist;        pathlist << "scriptsFolder" << "methodsFolder" << "pathwaysFolder" << "Rprogram" << "RawExtractProgram";

   unsigned int itemCount=0;
    Q_FOREACH(QString itemName, pathlist) {
        if(settings->contains(itemName)) items[itemCount]->setText( settings->value(itemName).toString());
        itemCount++;
    }

    if(settings->contains("data_server_url"))
        data_server_url->setText( settings->value("data_server_url").toString());

    if(settings->contains("centroidScans"))
        centroid_scan_flag->setCheckState( (Qt::CheckState) settings->value("centroidScans").toInt());

    if(settings->contains("scanFilterMinIntensity"))
        scan_filter_min_intensity->setValue( settings->value("scanFilterMinIntensity").toInt());

    if(settings->contains("scanFilterMinQuantile"))
        scan_filter_min_quantile->setValue( settings->value("scanFilterMinQuantile").toInt());
}


void SettingsForm::getFormValues()
{
    if (settings == NULL) return;
    //qDebug() << "SettingsForm::getFormValues() ";

    settings->setValue("filterline", filterlineComboBox->currentText());

    settings->setValue("centroidScans", centroid_scan_flag->checkState() );
    settings->setValue("scanFilterMinIntensity", scan_filter_min_intensity->value());
    settings->setValue("scanFilterMinQuantile", scan_filter_min_quantile->value());

    settings->setValue("data_server_url", data_server_url->text());

    settings->setValue("centroidScanFlag", centroid_scan_flag->checkState());
    settings->setValue("scanFilterMinIntensity", scan_filter_min_intensity->value());
    settings->setValue("scanFilterMinQuantile", scan_filter_min_quantile->value());

    // change baseline estimation parameters
    settings->setValue("aslsBaselineMode", baselineModeTab->currentIndex());
    settings->setValue("baselineQuantile", baseline_quantile->value());
    settings->setValue("baselineSmoothing", baseline_smoothing->value());
    settings->setValue("aslsSmoothness", smoothnessSlider->value());
    settings->setValue("aslsAsymmetry", asymmetrySlider->value());

    // change ionization type
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

void SettingsForm::setAppropriatePolarity() {
    if (ionizationType->currentText() == "EI") {
        ionizationMode->setCurrentIndex(2);
        ionizationMode->setDisabled(true);
    } else {
        ionizationMode->setEnabled(true);
        ionizationMode->setCurrentIndex(0);
    }
}

void SettingsForm::show() {
    if (mainwindow == NULL) return;

    mainwindow->getAnalytics()->hitScreenView("OptionsDialog");

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

        if (settings->value("filterline") == "All") {
            mavenParameters->filterline = "";
        } else {
            mavenParameters->filterline = settings->value("filterline").toString().toStdString();
        }

        mavenParameters->aslsBaselineMode =
                settings->value("aslsBaselineMode").toBool();
        mavenParameters->baseline_dropTopX =
                settings->value("baselineQuantile").toInt();
        mavenParameters->baseline_smoothingWindow =
                settings->value("baselineSmoothing").toInt();
        mavenParameters->aslsSmoothness =
                settings->value("aslsSmoothness").toInt();
        mavenParameters->aslsAsymmetry =
                settings->value("aslsAsymmetry").toInt();
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

void SettingsForm::_smoothnessChanged(int value)
{
    smoothnessValue->setText(QString::number(value));
    recomputeEIC();
}

void SettingsForm::_asymmetryChanged(int value)
{
    double actualAsym = static_cast<double>(value) / 100.0;
    asymmetryValue->setText(QString::number(actualAsym, 'f', 2));
    recomputeEIC();
}

void SettingsForm::_connectAnalytics()
{
    // inescapable ugly types
    auto comboBoxIndexChanged =
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
    auto spinBoxValueChanged =
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    auto doubleSpinBoxValueChanged =
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);

    // analytics for instrumentation settings
    _analyticsConnections.append(connect(ionizationMode,
                                         comboBoxIndexChanged,
                                         [this](const int val)
    {
        QString mode = ionizationMode->currentText();
        mainwindow->getAnalytics()->hitEvent("Instrumentation Settings Changed",
                                             "Ionization Mode",
                                             mode);
    }));
    _analyticsConnections.append(connect(ionizationType,
                                         comboBoxIndexChanged,
                                         [this](const int val)
    {
        QString type = ionizationType->currentText();
        mainwindow->getAnalytics()->hitEvent("Instrumentation Settings Changed",
                                             "Ionization Type",
                                             type);
    }));
    _analyticsConnections.append(connect(amuQ1,
                                         doubleSpinBoxValueChanged,
                                         [this]()
    {
        mainwindow->getAnalytics()->hitEvent("Instrumentation Settings Changed",
                                             "Q1 Accuracy");
    }));
    _analyticsConnections.append(connect(amuQ3,
                                         doubleSpinBoxValueChanged,
                                         [this]()
    {
        mainwindow->getAnalytics()->hitEvent("Instrumentation Settings Changed",
                                             "Q3 Accuracy");
    }));
    _analyticsConnections.append(connect(filterlineComboBox,
                                         comboBoxIndexChanged,
                                         [this]()
    {
        mainwindow->getAnalytics()->hitEvent("Instrumentation Settings Changed",
                                             "Filterline");
    }));

    // analytics for file import settings
    _analyticsConnections.append(connect(centroid_scan_flag,
                                         &QCheckBox::toggled,
                                         [this](const bool val)
    {
        QString state = val? "On" : "Off";
        mainwindow->getAnalytics()->hitEvent("File Import Settings Changed",
                                             "Centroid Scans",
                                             state);
    }));
    _analyticsConnections.append(connect(scan_filter_polarity,
                                         comboBoxIndexChanged,
                                         [this](const int val)
    {
        QString polarity = scan_filter_polarity->currentText();
        mainwindow->getAnalytics()->hitEvent("File Import Settings Changed",
                                             "Scan Filter Polarity",
                                             polarity);
    }));
    _analyticsConnections.append(connect(scan_filter_mslevel,
                                         comboBoxIndexChanged,
                                         [this](const int val)
    {
        QString mslevel = scan_filter_mslevel->currentText();
        mainwindow->getAnalytics()->hitEvent("File Import Settings Changed",
                                             "Scan Filter MSLevel",
                                             mslevel);
    }));
    _analyticsConnections.append(connect(scan_filter_min_quantile,
                                         spinBoxValueChanged,
                                         [this]()
    {
        mainwindow->getAnalytics()->hitEvent("File Import Settings Changed",
                                             "Scan Filter Minimum Quantile");
    }));
    _analyticsConnections.append(connect(scan_filter_min_intensity,
                                         spinBoxValueChanged,
                                         [this]()
    {
        mainwindow->getAnalytics()->hitEvent("File Import Settings Changed",
                                             "Scan Filter Minimum Intensity");
    }));
    _analyticsConnections.append(connect(checkBoxMultiprocessing,
                                         &QCheckBox::toggled,
                                         [this](const bool val)
    {
        QString state = val? "On" : "Off";
        mainwindow->getAnalytics()->hitEvent("File Import Settings Changed",
                                             "Upload Multiprocessing",
                                             state);
    }));
}

void SettingsForm::_disconnectAnalytics()
{
    for (const auto& connection : _analyticsConnections)
        QObject::disconnect(connection);

    _analyticsConnections.clear();
}
