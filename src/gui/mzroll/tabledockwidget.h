#ifndef TABLEDOCKWIDGET_H
#define TABLEDOCKWIDGET_H

#include <QWidgetAction>

#include "stable.h"
#include "PeakGroup.h"

class MainWindow;
class ClusterDialog;
class PeakTableDeletionDialog;
class NumericTreeWidgetItem;
class ListView;
class JSONReports;
class PeakGroup;
class EIC;
class QHistogramSlider;
class PeakDetector;
class PeakGroupTreeWidget;
class MultiSelectComboBox;
class ClassificationWidget;

using namespace std;

struct RowData {
    enum class ChildType {
        Isotope,
        Adduct,
        None
    };

    // main attributes of interest
    qint64 tableId;
    quint64 parentIndex;
    ChildType childType;
    quint64 childIndex;

    // default constructor
    RowData();

    // copy operator
    bool operator==(const RowData& b) const;
};

Q_DECLARE_METATYPE(RowData);

QDataStream& operator<<(QDataStream& stream, const RowData& rowData);
QDataStream& operator>>(QDataStream& stream, RowData& rowData);

class TableDockWidget : public QDockWidget {
  Q_OBJECT

  friend PeakGroupTreeWidget;

public:
  QWidget *dockWidgetContents;
  QHBoxLayout *horizontalLayout;
  PeakGroupTreeWidget *treeWidget;
  QLabel *titlePeakTable;
  JSONReports *jsonReports;
  ClassificationWidget* classificationWidget;
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
  QString uploadId;
  int uploadCount = 0;
  QList<PeakGroup> allgroups;
  map<int, pair<string, float>> undoBuffer;

  enum tableViewType { groupView = 0, peakView = 1 };

  enum PeakTableSubsetType {
      Selected = 0,
      All = 1,
      Good = 2,
      Bad = 3,
      ExcludeBad = 4,
      Unmarked = 5,
      Correlated = 6,
      Variance = 7,
      CorrelatedVariance = 8
  };

  static const QMap<PeakGroup::ClassifiedLabel, QString> labelsForLegend();
  static const QMap<PeakGroup::ClassifiedLabel, QIcon> iconsForLegend();

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
  int topLevelGroupCount() { return _topLevelGroups.size(); }

  MainWindow* getMainWindow() { return _mainwindow; }

  /**
   * @brief Obtain a list of groups present in <allgroups>.
   * @return QList of PeakGroup pointers.
   */
  QList<shared_ptr<PeakGroup>> getGroups();

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

  void setLegend(MultiSelectComboBox *legend) { _legend = legend; }

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

  /**
   * @brief updateTableAfterAlignment Updates table if table existed before
   * samples were assigned.
   */
  void updateTableAfterAlignment();

  /**
   * @brief Raises a prompt for the user when there are no peakgroups found in 
   * peak detection  process.
   */
  void noPeakFound();
  
  /*
   * @brief Raises the warning for user if the user tries to delete
   * all the peakgroups from the table at once.
   */ 
  bool deleteAllgroupsWarning();
  
public slots:

  void moveSelectedRows(QString destinationTableName);
  void updateCompoundWidget();
  shared_ptr<PeakGroup> addPeakGroup(PeakGroup *group);
  void sortChildrenAscending(QTreeWidgetItem *item);
  virtual void setupPeakTable();
  shared_ptr<PeakGroup> getSelectedGroup();
  QList<shared_ptr<PeakGroup>> getSelectedGroups();

  void showFocusedGroups();
  void clearFocusedGroups();

  // output to csv file
  void exportGroupsToSpreadsheet();
  void prepareDataForPolly(QString writableTempDir,
                           QString exportFormat,
                           QString userFilename);
  void exportJsonToPolly(QString writableTempDir, QString jsonfileName, bool addMLInfo);

  void showClusterDialog();

  inline void selectedPeaks() {
    peakTableSelection = PeakTableSubsetType::Selected;
  };

  inline void allPeaks() {
    peakTableSelection = PeakTableSubsetType::All;
  };

  inline void goodPeaks() {
    peakTableSelection = PeakTableSubsetType::Good;
  };

  inline void badPeaks() {
    peakTableSelection = PeakTableSubsetType::Bad;
  };

  inline void excludeBadPeaks() {
      peakTableSelection = PeakTableSubsetType::ExcludeBad;
  };

  inline void unmarkedPeaks() {
      peakTableSelection = PeakTableSubsetType::Unmarked;
  };

  /**
   * @brief Query the peak table for number of peak-groups that belong to each
   * subset type.
   * @return Each `PeakTableSubsetType` mapping to the number of peak-groups
   * that fall within its category.
   */
  QMap<PeakTableSubsetType, int> countBySubsets();

  /**
   * @brief Given a list of subset types, hides all other subsets, disregarding
   * `All` and `Selected` subset types.
   * @param subsets A list of subsets whose items will remain visible.
   */
  void showOnlySubsets(QList<PeakTableSubsetType> visibleSubsets);

  /**
   * @brief Filters the tree-view such that only labels selected in the legend
   * dropdown are visible.
   */
  void filterForSelectedLabels();

  void exportJson();
  void exportSpectralLib();

  void markDuplicatesAsBad();

  void UploadPeakBatchToCloud();
  void showSelectedGroup();
  void setGroupLabel(char label);
  void showLastGroup();
  void showNextGroup();

  void printPdfReport();
  /**
   * @brief renderPdf Creates the Printer and calls corresponding
   * functions from eicWidget to render pdf.
   * @param fileName  Name of the pdf to be created.
   */
  void renderPdf(QString fileName);
  /**
   * @brief pdfReadyNotification After pdf is successfully stored,
   * this function creates the notification for the user about the
   * same.
   */
  void pdfReadyNotification();

  void undoLabel();

  void updateTable();
  void updateItem(QTreeWidgetItem *item, bool updateChildren = true);
  void updateStatus();

  virtual void markGroupBad();
  virtual void markGroupGood();
  virtual void unmarkGroup();
  void markGroupIgnored();
  void showScatterPlot();
  void setClipboard();

  void showConsensusSpectra();

  virtual void showAllGroups();
  virtual void deleteSelectedItems(bool groupsMovedToAnotherTable = false);
  virtual void deleteGroup(PeakGroup* group);

  bool deleteAll(bool askConfirmation = true);
  void clusterGroups();

  void switchTableView();

  void setTableView(tableViewType t) { viewType = t; }
  void clearClusters();

  void writeQEInclusionList(QString fileName);
  void writeMascotGeneric(QString fileName);
  vector<EIC *> getEICs(float rtmin, float rtmax, PeakGroup &grp);

  /**
   * @brief Selects the item in the tree which stores the given peak-group.
   * @param A shared pointer to a `PeakGroup` object that will be used to
   * compare against stored peak-groups.
   * @return Boolean value denoting whether the peak-group was found and
   * selected or not.
   */
  bool selectPeakGroup(shared_ptr<PeakGroup> group);

  /**
   * @brief Bring up a peak-editor to edit the RT regions for individual or a
   * set of peaks of the currently selected peak-group item.
   */
  void editSelectedPeakGroup();

  /**
   * @brief Show a `GroupSettingsLog` dialog listing all relevant parameters
   * used when the currently selected peak-group was integrated.
   */
  void showIntegrationSettings();

  /**
   * @brief Sets the default styling for this dock widget.
   * @param isActive If set to true, some additional styling will be added to
   * make it distinct from non-active widgets.
   */
  void setDefaultStyle(bool isActive = false);

  shared_ptr<PeakGroup> groupForItem(QTreeWidgetItem* item);

  void refreshParentItem(QTreeWidgetItem* item);

protected:
  MainWindow *_mainwindow;
  tableViewType viewType;
  QList<shared_ptr<PeakGroup>> _topLevelGroups;
  int _labeledGroups;
  int _targetedGroups;
  int _nextGroupId;
  QList<QTreeWidgetItem*> _cycleBuffer;

  /**
   * @brief A map storing the unique ID of all tables mapping to their titles.
   */
  static QMap<int, QString> _idTitleMap;

  void focusInEvent(QFocusEvent *event);
  void focusOutEvent(QFocusEvent *event);
  void keyPressEvent(QKeyEvent *e);
  void contextMenuEvent(QContextMenuEvent *event);
  void hideEvent(QHideEvent *event);
  void showEvent(QShowEvent *event);

signals:
  void updateProgressBar(QString, int, int, bool = false);
  void UploadPeakBatch();
  void renderedPdf();
  void ghostPeakGroupSelected(bool);

private:
  MultiSelectComboBox *_legend;
  bool _cycleInProgress;

  QPalette pal;

  RowData
  _rowDataForThisTable(size_t parentIndex,
                       RowData::ChildType childType = RowData::ChildType::None,
                       size_t childIndex = 0);

  void _deleteItemsAndGroups(QSet<QTreeWidgetItem*>& items);

  void _paintClassificationDisagreement(QTreeWidgetItem* item);

  void addRow(RowData& indexData, QTreeWidgetItem *root);
  void heatmapBackground(QTreeWidgetItem *item);

  // TODO: investigate and remove this dialog if not being used
  void setupFiltersDialog();

  /**
   * @brief Adds color to the label cell if El-MAVEN's quality assignment
   * disagrees with the good/bad curation done by the user. Darker red means
   * greater disagreement.
   * @param item The tree widget item to be re-painted.
   */
  void _paintClassificationDisagreement(QTreeWidgetItem *item);

  ClusterDialog *clusterDialog;
  peakTableSelectionType peakTableSelection;
  bool tableSelectionFlagUp;
  bool tableSelectionFlagDown;
  QShortcut * ctrlZ;

private slots:
  void _refreshCycleBuffer();
};

class PeakTableDockWidget : public TableDockWidget {
  Q_OBJECT

public:
  PeakTableDockWidget (MainWindow *mw, const QString& tableTitle="");
  ~PeakTableDockWidget();

public Q_SLOTS:
  void destroy();
  void deleteAll();
  virtual void showAllGroups();

private:
  QToolBar *toolBar;
  PeakTableDeletionDialog *deletionDialog;

private:
  void cleanUp();

private slots:
  void showDeletionDialog();

signals:
  void unSetFromEicWidget(shared_ptr<PeakGroup>);
};

class BookmarkTableDockWidget : public TableDockWidget {
  Q_OBJECT

public:
  BookmarkTableDockWidget (MainWindow *mw);
  ~BookmarkTableDockWidget();

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

  void deleteGroup(PeakGroup* group);
  void deleteSelectedItems();
  void deleteAll(bool askConfirmation = true);

  void markGroupGood();
  void markGroupBad();

private:
  QToolBar *toolBar;
  QToolButton *btnMerge;
  QMenu *btnMergeMenu;
  void showSameGroup(QPair<int, int> sameMzRtGroupIndexHash);
  void _removeGroupHash(PeakGroup* group);
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

class PeakGroupTreeWidget : public QTreeWidget {
    Q_OBJECT

public:
    PeakGroupTreeWidget(TableDockWidget* parent = nullptr);

    TableDockWidget* table;
    static RowData dragData;
    static bool moveInProgress;

    Qt::SortOrder sortOrder() const { return _sortOrder; }
    void sortByColumn(int column, Qt::SortOrder order);
    void sortItems(int column, Qt::SortOrder order)
    {
        sortByColumn(column, order);
    }

protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    Qt::DropActions supportedDropActions() const;
    void paintEvent(QPaintEvent* event);

private:
    Qt::SortOrder _sortOrder;
};

#endif
