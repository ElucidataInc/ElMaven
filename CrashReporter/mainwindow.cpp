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
     if (this->windowState == 1) {
        QProcess * myProcess = new QProcess();
        myProcess->start(this->restartApplicationPath);
     }
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
    QS3::Bucket* bucket = s3.bucket(this->bucketName, this->accessKey, this->secretKey);

    const QDateTime nowTime = QDateTime::currentDateTime(); 
	const QString timestamp = nowTime.toString(QLatin1String("yyyyMMdd-hhmmsszzz"));
	QString AutosavePath;
    QString userMessage;
    if (this->windowState == 1) {
        userMessage = "email: " + ui->lineEdit->text() + "\nmessage:" + ui->plainTextEdit->toPlainText() + "\n" + "log: " + this->logInformation;
        AutosavePath = timestamp + "_Crash.log";
    } else {
        userMessage = "email: " + ui->lineEdit->text() + "\nmessage:" + ui->plainTextEdit->toPlainText() + "\n";
        AutosavePath = timestamp + "_Feedback.log";
    }
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

void MainWindow::onStart()
{
    if (this->windowState != 1) {
        ui->pushButton->setVisible( false );
        ui->label_3->setText("E-mail");
        ui->label->setText("Description of feature/ bug");
        this->setWindowTitle ("Bug reporting and Feature request");
        ui->groupBox->setTitle("");
        ui->label_4->setText( "<b>Make El Maven better by giving us your feedback.</b>");
        ui->label_5->setText( "If you have a feature wish list or if you identified a bug please get in touch with us. A brief description below would help us a lot. We would love to hear from you." );
        ui->pushButton_3->setText("Submit Feedback");
    }
    this->show();
}