#include "gettingstarted.h"
#include "ui_gettingstarted.h"

GettingStarted::GettingStarted(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GettingStarted)
{
    ui->setupUi(this);
}

GettingStarted::~GettingStarted()
{
    delete ui;
}
