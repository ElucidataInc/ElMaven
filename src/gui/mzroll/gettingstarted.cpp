#include "gettingstarted.h"
#include "ui_gettingstarted.h"
#define _STR(X) #X
#define STR(X) _STR(X)



GettingStarted::GettingStarted(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GettingStarted)
    {
    
    QSettings settings("mzRoll", "Application Settings");

        ui->setupUi(this);
    setFixedSize(width(), height());
    ui->textBrowser_2->setOpenExternalLinks(true);
    ui->textBrowser->setOpenExternalLinks(true);
    setWindowTitle("Getting Started");
    
    if(!settings.contains(STR(EL_MAVEN_VERSION)))
            {
                GettingStarted::setVisible(true);
            } 
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(setFlag()));
    
    }


void GettingStarted::closeEvent(QCloseEvent* event)
{
    if(ui->checkBox->checkState())
        {
            showDialog();
        }
            
     QDialog::closeEvent(event);
}

GettingStarted::~GettingStarted()
    {
        delete ui;
    }

bool GettingStarted::showDialog()
{
    QSettings settings("mzRoll", "Application Settings");
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
    GettingStarted::close();
    }
