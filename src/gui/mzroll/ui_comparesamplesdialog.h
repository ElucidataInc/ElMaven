/********************************************************************************
** Form generated from reading UI file 'comparesamplesdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPARESAMPLESDIALOG_H
#define UI_COMPARESAMPLESDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_CompareSamplesDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label_6;
    QPushButton *compareButton;
    QPushButton *resetButton;
    QPushButton *cancelButton;
    QDoubleSpinBox *minFoldDiff;
    QLabel *label_5;
    QLabel *label_4;
    QDoubleSpinBox *minIntensity;
    QDoubleSpinBox *minPValue;
    QComboBox *correctionBox;
    QLabel *label_3;
    QDoubleSpinBox *missingValue;
    QLabel *label_7;
    QLabel *label_8;
    QSpinBox *minGoodSamples;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QLabel *label_2;
    QListWidget *filelist1;
    QLabel *label;
    QLabel *fileCount1;
    QLabel *fileCount2;
    QListWidget *filelist2;

    void setupUi(QDialog *CompareSamplesDialog)
    {
        if (CompareSamplesDialog->objectName().isEmpty())
            CompareSamplesDialog->setObjectName(QStringLiteral("CompareSamplesDialog"));
        CompareSamplesDialog->resize(571, 319);
        gridLayout = new QGridLayout(CompareSamplesDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_6 = new QLabel(CompareSamplesDialog);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 7, 4, 1, 1);

        compareButton = new QPushButton(CompareSamplesDialog);
        compareButton->setObjectName(QStringLiteral("compareButton"));

        gridLayout->addWidget(compareButton, 5, 0, 1, 1);

        resetButton = new QPushButton(CompareSamplesDialog);
        resetButton->setObjectName(QStringLiteral("resetButton"));

        gridLayout->addWidget(resetButton, 5, 1, 1, 1);

        cancelButton = new QPushButton(CompareSamplesDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        gridLayout->addWidget(cancelButton, 5, 2, 1, 1);

        minFoldDiff = new QDoubleSpinBox(CompareSamplesDialog);
        minFoldDiff->setObjectName(QStringLiteral("minFoldDiff"));
        minFoldDiff->setDecimals(2);
        minFoldDiff->setMinimum(-1e+09);
        minFoldDiff->setMaximum(1e+09);
        minFoldDiff->setValue(-100);

        gridLayout->addWidget(minFoldDiff, 7, 1, 1, 1);

        label_5 = new QLabel(CompareSamplesDialog);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 7, 0, 1, 1);

        label_4 = new QLabel(CompareSamplesDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 7, 2, 1, 1);

        minIntensity = new QDoubleSpinBox(CompareSamplesDialog);
        minIntensity->setObjectName(QStringLiteral("minIntensity"));
        minIntensity->setDecimals(1);
        minIntensity->setMinimum(1);
        minIntensity->setMaximum(1e+09);
        minIntensity->setValue(1);

        gridLayout->addWidget(minIntensity, 7, 3, 1, 1);

        minPValue = new QDoubleSpinBox(CompareSamplesDialog);
        minPValue->setObjectName(QStringLiteral("minPValue"));
        minPValue->setDecimals(8);
        minPValue->setMaximum(1);
        minPValue->setSingleStep(0.01);
        minPValue->setValue(1);

        gridLayout->addWidget(minPValue, 7, 5, 1, 1);

        correctionBox = new QComboBox(CompareSamplesDialog);
        correctionBox->setObjectName(QStringLiteral("correctionBox"));

        gridLayout->addWidget(correctionBox, 8, 5, 1, 1);

        label_3 = new QLabel(CompareSamplesDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 8, 0, 1, 1);

        missingValue = new QDoubleSpinBox(CompareSamplesDialog);
        missingValue->setObjectName(QStringLiteral("missingValue"));
        missingValue->setMaximum(1e+09);
        missingValue->setValue(1);

        gridLayout->addWidget(missingValue, 8, 1, 1, 1);

        label_7 = new QLabel(CompareSamplesDialog);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 8, 4, 1, 1);

        label_8 = new QLabel(CompareSamplesDialog);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout->addWidget(label_8, 8, 2, 1, 1);

        minGoodSamples = new QSpinBox(CompareSamplesDialog);
        minGoodSamples->setObjectName(QStringLiteral("minGoodSamples"));
        minGoodSamples->setValue(0);

        gridLayout->addWidget(minGoodSamples, 8, 3, 1, 1);

        frame = new QFrame(CompareSamplesDialog);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_2, 0, 2, 1, 1);

        filelist1 = new QListWidget(frame);
        filelist1->setObjectName(QStringLiteral("filelist1"));
        filelist1->setAcceptDrops(true);
        filelist1->setFrameShape(QFrame::WinPanel);
        filelist1->setFrameShadow(QFrame::Raised);
        filelist1->setEditTriggers(QAbstractItemView::NoEditTriggers);
        filelist1->setDragEnabled(true);
        filelist1->setDragDropOverwriteMode(true);
        filelist1->setDragDropMode(QAbstractItemView::DropOnly);
        filelist1->setAlternatingRowColors(false);
        filelist1->setSelectionMode(QAbstractItemView::MultiSelection);
        filelist1->setSortingEnabled(false);

        gridLayout_2->addWidget(filelist1, 2, 0, 1, 1);

        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        fileCount1 = new QLabel(frame);
        fileCount1->setObjectName(QStringLiteral("fileCount1"));

        gridLayout_2->addWidget(fileCount1, 3, 0, 1, 1);

        fileCount2 = new QLabel(frame);
        fileCount2->setObjectName(QStringLiteral("fileCount2"));

        gridLayout_2->addWidget(fileCount2, 3, 2, 1, 1);

        filelist2 = new QListWidget(frame);
        filelist2->setObjectName(QStringLiteral("filelist2"));
        filelist2->setAcceptDrops(true);
        filelist2->setFrameShape(QFrame::WinPanel);
        filelist2->setFrameShadow(QFrame::Raised);
        filelist2->setProperty("showDropIndicator", QVariant(false));
        filelist2->setDragEnabled(false);
        filelist2->setDragDropOverwriteMode(false);
        filelist2->setDragDropMode(QAbstractItemView::DropOnly);
        filelist2->setSelectionMode(QAbstractItemView::MultiSelection);

        gridLayout_2->addWidget(filelist2, 2, 2, 1, 1);


        gridLayout->addWidget(frame, 2, 0, 2, 7);


        retranslateUi(CompareSamplesDialog);
        QObject::connect(cancelButton, SIGNAL(clicked()), CompareSamplesDialog, SLOT(close()));

        QMetaObject::connectSlotsByName(CompareSamplesDialog);
    } // setupUi

    void retranslateUi(QDialog *CompareSamplesDialog)
    {
        CompareSamplesDialog->setWindowTitle(QApplication::translate("CompareSamplesDialog", "Compare Samples", 0));
        label_6->setText(QApplication::translate("CompareSamplesDialog", "p. value", 0));
        compareButton->setText(QApplication::translate("CompareSamplesDialog", "Compare Sets", 0));
        resetButton->setText(QApplication::translate("CompareSamplesDialog", "Reset", 0));
        cancelButton->setText(QApplication::translate("CompareSamplesDialog", "Done", 0));
        label_5->setText(QApplication::translate("CompareSamplesDialog", "Min LOG2 Fold Diff", 0));
        label_4->setText(QApplication::translate("CompareSamplesDialog", "Min. Intensity", 0));
        correctionBox->clear();
        correctionBox->insertItems(0, QStringList()
         << QApplication::translate("CompareSamplesDialog", "No Correction", 0)
         << QApplication::translate("CompareSamplesDialog", "Bonferroni", 0)
         << QApplication::translate("CompareSamplesDialog", "Holms", 0)
         << QApplication::translate("CompareSamplesDialog", "Benjamini", 0)
        );
        label_3->setText(QApplication::translate("CompareSamplesDialog", "Set Missing Values", 0));
        label_7->setText(QApplication::translate("CompareSamplesDialog", "FDR Correction", 0));
        label_8->setText(QApplication::translate("CompareSamplesDialog", "Min. Good Samples", 0));
        label_2->setText(QApplication::translate("CompareSamplesDialog", "Set 2", 0));
        label->setText(QApplication::translate("CompareSamplesDialog", "Set 1", 0));
        fileCount1->setText(QApplication::translate("CompareSamplesDialog", "0 samples selected", 0));
        fileCount2->setText(QApplication::translate("CompareSamplesDialog", "0 samples selected", 0));
    } // retranslateUi

};

namespace Ui {
    class CompareSamplesDialog: public Ui_CompareSamplesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPARESAMPLESDIALOG_H
