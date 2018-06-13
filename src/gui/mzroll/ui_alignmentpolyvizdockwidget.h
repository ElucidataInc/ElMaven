/********************************************************************************
** Form generated from reading UI file 'alignmentpolyvizdockwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALIGNMENTPOLYVIZDOCKWIDGET_H
#define UI_ALIGNMENTPOLYVIZDOCKWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AlignmentPolyVizDockWidget
{
public:
    QWidget *dockWidgetContents;

    void setupUi(QDockWidget *AlignmentPolyVizDockWidget)
    {
        if (AlignmentPolyVizDockWidget->objectName().isEmpty())
            AlignmentPolyVizDockWidget->setObjectName(QStringLiteral("AlignmentPolyVizDockWidget"));
        AlignmentPolyVizDockWidget->resize(400, 300);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        AlignmentPolyVizDockWidget->setWidget(dockWidgetContents);

        retranslateUi(AlignmentPolyVizDockWidget);

        QMetaObject::connectSlotsByName(AlignmentPolyVizDockWidget);
    } // setupUi

    void retranslateUi(QDockWidget *AlignmentPolyVizDockWidget)
    {
        AlignmentPolyVizDockWidget->setWindowTitle(QApplication::translate("AlignmentPolyVizDockWidget", "DockWidget", 0));
    } // retranslateUi

};

namespace Ui {
    class AlignmentPolyVizDockWidget: public Ui_AlignmentPolyVizDockWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALIGNMENTPOLYVIZDOCKWIDGET_H
