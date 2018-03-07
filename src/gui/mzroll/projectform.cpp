#include "projectform.h"
#include "ui_projectform.h"
#include <QMessageBox>


ProjectForm::ProjectForm(MainWindow *parent,PollyIntegration* pollyintegration) :
    QDialog(parent),
    ui(new Ui::ProjectForm)
{
    ui->setupUi(this);
    projectnames_id = _pollyintegration->getUserProjects();
    QStringList keys= projectnames_id.keys();
    for (int i=0; i < keys.size(); ++i){
            ui->comboBox_1->addItem(projectnames_id[keys.at(i)].toString());
    }
    _mainwindow=parent;
    _pollyintegration = pollyintegration;
    
}

ProjectForm::~ProjectForm()
{
    delete ui;
}

void ProjectForm::on_pushButton_clicked()
{
    QString new_project_id;
    QString new_projectname = ui->lineEdit_projectname->text();
    projectname = ui->comboBox_1->currentText();
    QString project_id;
    hide();
    QMessageBox msgBox(this);
    msgBox.setWindowModality(Qt::NonModal);
    msgBox.setWindowTitle("Uploading to polly..");
    msgBox.setTextFormat(Qt::RichText);   //this is what makes the links clickable
    msgBox.setText("uploading..please wait..");
    msgBox.show();

    if (new_projectname==""){
        ui->status_label->setStyleSheet("QLabel {color : green; }");
        ui->status_label->setText("Uploading files to new project..");
    
        QStringList keys= projectnames_id.keys();
        for (int i=0; i < keys.size(); ++i){
            if (projectnames_id[keys.at(i)].toString()==projectname){
                project_id= keys.at(i);
            }
        }
        new_project_id = _pollyintegration->exportData(projectname,project_id);
    }
    else{
        ui->status_label->setStyleSheet("QLabel {color : green; }");
        ui->status_label->setText("Uploading files to existing project..");
        new_project_id = _pollyintegration->exportData(new_projectname,project_id);       
    }
    
    if (new_project_id!=QString("None")){
        msgBox.close();
        QString redirection_url = QString("<a href='https://polly.elucidata.io/projects/%1#auto-redirect=firstview'>Go To Polly</a>").arg(new_project_id);
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Redirecting to polly..");
        msgBox.setTextFormat(Qt::RichText);   //this is what makes the links clickable
        msgBox.setText(redirection_url);
        msgBox.exec();
    }
    else{
        ui->status_label->setStyleSheet("QLabel {color : red; }");
        ui->status_label->setText("error in uploading files..");
    
    }
}
