/********************************************************************************
** Form generated from reading UI file 'rconsolewidget.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RCONSOLEWIDGET_H
#define UI_RCONSOLEWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RconsoleWidget
{
public:
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout;
    QTextEdit *editor;
    QTabWidget *tabWidget;
    QWidget *outputTab;
    QHBoxLayout *horizontalLayout;
    QTextEdit *outputLog;
    QWidget *errorTab;
    QHBoxLayout *horizontalLayout_2;
    QPlainTextEdit *errorLog;
    QSpacerItem *horizontalSpacer;
    QLabel *statusLabel;
    QPushButton *btnRun;
    QPushButton *btnOpen;
    QPushButton *btnSave;
    QLabel *fileLabel;

    void setupUi(QDockWidget *RconsoleWidget)
    {
        if (RconsoleWidget->objectName().isEmpty())
            RconsoleWidget->setObjectName(QStringLiteral("RconsoleWidget"));
        RconsoleWidget->resize(465, 586);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        gridLayout = new QGridLayout(dockWidgetContents);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        editor = new QTextEdit(dockWidgetContents);
        editor->setObjectName(QStringLiteral("editor"));

        gridLayout->addWidget(editor, 6, 0, 1, 9);

        tabWidget = new QTabWidget(dockWidgetContents);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        outputTab = new QWidget();
        outputTab->setObjectName(QStringLiteral("outputTab"));
        horizontalLayout = new QHBoxLayout(outputTab);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        outputLog = new QTextEdit(outputTab);
        outputLog->setObjectName(QStringLiteral("outputLog"));

        horizontalLayout->addWidget(outputLog);

        tabWidget->addTab(outputTab, QString());
        errorTab = new QWidget();
        errorTab->setObjectName(QStringLiteral("errorTab"));
        horizontalLayout_2 = new QHBoxLayout(errorTab);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        errorLog = new QPlainTextEdit(errorTab);
        errorLog->setObjectName(QStringLiteral("errorLog"));

        horizontalLayout_2->addWidget(errorLog);

        tabWidget->addTab(errorTab, QString());

        gridLayout->addWidget(tabWidget, 9, 0, 1, 9);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 2, 1, 1);

        statusLabel = new QLabel(dockWidgetContents);
        statusLabel->setObjectName(QStringLiteral("statusLabel"));

        gridLayout->addWidget(statusLabel, 0, 5, 1, 1);

        btnRun = new QPushButton(dockWidgetContents);
        btnRun->setObjectName(QStringLiteral("btnRun"));

        gridLayout->addWidget(btnRun, 0, 6, 1, 1);

        btnOpen = new QPushButton(dockWidgetContents);
        btnOpen->setObjectName(QStringLiteral("btnOpen"));

        gridLayout->addWidget(btnOpen, 0, 0, 1, 1);

        btnSave = new QPushButton(dockWidgetContents);
        btnSave->setObjectName(QStringLiteral("btnSave"));
        btnSave->setEnabled(false);

        gridLayout->addWidget(btnSave, 0, 1, 1, 1);

        fileLabel = new QLabel(dockWidgetContents);
        fileLabel->setObjectName(QStringLiteral("fileLabel"));

        gridLayout->addWidget(fileLabel, 4, 0, 1, 7);

        RconsoleWidget->setWidget(dockWidgetContents);

        retranslateUi(RconsoleWidget);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(RconsoleWidget);
    } // setupUi

    void retranslateUi(QDockWidget *RconsoleWidget)
    {
        RconsoleWidget->setWindowTitle(QApplication::translate("RconsoleWidget", "R Script Editor", 0));
        tabWidget->setTabText(tabWidget->indexOf(outputTab), QApplication::translate("RconsoleWidget", "Output", 0));
        tabWidget->setTabText(tabWidget->indexOf(errorTab), QApplication::translate("RconsoleWidget", "Error Log", 0));
        statusLabel->setText(QApplication::translate("RconsoleWidget", "Status:", 0));
        btnRun->setText(QApplication::translate("RconsoleWidget", "Run", 0));
        btnOpen->setText(QApplication::translate("RconsoleWidget", "Load Script", 0));
        btnSave->setText(QApplication::translate("RconsoleWidget", "Save Script", 0));
        fileLabel->setText(QApplication::translate("RconsoleWidget", "File:", 0));
    } // retranslateUi

};

namespace Ui {
    class RconsoleWidget: public Ui_RconsoleWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RCONSOLEWIDGET_H
