/********************************************************************************
** Form generated from reading UI file 'adductwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDUCTWIDGET_H
#define UI_ADDUCTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AdductWidget
{
public:
    QWidget *dockWidgetContents;
    QHBoxLayout *horizontalLayout;
    QTableWidget *adductTable;

    void setupUi(QDockWidget *AdductWidget)
    {
        if (AdductWidget->objectName().isEmpty())
            AdductWidget->setObjectName(QStringLiteral("AdductWidget"));
        AdductWidget->resize(562, 490);
        AdductWidget->setMaximumSize(QSize(16777215, 16777215));
        AdductWidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        dockWidgetContents->setGeometry(QRect(0, 22, 562, 468));
        horizontalLayout = new QHBoxLayout(dockWidgetContents);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        adductTable = new QTableWidget(dockWidgetContents);
        adductTable->setObjectName(QStringLiteral("adductTable"));

        horizontalLayout->addWidget(adductTable);

        AdductWidget->setWidget(dockWidgetContents);

        retranslateUi(AdductWidget);

        QMetaObject::connectSlotsByName(AdductWidget);
    } // setupUi

    void retranslateUi(QDockWidget *AdductWidget)
    {
        AdductWidget->setWindowTitle(QApplication::translate("AdductWidget", "Adducts", 0));
    } // retranslateUi

};

namespace Ui {
    class AdductWidget: public Ui_AdductWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDUCTWIDGET_H
