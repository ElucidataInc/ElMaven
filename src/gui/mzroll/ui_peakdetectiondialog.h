/********************************************************************************
** Form generated from reading UI file 'peakdetectiondialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PEAKDETECTIONDIALOG_H
#define UI_PEAKDETECTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PeakDetectionDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabwidget;
    QWidget *featureSelectionTab;
    QGridLayout *gridLayout_10;
    QGroupBox *featureOptions;
    QGridLayout *gridLayout;
    QLabel *label_17;
    QLabel *label_19;
    QDoubleSpinBox *rtMax;
    QLabel *label_20;
    QRadioButton *ignoreIsotopes;
    QDoubleSpinBox *ppmStep;
    QDoubleSpinBox *rtStep;
    QDoubleSpinBox *minIntensity;
    QSpinBox *chargeMin;
    QDoubleSpinBox *maxIntensity;
    QLabel *label;
    QSpinBox *chargeMax;
    QDoubleSpinBox *mzMax;
    QDoubleSpinBox *rtMin;
    QLabel *label_7;
    QDoubleSpinBox *mzMin;
    QLabel *label_18;
    QGroupBox *dbOptions;
    QGridLayout *gridLayout_2;
    QLabel *label_3;
    QLabel *label_16;
    QDoubleSpinBox *compoundPPMWindow;
    QSpinBox *eicMaxGroups;
    QLabel *label_11;
    QDoubleSpinBox *compoundRTWindow;
    QComboBox *compoundDatabase;
    QCheckBox *matchRt;
    QGroupBox *matchFragmentatioOptions;
    QGridLayout *gridLayout_8;
    QDoubleSpinBox *doubleSpinBox;
    QDoubleSpinBox *minFragMatchScore;
    QLabel *label_21;
    QDoubleSpinBox *doubleSpinBox_2;
    QLabel *label_4;
    QLabel *label_22;
    QGroupBox *reportIsotopesOptions;
    QVBoxLayout *verticalLayout_2;
    QPushButton *changeIsotopeOptions;
    QWidget *groupFilteringTab;
    QGridLayout *gridLayout_13;
    QGroupBox *peakScoringOptions;
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout_5;
    QLabel *label_28;
    QSpacerItem *verticalSpacer_3;
    QSpinBox *minGoodGroupCount;
    QLabel *qualityQuantileStatus;
    QLabel *label_8;
    QSpinBox *minNoNoiseObs;
    QSlider *quantileQuality;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_10;
    QComboBox *peakQuantitation;
    QLabel *label_27;
    QDoubleSpinBox *minGroupIntensity;
    QLabel *label_6;
    QLabel *label_29;
    QDoubleSpinBox *sigBlankRatio;
    QLabel *blankQuantileStatus;
    QLabel *baselineQuantileStatus;
    QSlider *quantileSignalBlankRatio;
    QLabel *label_30;
    QLabel *labelMinQuality;
    QSpinBox *sigBaselineRatio;
    QSpacerItem *verticalSpacer_4;
    QLabel *label_12;
    QLabel *intensityQuantileStatus;
    QSpacerItem *verticalSpacer;
    QSlider *quantileIntensity;
    QDoubleSpinBox *doubleSpinBoxMinQuality;
    QLabel *label_100;
    QLabel *label_9;
    QSlider *quantileSignalBaselineRatio;
    QLabel *label_5;
    QLineEdit *classificationModelFilename;
    QPushButton *loadModelButton;
    QWidget *methodSummaryTab;
    QGridLayout *gridLayout_11;
    QGroupBox *methodSummaryOptions;
    QGridLayout *gridLayout_7;
    QPushButton *saveMethodButton;
    QPushButton *loadMethodButton;
    QTextBrowser *methodSummary;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_9;
    QPushButton *setOutputDirButton;
    QLineEdit *outputDirName;
    QLabel *label_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_12;
    QComboBox *outputTableComboBox;
    QLabel *label_23;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_4;
    QPushButton *cancelButton;
    QPushButton *computeButton;
    QProgressBar *progressBar;
    QLabel *statusText;

    void setupUi(QDialog *PeakDetectionDialog)
    {
        if (PeakDetectionDialog->objectName().isEmpty())
            PeakDetectionDialog->setObjectName(QStringLiteral("PeakDetectionDialog"));
        PeakDetectionDialog->resize(876, 740);
        PeakDetectionDialog->setSizeIncrement(QSize(0, 0));
        verticalLayout = new QVBoxLayout(PeakDetectionDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabwidget = new QTabWidget(PeakDetectionDialog);
        tabwidget->setObjectName(QStringLiteral("tabwidget"));
        featureSelectionTab = new QWidget();
        featureSelectionTab->setObjectName(QStringLiteral("featureSelectionTab"));
        gridLayout_10 = new QGridLayout(featureSelectionTab);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        featureOptions = new QGroupBox(featureSelectionTab);
        featureOptions->setObjectName(QStringLiteral("featureOptions"));
        featureOptions->setEnabled(true);
        featureOptions->setAutoFillBackground(false);
        featureOptions->setFlat(false);
        featureOptions->setCheckable(true);
        featureOptions->setChecked(false);
        gridLayout = new QGridLayout(featureOptions);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_17 = new QLabel(featureOptions);
        label_17->setObjectName(QStringLiteral("label_17"));

        gridLayout->addWidget(label_17, 0, 2, 1, 1);

        label_19 = new QLabel(featureOptions);
        label_19->setObjectName(QStringLiteral("label_19"));

        gridLayout->addWidget(label_19, 2, 2, 1, 1);

        rtMax = new QDoubleSpinBox(featureOptions);
        rtMax->setObjectName(QStringLiteral("rtMax"));
        rtMax->setMaximum(1e+09);
        rtMax->setValue(1e+09);

        gridLayout->addWidget(rtMax, 1, 4, 1, 1);

        label_20 = new QLabel(featureOptions);
        label_20->setObjectName(QStringLiteral("label_20"));

        gridLayout->addWidget(label_20, 3, 2, 1, 1);

        ignoreIsotopes = new QRadioButton(featureOptions);
        ignoreIsotopes->setObjectName(QStringLiteral("ignoreIsotopes"));

        gridLayout->addWidget(ignoreIsotopes, 2, 0, 1, 2);

        ppmStep = new QDoubleSpinBox(featureOptions);
        ppmStep->setObjectName(QStringLiteral("ppmStep"));
        ppmStep->setDecimals(3);
        ppmStep->setMaximum(1e+09);
        ppmStep->setSingleStep(1);
        ppmStep->setValue(20);

        gridLayout->addWidget(ppmStep, 0, 1, 1, 1);

        rtStep = new QDoubleSpinBox(featureOptions);
        rtStep->setObjectName(QStringLiteral("rtStep"));
        rtStep->setDecimals(0);
        rtStep->setMinimum(1);
        rtStep->setMaximum(1e+06);
        rtStep->setSingleStep(5);
        rtStep->setValue(10);

        gridLayout->addWidget(rtStep, 1, 1, 1, 1);

        minIntensity = new QDoubleSpinBox(featureOptions);
        minIntensity->setObjectName(QStringLiteral("minIntensity"));
        minIntensity->setMaximum(1e+10);
        minIntensity->setValue(50000);

        gridLayout->addWidget(minIntensity, 2, 3, 1, 1);

        chargeMin = new QSpinBox(featureOptions);
        chargeMin->setObjectName(QStringLiteral("chargeMin"));
        chargeMin->setMinimum(0);
        chargeMin->setMaximum(999999999);
        chargeMin->setValue(0);

        gridLayout->addWidget(chargeMin, 3, 3, 1, 1);

        maxIntensity = new QDoubleSpinBox(featureOptions);
        maxIntensity->setObjectName(QStringLiteral("maxIntensity"));
        maxIntensity->setMaximum(1e+10);
        maxIntensity->setValue(1e+10);

        gridLayout->addWidget(maxIntensity, 2, 4, 1, 1);

        label = new QLabel(featureOptions);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        chargeMax = new QSpinBox(featureOptions);
        chargeMax->setObjectName(QStringLiteral("chargeMax"));
        chargeMax->setMinimum(0);
        chargeMax->setMaximum(999999999);
        chargeMax->setValue(0);

        gridLayout->addWidget(chargeMax, 3, 4, 1, 1);

        mzMax = new QDoubleSpinBox(featureOptions);
        mzMax->setObjectName(QStringLiteral("mzMax"));
        mzMax->setMaximum(1e+09);
        mzMax->setValue(1e+09);

        gridLayout->addWidget(mzMax, 0, 4, 1, 1);

        rtMin = new QDoubleSpinBox(featureOptions);
        rtMin->setObjectName(QStringLiteral("rtMin"));
        rtMin->setMaximum(1e+09);

        gridLayout->addWidget(rtMin, 1, 3, 1, 1);

        label_7 = new QLabel(featureOptions);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 0, 0, 1, 1);

        mzMin = new QDoubleSpinBox(featureOptions);
        mzMin->setObjectName(QStringLiteral("mzMin"));
        mzMin->setMaximum(1e+09);

        gridLayout->addWidget(mzMin, 0, 3, 1, 1);

        label_18 = new QLabel(featureOptions);
        label_18->setObjectName(QStringLiteral("label_18"));

        gridLayout->addWidget(label_18, 1, 2, 1, 1);


        gridLayout_10->addWidget(featureOptions, 0, 0, 1, 2);

        dbOptions = new QGroupBox(featureSelectionTab);
        dbOptions->setObjectName(QStringLiteral("dbOptions"));
        dbOptions->setCheckable(true);
        dbOptions->setChecked(false);
        gridLayout_2 = new QGridLayout(dbOptions);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_3 = new QLabel(dbOptions);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_2->addWidget(label_3, 0, 0, 1, 1);

        label_16 = new QLabel(dbOptions);
        label_16->setObjectName(QStringLiteral("label_16"));

        gridLayout_2->addWidget(label_16, 4, 0, 1, 1);

        compoundPPMWindow = new QDoubleSpinBox(dbOptions);
        compoundPPMWindow->setObjectName(QStringLiteral("compoundPPMWindow"));
        compoundPPMWindow->setMaximum(1e+06);
        compoundPPMWindow->setValue(20);

        gridLayout_2->addWidget(compoundPPMWindow, 1, 2, 1, 1);

        eicMaxGroups = new QSpinBox(dbOptions);
        eicMaxGroups->setObjectName(QStringLiteral("eicMaxGroups"));
        eicMaxGroups->setMinimum(1);
        eicMaxGroups->setMaximum(999999999);
        eicMaxGroups->setValue(10);

        gridLayout_2->addWidget(eicMaxGroups, 4, 2, 1, 1);

        label_11 = new QLabel(dbOptions);
        label_11->setObjectName(QStringLiteral("label_11"));

        gridLayout_2->addWidget(label_11, 1, 0, 1, 1);

        compoundRTWindow = new QDoubleSpinBox(dbOptions);
        compoundRTWindow->setObjectName(QStringLiteral("compoundRTWindow"));
        compoundRTWindow->setMaximum(1e+09);
        compoundRTWindow->setSingleStep(0.5);
        compoundRTWindow->setValue(1);

        gridLayout_2->addWidget(compoundRTWindow, 3, 2, 1, 1);

        compoundDatabase = new QComboBox(dbOptions);
        compoundDatabase->setObjectName(QStringLiteral("compoundDatabase"));

        gridLayout_2->addWidget(compoundDatabase, 0, 2, 1, 1);

        matchRt = new QCheckBox(dbOptions);
        matchRt->setObjectName(QStringLiteral("matchRt"));
        matchRt->setChecked(false);

        gridLayout_2->addWidget(matchRt, 3, 0, 1, 1);


        gridLayout_10->addWidget(dbOptions, 1, 0, 2, 2);

        matchFragmentatioOptions = new QGroupBox(featureSelectionTab);
        matchFragmentatioOptions->setObjectName(QStringLiteral("matchFragmentatioOptions"));
        matchFragmentatioOptions->setEnabled(false);
        matchFragmentatioOptions->setCheckable(true);
        matchFragmentatioOptions->setChecked(false);
        gridLayout_8 = new QGridLayout(matchFragmentatioOptions);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        doubleSpinBox = new QDoubleSpinBox(matchFragmentatioOptions);
        doubleSpinBox->setObjectName(QStringLiteral("doubleSpinBox"));

        gridLayout_8->addWidget(doubleSpinBox, 1, 1, 1, 1);

        minFragMatchScore = new QDoubleSpinBox(matchFragmentatioOptions);
        minFragMatchScore->setObjectName(QStringLiteral("minFragMatchScore"));
        minFragMatchScore->setMaximum(1);
        minFragMatchScore->setSingleStep(0.1);

        gridLayout_8->addWidget(minFragMatchScore, 2, 1, 1, 1);

        label_21 = new QLabel(matchFragmentatioOptions);
        label_21->setObjectName(QStringLiteral("label_21"));

        gridLayout_8->addWidget(label_21, 0, 0, 1, 1);

        doubleSpinBox_2 = new QDoubleSpinBox(matchFragmentatioOptions);
        doubleSpinBox_2->setObjectName(QStringLiteral("doubleSpinBox_2"));

        gridLayout_8->addWidget(doubleSpinBox_2, 0, 1, 1, 1);

        label_4 = new QLabel(matchFragmentatioOptions);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_8->addWidget(label_4, 1, 0, 1, 1);

        label_22 = new QLabel(matchFragmentatioOptions);
        label_22->setObjectName(QStringLiteral("label_22"));

        gridLayout_8->addWidget(label_22, 2, 0, 1, 1);

        label_22->raise();
        minFragMatchScore->raise();
        doubleSpinBox->raise();
        label_4->raise();
        doubleSpinBox_2->raise();
        label_21->raise();

        gridLayout_10->addWidget(matchFragmentatioOptions, 4, 0, 1, 1);

        reportIsotopesOptions = new QGroupBox(featureSelectionTab);
        reportIsotopesOptions->setObjectName(QStringLiteral("reportIsotopesOptions"));
        reportIsotopesOptions->setEnabled(false);
        reportIsotopesOptions->setCheckable(true);
        reportIsotopesOptions->setChecked(false);
        verticalLayout_2 = new QVBoxLayout(reportIsotopesOptions);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        changeIsotopeOptions = new QPushButton(reportIsotopesOptions);
        changeIsotopeOptions->setObjectName(QStringLiteral("changeIsotopeOptions"));

        verticalLayout_2->addWidget(changeIsotopeOptions);


        gridLayout_10->addWidget(reportIsotopesOptions, 4, 1, 1, 1);

        tabwidget->addTab(featureSelectionTab, QString());
        groupFilteringTab = new QWidget();
        groupFilteringTab->setObjectName(QStringLiteral("groupFilteringTab"));
        gridLayout_13 = new QGridLayout(groupFilteringTab);
        gridLayout_13->setObjectName(QStringLiteral("gridLayout_13"));
        peakScoringOptions = new QGroupBox(groupFilteringTab);
        peakScoringOptions->setObjectName(QStringLiteral("peakScoringOptions"));
        gridLayout_3 = new QGridLayout(peakScoringOptions);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_5 = new QGridLayout();
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        label_28 = new QLabel(peakScoringOptions);
        label_28->setObjectName(QStringLiteral("label_28"));
        label_28->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_28, 4, 0, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_3, 8, 1, 1, 1);

        minGoodGroupCount = new QSpinBox(peakScoringOptions);
        minGoodGroupCount->setObjectName(QStringLiteral("minGoodGroupCount"));
        minGoodGroupCount->setMaximum(100000000);
        minGoodGroupCount->setValue(1);

        gridLayout_5->addWidget(minGoodGroupCount, 13, 1, 1, 1);

        qualityQuantileStatus = new QLabel(peakScoringOptions);
        qualityQuantileStatus->setObjectName(QStringLiteral("qualityQuantileStatus"));

        gridLayout_5->addWidget(qualityQuantileStatus, 4, 2, 1, 1);

        label_8 = new QLabel(peakScoringOptions);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_8, 1, 0, 1, 1);

        minNoNoiseObs = new QSpinBox(peakScoringOptions);
        minNoNoiseObs->setObjectName(QStringLiteral("minNoNoiseObs"));
        minNoNoiseObs->setMinimum(1);
        minNoNoiseObs->setMaximum(1000000000);
        minNoNoiseObs->setValue(3);

        gridLayout_5->addWidget(minNoNoiseObs, 12, 1, 1, 1);

        quantileQuality = new QSlider(peakScoringOptions);
        quantileQuality->setObjectName(QStringLiteral("quantileQuality"));
        quantileQuality->setMaximum(100);
        quantileQuality->setSingleStep(5);
        quantileQuality->setSliderPosition(0);
        quantileQuality->setOrientation(Qt::Horizontal);
        quantileQuality->setTickPosition(QSlider::TicksBelow);
        quantileQuality->setTickInterval(5);

        gridLayout_5->addWidget(quantileQuality, 4, 1, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_2, 5, 1, 1, 1);

        label_10 = new QLabel(peakScoringOptions);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_10, 0, 0, 1, 1);

        peakQuantitation = new QComboBox(peakScoringOptions);
        peakQuantitation->setObjectName(QStringLiteral("peakQuantitation"));

        gridLayout_5->addWidget(peakQuantitation, 0, 2, 1, 1);

        label_27 = new QLabel(peakScoringOptions);
        label_27->setObjectName(QStringLiteral("label_27"));

        gridLayout_5->addWidget(label_27, 5, 0, 1, 1);

        minGroupIntensity = new QDoubleSpinBox(peakScoringOptions);
        minGroupIntensity->setObjectName(QStringLiteral("minGroupIntensity"));
        minGroupIntensity->setMaximumSize(QSize(502, 16777215));
        minGroupIntensity->setProperty("showGroupSeparator", QVariant(true));
        minGroupIntensity->setMaximum(1e+09);
        minGroupIntensity->setValue(1e+06);

        gridLayout_5->addWidget(minGroupIntensity, 0, 1, 1, 1);

        label_6 = new QLabel(peakScoringOptions);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_6, 9, 0, 1, 1);

        label_29 = new QLabel(peakScoringOptions);
        label_29->setObjectName(QStringLiteral("label_29"));
        label_29->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_29, 7, 0, 1, 1);

        sigBlankRatio = new QDoubleSpinBox(peakScoringOptions);
        sigBlankRatio->setObjectName(QStringLiteral("sigBlankRatio"));
        sigBlankRatio->setMaximum(1e+07);
        sigBlankRatio->setValue(2);

        gridLayout_5->addWidget(sigBlankRatio, 6, 1, 1, 1);

        blankQuantileStatus = new QLabel(peakScoringOptions);
        blankQuantileStatus->setObjectName(QStringLiteral("blankQuantileStatus"));

        gridLayout_5->addWidget(blankQuantileStatus, 7, 2, 1, 1);

        baselineQuantileStatus = new QLabel(peakScoringOptions);
        baselineQuantileStatus->setObjectName(QStringLiteral("baselineQuantileStatus"));

        gridLayout_5->addWidget(baselineQuantileStatus, 10, 2, 1, 1);

        quantileSignalBlankRatio = new QSlider(peakScoringOptions);
        quantileSignalBlankRatio->setObjectName(QStringLiteral("quantileSignalBlankRatio"));
        quantileSignalBlankRatio->setMaximum(100);
        quantileSignalBlankRatio->setSingleStep(5);
        quantileSignalBlankRatio->setSliderPosition(0);
        quantileSignalBlankRatio->setOrientation(Qt::Horizontal);
        quantileSignalBlankRatio->setTickPosition(QSlider::TicksBelow);
        quantileSignalBlankRatio->setTickInterval(5);

        gridLayout_5->addWidget(quantileSignalBlankRatio, 7, 1, 1, 1);

        label_30 = new QLabel(peakScoringOptions);
        label_30->setObjectName(QStringLiteral("label_30"));
        label_30->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_30, 10, 0, 1, 1);

        labelMinQuality = new QLabel(peakScoringOptions);
        labelMinQuality->setObjectName(QStringLiteral("labelMinQuality"));
        labelMinQuality->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(labelMinQuality, 3, 0, 1, 1);

        sigBaselineRatio = new QSpinBox(peakScoringOptions);
        sigBaselineRatio->setObjectName(QStringLiteral("sigBaselineRatio"));
        sigBaselineRatio->setMaximum(10000000);
        sigBaselineRatio->setValue(2);

        gridLayout_5->addWidget(sigBaselineRatio, 9, 1, 1, 1);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_4, 11, 1, 1, 1);

        label_12 = new QLabel(peakScoringOptions);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_12, 12, 0, 1, 1);

        intensityQuantileStatus = new QLabel(peakScoringOptions);
        intensityQuantileStatus->setObjectName(QStringLiteral("intensityQuantileStatus"));

        gridLayout_5->addWidget(intensityQuantileStatus, 1, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer, 2, 1, 1, 1);

        quantileIntensity = new QSlider(peakScoringOptions);
        quantileIntensity->setObjectName(QStringLiteral("quantileIntensity"));
        quantileIntensity->setMaximum(100);
        quantileIntensity->setSingleStep(5);
        quantileIntensity->setSliderPosition(0);
        quantileIntensity->setOrientation(Qt::Horizontal);
        quantileIntensity->setTickPosition(QSlider::TicksBelow);
        quantileIntensity->setTickInterval(5);

        gridLayout_5->addWidget(quantileIntensity, 1, 1, 1, 1);

        doubleSpinBoxMinQuality = new QDoubleSpinBox(peakScoringOptions);
        doubleSpinBoxMinQuality->setObjectName(QStringLiteral("doubleSpinBoxMinQuality"));
        doubleSpinBoxMinQuality->setMaximum(1);
        doubleSpinBoxMinQuality->setSingleStep(0.1);
        doubleSpinBoxMinQuality->setValue(0.5);

        gridLayout_5->addWidget(doubleSpinBoxMinQuality, 3, 1, 1, 1);

        label_100 = new QLabel(peakScoringOptions);
        label_100->setObjectName(QStringLiteral("label_100"));
        label_100->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_100, 13, 0, 1, 1);

        label_9 = new QLabel(peakScoringOptions);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_9, 6, 0, 1, 1);

        quantileSignalBaselineRatio = new QSlider(peakScoringOptions);
        quantileSignalBaselineRatio->setObjectName(QStringLiteral("quantileSignalBaselineRatio"));
        quantileSignalBaselineRatio->setMaximum(100);
        quantileSignalBaselineRatio->setSingleStep(5);
        quantileSignalBaselineRatio->setSliderPosition(0);
        quantileSignalBaselineRatio->setOrientation(Qt::Horizontal);
        quantileSignalBaselineRatio->setInvertedAppearance(false);
        quantileSignalBaselineRatio->setInvertedControls(false);
        quantileSignalBaselineRatio->setTickPosition(QSlider::TicksBelow);
        quantileSignalBaselineRatio->setTickInterval(5);

        gridLayout_5->addWidget(quantileSignalBaselineRatio, 10, 1, 1, 1);

        label_5 = new QLabel(peakScoringOptions);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_5, 14, 0, 1, 1);

        classificationModelFilename = new QLineEdit(peakScoringOptions);
        classificationModelFilename->setObjectName(QStringLiteral("classificationModelFilename"));

        gridLayout_5->addWidget(classificationModelFilename, 14, 1, 1, 1);

        loadModelButton = new QPushButton(peakScoringOptions);
        loadModelButton->setObjectName(QStringLiteral("loadModelButton"));

        gridLayout_5->addWidget(loadModelButton, 14, 2, 1, 1);


        gridLayout_3->addLayout(gridLayout_5, 0, 0, 1, 4);


        gridLayout_13->addWidget(peakScoringOptions, 0, 0, 1, 1);

        tabwidget->addTab(groupFilteringTab, QString());
        methodSummaryTab = new QWidget();
        methodSummaryTab->setObjectName(QStringLiteral("methodSummaryTab"));
        gridLayout_11 = new QGridLayout(methodSummaryTab);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        methodSummaryOptions = new QGroupBox(methodSummaryTab);
        methodSummaryOptions->setObjectName(QStringLiteral("methodSummaryOptions"));
        gridLayout_7 = new QGridLayout(methodSummaryOptions);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        saveMethodButton = new QPushButton(methodSummaryOptions);
        saveMethodButton->setObjectName(QStringLiteral("saveMethodButton"));

        gridLayout_7->addWidget(saveMethodButton, 0, 0, 1, 1);

        loadMethodButton = new QPushButton(methodSummaryOptions);
        loadMethodButton->setObjectName(QStringLiteral("loadMethodButton"));

        gridLayout_7->addWidget(loadMethodButton, 0, 1, 1, 1);

        methodSummary = new QTextBrowser(methodSummaryOptions);
        methodSummary->setObjectName(QStringLiteral("methodSummary"));

        gridLayout_7->addWidget(methodSummary, 3, 0, 1, 2);


        gridLayout_11->addWidget(methodSummaryOptions, 3, 1, 1, 1);

        groupBox_3 = new QGroupBox(methodSummaryTab);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setCheckable(true);
        groupBox_3->setChecked(false);
        gridLayout_9 = new QGridLayout(groupBox_3);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        setOutputDirButton = new QPushButton(groupBox_3);
        setOutputDirButton->setObjectName(QStringLiteral("setOutputDirButton"));

        gridLayout_9->addWidget(setOutputDirButton, 1, 2, 1, 1);

        outputDirName = new QLineEdit(groupBox_3);
        outputDirName->setObjectName(QStringLiteral("outputDirName"));

        gridLayout_9->addWidget(outputDirName, 1, 1, 1, 1);

        label_2 = new QLabel(groupBox_3);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_9->addWidget(label_2, 1, 0, 1, 1);


        gridLayout_11->addWidget(groupBox_3, 5, 1, 1, 1);

        groupBox = new QGroupBox(methodSummaryTab);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setCheckable(true);
        groupBox->setChecked(true);
        gridLayout_12 = new QGridLayout(groupBox);
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        outputTableComboBox = new QComboBox(groupBox);
        outputTableComboBox->setObjectName(QStringLiteral("outputTableComboBox"));

        gridLayout_12->addWidget(outputTableComboBox, 0, 1, 1, 1);

        label_23 = new QLabel(groupBox);
        label_23->setObjectName(QStringLiteral("label_23"));

        gridLayout_12->addWidget(label_23, 0, 0, 1, 1);


        gridLayout_11->addWidget(groupBox, 4, 1, 1, 1);

        tabwidget->addTab(methodSummaryTab, QString());

        verticalLayout->addWidget(tabwidget);

        groupBox_4 = new QGroupBox(PeakDetectionDialog);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout_4 = new QGridLayout(groupBox_4);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        cancelButton = new QPushButton(groupBox_4);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        gridLayout_4->addWidget(cancelButton, 4, 5, 1, 1);

        computeButton = new QPushButton(groupBox_4);
        computeButton->setObjectName(QStringLiteral("computeButton"));

        gridLayout_4->addWidget(computeButton, 4, 4, 1, 1);

        progressBar = new QProgressBar(groupBox_4);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(0);

        gridLayout_4->addWidget(progressBar, 4, 3, 1, 1);

        statusText = new QLabel(groupBox_4);
        statusText->setObjectName(QStringLiteral("statusText"));

        gridLayout_4->addWidget(statusText, 3, 3, 1, 1);


        verticalLayout->addWidget(groupBox_4);

        QWidget::setTabOrder(tabwidget, featureOptions);
        QWidget::setTabOrder(featureOptions, ppmStep);
        QWidget::setTabOrder(ppmStep, mzMin);
        QWidget::setTabOrder(mzMin, mzMax);
        QWidget::setTabOrder(mzMax, rtStep);
        QWidget::setTabOrder(rtStep, rtMin);
        QWidget::setTabOrder(rtMin, rtMax);
        QWidget::setTabOrder(rtMax, ignoreIsotopes);
        QWidget::setTabOrder(ignoreIsotopes, minIntensity);
        QWidget::setTabOrder(minIntensity, maxIntensity);
        QWidget::setTabOrder(maxIntensity, chargeMin);
        QWidget::setTabOrder(chargeMin, chargeMax);
        QWidget::setTabOrder(chargeMax, dbOptions);
        QWidget::setTabOrder(dbOptions, compoundDatabase);
        QWidget::setTabOrder(compoundDatabase, compoundPPMWindow);
        QWidget::setTabOrder(compoundPPMWindow, matchRt);
        QWidget::setTabOrder(matchRt, compoundRTWindow);
        QWidget::setTabOrder(compoundRTWindow, eicMaxGroups);
        QWidget::setTabOrder(eicMaxGroups, matchFragmentatioOptions);
        QWidget::setTabOrder(matchFragmentatioOptions, doubleSpinBox_2);
        QWidget::setTabOrder(doubleSpinBox_2, doubleSpinBox);
        QWidget::setTabOrder(doubleSpinBox, minFragMatchScore);
        QWidget::setTabOrder(minFragMatchScore, reportIsotopesOptions);
        QWidget::setTabOrder(reportIsotopesOptions, changeIsotopeOptions);
        QWidget::setTabOrder(changeIsotopeOptions, minGroupIntensity);
        QWidget::setTabOrder(minGroupIntensity, peakQuantitation);
        QWidget::setTabOrder(peakQuantitation, quantileIntensity);
        QWidget::setTabOrder(quantileIntensity, doubleSpinBoxMinQuality);
        QWidget::setTabOrder(doubleSpinBoxMinQuality, quantileQuality);
        QWidget::setTabOrder(quantileQuality, sigBlankRatio);
        QWidget::setTabOrder(sigBlankRatio, quantileSignalBlankRatio);
        QWidget::setTabOrder(quantileSignalBlankRatio, sigBaselineRatio);
        QWidget::setTabOrder(sigBaselineRatio, quantileSignalBaselineRatio);
        QWidget::setTabOrder(quantileSignalBaselineRatio, minNoNoiseObs);
        QWidget::setTabOrder(minNoNoiseObs, minGoodGroupCount);
        QWidget::setTabOrder(minGoodGroupCount, classificationModelFilename);
        QWidget::setTabOrder(classificationModelFilename, loadModelButton);
        QWidget::setTabOrder(loadModelButton, computeButton);
        QWidget::setTabOrder(computeButton, cancelButton);
        QWidget::setTabOrder(cancelButton, saveMethodButton);
        QWidget::setTabOrder(saveMethodButton, loadMethodButton);
        QWidget::setTabOrder(loadMethodButton, methodSummary);
        QWidget::setTabOrder(methodSummary, groupBox);
        QWidget::setTabOrder(groupBox, outputTableComboBox);
        QWidget::setTabOrder(outputTableComboBox, groupBox_3);
        QWidget::setTabOrder(groupBox_3, outputDirName);
        QWidget::setTabOrder(outputDirName, setOutputDirButton);

        retranslateUi(PeakDetectionDialog);

        tabwidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(PeakDetectionDialog);
    } // setupUi

    void retranslateUi(QDialog *PeakDetectionDialog)
    {
        PeakDetectionDialog->setWindowTitle(QApplication::translate("PeakDetectionDialog", "Peak Detection", 0));
#ifndef QT_NO_STATUSTIP
        featureOptions->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
        featureOptions->setTitle(QApplication::translate("PeakDetectionDialog", "Automated Feature Detection:   Find peaks by slicing m/z and retention time space", 0));
        label_17->setText(QApplication::translate("PeakDetectionDialog", "Limit m/z Range", 0));
        label_19->setText(QApplication::translate("PeakDetectionDialog", "Limit Intensity Range", 0));
        label_20->setText(QApplication::translate("PeakDetectionDialog", "Limit Charged Species", 0));
        ignoreIsotopes->setText(QApplication::translate("PeakDetectionDialog", "Auto Detect And Ignore Isotopes", 0));
#ifndef QT_NO_STATUSTIP
        rtStep->setStatusTip(QApplication::translate("PeakDetectionDialog", "mz-rt matrix binning", 0));
#endif // QT_NO_STATUSTIP
        rtStep->setPrefix(QString());
        rtStep->setSuffix(QApplication::translate("PeakDetectionDialog", " scans", 0));
        label->setText(QApplication::translate("PeakDetectionDialog", "Time Domain Resolution (scans)", 0));
        label_7->setText(QApplication::translate("PeakDetectionDialog", "Mass Domain Resolution (ppm)", 0));
        label_18->setText(QApplication::translate("PeakDetectionDialog", "Limit Time Range", 0));
        dbOptions->setTitle(QApplication::translate("PeakDetectionDialog", "Compound Database Search:  Limit slices to set of known m/z and retention time values", 0));
        label_3->setText(QApplication::translate("PeakDetectionDialog", "Select Database", 0));
        label_16->setText(QApplication::translate("PeakDetectionDialog", "Limit Number of Reported Groups per Compound", 0));
        compoundPPMWindow->setSuffix(QApplication::translate("PeakDetectionDialog", " ppm", 0));
        eicMaxGroups->setSuffix(QApplication::translate("PeakDetectionDialog", " best", 0));
        label_11->setText(QApplication::translate("PeakDetectionDialog", "EIC Extraction Window  +/- PPM ", 0));
        compoundRTWindow->setPrefix(QString());
        compoundRTWindow->setSuffix(QApplication::translate("PeakDetectionDialog", " min", 0));
        matchRt->setText(QApplication::translate("PeakDetectionDialog", "Match Retention Time (+/-)", 0));
        matchFragmentatioOptions->setTitle(QApplication::translate("PeakDetectionDialog", "Match Fragmenation", 0));
        minFragMatchScore->setPrefix(QString());
        minFragMatchScore->setSuffix(QString());
        label_21->setText(QApplication::translate("PeakDetectionDialog", "Precursor Match Tollerance", 0));
        label_4->setText(QApplication::translate("PeakDetectionDialog", "Fragment Peak Matching Tollerance", 0));
        label_22->setText(QApplication::translate("PeakDetectionDialog", "Minumum Match Score", 0));
        reportIsotopesOptions->setTitle(QApplication::translate("PeakDetectionDialog", "Report Isotopic Peaks", 0));
        changeIsotopeOptions->setText(QApplication::translate("PeakDetectionDialog", "Isotope Detection Options", 0));
        tabwidget->setTabText(tabwidget->indexOf(featureSelectionTab), QApplication::translate("PeakDetectionDialog", "Feature Detection Selection", 0));
        peakScoringOptions->setTitle(QApplication::translate("PeakDetectionDialog", "Group Filtering", 0));
        label_28->setText(QApplication::translate("PeakDetectionDialog", "At least", 0));
        minGoodGroupCount->setSuffix(QApplication::translate("PeakDetectionDialog", " peaks", 0));
        qualityQuantileStatus->setText(QApplication::translate("PeakDetectionDialog", "% peaks above minimum quality", 0));
        label_8->setText(QApplication::translate("PeakDetectionDialog", "At least", 0));
        minNoNoiseObs->setSuffix(QApplication::translate("PeakDetectionDialog", " scans", 0));
        label_10->setText(QApplication::translate("PeakDetectionDialog", "Minimum Peak Intensity", 0));
        peakQuantitation->clear();
        peakQuantitation->insertItems(0, QStringList()
         << QApplication::translate("PeakDetectionDialog", "AreaTop", 0)
         << QApplication::translate("PeakDetectionDialog", "Area", 0)
         << QApplication::translate("PeakDetectionDialog", "Height", 0)
         << QApplication::translate("PeakDetectionDialog", "AreaNotCorrected", 0)
         << QApplication::translate("PeakDetectionDialog", "AreaTopNotCorrected", 0)
        );
        label_27->setText(QString());
        minGroupIntensity->setSuffix(QString());
        label_6->setText(QApplication::translate("PeakDetectionDialog", "Min. Signal/BaseLine Ratio", 0));
        label_29->setText(QApplication::translate("PeakDetectionDialog", "At least", 0));
        blankQuantileStatus->setText(QApplication::translate("PeakDetectionDialog", "% peaks above minimum signal/blank ratio", 0));
        baselineQuantileStatus->setText(QApplication::translate("PeakDetectionDialog", "% peaks above minimum signal/baseline ratio", 0));
        label_30->setText(QApplication::translate("PeakDetectionDialog", "At least", 0));
        labelMinQuality->setText(QApplication::translate("PeakDetectionDialog", "Minimum Quality", 0));
        label_12->setText(QApplication::translate("PeakDetectionDialog", "Minimum Peak Width", 0));
        intensityQuantileStatus->setText(QApplication::translate("PeakDetectionDialog", "% peaks above minimum intensity ", 0));
        label_100->setText(QApplication::translate("PeakDetectionDialog", "Min. Good Peak / Group", 0));
        label_9->setText(QApplication::translate("PeakDetectionDialog", "Minimum Signal/Blank Ratio", 0));
        label_5->setText(QApplication::translate("PeakDetectionDialog", "Peak Classifier Model File", 0));
        classificationModelFilename->setText(QString());
        loadModelButton->setText(QApplication::translate("PeakDetectionDialog", "Load Model", 0));
        tabwidget->setTabText(tabwidget->indexOf(groupFilteringTab), QApplication::translate("PeakDetectionDialog", "Group Filtering", 0));
        methodSummaryOptions->setTitle(QApplication::translate("PeakDetectionDialog", "Current Peak Dection Method Summary", 0));
        saveMethodButton->setText(QApplication::translate("PeakDetectionDialog", "Save Method", 0));
        loadMethodButton->setText(QApplication::translate("PeakDetectionDialog", "Load Method", 0));
        groupBox_3->setTitle(QApplication::translate("PeakDetectionDialog", "Output Detected Peaks to CSV File", 0));
        setOutputDirButton->setText(QApplication::translate("PeakDetectionDialog", "Set Directory", 0));
        label_2->setText(QApplication::translate("PeakDetectionDialog", "Output Directory", 0));
        groupBox->setTitle(QApplication::translate("PeakDetectionDialog", "Output Detected Peak to a Table", 0));
        outputTableComboBox->clear();
        outputTableComboBox->insertItems(0, QStringList()
         << QApplication::translate("PeakDetectionDialog", "Create New Peak Table", 0)
         << QApplication::translate("PeakDetectionDialog", "Bookmarked Peaks Table", 0)
        );
        label_23->setText(QApplication::translate("PeakDetectionDialog", "Output Table", 0));
        tabwidget->setTabText(tabwidget->indexOf(methodSummaryTab), QApplication::translate("PeakDetectionDialog", "Method Summary", 0));
        cancelButton->setText(QApplication::translate("PeakDetectionDialog", "Cancel", 0));
        computeButton->setText(QApplication::translate("PeakDetectionDialog", "Find Peaks", 0));
        statusText->setText(QApplication::translate("PeakDetectionDialog", "Status", 0));
    } // retranslateUi

};

namespace Ui {
    class PeakDetectionDialog: public Ui_PeakDetectionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PEAKDETECTIONDIALOG_H
