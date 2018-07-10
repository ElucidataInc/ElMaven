/********************************************************************************
** Form generated from reading UI file 'gettingstarted.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GETTINGSTARTED_H
#define UI_GETTINGSTARTED_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GettingStarted
{
public:
    QPushButton *closeButton;
    QCheckBox *checkBox;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QTextBrowser *textBrowser_2;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QTextBrowser *textBrowser;

    void setupUi(QDialog *GettingStarted)
    {
        if (GettingStarted->objectName().isEmpty())
            GettingStarted->setObjectName(QStringLiteral("GettingStarted"));
        GettingStarted->resize(867, 540);
        closeButton = new QPushButton(GettingStarted);
        closeButton->setObjectName(QStringLiteral("closeButton"));
        closeButton->setGeometry(QRect(750, 510, 89, 25));
        checkBox = new QCheckBox(GettingStarted);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(20, 510, 181, 23));
        verticalLayoutWidget = new QWidget(GettingStarted);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, -3, 851, 281));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        textBrowser_2 = new QTextBrowser(verticalLayoutWidget);
        textBrowser_2->setObjectName(QStringLiteral("textBrowser_2"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(textBrowser_2->sizePolicy().hasHeightForWidth());
        textBrowser_2->setSizePolicy(sizePolicy);
        textBrowser_2->setStyleSheet(QStringLiteral("border:0;"));
        textBrowser_2->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);

        horizontalLayout->addWidget(textBrowser_2);


        verticalLayout->addLayout(horizontalLayout);

        verticalLayoutWidget_2 = new QWidget(GettingStarted);
        verticalLayoutWidget_2->setObjectName(QStringLiteral("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(10, 280, 851, 221));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        textBrowser = new QTextBrowser(verticalLayoutWidget_2);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setStyleSheet(QStringLiteral("border:0;"));

        verticalLayout_2->addWidget(textBrowser);


        retranslateUi(GettingStarted);

        QMetaObject::connectSlotsByName(GettingStarted);
    } // setupUi

    void retranslateUi(QDialog *GettingStarted)
    {
        GettingStarted->setWindowTitle(QApplication::translate("GettingStarted", "Dialog", Q_NULLPTR));
        closeButton->setText(QApplication::translate("GettingStarted", "Close", Q_NULLPTR));
        checkBox->setText(QApplication::translate("GettingStarted", " Don't show this again.", Q_NULLPTR));
        label->setText(QApplication::translate("GettingStarted", "<html><head/><body><p><img src=\":/images/GettingStartedpic.png\"/></p></body></html>", Q_NULLPTR));
        textBrowser_2->setHtml(QApplication::translate("GettingStarted", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">   </p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> <span style=\" font-size:14pt; font-weight:600;\">1. Upload Sample Datasets</span></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">         ElMAVEN accepts .mzXML and .mzML files. </p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent"
                        ":0; text-indent:0px;\">              Convert raw files <a href=\"http://proteowizard.sourceforge.net/download.html\"><span style=\" text-decoration: underline; color:#0000ff;\">here</span></a>.</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:400px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">       </p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:14pt; font-weight:600;\">2. Upload Compound Database</span></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:430px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:14pt; font-weight:600;\"><br /></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:14pt; font-weight:600;\">3. Curate Peaks</span></p>\n"
"<p align=\"center\" s"
                        "tyle=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Select Automated or Manual Peak Detection and click </p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">&quot;Find Peaks&quot; on the window that appears.</p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:14pt; font-weight:600;\">4. </span><a href=\"https://polly.elucidata.io/\"><span style=\" font-size:14pt; font-weight:600; text-decoration: underline; color:#0000ff;\">Go To Polly</span></a></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-i"
                        "ndent:0; text-indent:0px;\"><span style=\" font-size:14pt;\"> </span>Store, Collaborate, Analyze and Visualize your omics </p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">        data on cloud.</p></body></html>", Q_NULLPTR));
        textBrowser->setHtml(QApplication::translate("GettingStarted", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Try El-MAVEN with sample data: </span><a href=\"http://genomics-pubs.princeton.edu/mzroll/datasets/exampleProject.zip\"><span style=\" text-decoration: underline; color:#0000ff;\">Download</span></a></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-"
                        "indent:0; text-indent:0px; text-decoration: underline; color:#0000ff;\"><br /></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">         <span style=\" font-weight:600;\">Note: Parameters for labeled/unlabeled, LCMS and LCMS/MS data are listed in Documentation</span></p>\n"
"<hr />\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:14pt; font-weight:600;\">How to get Help?</span><br /></p>\n"
"<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\"><li style=\" margin-top:12px; margin-bottom:0px; margin-left:170px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"https://github.com/ElucidataInc/ElMaven/wiki\"><span style=\" text-decoration: underline; color:#0000ff;\">Documentation</span></a> Check our step  by step guide, for your "
                        "workflow.</li>\n"
"<li style=\" margin-top:0px; margin-bottom:0px; margin-left:170px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"https://www.youtube.com/channel/UCZYVM0I1zqRgkGTdIlQZ9Yw/videos\"><span style=\" text-decoration: underline; color:#0000ff;\">Video Tutorials</span></a> Watch our tutorials to get started easily.</li>\n"
"<li style=\" margin-top:0px; margin-bottom:0px; margin-left:170px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"https://elucidatainc.github.io/ElMaven/faq/\"><span style=\" text-decoration: underline; color:#0000ff;\">FAQs</span></a> Ask your questions. We'll answer promptly!</li></ul></body></html>", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GettingStarted: public Ui_GettingStarted {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GETTINGSTARTED_H
