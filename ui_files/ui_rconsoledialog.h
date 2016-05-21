/********************************************************************************
** Form generated from reading UI file 'rconsoledialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RCONSOLEDIALOG_H
#define UI_RCONSOLEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RConsoleDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QGridLayout *gridLayout;
    QPushButton *runButton;
    QPushButton *clearButton;
    QTextEdit *scriptEdit;
    QLabel *label;
    QLabel *statusLabel;

    void setupUi(QDialog *RConsoleDialog)
    {
        if (RConsoleDialog->objectName().isEmpty())
            RConsoleDialog->setObjectName(QString::fromUtf8("RConsoleDialog"));
        RConsoleDialog->resize(511, 486);
        verticalLayout = new QVBoxLayout(RConsoleDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame = new QFrame(RConsoleDialog);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::NoFrame);
        frame->setFrameShadow(QFrame::Plain);
        frame->setLineWidth(0);
        gridLayout = new QGridLayout(frame);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        runButton = new QPushButton(frame);
        runButton->setObjectName(QString::fromUtf8("runButton"));

        gridLayout->addWidget(runButton, 3, 3, 1, 1);

        clearButton = new QPushButton(frame);
        clearButton->setObjectName(QString::fromUtf8("clearButton"));

        gridLayout->addWidget(clearButton, 3, 2, 1, 1);

        scriptEdit = new QTextEdit(frame);
        scriptEdit->setObjectName(QString::fromUtf8("scriptEdit"));
        scriptEdit->setAcceptRichText(false);

        gridLayout->addWidget(scriptEdit, 1, 0, 1, 4);

        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        statusLabel = new QLabel(frame);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));

        gridLayout->addWidget(statusLabel, 3, 0, 1, 2);


        verticalLayout->addWidget(frame);


        retranslateUi(RConsoleDialog);
        QObject::connect(clearButton, SIGNAL(clicked()), scriptEdit, SLOT(clear()));

        QMetaObject::connectSlotsByName(RConsoleDialog);
    } // setupUi

    void retranslateUi(QDialog *RConsoleDialog)
    {
        RConsoleDialog->setWindowTitle(QApplication::translate("RConsoleDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        runButton->setText(QApplication::translate("RConsoleDialog", "Run Script", 0, QApplication::UnicodeUTF8));
        clearButton->setText(QApplication::translate("RConsoleDialog", "Clear Script", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("RConsoleDialog", "R Script", 0, QApplication::UnicodeUTF8));
        statusLabel->setText(QApplication::translate("RConsoleDialog", "Status", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RConsoleDialog: public Ui_RConsoleDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RCONSOLEDIALOG_H
