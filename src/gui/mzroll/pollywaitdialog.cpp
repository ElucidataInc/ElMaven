#include "pollywaitdialog.h"
#include "ui_pollywaitdialog.h"

PollyWaitDialog::PollyWaitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PollyWaitDialog)
{
    ui->setupUi(this);

    statusLabel = ui->statusLabel;
    label = ui->label;

    setWindowTitle("Please Wait..");
}

PollyWaitDialog::~PollyWaitDialog()
{
    delete ui;
}
