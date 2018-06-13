/********************************************************************************
** Form generated from reading UI file 'clusterdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLUSTERDIALOG_H
#define UI_CLUSTERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_ClusterDialog
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_6;
    QLabel *label_10;
    QLabel *label_14;
    QDoubleSpinBox *minSampleCorr;
    QLabel *label_15;
    QDoubleSpinBox *minRt;
    QDoubleSpinBox *maxRtDiff_2;
    QPushButton *clusterButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *clearButton;

    void setupUi(QDialog *ClusterDialog)
    {
        if (ClusterDialog->objectName().isEmpty())
            ClusterDialog->setObjectName(QStringLiteral("ClusterDialog"));
        ClusterDialog->resize(378, 174);
        gridLayout = new QGridLayout(ClusterDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        groupBox_5 = new QGroupBox(ClusterDialog);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        gridLayout_6 = new QGridLayout(groupBox_5);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        label_10 = new QLabel(groupBox_5);
        label_10->setObjectName(QStringLiteral("label_10"));

        gridLayout_6->addWidget(label_10, 0, 0, 1, 1);

        label_14 = new QLabel(groupBox_5);
        label_14->setObjectName(QStringLiteral("label_14"));

        gridLayout_6->addWidget(label_14, 2, 0, 1, 1);

        minSampleCorr = new QDoubleSpinBox(groupBox_5);
        minSampleCorr->setObjectName(QStringLiteral("minSampleCorr"));
        minSampleCorr->setMinimum(-1);
        minSampleCorr->setMaximum(1);
        minSampleCorr->setSingleStep(0.1);
        minSampleCorr->setValue(0.6);

        gridLayout_6->addWidget(minSampleCorr, 2, 1, 1, 1);

        label_15 = new QLabel(groupBox_5);
        label_15->setObjectName(QStringLiteral("label_15"));

        gridLayout_6->addWidget(label_15, 3, 0, 1, 1);

        minRt = new QDoubleSpinBox(groupBox_5);
        minRt->setObjectName(QStringLiteral("minRt"));
        minRt->setMinimum(-1);
        minRt->setMaximum(1);
        minRt->setSingleStep(0.1);
        minRt->setValue(0.8);

        gridLayout_6->addWidget(minRt, 0, 1, 1, 1);

        maxRtDiff_2 = new QDoubleSpinBox(groupBox_5);
        maxRtDiff_2->setObjectName(QStringLiteral("maxRtDiff_2"));
        maxRtDiff_2->setMaximum(1000);
        maxRtDiff_2->setSingleStep(0.1);
        maxRtDiff_2->setValue(0.5);

        gridLayout_6->addWidget(maxRtDiff_2, 3, 1, 1, 1);


        gridLayout->addWidget(groupBox_5, 0, 0, 1, 3);

        clusterButton = new QPushButton(ClusterDialog);
        clusterButton->setObjectName(QStringLiteral("clusterButton"));

        gridLayout->addWidget(clusterButton, 2, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 2, 1, 1);

        clearButton = new QPushButton(ClusterDialog);
        clearButton->setObjectName(QStringLiteral("clearButton"));

        gridLayout->addWidget(clearButton, 2, 1, 1, 1);


        retranslateUi(ClusterDialog);

        QMetaObject::connectSlotsByName(ClusterDialog);
    } // setupUi

    void retranslateUi(QDialog *ClusterDialog)
    {
        ClusterDialog->setWindowTitle(QApplication::translate("ClusterDialog", "Cluster PeakGroups", 0));
        groupBox_5->setTitle(QApplication::translate("ClusterDialog", "Peak Group Clustering", 0));
        label_10->setText(QApplication::translate("ClusterDialog", "Peak Shape Correlation", 0));
        label_14->setText(QApplication::translate("ClusterDialog", "Sample Intensity  Correlation", 0));
        label_15->setText(QApplication::translate("ClusterDialog", "Group - Group Max Rt Difference", 0));
        maxRtDiff_2->setSuffix(QApplication::translate("ClusterDialog", " min", 0));
        clusterButton->setText(QApplication::translate("ClusterDialog", "Cluster", 0));
        clearButton->setText(QApplication::translate("ClusterDialog", "Clear Clusters", 0));
    } // retranslateUi

};

namespace Ui {
    class ClusterDialog: public Ui_ClusterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLUSTERDIALOG_H
