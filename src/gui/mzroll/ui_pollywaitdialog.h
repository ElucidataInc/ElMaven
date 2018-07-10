/********************************************************************************
** Form generated from reading UI file 'pollywaitdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POLLYWAITDIALOG_H
#define UI_POLLYWAITDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_PollyWaitDialog
{
public:
    QLabel *label;
    QLabel *statusLabel;

    void setupUi(QDialog *PollyWaitDialog)
    {
        if (PollyWaitDialog->objectName().isEmpty())
            PollyWaitDialog->setObjectName(QStringLiteral("PollyWaitDialog"));
        PollyWaitDialog->setWindowModality(Qt::WindowModal);
        PollyWaitDialog->resize(409, 341);
        label = new QLabel(PollyWaitDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(80, 30, 241, 211));
        statusLabel = new QLabel(PollyWaitDialog);
        statusLabel->setObjectName(QStringLiteral("statusLabel"));
        statusLabel->setGeometry(QRect(110, 290, 181, 17));

        retranslateUi(PollyWaitDialog);

        QMetaObject::connectSlotsByName(PollyWaitDialog);
    } // setupUi

    void retranslateUi(QDialog *PollyWaitDialog)
    {
        PollyWaitDialog->setWindowTitle(QApplication::translate("PollyWaitDialog", "Dialog", Q_NULLPTR));
        label->setText(QApplication::translate("PollyWaitDialog", "<html><head/><body><p align=\"center\"><img src=\":/images/loading.gif\"/></p></body></html>", Q_NULLPTR));
        statusLabel->setText(QApplication::translate("PollyWaitDialog", "<html><head/><body><p align=\"center\">TextLabel</p></body></html>", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class PollyWaitDialog: public Ui_PollyWaitDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POLLYWAITDIALOG_H
