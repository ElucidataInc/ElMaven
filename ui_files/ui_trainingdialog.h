/********************************************************************************
** Form generated from reading UI file 'trainingdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRAININGDIALOG_H
#define UI_TRAININGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_TrainingDialog
{
public:
    QGridLayout *gridLayout;
    QPushButton *trainButton;
    QPushButton *saveButton;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QLabel *TP;
    QLabel *label;
    QLabel *label_2;
    QLabel *FN;
    QLabel *TN;
    QLabel *FP;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *accuracy;
    QLabel *label_5;

    void setupUi(QDialog *TrainingDialog)
    {
        if (TrainingDialog->objectName().isEmpty())
            TrainingDialog->setObjectName(QString::fromUtf8("TrainingDialog"));
        TrainingDialog->resize(308, 242);
        gridLayout = new QGridLayout(TrainingDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        trainButton = new QPushButton(TrainingDialog);
        trainButton->setObjectName(QString::fromUtf8("trainButton"));

        gridLayout->addWidget(trainButton, 0, 1, 1, 1);

        saveButton = new QPushButton(TrainingDialog);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));

        gridLayout->addWidget(saveButton, 0, 2, 1, 1);

        groupBox_2 = new QGroupBox(TrainingDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        TP = new QLabel(groupBox_2);
        TP->setObjectName(QString::fromUtf8("TP"));
        TP->setMaximumSize(QSize(100, 16777215));
        QFont font;
        font.setPointSize(16);
        TP->setFont(font);
        TP->setLayoutDirection(Qt::LeftToRight);
        TP->setStyleSheet(QString::fromUtf8("background-color: rgb(170, 170, 255)"));
        TP->setFrameShape(QFrame::Box);

        gridLayout_3->addWidget(TP, 2, 2, 1, 1);

        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_3->addWidget(label, 2, 0, 1, 1);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_3->addWidget(label_2, 3, 0, 1, 1);

        FN = new QLabel(groupBox_2);
        FN->setObjectName(QString::fromUtf8("FN"));
        FN->setMaximumSize(QSize(100, 16777215));
        QFont font1;
        font1.setFamily(QString::fromUtf8("AlArabiya"));
        font1.setPointSize(16);
        FN->setFont(font1);
        FN->setLayoutDirection(Qt::RightToLeft);
        FN->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 170, 127)"));
        FN->setFrameShape(QFrame::Box);

        gridLayout_3->addWidget(FN, 2, 1, 1, 1);

        TN = new QLabel(groupBox_2);
        TN->setObjectName(QString::fromUtf8("TN"));
        TN->setMaximumSize(QSize(100, 16777215));
        TN->setFont(font);
        TN->setLayoutDirection(Qt::RightToLeft);
        TN->setAutoFillBackground(false);
        TN->setStyleSheet(QString::fromUtf8("background-color: rgb(170, 170, 255)"));
        TN->setFrameShape(QFrame::Box);

        gridLayout_3->addWidget(TN, 3, 1, 1, 1);

        FP = new QLabel(groupBox_2);
        FP->setObjectName(QString::fromUtf8("FP"));
        FP->setMaximumSize(QSize(100, 16777215));
        FP->setFont(font1);
        FP->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 170, 127)"));
        FP->setFrameShape(QFrame::Box);

        gridLayout_3->addWidget(FP, 3, 2, 1, 1);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_3->addWidget(label_3, 1, 1, 1, 1);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_3->addWidget(label_4, 1, 2, 1, 1);

        accuracy = new QLabel(groupBox_2);
        accuracy->setObjectName(QString::fromUtf8("accuracy"));
        accuracy->setFont(font1);
        accuracy->setLayoutDirection(Qt::RightToLeft);
        accuracy->setFrameShape(QFrame::Box);

        gridLayout_3->addWidget(accuracy, 7, 1, 1, 2);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_3->addWidget(label_5, 7, 0, 1, 1);


        gridLayout->addWidget(groupBox_2, 2, 1, 1, 2);


        retranslateUi(TrainingDialog);

        QMetaObject::connectSlotsByName(TrainingDialog);
    } // setupUi

    void retranslateUi(QDialog *TrainingDialog)
    {
        TrainingDialog->setWindowTitle(QApplication::translate("TrainingDialog", "Model Accuracy Dialog", 0, QApplication::UnicodeUTF8));
        trainButton->setText(QApplication::translate("TrainingDialog", "Train", 0, QApplication::UnicodeUTF8));
        saveButton->setText(QApplication::translate("TrainingDialog", "Save Model", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("TrainingDialog", "Training Accuracy", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        TP->setToolTip(QApplication::translate("TrainingDialog", "True Positives", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        TP->setStatusTip(QApplication::translate("TrainingDialog", "TP", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        TP->setText(QApplication::translate("TrainingDialog", "TP", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("TrainingDialog", "Good Peaks", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("TrainingDialog", "Bad Peak", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        FN->setToolTip(QApplication::translate("TrainingDialog", "False Negatives", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        FN->setStatusTip(QApplication::translate("TrainingDialog", "FN", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        FN->setText(QApplication::translate("TrainingDialog", "FN", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        TN->setToolTip(QApplication::translate("TrainingDialog", "True Negatives", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        TN->setStatusTip(QApplication::translate("TrainingDialog", "TN", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        TN->setText(QApplication::translate("TrainingDialog", "TN", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        FP->setToolTip(QApplication::translate("TrainingDialog", "False Positives", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        FP->setStatusTip(QApplication::translate("TrainingDialog", "FP", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        FP->setText(QApplication::translate("TrainingDialog", "FP", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("TrainingDialog", "Quality < 0.5", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("TrainingDialog", "Quality > 0.5", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        accuracy->setToolTip(QApplication::translate("TrainingDialog", "ACCURACY", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        accuracy->setStatusTip(QApplication::translate("TrainingDialog", "ACCURACY", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        accuracy->setText(QApplication::translate("TrainingDialog", "ACCURACY", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("TrainingDialog", "% Accuracy", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TrainingDialog: public Ui_TrainingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRAININGDIALOG_H
