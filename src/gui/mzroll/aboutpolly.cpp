#include "aboutpolly.h"
#include "ui_aboutpolly.h"
#include <QMessageBox>

AboutPolly::AboutPolly() :
    QDialog(),
    ui(new Ui::AboutPolly)
    
{
    ui->setupUi(this);

    ui->label_3->setOpenExternalLinks(true);
    ui->label_2->setOpenExternalLinks(true);

    setWindowTitle("About Polly");
}

AboutPolly::~AboutPolly()
{
    delete ui;
}

void AboutPolly::cancel(){
    close();
}
