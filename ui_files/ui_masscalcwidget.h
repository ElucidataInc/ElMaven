/********************************************************************************
** Form generated from reading UI file 'masscalcwidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MASSCALCWIDGET_H
#define UI_MASSCALCWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MassCalcWidget
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_5;
    QLineEdit *lineEdit;
    QLabel *label;
    QTableWidget *mTable;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_2;
    QDoubleSpinBox *ionization;
    QLabel *label_3;
    QDoubleSpinBox *maxppmdiff;
    QPushButton *computeButton;

    void setupUi(QDockWidget *MassCalcWidget)
    {
        if (MassCalcWidget->objectName().isEmpty())
            MassCalcWidget->setObjectName(QString::fromUtf8("MassCalcWidget"));
        MassCalcWidget->resize(405, 325);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(14);
        sizePolicy.setHeightForWidth(MassCalcWidget->sizePolicy().hasHeightForWidth());
        MassCalcWidget->setSizePolicy(sizePolicy);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        lineEdit = new QLineEdit(dockWidgetContents);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout_5->addWidget(lineEdit);

        label = new QLabel(dockWidgetContents);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_5->addWidget(label);


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
        mTable->setObjectName(QString::fromUtf8("mTable"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(mTable->sizePolicy().hasHeightForWidth());
        mTable->setSizePolicy(sizePolicy1);
        mTable->setMinimumSize(QSize(0, 0));
        mTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        mTable->setAlternatingRowColors(false);
        mTable->setSortingEnabled(true);
        mTable->horizontalHeader()->setStretchLastSection(false);
        mTable->verticalHeader()->setStretchLastSection(false);

        verticalLayout->addWidget(mTable);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_2 = new QLabel(dockWidgetContents);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_4->addWidget(label_2);

        ionization = new QDoubleSpinBox(dockWidgetContents);
        ionization->setObjectName(QString::fromUtf8("ionization"));
        ionization->setDecimals(0);
        ionization->setMinimum(-99);

        horizontalLayout_4->addWidget(ionization);

        label_3 = new QLabel(dockWidgetContents);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_4->addWidget(label_3);

        maxppmdiff = new QDoubleSpinBox(dockWidgetContents);
        maxppmdiff->setObjectName(QString::fromUtf8("maxppmdiff"));
        maxppmdiff->setMaximum(10000);

        horizontalLayout_4->addWidget(maxppmdiff);

        computeButton = new QPushButton(dockWidgetContents);
        computeButton->setObjectName(QString::fromUtf8("computeButton"));

        horizontalLayout_4->addWidget(computeButton);


        verticalLayout->addLayout(horizontalLayout_4);

        MassCalcWidget->setWidget(dockWidgetContents);

        retranslateUi(MassCalcWidget);

        QMetaObject::connectSlotsByName(MassCalcWidget);
    } // setupUi

    void retranslateUi(QDockWidget *MassCalcWidget)
    {
        MassCalcWidget->setWindowTitle(QApplication::translate("MassCalcWidget", "Compound Search", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MassCalcWidget", "m/z", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = mTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MassCalcWidget", "Formula/Compound", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = mTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MassCalcWidget", "Mass", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = mTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("MassCalcWidget", "ppmDiff", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MassCalcWidget", "Ionization Mode", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MassCalcWidget", "ppm", 0, QApplication::UnicodeUTF8));
        computeButton->setText(QApplication::translate("MassCalcWidget", "Compute", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MassCalcWidget: public Ui_MassCalcWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MASSCALCWIDGET_H
