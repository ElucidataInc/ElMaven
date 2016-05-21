/********************************************************************************
** Form generated from reading UI file 'spectramatching.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPECTRAMATCHING_H
#define UI_SPECTRAMATCHING_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTreeWidget>

QT_BEGIN_NAMESPACE

class Ui_SpectraMatchingForm
{
public:
    QGridLayout *gridLayout;
    QPlainTextEdit *fragmentsText;
    QLabel *label;
    QPushButton *findButton;
    QLabel *label_2;
    QLineEdit *precursorMz;
    QLabel *label_3;
    QComboBox *scanTypeComboBox;
    QTreeWidget *resultTable;

    void setupUi(QDialog *SpectraMatchingForm)
    {
        if (SpectraMatchingForm->objectName().isEmpty())
            SpectraMatchingForm->setObjectName(QString::fromUtf8("SpectraMatchingForm"));
        SpectraMatchingForm->resize(615, 359);
        gridLayout = new QGridLayout(SpectraMatchingForm);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        fragmentsText = new QPlainTextEdit(SpectraMatchingForm);
        fragmentsText->setObjectName(QString::fromUtf8("fragmentsText"));
        fragmentsText->setBackgroundVisible(false);

        gridLayout->addWidget(fragmentsText, 6, 0, 1, 1);

        label = new QLabel(SpectraMatchingForm);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 2, 0, 1, 1);

        findButton = new QPushButton(SpectraMatchingForm);
        findButton->setObjectName(QString::fromUtf8("findButton"));

        gridLayout->addWidget(findButton, 7, 3, 1, 1);

        label_2 = new QLabel(SpectraMatchingForm);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 4, 0, 1, 1);

        precursorMz = new QLineEdit(SpectraMatchingForm);
        precursorMz->setObjectName(QString::fromUtf8("precursorMz"));

        gridLayout->addWidget(precursorMz, 3, 0, 1, 1);

        label_3 = new QLabel(SpectraMatchingForm);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 2, 1, 1);

        scanTypeComboBox = new QComboBox(SpectraMatchingForm);
        scanTypeComboBox->setObjectName(QString::fromUtf8("scanTypeComboBox"));

        gridLayout->addWidget(scanTypeComboBox, 3, 2, 1, 1);

        resultTable = new QTreeWidget(SpectraMatchingForm);
        resultTable->setObjectName(QString::fromUtf8("resultTable"));
        resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        resultTable->setAlternatingRowColors(false);

        gridLayout->addWidget(resultTable, 6, 1, 1, 3);


        retranslateUi(SpectraMatchingForm);

        QMetaObject::connectSlotsByName(SpectraMatchingForm);
    } // setupUi

    void retranslateUi(QDialog *SpectraMatchingForm)
    {
        SpectraMatchingForm->setWindowTitle(QApplication::translate("SpectraMatchingForm", "Spectra Matching", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SpectraMatchingForm", "Precurrsor m/z ", 0, QApplication::UnicodeUTF8));
        findButton->setText(QApplication::translate("SpectraMatchingForm", "Find", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("SpectraMatchingForm", "List of mz values(or list of mz intensity pairs) seperated by commas ", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("SpectraMatchingForm", "Scan Type:", 0, QApplication::UnicodeUTF8));
        scanTypeComboBox->clear();
        scanTypeComboBox->insertItems(0, QStringList()
         << QApplication::translate("SpectraMatchingForm", "any", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SpectraMatchingForm", "ms1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SpectraMatchingForm", "ms2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SpectraMatchingForm", "ms3", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SpectraMatchingForm", "ms4", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SpectraMatchingForm", "ms5", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SpectraMatchingForm", "ms6", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SpectraMatchingForm", "ms7", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SpectraMatchingForm", "ms8", 0, QApplication::UnicodeUTF8)
        );
        QTreeWidgetItem *___qtreewidgetitem = resultTable->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("SpectraMatchingForm", "Score", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("SpectraMatchingForm", "Scan#", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SpectraMatchingForm: public Ui_SpectraMatchingForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPECTRAMATCHING_H
