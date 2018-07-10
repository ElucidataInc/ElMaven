/********************************************************************************
** Form generated from reading UI file 'isotopeplotdockwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ISOTOPEPLOTDOCKWIDGET_H
#define UI_ISOTOPEPLOTDOCKWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_IsotopePlotDockWidget
{
public:
    QWidget *isotopePlot;

    void setupUi(QDockWidget *IsotopePlotDockWidget)
    {
        if (IsotopePlotDockWidget->objectName().isEmpty())
            IsotopePlotDockWidget->setObjectName(QStringLiteral("IsotopePlotDockWidget"));
        IsotopePlotDockWidget->resize(595, 300);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(IsotopePlotDockWidget->sizePolicy().hasHeightForWidth());
        IsotopePlotDockWidget->setSizePolicy(sizePolicy);
        isotopePlot = new QWidget();
        isotopePlot->setObjectName(QStringLiteral("isotopePlot"));
        IsotopePlotDockWidget->setWidget(isotopePlot);

        retranslateUi(IsotopePlotDockWidget);

        QMetaObject::connectSlotsByName(IsotopePlotDockWidget);
    } // setupUi

    void retranslateUi(QDockWidget *IsotopePlotDockWidget)
    {
        IsotopePlotDockWidget->setWindowTitle(QApplication::translate("IsotopePlotDockWidget", "DockWidget", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class IsotopePlotDockWidget: public Ui_IsotopePlotDockWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ISOTOPEPLOTDOCKWIDGET_H
