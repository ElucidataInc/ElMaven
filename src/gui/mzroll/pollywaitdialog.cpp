#include "pollywaitdialog.h"

PollyWaitDialog::PollyWaitDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PollyWaitDialog) {
    setupUi(this);
    statusLabel->setStyleSheet("QLabel {color : green; }");
    statusLabel->setAlignment(Qt::AlignCenter);

    movie = new QMovie(":/images/loading.gif");
    movie->start();

    setWindowTitle("Please Wait..");
}

PollyWaitDialog::~PollyWaitDialog()
{
    delete ui;
}
