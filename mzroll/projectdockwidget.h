#ifndef PROJECTDOCKWIDGET_H
#define PROJECTDOCKWIDGET_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"


class ProjectDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit ProjectDockWidget(QMainWindow *parent = 0);


Q_SIGNALS:

public Q_SLOTS:
    void showInfo();
    QString getProjectDescription();
    void setProjectDescription(QString text);
    void setInfo(vector<mzSample*>&samples);
    void changeSampleOrder();
    void updateSampleList();
    void loadProject();
    void saveProject();
    void loadProject(QString filename);
    void saveProject(QString filename, TableDockWidget* peakTable = 0);
    void setSampleColor(mzSample* sample, QColor color); //TODO: Sahil, Added while merging projectdockwidget
    void unloadSelectedSamples(); //TODO: Sahil, Added while merging projectdockwidget
    void sampleupload();
protected Q_SLOTS:
      void keyPressEvent( QKeyEvent *e );
      void contextMenuEvent ( QContextMenuEvent * event );

private Q_SLOTS:
    void showSample(QTreeWidgetItem* item, int col);
    void showSampleInfo(QTreeWidgetItem* item, int col);
    void changeSampleColor(QTreeWidgetItem* item, int col);
    void changeNormalizationConstant(QTreeWidgetItem* item, int col);
    void changeSampleSet(QTreeWidgetItem* item, int col);
    void selectSample(QTreeWidgetItem* item, int col);
    void changeColors();
    void checkUncheck(); //TODO: Sahil, Added while merging projectdockwidget
    void setSampleColor(QTreeWidgetItem* item, QColor color);
    void dropEvent (QDropEvent*event);
    // void unloadSample(); //TODO: Sahil, Removed while merging projectdockwidget
    void unloadSample(mzSample* sample); //TODO: Sahil, Added while merging projectdockwidget
    void filterTreeItems(QString filterString);

private:
    QTreeWidgetItem* getParentFolder(QString filename);
    QMap<QString,QTreeWidgetItem*> parentMap;
    QTextEdit* _editor;
    MainWindow* _mainwindow;
    QTreeWidget* _treeWidget;
    QSplitter*  _splitter;

    QString lastOpennedProject;
    QString lastSavedProject;
    QColor  lastUsedSampleColor;


};

#endif // PROJECTDOCKWIDGET_H
