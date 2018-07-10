/********************************************************************************
** Form generated from reading UI file 'advancedsettings.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADVANCEDSETTINGS_H
#define UI_ADVANCEDSETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AdvancedSettings
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabwidget;
    QWidget *advanced_options;
    QGroupBox *groupBox;
    QFrame *upload_peaks_frame;
    QLabel *label_10;
    QComboBox *comboBox_export_table;
    QLabel *label_11;
    QComboBox *comboBox_export_format;
    QLabel *label_13;
    QLineEdit *lineEdit_filename;
    QLabel *label_3;
    QCheckBox *checkBox_upload_compond_DB;
    QFrame *upload_compound_DB_frame;
    QLabel *label_23;
    QComboBox *comboBox_compound_db;
    QLabel *label_14;
    QLineEdit *lineEdit_compound_DB_name;
    QLabel *upload_status;
    QCheckBox *checkBox_upload_Peak_Table;

    void setupUi(QDialog *AdvancedSettings)
    {
        if (AdvancedSettings->objectName().isEmpty())
            AdvancedSettings->setObjectName(QStringLiteral("AdvancedSettings"));
        AdvancedSettings->resize(537, 493);
        AdvancedSettings->setSizeIncrement(QSize(0, 0));
        AdvancedSettings->setSizeGripEnabled(true);
        verticalLayout = new QVBoxLayout(AdvancedSettings);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabwidget = new QTabWidget(AdvancedSettings);
        tabwidget->setObjectName(QStringLiteral("tabwidget"));
        advanced_options = new QWidget();
        advanced_options->setObjectName(QStringLiteral("advanced_options"));
        groupBox = new QGroupBox(advanced_options);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(0, 0, 491, 431));
        upload_peaks_frame = new QFrame(groupBox);
        upload_peaks_frame->setObjectName(QStringLiteral("upload_peaks_frame"));
        upload_peaks_frame->setEnabled(false);
        upload_peaks_frame->setGeometry(QRect(40, 50, 441, 191));
        upload_peaks_frame->setAutoFillBackground(false);
        upload_peaks_frame->setFrameShape(QFrame::StyledPanel);
        upload_peaks_frame->setFrameShadow(QFrame::Plain);
        label_10 = new QLabel(upload_peaks_frame);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(10, 50, 111, 17));
        comboBox_export_table = new QComboBox(upload_peaks_frame);
        comboBox_export_table->setObjectName(QStringLiteral("comboBox_export_table"));
        comboBox_export_table->setGeometry(QRect(300, 40, 111, 25));
        label_11 = new QLabel(upload_peaks_frame);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(10, 90, 101, 17));
        comboBox_export_format = new QComboBox(upload_peaks_frame);
        comboBox_export_format->setObjectName(QStringLiteral("comboBox_export_format"));
        comboBox_export_format->setGeometry(QRect(300, 80, 111, 25));
        label_13 = new QLabel(upload_peaks_frame);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(10, 140, 121, 17));
        lineEdit_filename = new QLineEdit(upload_peaks_frame);
        lineEdit_filename->setObjectName(QStringLiteral("lineEdit_filename"));
        lineEdit_filename->setGeometry(QRect(210, 130, 201, 31));
        label_3 = new QLabel(upload_peaks_frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(70, 10, 281, 17));
        checkBox_upload_compond_DB = new QCheckBox(groupBox);
        checkBox_upload_compond_DB->setObjectName(QStringLiteral("checkBox_upload_compond_DB"));
        checkBox_upload_compond_DB->setGeometry(QRect(40, 260, 231, 23));
        upload_compound_DB_frame = new QFrame(groupBox);
        upload_compound_DB_frame->setObjectName(QStringLiteral("upload_compound_DB_frame"));
        upload_compound_DB_frame->setEnabled(false);
        upload_compound_DB_frame->setGeometry(QRect(40, 280, 441, 141));
        upload_compound_DB_frame->setFrameShape(QFrame::StyledPanel);
        upload_compound_DB_frame->setFrameShadow(QFrame::Raised);
        label_23 = new QLabel(upload_compound_DB_frame);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setGeometry(QRect(10, 30, 191, 17));
        comboBox_compound_db = new QComboBox(upload_compound_DB_frame);
        comboBox_compound_db->setObjectName(QStringLiteral("comboBox_compound_db"));
        comboBox_compound_db->setGeometry(QRect(230, 30, 201, 25));
        label_14 = new QLabel(upload_compound_DB_frame);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(10, 90, 151, 17));
        lineEdit_compound_DB_name = new QLineEdit(upload_compound_DB_frame);
        lineEdit_compound_DB_name->setObjectName(QStringLiteral("lineEdit_compound_DB_name"));
        lineEdit_compound_DB_name->setGeometry(QRect(230, 80, 201, 31));
        upload_status = new QLabel(groupBox);
        upload_status->setObjectName(QStringLiteral("upload_status"));
        upload_status->setGeometry(QRect(0, 390, 361, 17));
        checkBox_upload_Peak_Table = new QCheckBox(groupBox);
        checkBox_upload_Peak_Table->setObjectName(QStringLiteral("checkBox_upload_Peak_Table"));
        checkBox_upload_Peak_Table->setGeometry(QRect(40, 30, 431, 23));
        tabwidget->addTab(advanced_options, QString());

        verticalLayout->addWidget(tabwidget);


        retranslateUi(AdvancedSettings);

        tabwidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(AdvancedSettings);
    } // setupUi

    void retranslateUi(QDialog *AdvancedSettings)
    {
        AdvancedSettings->setWindowTitle(QApplication::translate("AdvancedSettings", "AdvancedSettings", Q_NULLPTR));
        groupBox->setTitle(QString());
        label_10->setText(QApplication::translate("AdvancedSettings", "Export Options", Q_NULLPTR));
        label_11->setText(QApplication::translate("AdvancedSettings", "Export Format", Q_NULLPTR));
        label_13->setText(QApplication::translate("AdvancedSettings", "File name", Q_NULLPTR));
        label_3->setText(QApplication::translate("AdvancedSettings", "Advanced Export Options For Peak Table", Q_NULLPTR));
        checkBox_upload_compond_DB->setText(QApplication::translate("AdvancedSettings", "Upload Compound Database", Q_NULLPTR));
        label_23->setText(QApplication::translate("AdvancedSettings", "Select Compound Database", Q_NULLPTR));
        label_14->setText(QApplication::translate("AdvancedSettings", "File name", Q_NULLPTR));
        upload_status->setText(QString());
        checkBox_upload_Peak_Table->setText(QApplication::translate("AdvancedSettings", "Peak Table Export", Q_NULLPTR));
        tabwidget->setTabText(tabwidget->indexOf(advanced_options), QApplication::translate("AdvancedSettings", "Advanced Settings", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AdvancedSettings: public Ui_AdvancedSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADVANCEDSETTINGS_H
