/********************************************************************************
** Form generated from reading UI file 'isotopeswidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ISOTOPESWIDGET_H
#define UI_ISOTOPESWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_isotopesWidget
{
public:
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout_2;
    QLineEdit *formula;
    QDoubleSpinBox *ionization;
    QLabel *label_2;
    QLabel *label;
    QTreeWidget *treeWidget;

    void setupUi(QDockWidget *isotopesWidget)
    {
        if (isotopesWidget->objectName().isEmpty())
            isotopesWidget->setObjectName(QString::fromUtf8("isotopesWidget"));
        isotopesWidget->resize(479, 412);
        isotopesWidget->setMaximumSize(QSize(524287, 524287));
        isotopesWidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        gridLayout_2 = new QGridLayout(dockWidgetContents);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        formula = new QLineEdit(dockWidgetContents);
        formula->setObjectName(QString::fromUtf8("formula"));

        gridLayout_2->addWidget(formula, 1, 1, 1, 1);

        ionization = new QDoubleSpinBox(dockWidgetContents);
        ionization->setObjectName(QString::fromUtf8("ionization"));
        ionization->setDecimals(1);
        ionization->setMinimum(-100);

        gridLayout_2->addWidget(ionization, 1, 3, 1, 1);

        label_2 = new QLabel(dockWidgetContents);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 1, 2, 1, 1);

        label = new QLabel(dockWidgetContents);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        treeWidget = new QTreeWidget(dockWidgetContents);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));

        gridLayout_2->addWidget(treeWidget, 2, 0, 1, 4);

        isotopesWidget->setWidget(dockWidgetContents);

        retranslateUi(isotopesWidget);

        QMetaObject::connectSlotsByName(isotopesWidget);
    } // setupUi

    void retranslateUi(QDockWidget *isotopesWidget)
    {
        isotopesWidget->setWindowTitle(QApplication::translate("isotopesWidget", "Isotopes", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("isotopesWidget", "Ionization", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("isotopesWidget", "Formula", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class isotopesWidget: public Ui_isotopesWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ISOTOPESWIDGET_H
