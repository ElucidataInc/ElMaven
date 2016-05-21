/********************************************************************************
** Form generated from reading UI file 'alignmentdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALIGNMENTDIALOG_H
#define UI_ALIGNMENTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QToolButton>

QT_BEGIN_NAMESPACE

class Ui_AlignmentDialog
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label_2;
    QSpinBox *minGoodPeakCount;
    QLabel *label_3;
    QSpinBox *limitGroupCount;
    QLabel *label_4;
    QSpinBox *groupingWindow;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout_2;
    QLabel *label;
    QDoubleSpinBox *minGroupIntensity;
    QLabel *label_5;
    QSpinBox *minSN;
    QLabel *label_6;
    QSpinBox *minPeakWidth;
    QGroupBox *groupBox_3;
    QFormLayout *formLayout_3;
    QLabel *label_7;
    QSpinBox *maxItterations;
    QLabel *label_8;
    QSpinBox *polynomialDegree;
    QPushButton *alignButton;
    QToolButton *UndoAlignment;

    void setupUi(QDialog *AlignmentDialog)
    {
        if (AlignmentDialog->objectName().isEmpty())
            AlignmentDialog->setObjectName(QString::fromUtf8("AlignmentDialog"));
        AlignmentDialog->resize(495, 429);
        gridLayout = new QGridLayout(AlignmentDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox = new QGroupBox(AlignmentDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        formLayout = new QFormLayout(groupBox);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_2);

        minGoodPeakCount = new QSpinBox(groupBox);
        minGoodPeakCount->setObjectName(QString::fromUtf8("minGoodPeakCount"));
        minGoodPeakCount->setMaximum(1000000000);
        minGoodPeakCount->setValue(2);

        formLayout->setWidget(0, QFormLayout::FieldRole, minGoodPeakCount);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        limitGroupCount = new QSpinBox(groupBox);
        limitGroupCount->setObjectName(QString::fromUtf8("limitGroupCount"));
        limitGroupCount->setMaximum(1000000000);
        limitGroupCount->setValue(1000);

        formLayout->setWidget(1, QFormLayout::FieldRole, limitGroupCount);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_4);

        groupingWindow = new QSpinBox(groupBox);
        groupingWindow->setObjectName(QString::fromUtf8("groupingWindow"));
        groupingWindow->setMinimum(1);
        groupingWindow->setMaximum(999999999);
        groupingWindow->setValue(20);

        formLayout->setWidget(2, QFormLayout::FieldRole, groupingWindow);


        gridLayout->addWidget(groupBox, 0, 1, 1, 1);

        groupBox_2 = new QGroupBox(AlignmentDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        formLayout_2 = new QFormLayout(groupBox_2);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label);

        minGroupIntensity = new QDoubleSpinBox(groupBox_2);
        minGroupIntensity->setObjectName(QString::fromUtf8("minGroupIntensity"));
        minGroupIntensity->setMinimum(0);
        minGroupIntensity->setMaximum(1e+09);
        minGroupIntensity->setSingleStep(1000);
        minGroupIntensity->setValue(5000);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, minGroupIntensity);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_5);

        minSN = new QSpinBox(groupBox_2);
        minSN->setObjectName(QString::fromUtf8("minSN"));
        minSN->setMaximum(999999999);
        minSN->setSingleStep(1);
        minSN->setValue(5);

        formLayout_2->setWidget(3, QFormLayout::FieldRole, minSN);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_6);

        minPeakWidth = new QSpinBox(groupBox_2);
        minPeakWidth->setObjectName(QString::fromUtf8("minPeakWidth"));
        minPeakWidth->setMaximum(999999999);
        minPeakWidth->setValue(5);

        formLayout_2->setWidget(4, QFormLayout::FieldRole, minPeakWidth);


        gridLayout->addWidget(groupBox_2, 1, 1, 1, 1);

        groupBox_3 = new QGroupBox(AlignmentDialog);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        formLayout_3 = new QFormLayout(groupBox_3);
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_7);

        maxItterations = new QSpinBox(groupBox_3);
        maxItterations->setObjectName(QString::fromUtf8("maxItterations"));
        maxItterations->setMinimum(1);
        maxItterations->setValue(10);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, maxItterations);

        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout_3->setWidget(2, QFormLayout::LabelRole, label_8);

        polynomialDegree = new QSpinBox(groupBox_3);
        polynomialDegree->setObjectName(QString::fromUtf8("polynomialDegree"));
        polynomialDegree->setMinimum(1);
        polynomialDegree->setMaximum(5);
        polynomialDegree->setValue(5);

        formLayout_3->setWidget(2, QFormLayout::FieldRole, polynomialDegree);

        alignButton = new QPushButton(groupBox_3);
        alignButton->setObjectName(QString::fromUtf8("alignButton"));

        formLayout_3->setWidget(3, QFormLayout::FieldRole, alignButton);


        gridLayout->addWidget(groupBox_3, 2, 1, 1, 1);

        UndoAlignment = new QToolButton(AlignmentDialog);
        UndoAlignment->setObjectName(QString::fromUtf8("UndoAlignment"));

        gridLayout->addWidget(UndoAlignment, 4, 1, 1, 1);


        retranslateUi(AlignmentDialog);

        QMetaObject::connectSlotsByName(AlignmentDialog);
    } // setupUi

    void retranslateUi(QDialog *AlignmentDialog)
    {
        AlignmentDialog->setWindowTitle(QApplication::translate("AlignmentDialog", "Alignment Options", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("AlignmentDialog", "Group Selection Criteria for use in Alignment", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("AlignmentDialog", "Group must contain at least  [X] good peaks", 0, QApplication::UnicodeUTF8));
        minGoodPeakCount->setSuffix(QApplication::translate("AlignmentDialog", " peaks", 0, QApplication::UnicodeUTF8));
        minGoodPeakCount->setPrefix(QString());
        label_3->setText(QApplication::translate("AlignmentDialog", "Limit total number of groups in alignment to", 0, QApplication::UnicodeUTF8));
        limitGroupCount->setSuffix(QApplication::translate("AlignmentDialog", " groups", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("AlignmentDialog", "Peak Grouping Window", 0, QApplication::UnicodeUTF8));
        groupingWindow->setSuffix(QApplication::translate("AlignmentDialog", " scans", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("AlignmentDialog", "Peak Selection", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("AlignmentDialog", "Minumum Peak Intensity", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("AlignmentDialog", "Minimum Peak S/N Ratio", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("AlignmentDialog", "Minimum Peak Width", 0, QApplication::UnicodeUTF8));
        minPeakWidth->setSuffix(QApplication::translate("AlignmentDialog", " scans", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("AlignmentDialog", "Alignment Algorithm", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("AlignmentDialog", "Maximum Number of Itterations", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("AlignmentDialog", "Polynomial Degree", 0, QApplication::UnicodeUTF8));
        alignButton->setText(QApplication::translate("AlignmentDialog", "Align", 0, QApplication::UnicodeUTF8));
        UndoAlignment->setText(QApplication::translate("AlignmentDialog", "Undo Aligment", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AlignmentDialog: public Ui_AlignmentDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALIGNMENTDIALOG_H
