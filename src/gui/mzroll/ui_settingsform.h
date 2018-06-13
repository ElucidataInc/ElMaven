/********************************************************************************
** Form generated from reading UI file 'settingsform.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSFORM_H
#define UI_SETTINGSFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsForm
{
public:
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *instrumentationOptions;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *instrumentSettings;
    QGridLayout *gridLayout_2;
    QLabel *label_3;
    QComboBox *ionizationMode;
    QLabel *label_7;
    QDoubleSpinBox *amuQ1;
    QComboBox *ionizationType;
    QLabel *label_13;
    QLabel *label_14;
    QDoubleSpinBox *amuQ3;
    QLabel *label_4;
    QComboBox *instrumentType;
    QLabel *label_35;
    QComboBox *filterlineComboBox;
    QWidget *FileImport;
    QGridLayout *gridLayout_8;
    QCheckBox *checkBoxMultiprocessing;
    QLabel *label_16;
    QCheckBox *centroid_scan_flag;
    QSpinBox *scan_filter_min_quantile;
    QSpinBox *scan_filter_min_intensity;
    QLabel *label_15;
    QComboBox *scan_filter_polarity;
    QLabel *label_9;
    QLabel *label_12;
    QComboBox *scan_filter_mslevel;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QWidget *peakDetectOptions;
    QHBoxLayout *horizontalLayout_7;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout1;
    QLabel *label_2;
    QSpinBox *eic_smoothingWindow;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_8;
    QDoubleSpinBox *grouping_maxRtWindow;
    QLabel *label_22;
    QComboBox *eic_smoothingAlgorithm;
    QGroupBox *groupBox_7;
    QGridLayout *gridLayout_6;
    QLabel *label_20;
    QLabel *label_21;
    QSpinBox *baseline_quantile;
    QSpinBox *baseline_smoothing;
    QWidget *peakFilteringTab;
    QGridLayout *gridLayout_7;
    QCheckBox *isIsotopeEqualPeakFilter;
    QGroupBox *peakFilteringGroupBox;
    QGridLayout *gridLayout_12;
    QLabel *label_34;
    QDoubleSpinBox *minSignalBaselineDifference;
    QLabel *label_37;
    QDoubleSpinBox *minPeakQuality;
    QGroupBox *isotopeFilteringGroupBox;
    QGridLayout *gridLayout_13;
    QLabel *label_36;
    QDoubleSpinBox *isotopicMinSignalBaselineDifference;
    QLabel *label_38;
    QDoubleSpinBox *minIsotopicPeakQuality;
    QWidget *isotopeDetectOptions;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QCheckBox *S34Labeled_Barplot;
    QCheckBox *D2Labeled_IsoWidget;
    QLabel *label_Barplot;
    QCheckBox *N15Labeled_BPE;
    QCheckBox *N15Labeled_Barplot;
    QCheckBox *D2Labeled_Barplot;
    QCheckBox *C13Labeled_Barplot;
    QLabel *label_IsoWidget;
    QCheckBox *S34Labeled_IsoWidget;
    QLabel *label_BPE;
    QCheckBox *C13Labeled_IsoWidget;
    QCheckBox *D2Labeled_BPE;
    QCheckBox *C13Labeled_BPE;
    QCheckBox *N15Labeled_IsoWidget;
    QCheckBox *S34Labeled_BPE;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_4;
    QLabel *label_26;
    QLabel *label_10;
    QLabel *label_11;
    QDoubleSpinBox *maxNaturalAbundanceErr;
    QDoubleSpinBox *minIsotopicCorrelation;
    QLabel *label;
    QCheckBox *isotopeC13Correction;
    QSpinBox *maxIsotopeScanDiff;
    QDoubleSpinBox *doubleSpinBoxAbThresh;
    QWidget *eicOptions;
    QHBoxLayout *horizontalLayout_5;
    QGroupBox *eicCalculation;
    QComboBox *eicTypeComboBox;
    QLabel *label_28;
    QWidget *peakGrouping;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_11;
    QTextBrowser *formulaWithOverlap;
    QTextBrowser *formulaWithoutOverlap;
    QWidget *gridLayoutWidget_2;
    QGridLayout *gridLayout_9;
    QSlider *distXSlider;
    QLabel *distXStatus;
    QLabel *label_29;
    QLabel *label_31;
    QLabel *distYStatus;
    QSlider *distYSlider;
    QLabel *label_30;
    QSlider *overlapSlider;
    QLabel *overlapStatus;
    QCheckBox *useOverlap;
    QWidget *tab_3;
    QGroupBox *groupBox_5;
    QTextBrowser *formulaWithRt;
    QTextBrowser *formulaWithoutRt;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout_14;
    QSlider *intensityWeight;
    QSlider *qualityWeight;
    QLabel *intensityWeightStatus;
    QLabel *label_32;
    QSlider *deltaRTWeight;
    QLabel *deltaRTWeightStatus;
    QLabel *qualityWeightStatus;
    QLabel *label_drtWeight;
    QLabel *label_33;
    QCheckBox *deltaRTCheck;
    QWidget *Libraries;
    QHBoxLayout *horizontalLayout_3;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_10;
    QLabel *label_17;
    QLineEdit *data_server_url;
    QLabel *label_18;
    QPushButton *methodsFolderSelect;
    QLineEdit *methodsFolder;
    QLabel *label_19;
    QPushButton *pathwaysFolderSelect;
    QLineEdit *pathwaysFolder;
    QLabel *label_23;
    QPushButton *scriptsFolderSelect;
    QLineEdit *scriptsFolder;
    QPushButton *fetchCompounds;
    QWidget *ExternalPrograms;
    QGridLayout *gridLayout_5;
    QLineEdit *Rprogram;
    QLineEdit *RawExtractProgram;
    QLabel *label_25;
    QLabel *label_24;
    QPushButton *rawExtractSelect;
    QPushButton *RProgramSelect;

    void setupUi(QWidget *SettingsForm)
    {
        if (SettingsForm->objectName().isEmpty())
            SettingsForm->setObjectName(QStringLiteral("SettingsForm"));
        SettingsForm->resize(1560, 318);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SettingsForm->sizePolicy().hasHeightForWidth());
        SettingsForm->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(SettingsForm);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        tabWidget = new QTabWidget(SettingsForm);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        instrumentationOptions = new QWidget();
        instrumentationOptions->setObjectName(QStringLiteral("instrumentationOptions"));
        horizontalLayout_2 = new QHBoxLayout(instrumentationOptions);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        instrumentSettings = new QGroupBox(instrumentationOptions);
        instrumentSettings->setObjectName(QStringLiteral("instrumentSettings"));
        gridLayout_2 = new QGridLayout(instrumentSettings);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_3 = new QLabel(instrumentSettings);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_2->addWidget(label_3, 0, 0, 1, 1);

        ionizationMode = new QComboBox(instrumentSettings);
        ionizationMode->setObjectName(QStringLiteral("ionizationMode"));

        gridLayout_2->addWidget(ionizationMode, 0, 1, 1, 1);

        label_7 = new QLabel(instrumentSettings);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout_2->addWidget(label_7, 0, 2, 1, 1);

        amuQ1 = new QDoubleSpinBox(instrumentSettings);
        amuQ1->setObjectName(QStringLiteral("amuQ1"));
        amuQ1->setValue(0.5);

        gridLayout_2->addWidget(amuQ1, 0, 3, 1, 1);

        ionizationType = new QComboBox(instrumentSettings);
        ionizationType->setObjectName(QStringLiteral("ionizationType"));

        gridLayout_2->addWidget(ionizationType, 1, 1, 1, 1);

        label_13 = new QLabel(instrumentSettings);
        label_13->setObjectName(QStringLiteral("label_13"));

        gridLayout_2->addWidget(label_13, 1, 0, 1, 1);

        label_14 = new QLabel(instrumentSettings);
        label_14->setObjectName(QStringLiteral("label_14"));

        gridLayout_2->addWidget(label_14, 1, 2, 1, 1);

        amuQ3 = new QDoubleSpinBox(instrumentSettings);
        amuQ3->setObjectName(QStringLiteral("amuQ3"));
        amuQ3->setMaximum(1e+09);
        amuQ3->setValue(0.5);

        gridLayout_2->addWidget(amuQ3, 1, 3, 1, 1);

        label_4 = new QLabel(instrumentSettings);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 5, 0, 1, 1);

        instrumentType = new QComboBox(instrumentSettings);
        instrumentType->setObjectName(QStringLiteral("instrumentType"));
        instrumentType->setEnabled(false);

        gridLayout_2->addWidget(instrumentType, 5, 1, 1, 1);

        label_35 = new QLabel(instrumentSettings);
        label_35->setObjectName(QStringLiteral("label_35"));

        gridLayout_2->addWidget(label_35, 5, 2, 1, 1);

        filterlineComboBox = new QComboBox(instrumentSettings);
        filterlineComboBox->setObjectName(QStringLiteral("filterlineComboBox"));

        gridLayout_2->addWidget(filterlineComboBox, 5, 3, 1, 1);


        horizontalLayout_2->addWidget(instrumentSettings);

        tabWidget->addTab(instrumentationOptions, QString());
        FileImport = new QWidget();
        FileImport->setObjectName(QStringLiteral("FileImport"));
        gridLayout_8 = new QGridLayout(FileImport);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        checkBoxMultiprocessing = new QCheckBox(FileImport);
        checkBoxMultiprocessing->setObjectName(QStringLiteral("checkBoxMultiprocessing"));
        checkBoxMultiprocessing->setEnabled(true);
        checkBoxMultiprocessing->setChecked(true);

        gridLayout_8->addWidget(checkBoxMultiprocessing, 1, 3, 1, 1);

        label_16 = new QLabel(FileImport);
        label_16->setObjectName(QStringLiteral("label_16"));

        gridLayout_8->addWidget(label_16, 3, 0, 1, 1);

        centroid_scan_flag = new QCheckBox(FileImport);
        centroid_scan_flag->setObjectName(QStringLiteral("centroid_scan_flag"));

        gridLayout_8->addWidget(centroid_scan_flag, 0, 0, 1, 1);

        scan_filter_min_quantile = new QSpinBox(FileImport);
        scan_filter_min_quantile->setObjectName(QStringLiteral("scan_filter_min_quantile"));

        gridLayout_8->addWidget(scan_filter_min_quantile, 3, 1, 1, 1);

        scan_filter_min_intensity = new QSpinBox(FileImport);
        scan_filter_min_intensity->setObjectName(QStringLiteral("scan_filter_min_intensity"));
        scan_filter_min_intensity->setMaximum(1000000000);

        gridLayout_8->addWidget(scan_filter_min_intensity, 4, 1, 1, 1);

        label_15 = new QLabel(FileImport);
        label_15->setObjectName(QStringLiteral("label_15"));

        gridLayout_8->addWidget(label_15, 4, 0, 1, 1);

        scan_filter_polarity = new QComboBox(FileImport);
        scan_filter_polarity->setObjectName(QStringLiteral("scan_filter_polarity"));

        gridLayout_8->addWidget(scan_filter_polarity, 1, 1, 1, 1);

        label_9 = new QLabel(FileImport);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout_8->addWidget(label_9, 1, 0, 1, 1);

        label_12 = new QLabel(FileImport);
        label_12->setObjectName(QStringLiteral("label_12"));

        gridLayout_8->addWidget(label_12, 2, 0, 1, 1);

        scan_filter_mslevel = new QComboBox(FileImport);
        scan_filter_mslevel->setObjectName(QStringLiteral("scan_filter_mslevel"));

        gridLayout_8->addWidget(scan_filter_mslevel, 2, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_8->addItem(horizontalSpacer_2, 1, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_8->addItem(horizontalSpacer, 3, 4, 1, 1);

        tabWidget->addTab(FileImport, QString());
        peakDetectOptions = new QWidget();
        peakDetectOptions->setObjectName(QStringLiteral("peakDetectOptions"));
        horizontalLayout_7 = new QHBoxLayout(peakDetectOptions);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        groupBox_3 = new QGroupBox(peakDetectOptions);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        gridLayout1 = new QGridLayout(groupBox_3);
        gridLayout1->setObjectName(QStringLiteral("gridLayout1"));
        label_2 = new QLabel(groupBox_3);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout1->addWidget(label_2, 1, 0, 1, 1);

        eic_smoothingWindow = new QSpinBox(groupBox_3);
        eic_smoothingWindow->setObjectName(QStringLiteral("eic_smoothingWindow"));
        eic_smoothingWindow->setMinimum(1);
        eic_smoothingWindow->setValue(10);

        gridLayout1->addWidget(eic_smoothingWindow, 1, 1, 1, 1);

        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout1->addWidget(label_5, 1, 2, 1, 1);

        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout1->addWidget(label_6, 3, 0, 1, 1);

        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout1->addWidget(label_8, 3, 2, 1, 1);

        grouping_maxRtWindow = new QDoubleSpinBox(groupBox_3);
        grouping_maxRtWindow->setObjectName(QStringLiteral("grouping_maxRtWindow"));
        grouping_maxRtWindow->setMaximum(1e+09);
        grouping_maxRtWindow->setSingleStep(0.1);
        grouping_maxRtWindow->setValue(0.25);

        gridLayout1->addWidget(grouping_maxRtWindow, 3, 1, 1, 1);

        label_22 = new QLabel(groupBox_3);
        label_22->setObjectName(QStringLiteral("label_22"));

        gridLayout1->addWidget(label_22, 0, 0, 1, 1);

        eic_smoothingAlgorithm = new QComboBox(groupBox_3);
        eic_smoothingAlgorithm->setObjectName(QStringLiteral("eic_smoothingAlgorithm"));

        gridLayout1->addWidget(eic_smoothingAlgorithm, 0, 1, 1, 1);


        horizontalLayout_7->addWidget(groupBox_3);

        groupBox_7 = new QGroupBox(peakDetectOptions);
        groupBox_7->setObjectName(QStringLiteral("groupBox_7"));
        gridLayout_6 = new QGridLayout(groupBox_7);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        label_20 = new QLabel(groupBox_7);
        label_20->setObjectName(QStringLiteral("label_20"));

        gridLayout_6->addWidget(label_20, 1, 0, 1, 1);

        label_21 = new QLabel(groupBox_7);
        label_21->setObjectName(QStringLiteral("label_21"));

        gridLayout_6->addWidget(label_21, 0, 0, 1, 1);

        baseline_quantile = new QSpinBox(groupBox_7);
        baseline_quantile->setObjectName(QStringLiteral("baseline_quantile"));
        baseline_quantile->setMaximum(100);
        baseline_quantile->setValue(80);

        gridLayout_6->addWidget(baseline_quantile, 0, 1, 1, 1);

        baseline_smoothing = new QSpinBox(groupBox_7);
        baseline_smoothing->setObjectName(QStringLiteral("baseline_smoothing"));
        baseline_smoothing->setMaximum(1000000);
        baseline_smoothing->setValue(5);

        gridLayout_6->addWidget(baseline_smoothing, 1, 1, 1, 1);


        horizontalLayout_7->addWidget(groupBox_7);

        tabWidget->addTab(peakDetectOptions, QString());
        peakFilteringTab = new QWidget();
        peakFilteringTab->setObjectName(QStringLiteral("peakFilteringTab"));
        gridLayout_7 = new QGridLayout(peakFilteringTab);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        isIsotopeEqualPeakFilter = new QCheckBox(peakFilteringTab);
        isIsotopeEqualPeakFilter->setObjectName(QStringLiteral("isIsotopeEqualPeakFilter"));
        isIsotopeEqualPeakFilter->setCursor(QCursor(Qt::PointingHandCursor));

        gridLayout_7->addWidget(isIsotopeEqualPeakFilter, 0, 0, 1, 1);

        peakFilteringGroupBox = new QGroupBox(peakFilteringTab);
        peakFilteringGroupBox->setObjectName(QStringLiteral("peakFilteringGroupBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(peakFilteringGroupBox->sizePolicy().hasHeightForWidth());
        peakFilteringGroupBox->setSizePolicy(sizePolicy1);
        gridLayout_12 = new QGridLayout(peakFilteringGroupBox);
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        label_34 = new QLabel(peakFilteringGroupBox);
        label_34->setObjectName(QStringLiteral("label_34"));

        gridLayout_12->addWidget(label_34, 0, 0, 1, 1);

        minSignalBaselineDifference = new QDoubleSpinBox(peakFilteringGroupBox);
        minSignalBaselineDifference->setObjectName(QStringLiteral("minSignalBaselineDifference"));
        minSignalBaselineDifference->setMaximum(1e+20);

        gridLayout_12->addWidget(minSignalBaselineDifference, 0, 1, 1, 1);

        label_37 = new QLabel(peakFilteringGroupBox);
        label_37->setObjectName(QStringLiteral("label_37"));

        gridLayout_12->addWidget(label_37, 1, 0, 1, 1);

        minPeakQuality = new QDoubleSpinBox(peakFilteringGroupBox);
        minPeakQuality->setObjectName(QStringLiteral("minPeakQuality"));
        minPeakQuality->setMaximum(1);
        minPeakQuality->setSingleStep(0.1);

        gridLayout_12->addWidget(minPeakQuality, 1, 1, 1, 1);


        gridLayout_7->addWidget(peakFilteringGroupBox, 1, 0, 1, 1);

        isotopeFilteringGroupBox = new QGroupBox(peakFilteringTab);
        isotopeFilteringGroupBox->setObjectName(QStringLiteral("isotopeFilteringGroupBox"));
        sizePolicy1.setHeightForWidth(isotopeFilteringGroupBox->sizePolicy().hasHeightForWidth());
        isotopeFilteringGroupBox->setSizePolicy(sizePolicy1);
        gridLayout_13 = new QGridLayout(isotopeFilteringGroupBox);
        gridLayout_13->setObjectName(QStringLiteral("gridLayout_13"));
        label_36 = new QLabel(isotopeFilteringGroupBox);
        label_36->setObjectName(QStringLiteral("label_36"));

        gridLayout_13->addWidget(label_36, 0, 0, 1, 1);

        isotopicMinSignalBaselineDifference = new QDoubleSpinBox(isotopeFilteringGroupBox);
        isotopicMinSignalBaselineDifference->setObjectName(QStringLiteral("isotopicMinSignalBaselineDifference"));
        QFont font;
        font.setFamily(QStringLiteral("Ubuntu"));
        font.setPointSize(11);
        isotopicMinSignalBaselineDifference->setFont(font);
        isotopicMinSignalBaselineDifference->setMaximum(1e+20);

        gridLayout_13->addWidget(isotopicMinSignalBaselineDifference, 0, 1, 1, 1);

        label_38 = new QLabel(isotopeFilteringGroupBox);
        label_38->setObjectName(QStringLiteral("label_38"));

        gridLayout_13->addWidget(label_38, 1, 0, 1, 1);

        minIsotopicPeakQuality = new QDoubleSpinBox(isotopeFilteringGroupBox);
        minIsotopicPeakQuality->setObjectName(QStringLiteral("minIsotopicPeakQuality"));
        minIsotopicPeakQuality->setMaximum(1);
        minIsotopicPeakQuality->setSingleStep(0.1);

        gridLayout_13->addWidget(minIsotopicPeakQuality, 1, 1, 1, 1);


        gridLayout_7->addWidget(isotopeFilteringGroupBox, 1, 1, 1, 1);

        tabWidget->addTab(peakFilteringTab, QString());
        isotopeDetectOptions = new QWidget();
        isotopeDetectOptions->setObjectName(QStringLiteral("isotopeDetectOptions"));
        horizontalLayout = new QHBoxLayout(isotopeDetectOptions);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        groupBox = new QGroupBox(isotopeDetectOptions);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setFlat(false);
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        S34Labeled_Barplot = new QCheckBox(groupBox);
        S34Labeled_Barplot->setObjectName(QStringLiteral("S34Labeled_Barplot"));

        gridLayout_3->addWidget(S34Labeled_Barplot, 2, 4, 1, 1);

        D2Labeled_IsoWidget = new QCheckBox(groupBox);
        D2Labeled_IsoWidget->setObjectName(QStringLiteral("D2Labeled_IsoWidget"));

        gridLayout_3->addWidget(D2Labeled_IsoWidget, 3, 1, 1, 1);

        label_Barplot = new QLabel(groupBox);
        label_Barplot->setObjectName(QStringLiteral("label_Barplot"));

        gridLayout_3->addWidget(label_Barplot, 2, 0, 1, 1);

        N15Labeled_BPE = new QCheckBox(groupBox);
        N15Labeled_BPE->setObjectName(QStringLiteral("N15Labeled_BPE"));

        gridLayout_3->addWidget(N15Labeled_BPE, 0, 3, 1, 1);

        N15Labeled_Barplot = new QCheckBox(groupBox);
        N15Labeled_Barplot->setObjectName(QStringLiteral("N15Labeled_Barplot"));

        gridLayout_3->addWidget(N15Labeled_Barplot, 2, 3, 1, 1);

        D2Labeled_Barplot = new QCheckBox(groupBox);
        D2Labeled_Barplot->setObjectName(QStringLiteral("D2Labeled_Barplot"));

        gridLayout_3->addWidget(D2Labeled_Barplot, 2, 1, 1, 1);

        C13Labeled_Barplot = new QCheckBox(groupBox);
        C13Labeled_Barplot->setObjectName(QStringLiteral("C13Labeled_Barplot"));

        gridLayout_3->addWidget(C13Labeled_Barplot, 2, 2, 1, 1);

        label_IsoWidget = new QLabel(groupBox);
        label_IsoWidget->setObjectName(QStringLiteral("label_IsoWidget"));

        gridLayout_3->addWidget(label_IsoWidget, 3, 0, 1, 1);

        S34Labeled_IsoWidget = new QCheckBox(groupBox);
        S34Labeled_IsoWidget->setObjectName(QStringLiteral("S34Labeled_IsoWidget"));

        gridLayout_3->addWidget(S34Labeled_IsoWidget, 3, 4, 1, 1);

        label_BPE = new QLabel(groupBox);
        label_BPE->setObjectName(QStringLiteral("label_BPE"));

        gridLayout_3->addWidget(label_BPE, 0, 0, 1, 1);

        C13Labeled_IsoWidget = new QCheckBox(groupBox);
        C13Labeled_IsoWidget->setObjectName(QStringLiteral("C13Labeled_IsoWidget"));

        gridLayout_3->addWidget(C13Labeled_IsoWidget, 3, 2, 1, 1);

        D2Labeled_BPE = new QCheckBox(groupBox);
        D2Labeled_BPE->setObjectName(QStringLiteral("D2Labeled_BPE"));
        D2Labeled_BPE->setEnabled(true);

        gridLayout_3->addWidget(D2Labeled_BPE, 0, 1, 1, 1);

        C13Labeled_BPE = new QCheckBox(groupBox);
        C13Labeled_BPE->setObjectName(QStringLiteral("C13Labeled_BPE"));

        gridLayout_3->addWidget(C13Labeled_BPE, 0, 2, 1, 1);

        N15Labeled_IsoWidget = new QCheckBox(groupBox);
        N15Labeled_IsoWidget->setObjectName(QStringLiteral("N15Labeled_IsoWidget"));

        gridLayout_3->addWidget(N15Labeled_IsoWidget, 3, 3, 1, 1);

        S34Labeled_BPE = new QCheckBox(groupBox);
        S34Labeled_BPE->setObjectName(QStringLiteral("S34Labeled_BPE"));

        gridLayout_3->addWidget(S34Labeled_BPE, 0, 4, 1, 1);


        horizontalLayout->addWidget(groupBox);

        groupBox_4 = new QGroupBox(isotopeDetectOptions);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout_4 = new QGridLayout(groupBox_4);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        label_26 = new QLabel(groupBox_4);
        label_26->setObjectName(QStringLiteral("label_26"));

        gridLayout_4->addWidget(label_26, 2, 0, 1, 1);

        label_10 = new QLabel(groupBox_4);
        label_10->setObjectName(QStringLiteral("label_10"));

        gridLayout_4->addWidget(label_10, 3, 0, 1, 1);

        label_11 = new QLabel(groupBox_4);
        label_11->setObjectName(QStringLiteral("label_11"));

        gridLayout_4->addWidget(label_11, 1, 0, 1, 1);

        maxNaturalAbundanceErr = new QDoubleSpinBox(groupBox_4);
        maxNaturalAbundanceErr->setObjectName(QStringLiteral("maxNaturalAbundanceErr"));
        maxNaturalAbundanceErr->setDecimals(2);
        maxNaturalAbundanceErr->setMinimum(0);
        maxNaturalAbundanceErr->setMaximum(1e+09);
        maxNaturalAbundanceErr->setValue(100);

        gridLayout_4->addWidget(maxNaturalAbundanceErr, 3, 1, 1, 1);

        minIsotopicCorrelation = new QDoubleSpinBox(groupBox_4);
        minIsotopicCorrelation->setObjectName(QStringLiteral("minIsotopicCorrelation"));
        minIsotopicCorrelation->setProperty("showGroupSeparator", QVariant(false));
        minIsotopicCorrelation->setMinimum(-1);
        minIsotopicCorrelation->setMaximum(1);
        minIsotopicCorrelation->setSingleStep(0.1);
        minIsotopicCorrelation->setValue(0.2);

        gridLayout_4->addWidget(minIsotopicCorrelation, 0, 1, 1, 1);

        label = new QLabel(groupBox_4);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_4->addWidget(label, 0, 0, 1, 1);

        isotopeC13Correction = new QCheckBox(groupBox_4);
        isotopeC13Correction->setObjectName(QStringLiteral("isotopeC13Correction"));

        gridLayout_4->addWidget(isotopeC13Correction, 4, 0, 1, 2);

        maxIsotopeScanDiff = new QSpinBox(groupBox_4);
        maxIsotopeScanDiff->setObjectName(QStringLiteral("maxIsotopeScanDiff"));
        maxIsotopeScanDiff->setMinimum(0);
        maxIsotopeScanDiff->setMaximum(999999999);
        maxIsotopeScanDiff->setValue(5);

        gridLayout_4->addWidget(maxIsotopeScanDiff, 1, 1, 1, 1);

        doubleSpinBoxAbThresh = new QDoubleSpinBox(groupBox_4);
        doubleSpinBoxAbThresh->setObjectName(QStringLiteral("doubleSpinBoxAbThresh"));
        doubleSpinBoxAbThresh->setDecimals(1);
        doubleSpinBoxAbThresh->setMaximum(100);
        doubleSpinBoxAbThresh->setSingleStep(0.5);
        doubleSpinBoxAbThresh->setValue(1);

        gridLayout_4->addWidget(doubleSpinBoxAbThresh, 2, 1, 1, 1);


        horizontalLayout->addWidget(groupBox_4);

        tabWidget->addTab(isotopeDetectOptions, QString());
        eicOptions = new QWidget();
        eicOptions->setObjectName(QStringLiteral("eicOptions"));
        horizontalLayout_5 = new QHBoxLayout(eicOptions);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        eicCalculation = new QGroupBox(eicOptions);
        eicCalculation->setObjectName(QStringLiteral("eicCalculation"));
        eicTypeComboBox = new QComboBox(eicCalculation);
        eicTypeComboBox->setObjectName(QStringLiteral("eicTypeComboBox"));
        eicTypeComboBox->setGeometry(QRect(160, 50, 151, 20));
        label_28 = new QLabel(eicCalculation);
        label_28->setObjectName(QStringLiteral("label_28"));
        label_28->setGeometry(QRect(10, 50, 91, 16));

        horizontalLayout_5->addWidget(eicCalculation);

        tabWidget->addTab(eicOptions, QString());
        peakGrouping = new QWidget();
        peakGrouping->setObjectName(QStringLiteral("peakGrouping"));
        groupBox_2 = new QGroupBox(peakGrouping);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(9, 9, 591, 251));
        gridLayout_11 = new QGridLayout(groupBox_2);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        formulaWithOverlap = new QTextBrowser(groupBox_2);
        formulaWithOverlap->setObjectName(QStringLiteral("formulaWithOverlap"));

        gridLayout_11->addWidget(formulaWithOverlap, 1, 3, 1, 2);

        formulaWithoutOverlap = new QTextBrowser(groupBox_2);
        formulaWithoutOverlap->setObjectName(QStringLiteral("formulaWithoutOverlap"));

        gridLayout_11->addWidget(formulaWithoutOverlap, 2, 3, 1, 2);

        gridLayoutWidget_2 = new QWidget(peakGrouping);
        gridLayoutWidget_2->setObjectName(QStringLiteral("gridLayoutWidget_2"));
        gridLayoutWidget_2->setGeometry(QRect(700, 56, 531, 191));
        gridLayout_9 = new QGridLayout(gridLayoutWidget_2);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        gridLayout_9->setContentsMargins(0, 0, 0, 0);
        distXSlider = new QSlider(gridLayoutWidget_2);
        distXSlider->setObjectName(QStringLiteral("distXSlider"));
        distXSlider->setMaximum(100);
        distXSlider->setSingleStep(1);
        distXSlider->setValue(10);
        distXSlider->setOrientation(Qt::Horizontal);

        gridLayout_9->addWidget(distXSlider, 0, 1, 1, 1);

        distXStatus = new QLabel(gridLayoutWidget_2);
        distXStatus->setObjectName(QStringLiteral("distXStatus"));

        gridLayout_9->addWidget(distXStatus, 0, 2, 1, 1);

        label_29 = new QLabel(gridLayoutWidget_2);
        label_29->setObjectName(QStringLiteral("label_29"));

        gridLayout_9->addWidget(label_29, 0, 0, 1, 1);

        label_31 = new QLabel(gridLayoutWidget_2);
        label_31->setObjectName(QStringLiteral("label_31"));

        gridLayout_9->addWidget(label_31, 1, 0, 1, 1);

        distYStatus = new QLabel(gridLayoutWidget_2);
        distYStatus->setObjectName(QStringLiteral("distYStatus"));

        gridLayout_9->addWidget(distYStatus, 1, 2, 1, 1);

        distYSlider = new QSlider(gridLayoutWidget_2);
        distYSlider->setObjectName(QStringLiteral("distYSlider"));
        distYSlider->setMaximum(100);
        distYSlider->setValue(10);
        distYSlider->setOrientation(Qt::Horizontal);

        gridLayout_9->addWidget(distYSlider, 1, 1, 1, 1);

        label_30 = new QLabel(gridLayoutWidget_2);
        label_30->setObjectName(QStringLiteral("label_30"));

        gridLayout_9->addWidget(label_30, 2, 0, 1, 1);

        overlapSlider = new QSlider(gridLayoutWidget_2);
        overlapSlider->setObjectName(QStringLiteral("overlapSlider"));
        overlapSlider->setMaximum(100);
        overlapSlider->setValue(10);
        overlapSlider->setOrientation(Qt::Horizontal);

        gridLayout_9->addWidget(overlapSlider, 2, 1, 1, 1);

        overlapStatus = new QLabel(gridLayoutWidget_2);
        overlapStatus->setObjectName(QStringLiteral("overlapStatus"));

        gridLayout_9->addWidget(overlapStatus, 2, 2, 1, 1);

        useOverlap = new QCheckBox(peakGrouping);
        useOverlap->setObjectName(QStringLiteral("useOverlap"));
        useOverlap->setGeometry(QRect(690, 20, 145, 22));
        useOverlap->setChecked(true);
        useOverlap->setTristate(false);
        tabWidget->addTab(peakGrouping, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        groupBox_5 = new QGroupBox(tab_3);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        groupBox_5->setGeometry(QRect(10, 10, 681, 241));
        formulaWithRt = new QTextBrowser(groupBox_5);
        formulaWithRt->setObjectName(QStringLiteral("formulaWithRt"));
        formulaWithRt->setEnabled(true);
        formulaWithRt->setGeometry(QRect(5, 40, 661, 201));
        formulaWithoutRt = new QTextBrowser(groupBox_5);
        formulaWithoutRt->setObjectName(QStringLiteral("formulaWithoutRt"));
        formulaWithoutRt->setGeometry(QRect(5, 40, 661, 201));
        gridLayoutWidget = new QWidget(tab_3);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(700, 50, 541, 201));
        gridLayout_14 = new QGridLayout(gridLayoutWidget);
        gridLayout_14->setObjectName(QStringLiteral("gridLayout_14"));
        gridLayout_14->setContentsMargins(0, 0, 0, 0);
        intensityWeight = new QSlider(gridLayoutWidget);
        intensityWeight->setObjectName(QStringLiteral("intensityWeight"));
        intensityWeight->setMaximum(50);
        intensityWeight->setValue(10);
        intensityWeight->setSliderPosition(10);
        intensityWeight->setOrientation(Qt::Horizontal);

        gridLayout_14->addWidget(intensityWeight, 1, 1, 1, 1);

        qualityWeight = new QSlider(gridLayoutWidget);
        qualityWeight->setObjectName(QStringLiteral("qualityWeight"));
        qualityWeight->setMaximum(50);
        qualityWeight->setValue(10);
        qualityWeight->setSliderPosition(10);
        qualityWeight->setOrientation(Qt::Horizontal);

        gridLayout_14->addWidget(qualityWeight, 0, 1, 1, 1);

        intensityWeightStatus = new QLabel(gridLayoutWidget);
        intensityWeightStatus->setObjectName(QStringLiteral("intensityWeightStatus"));

        gridLayout_14->addWidget(intensityWeightStatus, 1, 2, 1, 1);

        label_32 = new QLabel(gridLayoutWidget);
        label_32->setObjectName(QStringLiteral("label_32"));

        gridLayout_14->addWidget(label_32, 0, 0, 1, 1);

        deltaRTWeight = new QSlider(gridLayoutWidget);
        deltaRTWeight->setObjectName(QStringLiteral("deltaRTWeight"));
        deltaRTWeight->setEnabled(false);
        deltaRTWeight->setMaximum(50);
        deltaRTWeight->setValue(10);
        deltaRTWeight->setOrientation(Qt::Horizontal);

        gridLayout_14->addWidget(deltaRTWeight, 2, 1, 1, 1);

        deltaRTWeightStatus = new QLabel(gridLayoutWidget);
        deltaRTWeightStatus->setObjectName(QStringLiteral("deltaRTWeightStatus"));

        gridLayout_14->addWidget(deltaRTWeightStatus, 2, 2, 1, 1);

        qualityWeightStatus = new QLabel(gridLayoutWidget);
        qualityWeightStatus->setObjectName(QStringLiteral("qualityWeightStatus"));

        gridLayout_14->addWidget(qualityWeightStatus, 0, 2, 1, 1);

        label_drtWeight = new QLabel(gridLayoutWidget);
        label_drtWeight->setObjectName(QStringLiteral("label_drtWeight"));

        gridLayout_14->addWidget(label_drtWeight, 2, 0, 1, 1);

        label_33 = new QLabel(gridLayoutWidget);
        label_33->setObjectName(QStringLiteral("label_33"));

        gridLayout_14->addWidget(label_33, 1, 0, 1, 1);

        deltaRTCheck = new QCheckBox(tab_3);
        deltaRTCheck->setObjectName(QStringLiteral("deltaRTCheck"));
        deltaRTCheck->setEnabled(true);
        deltaRTCheck->setGeometry(QRect(700, 20, 199, 22));
        tabWidget->addTab(tab_3, QString());
        Libraries = new QWidget();
        Libraries->setObjectName(QStringLiteral("Libraries"));
        horizontalLayout_3 = new QHBoxLayout(Libraries);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        groupBox_6 = new QGroupBox(Libraries);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        gridLayout_10 = new QGridLayout(groupBox_6);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        label_17 = new QLabel(groupBox_6);
        label_17->setObjectName(QStringLiteral("label_17"));

        gridLayout_10->addWidget(label_17, 0, 0, 1, 1);

        data_server_url = new QLineEdit(groupBox_6);
        data_server_url->setObjectName(QStringLiteral("data_server_url"));

        gridLayout_10->addWidget(data_server_url, 0, 2, 1, 1);

        label_18 = new QLabel(groupBox_6);
        label_18->setObjectName(QStringLiteral("label_18"));

        gridLayout_10->addWidget(label_18, 1, 0, 1, 1);

        methodsFolderSelect = new QPushButton(groupBox_6);
        methodsFolderSelect->setObjectName(QStringLiteral("methodsFolderSelect"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        methodsFolderSelect->setIcon(icon);

        gridLayout_10->addWidget(methodsFolderSelect, 1, 1, 1, 1);

        methodsFolder = new QLineEdit(groupBox_6);
        methodsFolder->setObjectName(QStringLiteral("methodsFolder"));

        gridLayout_10->addWidget(methodsFolder, 1, 2, 1, 1);

        label_19 = new QLabel(groupBox_6);
        label_19->setObjectName(QStringLiteral("label_19"));

        gridLayout_10->addWidget(label_19, 2, 0, 1, 1);

        pathwaysFolderSelect = new QPushButton(groupBox_6);
        pathwaysFolderSelect->setObjectName(QStringLiteral("pathwaysFolderSelect"));
        pathwaysFolderSelect->setIcon(icon);

        gridLayout_10->addWidget(pathwaysFolderSelect, 2, 1, 1, 1);

        pathwaysFolder = new QLineEdit(groupBox_6);
        pathwaysFolder->setObjectName(QStringLiteral("pathwaysFolder"));

        gridLayout_10->addWidget(pathwaysFolder, 2, 2, 1, 1);

        label_23 = new QLabel(groupBox_6);
        label_23->setObjectName(QStringLiteral("label_23"));

        gridLayout_10->addWidget(label_23, 3, 0, 1, 1);

        scriptsFolderSelect = new QPushButton(groupBox_6);
        scriptsFolderSelect->setObjectName(QStringLiteral("scriptsFolderSelect"));
        scriptsFolderSelect->setIcon(icon);

        gridLayout_10->addWidget(scriptsFolderSelect, 3, 1, 1, 1);

        scriptsFolder = new QLineEdit(groupBox_6);
        scriptsFolder->setObjectName(QStringLiteral("scriptsFolder"));

        gridLayout_10->addWidget(scriptsFolder, 3, 2, 1, 1);

        fetchCompounds = new QPushButton(groupBox_6);
        fetchCompounds->setObjectName(QStringLiteral("fetchCompounds"));

        gridLayout_10->addWidget(fetchCompounds, 0, 3, 1, 1);


        horizontalLayout_3->addWidget(groupBox_6);

        tabWidget->addTab(Libraries, QString());
        ExternalPrograms = new QWidget();
        ExternalPrograms->setObjectName(QStringLiteral("ExternalPrograms"));
        gridLayout_5 = new QGridLayout(ExternalPrograms);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        Rprogram = new QLineEdit(ExternalPrograms);
        Rprogram->setObjectName(QStringLiteral("Rprogram"));

        gridLayout_5->addWidget(Rprogram, 1, 5, 1, 1);

        RawExtractProgram = new QLineEdit(ExternalPrograms);
        RawExtractProgram->setObjectName(QStringLiteral("RawExtractProgram"));

        gridLayout_5->addWidget(RawExtractProgram, 0, 5, 1, 1);

        label_25 = new QLabel(ExternalPrograms);
        label_25->setObjectName(QStringLiteral("label_25"));

        gridLayout_5->addWidget(label_25, 0, 1, 1, 1);

        label_24 = new QLabel(ExternalPrograms);
        label_24->setObjectName(QStringLiteral("label_24"));

        gridLayout_5->addWidget(label_24, 1, 1, 1, 1);

        rawExtractSelect = new QPushButton(ExternalPrograms);
        rawExtractSelect->setObjectName(QStringLiteral("rawExtractSelect"));
        rawExtractSelect->setIcon(icon);

        gridLayout_5->addWidget(rawExtractSelect, 0, 4, 1, 1);

        RProgramSelect = new QPushButton(ExternalPrograms);
        RProgramSelect->setObjectName(QStringLiteral("RProgramSelect"));
        RProgramSelect->setIcon(icon);

        gridLayout_5->addWidget(RProgramSelect, 1, 4, 1, 1);

        tabWidget->addTab(ExternalPrograms, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);

        QWidget::setTabOrder(ionizationMode, amuQ1);
        QWidget::setTabOrder(amuQ1, ionizationType);
        QWidget::setTabOrder(ionizationType, amuQ3);
        QWidget::setTabOrder(amuQ3, instrumentType);
        QWidget::setTabOrder(instrumentType, filterlineComboBox);
        QWidget::setTabOrder(filterlineComboBox, centroid_scan_flag);
        QWidget::setTabOrder(centroid_scan_flag, scan_filter_polarity);
        QWidget::setTabOrder(scan_filter_polarity, scan_filter_mslevel);
        QWidget::setTabOrder(scan_filter_mslevel, scan_filter_min_quantile);
        QWidget::setTabOrder(scan_filter_min_quantile, scan_filter_min_intensity);
        QWidget::setTabOrder(scan_filter_min_intensity, checkBoxMultiprocessing);
        QWidget::setTabOrder(checkBoxMultiprocessing, eic_smoothingAlgorithm);
        QWidget::setTabOrder(eic_smoothingAlgorithm, eic_smoothingWindow);
        QWidget::setTabOrder(eic_smoothingWindow, grouping_maxRtWindow);
        QWidget::setTabOrder(grouping_maxRtWindow, baseline_quantile);
        QWidget::setTabOrder(baseline_quantile, baseline_smoothing);
        QWidget::setTabOrder(baseline_smoothing, isIsotopeEqualPeakFilter);
        QWidget::setTabOrder(isIsotopeEqualPeakFilter, minSignalBaselineDifference);
        QWidget::setTabOrder(minSignalBaselineDifference, minPeakQuality);
        QWidget::setTabOrder(minPeakQuality, isotopicMinSignalBaselineDifference);
        QWidget::setTabOrder(isotopicMinSignalBaselineDifference, minIsotopicPeakQuality);
        QWidget::setTabOrder(minIsotopicPeakQuality, D2Labeled_BPE);
        QWidget::setTabOrder(D2Labeled_BPE, C13Labeled_BPE);
        QWidget::setTabOrder(C13Labeled_BPE, N15Labeled_BPE);
        QWidget::setTabOrder(N15Labeled_BPE, S34Labeled_BPE);
        QWidget::setTabOrder(S34Labeled_BPE, D2Labeled_Barplot);
        QWidget::setTabOrder(D2Labeled_Barplot, C13Labeled_Barplot);
        QWidget::setTabOrder(C13Labeled_Barplot, N15Labeled_Barplot);
        QWidget::setTabOrder(N15Labeled_Barplot, S34Labeled_Barplot);
        QWidget::setTabOrder(S34Labeled_Barplot, D2Labeled_IsoWidget);
        QWidget::setTabOrder(D2Labeled_IsoWidget, C13Labeled_IsoWidget);
        QWidget::setTabOrder(C13Labeled_IsoWidget, N15Labeled_IsoWidget);
        QWidget::setTabOrder(N15Labeled_IsoWidget, S34Labeled_IsoWidget);
        QWidget::setTabOrder(S34Labeled_IsoWidget, minIsotopicCorrelation);
        QWidget::setTabOrder(minIsotopicCorrelation, maxIsotopeScanDiff);
        QWidget::setTabOrder(maxIsotopeScanDiff, doubleSpinBoxAbThresh);
        QWidget::setTabOrder(doubleSpinBoxAbThresh, maxNaturalAbundanceErr);
        QWidget::setTabOrder(maxNaturalAbundanceErr, isotopeC13Correction);
        QWidget::setTabOrder(isotopeC13Correction, eicTypeComboBox);
        QWidget::setTabOrder(eicTypeComboBox, useOverlap);
        QWidget::setTabOrder(useOverlap, distXSlider);
        QWidget::setTabOrder(distXSlider, distYSlider);
        QWidget::setTabOrder(distYSlider, overlapSlider);
        QWidget::setTabOrder(overlapSlider, deltaRTCheck);
        QWidget::setTabOrder(deltaRTCheck, qualityWeight);
        QWidget::setTabOrder(qualityWeight, intensityWeight);
        QWidget::setTabOrder(intensityWeight, deltaRTWeight);
        QWidget::setTabOrder(deltaRTWeight, data_server_url);
        QWidget::setTabOrder(data_server_url, methodsFolderSelect);
        QWidget::setTabOrder(methodsFolderSelect, methodsFolder);
        QWidget::setTabOrder(methodsFolder, pathwaysFolderSelect);
        QWidget::setTabOrder(pathwaysFolderSelect, pathwaysFolder);
        QWidget::setTabOrder(pathwaysFolder, scriptsFolderSelect);
        QWidget::setTabOrder(scriptsFolderSelect, scriptsFolder);
        QWidget::setTabOrder(scriptsFolder, rawExtractSelect);
        QWidget::setTabOrder(rawExtractSelect, RawExtractProgram);
        QWidget::setTabOrder(RawExtractProgram, RProgramSelect);
        QWidget::setTabOrder(RProgramSelect, Rprogram);
        QWidget::setTabOrder(Rprogram, tabWidget);
        QWidget::setTabOrder(tabWidget, formulaWithOverlap);
        QWidget::setTabOrder(formulaWithOverlap, fetchCompounds);
        QWidget::setTabOrder(fetchCompounds, formulaWithoutRt);
        QWidget::setTabOrder(formulaWithoutRt, formulaWithRt);
        QWidget::setTabOrder(formulaWithRt, formulaWithoutOverlap);

        retranslateUi(SettingsForm);
        QObject::connect(isIsotopeEqualPeakFilter, SIGNAL(clicked(bool)), isotopeFilteringGroupBox, SLOT(setDisabled(bool)));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SettingsForm);
    } // setupUi

    void retranslateUi(QWidget *SettingsForm)
    {
        SettingsForm->setWindowTitle(QApplication::translate("SettingsForm", "Options", 0));
        instrumentSettings->setTitle(QApplication::translate("SettingsForm", "Instrumentation Type", 0));
        label_3->setText(QApplication::translate("SettingsForm", "Polarity / Ionization Mode:", 0));
        ionizationMode->clear();
        ionizationMode->insertItems(0, QStringList()
         << QApplication::translate("SettingsForm", "Auto Detect", 0)
         << QApplication::translate("SettingsForm", "Neutral (0)", 0)
         << QApplication::translate("SettingsForm", "Positive (+1)", 0)
         << QApplication::translate("SettingsForm", "Negative (-1)", 0)
        );
        label_7->setText(QApplication::translate("SettingsForm", "Q1 Accuracy", 0));
        amuQ1->setPrefix(QString());
        amuQ1->setSuffix(QApplication::translate("SettingsForm", " amu", 0));
        ionizationType->clear();
        ionizationType->insertItems(0, QStringList()
         << QApplication::translate("SettingsForm", "ESI", 0)
         << QApplication::translate("SettingsForm", "EI", 0)
         << QApplication::translate("SettingsForm", "CI", 0)
         << QApplication::translate("SettingsForm", "APCI", 0)
         << QApplication::translate("SettingsForm", "FI/FD", 0)
         << QApplication::translate("SettingsForm", "FAB", 0)
         << QApplication::translate("SettingsForm", "MALDI", 0)
        );
        label_13->setText(QApplication::translate("SettingsForm", "Ionization Type", 0));
        label_14->setText(QApplication::translate("SettingsForm", "Q3 Accuracy", 0));
        amuQ3->setSuffix(QApplication::translate("SettingsForm", " amu", 0));
        label_4->setText(QApplication::translate("SettingsForm", "Instrument Type:", 0));
        instrumentType->clear();
        instrumentType->insertItems(0, QStringList()
         << QApplication::translate("SettingsForm", "Full Spectrum", 0)
         << QApplication::translate("SettingsForm", "Triple Quad", 0)
        );
        label_35->setText(QApplication::translate("SettingsForm", "Filterline", 0));
        filterlineComboBox->clear();
        filterlineComboBox->insertItems(0, QStringList()
         << QApplication::translate("SettingsForm", "All", 0)
        );
        tabWidget->setTabText(tabWidget->indexOf(instrumentationOptions), QApplication::translate("SettingsForm", "Instrumentation", 0));
        checkBoxMultiprocessing->setText(QApplication::translate("SettingsForm", "Enable multiprocessing", 0));
        label_16->setText(QApplication::translate("SettingsForm", "Scan Filter Intensity Minimum Quantile Cutoff ", 0));
        centroid_scan_flag->setText(QApplication::translate("SettingsForm", "Centroid Scans", 0));
        scan_filter_min_quantile->setSuffix(QApplication::translate("SettingsForm", " %", 0));
        scan_filter_min_intensity->setSuffix(QApplication::translate("SettingsForm", " ions", 0));
        scan_filter_min_intensity->setPrefix(QString());
        label_15->setText(QApplication::translate("SettingsForm", "Scan Filter:  Minimum Intensity ", 0));
        scan_filter_polarity->clear();
        scan_filter_polarity->insertItems(0, QStringList()
         << QApplication::translate("SettingsForm", "Not Filtered", 0)
         << QApplication::translate("SettingsForm", "Positive Polarity Scans Only", 0)
         << QApplication::translate("SettingsForm", "Negative Polarity Scans Only", 0)
        );
        label_9->setText(QApplication::translate("SettingsForm", "Scan Filter Polarity", 0));
        label_12->setText(QApplication::translate("SettingsForm", "Scan Filter MS Level", 0));
        scan_filter_mslevel->clear();
        scan_filter_mslevel->insertItems(0, QStringList()
         << QApplication::translate("SettingsForm", "Not Filtered", 0)
         << QApplication::translate("SettingsForm", "MS1 Scans Only", 0)
         << QApplication::translate("SettingsForm", "MS2 Scans Only", 0)
        );
        tabWidget->setTabText(tabWidget->indexOf(FileImport), QApplication::translate("SettingsForm", "File Import", 0));
        groupBox_3->setTitle(QApplication::translate("SettingsForm", "Peak Detection and Grouping Settings", 0));
        label_2->setText(QApplication::translate("SettingsForm", "EIC Smoothing Window ", 0));
        label_5->setText(QApplication::translate("SettingsForm", "scans", 0));
        label_6->setText(QApplication::translate("SettingsForm", "Max Retention Time difference between Peaks", 0));
        label_8->setText(QApplication::translate("SettingsForm", "min", 0));
        label_22->setText(QApplication::translate("SettingsForm", "EIC Smoothing Algorithm", 0));
        eic_smoothingAlgorithm->clear();
        eic_smoothingAlgorithm->insertItems(0, QStringList()
         << QApplication::translate("SettingsForm", "Savitzky-Golay", 0)
         << QApplication::translate("SettingsForm", "Gaussian", 0)
         << QApplication::translate("SettingsForm", "Moving Average", 0)
        );
        groupBox_7->setTitle(QApplication::translate("SettingsForm", "Baseline Calculation", 0));
        label_20->setText(QApplication::translate("SettingsForm", "Baseline Smoothing ", 0));
        label_21->setText(QApplication::translate("SettingsForm", "Drop top x%  intensities from chromatogram", 0));
        baseline_quantile->setSuffix(QApplication::translate("SettingsForm", "%", 0));
        baseline_smoothing->setSuffix(QApplication::translate("SettingsForm", " scans", 0));
        tabWidget->setTabText(tabWidget->indexOf(peakDetectOptions), QApplication::translate("SettingsForm", "Peak Detection", 0));
        isIsotopeEqualPeakFilter->setText(QApplication::translate("SettingsForm", "Isotope Peak Filtering is same as Peak Filtering", 0));
        peakFilteringGroupBox->setTitle(QApplication::translate("SettingsForm", "Parent Peak Filtering", 0));
        label_34->setText(QApplication::translate("SettingsForm", "Min. Signal Baseline Difference", 0));
        label_37->setText(QApplication::translate("SettingsForm", "Min. Peak Quality", 0));
        isotopeFilteringGroupBox->setTitle(QApplication::translate("SettingsForm", "Isotopic Peak Filtering", 0));
        label_36->setText(QApplication::translate("SettingsForm", "Min. Signal Baseline Difference", 0));
        label_38->setText(QApplication::translate("SettingsForm", "Min. Peak Quality", 0));
        tabWidget->setTabText(tabWidget->indexOf(peakFilteringTab), QApplication::translate("SettingsForm", "Peak Filtering", 0));
        groupBox->setTitle(QApplication::translate("SettingsForm", "Samples Are Labeled? ", 0));
        S34Labeled_Barplot->setText(QApplication::translate("SettingsForm", "S34", 0));
        D2Labeled_IsoWidget->setText(QApplication::translate("SettingsForm", "D2", 0));
        label_Barplot->setText(QApplication::translate("SettingsForm", "Isotopic barplot ", 0));
        N15Labeled_BPE->setText(QApplication::translate("SettingsForm", "N15", 0));
        N15Labeled_Barplot->setText(QApplication::translate("SettingsForm", "N15", 0));
        D2Labeled_Barplot->setText(QApplication::translate("SettingsForm", "D2", 0));
        C13Labeled_Barplot->setText(QApplication::translate("SettingsForm", "C13", 0));
        label_IsoWidget->setText(QApplication::translate("SettingsForm", "Isotopic widget", 0));
        S34Labeled_IsoWidget->setText(QApplication::translate("SettingsForm", "S34", 0));
        label_BPE->setText(QApplication::translate("SettingsForm", "Bookmark, peakdetection & file export", 0));
        C13Labeled_IsoWidget->setText(QApplication::translate("SettingsForm", "C13", 0));
        D2Labeled_BPE->setText(QApplication::translate("SettingsForm", "D2", 0));
        C13Labeled_BPE->setText(QApplication::translate("SettingsForm", "C13", 0));
        N15Labeled_IsoWidget->setText(QApplication::translate("SettingsForm", "N15", 0));
        S34Labeled_BPE->setText(QApplication::translate("SettingsForm", "S34", 0));
        groupBox_4->setTitle(QApplication::translate("SettingsForm", "Filter Isotopic Peaks based on these criteria", 0));
        label_26->setText(QApplication::translate("SettingsForm", "Abundance Threshold", 0));
        label_10->setText(QApplication::translate("SettingsForm", "Maximum %Error to Natural Abundance", 0));
        label_11->setText(QApplication::translate("SettingsForm", "Isotope is within [X] scans of parent", 0));
        maxNaturalAbundanceErr->setPrefix(QString());
        maxNaturalAbundanceErr->setSuffix(QApplication::translate("SettingsForm", " %", 0));
        label->setText(QApplication::translate("SettingsForm", "Minimum Isotope-parent correlation", 0));
        isotopeC13Correction->setText(QApplication::translate("SettingsForm", "Correct for Natural C13 Isotope Abundance", 0));
        maxIsotopeScanDiff->setSuffix(QApplication::translate("SettingsForm", " scans", 0));
        tabWidget->setTabText(tabWidget->indexOf(isotopeDetectOptions), QApplication::translate("SettingsForm", "Isotope Detection", 0));
        eicCalculation->setTitle(QApplication::translate("SettingsForm", "EIC Calculation", 0));
        eicTypeComboBox->clear();
        eicTypeComboBox->insertItems(0, QStringList()
         << QApplication::translate("SettingsForm", "Max", 0)
         << QApplication::translate("SettingsForm", "Sum", 0)
        );
        label_28->setText(QApplication::translate("SettingsForm", "EIC Type", 0));
        tabWidget->setTabText(tabWidget->indexOf(eicOptions), QApplication::translate("SettingsForm", "EIC (XIC) [BETA]", 0));
        groupBox_2->setTitle(QApplication::translate("SettingsForm", "Peak Grouping Score", 0));
        formulaWithOverlap->setHtml(QApplication::translate("SettingsForm", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Droid Sans Mono,Courier New,monospace,Droid Sans Fallback'; font-size:11pt; color:#000000;\">score = 1.0/((distX*A)+0.01)/((distY*B)+0.01)*(C*overlap)</span><span style=\" font-family:'Ubuntu'; font-size:11pt;\"><br /></span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">where</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0"
                        "px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">	distX = Rt difference of peaks being compared</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">	A = Weight of distX</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">	distY = Difference between peak Intensities</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">	B = Weight of distY</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">	overlap = Overlap of"
                        " peaks (between 0 to 1)</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">	C = Overlap weight</span></p></body></html>", 0));
        formulaWithoutOverlap->setHtml(QApplication::translate("SettingsForm", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Droid Sans Mono,Courier New,monospace,Droid Sans Fallback'; font-size:11pt; color:#000000;\">score = 1.0/((distX*A)+0.01)/((distY*B)+0.01)</span><span style=\" font-family:'Ubuntu'; font-size:11pt;\"><br /></span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">where</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-"
                        "indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">	distX = Rt difference of peaks being compared</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">	A = Weight of distX</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">	distY = Difference between peak Intensities</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">	B = Weight of distY</span></p></body></html>", 0));
        distXStatus->setText(QApplication::translate("SettingsForm", "Status", 0));
        label_29->setText(QApplication::translate("SettingsForm", "distX Weight", 0));
        label_31->setText(QApplication::translate("SettingsForm", "distY Weight", 0));
        distYStatus->setText(QApplication::translate("SettingsForm", "Status", 0));
        label_30->setText(QApplication::translate("SettingsForm", "Overlap Weight", 0));
        overlapStatus->setText(QApplication::translate("SettingsForm", "Status", 0));
#ifndef QT_NO_TOOLTIP
        useOverlap->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        useOverlap->setText(QApplication::translate("SettingsForm", "Consider overlap", 0));
        tabWidget->setTabText(tabWidget->indexOf(peakGrouping), QApplication::translate("SettingsForm", "Peak Grouping", 0));
        groupBox_5->setTitle(QApplication::translate("SettingsForm", "Group Rank Formula", 0));
        formulaWithRt->setHtml(QApplication::translate("SettingsForm", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Ubuntu'; font-size:11pt;\"><br /></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">Group Rank = ((1.1 - Q)^A) * (1 /( log(I + 1))^B) * (dRT)^(2*C)</span></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Ubuntu'; font-size:"
                        "11pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">Where   </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">                Q =  Quality of a group </span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Ubuntu'; font-size:11pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">                A =  Quality Weightage</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0p"
                        "x; font-family:'Ubuntu'; font-size:11pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">                I  =  Intensity of a group   </span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Ubuntu'; font-size:11pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">               B =  Intensity Weightage</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Ubuntu'; font-size:11pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0;"
                        " text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">               dRT = Difference between compound retention time and group mean retention time</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Ubuntu'; font-size:11pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">               C =  dRT Weightage</span></p></body></html>", 0));
        formulaWithoutRt->setHtml(QApplication::translate("SettingsForm", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Ubuntu'; font-size:11pt;\"><br /></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">Group Rank = ((1.1 - Q)^A) * (1 /( log(I + 1))^B) </span></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Ubuntu'; font-size:11pt;\"><br /></p>\n"
"<p styl"
                        "e=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">Where   </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">                Q =  Quality of a group </span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Ubuntu'; font-size:11pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">                A =  Quality Weightage</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Ubuntu'; font-"
                        "size:11pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">                I  =  Intensity of a group   </span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Ubuntu'; font-size:11pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">               B =  Intensity Weightage</span></p></body></html>", 0));
        intensityWeightStatus->setText(QApplication::translate("SettingsForm", "Status", 0));
        label_32->setText(QApplication::translate("SettingsForm", "Quality Weight", 0));
        deltaRTWeightStatus->setText(QApplication::translate("SettingsForm", "Status", 0));
        qualityWeightStatus->setText(QApplication::translate("SettingsForm", "Status", 0));
        label_drtWeight->setText(QApplication::translate("SettingsForm", "dRT Weight", 0));
        label_33->setText(QApplication::translate("SettingsForm", "Intensity Weight", 0));
        deltaRTCheck->setText(QApplication::translate("SettingsForm", "Consider Retention Time", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("SettingsForm", "Group Rank", 0));
        groupBox_6->setTitle(QApplication::translate("SettingsForm", "Location of Remote and Local Libraries", 0));
        label_17->setText(QApplication::translate("SettingsForm", "Remote Data Server", 0));
        data_server_url->setText(QApplication::translate("SettingsForm", "http://genomics-pubs.princeton.edu/mzroll/", 0));
        label_18->setText(QApplication::translate("SettingsForm", "Methods Folder", 0));
        label_19->setText(QApplication::translate("SettingsForm", "Pathways Folder", 0));
        label_23->setText(QApplication::translate("SettingsForm", "Scripts Folder", 0));
        fetchCompounds->setText(QApplication::translate("SettingsForm", "Fetch Compounds", 0));
        tabWidget->setTabText(tabWidget->indexOf(Libraries), QApplication::translate("SettingsForm", "Libraries", 0));
        Rprogram->setInputMask(QString());
        Rprogram->setPlaceholderText(QApplication::translate("SettingsForm", "Rscript.exe", 0));
        RawExtractProgram->setText(QApplication::translate("SettingsForm", "msconvert.exe", 0));
        RawExtractProgram->setPlaceholderText(QApplication::translate("SettingsForm", "rawExtractor.exe", 0));
        label_25->setText(QApplication::translate("SettingsForm", "ProteoWizzard", 0));
        label_24->setText(QApplication::translate("SettingsForm", "R Executable ", 0));
        tabWidget->setTabText(tabWidget->indexOf(ExternalPrograms), QApplication::translate("SettingsForm", "External Programs", 0));
    } // retranslateUi

};

namespace Ui {
    class SettingsForm: public Ui_SettingsForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSFORM_H
