/********************************************************************************
** Form generated from reading UI file 'peakdetectiondialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PEAKDETECTIONDIALOG_H
#define UI_PEAKDETECTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_PeakDetectionDialog
{
public:
    QGridLayout *gridLayout_7;
    QGroupBox *dbOptions;
    QGridLayout *gridLayout_2;
    QLabel *label_3;
    QComboBox *compoundDatabase;
    QCheckBox *matchRt;
    QCheckBox *reportIsotopes;
    QDoubleSpinBox *compoundPPMWindow;
    QLabel *label_11;
    QLabel *label_4;
    QDoubleSpinBox *compoundRTWindow;
    QLabel *label_16;
    QSpinBox *eicMaxGroups;
    QGroupBox *featureOptions;
    QGridLayout *gridLayout;
    QLabel *label;
    QDoubleSpinBox *rtStep;
    QDoubleSpinBox *ppmStep;
    QLabel *label_7;
    QHBoxLayout *peakScoringOptions;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_3;
    QLabel *label_9;
    QLabel *label_5;
    QSpinBox *minGoodGroupCount;
    QLabel *label_6;
    QSpinBox *sigBaselineRatio;
    QDoubleSpinBox *sigBlankRatio;
    QDoubleSpinBox *minGroupIntensity;
    QLabel *label_10;
    QSpinBox *minNoNoiseObs;
    QLabel *label_12;
    QPushButton *loadModelButton;
    QLineEdit *classificationModelFilename;
    QLabel *label_100;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *outputDirName;
    QPushButton *setOutputDirButton;
    QFrame *frame_2;
    QGridLayout *gridLayout_4;
    QPushButton *computeButton;
    QPushButton *cancelButton;
    QProgressBar *progressBar;
    QLabel *statusText;
    QGroupBox *eicOptions;
    QGridLayout *gridLayout_5;
    QLabel *label_13;
    QLabel *label_14;
    QDoubleSpinBox *grouping_maxRtDiff;
    QSpinBox *eic_smoothingWindow;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_6;
    QLabel *label_8;
    QSpinBox *baseline_smoothing;
    QSpinBox *baseline_quantile;
    QLabel *label_15;

    void setupUi(QDialog *PeakDetectionDialog)
    {
        if (PeakDetectionDialog->objectName().isEmpty())
            PeakDetectionDialog->setObjectName(QString::fromUtf8("PeakDetectionDialog"));
        PeakDetectionDialog->resize(577, 803);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PeakDetectionDialog->sizePolicy().hasHeightForWidth());
        PeakDetectionDialog->setSizePolicy(sizePolicy);
        gridLayout_7 = new QGridLayout(PeakDetectionDialog);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        dbOptions = new QGroupBox(PeakDetectionDialog);
        dbOptions->setObjectName(QString::fromUtf8("dbOptions"));
        gridLayout_2 = new QGridLayout(dbOptions);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_3 = new QLabel(dbOptions);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 0, 0, 1, 1);

        compoundDatabase = new QComboBox(dbOptions);
        compoundDatabase->setObjectName(QString::fromUtf8("compoundDatabase"));

        gridLayout_2->addWidget(compoundDatabase, 0, 2, 1, 1);

        matchRt = new QCheckBox(dbOptions);
        matchRt->setObjectName(QString::fromUtf8("matchRt"));
        matchRt->setChecked(false);

        gridLayout_2->addWidget(matchRt, 1, 0, 1, 1);

        reportIsotopes = new QCheckBox(dbOptions);
        reportIsotopes->setObjectName(QString::fromUtf8("reportIsotopes"));
        reportIsotopes->setChecked(false);

        gridLayout_2->addWidget(reportIsotopes, 1, 2, 1, 1);

        compoundPPMWindow = new QDoubleSpinBox(dbOptions);
        compoundPPMWindow->setObjectName(QString::fromUtf8("compoundPPMWindow"));
        compoundPPMWindow->setMaximum(1e+06);
        compoundPPMWindow->setValue(20);

        gridLayout_2->addWidget(compoundPPMWindow, 2, 2, 1, 1);

        label_11 = new QLabel(dbOptions);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout_2->addWidget(label_11, 2, 0, 1, 1);

        label_4 = new QLabel(dbOptions);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 3, 0, 1, 1);

        compoundRTWindow = new QDoubleSpinBox(dbOptions);
        compoundRTWindow->setObjectName(QString::fromUtf8("compoundRTWindow"));
        compoundRTWindow->setMaximum(1e+09);
        compoundRTWindow->setSingleStep(0.5);
        compoundRTWindow->setValue(2);

        gridLayout_2->addWidget(compoundRTWindow, 3, 2, 1, 1);

        label_16 = new QLabel(dbOptions);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout_2->addWidget(label_16, 4, 0, 1, 1);

        eicMaxGroups = new QSpinBox(dbOptions);
        eicMaxGroups->setObjectName(QString::fromUtf8("eicMaxGroups"));
        eicMaxGroups->setMinimum(1);
        eicMaxGroups->setMaximum(999999999);
        eicMaxGroups->setValue(10);

        gridLayout_2->addWidget(eicMaxGroups, 4, 2, 1, 1);


        gridLayout_7->addWidget(dbOptions, 0, 0, 1, 1);

        featureOptions = new QGroupBox(PeakDetectionDialog);
        featureOptions->setObjectName(QString::fromUtf8("featureOptions"));
        featureOptions->setEnabled(true);
        featureOptions->setAutoFillBackground(false);
        featureOptions->setFlat(false);
        featureOptions->setCheckable(false);
        gridLayout = new QGridLayout(featureOptions);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(featureOptions);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 4, 0, 1, 1);

        rtStep = new QDoubleSpinBox(featureOptions);
        rtStep->setObjectName(QString::fromUtf8("rtStep"));
        rtStep->setDecimals(0);
        rtStep->setMinimum(1);
        rtStep->setMaximum(1e+06);
        rtStep->setSingleStep(5);
        rtStep->setValue(10);

        gridLayout->addWidget(rtStep, 4, 1, 1, 1);

        ppmStep = new QDoubleSpinBox(featureOptions);
        ppmStep->setObjectName(QString::fromUtf8("ppmStep"));
        ppmStep->setDecimals(3);
        ppmStep->setMaximum(1e+09);
        ppmStep->setSingleStep(1);
        ppmStep->setValue(20);

        gridLayout->addWidget(ppmStep, 0, 1, 1, 1);

        label_7 = new QLabel(featureOptions);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 0, 0, 1, 1);


        gridLayout_7->addWidget(featureOptions, 1, 0, 1, 1);

        peakScoringOptions = new QHBoxLayout();
        peakScoringOptions->setObjectName(QString::fromUtf8("peakScoringOptions"));
        groupBox_5 = new QGroupBox(PeakDetectionDialog);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        gridLayout_3 = new QGridLayout(groupBox_5);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_9 = new QLabel(groupBox_5);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout_3->addWidget(label_9, 3, 2, 1, 1);

        label_5 = new QLabel(groupBox_5);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_3->addWidget(label_5, 3, 0, 1, 1);

        minGoodGroupCount = new QSpinBox(groupBox_5);
        minGoodGroupCount->setObjectName(QString::fromUtf8("minGoodGroupCount"));
        minGoodGroupCount->setMaximum(100000000);
        minGoodGroupCount->setValue(1);

        gridLayout_3->addWidget(minGoodGroupCount, 3, 1, 1, 1);

        label_6 = new QLabel(groupBox_5);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_3->addWidget(label_6, 4, 0, 1, 1);

        sigBaselineRatio = new QSpinBox(groupBox_5);
        sigBaselineRatio->setObjectName(QString::fromUtf8("sigBaselineRatio"));
        sigBaselineRatio->setMaximum(10000000);
        sigBaselineRatio->setValue(2);

        gridLayout_3->addWidget(sigBaselineRatio, 4, 1, 1, 1);

        sigBlankRatio = new QDoubleSpinBox(groupBox_5);
        sigBlankRatio->setObjectName(QString::fromUtf8("sigBlankRatio"));
        sigBlankRatio->setMaximum(1e+07);
        sigBlankRatio->setValue(2);

        gridLayout_3->addWidget(sigBlankRatio, 3, 3, 1, 1);

        minGroupIntensity = new QDoubleSpinBox(groupBox_5);
        minGroupIntensity->setObjectName(QString::fromUtf8("minGroupIntensity"));
        minGroupIntensity->setMaximum(1e+09);
        minGroupIntensity->setValue(5000);

        gridLayout_3->addWidget(minGroupIntensity, 4, 3, 1, 1);

        label_10 = new QLabel(groupBox_5);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout_3->addWidget(label_10, 4, 2, 1, 1);

        minNoNoiseObs = new QSpinBox(groupBox_5);
        minNoNoiseObs->setObjectName(QString::fromUtf8("minNoNoiseObs"));
        minNoNoiseObs->setMinimum(1);
        minNoNoiseObs->setMaximum(1000000000);
        minNoNoiseObs->setValue(3);

        gridLayout_3->addWidget(minNoNoiseObs, 5, 1, 1, 1);

        label_12 = new QLabel(groupBox_5);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout_3->addWidget(label_12, 5, 0, 1, 1);

        loadModelButton = new QPushButton(groupBox_5);
        loadModelButton->setObjectName(QString::fromUtf8("loadModelButton"));

        gridLayout_3->addWidget(loadModelButton, 1, 3, 1, 1);

        classificationModelFilename = new QLineEdit(groupBox_5);
        classificationModelFilename->setObjectName(QString::fromUtf8("classificationModelFilename"));

        gridLayout_3->addWidget(classificationModelFilename, 1, 2, 1, 1);

        label_100 = new QLabel(groupBox_5);
        label_100->setObjectName(QString::fromUtf8("label_100"));

        gridLayout_3->addWidget(label_100, 1, 1, 1, 1);


        peakScoringOptions->addWidget(groupBox_5);


        gridLayout_7->addLayout(peakScoringOptions, 4, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(PeakDetectionDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        outputDirName = new QLineEdit(PeakDetectionDialog);
        outputDirName->setObjectName(QString::fromUtf8("outputDirName"));

        horizontalLayout_2->addWidget(outputDirName);

        setOutputDirButton = new QPushButton(PeakDetectionDialog);
        setOutputDirButton->setObjectName(QString::fromUtf8("setOutputDirButton"));

        horizontalLayout_2->addWidget(setOutputDirButton);


        gridLayout_7->addLayout(horizontalLayout_2, 5, 0, 1, 1);

        frame_2 = new QFrame(PeakDetectionDialog);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        gridLayout_4 = new QGridLayout(frame_2);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        computeButton = new QPushButton(frame_2);
        computeButton->setObjectName(QString::fromUtf8("computeButton"));

        gridLayout_4->addWidget(computeButton, 1, 2, 1, 1);

        cancelButton = new QPushButton(frame_2);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        gridLayout_4->addWidget(cancelButton, 1, 3, 1, 1);

        progressBar = new QProgressBar(frame_2);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(0);

        gridLayout_4->addWidget(progressBar, 1, 1, 1, 1);

        statusText = new QLabel(frame_2);
        statusText->setObjectName(QString::fromUtf8("statusText"));

        gridLayout_4->addWidget(statusText, 0, 1, 1, 1);


        gridLayout_7->addWidget(frame_2, 6, 0, 1, 1);

        eicOptions = new QGroupBox(PeakDetectionDialog);
        eicOptions->setObjectName(QString::fromUtf8("eicOptions"));
        eicOptions->setEnabled(true);
        gridLayout_5 = new QGridLayout(eicOptions);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        label_13 = new QLabel(eicOptions);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        gridLayout_5->addWidget(label_13, 0, 0, 1, 1);

        label_14 = new QLabel(eicOptions);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        gridLayout_5->addWidget(label_14, 2, 0, 1, 1);

        grouping_maxRtDiff = new QDoubleSpinBox(eicOptions);
        grouping_maxRtDiff->setObjectName(QString::fromUtf8("grouping_maxRtDiff"));
        grouping_maxRtDiff->setMaximum(1e+06);
        grouping_maxRtDiff->setSingleStep(0.1);
        grouping_maxRtDiff->setValue(0.5);

        gridLayout_5->addWidget(grouping_maxRtDiff, 2, 1, 1, 1);

        eic_smoothingWindow = new QSpinBox(eicOptions);
        eic_smoothingWindow->setObjectName(QString::fromUtf8("eic_smoothingWindow"));
        eic_smoothingWindow->setMinimum(1);
        eic_smoothingWindow->setMaximum(1000000);
        eic_smoothingWindow->setSingleStep(1);
        eic_smoothingWindow->setValue(10);

        gridLayout_5->addWidget(eic_smoothingWindow, 0, 1, 1, 1);

        groupBox = new QGroupBox(eicOptions);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_6 = new QGridLayout(groupBox);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_6->addWidget(label_8, 1, 0, 1, 1);

        baseline_smoothing = new QSpinBox(groupBox);
        baseline_smoothing->setObjectName(QString::fromUtf8("baseline_smoothing"));
        baseline_smoothing->setMaximum(1000000);
        baseline_smoothing->setValue(5);

        gridLayout_6->addWidget(baseline_smoothing, 1, 1, 1, 1);

        baseline_quantile = new QSpinBox(groupBox);
        baseline_quantile->setObjectName(QString::fromUtf8("baseline_quantile"));
        baseline_quantile->setMaximum(100);
        baseline_quantile->setValue(80);

        gridLayout_6->addWidget(baseline_quantile, 0, 1, 1, 1);

        label_15 = new QLabel(groupBox);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout_6->addWidget(label_15, 0, 0, 1, 1);


        gridLayout_5->addWidget(groupBox, 3, 0, 1, 2);


        gridLayout_7->addWidget(eicOptions, 2, 0, 1, 1);


        retranslateUi(PeakDetectionDialog);

        QMetaObject::connectSlotsByName(PeakDetectionDialog);
    } // setupUi

    void retranslateUi(QDialog *PeakDetectionDialog)
    {
        PeakDetectionDialog->setWindowTitle(QApplication::translate("PeakDetectionDialog", "Peak Detection", 0, QApplication::UnicodeUTF8));
        dbOptions->setTitle(QApplication::translate("PeakDetectionDialog", "Compound Database", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("PeakDetectionDialog", "Compound Subset", 0, QApplication::UnicodeUTF8));
        matchRt->setText(QApplication::translate("PeakDetectionDialog", "Match Retention Times", 0, QApplication::UnicodeUTF8));
        reportIsotopes->setText(QApplication::translate("PeakDetectionDialog", "Report Isotopic Peaks", 0, QApplication::UnicodeUTF8));
        compoundPPMWindow->setSuffix(QApplication::translate("PeakDetectionDialog", " ppm", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("PeakDetectionDialog", "EIC Extraction Window  +/- PPM ", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("PeakDetectionDialog", "Compound Retention Time Matching Window", 0, QApplication::UnicodeUTF8));
        compoundRTWindow->setPrefix(QString());
        compoundRTWindow->setSuffix(QApplication::translate("PeakDetectionDialog", " min", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("PeakDetectionDialog", "Limit Number of Reported Groups per Compound", 0, QApplication::UnicodeUTF8));
        eicMaxGroups->setSuffix(QApplication::translate("PeakDetectionDialog", " best", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        featureOptions->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
        featureOptions->setTitle(QApplication::translate("PeakDetectionDialog", "Feature Detection", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("PeakDetectionDialog", "Time Domain Resolution (scans)", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        rtStep->setStatusTip(QApplication::translate("PeakDetectionDialog", "mz-rt matrix binning", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        rtStep->setPrefix(QString());
        rtStep->setSuffix(QApplication::translate("PeakDetectionDialog", " scans", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("PeakDetectionDialog", "Mass Domain Resolution (ppm)", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("PeakDetectionDialog", "Peak Scoring", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("PeakDetectionDialog", "Min. Signal/Blank Ratio", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("PeakDetectionDialog", "Min. Good Peak / Group", 0, QApplication::UnicodeUTF8));
        minGoodGroupCount->setSuffix(QApplication::translate("PeakDetectionDialog", " peaks", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("PeakDetectionDialog", "Min. Signal/BaseLine Ratio", 0, QApplication::UnicodeUTF8));
        minGroupIntensity->setSuffix(QApplication::translate("PeakDetectionDialog", " ions", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("PeakDetectionDialog", "Min. Peak Intensity", 0, QApplication::UnicodeUTF8));
        minNoNoiseObs->setSuffix(QApplication::translate("PeakDetectionDialog", " scans", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("PeakDetectionDialog", "Min. Peak Width", 0, QApplication::UnicodeUTF8));
        loadModelButton->setText(QApplication::translate("PeakDetectionDialog", "Load Model", 0, QApplication::UnicodeUTF8));
        classificationModelFilename->setText(QApplication::translate("PeakDetectionDialog", "default.model", 0, QApplication::UnicodeUTF8));
        label_100->setText(QApplication::translate("PeakDetectionDialog", "Peak Classifier Model File", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("PeakDetectionDialog", "Output Directory", 0, QApplication::UnicodeUTF8));
        setOutputDirButton->setText(QApplication::translate("PeakDetectionDialog", "Set Directory", 0, QApplication::UnicodeUTF8));
        computeButton->setText(QApplication::translate("PeakDetectionDialog", "Find Peaks", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("PeakDetectionDialog", "Cancel", 0, QApplication::UnicodeUTF8));
        statusText->setText(QApplication::translate("PeakDetectionDialog", "TextLabel", 0, QApplication::UnicodeUTF8));
        eicOptions->setTitle(QApplication::translate("PeakDetectionDialog", "EIC Processing", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("PeakDetectionDialog", "EIC Smoothing", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("PeakDetectionDialog", "Peak Grouping (Max Group Rt Difference)", 0, QApplication::UnicodeUTF8));
        grouping_maxRtDiff->setSuffix(QApplication::translate("PeakDetectionDialog", " min", 0, QApplication::UnicodeUTF8));
        eic_smoothingWindow->setSuffix(QApplication::translate("PeakDetectionDialog", " scans", 0, QApplication::UnicodeUTF8));
        eic_smoothingWindow->setPrefix(QString());
        groupBox->setTitle(QApplication::translate("PeakDetectionDialog", "Baseline Calculation", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("PeakDetectionDialog", "Baseline Smooting ", 0, QApplication::UnicodeUTF8));
        baseline_smoothing->setSuffix(QApplication::translate("PeakDetectionDialog", " scans", 0, QApplication::UnicodeUTF8));
        baseline_quantile->setSuffix(QApplication::translate("PeakDetectionDialog", " %", 0, QApplication::UnicodeUTF8));
        baseline_quantile->setPrefix(QString());
        label_15->setText(QApplication::translate("PeakDetectionDialog", "Drop top x%  intensities from chromatogram", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PeakDetectionDialog: public Ui_PeakDetectionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PEAKDETECTIONDIALOG_H
