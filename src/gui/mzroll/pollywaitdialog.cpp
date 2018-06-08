#include "pollywaitdialog.h"

PollyWaitDialog::PollyWaitDialog(QWidget *parent) : QDialog(parent) {
    setupUi(this);
    statusLabel->setStyleSheet("QLabel {color : green; }");

    movie = new QMovie(":/images/loading.gif");
    movie->start();

    setWindowTitle("Please Wait..");
}

PollyWaitDialog::~PollyWaitDialog()
{
    delete ui;
}
