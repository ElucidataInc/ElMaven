#include "pollyelmaveninterface.h"
#include <string>
#include <QVariant>

PollyElmavenInterfaceSettings::PollyElmavenInterfaceSettings(PollyElmavenInterfaceDialog* dialog):pd(dialog)
{
    // automated feature detection settings
    // settings.insert("automatedDetection", QVariant::fromValue(pd->featureOptions));

    // //TODO: replace with massCutoffMerge
    // settings.insert("massCutoffMerge", QVariant::fromValue(pd->ppmStep));

    // settings.insert("rtStep", QVariant::fromValue(pd->rtStep));
    // settings.insert("mzMin", QVariant::fromValue(pd->mzMin));
    // settings.insert("mzMax", QVariant::fromValue(pd->mzMax));
    // settings.insert("rtMin", QVariant::fromValue(pd->rtMin));
    // settings.insert("rtMax", QVariant::fromValue(pd->rtMax));
    // settings.insert("minIntensity", QVariant::fromValue(pd->minIntensity));
    // settings.insert("maxIntensity", QVariant::fromValue(pd->maxIntensity));
    // settings.insert("chargeMax", QVariant::fromValue(pd->chargeMax));
    // settings.insert("chargeMin", QVariant::fromValue(pd->chargeMin));

    // // db search settings
    // settings.insert("dbDetection", QVariant::fromValue(pd->dbOptions));

    // settings.insert("compoundMassCutoffWindow", QVariant::fromValue(pd->compoundPPMWindow));

    // settings.insert("compoundRTWindow", QVariant::fromValue(pd->compoundRTWindow));
    // settings.insert("matchRt", QVariant::fromValue(pd->matchRt));
    // settings.insert("eicMaxGroups", QVariant::fromValue(pd->eicMaxGroups));

    // // fragmentation settings
    // settings.insert("matchFragmentationOptions", QVariant::fromValue(pd->matchFragmentatioOptions));

    // // isotope detection
    // settings.insert("reportIsotopesOptions", QVariant::fromValue(pd->reportIsotopesOptions));

    // // group filtering settings
    // settings.insert("minGroupIntensity", QVariant::fromValue(pd->minGroupIntensity));
    // settings.insert("peakQuantitation", QVariant::fromValue(pd->peakQuantitation));
    // settings.insert("quantileIntensity", QVariant::fromValue(pd->quantileIntensity));

    // settings.insert("minQuality", QVariant::fromValue(pd->doubleSpinBoxMinQuality));
    // settings.insert("quantileQuality", QVariant::fromValue(pd->quantileQuality));

    // settings.insert("sigBlankRatio", QVariant::fromValue(pd->sigBlankRatio));
    // settings.insert("quantileSignalBlankRatio", QVariant::fromValue(pd->quantileSignalBlankRatio));

    // settings.insert("sigBaselineRatio", QVariant::fromValue(pd->sigBaselineRatio));
    // settings.insert("quantileSignalBaselineRatio", QVariant::fromValue(pd->quantileSignalBaselineRatio));

    // settings.insert("minNoNoiseObs", QVariant::fromValue(pd->minNoNoiseObs));
    // settings.insert("minGoodGroupCount", QVariant::fromValue(pd->minGoodGroupCount));

    // /* special case: there is no Ui element defined inside Peaks dialog that can be used
    //  * to change/access massCutOfftype. the only way to change massCutofftype is to change it from mainWindow(top right corner).
    //  * PeakDetectionDialog::masCutOffType is a variable that stores the value of MassCutOfftype defined in mainWindow
    //  * Better would be to have a ui element that allows to change/access massCutoff from peaks dialog
    //  */
    // settings.insert("massCutoffType", QVariant::fromValue(&pd->massCutoffType));
}

void PollyElmavenInterfaceSettings::updatePollySettings(string key, string value)
{

    // if(settings.find(QString(key.c_str())) != settings.end() && !value.empty()) {


    //     const QVariant& v = settings[QString(key.c_str())];
    //     // convert the val to proper type;
    //     if(QString(v.typeName()).contains("QDoubleSpinBox"))
    //         v.value<QDoubleSpinBox*>()->setValue(std::stod(value));

    //     if(QString(v.typeName()).contains("QGroupBox"))
    //         v.value<QGroupBox*>()->setChecked(std::stod(value));


    //     if(QString(v.typeName()).contains("QCheckBox"))
    //         v.value<QCheckBox*>()->setChecked(std::stod(value));

    //     if(QString(v.typeName()).contains("QSpinBox"))
    //         v.value<QSpinBox*>()->setValue(std::stod(value));

    //     if(QString(v.typeName()).contains("QSlider"))
    //         v.value<QSlider*>()->setValue(std::stod(value));

    //     if(QString(v.typeName()).contains("QComboBox"))
    //         v.value<QComboBox*>()->setCurrentIndex(std::stoi(value));

    //     /* IMPORTANT
    //      * special case: only pd->massCutOfftype  and the places where it is used are updated here
    //      * there is no other Ui element that with  typeName as "QString".
    //      * Better solution is to have a Ui element in Peaks Dialog that can be used to
    //      * change/access massCutoff type
    //      */
    //     if(QString(v.typeName()).contains("QString")) {
    //         pd->massCutoffType = value.c_str();
    //         pd->getMainWindow()->massCutoffComboBox->setCurrentText(pd->massCutoffType);
    //     }


    // }

}

PollyElmavenInterfaceDialog::PollyElmavenInterfaceDialog(MainWindow* mw) :
        QDialog(mw),
        mainwindow(mw)
{
        setupUi(this);
        setModal(true);
        pdSettings = new PollyElmavenInterfaceSettings(this);
        _pollyIntegration = new PollyIntegration(mainwindow,this);
        connect(computeButton_upload, SIGNAL(clicked(bool)), SLOT(uploadDataToPolly()));
        // connect(this, SIGNAL(isVisible()), SLOT(initialSetup()));
        connect(cancelButton_upload, SIGNAL(clicked(bool)), SLOT(cancel()));
        connect(pushButton_load, SIGNAL(clicked(bool)), SLOT(loadDataFromPolly()));
        connect(cancelButton_load, SIGNAL(clicked(bool)), SLOT(cancel()));
        // connect(comboBox_load_projects, SIGNAL(clicked(bool)), SLOT(loadsettings()));
        // connect(this, &QDialog::rejected, this, &PollyElmavenInterfaceDialog::dialogRejected);
        // connect(this, &PollyElmavenInterfaceDialog::settingsChanged, pdSettings, &PollyElmavenInterfaceSettings::updatePollySettings);

}

// void PollyElmavenInterfaceDialog::showEvent( QShowEvent* event ) {
//     QWidget::showEvent( event );
//     initialSetup();
//     qDebug()<<"is visible now ?- "<<isVisible();
// }

void PollyElmavenInterfaceDialog::initialSetup()
{
        // QMessageBox msgBox(NULL);
        // msgBox.setWindowModality(Qt::NonModal);
        // msgBox.setWindowTitle("Connecting to polly..");
        // msgBox.show();
        // qDebug()<<"yoyo";
        // QProgressDialog progress("Getting data from Polly...", "Cancle", 0, 100, this);
        // progress.setWindowModality(Qt::WindowModal);
        // for (int i = 0; i < 1; i++) {
        //     _pollyIntegration->transferData();
        //     if (progress.wasCanceled())
        //         break;
        // }
        // progress.setValue(100);
        _pollyIntegration->transferData();
        qDebug()<<"outside tranfer data now..";
        
        // msgBox.close();
}

QVariantMap PollyElmavenInterfaceDialog::loadFormData(){
    QMessageBox msgBox(NULL);
    msgBox.setWindowModality(Qt::NonModal);
    msgBox.setWindowTitle("getting data from polly..");
    msgBox.show();
    mainwindow->check_polly_login->setText("connected");
    mainwindow->check_polly_login->setStyleSheet("QLabel { background-color : white; color : green; }");
    comboBox_collaborators->clear();
    comboBox_load_projects->clear();
    comboBox_existing_projects->clear();
    progressBar_upload->setValue(0);
    progressBar_load_project->setValue(0);
    projectnames_id = _pollyIntegration->getUserProjects();    
    QStringList keys= projectnames_id.keys();

    userProjectFilesMap = _pollyIntegration->getUserProjectFiles(keys);
    qDebug()<<"userProjectFiles    "<<userProjectFilesMap<<endl;
    
    for (int i=0; i < keys.size(); ++i){
        comboBox_existing_projects->addItem(projectnames_id[keys.at(i)].toString());
        comboBox_load_projects->addItem(projectnames_id[keys.at(i)].toString());
    }
    comboBox_collaborators->addItem("Aman");
    comboBox_collaborators->addItem("Sabu");
    comboBox_collaborators->addItem("Swetabh");
    comboBox_collaborators->addItem("Nikita");
    comboBox_collaborators->addItem("Sahil");
    // msgBox.close();
    return projectnames_id;
}

void PollyElmavenInterfaceDialog::on_comboBox_load_projects_activated(const QString &arg1)
{
    comboBox_load_db->clear();
    comboBox_load_settings->clear();
    // QVariantMap projectnames_id = _pollyIntegration->getUserProjects();
    QStringList keys= projectnames_id.keys();
    QString projectname = comboBox_load_projects->currentText();
    QString ProjectId;
    for (int i=0; i < keys.size(); ++i){
        if (projectnames_id[keys.at(i)].toString()==projectname){
            ProjectId= keys.at(i);
        }
    }
    QStringList userProjectFiles= userProjectFilesMap[ProjectId].toStringList();
    
    // comboBox_load_settings->addItem("CustomSettings1");
    // comboBox_load_db->addItem("CustomDB1");
    for (int i=0; i < userProjectFiles.size(); ++i){
        QString filename = userProjectFiles.at(i);
        qDebug()<<"filename  "<<filename;
        qDebug()<<"filename.split('.')[filename.split('.').size()-1]   - "<<filename.split('.')[filename.split('.').size()-1];
        if (filename.split('.')[filename.split('.').size()-1]=="xml"){
            comboBox_load_settings->addItem(filename);
        }
        else{
            comboBox_load_db->addItem(filename);
        }
    }
}

void PollyElmavenInterfaceDialog::uploadDataToPolly()
{   
    QVariantMap projectnames_id = _pollyIntegration->getUserProjects();
    qDebug()<<"inside uploadDataToPolly.....uploading data to polly now.."<<endl;
    QString new_project_id;
    QString new_projectname = lineEdit_new_project_name->text();
    QString projectname = comboBox_existing_projects->currentText();
    QString project_id;
    // QProgressDialog progress("Copying files...", "Abort Copy", 0, numFiles, this);
    // progress.setWindowModality(Qt::WindowModal);

    // for (int i = 0; i < numFiles; i++) {
    //     progress.setValue(i);

    //     if (progress.wasCanceled())
    //         break;
    //     //... copy one file
    // }
    // progress.setValue(numFiles);
    progressBar_upload->setValue(50);
    if (new_projectname==""){
        QStringList keys= projectnames_id.keys();
        for (int i=0; i < keys.size(); ++i){
            if (projectnames_id[keys.at(i)].toString()==projectname){
                project_id= keys.at(i);
            }
        }
            new_project_id = _pollyIntegration->exportData(projectname,project_id);
        }
    else{
        new_project_id = _pollyIntegration->exportData(new_projectname,project_id);       
    }
    qDebug()<<"result of exportdata...."<<new_project_id;
    progressBar_upload->setValue(100);
    if (new_project_id==QString("nullptr")){
        QMessageBox msgBox(mainwindow);
        msgBox.setWindowTitle("No peaks found");
        msgBox.exec();
        return;
    }
    if (new_project_id!=QString("None")){
        QString redirection_url = QString("<a href='https://polly.elucidata.io/main#project=%1&auto-redirect=firstview'>Go To Polly</a>").arg(new_project_id);
        qDebug()<<"redirection_url     - "<<redirection_url;
        QMessageBox msgBox(mainwindow);
        msgBox.setWindowTitle("Redirecting to polly..");
        msgBox.setTextFormat(Qt::RichText);   //this is what makes the links clickable
        msgBox.setText(redirection_url);
        msgBox.exec();
    }
    else{
        QMessageBox msgBox(mainwindow);
        msgBox.setText("Unable to upload data.");
        msgBox.exec();
    }
}
void PollyElmavenInterfaceDialog::loadDataFromPolly()
{
    QStringList filenames;
    QString db_file = comboBox_load_db->currentText();
    QString settings_file = comboBox_load_settings->currentText();
    filenames.append(db_file);
    filenames.append(settings_file);
    QStringList keys= projectnames_id.keys();
    QString projectname = comboBox_load_projects->currentText();
    QString ProjectId;
    for (int i=0; i < keys.size(); ++i){
        if (projectnames_id[keys.at(i)].toString()==projectname){
            ProjectId= keys.at(i);
        }
    }
    QString run_id = _pollyIntegration->loadDataFromPolly(ProjectId,filenames);
    // QString fileName = QString(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QDir::separator() + "lastRun.xml");
    qDebug()<<"outside _pollyIntegration->loadDataFromPolly now..";
    // mainwindow->loadPollySettings(settings_file);
    // update maven peak settings whenever we close the dilaog box or click on 'cancel' button. 
    // cancel in turn calls close();
    // emit updateSettings(pdSettings);
    progressBar_load_project->setValue(100);
}
// void PollyElmavenInterfaceDialog::closeEvent(QCloseEvent* event)
// {
    // update maven peak settings whenever we close the dilaog box or click on 'cancel' button. 
    // cancel in turn calls close();
    // emit updateSettings(pdSettings);
// }
void PollyElmavenInterfaceDialog::cancel() {
    LOGD;
    close();   
}
// void PollyElmavenInterfaceDialog::dialogRejected() {
//     LOGD;
    
// }