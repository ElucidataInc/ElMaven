#include "Compound.h"
#include "csvreports.h"
#include "globals.h"
#include "highlighter.h"
#include "mainwindow.h"
#include "mzSample.h"
#include "rconsolewidget.h"
#include "Scan.h"
#include "tabledockwidget.h"

RconsoleWidget::RconsoleWidget(QWidget *parent): QDockWidget(parent)
{
    setupUi(this);
    setFloating(false);

    _mainwindow = (MainWindow*) parent;
    _settings = _mainwindow->getSettings();
    _linkedTable = _mainwindow->getBookmarkedPeaks();

    setupEditor();

    connect(btnOpen, SIGNAL(clicked()), SLOT(openFile()));
    connect(btnRun, SIGNAL(clicked()), SLOT(runAnalysis()));
    connect(btnSave,SIGNAL(clicked()),SLOT(saveFile()));
    btnSave->setDisabled(true);

    QString pid = QString::number(QCoreApplication::applicationPid());
    processScriptFile = QDir::tempPath() + "/script" + pid + ".R";
    processErrorFile=QDir::tempPath() + "/Rprocess." + pid + ".out";
    processOutFile=QDir::tempPath()  +  "/Rprocess." + pid + ".err";
    groupsTableFile=QDir::tempPath()  +  "/groups."  + pid + ".tab";

    process = new QProcess();
    process->setStandardOutputFile(processOutFile);
    process->setStandardErrorFile(processErrorFile);
    connect(process, SIGNAL(finished(int)), this, SLOT(readProcessOutput(int)));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readStdErr()));
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdOut()));
    this->setVisible(false);
    this->setWindowTitle("R Console");

    if(_settings->contains("lastScript") ) {
        loadScript(_settings->value("lastScript").toString());
    }

}

RconsoleWidget::~RconsoleWidget()
{
}


void RconsoleWidget::newFile()
{
    editor->clear();
}

void RconsoleWidget::openFile()
{
    
    //get script name
    QString dir;
    if ( _settings->contains("scriptsFolder") )
        dir = _settings->value("scriptsFolder").value<QString>();

    QString scriptName = QFileDialog::getOpenFileName(this, "Run Script",
                                                      dir, "R Script (*.R)");
    loadScript(scriptName);
}

void RconsoleWidget::loadScript(QString scriptFileName)
{
    if (scriptFileName.isEmpty()) return;

    QFile file(scriptFileName);
    if (file.open(QFile::ReadOnly | QFile::Text))
        editor->setPlainText(file.readAll());

    _settings->setValue("lastScript",scriptFileName);
    fileLabel->setText(scriptFileName);
    userScriptFile = scriptFileName;
    btnSave->setDisabled(true);
}

void RconsoleWidget::saveFile()
{
    //get script name

    QString dir;
    if ( _settings->contains("scriptsFolder") ) {
        dir = _settings->value("scriptsFolder").value<QString>();
    }


    QString fileName =   QFileDialog::getSaveFileName(this, "Save Script",
                                        userScriptFile, "R Script (*.R)");

    if(fileName.isEmpty()) return;


    QFile scriptFileHandle(fileName);
    if( scriptFileHandle.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream stream(&scriptFileHandle);
        stream << editor->toPlainText();
        scriptFileHandle.close();

        _settings->setValue("lastScript",fileName);
        fileLabel->setText(fileName);
        userScriptFile=fileName;
        btnSave->setDisabled(true);

    } else {
        errorLog->appendPlainText("Error: Can't Save " + fileName );
    }

}

void RconsoleWidget::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    editor->setFont(font);

    connect(editor,SIGNAL(textChanged()),this,SLOT(enableScriptSave()));
    highlighter = new Highlighter(editor->document());

}

void RconsoleWidget::updateStatus() {
    int groupCount = 0;
    if (_linkedTable) groupCount = _linkedTable->getSelectedGroups().size();
    setStatusLabel(tr("Linked to %1 groups").arg(groupCount));
}

void RconsoleWidget::runAnalysis() {
    
    //QString scriptText = editor->toPlainText()
    //save file to temp buffer
    //QTemporaryFile tempScriptFile;
    //tempScriptFile.setAutoRemove(true);
    //tempScriptFile.open();
    //tempScriptFile.close();
    updateStatus();

    if (process->pid()){
           process->terminate();
           errorLog->appendPlainText("Killing process")    ;
           return;
    }

    // exec R
    QString Rexe = _settings->value("Rprogram").toString();
    Rexe.replace(' ',"\\ ");

    if(!QFile::exists(Rexe)) {
        errorLog->appendPlainText("Can't find " + Rexe);
        return;
    }

    //QString scriptFileName=tempScriptFile.fileName();
    QFile scriptFileHandle(processScriptFile);
    if( scriptFileHandle.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream stream(&scriptFileHandle);
        stream << editor->toPlainText();
        scriptFileHandle.close();
    } else {
        errorLog->appendPlainText("Error: Can't Open " + processScriptFile );
    }

    exportGroupsToTable();

    errorLog->appendPlainText(processScriptFile);

    outputLog->clear();
    errorLog->clear();
    errorLog->appendPlainText("Running " + Rexe);
    btnRun->setText("Stop!");


#define xstr(s) str(s)
#define str(s) #s

    //start process
    QStringList arguments;
    QString os = QString(xstr(PLATFORM));
    //windows operationg system arguments
    if (os == "Windows") {
         arguments  << "--slave" << "--no-save" << processScriptFile  << groupsTableFile;
    } else  {
        arguments  << "--slave" << "--no-save" << "-f" << processScriptFile << "--args" << groupsTableFile;
    }

    qDebug() << "Running:" << Rexe << arguments; 

    process->start(Rexe, arguments);

    if (!process->waitForStarted())
    {
       process->kill();
       errorLog->appendPlainText("Killing process");
       btnRun->setText("Run");
       return;
    }

    //while (!process->waitForFinished()) {
   //   if( stopProcessFlag) {
    //        process->kill();
   //     }
   //}
}

void RconsoleWidget::readProcessOutput(int) {
   readStdErr();
   readStdOut();

    if (process->pid() == 0) {
        btnRun->setText("Run");
    }

   //files tmp remove
   QStringList tmpfiles; tmpfiles << processScriptFile << processErrorFile << processOutFile;
   Q_FOREACH(QString filename, tmpfiles) {
       QFile fileA(filename);
       if(fileA.exists()) fileA.remove();
   }
}

void RconsoleWidget::readStdOut() {
    QFile outFile(processOutFile);
    if (outFile.open(QFile::ReadOnly)) {
        QTextStream streamOut(&outFile);
        outputLog->append(streamOut.readAll());
    } else {
         errorLog->appendPlainText("Error: Can't Open " + outFile.fileName() );
    }
    outFile.close();
}


void RconsoleWidget::readStdErr() {
    QFile errFile(processErrorFile);
    if (errFile.open(QFile::ReadOnly)) {
        QTextStream streamOut(&errFile);
        errorLog->appendPlainText(streamOut.readAll());
    } else {
         errorLog->appendPlainText("Error: Can't Open " + errFile.fileName() );
    }
    errFile.close();
}

void RconsoleWidget::exportGroupsToTable()
{
    if( _linkedTable) groups = _linkedTable->getSelectedGroups();

    //prepare data
    QFile peaks(groupsTableFile);

    //Added to pass into csvreports file when merged with Maven776 - Kiran
    bool includeSetNames=false;
    if(peaks.open(QFile::WriteOnly | QFile::Truncate)) {
        vector<mzSample*> vsamples = _mainwindow->getVisibleSamples();
        sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
   
        auto prmGroupAt = find_if(begin(groups),
                                  end(groups),
                                  [] (PeakGroup* group) {
                                      return group->getCompound()->type() == Compound::Type::MS2;
                                  });
        bool prmGroupExists = prmGroupAt != end(groups);

        CSVReports* csvreports = new CSVReports(groupsTableFile.toStdString(),
                                                CSVReports::ReportType::GroupReport, vsamples,
                                                _mainwindow->getUserQuantType(),
                                                prmGroupExists,includeSetNames,
                                                _mainwindow->mavenParameters);

        for(int i=0; i<groups.size(); i++ ) {
            csvreports->addGroup(groups[i]);
        }


        peaks.close();
    } else {
       errorLog->appendPlainText("Can't write to " + groupsTableFile);
    }

    //prepare data
    QFile sampleInfo(groupsTableFile+".info");
    if(sampleInfo.open(QFile::WriteOnly | QFile::Truncate)) {
        vector<mzSample*> vsamples = _mainwindow->getVisibleSamples();
        sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
        QTextStream stream(&sampleInfo);
        stream << "sampleName" << "\t" << "setName" << "\t" << "sampleColor" << endl;
        for(int i=0; i < vsamples.size(); i++) {
            mzSample* sample = vsamples[i];
            QColor color = QColor::fromRgbF( sample->color[0], sample->color[1], sample->color[2], 1 );
            stream << QString(vsamples[i]->sampleName.c_str())
                   << "\t" << QString(vsamples[i]->_setName.c_str())
                   << "\t" << color.name()
                   << endl;
        }
        sampleInfo.close();
    } else {
       errorLog->appendPlainText("Can't write sample info " + groupsTableFile);
    }
}
