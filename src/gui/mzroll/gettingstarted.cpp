#include "gettingstarted.h"
#include <QSettings>
#include "ui_gettingstarted.h"
#define _STR(X) #X
#define STR(X) _STR(X)

GettingStarted::GettingStarted(QWidget *parent) : QDialog(parent),ui(new Ui::GettingStarted)
{



    ui->setupUi(this);
    setFixedSize(width(), height());
    ui->textBrowser_2->setOpenExternalLinks(true);
    ui->textBrowser->setOpenExternalLinks(true);
    setWindowTitle("Getting Started");

    QSettings settings("mzRoll", "Application Settings");

    if(settings.value(STR(EL_MAVEN_VERSION)).toBool()) {
        ui->checkBox->setChecked(true);
        ui->checkBox->setEnabled(false);
    }

    connect(ui->checkBox, &QCheckBox::stateChanged, this, &GettingStarted::setFlag);
    connect(ui->closeButton, &QPushButton::clicked, this, &GettingStarted::close);
}


GettingStarted::~GettingStarted()
{
    delete ui;
}

void GettingStarted::showDialog()
{
    QSettings settings("mzRoll", "Application Settings");
    if (!settings.value(STR(EL_MAVEN_VERSION), false).toBool()) {
        show();
    }
}

void GettingStarted::setFlag(int state)
{

    qDebug() << "state : " << state;
    QSettings settings("mzRoll", "Application Settings");
    settings.setValue(STR(EL_MAVEN_VERSION), state);

}
