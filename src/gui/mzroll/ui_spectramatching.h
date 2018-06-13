/********************************************************************************
** Form generated from reading UI file 'spectramatching.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPECTRAMATCHING_H
#define UI_SPECTRAMATCHING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTreeWidget>

QT_BEGIN_NAMESPACE

class Ui_SpectraMatchingForm
{
public:
    QGridLayout *gridLayout;
    QLabel *label_3;
    QComboBox *scanTypeComboBox;
    QTreeWidget *resultTable;
    QPlainTextEdit *fragmentsText;
    QLabel *label_4;
    QLabel *label_2;
    QSpinBox *productPPM;
    QLabel *label;
    QLabel *label_5;
    QSpinBox *precursorPPM;
    QLabel *label_6;
    QComboBox *algorithm;
    QPushButton *findButton;
    QProgressBar *progressBar;
    QLineEdit *precursorMz;
    QLabel *label_7;
    QSpinBox *minPeakMatches;
    QPushButton *exportButton;

    void setupUi(QDialog *SpectraMatchingForm)
    {
        if (SpectraMatchingForm->objectName().isEmpty())
            SpectraMatchingForm->setObjectName(QStringLiteral("SpectraMatchingForm"));
        SpectraMatchingForm->resize(484, 492);
        gridLayout = new QGridLayout(SpectraMatchingForm);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_3 = new QLabel(SpectraMatchingForm);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 0, 2, 1, 1);

        scanTypeComboBox = new QComboBox(SpectraMatchingForm);
        scanTypeComboBox->setObjectName(QStringLiteral("scanTypeComboBox"));

        gridLayout->addWidget(scanTypeComboBox, 1, 2, 1, 1);

        resultTable = new QTreeWidget(SpectraMatchingForm);
        resultTable->setObjectName(QStringLiteral("resultTable"));
        resultTable->setEnabled(false);
        resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        resultTable->setAlternatingRowColors(false);

        gridLayout->addWidget(resultTable, 20, 0, 1, 3);

        fragmentsText = new QPlainTextEdit(SpectraMatchingForm);
        fragmentsText->setObjectName(QStringLiteral("fragmentsText"));
        fragmentsText->setBackgroundVisible(false);

        gridLayout->addWidget(fragmentsText, 11, 0, 1, 4);

        label_4 = new QLabel(SpectraMatchingForm);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 0, 0, 1, 1);

        label_2 = new QLabel(SpectraMatchingForm);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 10, 0, 1, 3);

        productPPM = new QSpinBox(SpectraMatchingForm);
        productPPM->setObjectName(QStringLiteral("productPPM"));
        productPPM->setMaximum(100000);
        productPPM->setValue(100);

        gridLayout->addWidget(productPPM, 1, 1, 1, 1);

        label = new QLabel(SpectraMatchingForm);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 7, 0, 1, 1);

        label_5 = new QLabel(SpectraMatchingForm);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 0, 1, 1, 1);

        precursorPPM = new QSpinBox(SpectraMatchingForm);
        precursorPPM->setObjectName(QStringLiteral("precursorPPM"));
        precursorPPM->setMaximum(10000);
        precursorPPM->setValue(100);

        gridLayout->addWidget(precursorPPM, 8, 1, 1, 1);

        label_6 = new QLabel(SpectraMatchingForm);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 7, 1, 1, 1);

        algorithm = new QComboBox(SpectraMatchingForm);
        algorithm->setObjectName(QStringLiteral("algorithm"));

        gridLayout->addWidget(algorithm, 1, 0, 1, 1);

        findButton = new QPushButton(SpectraMatchingForm);
        findButton->setObjectName(QStringLiteral("findButton"));

        gridLayout->addWidget(findButton, 21, 2, 1, 1);

        progressBar = new QProgressBar(SpectraMatchingForm);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(0);

        gridLayout->addWidget(progressBar, 21, 0, 1, 1);

        precursorMz = new QLineEdit(SpectraMatchingForm);
        precursorMz->setObjectName(QStringLiteral("precursorMz"));

        gridLayout->addWidget(precursorMz, 8, 0, 1, 1);

        label_7 = new QLabel(SpectraMatchingForm);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 7, 2, 1, 1);

        minPeakMatches = new QSpinBox(SpectraMatchingForm);
        minPeakMatches->setObjectName(QStringLiteral("minPeakMatches"));
        minPeakMatches->setMinimum(2);
        minPeakMatches->setMaximum(1000000000);

        gridLayout->addWidget(minPeakMatches, 8, 2, 1, 1);

        exportButton = new QPushButton(SpectraMatchingForm);
        exportButton->setObjectName(QStringLiteral("exportButton"));
        exportButton->setEnabled(false);

        gridLayout->addWidget(exportButton, 21, 1, 1, 1);


        retranslateUi(SpectraMatchingForm);

        QMetaObject::connectSlotsByName(SpectraMatchingForm);
    } // setupUi

    void retranslateUi(QDialog *SpectraMatchingForm)
    {
        SpectraMatchingForm->setWindowTitle(QApplication::translate("SpectraMatchingForm", "Spectra Matching", 0));
        label_3->setText(QApplication::translate("SpectraMatchingForm", "Scan Type:", 0));
        scanTypeComboBox->clear();
        scanTypeComboBox->insertItems(0, QStringList()
         << QApplication::translate("SpectraMatchingForm", "any", 0)
         << QApplication::translate("SpectraMatchingForm", "ms1", 0)
         << QApplication::translate("SpectraMatchingForm", "ms2", 0)
         << QApplication::translate("SpectraMatchingForm", "ms3", 0)
         << QApplication::translate("SpectraMatchingForm", "ms4", 0)
         << QApplication::translate("SpectraMatchingForm", "ms5", 0)
         << QApplication::translate("SpectraMatchingForm", "ms6", 0)
         << QApplication::translate("SpectraMatchingForm", "ms7", 0)
         << QApplication::translate("SpectraMatchingForm", "ms8", 0)
        );
        QTreeWidgetItem *___qtreewidgetitem = resultTable->headerItem();
        ___qtreewidgetitem->setText(4, QApplication::translate("SpectraMatchingForm", "intensityList", 0));
        ___qtreewidgetitem->setText(3, QApplication::translate("SpectraMatchingForm", "matchedPeaks", 0));
        ___qtreewidgetitem->setText(2, QApplication::translate("SpectraMatchingForm", "precursorMz", 0));
        ___qtreewidgetitem->setText(1, QApplication::translate("SpectraMatchingForm", "Scan#", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("SpectraMatchingForm", "Score", 0));
        label_4->setText(QApplication::translate("SpectraMatchingForm", "Algorithm", 0));
        label_2->setText(QApplication::translate("SpectraMatchingForm", "List of mz values OR List of mz intensity pairs seperated by commas", 0));
        productPPM->setSuffix(QApplication::translate("SpectraMatchingForm", " ppm", 0));
        productPPM->setPrefix(QApplication::translate("SpectraMatchingForm", "+/- ", 0));
        label->setText(QApplication::translate("SpectraMatchingForm", "Precurrsor m/z ", 0));
        label_5->setText(QApplication::translate("SpectraMatchingForm", "Mass Tollerance", 0));
        precursorPPM->setSuffix(QApplication::translate("SpectraMatchingForm", " ppm", 0));
        precursorPPM->setPrefix(QApplication::translate("SpectraMatchingForm", "+/- ", 0));
        label_6->setText(QApplication::translate("SpectraMatchingForm", "Precursor Mass  Tollerance", 0));
        algorithm->clear();
        algorithm->insertItems(0, QStringList()
         << QApplication::translate("SpectraMatchingForm", "Fragment Search", 0)
         << QApplication::translate("SpectraMatchingForm", "Isotopic Pattern Search", 0)
        );
        findButton->setText(QApplication::translate("SpectraMatchingForm", "Find Matching Spectra", 0));
        label_7->setText(QApplication::translate("SpectraMatchingForm", "Match At Least X peaks", 0));
        minPeakMatches->setSuffix(QApplication::translate("SpectraMatchingForm", " peaks", 0));
        exportButton->setText(QApplication::translate("SpectraMatchingForm", "Export Results", 0));
    } // retranslateUi

};

namespace Ui {
    class SpectraMatchingForm: public Ui_SpectraMatchingForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPECTRAMATCHING_H
