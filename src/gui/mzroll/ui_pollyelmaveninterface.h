/********************************************************************************
** Form generated from reading UI file 'pollyelmaveninterface.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POLLYELMAVENINTERFACE_H
#define UI_POLLYELMAVENINTERFACE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PollyElmavenInterfaceDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabwidget;
    QWidget *featureSelectionTab;
    QGridLayout *gridLayout_10;
    QGroupBox *groupBox;
    QLabel *label;
    QComboBox *comboBox_existing_projects;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_7;
    QLineEdit *lineEdit_new_project_name;
    QComboBox *comboBox_collaborators;
    QTextEdit *textEdit_analysis_description;
    QLabel *label_9;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_4;
    QPushButton *cancelButton_upload;
    QPushButton *computeButton_upload;
    QProgressBar *progressBar_upload;
    QLabel *statusText;
    QWidget *groupFilteringTab;
    QGroupBox *groupBox_2;
    QLabel *label_6;
    QComboBox *comboBox_load_projects;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_6;
    QProgressBar *progressBar_load_project;
    QLabel *statusText_3;
    QPushButton *cancelButton_load;
    QPushButton *pushButton_load;
    QLabel *label_load_status;
    QLabel *label_2;
    QLabel *label_5;
    QComboBox *comboBox_load_db;
    QComboBox *comboBox_load_settings;
    QLabel *label_welcome_load;

    void setupUi(QDialog *PollyElmavenInterfaceDialog)
    {
        if (PollyElmavenInterfaceDialog->objectName().isEmpty())
            PollyElmavenInterfaceDialog->setObjectName(QStringLiteral("PollyElmavenInterfaceDialog"));
        PollyElmavenInterfaceDialog->resize(573, 460);
        PollyElmavenInterfaceDialog->setSizeIncrement(QSize(0, 0));
        verticalLayout = new QVBoxLayout(PollyElmavenInterfaceDialog);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabwidget = new QTabWidget(PollyElmavenInterfaceDialog);
        tabwidget->setObjectName(QStringLiteral("tabwidget"));
        featureSelectionTab = new QWidget();
        featureSelectionTab->setObjectName(QStringLiteral("featureSelectionTab"));
        gridLayout_10 = new QGridLayout(featureSelectionTab);
        gridLayout_10->setSpacing(6);
        gridLayout_10->setContentsMargins(11, 11, 11, 11);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        groupBox = new QGroupBox(featureSelectionTab);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 40, 201, 31));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        comboBox_existing_projects = new QComboBox(groupBox);
        comboBox_existing_projects->setObjectName(QStringLiteral("comboBox_existing_projects"));
        comboBox_existing_projects->setGeometry(QRect(250, 40, 261, 31));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 160, 201, 31));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(250, 80, 67, 17));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(20, 110, 201, 31));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEdit_new_project_name = new QLineEdit(groupBox);
        lineEdit_new_project_name->setObjectName(QStringLiteral("lineEdit_new_project_name"));
        lineEdit_new_project_name->setGeometry(QRect(250, 110, 261, 31));
        comboBox_collaborators = new QComboBox(groupBox);
        comboBox_collaborators->setObjectName(QStringLiteral("comboBox_collaborators"));
        comboBox_collaborators->setGeometry(QRect(250, 160, 261, 31));
        textEdit_analysis_description = new QTextEdit(groupBox);
        textEdit_analysis_description->setObjectName(QStringLiteral("textEdit_analysis_description"));
        textEdit_analysis_description->setGeometry(QRect(250, 210, 261, 70));
        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(20, 210, 201, 31));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        groupBox_4 = new QGroupBox(groupBox);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 280, 501, 92));
        gridLayout_4 = new QGridLayout(groupBox_4);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        cancelButton_upload = new QPushButton(groupBox_4);
        cancelButton_upload->setObjectName(QStringLiteral("cancelButton_upload"));

        gridLayout_4->addWidget(cancelButton_upload, 4, 5, 1, 1);

        computeButton_upload = new QPushButton(groupBox_4);
        computeButton_upload->setObjectName(QStringLiteral("computeButton_upload"));

        gridLayout_4->addWidget(computeButton_upload, 4, 4, 1, 1);

        progressBar_upload = new QProgressBar(groupBox_4);
        progressBar_upload->setObjectName(QStringLiteral("progressBar_upload"));
        progressBar_upload->setValue(0);

        gridLayout_4->addWidget(progressBar_upload, 4, 3, 1, 1);

        statusText = new QLabel(groupBox_4);
        statusText->setObjectName(QStringLiteral("statusText"));

        gridLayout_4->addWidget(statusText, 3, 3, 1, 1);


        gridLayout_10->addWidget(groupBox, 1, 0, 1, 1);

        tabwidget->addTab(featureSelectionTab, QString());
        groupFilteringTab = new QWidget();
        groupFilteringTab->setObjectName(QStringLiteral("groupFilteringTab"));
        groupBox_2 = new QGroupBox(groupFilteringTab);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(9, 21, 538, 421));
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 50, 221, 31));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        comboBox_load_projects = new QComboBox(groupBox_2);
        comboBox_load_projects->setObjectName(QStringLiteral("comboBox_load_projects"));
        comboBox_load_projects->setGeometry(QRect(250, 50, 261, 31));
        groupBox_6 = new QGroupBox(groupBox_2);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setGeometry(QRect(10, 200, 501, 92));
        gridLayout_6 = new QGridLayout(groupBox_6);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        progressBar_load_project = new QProgressBar(groupBox_6);
        progressBar_load_project->setObjectName(QStringLiteral("progressBar_load_project"));
        progressBar_load_project->setValue(0);

        gridLayout_6->addWidget(progressBar_load_project, 4, 3, 1, 1);

        statusText_3 = new QLabel(groupBox_6);
        statusText_3->setObjectName(QStringLiteral("statusText_3"));

        gridLayout_6->addWidget(statusText_3, 3, 3, 1, 1);

        cancelButton_load = new QPushButton(groupBox_6);
        cancelButton_load->setObjectName(QStringLiteral("cancelButton_load"));

        gridLayout_6->addWidget(cancelButton_load, 4, 5, 1, 1);

        pushButton_load = new QPushButton(groupBox_6);
        pushButton_load->setObjectName(QStringLiteral("pushButton_load"));

        gridLayout_6->addWidget(pushButton_load, 4, 4, 1, 1);

        label_load_status = new QLabel(groupBox_2);
        label_load_status->setObjectName(QStringLiteral("label_load_status"));
        label_load_status->setGeometry(QRect(10, 300, 501, 81));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 130, 221, 21));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 180, 221, 16));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        comboBox_load_db = new QComboBox(groupBox_2);
        comboBox_load_db->setObjectName(QStringLiteral("comboBox_load_db"));
        comboBox_load_db->setGeometry(QRect(250, 120, 261, 31));
        comboBox_load_settings = new QComboBox(groupBox_2);
        comboBox_load_settings->setObjectName(QStringLiteral("comboBox_load_settings"));
        comboBox_load_settings->setGeometry(QRect(250, 170, 261, 31));
        label_welcome_load = new QLabel(groupFilteringTab);
        label_welcome_load->setObjectName(QStringLiteral("label_welcome_load"));
        label_welcome_load->setGeometry(QRect(9, 9, 133, 17));
        tabwidget->addTab(groupFilteringTab, QString());

        verticalLayout->addWidget(tabwidget);


        retranslateUi(PollyElmavenInterfaceDialog);

        tabwidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(PollyElmavenInterfaceDialog);
    } // setupUi

    void retranslateUi(QDialog *PollyElmavenInterfaceDialog)
    {
        PollyElmavenInterfaceDialog->setWindowTitle(QApplication::translate("PollyElmavenInterfaceDialog", "Polly Elmaven Interface", 0));
        groupBox->setTitle(QString());
        label->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Upload to existing project", 0));
        label_3->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Add collaborators ", 0));
        label_4->setText(QApplication::translate("PollyElmavenInterfaceDialog", "OR", 0));
        label_7->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Create new project", 0));
        label_9->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Analysis description", 0));
        cancelButton_upload->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Cancel", 0));
        computeButton_upload->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Upload", 0));
        statusText->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Status", 0));
        tabwidget->setTabText(tabwidget->indexOf(featureSelectionTab), QApplication::translate("PollyElmavenInterfaceDialog", "Upload to Polly", 0));
        groupBox_2->setTitle(QString());
        label_6->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Select project", 0));
        statusText_3->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Status", 0));
        cancelButton_load->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Cancel", 0));
        pushButton_load->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Load project", 0));
        label_load_status->setText(QString());
        label_2->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Load compound database", 0));
        label_5->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Load settings", 0));
        label_welcome_load->setText(QApplication::translate("PollyElmavenInterfaceDialog", "Welcome to Polly ", 0));
        tabwidget->setTabText(tabwidget->indexOf(groupFilteringTab), QApplication::translate("PollyElmavenInterfaceDialog", "Load from Polly", 0));
    } // retranslateUi

};

namespace Ui {
    class PollyElmavenInterfaceDialog: public Ui_PollyElmavenInterfaceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POLLYELMAVENINTERFACE_H
