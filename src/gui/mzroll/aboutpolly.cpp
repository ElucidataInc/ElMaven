#include "aboutpolly.h"
#include "ui_aboutpolly.h"
#include <QMessageBox>

AboutPolly::AboutPolly() :
    QDialog(),
    ui(new Ui::AboutPolly)
    
{
    ui->setupUi(this);
    ui->label_video_link->setText("<a href=\"https://www.youtube.com/watch?v=tfxksBKWTS0\">Video link</a>");
    ui->label_video_link->setTextFormat(Qt::RichText);
    ui->label_video_link->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_video_link->setOpenExternalLinks(true);

    ui->label_visit_polly->setText("<a href=\"https://polly.elucidata.io\">Visit our website:Polly</a>");
    ui->label_visit_polly->setTextFormat(Qt::RichText);
    ui->label_visit_polly->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_visit_polly->setOpenExternalLinks(true);
    QPixmap pix( ":/images/POLLY.png");
    ui->label_polly_logo->setPixmap(pix);
    ui->label_polly_logo->setGeometry(220,10, 61,51);

}

AboutPolly::~AboutPolly()
{
    delete ui;
}

void AboutPolly::cancel(){
    qDebug()<<"closing the info tab now..";
    close();
}
