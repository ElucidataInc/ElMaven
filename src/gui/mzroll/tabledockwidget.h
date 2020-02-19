#ifndef TABLEDOCKWIDGET_H
#define TABLEDOCKWIDGET_H

#include <QWidgetAction>

#include "pollyintegration.h"
#include "stable.h"

class MainWindow;
class TrainDialog;
class ClusterDialog;
class PeakTableDeletionDialog;
class NumericTreeWidgetItem;
class ListView;
class JSONReports;
class PeakGroup;
class EIC;
class QHistogramSlider;

using namespace std;

class TableDockWidget : public QDockWidget {
  Q_OBJECT

public:
  QWidget *dockWidgetContents;
  QHBoxLayout *horizontalLayout;
  QTreeWidget *treeWidget;
  QLabel *titlePeakTable;
  JSONReports *jsonReports;
  int numberOfGroupsMarked = 0;
  QString writableTempS3Dir;
  /**
   * @brief vallgroups will be used by libmaven/jsonReports.cpp
   * @detail For json export. Since libmaven is written only standard
   * cpp, all groups from <allgroups> get copied to <vallgroups> at
   * time of json exporting
   * @see- <TableDockWidget::exportJson>
   */
  vector<PeakGroup> vallgroups;
  vector<PeakGroup> subsetPeakGroups;
  int maxPeaks;
  QList<PeakGroup> allgroups;
  QString uploadId;
  int uploadCount = 0;

  enum tableViewType { groupView = 0, peakView = 1 };
  enum peakTableSelectionType {
      Selected = 0,
      Whole = 1,
      Good = 2,
      Bad = 3,
      NotBad = 4
  };

  /**
   * @brief Construct and initialize a TableDockWidget.
   * @detail Sets up widgets (treewidget, dialogs, etc.) and connects
   * any necessary signals.
   * 
   * @param mw A QMainWindow to which the dock belongs.
   */
  TableDockWidget(MainWindow *mw);
  ~TableDockWidget();

  /**
   * @brief Obtain the number of groups in the table.
   * @return Integer count.
   */
  int groupCount() { return allgroups.size(); }

  MainWindow* getMainWindow() { return _mainwindow; }

  /**
   * @brief Obtain a list of groups present in <allgroups>.
   * @return QList of PeakGroup pointers.
   */
  QList<PeakGroup *> getGroups();

  int tableId;

  /**
   * @brief Update the name of the intensity type field in table
   * according to the quantity type currenly selected by the user.
   */
  void setIntensityColName();

  /**
   * @brief Obtain the maximum intensity for a group according to
   * the quantity type currently selected by the user.
   * 
   * @param group Pointer to the PeakGroup.
   * @return Floating precision value of maximum intensity
   */
  float extractMaxIntensity(PeakGroup *group);

  /**
   * @brief Get the number of targeted groups in this peak table.
   * @return Targeted group count as integer.
   */
  int getTargetedGroupCount();

  /**
   * @brief Get the number of labeled groups in this peak table.
   * @return Targeted group count as integer.
   */
  int getLabeledGroupCount();

  /**
   * @brief Obtain the title of a TableDockWidget, identified by its unique ID.
   * @details Two IDs are reserved: the IDs -1 and 0 will always return
   * "Scatterplot Peak Table" and "Bookmark Table", respectively.
   * @param tableId An integer representing the unique ID of a peak table.
   * @return A QString set to the title of the peak table if found, empty
   * otherwise.
   */
  static QString getTitleForId(int tableId);

  /**
   * @brief Store the title of a peak table corresponding to its ID, for future
   * queries using `getTitleForId`.
   * @details The IDs -1 and 0 will always be saved with a constant title,
   * regardless of the title being passed. If a table title already exists, then
   * a counter is added to the title to differentiate it from all the other
   * tables in the registry.
   * @param tableId A unique ID (that has not already been used for a previous
   * table).
   * @param title The title that needs to be saved for the given table ID. This
   * title, however, will be modified if a table with this name has already been
   * registered.
   */
  static void setTitleForId(int tableId, const QString& tableTitle="");

  /**
   * @brief Get the table ID of the last registered peak table.
   * @return An integer representing a table ID.
   */
  static int lastTableId();

  /**
   * @brief Clears all key-value pairs from the `_idTitleMap` object. This is
   * meant to be called only when a "new session" behavior is wanted.
   */
  static void clearTitleRegistry() {
      _idTitleMap.clear();
      setTitleForId(-1);
      setTitleForId(0);
  }

public Q_SLOTS:
  void updateCompoundWidget();
  PeakGroup *addPeakGroup(PeakGroup *group);
  void sortChildrenAscending(QTreeWidgetItem *item);
  virtual void setupPeakTable();
  PeakGroup *getSelectedGroup();
  QList<PeakGroup *> getSelectedGroups();
  void showNotification();

  void showFocusedGroups();
  void clearFocusedGroups();
  void unhideFocusedGroups();

  // output to csv file
  void exportGroupsToSpreadsheet();
  void prepareDataForPolly(QString writableTempDir,
                           QString exportFormat,
                           QString userFilename);
  void exportJsonToPolly(QString writableTempDir, QString jsonfileName, bool addMLInfo);

  void showTrainDialog();
  void showClusterDialog();

  inline void selectedPeakSet() {
    peakTableSelection = peakTableSelectionType::Selected;
  };

  inline void wholePeakSet() {
    peakTableSelection = peakTableSelectionType::Whole;
  };

  inline void goodPeakSet() {
    peakTableSelection = peakTableSelectionType::Good;
  };

  inline void badPeakSet() {
    peakTableSelection = peakTableSelectionType::Bad;
  };

  inline void excludeBadPeakSet() {
      peakTableSelection = peakTableSelectionType::NotBad;
  };

  void exportJson();
  void UploadPeakBatchToCloud();
  void showSelectedGroup();
  void setGroupLabel(char label);
  void showLastGroup();
  void showNextGroup();

  // Training methods
  void Train();
  float showAccuracy(vector<PeakGroup *> &groups);
  void saveModel();

  void printPdfReport();

  void updateTable();
  void updateItem(QTreeWidgetItem *item, bool updateChildren = true);
  void updateStatus();

  //Group validation functions
  void validateGroup(PeakGroup* grp, QTreeWidgetItem* item);

  virtual void markGroupBad();
  virtual void markGroupGood();
  virtual void unmarkGroup();
  bool checkLabeledGroups();
  void markGroupIgnored();
  void showAllGroups();
  void showHeatMap();
  void showScatterPlot();
  void setClipboard();

  void showConsensusSpectra();

  virtual void deleteGroups();
  virtual void deleteGroup(PeakGroup *groupX);

  void sortBy(int);
  void align();
  void deleteAll();
  void clusterGroups();
  void findMatchingCompounds();
  void showFiltersDialog();
  void filterPeakTable();

  int loadSpreadsheet(QString fileName);
  int loadCSVFile(QString filename, QString sep);

  void switchTableView();

  void setTableView(tableViewType t) { viewType = t; }
  void clearClusters();

  void writeQEInclusionList(QString fileName);
  void writeMascotGeneric(QString fileName);
  vector<EIC *> getEICs(float rtmin, float rtmax, PeakGroup &grp);

  /**
   * @brief Selects the item in the tree which stores the given peak-group.
   * @param A pointer to a `PeakGroup` object that will be used to compare
   * against stored peak-groups.
   * @return Boolean value denoting whether the peak-group was found and
   * selected or not.
   */
  bool selectPeakGroup(PeakGroup *group);

protected:
  MainWindow *_mainwindow;
  tableViewType viewType;
  int _labeledGroups;
  int _targetedGroups;

  /**
   * @brief A map storing the unique ID of all tables mapping to their titles.
   */
  static QMap<int, QString> _idTitleMap;

  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);
  void focusInEvent(QFocusEvent *event);
  void focusOutEvent(QFocusEvent *event);

Q_SIGNALS:
  void updateProgressBar(QString, int, int);
  void UploadPeakBatch();

protected Q_SLOTS:
  void keyPressEvent(QKeyEvent *e);
  void contextMenuEvent(QContextMenuEvent *event);

private:
  QPalette pal;
  void addRow(PeakGroup *group, QTreeWidgetItem *root);
  void heatmapBackground(QTreeWidgetItem *item);

  // TODO: investigate and remove this dialog if not being used
  void setupFiltersDialog();

  TrainDialog *traindialog;
  ClusterDialog *clusterDialog;
  QDialog *filtersDialog;
  QMap<QString, QHistogramSlider *> sliders;
  peakTableSelectionType peakTableSelection;
  QList<PeakGroup *> getCustomGroups(peakTableSelectionType peakSelection);
  bool tableSelectionFlagUp;
  bool tableSelectionFlagDown;
};

class PeakTableDockWidget : public TableDockWidget {
  Q_OBJECT

public:
  PeakTableDockWidget (MainWindow *mw, const QString& tableTitle="");
  ~PeakTableDockWidget();

public Q_SLOTS:
  void destroy();

private:
  QToolBar *toolBar;
  PeakTableDeletionDialog *deletionDialog;

private:
  void cleanUp();

private Q_SLOTS:
  void showDeletionDialog();

Q_SIGNALS:
  void unSetFromEicWidget(PeakGroup*);
};

class BookmarkTableDockWidget : public TableDockWidget {
  Q_OBJECT

public:
  BookmarkTableDockWidget (MainWindow *mw);
  ~BookmarkTableDockWidget();

  /**
   * @param promptDialog is main dialog box to show user prompt for already
   * bookmarked groups with same mz and rt value. It will hold <save>, <cancel>,
   * text labels (<upperLabel> and <lowerLabel>), list of already
   * bookmarked groups <listTextView>.
   */
  QDialog *promptDialog;
  QHBoxLayout *buttonLayout;
  QVBoxLayout *promptDialogLayout;

  /**
   * @param- holds all widget (upperLabel,listTextView,lowerLabel) and
   * <buttonLayout>
   */
  QLabel *upperLabel;
  QLabel *lowerLabel;
  QPushButton *cancel;
  QPushButton *save;
  ListView *listTextView;

  /**
   * @param-  holds all already group's corresponding
   * compound name
   * @param-  model of compound name, will be set to <listTextView>
   */
  QStringListModel *stringModel;

  QMap<QPair<int, int>, QList<QString>> sameMzRtGroups;
  bool addSameMzRtGroup;

  QMap<QAction *, int> mergeAction;

  /**
   * @brief This method allows checking if a group is already present in
   * bookmarked groups <allgroups>.
   * @detail In case of groups with same mz and rt value, execution will be
   * held by QEventLoop <loop> and the user will be prompted to choose whether
   * they want to add the given group to bookmarked groups or not by executing
   * method <showSameGroup>. <loop> will hold execution of this funtion till
   * the user presses a button on the prompt dialog.
   * 
   * @param group pointer to a PeakGroup object (presented by the table).
   * @return Boolean value denoting whether group is present in <allgroups>.
   */
  bool hasPeakGroup(PeakGroup *group);

public Q_SLOTS:
  void showMergeTableOptions();
  void showMsgBox(bool check, int tableNo);
  /**
   * @brief This method moves the bookmarked groups to the selected peak table
   * @details Merges bookmark and peak tables and then displays a success or error
   * message
   * @param pointer to QAction that stores the selected peak table
   */
  void mergeGroupsIntoPeakTable(QAction *action);

  /**
   * @details This is a slot tied with <save> button of prompt dialog
   * <promptDialog> to show already bookmarked group with same mz and rt value.
   * If user press <save> button <addSameMzRtGroup> sets to true which will be
   * used to add this group's corresponding compound name to already bookmarked
   * group of same rt and mz value.
   */
  void acceptGroup();

  /**
   * @details This is a slot tied with <save> button of prompt dialog
   * <promptDialog> to show already bookmarked group with same mz and rt value.
   * If user press <cancel> button <addSameMzRtGroup> sets to false which will
   * be used to reject this group's corresponding compound name.
   */
  void rejectGroup();

  void deleteGroup(PeakGroup *groupX);
  void markGroupGood();
  void markGroupBad();

private:
  QToolBar *toolBar;
  QToolButton *btnMerge;
  QMenu *btnMergeMenu;
  void showSameGroup(QPair<int, int> sameMzRtGroupIndexHash);
};

class ScatterplotTableDockWidget : public TableDockWidget {
  Q_OBJECT

public:
  ScatterplotTableDockWidget(MainWindow *mw);
  ~ScatterplotTableDockWidget();
  void setupPeakTable();

private:
  QToolBar *toolBar;
};

class TableToolBarWidgetAction : public QWidgetAction {
public:
  TableDockWidget *td;
  TableToolBarWidgetAction(QObject *parent,
                           TableDockWidget *table,
                           QString btnType)
      : QWidgetAction(parent) {
    btnName = btnType;
    td = table;
  }
  virtual ~TableToolBarWidgetAction() {}

protected:
  /**
   * [This is a virtual function of class QWidgetAction. This function gets
   * called when we create instance of class QWidgetAcion or class inherting
   * QWidgetAction. This widget creates custom Widgets]
   * @param parent [parent of the  instance]
   */
  virtual QWidget *createWidget(QWidget *parent);

private:
  QString btnName;
};

class ListView : public QListView {
private:
  QStringList strings;

public:
  /**
   * @details- this method will execute when user select and press <ctrl + c>
   * to copy list of compound from prompt dialog which show already bookmarked
   * group with same rt and mz value.
   */
  virtual void keyPressEvent(QKeyEvent *event);
  void setData(QStringList vstrings) { strings = vstrings; }
};

class UploadPeaksToCloudThread : public QThread
{
    Q_OBJECT
    public:
        UploadPeaksToCloudThread(PollyIntegration* iPolly);
        ~UploadPeaksToCloudThread();
        void run();
        QString sessionId;
        QString fileName;
        QString filePath;
        PollyIntegration* _pollyintegration;
    signals:
        void resultReady(QString sessionId);
};


#endif
