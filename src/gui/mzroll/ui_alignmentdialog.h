/********************************************************************************
** Form generated from reading UI file 'alignmentdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALIGNMENTDIALOG_H
#define UI_ALIGNMENTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QToolButton>

QT_BEGIN_NAMESPACE

class Ui_AlignmentDialog
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout_2;
    QLabel *label;
    QDoubleSpinBox *minGroupIntensity;
    QLabel *label_5;
    QSpinBox *minSN;
    QLabel *label_6;
    QSpinBox *minPeakWidth;
    QLabel *label_9;
    QComboBox *peakDetectionAlgo;
    QLabel *selectDatabase;
    QComboBox *selectDatabaseComboBox;
    QLabel *label_10;
    QDoubleSpinBox *minIntensity;
    QLabel *label_11;
    QDoubleSpinBox *maxIntensity;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_2;
    QPushButton *cancelButton;
    QProgressBar *progressBar;
    QPushButton *alignButton;
    QLabel *statusText;
    QToolButton *UndoAlignment;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label_2;
    QSpinBox *minGoodPeakCount;
    QLabel *label_3;
    QSpinBox *limitGroupCount;
    QLabel *label_4;
    QSpinBox *groupingWindow;
    QCheckBox *alignWrtExpectedRt;
    QLabel *label_13;
    QGroupBox *groupBox_3;
    QFormLayout *formLayout_3;
    QLabel *label_12;
    QComboBox *alignAlgo;
    QLabel *label_7;
    QSpinBox *maxItterations;
    QLabel *label_8;
    QSpinBox *polynomialDegree;
    QLabel *labelScoreObi;
    QComboBox *scoreObi;
    QLabel *labelResponseObiWarp;
    QDoubleSpinBox *responseObiWarp;
    QLabel *labelBinSizeObiWarp;
    QDoubleSpinBox *binSizeObiWarp;
    QLabel *labelGapInit;
    QDoubleSpinBox *gapInit;
    QLabel *labelGapExtend;
    QDoubleSpinBox *gapExtend;
    QLabel *labelFactorDiag;
    QDoubleSpinBox *factorDiag;
    QLabel *labelFactorGap;
    QDoubleSpinBox *factorGap;
    QLabel *labelNoStdNormal;
    QCheckBox *noStdNormal;
    QLabel *labelLocal;
    QCheckBox *local;
    QLabel *labelInitPenalty;
    QDoubleSpinBox *initPenalty;
    QLabel *labelRestoreDefaultObiWarpParams;
    QPushButton *restoreDefaultObiWarpParams;
    QLabel *labelShowAdvanceParams;
    QCheckBox *showAdvanceParams;

    void setupUi(QDialog *AlignmentDialog)
    {
        if (AlignmentDialog->objectName().isEmpty())
            AlignmentDialog->setObjectName(QStringLiteral("AlignmentDialog"));
        AlignmentDialog->resize(520, 1039);
        gridLayout = new QGridLayout(AlignmentDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        groupBox_2 = new QGroupBox(AlignmentDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy);
        formLayout_2 = new QFormLayout(groupBox_2);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label = new QLabel(groupBox_2);
        label->setObjectName(QStringLiteral("label"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label);

        minGroupIntensity = new QDoubleSpinBox(groupBox_2);
        minGroupIntensity->setObjectName(QStringLiteral("minGroupIntensity"));
        minGroupIntensity->setMinimum(0);
        minGroupIntensity->setMaximum(1e+09);
        minGroupIntensity->setSingleStep(1000);
        minGroupIntensity->setValue(5000);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, minGroupIntensity);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QStringLiteral("label_5"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_5);

        minSN = new QSpinBox(groupBox_2);
        minSN->setObjectName(QStringLiteral("minSN"));
        minSN->setMaximum(999999999);
        minSN->setSingleStep(1);
        minSN->setValue(5);

        formLayout_2->setWidget(3, QFormLayout::FieldRole, minSN);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_6);

        minPeakWidth = new QSpinBox(groupBox_2);
        minPeakWidth->setObjectName(QStringLiteral("minPeakWidth"));
        minPeakWidth->setMaximum(999999999);
        minPeakWidth->setValue(5);

        formLayout_2->setWidget(4, QFormLayout::FieldRole, minPeakWidth);

        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QStringLiteral("label_9"));

        formLayout_2->setWidget(5, QFormLayout::LabelRole, label_9);

        peakDetectionAlgo = new QComboBox(groupBox_2);
        peakDetectionAlgo->setObjectName(QStringLiteral("peakDetectionAlgo"));

        formLayout_2->setWidget(5, QFormLayout::FieldRole, peakDetectionAlgo);

        selectDatabase = new QLabel(groupBox_2);
        selectDatabase->setObjectName(QStringLiteral("selectDatabase"));

        formLayout_2->setWidget(6, QFormLayout::LabelRole, selectDatabase);

        selectDatabaseComboBox = new QComboBox(groupBox_2);
        selectDatabaseComboBox->setObjectName(QStringLiteral("selectDatabaseComboBox"));

        formLayout_2->setWidget(6, QFormLayout::FieldRole, selectDatabaseComboBox);

        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName(QStringLiteral("label_10"));

        formLayout_2->setWidget(7, QFormLayout::LabelRole, label_10);

        minIntensity = new QDoubleSpinBox(groupBox_2);
        minIntensity->setObjectName(QStringLiteral("minIntensity"));
        minIntensity->setMaximum(1e+10);
        minIntensity->setValue(50000);

        formLayout_2->setWidget(7, QFormLayout::FieldRole, minIntensity);

        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QStringLiteral("label_11"));

        formLayout_2->setWidget(8, QFormLayout::LabelRole, label_11);

        maxIntensity = new QDoubleSpinBox(groupBox_2);
        maxIntensity->setObjectName(QStringLiteral("maxIntensity"));
        maxIntensity->setMaximum(1e+10);
        maxIntensity->setValue(1e+10);

        formLayout_2->setWidget(8, QFormLayout::FieldRole, maxIntensity);


        gridLayout->addWidget(groupBox_2, 1, 1, 1, 1);

        groupBox_4 = new QGroupBox(AlignmentDialog);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout_2 = new QGridLayout(groupBox_4);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        cancelButton = new QPushButton(groupBox_4);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        gridLayout_2->addWidget(cancelButton, 3, 2, 1, 1);

        progressBar = new QProgressBar(groupBox_4);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(0);

        gridLayout_2->addWidget(progressBar, 3, 0, 1, 1);

        alignButton = new QPushButton(groupBox_4);
        alignButton->setObjectName(QStringLiteral("alignButton"));

        gridLayout_2->addWidget(alignButton, 3, 1, 1, 1);

        statusText = new QLabel(groupBox_4);
        statusText->setObjectName(QStringLiteral("statusText"));

        gridLayout_2->addWidget(statusText, 1, 0, 1, 1);

        UndoAlignment = new QToolButton(groupBox_4);
        UndoAlignment->setObjectName(QStringLiteral("UndoAlignment"));

        gridLayout_2->addWidget(UndoAlignment, 1, 2, 1, 1);


        gridLayout->addWidget(groupBox_4, 5, 1, 1, 1);

        groupBox = new QGroupBox(AlignmentDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        formLayout = new QFormLayout(groupBox);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_2);

        minGoodPeakCount = new QSpinBox(groupBox);
        minGoodPeakCount->setObjectName(QStringLiteral("minGoodPeakCount"));
        minGoodPeakCount->setMaximum(1000000000);
        minGoodPeakCount->setValue(2);

        formLayout->setWidget(0, QFormLayout::FieldRole, minGoodPeakCount);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        limitGroupCount = new QSpinBox(groupBox);
        limitGroupCount->setObjectName(QStringLiteral("limitGroupCount"));
        limitGroupCount->setMaximum(1000000000);
        limitGroupCount->setValue(1000);

        formLayout->setWidget(1, QFormLayout::FieldRole, limitGroupCount);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_4);

        groupingWindow = new QSpinBox(groupBox);
        groupingWindow->setObjectName(QStringLiteral("groupingWindow"));
        groupingWindow->setMinimum(1);
        groupingWindow->setMaximum(999999999);
        groupingWindow->setValue(20);

        formLayout->setWidget(2, QFormLayout::FieldRole, groupingWindow);

        alignWrtExpectedRt = new QCheckBox(groupBox);
        alignWrtExpectedRt->setObjectName(QStringLiteral("alignWrtExpectedRt"));

        formLayout->setWidget(3, QFormLayout::FieldRole, alignWrtExpectedRt);

        label_13 = new QLabel(groupBox);
        label_13->setObjectName(QStringLiteral("label_13"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_13);


        gridLayout->addWidget(groupBox, 0, 1, 1, 1);

        groupBox_3 = new QGroupBox(AlignmentDialog);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        formLayout_3 = new QFormLayout(groupBox_3);
        formLayout_3->setObjectName(QStringLiteral("formLayout_3"));
        label_12 = new QLabel(groupBox_3);
        label_12->setObjectName(QStringLiteral("label_12"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_12);

        alignAlgo = new QComboBox(groupBox_3);
        alignAlgo->setObjectName(QStringLiteral("alignAlgo"));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, alignAlgo);

        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QStringLiteral("label_7"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, label_7);

        maxItterations = new QSpinBox(groupBox_3);
        maxItterations->setObjectName(QStringLiteral("maxItterations"));
        maxItterations->setMinimum(1);
        maxItterations->setValue(10);

        formLayout_3->setWidget(1, QFormLayout::FieldRole, maxItterations);

        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QStringLiteral("label_8"));

        formLayout_3->setWidget(3, QFormLayout::LabelRole, label_8);

        polynomialDegree = new QSpinBox(groupBox_3);
        polynomialDegree->setObjectName(QStringLiteral("polynomialDegree"));
        polynomialDegree->setMinimum(1);
        polynomialDegree->setMaximum(5);
        polynomialDegree->setValue(5);

        formLayout_3->setWidget(3, QFormLayout::FieldRole, polynomialDegree);

        labelScoreObi = new QLabel(groupBox_3);
        labelScoreObi->setObjectName(QStringLiteral("labelScoreObi"));

        formLayout_3->setWidget(5, QFormLayout::LabelRole, labelScoreObi);

        scoreObi = new QComboBox(groupBox_3);
        scoreObi->setObjectName(QStringLiteral("scoreObi"));

        formLayout_3->setWidget(5, QFormLayout::FieldRole, scoreObi);

        labelResponseObiWarp = new QLabel(groupBox_3);
        labelResponseObiWarp->setObjectName(QStringLiteral("labelResponseObiWarp"));

        formLayout_3->setWidget(6, QFormLayout::LabelRole, labelResponseObiWarp);

        responseObiWarp = new QDoubleSpinBox(groupBox_3);
        responseObiWarp->setObjectName(QStringLiteral("responseObiWarp"));
        responseObiWarp->setMaximum(100);
        responseObiWarp->setSingleStep(1);
        responseObiWarp->setValue(20);

        formLayout_3->setWidget(6, QFormLayout::FieldRole, responseObiWarp);

        labelBinSizeObiWarp = new QLabel(groupBox_3);
        labelBinSizeObiWarp->setObjectName(QStringLiteral("labelBinSizeObiWarp"));

        formLayout_3->setWidget(7, QFormLayout::LabelRole, labelBinSizeObiWarp);

        binSizeObiWarp = new QDoubleSpinBox(groupBox_3);
        binSizeObiWarp->setObjectName(QStringLiteral("binSizeObiWarp"));
        binSizeObiWarp->setMaximum(2);
        binSizeObiWarp->setSingleStep(0.1);
        binSizeObiWarp->setValue(0.6);

        formLayout_3->setWidget(7, QFormLayout::FieldRole, binSizeObiWarp);

        labelGapInit = new QLabel(groupBox_3);
        labelGapInit->setObjectName(QStringLiteral("labelGapInit"));

        formLayout_3->setWidget(8, QFormLayout::LabelRole, labelGapInit);

        gapInit = new QDoubleSpinBox(groupBox_3);
        gapInit->setObjectName(QStringLiteral("gapInit"));
        gapInit->setMaximum(97.99);
        gapInit->setSingleStep(0.5);
        gapInit->setValue(0.2);

        formLayout_3->setWidget(8, QFormLayout::FieldRole, gapInit);

        labelGapExtend = new QLabel(groupBox_3);
        labelGapExtend->setObjectName(QStringLiteral("labelGapExtend"));

        formLayout_3->setWidget(9, QFormLayout::LabelRole, labelGapExtend);

        gapExtend = new QDoubleSpinBox(groupBox_3);
        gapExtend->setObjectName(QStringLiteral("gapExtend"));
        gapExtend->setSingleStep(0.5);
        gapExtend->setValue(3.4);

        formLayout_3->setWidget(9, QFormLayout::FieldRole, gapExtend);

        labelFactorDiag = new QLabel(groupBox_3);
        labelFactorDiag->setObjectName(QStringLiteral("labelFactorDiag"));

        formLayout_3->setWidget(10, QFormLayout::LabelRole, labelFactorDiag);

        factorDiag = new QDoubleSpinBox(groupBox_3);
        factorDiag->setObjectName(QStringLiteral("factorDiag"));
        factorDiag->setSingleStep(0.5);
        factorDiag->setValue(2);

        formLayout_3->setWidget(10, QFormLayout::FieldRole, factorDiag);

        labelFactorGap = new QLabel(groupBox_3);
        labelFactorGap->setObjectName(QStringLiteral("labelFactorGap"));

        formLayout_3->setWidget(11, QFormLayout::LabelRole, labelFactorGap);

        factorGap = new QDoubleSpinBox(groupBox_3);
        factorGap->setObjectName(QStringLiteral("factorGap"));
        factorGap->setSingleStep(0.5);
        factorGap->setValue(1);

        formLayout_3->setWidget(11, QFormLayout::FieldRole, factorGap);

        labelNoStdNormal = new QLabel(groupBox_3);
        labelNoStdNormal->setObjectName(QStringLiteral("labelNoStdNormal"));

        formLayout_3->setWidget(12, QFormLayout::LabelRole, labelNoStdNormal);

        noStdNormal = new QCheckBox(groupBox_3);
        noStdNormal->setObjectName(QStringLiteral("noStdNormal"));

        formLayout_3->setWidget(12, QFormLayout::FieldRole, noStdNormal);

        labelLocal = new QLabel(groupBox_3);
        labelLocal->setObjectName(QStringLiteral("labelLocal"));

        formLayout_3->setWidget(13, QFormLayout::LabelRole, labelLocal);

        local = new QCheckBox(groupBox_3);
        local->setObjectName(QStringLiteral("local"));

        formLayout_3->setWidget(13, QFormLayout::FieldRole, local);

        labelInitPenalty = new QLabel(groupBox_3);
        labelInitPenalty->setObjectName(QStringLiteral("labelInitPenalty"));

        formLayout_3->setWidget(14, QFormLayout::LabelRole, labelInitPenalty);

        initPenalty = new QDoubleSpinBox(groupBox_3);
        initPenalty->setObjectName(QStringLiteral("initPenalty"));

        formLayout_3->setWidget(14, QFormLayout::FieldRole, initPenalty);

        labelRestoreDefaultObiWarpParams = new QLabel(groupBox_3);
        labelRestoreDefaultObiWarpParams->setObjectName(QStringLiteral("labelRestoreDefaultObiWarpParams"));

        formLayout_3->setWidget(15, QFormLayout::LabelRole, labelRestoreDefaultObiWarpParams);

        restoreDefaultObiWarpParams = new QPushButton(groupBox_3);
        restoreDefaultObiWarpParams->setObjectName(QStringLiteral("restoreDefaultObiWarpParams"));

        formLayout_3->setWidget(15, QFormLayout::FieldRole, restoreDefaultObiWarpParams);

        labelShowAdvanceParams = new QLabel(groupBox_3);
        labelShowAdvanceParams->setObjectName(QStringLiteral("labelShowAdvanceParams"));

        formLayout_3->setWidget(4, QFormLayout::LabelRole, labelShowAdvanceParams);

        showAdvanceParams = new QCheckBox(groupBox_3);
        showAdvanceParams->setObjectName(QStringLiteral("showAdvanceParams"));

        formLayout_3->setWidget(4, QFormLayout::FieldRole, showAdvanceParams);


        gridLayout->addWidget(groupBox_3, 2, 1, 3, 1);

        QWidget::setTabOrder(minGoodPeakCount, limitGroupCount);
        QWidget::setTabOrder(limitGroupCount, groupingWindow);
        QWidget::setTabOrder(groupingWindow, minGroupIntensity);
        QWidget::setTabOrder(minGroupIntensity, minSN);
        QWidget::setTabOrder(minSN, minPeakWidth);
        QWidget::setTabOrder(minPeakWidth, peakDetectionAlgo);
        QWidget::setTabOrder(peakDetectionAlgo, selectDatabaseComboBox);
        QWidget::setTabOrder(selectDatabaseComboBox, minIntensity);
        QWidget::setTabOrder(minIntensity, maxIntensity);
        QWidget::setTabOrder(maxIntensity, alignAlgo);
        QWidget::setTabOrder(alignAlgo, maxItterations);
        QWidget::setTabOrder(maxItterations, polynomialDegree);
        QWidget::setTabOrder(polynomialDegree, UndoAlignment);
        QWidget::setTabOrder(UndoAlignment, alignButton);
        QWidget::setTabOrder(alignButton, cancelButton);

        retranslateUi(AlignmentDialog);

        QMetaObject::connectSlotsByName(AlignmentDialog);
    } // setupUi

    void retranslateUi(QDialog *AlignmentDialog)
    {
        AlignmentDialog->setWindowTitle(QApplication::translate("AlignmentDialog", "Alignment Options", 0));
        groupBox_2->setTitle(QApplication::translate("AlignmentDialog", "Peak Selection", 0));
        label->setText(QApplication::translate("AlignmentDialog", "Minumum Peak Intensity", 0));
        label_5->setText(QApplication::translate("AlignmentDialog", "Minimum Peak S/N Ratio", 0));
        label_6->setText(QApplication::translate("AlignmentDialog", "Minimum Peak Width", 0));
        minPeakWidth->setSuffix(QApplication::translate("AlignmentDialog", " scans", 0));
        label_9->setText(QApplication::translate("AlignmentDialog", "Peak Detection Algorithm", 0));
        peakDetectionAlgo->clear();
        peakDetectionAlgo->insertItems(0, QStringList()
         << QApplication::translate("AlignmentDialog", "Compound Database Search", 0)
         << QApplication::translate("AlignmentDialog", "Automated Peak Detection", 0)
        );
        selectDatabase->setText(QApplication::translate("AlignmentDialog", "Select Database", 0));
        label_10->setText(QApplication::translate("AlignmentDialog", "Min Intensity for Scans", 0));
        label_11->setText(QApplication::translate("AlignmentDialog", "Max Intensity for Scans", 0));
        groupBox_4->setTitle(QString());
        cancelButton->setText(QApplication::translate("AlignmentDialog", "Cancel", 0));
        alignButton->setText(QApplication::translate("AlignmentDialog", "Align", 0));
        statusText->setText(QApplication::translate("AlignmentDialog", "StatusText", 0));
        UndoAlignment->setText(QApplication::translate("AlignmentDialog", "Undo Aligment", 0));
        groupBox->setTitle(QApplication::translate("AlignmentDialog", "Group Selection Criteria for use in Alignment", 0));
        label_2->setText(QApplication::translate("AlignmentDialog", "Group must contain at least  [X] good peaks", 0));
        minGoodPeakCount->setSuffix(QApplication::translate("AlignmentDialog", " peaks", 0));
        minGoodPeakCount->setPrefix(QString());
        label_3->setText(QApplication::translate("AlignmentDialog", "Limit total number of groups in alignment to", 0));
        limitGroupCount->setSuffix(QApplication::translate("AlignmentDialog", " groups", 0));
        label_4->setText(QApplication::translate("AlignmentDialog", "Peak Grouping Window", 0));
        groupingWindow->setSuffix(QApplication::translate("AlignmentDialog", " scans", 0));
        alignWrtExpectedRt->setText(QString());
        label_13->setText(QApplication::translate("AlignmentDialog", "Alignment w.r.t Expected Rt", 0));
        groupBox_3->setTitle(QApplication::translate("AlignmentDialog", "Alignment Algorithm", 0));
        label_12->setText(QApplication::translate("AlignmentDialog", "Alignment Algorithm", 0));
        alignAlgo->clear();
        alignAlgo->insertItems(0, QStringList()
         << QApplication::translate("AlignmentDialog", "Poly Fit", 0)
         << QApplication::translate("AlignmentDialog", "Loess Fit", 0)
         << QApplication::translate("AlignmentDialog", "OBI-Warp (beta)", 0)
        );
        alignAlgo->setCurrentText(QApplication::translate("AlignmentDialog", "Poly Fit", 0));
        label_7->setText(QApplication::translate("AlignmentDialog", "Maximum Number of Itterations", 0));
        label_8->setText(QApplication::translate("AlignmentDialog", "Polynomial Degree", 0));
        labelScoreObi->setText(QApplication::translate("AlignmentDialog", "Score", 0));
        scoreObi->clear();
        scoreObi->insertItems(0, QStringList()
         << QApplication::translate("AlignmentDialog", "cor", 0)
         << QApplication::translate("AlignmentDialog", "cov", 0)
         << QApplication::translate("AlignmentDialog", "prd", 0)
         << QApplication::translate("AlignmentDialog", "euc", 0)
        );
        labelResponseObiWarp->setText(QApplication::translate("AlignmentDialog", "Response", 0));
        labelBinSizeObiWarp->setText(QApplication::translate("AlignmentDialog", "Bin Size", 0));
        labelGapInit->setText(QApplication::translate("AlignmentDialog", "Gap-init", 0));
        labelGapExtend->setText(QApplication::translate("AlignmentDialog", "Gap-extend", 0));
        labelFactorDiag->setText(QApplication::translate("AlignmentDialog", "Factor-diag", 0));
        labelFactorGap->setText(QApplication::translate("AlignmentDialog", "Factor-gap", 0));
        labelNoStdNormal->setText(QApplication::translate("AlignmentDialog", "No std normal", 0));
        noStdNormal->setText(QString());
        labelLocal->setText(QApplication::translate("AlignmentDialog", "Local", 0));
        local->setText(QString());
        labelInitPenalty->setText(QApplication::translate("AlignmentDialog", "Init. penalty", 0));
        labelRestoreDefaultObiWarpParams->setText(QApplication::translate("AlignmentDialog", "Restore Default Values", 0));
        restoreDefaultObiWarpParams->setText(QApplication::translate("AlignmentDialog", "OK", 0));
        labelShowAdvanceParams->setText(QApplication::translate("AlignmentDialog", "Show Advance Params", 0));
        showAdvanceParams->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class AlignmentDialog: public Ui_AlignmentDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALIGNMENTDIALOG_H
