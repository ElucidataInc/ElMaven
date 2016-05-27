#ifndef RCONSOLEWIDGET_H
#define RCONSOLEWIDGET_H

#include "globals.h"
#include "stable.h"
#include "highlighter.h"
#include "ui_rconsolewidget.h"
#include "mainwindow.h"



class RconsoleWidget : public QDockWidget, public Ui_RconsoleWidget 
{
    Q_OBJECT

public:
    RconsoleWidget(QWidget *parent = 0);
    ~RconsoleWidget();

public slots:
    void openFile();
    void saveFile();
    void runAnalysis();
    void newFile();
    void readProcessOutput(int);
    void setPeakGroups(QList<PeakGroup*>grouplist) { groups=grouplist; updateStatus();}
    void stopProcess() { if(process->pid()) process->terminate(); }
    void linkTable(TableDockWidget* table) { _linkedTable=table; }
    void enableScriptSave() { btnSave->setEnabled(true); }
    void readStdOut();
    void setStatusLabel(QString l) { statusLabel->setText(l); }
    void readStdErr();
    void updateStatus();
    void loadScript(QString filename);


private:
    void setupEditor();
    void exportGroupsToTable();

    Highlighter *highlighter;
    MainWindow* _mainwindow;
    QSettings* _settings;
    TableDockWidget* _linkedTable;
    QString userScriptFile;

    QProcess *process;
    QString processScriptFile;
    QString processErrorFile;
    QString processOutFile;
    QString groupsTableFile;

    QList<PeakGroup*> groups;
};

#endif
