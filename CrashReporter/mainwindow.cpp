#include "mainwindow.h"




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->restartApplicationPath = "";
}

MainWindow::~MainWindow()
{
    delete ui;
}

 void MainWindow::startElMaven()
 {
     QProcess * myProcess = new QProcess();
     myProcess->start(this->restartApplicationPath);
     QCoreApplication::quit();
 }

void MainWindow::on_pushButton_clicked()
{
    this->startElMaven();
}

void MainWindow::on_pushButton_3_clicked()
{
    QString s3Host = "s3.amazonaws.com";

    QS3::S3 s3(s3Host);
    QS3::Bucket* bucket = s3.bucket("elmaven", "AKIAI74WRZJ6ZHGJIQRA", "U4NHyMGkoq+yJ7n8ix5pEaW5v0Ig8IGyEQYRkIxK");

    const QDateTime nowTime = QDateTime::currentDateTime(); 
	const QString timestamp = nowTime.toString(QLatin1String("yyyyMMdd-hhmmsszzz"));
	QString AutosavePath = timestamp + ".log";

    QString userMessage = "email: " + ui->lineEdit->text() + "\nmessage:" + ui->plainTextEdit->toPlainText() + "\n";
    QByteArray data(userMessage.toUtf8());
    bucket->upload(AutosavePath, data);

    QEventLoop loop;
    connect(bucket, SIGNAL(finished()), this, SLOT(uploadToS3Done()));
    loop.exec();


}

void MainWindow::on_pushButton_2_clicked()
{
    QCoreApplication::quit();
}

void MainWindow::uploadToS3Done() {
    ui->plainTextEdit->clear();
    ui->lineEdit->clear();
    ui->statusBar->showMessage("Report is sent. Our team will get back to you as soon as possible.");
    this->startElMaven();
}
