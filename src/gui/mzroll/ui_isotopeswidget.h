/********************************************************************************
** Form generated from reading UI file 'isotopeswidget.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ISOTOPESWIDGET_H
#define UI_ISOTOPESWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_isotopesWidget
{
public:
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout_2;
    QTreeWidget *treeWidget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *formula;
    QLabel *label_3;
    QComboBox *sampleList;
    QLabel *label_2;
    QDoubleSpinBox *ionization;

    void setupUi(QDockWidget *isotopesWidget)
    {
        if (isotopesWidget->objectName().isEmpty())
            isotopesWidget->setObjectName(QStringLiteral("isotopesWidget"));
        isotopesWidget->resize(479, 412);
        isotopesWidget->setMaximumSize(QSize(524287, 524287));
        isotopesWidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        gridLayout_2 = new QGridLayout(dockWidgetContents);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        treeWidget = new QTreeWidget(dockWidgetContents);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));

        gridLayout_2->addWidget(treeWidget, 2, 0, 1, 6);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(dockWidgetContents);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        formula = new QLineEdit(dockWidgetContents);
        formula->setObjectName(QStringLiteral("formula"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(formula->sizePolicy().hasHeightForWidth());
        formula->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(formula);

        label_3 = new QLabel(dockWidgetContents);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_2->addWidget(label_3);

        sampleList = new QComboBox(dockWidgetContents);
        sampleList->setObjectName(QStringLiteral("sampleList"));
        sizePolicy.setHeightForWidth(sampleList->sizePolicy().hasHeightForWidth());
        sampleList->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(sampleList);

        label_2 = new QLabel(dockWidgetContents);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        ionization = new QDoubleSpinBox(dockWidgetContents);
        ionization->setObjectName(QStringLiteral("ionization"));
        ionization->setDecimals(1);
        ionization->setMinimum(-100);

        horizontalLayout_2->addWidget(ionization);


        gridLayout_2->addLayout(horizontalLayout_2, 0, 0, 1, 6);

        isotopesWidget->setWidget(dockWidgetContents);

        retranslateUi(isotopesWidget);

        QMetaObject::connectSlotsByName(isotopesWidget);
    } // setupUi

    void retranslateUi(QDockWidget *isotopesWidget)
    {
        isotopesWidget->setWindowTitle(QApplication::translate("isotopesWidget", "Isotopes", 0));
        label->setText(QApplication::translate("isotopesWidget", "Formula", 0));
        label_3->setText(QApplication::translate("isotopesWidget", "Sample", 0));
        label_2->setText(QApplication::translate("isotopesWidget", "Ionization", 0));
    } // retranslateUi

};

namespace Ui {
    class isotopesWidget: public Ui_isotopesWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ISOTOPESWIDGET_H
