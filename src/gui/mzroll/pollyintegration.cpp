#include "pollyintegration.h"
#include <QMessageBox>
#include <QObject>

PollyIntegration::PollyIntegration(TableDockWidget* tableDockWidget)
{

    _tableDockWidget = tableDockWidget;
    qDebug()<<"entering PollyIntegration now....";

    // connect(w, signal, this, slot())

}

PollyIntegration::~PollyIntegration()
{
    qDebug()<<"exiting PollyIntegration now....";
}





void PollyIntegration::exportGroupsToSpreadsheet_polly() {
    qDebug()<<"inside exportGroupsToSpreadsheet_polly now.......";
    // LOGD;
    // //Merged to Maven776 - Kiran
    // // CSVReports* csvreport = new CSVReports;
    // vector<mzSample*> samples = _mainwindow->getSamples();
    // CSVReports* csvreports = new CSVReports(samples);
    // csvreports->setMavenParameters(_mainwindow->mavenParameters);
    // if (allgroups.size() == 0 ) {
    //     QString msg = "Peaks Table is Empty";
    //     QMessageBox::warning(this, tr("Error"), msg);
    //     return;
    // }

    // QString dir = ".";
    // QSettings* settings = _mainwindow->getSettings();

    // if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    // QString groupsTAB = "Groups Summary Matrix Format (*.tab)";
    // QString groupsSTAB = "Groups Summary Matrix Format Without Set Name (*.tab)";    
    // QString peaksTAB =  "Peaks Detailed Format (*.tab)";
    // QString groupsCSV = "Groups Summary Matrix Format Comma Delimited (*.csv)";
    // QString groupsSCSV = "Groups Summary Matrix Format Comma Delimited Without Set Name (*.csv)";
    // QString peaksCSV =  "Peaks Detailed Format Comma Delimited (*.csv)";
    // //Added when Merging to Maven776 - Kiran
    // QString peaksListQE= "Inclusion List QE (*.csv)";
    // QString mascotMGF=   "Mascot Format MS2 Scans (*.mgf)";

    // QString sFilterSel = groupsCSV;
    // QDir().mkdir(dir+QString("/tmp_files"));
    // QString fileName = dir+QString("/tmp_files/export_all_groups.csv");
    // qDebug()<<"saving exported files to this dir...."<<fileName;
    // // QString fileName = QFileDialog::getSaveFileName(this, 
    // //         tr("Export Groups"), dir, 
    // //         groupsTAB + ";;" + groupsSTAB + ";;" + peaksTAB + ";;" + groupsCSV + ";;" + groupsSCSV + ";;" + peaksCSV + ";;" + peaksListQE + ";;" + mascotMGF,
    // //         &sFilterSel);

    // if(fileName.isEmpty()) return;

    // if ( sFilterSel == groupsCSV || sFilterSel == peaksCSV) {
    //     if(!fileName.endsWith(".csv",Qt::CaseInsensitive)) fileName = fileName + ".csv";
    // }
    // if ( sFilterSel == groupsSCSV) {
    //     if(!fileName.endsWith(".csv",Qt::CaseInsensitive)) fileName = fileName + ".csv";
    //     cerr <<"csv without:";
    //     csvreports->flag = 0;
    //     cerr <<"csv without:1";
    // }
    // if ( sFilterSel == groupsTAB || sFilterSel == peaksTAB) {
    //     if(!fileName.endsWith(".tab",Qt::CaseInsensitive)) fileName = fileName + ".tab";
    // }
    // if ( sFilterSel == groupsSTAB) {
    //     cerr <<"tab without:";
    //     if(!fileName.endsWith(".tab",Qt::CaseInsensitive)) fileName = fileName + ".tab";
    //     csvreports->flag = 0;
    //     cerr <<"tab without:1";
    // }
    
    // if ( samples.size() == 0) return;

    // //Added when Merging to Maven776 - Kiran
    // if (sFilterSel == peaksListQE ) { 
    //     writeQEInclusionList(fileName); 
    //     return;
    // } else if (sFilterSel == mascotMGF ) {
    //     writeMascotGeneric(fileName);
    //     return;
    // }

   
    // csvreports->setUserQuantType( _mainwindow->getUserQuantType() );

    // //Added to pass into csvreports file when merged with Maven776 - Kiran
    // bool includeSetNamesLines=true;

    // if (sFilterSel == groupsCSV) {
    //     csvreports->openGroupReport(fileName.toStdString(),includeSetNamesLines);
    // } else if (sFilterSel == groupsTAB )  {
    //     csvreports->openGroupReport(fileName.toStdString(),includeSetNamesLines);
    // } else if (sFilterSel == peaksCSV )  {
    //     csvreports->openPeakReport(fileName.toStdString());
    // } else if (sFilterSel == peaksTAB )  {
    //     csvreports->openPeakReport(fileName.toStdString());
    // } else {    //default to group summary
    //     //Updated when csvreports file was merged with Maven776 - Kiran
    //     csvreports->openGroupReport(fileName.toStdString(),includeSetNamesLines);
    // }

    // QList<PeakGroup*> selectedGroups = getSelectedGroups();
    // csvreports->setSelectionFlag(static_cast<int>(peakTableSelection));

    // for(int i=0; i<allgroups.size(); i++ ) {
    //     if (selectedGroups.contains(&allgroups[i])) {
    //         PeakGroup& group = allgroups[i];
    //         csvreports->addGroup(&group);
    //     }
    // }
    // csvreports->closeFiles();

    // if (csvreports->getErrorReport() != "") {
    //     QMessageBox msgBox(_mainwindow);
    //     msgBox.setIcon(QMessageBox::Critical);
    //     msgBox.setText(csvreports->getErrorReport());
    //     msgBox.exec();
    // }
    qDebug()<<"hopefully the csv files are saved now..";
    // QDir qdir(dir+QString("/tmp_files/"));
    // qdir.removeRecursively();
    
}


QByteArray PollyIntegration::run_qt_process(QString command){
    qDebug() << "command -   " << command;
    QProcess process;
    process.start(command);
    process.waitForFinished();
    QByteArray result = process.readAllStandardOutput();
    QByteArray result2 = process.readAllStandardError();
    qDebug() << "QByteArray std out result  " << result;
    qDebug() << "QByteArray std error result   " << result2;
    return result;
}

QByteArray PollyIntegration::run_system_process(QString command){
    qDebug() << "command inside run_system_process-   " << command;
    QByteArray ba = command.toLatin1();
    const char *c_str2 = ba.data();
    system(c_str2);
    return "";
}


QString PollyIntegration::get_run_id(QByteArray result){
    QList<QByteArray> test_list = result.split('\n');
    int size = test_list.size();
    QByteArray result2 = test_list[size-2];
    QJsonDocument doc(QJsonDocument::fromJson(result2));
    // Get JSON object
    QJsonObject json = doc.object();
    QVariantMap json_map = json.toVariantMap();
    QString run_id =  json_map["id"].toString();
    qDebug() << run_id;
    return run_id;
}

QString PollyIntegration::get_patch_id(QByteArray result){
    QList<QByteArray> test_list = result.split('\n');
    int size = test_list.size();
    QByteArray result2 = test_list[size-2];
    QString patch_id = QString("%1").arg(result2.toInt());
    return patch_id;
}

QStringList PollyIntegration::get_system_urls(QString filename){
    QStringList upload_commands ;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return upload_commands;
    QTextStream in(&file);
    QString line = in.readLine();
    while (!line.isNull()) {
        if (line.contains("file_upload_urls")){
            QByteArray byte_line = line.toLatin1();
            QJsonDocument doc(QJsonDocument::fromJson(byte_line));
            // Get JSON object
            QJsonObject json = doc.object();
            QVariantMap json_map = json.toVariantMap();
            QVariant url_json =  json_map["file_upload_urls"];
            QVariantMap url_map = url_json.toMap();
            QStringList keys= url_map.keys();
            for (int i = 0; i < keys.size(); ++i){
                QString key=keys.at(i).toLocal8Bit().constData();
                qDebug() <<"key - "<<key;
                QString url_map_json =  url_map[key].toList()[0].toString();
                qDebug() << "this is the url "<<url_map_json;
                QString key2= "test.csv";        
                // QString command= "mithoo createPutRequest %1 %2".arg(url_map_json).arg(key);
                QString command= QString("mithoo createPutRequest \"%1\" \"%2\"").arg(url_map_json).arg(key2);
                upload_commands.append(command);
            }
        }
        line = in.readLine();
    }
    return upload_commands;
}


QStringList PollyIntegration::get_project_upload_url_commands(QByteArray result2,QStringList filenames){
    QStringList upload_commands ;
    qDebug()<<"result2  "<<result2;
    QList<QByteArray> test_list = result2.split('\n');
    int size = test_list.size();
    QByteArray url_jsons = test_list[size-2];
    qDebug()<<"url_jsons  "<<url_jsons;
    QJsonDocument doc(QJsonDocument::fromJson(url_jsons));
    // Get JSON object
    QJsonObject json = doc.object();
    QVariantMap json_map = json.toVariantMap();
    QString url_with_wildcard =  json_map["file_upload_urls"].toString();
    for (int i=0; i < filenames.size(); ++i){
        QString filename = filenames.at(i);
        QString random_filename = "demo_file_name.json";
        QString url_map_json = url_with_wildcard.replace("*",random_filename) ;
        QString command= QString("mithoo upload_project_data \"%1\" \"%2\"").arg(url_map_json).arg(filename);
        upload_commands.append(command);
        
    }
    return upload_commands;
}

QString PollyIntegration::get_urls(QByteArray result){    
    QList<QByteArray> test_list = result.split('\n');
    int size = test_list.size();
    QByteArray result2 = test_list[size-2];
    QJsonDocument doc(QJsonDocument::fromJson(result2));
    QJsonObject json = doc.object();
    QVariantMap json_map = json.toVariantMap();
    QString url =  json_map["file_upload_urls"].toString();
    return url;
}



int PollyIntegration::check_already_logged_in(){
    int status;
    QString command = QString("mithoo authenticate");
    QByteArray result2 = run_qt_process(command);
    QList<QByteArray> test_list = result2.split('\n');
    QByteArray status_line = test_list[0];
    if (status_line=="already logged in"){
        status = 1;
    }
    else{
        status=0;
    }
    return status;
}


int PollyIntegration::authenticate_login(QString username,QString password){
    QString command = QString("mithoo authenticate %1 %2").arg(username).arg(password);
    QByteArray result = run_qt_process(command);
    int status_inside = check_already_logged_in();
    return status_inside;
}


void PollyIntegration::transferData(){

    int status = check_already_logged_in();
    if (status==0){
        qDebug()<<"not logged in....";
        login_user();
    }
    else {
        qDebug("user is already logged in... sending data to polly now..");
        exportData();
    }


}



void PollyIntegration::login_user(){
    LoginForm *w=new LoginForm(_tableDockWidget->_mainwindow,this);
    qDebug()<< "calling w.show now........";
    w->show();
    // connect(w,SIGNAL(),&loop,SLOT(quit()));
    
    // QEventLoop loop;
}
void PollyIntegration::exportData() {
    QList<PeakGroup> allgroups =  _tableDockWidget->getAllGroups();
    
    if (allgroups.size() == 0 ) {
        QString msg = "Peaks Table is Empty, can't export to POLLY";
        QMessageBox::warning(_tableDockWidget, "Error", msg);
        return;
    }

    /**
     * copy all groups from <allgroups> to <vallgroups> which is used by
     * < libmaven/jsonReports.cpp>
    */
    _tableDockWidget->vallgroups.clear();
    for(int i=0;i<allgroups.size();++i){
        _tableDockWidget->vallgroups.push_back(allgroups[i]);
    }

    QString dir = ".";
    QSettings* settings = _tableDockWidget->_mainwindow->getSettings();
    if ( settings->contains("lastDir") ) dir = settings->value("lastDir").value<QString>();

    qDebug() << "going to write polly cli code here....\n\n";
    qDebug() << "valid credentials,sending data to polly now....\n\n";
    _tableDockWidget->_mainwindow->check_polly_login->setText("connected");
    _tableDockWidget->_mainwindow->check_polly_login->setStyleSheet("QLabel { background-color : white; color : green; }");
    QStringList filenames;
    // QString fileName = QFileDialog::getSaveFileName(this, tr("Save EICs to Polly "),dir,tr("*.json"));
    // qDebug()<<"fileName - "<<fileName;
    QDir qdir(dir+QString("/tmp_files/"));
    if (!qdir.exists()){
     QDir().mkdir(dir+QString("/tmp_files"));   
     QDir qdir(dir+QString("/tmp_files/"));
    }
    qdir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList file_list = qdir.entryInfoList();
    for (int i = 0; i < file_list.size(); ++i){
        QFileInfo fileInfo = file_list.at(i);
        QString tmp_filename = dir+QString("/tmp_files/")+fileInfo.fileName();
        filenames.append(tmp_filename);

    }
    qDebug()<<"filenames   "<<filenames;
    // QDir().mkdir(dir+QString("/tmp_files"));
    QString jsonfileName = dir+QString("/tmp_files/export_json.json");
    if (jsonfileName.isEmpty()) return;
    if(!jsonfileName.endsWith(".json",Qt::CaseInsensitive)) jsonfileName = jsonfileName + ".json";
    saveJson * jsonSaveThread = new saveJson();
    jsonSaveThread->setMainwindow(_tableDockWidget->_mainwindow);
    jsonSaveThread->setPeakTable(_tableDockWidget);
    jsonSaveThread->setfileName(jsonfileName.toStdString());
    jsonSaveThread->start();
    filenames.append(jsonfileName);
    // QString command2 = QString("mithoo createRun %1 %2 %3 %4").arg("sampleqc_uploader").arg("22").arg("129").arg("exp_name");
    QString command2 = QString("mithoo createProject %1").arg("demo_Elmaven_Polly_project_internal");
    QByteArray result1 = run_qt_process(command2);
    QString run_id = get_run_id(result1);
    // QString filename = "test_postrun.txt";
    // QString postrun_command = QString("mithoo postRun --file_name '{\"%1\":\"%2\",\"%3\" :\"%4\"}' --sub_component SampleQC --id %5 --\"file_type\" raw_input_files > %6").arg("sample_cohort").arg("sample-cohort.csv").arg("lcms_conv").arg("intensity.csv").arg(run_id).arg(filename);
    QString get_upload_Project_urls = QString("mithoo get_upload_Project_urls --id %1").arg(run_id);
    QByteArray result2 = run_qt_process(get_upload_Project_urls);
    QStringList upload_project_data_commands = get_project_upload_url_commands(result2,filenames);
    // run_system_process(postrun_command);
    // QStringList put_commands = get_system_urls(filename);
    for (int i = 0; i < upload_project_data_commands.size(); ++i){
        QString command = upload_project_data_commands.at(i);    
        QByteArray patch_id_result = run_qt_process(command);
        // QString patch_id = get_patch_id(patch_id_result);
    }
    qDebug()<<"removing the tmp dir now..";
    
    qdir.removeRecursively();
    qDebug()<<"removed the tmp dir ..";
    // QString command5 = QString(" mithoo createPatchRequest %1").arg(run_id);
    // run_qt_process(command5);
    qDebug()<<" All 5 commands completed\n";

}