#include "pollywaitdialog.h"
#include "ui_pollywaitdialog.h"

PollyWaitDialog::PollyWaitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PollyWaitDialog)
{
    ui->setupUi(this);

    statusLabel = ui->statusLabel;
}

PollyWaitDialog::~PollyWaitDialog()
{
    delete ui;
}
