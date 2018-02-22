#include "projectform.h"
#include "ui_projectform.h"
#include <QMessageBox>


ProjectForm::ProjectForm(MainWindow *parent,PollyIntegration* pollyintegration) :
    QWidget(0),
    ui(new Ui::ProjectForm)
{
    ui->setupUi(this);
    projectnames_id = _pollyintegration->getUserProjects();
    // qDebug()<<"  projectnames_id "<<projectnames_id;
    // projectnames.append("2files_project1");
    // projectnames.append("2files_project2");
    QStringList keys= projectnames_id.keys();
    for (int i=0; i < keys.size(); ++i){
            ui->comboBox_1->addItem(projectnames_id[keys.at(i)].toString());
    }
    _mainwindow=parent;
    _pollyintegration = pollyintegration;
    
}

ProjectForm::~ProjectForm()
{
    qDebug()<<"inside ~ProjectForm now....";
    delete ui;
}

void ProjectForm::on_pushButton_clicked()
{
    QString new_project_id;
    QString new_projectname = ui->lineEdit_projectname->text();
    projectname = ui->comboBox_1->currentText();
    hide();
    QString project_id;
    if (new_projectname==""){
        QStringList keys= projectnames_id.keys();
        for (int i=0; i < keys.size(); ++i){
            if (projectnames_id[keys.at(i)].toString()==projectname){
                project_id= keys.at(i);
            }
        }
        new_project_id = _pollyintegration->exportData(projectname,project_id);
    }
    else{
        new_project_id = _pollyintegration->exportData(projectname,project_id);       
    }
    if (new_project_id!=QString("None")){
        QString redirection_url = QString("<a href=http://testpolly.elucidata.io/projects/%1>Go To Polly</a>").arg(new_project_id);
        QMessageBox::information(this,"Data uploaded..redirecting to polly",redirection_url);
    }
}
