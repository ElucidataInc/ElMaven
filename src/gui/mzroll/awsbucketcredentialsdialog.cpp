#include "awsbucketcredentialsdialog.h"

AwsBucketCredentialsDialog::AwsBucketCredentialsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AwsBucketCredentialsDialog)
{
    ui->setupUi(this);
    ui->warningStatus->setVisible(false);
    
}

AwsBucketCredentialsDialog::~AwsBucketCredentialsDialog()
{
    delete ui;
}

void AwsBucketCredentialsDialog::accept() {

    int emptyLabel=0;

    if (ui->bucketName->text().isEmpty()) {
        ui->warningStatus->setText("Bucket Name is Empty");
        ui->warningStatus->show();
        emptyLabel++;
    }

    if (ui->accessKey->text().isEmpty()) {
        ui->warningStatus->setText("Access Key is Empty");
        ui->warningStatus->show();
        emptyLabel++;
    }

    if (ui->secretKey->text().isEmpty()) {
        ui->warningStatus->setText("Secret Key is Empty");
        ui->warningStatus->show();
        emptyLabel++;
    }


    if (emptyLabel > 1) {
        ui->warningStatus->setText("More than one field is Empty");
        ui->warningStatus->show();
    }


    if (emptyLabel == 0) {
        settings->setValue("bucket_name", ui->bucketName->text());
        settings->setValue("access_key", ui->accessKey->text());
        settings->setValue("secret_key", ui->secretKey->text());
        QDialog::accept();
    }

}
