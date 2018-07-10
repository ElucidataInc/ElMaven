/********************************************************************************
** Form generated from reading UI file 'aboutpolly.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTPOLLY_H
#define UI_ABOUTPOLLY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_AboutPolly
{
public:
    QTextBrowser *textBrowser;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;

    void setupUi(QDialog *AboutPolly)
    {
        if (AboutPolly->objectName().isEmpty())
            AboutPolly->setObjectName(QStringLiteral("AboutPolly"));
        AboutPolly->resize(529, 404);
        textBrowser = new QTextBrowser(AboutPolly);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(20, 100, 491, 241));
        label = new QLabel(AboutPolly);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(200, 20, 111, 51));
        label_2 = new QLabel(AboutPolly);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 360, 161, 17));
        label_3 = new QLabel(AboutPolly);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(440, 360, 71, 20));

        retranslateUi(AboutPolly);

        QMetaObject::connectSlotsByName(AboutPolly);
    } // setupUi

    void retranslateUi(QDialog *AboutPolly)
    {
        AboutPolly->setWindowTitle(QApplication::translate("AboutPolly", "AboutPolly", Q_NULLPTR));
        textBrowser->setHtml(QApplication::translate("AboutPolly", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:13px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; background-color:#ffffff;\"><span style=\" font-family:'Heebo,sans-serif'; font-weight:296; color:#666666; background-color:#ffffff;\">Polly</span><span style=\" font-family:'Heebo,sans-serif'; font-weight:296; color:#666666; background-color:#ffffff; vertical-align:super;\">TM</span><span style=\" font-family:'Heebo,sans-serif'; font-weight:296; color:#666666;\">\302\240is a one-stop data analysis platform designed to accelerate the drug discovery process. Polly</span><span style=\" font-family:'Heebo,sans-serif'; font-weight:296; color:#666666; v"
                        "ertical-align:super;\">TM</span><span style=\" font-family:'Heebo,sans-serif'; font-weight:296; color:#666666;\">\302\240can host multiple applications that can be combined by users to create their own analytical workflows. A few clicks are all that are required to perform end-to-end computations and achieve a deeper understanding of the experimental data.</span></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:13px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; background-color:#ffffff;\"><span style=\" font-family:'Heebo,sans-serif'; font-weight:296; color:#666666; background-color:#ffffff;\">Polly</span><span style=\" font-family:'Heebo,sans-serif'; font-weight:296; color:#666666; background-color:#ffffff; vertical-align:super;\">TM</span><span style=\" font-family:'Heebo,sans-serif'; font-weight:296; color:#666666;\">\302\240currently hosts applications for 13C metabolic labeling data analysis and visualization for LC-MS and LC-MS/MS workflows. As Polly</span><sp"
                        "an style=\" font-family:'Heebo,sans-serif'; font-weight:296; color:#666666; vertical-align:super;\">TM</span><span style=\" font-family:'Heebo,sans-serif'; font-weight:296; color:#666666;\">\302\240grows, we envision adding applications for other computation-heavy areas of drug discovery and enable integrated omics analyses. The platform will soon allow users to host their own applications as well.</span></p></body></html>", Q_NULLPTR));
        label->setText(QApplication::translate("AboutPolly", "<html><head/><body><p><img src=\":/images/aboutPolly.png\"/></p></body></html>", Q_NULLPTR));
        label_2->setText(QApplication::translate("AboutPolly", "<html><head/><body><p>Visit our website: <a href=\"https://polly.elucidata.io/#/login\"><span style=\" text-decoration: underline; color:#0000ff;\">Polly</span></a></p></body></html>", Q_NULLPTR));
        label_3->setText(QApplication::translate("AboutPolly", "<html><head/><body><p><a href=\"https://www.youtube.com/watch?reload=9&amp;v=tfxksBKWTS0\"><span style=\" text-decoration: underline; color:#0000ff;\">Video Link</span></a></p></body></html>", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AboutPolly: public Ui_AboutPolly {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTPOLLY_H
