/********************************************************************************
** Form generated from reading UI file 'masscalcwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MASSCALCWIDGET_H
#define UI_MASSCALCWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MassCalcWidget
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label;
    QLineEdit *lineEdit;
    QPushButton *computeButton;
    QTableWidget *mTable;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_2;
    QDoubleSpinBox *ionization;
    QLabel *label_3;
    QDoubleSpinBox *maxppmdiff;

    void setupUi(QDockWidget *MassCalcWidget)
    {
        if (MassCalcWidget->objectName().isEmpty())
            MassCalcWidget->setObjectName(QStringLiteral("MassCalcWidget"));
        MassCalcWidget->resize(420, 325);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(14);
        sizePolicy.setHeightForWidth(MassCalcWidget->sizePolicy().hasHeightForWidth());
        MassCalcWidget->setSizePolicy(sizePolicy);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label = new QLabel(dockWidgetContents);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_5->addWidget(label);

        lineEdit = new QLineEdit(dockWidgetContents);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        horizontalLayout_5->addWidget(lineEdit);

        computeButton = new QPushButton(dockWidgetContents);
        computeButton->setObjectName(QStringLiteral("computeButton"));

        horizontalLayout_5->addWidget(computeButton);


        verticalLayout->addLayout(horizontalLayout_5);

        mTable = new QTableWidget(dockWidgetContents);
        if (mTable->columnCount() < 3)
            mTable->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        mTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        mTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        mTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        mTable->setObjectName(QStringLiteral("mTable"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(mTable->sizePolicy().hasHeightForWidth());
        mTable->setSizePolicy(sizePolicy1);
        mTable->setMinimumSize(QSize(0, 0));
        mTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        mTable->setAlternatingRowColors(false);
        mTable->setSortingEnabled(true);
        mTable->horizontalHeader()->setVisible(true);
        mTable->horizontalHeader()->setDefaultSectionSize(100);
        mTable->horizontalHeader()->setMinimumSectionSize(100);
        mTable->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
        mTable->horizontalHeader()->setStretchLastSection(false);
        mTable->verticalHeader()->setStretchLastSection(false);

        verticalLayout->addWidget(mTable);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_2 = new QLabel(dockWidgetContents);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_4->addWidget(label_2);

        ionization = new QDoubleSpinBox(dockWidgetContents);
        ionization->setObjectName(QStringLiteral("ionization"));
        ionization->setDecimals(0);
        ionization->setMinimum(-99);

        horizontalLayout_4->addWidget(ionization);

        label_3 = new QLabel(dockWidgetContents);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_4->addWidget(label_3);

        maxppmdiff = new QDoubleSpinBox(dockWidgetContents);
        maxppmdiff->setObjectName(QStringLiteral("maxppmdiff"));
        maxppmdiff->setMaximum(10000);

        horizontalLayout_4->addWidget(maxppmdiff);


        verticalLayout->addLayout(horizontalLayout_4);

        MassCalcWidget->setWidget(dockWidgetContents);

        retranslateUi(MassCalcWidget);

        QMetaObject::connectSlotsByName(MassCalcWidget);
    } // setupUi

    void retranslateUi(QDockWidget *MassCalcWidget)
    {
        MassCalcWidget->setWindowTitle(QApplication::translate("MassCalcWidget", "Compound Search", 0));
        label->setText(QApplication::translate("MassCalcWidget", "PrecursorMz", 0));
        computeButton->setText(QApplication::translate("MassCalcWidget", "Match", 0));
        QTableWidgetItem *___qtablewidgetitem = mTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MassCalcWidget", "Compound", 0));
        QTableWidgetItem *___qtablewidgetitem1 = mTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MassCalcWidget", "m/z", 0));
        QTableWidgetItem *___qtablewidgetitem2 = mTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("MassCalcWidget", "PPM Diff", 0));
        label_2->setText(QApplication::translate("MassCalcWidget", "Ionization", 0));
        label_3->setText(QApplication::translate("MassCalcWidget", "ppm", 0));
    } // retranslateUi

};

namespace Ui {
    class MassCalcWidget: public Ui_MassCalcWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MASSCALCWIDGET_H
