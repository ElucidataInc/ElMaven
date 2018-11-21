#ifndef PROJECTDOCKWIDGET_H
#define PROJECTDOCKWIDGET_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"
#include <boost/signals2.hpp>
#include <boost/bind.hpp>

extern Database DB; 

class ProjectDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit ProjectDockWidget(QMainWindow *parent = 0);
    boost::signals2::signal< void (const string&,unsigned int , int ) > boostSignal;
    QString lastOpenedProject;
    QString lastSavedProject;
    QColor  lastUsedSampleColor;
    QMap<mzSample*, QColor> storeSampleColors;
    QTreeWidget* getTreeWidget();
    void prepareSampleCohortFile(QString sampleCohortFileName);

Q_SIGNALS:

public Q_SLOTS:
    void showInfo();
    QString getProjectDescription();
    void setProjectDescription(QString text);
    void setInfo(vector<mzSample*>&samples);
    void changeSampleOrder();
    void updateSampleList();
    void saveProjectAsSQLite();
    void saveSQLiteProject();
    int saveBookmarkedGroup(PeakGroup* group);
    void loadSQLiteProject(QString filename);
    void saveAndCloseCurrentSQLiteProject();
    void clearSession();
    void saveProjectAsMzRoll();
    void saveMzRollProject();
    void loadMzRollProject(QString filename);
    void saveMzRollProject(QString filename, TableDockWidget* peakTable=nullptr);
    void setSampleColor(mzSample* sample, QColor color); //TODO: Sahil, Added while merging projectdockwidget
    void unloadSelectedSamples(); //TODO: Sahil, Added while merging projectdockwidget
    void sendBoostSignal( const string& progressText, unsigned int completed_samples, int total_samples)
    {
    boostSignal(progressText, completed_samples, total_samples);
    }


    void SetAsBlankSamples(); //TODO: --@Giridhari, Create function to Set samples as Blank Samples
protected Q_SLOTS:
      void keyPressEvent( QKeyEvent *e );

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
    void markBlank(QTreeWidgetItem* item);
    void unmarkBlank(QTreeWidgetItem* item);
    QMap<QString,QTreeWidgetItem*> parentMap;
    QTextEdit* _editor;
    MainWindow* _mainwindow;
    QTreeWidget* _treeWidget;


    QMap<QString, QColor> storeColor;
    QColor  usedColor;



};

#endif // PROJECTDOCKWIDGET_H
