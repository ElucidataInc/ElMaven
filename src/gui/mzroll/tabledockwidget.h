#ifndef TABLEDOCKWIDGET_H
#define TABLEDOCKWIDGET_H

#include "QHistogramSlider.h"
#include "grouprtwidget.h"
#include "clusterdialog.h"
#include "peaktabledeletiondialog.h"
#include "jsonReports.h";
#include "mainwindow.h"
#include "numeric_treewidgetitem.h"
#include "saveJson.h"
#include "stable.h"
#include "traindialog.h"
#include <algorithm>

class MainWindow;
class TrainDialog;
class ClusterDialog;
class PeakTableDeletionDialog;
class NumericTreeWidgetItem;
class ListView;
using namespace std;

class TableDockWidget : public QDockWidget {
  Q_OBJECT

public:
  QWidget *dockWidgetContents;
  QHBoxLayout *horizontalLayout;
  QTreeWidget *treeWidget;
  QLabel *titlePeakTable;
  JSONReports *jsonReports;
  int labeledGroups = 0;

  /**
   * @brief vallgroups will be used by libmaven/jsonReports.cpp
   * @detail For json export. Since libmaven is written only standard
   * cpp, all groups from <allgroups> get copied to <vallgroups> at
   * time of json exporting
   * @see- <TableDockWidget::exportJson>
   */
  vector<PeakGroup> vallgroups;
  QList<PeakGroup> allgroups;

  enum tableViewType { groupView = 0, peakView = 1 };
  enum peakTableSelectionType { Selected = 0, Whole = 1, Good = 2, Bad = 3 };

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
   * for making old mzroll compatible, this will act as a flag whether loaded
   * mzroll file is old or new one. this will be set by class method
   * <markv_0_1_5mzroll>
   */
  bool mzrollv_0_1_5;

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

  // input from xml
  void loadPeakTable();
  void loadPeakTable(QString infile);

  // output to xml
  void savePeakTable();
  void savePeakTable(QString fileName);
  void writePeakTableXML(QXmlStreamWriter &stream);

  // output to csv file
  void exportGroupsToSpreadsheet();
  void prepareDataForPolly(QString writableTempDir,
                           QString exportFormat,
                           QString userFilename);
  void exportJsonToPolly(QString writableTempDir, QString jsonfileName);

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

  void exportJson();
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
  void updateItem(QTreeWidgetItem *item);
  void updateStatus();

  virtual void markGroupBad();
  virtual void markGroupGood();
  bool checkLabeledGroups();
  void markGroupIgnored();
  void showAllGroups();
  void showHeatMap();

  /**
   * @brief  modify name appropriate for xml attribute naming
   * @detail This method makes sample name appropriate for using in attribute
   * naming in mzroll file It is just replacing '#' with '_' and adding 's' for
   * letting sample name start with english letter In future, if sample name has
   * some other special character, we have to replace those also with
   * appropriate character Error can be seen at compilation time
   */
  void cleanString(QString &name);
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

protected:
  MainWindow *_mainwindow;
  tableViewType viewType;
  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);
  void focusInEvent(QFocusEvent *event);
  void focusOutEvent(QFocusEvent *event);

Q_SIGNALS:
  void updateProgressBar(QString, int, int);

protected Q_SLOTS:
  void keyPressEvent(QKeyEvent *e);
  void contextMenuEvent(QContextMenuEvent *event);

private:
  QPalette pal;
  void addRow(PeakGroup *group, QTreeWidgetItem *root);
  void heatmapBackground(QTreeWidgetItem *item);
  PeakGroup *readGroupXML(QXmlStreamReader &xml, PeakGroup *parent);
  void writeGroupXML(QXmlStreamWriter &stream, PeakGroup *g);
  void readPeakXML(QXmlStreamReader &xml, PeakGroup *parent);

  /**
   * @brief- it will add samples used to group being generated while creating
   * from mzroll file
   * @detail This method will add all sample to group being
   * created from mzroll file. It will read SamplesUsed attribute of a group
   * and if it's value is "Used", then assign this mzSample to that group
   */
  void readSamplesXML(QXmlStreamReader &xml,
                      PeakGroup *group,
                      float mzrollVersion);

  /**
   * @brief-mark varible <mzrollv_0_1_5> true or false
   * @details  this method marks varible <mzrollv_0_1_5> true if loaded mzroll
   * file is of v0.1.5 or older otherwise false based on one attribute
   * <SamplesUsed> which is introduced here.
   */
  void markv_0_1_5mzroll(QString fileName);

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
  PeakTableDockWidget (MainWindow *mw);
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

#endif
