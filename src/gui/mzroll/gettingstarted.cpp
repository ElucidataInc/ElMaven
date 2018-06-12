#include "gettingstarted.h"
#include "ui_gettingstarted.h"
#define _STR(X) #X
#define STR(X) _STR(X)



GettingStarted::GettingStarted(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GettingStarted)
{
   settingsPath1 = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
   settingsPath = QDir::cleanPath(settingsPath1 + QDir::separator() + "El-MAVEN_gettingStarted.ini");
   QSettings settings(settingsPath, QSettings::IniFormat);

    ui->setupUi(this);
   setFixedSize(width(), height());
   ui->textBrowser->setOpenExternalLinks(true);
   setWindowTitle("Getting Started");
   
   if(!settings.contains(STR(EL_MAVEN_VERSION)))
   {
    GettingStarted::setVisible(true);
  } 
   connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(setFlag()));
}

GettingStarted::~GettingStarted()
{
    delete ui;
}

bool GettingStarted::showDialog()
{
    QSettings settings(settingsPath, QSettings::IniFormat);
    bool state;
    if (!settings.contains(STR(EL_MAVEN_VERSION)))
    {
       state = true;
       settings.setValue(STR(EL_MAVEN_VERSION), state);
       return true;
    }
    else
    {
       return false;
    }
    return false;
}

void GettingStarted::setFlag()
{
if(ui->checkBox->checkState())
{
    showDialog();
}
GettingStarted::close();
}
