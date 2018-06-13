/********************************************************************************
** Form generated from reading UI file 'awsbucketcredentialsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AWSBUCKETCREDENTIALSDIALOG_H
#define UI_AWSBUCKETCREDENTIALSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_AwsBucketCredentialsDialog
{
public:
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QLabel *label_3;
    QLineEdit *bucketName;
    QLineEdit *accessKey;
    QLineEdit *secretKey;
    QLabel *warningStatus;

    void setupUi(QDialog *AwsBucketCredentialsDialog)
    {
        if (AwsBucketCredentialsDialog->objectName().isEmpty())
            AwsBucketCredentialsDialog->setObjectName(QStringLiteral("AwsBucketCredentialsDialog"));
        AwsBucketCredentialsDialog->resize(476, 333);
        gridLayout_2 = new QGridLayout(AwsBucketCredentialsDialog);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        groupBox = new QGroupBox(AwsBucketCredentialsDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        buttonBox = new QDialogButtonBox(groupBox);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 4, 2, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        bucketName = new QLineEdit(groupBox);
        bucketName->setObjectName(QStringLiteral("bucketName"));

        gridLayout->addWidget(bucketName, 1, 1, 1, 2);

        accessKey = new QLineEdit(groupBox);
        accessKey->setObjectName(QStringLiteral("accessKey"));

        gridLayout->addWidget(accessKey, 2, 1, 1, 2);

        secretKey = new QLineEdit(groupBox);
        secretKey->setObjectName(QStringLiteral("secretKey"));

        gridLayout->addWidget(secretKey, 3, 1, 1, 2);

        warningStatus = new QLabel(groupBox);
        warningStatus->setObjectName(QStringLiteral("warningStatus"));
        warningStatus->setMinimumSize(QSize(250, 0));
        warningStatus->setStyleSheet(QStringLiteral("QLabel { color : red; }"));

        gridLayout->addWidget(warningStatus, 4, 0, 1, 2);


        gridLayout_2->addWidget(groupBox, 0, 0, 1, 1);


        retranslateUi(AwsBucketCredentialsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), AwsBucketCredentialsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), AwsBucketCredentialsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(AwsBucketCredentialsDialog);
    } // setupUi

    void retranslateUi(QDialog *AwsBucketCredentialsDialog)
    {
        AwsBucketCredentialsDialog->setWindowTitle(QApplication::translate("AwsBucketCredentialsDialog", "AWS S3 Bucket Credentials", 0));
        groupBox->setTitle(QString());
        label_2->setText(QApplication::translate("AwsBucketCredentialsDialog", "Access Key", 0));
        label->setText(QApplication::translate("AwsBucketCredentialsDialog", "Bucket Name", 0));
        label_3->setText(QApplication::translate("AwsBucketCredentialsDialog", "Secret Key", 0));
        warningStatus->setText(QApplication::translate("AwsBucketCredentialsDialog", "Status", 0));
    } // retranslateUi

};

namespace Ui {
    class AwsBucketCredentialsDialog: public Ui_AwsBucketCredentialsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AWSBUCKETCREDENTIALSDIALOG_H
