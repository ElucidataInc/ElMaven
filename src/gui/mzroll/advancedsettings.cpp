#include "advancedsettings.h"
#include <QMessageBox>

AdvancedSettings::AdvancedSettings() :
    QDialog(),
    ui(new Ui::AdvancedSettings)
    
{
    ui->setupUi(this);
    connect(ui->checkBox_upload_compond_DB,SIGNAL(clicked(bool)),SLOT(showCompoundDBUploadFrame()));
    connect(ui->checkBox_upload_Peak_Table,SIGNAL(clicked(bool)),SLOT(showpeakTableFrame()));
}


AdvancedSettings::~AdvancedSettings()
{
    delete ui;
}

void AdvancedSettings::showCompoundDBUploadFrame(){
    if (ui->upload_compound_DB_frame->isEnabled()){
        ui->upload_compound_DB_frame->setEnabled(false); 
    }
    else{
       ui->upload_compound_DB_frame->setEnabled(true); 
    }  
}

void AdvancedSettings::showpeakTableFrame(){
    if (ui->upload_peaks_frame->isEnabled()){
        ui->upload_peaks_frame->setEnabled(false); 
    }
    else{
       ui->upload_peaks_frame->setEnabled(true); 
    }  
}

void AdvancedSettings::initialSetup(){
    ui->comboBox_export_table->clear();
    ui->comboBox_export_format->clear();
    ui->comboBox_export_table->addItems(QStringList()<<"Export Selected"<<"Export All Groups"<<"Export Good"<<"Export Bad");

    QString groupsSTAB = "Groups Summary Matrix Format With Set Names (*.tab)";
    QString groupsTAB = "Groups Summary Matrix Format (*.tab)";    
    QString peaksTAB =  "Peaks Detailed Format (*.tab)";
    QString groupsSCSV = "Groups Summary Matrix Format Comma Delimited With Set Names (*.csv)";
    QString groupsCSV = "Groups Summary Matrix Format Comma Delimited (*.csv)";
    QString peaksCSV =  "Peaks Detailed Format Comma Delimited (*.csv)";

    ui->comboBox_export_format->addItems(QStringList()<<groupsSTAB<<groupsTAB<<peaksTAB<<groupsSCSV<<groupsCSV<<peaksCSV);
    
    populate_comboBox_compound_db();
    show();
}
void AdvancedSettings::populate_comboBox_compound_db() {
    ui->comboBox_compound_db->clear();
	QSet<QString>set;
	for(int i=0; i< DB.compoundsDB.size(); i++) {
        if (! set.contains( DB.compoundsDB[i]->db.c_str() ) )
            set.insert( DB.compoundsDB[i]->db.c_str() );
	}
    QIcon icon(":/images/dbsearch.png");
    QSetIterator<QString> i(set);
    int pos=0;
	while (i.hasNext()) { 
        ui->comboBox_compound_db->addItem(icon,i.next());
	}
}

QVariantMap AdvancedSettings::get_ui_elements(){
    QVariantMap ui_elements;
    ui_elements["export_option"]=ui->comboBox_export_table->currentText();
    ui_elements["export_format"]=ui->comboBox_export_format->currentText();
    ui_elements["user_filename"]=ui->lineEdit_filename->text();
    ui_elements["compound_db"]=ui->comboBox_compound_db->currentText();
    ui_elements["user_compound_DB_name"]=ui->lineEdit_compound_DB_name->text();

    return ui_elements;
}

bool AdvancedSettings::get_upload_compoundDB(){
    return ui->comboBox_compound_db->isEnabled();
}
bool AdvancedSettings::get_upload_Peak_Table(){
    return ui->checkBox_upload_Peak_Table->isEnabled();
}

void AdvancedSettings::cancel(){
    close();
}
