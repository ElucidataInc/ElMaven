#ifndef PROJECTDOCKWIDGET_H
#define PROJECTDOCKWIDGET_H

#include <boost/signals2.hpp>
#include <boost/bind.hpp>

#include "stable.h"

class QTextEdit;
class MainWindow;
class mzSample;
class TableDockWidget;
class PeakGroup;

class ProjectDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit ProjectDockWidget(QMainWindow *parent = 0);
    boost::signals2::signal< void (const string&,unsigned int , int ) > boostSignal;
    QColor  lastUsedSampleColor;
    QMap<mzSample*, QColor> storeSampleColors;
    QTreeWidget* getTreeWidget();
    void prepareSampleCohortFile(QString sampleCohortFileName);
    QString getLastOpenedProject();
    void setLastOpenedProject(QString filename);
    QColor getSampleColor(mzSample* sample);
    QIcon getSampleIcon(mzSample* sample);

Q_SIGNALS:
    void samplesDeleted();

public Q_SLOTS:
    void showInfo();
    QString getProjectDescription();
    void setProjectDescription(QString text);
    void setInfo(vector<mzSample*>&samples);
    void changeSampleOrder();
    void updateSampleList();

    /**
     * @brief Bring up a file dialog and allow the user to save current session
     * as an emDB project.
     */
    void saveProjectAsSQLite();

    /**
     * @brief Update saved data in the currently open emDB project. If no
     * project is currently open, prompts the user to create a new one.
     */
    void saveSQLiteProject();

    /**
     * @brief Save any pending changes and close the currently open SQLite
     * (emDB) project.
     */
    void saveAndCloseCurrentSQLiteProject();

    /**
     * @brief Clear out structures before loading a new SQLite (emDB) project.
     * @details This method tries to unload any loaded samples and deletes
     * peak tables in the current session. Although it should be noted that
     * this has historically not resulted in truly fresh session experience for
     * users. Still something like this should ideally be available to
     * developers for house cleaning before loading new projects into existing
     * sessions.
     */
    void clearSession();

    void loadMzRollProject(QString filename);
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

    QString _lastOpenedProject;
};

/**
 * @brief The ProjectTreeWidget class is meant to provide drag and drop
 * functionality to the regular tree widget used for storing samples in a
 * ProjectDockWidget class.
 */
class ProjectTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    ProjectTreeWidget(QWidget* parent=nullptr);

Q_SIGNALS:
    void itemDropped(QTreeWidgetItem* item);
    void itemsSorted();

protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

private:
    QTreeWidgetItem* _draggedItem;
};


/**
 * @brief The ProjectHeaderView class has been created to provide an added
 * sorting facility, even if the widget as been turned to enable drag-n-drop.
 */
class ProjectHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    ProjectHeaderView(Qt::Orientation orientation, QWidget* parent);

signals:
    /**
     * @brief Emitted whenever the column header is clicked, and should be
     * considered as a request to sort items based on the contents of this
     * column.
     * @param column An integer denoting the column that should be sorted for.
     * @param sortOrder The sort order for contents of the clicked column.
     */
    void sortRequested(int column, Qt::SortOrder sortOrder);

protected:
    void mouseReleaseEvent(QMouseEvent* event);
};

#endif // PROJECTDOCKWIDGET_H
