#include "gettingstarted.h"
#include "ui_gettingstarted.h"

GettingStarted::GettingStarted(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GettingStarted)
{
    ui->setupUi(this);
   ui->textBrowser->setOpenExternalLinks(true);
   setWindowTitle("Getting Started");
}

GettingStarted::~GettingStarted()
{
    delete ui;
}
