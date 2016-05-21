/********************************************************************************
** Form generated from reading UI file 'settingsform.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSFORM_H
#define UI_SETTINGSFORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsForm
{
public:
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *instrumentationOptions;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QLabel *label_7;
    QLabel *label_14;
    QDoubleSpinBox *amuQ3;
    QDoubleSpinBox *amuQ1;
    QComboBox *instrumentType;
    QLabel *label_4;
    QComboBox *ionizationMode;
    QLabel *label_3;
    QWidget *tab;
    QGridLayout *gridLayout_8;
    QCheckBox *centroid_scan_flag;
    QLabel *label_15;
    QSpinBox *scan_filter_min_intensity;
    QLabel *label_16;
    QSpacerItem *horizontalSpacer;
    QSpinBox *scan_filter_min_quantile;
    QWidget *peakDetectOptions;
    QGridLayout *gridLayout_7;
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
    QPushButton *recomputeEICButton;
    QGroupBox *groupBox_7;
    QGridLayout *gridLayout_6;
    QLabel *label_20;
    QSpinBox *baseline_smoothing;
    QSpinBox *baseline_quantile;
    QLabel *label_21;
    QWidget *isotopeDetectOptions;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QCheckBox *C13Labeled;
    QCheckBox *N15Labeled;
    QCheckBox *S34Labeled;
    QCheckBox *D2Labeled;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_4;
    QLabel *label_10;
    QDoubleSpinBox *maxNaturalAbundanceErr;
    QLabel *label_11;
    QLabel *label;
    QDoubleSpinBox *minIsotopicCorrelation;
    QSpinBox *maxIsotopeScanDiff;
    QCheckBox *isotopeC13Correction;
    QWidget *clusteringOptions;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_5;
    QLabel *label_9;
    QLabel *label_12;
    QDoubleSpinBox *minSampleCorrelation;
    QLabel *label_13;
    QDoubleSpinBox *minRtCorrelation;
    QDoubleSpinBox *maxRtDiff;
    QSpacerItem *horizontalSpacer_2;
    QWidget *tab_2;
    QHBoxLayout *horizontalLayout_3;
    QGroupBox *groupBox_6;
    QFormLayout *formLayout_2;
    QLineEdit *notes_server_url;
    QLineEdit *search_server_url;
    QLineEdit *pathway_server_url;
    QLabel *label_19;
    QLabel *label_18;
    QLabel *label_17;

    void setupUi(QWidget *SettingsForm)
    {
        if (SettingsForm->objectName().isEmpty())
            SettingsForm->setObjectName(QString::fromUtf8("SettingsForm"));
        SettingsForm->resize(940, 218);
        gridLayout = new QGridLayout(SettingsForm);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tabWidget = new QTabWidget(SettingsForm);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        instrumentationOptions = new QWidget();
        instrumentationOptions->setObjectName(QString::fromUtf8("instrumentationOptions"));
        horizontalLayout_2 = new QHBoxLayout(instrumentationOptions);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        groupBox_2 = new QGroupBox(instrumentationOptions);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_2->addWidget(label_7, 4, 0, 1, 1);

        label_14 = new QLabel(groupBox_2);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        gridLayout_2->addWidget(label_14, 4, 2, 1, 1);

        amuQ3 = new QDoubleSpinBox(groupBox_2);
        amuQ3->setObjectName(QString::fromUtf8("amuQ3"));
        amuQ3->setMaximum(1e+09);
        amuQ3->setValue(0.5);

        gridLayout_2->addWidget(amuQ3, 4, 3, 1, 1);

        amuQ1 = new QDoubleSpinBox(groupBox_2);
        amuQ1->setObjectName(QString::fromUtf8("amuQ1"));
        amuQ1->setValue(0.5);

        gridLayout_2->addWidget(amuQ1, 4, 1, 1, 1);

        instrumentType = new QComboBox(groupBox_2);
        instrumentType->setObjectName(QString::fromUtf8("instrumentType"));
        instrumentType->setEnabled(false);

        gridLayout_2->addWidget(instrumentType, 0, 3, 1, 1);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 0, 2, 1, 1);

        ionizationMode = new QComboBox(groupBox_2);
        ionizationMode->setObjectName(QString::fromUtf8("ionizationMode"));

        gridLayout_2->addWidget(ionizationMode, 0, 1, 1, 1);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 0, 0, 1, 1);


        horizontalLayout_2->addWidget(groupBox_2);

        tabWidget->addTab(instrumentationOptions, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_8 = new QGridLayout(tab);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        centroid_scan_flag = new QCheckBox(tab);
        centroid_scan_flag->setObjectName(QString::fromUtf8("centroid_scan_flag"));

        gridLayout_8->addWidget(centroid_scan_flag, 0, 0, 1, 1);

        label_15 = new QLabel(tab);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout_8->addWidget(label_15, 2, 0, 1, 1);

        scan_filter_min_intensity = new QSpinBox(tab);
        scan_filter_min_intensity->setObjectName(QString::fromUtf8("scan_filter_min_intensity"));
        scan_filter_min_intensity->setMaximum(1000000000);

        gridLayout_8->addWidget(scan_filter_min_intensity, 2, 1, 1, 1);

        label_16 = new QLabel(tab);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout_8->addWidget(label_16, 1, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_8->addItem(horizontalSpacer, 1, 2, 1, 1);

        scan_filter_min_quantile = new QSpinBox(tab);
        scan_filter_min_quantile->setObjectName(QString::fromUtf8("scan_filter_min_quantile"));

        gridLayout_8->addWidget(scan_filter_min_quantile, 1, 1, 1, 1);

        tabWidget->addTab(tab, QString());
        peakDetectOptions = new QWidget();
        peakDetectOptions->setObjectName(QString::fromUtf8("peakDetectOptions"));
        gridLayout_7 = new QGridLayout(peakDetectOptions);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        groupBox_3 = new QGroupBox(peakDetectOptions);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        gridLayout1 = new QGridLayout(groupBox_3);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        label_2 = new QLabel(groupBox_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout1->addWidget(label_2, 1, 0, 1, 1);

        eic_smoothingWindow = new QSpinBox(groupBox_3);
        eic_smoothingWindow->setObjectName(QString::fromUtf8("eic_smoothingWindow"));
        eic_smoothingWindow->setMinimum(1);
        eic_smoothingWindow->setValue(1);

        gridLayout1->addWidget(eic_smoothingWindow, 1, 1, 1, 1);

        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout1->addWidget(label_5, 1, 2, 1, 1);

        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout1->addWidget(label_6, 3, 0, 1, 1);

        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout1->addWidget(label_8, 3, 2, 1, 1);

        grouping_maxRtWindow = new QDoubleSpinBox(groupBox_3);
        grouping_maxRtWindow->setObjectName(QString::fromUtf8("grouping_maxRtWindow"));
        grouping_maxRtWindow->setMaximum(1e+09);
        grouping_maxRtWindow->setSingleStep(0.1);
        grouping_maxRtWindow->setValue(0.25);

        gridLayout1->addWidget(grouping_maxRtWindow, 3, 1, 1, 1);

        label_22 = new QLabel(groupBox_3);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        gridLayout1->addWidget(label_22, 0, 0, 1, 1);

        eic_smoothingAlgorithm = new QComboBox(groupBox_3);
        eic_smoothingAlgorithm->setObjectName(QString::fromUtf8("eic_smoothingAlgorithm"));

        gridLayout1->addWidget(eic_smoothingAlgorithm, 0, 1, 1, 1);


        gridLayout_7->addWidget(groupBox_3, 0, 0, 1, 1);

        recomputeEICButton = new QPushButton(peakDetectOptions);
        recomputeEICButton->setObjectName(QString::fromUtf8("recomputeEICButton"));

        gridLayout_7->addWidget(recomputeEICButton, 2, 0, 1, 1);

        groupBox_7 = new QGroupBox(peakDetectOptions);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        gridLayout_6 = new QGridLayout(groupBox_7);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        label_20 = new QLabel(groupBox_7);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        gridLayout_6->addWidget(label_20, 1, 0, 1, 1);

        baseline_smoothing = new QSpinBox(groupBox_7);
        baseline_smoothing->setObjectName(QString::fromUtf8("baseline_smoothing"));
        baseline_smoothing->setMaximum(1000000);
        baseline_smoothing->setValue(5);

        gridLayout_6->addWidget(baseline_smoothing, 1, 1, 1, 1);

        baseline_quantile = new QSpinBox(groupBox_7);
        baseline_quantile->setObjectName(QString::fromUtf8("baseline_quantile"));
        baseline_quantile->setMaximum(100);
        baseline_quantile->setValue(80);

        gridLayout_6->addWidget(baseline_quantile, 0, 1, 1, 1);

        label_21 = new QLabel(groupBox_7);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        gridLayout_6->addWidget(label_21, 0, 0, 1, 1);


        gridLayout_7->addWidget(groupBox_7, 0, 1, 1, 1);

        tabWidget->addTab(peakDetectOptions, QString());
        isotopeDetectOptions = new QWidget();
        isotopeDetectOptions->setObjectName(QString::fromUtf8("isotopeDetectOptions"));
        horizontalLayout = new QHBoxLayout(isotopeDetectOptions);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        groupBox = new QGroupBox(isotopeDetectOptions);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setFlat(false);
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        C13Labeled = new QCheckBox(groupBox);
        C13Labeled->setObjectName(QString::fromUtf8("C13Labeled"));

        gridLayout_3->addWidget(C13Labeled, 2, 0, 1, 1);

        N15Labeled = new QCheckBox(groupBox);
        N15Labeled->setObjectName(QString::fromUtf8("N15Labeled"));

        gridLayout_3->addWidget(N15Labeled, 3, 0, 1, 1);

        S34Labeled = new QCheckBox(groupBox);
        S34Labeled->setObjectName(QString::fromUtf8("S34Labeled"));

        gridLayout_3->addWidget(S34Labeled, 5, 0, 1, 1);

        D2Labeled = new QCheckBox(groupBox);
        D2Labeled->setObjectName(QString::fromUtf8("D2Labeled"));
        D2Labeled->setEnabled(true);

        gridLayout_3->addWidget(D2Labeled, 0, 0, 1, 1);


        horizontalLayout->addWidget(groupBox);

        groupBox_4 = new QGroupBox(isotopeDetectOptions);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        gridLayout_4 = new QGridLayout(groupBox_4);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        label_10 = new QLabel(groupBox_4);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout_4->addWidget(label_10, 3, 0, 1, 1);

        maxNaturalAbundanceErr = new QDoubleSpinBox(groupBox_4);
        maxNaturalAbundanceErr->setObjectName(QString::fromUtf8("maxNaturalAbundanceErr"));
        maxNaturalAbundanceErr->setDecimals(2);
        maxNaturalAbundanceErr->setMinimum(0);
        maxNaturalAbundanceErr->setMaximum(1e+09);
        maxNaturalAbundanceErr->setValue(100);

        gridLayout_4->addWidget(maxNaturalAbundanceErr, 3, 1, 1, 1);

        label_11 = new QLabel(groupBox_4);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout_4->addWidget(label_11, 1, 0, 1, 1);

        label = new QLabel(groupBox_4);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_4->addWidget(label, 0, 0, 1, 1);

        minIsotopicCorrelation = new QDoubleSpinBox(groupBox_4);
        minIsotopicCorrelation->setObjectName(QString::fromUtf8("minIsotopicCorrelation"));
        minIsotopicCorrelation->setMinimum(-1);
        minIsotopicCorrelation->setMaximum(1);
        minIsotopicCorrelation->setSingleStep(0.1);
        minIsotopicCorrelation->setValue(0.2);

        gridLayout_4->addWidget(minIsotopicCorrelation, 0, 1, 1, 1);

        maxIsotopeScanDiff = new QSpinBox(groupBox_4);
        maxIsotopeScanDiff->setObjectName(QString::fromUtf8("maxIsotopeScanDiff"));
        maxIsotopeScanDiff->setMinimum(0);
        maxIsotopeScanDiff->setMaximum(999999999);
        maxIsotopeScanDiff->setValue(5);

        gridLayout_4->addWidget(maxIsotopeScanDiff, 1, 1, 1, 1);

        isotopeC13Correction = new QCheckBox(groupBox_4);
        isotopeC13Correction->setObjectName(QString::fromUtf8("isotopeC13Correction"));

        gridLayout_4->addWidget(isotopeC13Correction, 4, 0, 1, 2);


        horizontalLayout->addWidget(groupBox_4);

        tabWidget->addTab(isotopeDetectOptions, QString());
        clusteringOptions = new QWidget();
        clusteringOptions->setObjectName(QString::fromUtf8("clusteringOptions"));
        verticalLayout = new QVBoxLayout(clusteringOptions);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox_5 = new QGroupBox(clusteringOptions);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        gridLayout_5 = new QGridLayout(groupBox_5);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        label_9 = new QLabel(groupBox_5);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout_5->addWidget(label_9, 0, 0, 1, 1);

        label_12 = new QLabel(groupBox_5);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout_5->addWidget(label_12, 2, 0, 1, 1);

        minSampleCorrelation = new QDoubleSpinBox(groupBox_5);
        minSampleCorrelation->setObjectName(QString::fromUtf8("minSampleCorrelation"));
        minSampleCorrelation->setMinimum(-1);
        minSampleCorrelation->setMaximum(1);
        minSampleCorrelation->setSingleStep(0.1);
        minSampleCorrelation->setValue(0.5);

        gridLayout_5->addWidget(minSampleCorrelation, 2, 1, 1, 1);

        label_13 = new QLabel(groupBox_5);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        gridLayout_5->addWidget(label_13, 3, 0, 1, 1);

        minRtCorrelation = new QDoubleSpinBox(groupBox_5);
        minRtCorrelation->setObjectName(QString::fromUtf8("minRtCorrelation"));
        minRtCorrelation->setMinimum(-1);
        minRtCorrelation->setMaximum(1);
        minRtCorrelation->setSingleStep(0.1);
        minRtCorrelation->setValue(0.5);

        gridLayout_5->addWidget(minRtCorrelation, 0, 1, 1, 1);

        maxRtDiff = new QDoubleSpinBox(groupBox_5);
        maxRtDiff->setObjectName(QString::fromUtf8("maxRtDiff"));
        maxRtDiff->setMaximum(1000);
        maxRtDiff->setValue(0.5);

        gridLayout_5->addWidget(maxRtDiff, 3, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_5->addItem(horizontalSpacer_2, 2, 2, 1, 1);


        verticalLayout->addWidget(groupBox_5);

        tabWidget->addTab(clusteringOptions, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        horizontalLayout_3 = new QHBoxLayout(tab_2);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        groupBox_6 = new QGroupBox(tab_2);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        formLayout_2 = new QFormLayout(groupBox_6);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        notes_server_url = new QLineEdit(groupBox_6);
        notes_server_url->setObjectName(QString::fromUtf8("notes_server_url"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, notes_server_url);

        search_server_url = new QLineEdit(groupBox_6);
        search_server_url->setObjectName(QString::fromUtf8("search_server_url"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, search_server_url);

        pathway_server_url = new QLineEdit(groupBox_6);
        pathway_server_url->setObjectName(QString::fromUtf8("pathway_server_url"));

        formLayout_2->setWidget(4, QFormLayout::FieldRole, pathway_server_url);

        label_19 = new QLabel(groupBox_6);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_19);

        label_18 = new QLabel(groupBox_6);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_18);

        label_17 = new QLabel(groupBox_6);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_17);


        horizontalLayout_3->addWidget(groupBox_6);

        tabWidget->addTab(tab_2, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);


        retranslateUi(SettingsForm);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(SettingsForm);
    } // setupUi

    void retranslateUi(QWidget *SettingsForm)
    {
        SettingsForm->setWindowTitle(QApplication::translate("SettingsForm", "Options", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("SettingsForm", "Instrumentation Type", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("SettingsForm", "Q1 Accuracy", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("SettingsForm", "Q3 Accuracy", 0, QApplication::UnicodeUTF8));
        amuQ3->setSuffix(QApplication::translate("SettingsForm", " amu", 0, QApplication::UnicodeUTF8));
        amuQ1->setPrefix(QString());
        amuQ1->setSuffix(QApplication::translate("SettingsForm", " amu", 0, QApplication::UnicodeUTF8));
        instrumentType->clear();
        instrumentType->insertItems(0, QStringList()
         << QApplication::translate("SettingsForm", "Full Spectrum", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SettingsForm", "Triple Quad", 0, QApplication::UnicodeUTF8)
        );
        label_4->setText(QApplication::translate("SettingsForm", "Instrument Type:", 0, QApplication::UnicodeUTF8));
        ionizationMode->clear();
        ionizationMode->insertItems(0, QStringList()
         << QApplication::translate("SettingsForm", "Auto Detect", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SettingsForm", "Neutral (0)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SettingsForm", "Positive (+1)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SettingsForm", "Negative (-1)", 0, QApplication::UnicodeUTF8)
        );
        label_3->setText(QApplication::translate("SettingsForm", "Polarity / Ionization Mode:", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(instrumentationOptions), QApplication::translate("SettingsForm", "Instrumentation", 0, QApplication::UnicodeUTF8));
        centroid_scan_flag->setText(QApplication::translate("SettingsForm", "Centroid Scans", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("SettingsForm", "Scan Filter:  Minimum Intensity ", 0, QApplication::UnicodeUTF8));
        scan_filter_min_intensity->setSuffix(QApplication::translate("SettingsForm", " ions", 0, QApplication::UnicodeUTF8));
        scan_filter_min_intensity->setPrefix(QString());
        label_16->setText(QApplication::translate("SettingsForm", "Scan Filter Intensity Minimum Quantile Cutoff ", 0, QApplication::UnicodeUTF8));
        scan_filter_min_quantile->setSuffix(QApplication::translate("SettingsForm", " %", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("SettingsForm", "File Import", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("SettingsForm", "Peak Detection and Grouping Settings", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("SettingsForm", "EIC Smoothing Window ", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("SettingsForm", "scans", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("SettingsForm", "Max Retention Time difference between Peaks", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("SettingsForm", "min", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("SettingsForm", "EIC Smoothing Algorithm", 0, QApplication::UnicodeUTF8));
        eic_smoothingAlgorithm->clear();
        eic_smoothingAlgorithm->insertItems(0, QStringList()
         << QApplication::translate("SettingsForm", "Savitzky-Golay", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SettingsForm", "Gaussian", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SettingsForm", "Moving Average", 0, QApplication::UnicodeUTF8)
        );
        recomputeEICButton->setText(QApplication::translate("SettingsForm", "Recompute EIC", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("SettingsForm", "Baseline Calculation", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("SettingsForm", "Baseline Smooting ", 0, QApplication::UnicodeUTF8));
        baseline_smoothing->setSuffix(QApplication::translate("SettingsForm", " scans", 0, QApplication::UnicodeUTF8));
        baseline_quantile->setSuffix(QApplication::translate("SettingsForm", " %", 0, QApplication::UnicodeUTF8));
        baseline_quantile->setPrefix(QString());
        label_21->setText(QApplication::translate("SettingsForm", "Drop top x%  intensities from chromatogram", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(peakDetectOptions), QApplication::translate("SettingsForm", "Peak Detection", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("SettingsForm", "Samples Are Labeled? ", 0, QApplication::UnicodeUTF8));
        C13Labeled->setText(QApplication::translate("SettingsForm", "C13 Labeled ", 0, QApplication::UnicodeUTF8));
        N15Labeled->setText(QApplication::translate("SettingsForm", "N15  Labeled", 0, QApplication::UnicodeUTF8));
        S34Labeled->setText(QApplication::translate("SettingsForm", "S34 Labeled", 0, QApplication::UnicodeUTF8));
        D2Labeled->setText(QApplication::translate("SettingsForm", "D2 Labeled", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("SettingsForm", "Filter Isotopic Peaks based on these criteria", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("SettingsForm", "Maximum %Error to Natural Abundance", 0, QApplication::UnicodeUTF8));
        maxNaturalAbundanceErr->setPrefix(QString());
        maxNaturalAbundanceErr->setSuffix(QApplication::translate("SettingsForm", " %", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("SettingsForm", "Isotope is within [X] scans of parent", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SettingsForm", "Minimum Isotope-parent correlation", 0, QApplication::UnicodeUTF8));
        maxIsotopeScanDiff->setSuffix(QApplication::translate("SettingsForm", " scans", 0, QApplication::UnicodeUTF8));
        isotopeC13Correction->setText(QApplication::translate("SettingsForm", "Correct for Natural C13 Isotope Abundance", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(isotopeDetectOptions), QApplication::translate("SettingsForm", "Isotope Detection", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("SettingsForm", "Group Clustering Options", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("SettingsForm", "Minimum Retention Time Correlation ", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("SettingsForm", "Minimum Cross Sample Peak Size Correlation", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("SettingsForm", "Maximum Retention Time Difference", 0, QApplication::UnicodeUTF8));
        maxRtDiff->setSuffix(QApplication::translate("SettingsForm", " min", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(clusteringOptions), QApplication::translate("SettingsForm", "Group Clustering", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("SettingsForm", "Web Servers and Links", 0, QApplication::UnicodeUTF8));
        notes_server_url->setText(QApplication::translate("SettingsForm", "http://10.228.70.159/idms/note_submission.php", 0, QApplication::UnicodeUTF8));
        search_server_url->setText(QApplication::translate("SettingsForm", "http://www.google.com/cse?cx=016585488632464380718%3A67c3i4mncde&ie=UTF-8&q=", 0, QApplication::UnicodeUTF8));
        pathway_server_url->setText(QApplication::translate("SettingsForm", "http://www.ncbi.nlm.nih.gov/sites/entrez?db=pccompound&term=", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("SettingsForm", "Pathway Server Url", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("SettingsForm", "Chemical Name Search Url", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("SettingsForm", "Remote Notes Server Url", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("SettingsForm", "Internet Links", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SettingsForm: public Ui_SettingsForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSFORM_H
