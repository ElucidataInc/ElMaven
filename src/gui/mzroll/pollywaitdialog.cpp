#include "pollywaitdialog.h"

PollyWaitDialog::PollyWaitDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PollyWaitDialog) {
    setupUi(this);
    statusLabel->setStyleSheet("QLabel {color : green; }");
    statusLabel->setAlignment(Qt::AlignCenter);

    movie = new QMovie(":/images/loading.gif");
    movie->start();

    //setWindowTitle("Please Wait..");
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

PollyWaitDialog::~PollyWaitDialog()
{
    delete ui;
}

void PollyWaitDialog::keyPressEvent(QKeyEvent* e)
{
    if(e->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(e);
}
