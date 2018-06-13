/********************************************************************************
** Form generated from reading UI file 'peptidefragmentation.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PEPTIDEFRAGMENTATION_H
#define UI_PEPTIDEFRAGMENTATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
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

class Ui_PaptideFragmentationWidget
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label;
    QLineEdit *peptideSequence;
    QLineEdit *precursorMZ;
    QPushButton *computeButton;
    QTableWidget *mTable;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_2;
    QDoubleSpinBox *charge;
    QLabel *label_3;
    QDoubleSpinBox *resolution;
    QLabel *label_5;
    QLabel *label_4;
    QComboBox *fragmenationType;

    void setupUi(QDockWidget *PaptideFragmentationWidget)
    {
        if (PaptideFragmentationWidget->objectName().isEmpty())
            PaptideFragmentationWidget->setObjectName(QStringLiteral("PaptideFragmentationWidget"));
        PaptideFragmentationWidget->resize(548, 331);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(14);
        sizePolicy.setHeightForWidth(PaptideFragmentationWidget->sizePolicy().hasHeightForWidth());
        PaptideFragmentationWidget->setSizePolicy(sizePolicy);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label = new QLabel(dockWidgetContents);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_5->addWidget(label);

        peptideSequence = new QLineEdit(dockWidgetContents);
        peptideSequence->setObjectName(QStringLiteral("peptideSequence"));

        horizontalLayout_5->addWidget(peptideSequence);

        precursorMZ = new QLineEdit(dockWidgetContents);
        precursorMZ->setObjectName(QStringLiteral("precursorMZ"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(precursorMZ->sizePolicy().hasHeightForWidth());
        precursorMZ->setSizePolicy(sizePolicy1);
        precursorMZ->setReadOnly(true);

        horizontalLayout_5->addWidget(precursorMZ);

        computeButton = new QPushButton(dockWidgetContents);
        computeButton->setObjectName(QStringLiteral("computeButton"));

        horizontalLayout_5->addWidget(computeButton);


        verticalLayout->addLayout(horizontalLayout_5);

        mTable = new QTableWidget(dockWidgetContents);
        mTable->setObjectName(QStringLiteral("mTable"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(mTable->sizePolicy().hasHeightForWidth());
        mTable->setSizePolicy(sizePolicy2);
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

        charge = new QDoubleSpinBox(dockWidgetContents);
        charge->setObjectName(QStringLiteral("charge"));
        charge->setDecimals(0);
        charge->setMinimum(1);
        charge->setMaximum(10000);

        horizontalLayout_4->addWidget(charge);

        label_3 = new QLabel(dockWidgetContents);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_4->addWidget(label_3);

        resolution = new QDoubleSpinBox(dockWidgetContents);
        resolution->setObjectName(QStringLiteral("resolution"));
        resolution->setDecimals(3);
        resolution->setMinimum(0.001);
        resolution->setMaximum(10000);
        resolution->setSingleStep(0.1);
        resolution->setValue(0.35);

        horizontalLayout_4->addWidget(resolution);

        label_5 = new QLabel(dockWidgetContents);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_4->addWidget(label_5);

        label_4 = new QLabel(dockWidgetContents);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_4->addWidget(label_4);

        fragmenationType = new QComboBox(dockWidgetContents);
        fragmenationType->setObjectName(QStringLiteral("fragmenationType"));

        horizontalLayout_4->addWidget(fragmenationType);


        verticalLayout->addLayout(horizontalLayout_4);

        PaptideFragmentationWidget->setWidget(dockWidgetContents);

        retranslateUi(PaptideFragmentationWidget);

        QMetaObject::connectSlotsByName(PaptideFragmentationWidget);
    } // setupUi

    void retranslateUi(QDockWidget *PaptideFragmentationWidget)
    {
        PaptideFragmentationWidget->setWindowTitle(QApplication::translate("PaptideFragmentationWidget", "Peptide Fragmentation", 0));
        label->setText(QApplication::translate("PaptideFragmentationWidget", "Sequence", 0));
        computeButton->setText(QApplication::translate("PaptideFragmentationWidget", "Match", 0));
        label_2->setText(QApplication::translate("PaptideFragmentationWidget", "Charge", 0));
        label_3->setText(QApplication::translate("PaptideFragmentationWidget", "Resolution", 0));
        resolution->setSuffix(QApplication::translate("PaptideFragmentationWidget", " amu", 0));
        label_5->setText(QApplication::translate("PaptideFragmentationWidget", "Fragmentation", 0));
        label_4->setText(QString());
        fragmenationType->clear();
        fragmenationType->insertItems(0, QStringList()
         << QApplication::translate("PaptideFragmentationWidget", "CID", 0)
         << QApplication::translate("PaptideFragmentationWidget", "ETD", 0)
         << QString()
        );
    } // retranslateUi

};

namespace Ui {
    class PaptideFragmentationWidget: public Ui_PaptideFragmentationWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PEPTIDEFRAGMENTATION_H
