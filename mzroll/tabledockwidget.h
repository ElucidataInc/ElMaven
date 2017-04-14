#ifndef TABLEDOCKWIDGET_H
#define TABLEDOCKWIDGET_H

#include <algorithm>
#include "stable.h"
#include "mainwindow.h"
#include "alignmentvizwidget.h"
#include "traindialog.h"
#include "clusterdialog.h"
#include "numeric_treewidgetitem.h"
#include "QHistogramSlider.h"
#include "saveJson.h"

class MainWindow;
class AlignmentVizWidget;
class TrainDialog;
class ClusterDialog;
class NumericTreeWidgetItem;
using namespace std;

class TableDockWidget: public QDockWidget {
      Q_OBJECT

public:
    QWidget 	*dockWidgetContents;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *treeWidget;
    QToolButton *btnMerge;
    QMenu* btnMergeMenu;
    QMap<QAction*,int> mergeAction;
    //QAction *hell;
    bool bookmarkPeaksTAble = false;
    //Added when Merging to Maven776 - Kiran
    enum tableViewType{ groupView=0, peakView=1 };
    enum peakTableSelectionType{ selected=0, Whole=1, Good=2, Bad=3 };

    TableDockWidget(MainWindow* mw, QString title, int numColms, int bookmarkFlag = 0);
	~TableDockWidget();

	int  groupCount() { return allgroups.size(); }
	bool hasPeakGroup(PeakGroup* group);
	QList<PeakGroup*> getGroups();
    int tableId;
    //Added when Merging to Maven776 - Kiran
    MatrixXf getGroupMatrix();
    MatrixXf getGroupMatrix(vector<mzSample*>& samples, PeakGroup::QType qtype);
    void writeGroupMzEICJson(PeakGroup& grp,ofstream& myfile, vector<string> vsampleNames);
    void saveMzEICJson(string filename);
    void setTableId();
    void setIntensityColName();
    float extractMaxIntensity(PeakGroup* group);
    string sanitizeJSONstring(string s);
    float outputRtWindow = 2.0;

public Q_SLOTS: 
	  //void showInfo(PeakGroup* group);
	  PeakGroup* addPeakGroup(PeakGroup* group);
	  void setupPeakTable();
      void showLog();
	  PeakGroup* getSelectedGroup();
      QList<PeakGroup*> getSelectedGroups();
          void showFocusedGroups();
          void clearFocusedGroups();
          void unhideFocusedGroups();

      //input from xml
          void loadPeakTable();
          void loadPeakTable(QString infile);

      //output to xml
	  void savePeakTable();
          void savePeakTable(QString fileName);
	  void writePeakTableXML(QXmlStreamWriter& stream);

      //output to csv file
      //Added when Merging to Maven776 - Kiran
      void exportGroupsToSpreadsheet();
    void showTrainDialog();
    void showClusterDialog();
    inline void selectedPeakSet() {
        peakTableSelection = peakTableSelectionType::selected;
    };

    inline void wholePeakSet () {
        peakTableSelection = peakTableSelectionType::Whole;
    };

    inline void goodPeakSet () {
        peakTableSelection = peakTableSelectionType::Good;
    };

    inline void badPeakSet () {
        peakTableSelection = peakTableSelectionType::Bad;
    };
    
    void exportJson();
	  void showSelectedGroup();
	  void setGroupLabel(char label);
	  void showPeakGroup(int row);
	  void showLastGroup();
	  void showNextGroup();
	  void Train();
	  float showAccuracy(vector<PeakGroup*>& groups);
	  void saveModel();
	  void printPdfReport();
	  void updateTable();
	  void updateItem(QTreeWidgetItem* item);
	  void updateStatus();
        //   void runScript();

	  void markGroupBad();
	  void markGroupGood();
      bool checkLabeledGroups();
	  void markGroupIgnored();
          void showAllGroups();
	  void showHeatMap();
	  void showGallery();
          void showTreeMap();
	  void showScatterPlot();
	  void setClipboard();
      //Added when Merging to Maven776 - Kiran
      void showConsensusSpectra();
	  void deleteGroups();
	  // merged with maven776 - Kiran
	  void deleteGroup(PeakGroup* groupX);
	  void sortBy(int);
	  void align();
	  void deleteAll();
          void clusterGroups();
	  void findMatchingCompounds();
      void showFiltersDialog();
      void filterPeakTable();
      int loadSpreadsheet(QString fileName);
      int loadCSVFile(QString filename, QString sep);
      void showMergeTableOptions();
      void showMsgBox(bool check, int tableNo);
      void mergeGroupsIntoPeakTable(QAction* action);
      void switchTableView();
     //Added when Merging to Maven776 - Kiran
      void setTableView(tableViewType t) {viewType=t; }
      void clearClusters();
      //Added when Merging to Maven776 - Kiran
      void writeQEInclusionList(QString fileName);
      void writeMascotGeneric(QString fileName);
      //void saveEICsJson(string filename);
      //void saveEICJson(ofstream& out, EIC* eic);
      vector<EIC*> getEICs(float rtmin, float rtmax, PeakGroup& grp);
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

Q_SIGNALS:
	void peakMarkedTableDock();
    void updateProgressBar(QString, int, int);

protected Q_SLOTS:
	  void keyPressEvent( QKeyEvent *e );
	  void contextMenuEvent ( QContextMenuEvent * event );

private:
          void deletePeaks();
          void addRow(PeakGroup* group, QTreeWidgetItem* root);
          void heatmapBackground(QTreeWidgetItem* item);
	  PeakGroup* readGroupXML(QXmlStreamReader& xml,PeakGroup* parent);
          void writeGroupXML(QXmlStreamWriter& stream, PeakGroup* g);
	  void readPeakXML(QXmlStreamReader& xml,PeakGroup* parent);
	  void setupFiltersDialog();
	  QString groupTagString(PeakGroup* group);

          MainWindow* _mainwindow;
          QList<PeakGroup>allgroups;

          TrainDialog* traindialog;
          ClusterDialog*       clusterDialog;
          QDialog* 	 filtersDialog;
          QMap<QString, QHistogramSlider*> sliders;
        float rtWindow=2;
          tableViewType viewType;
          peakTableSelectionType peakTableSelection;
          QList<PeakGroup*> getCustomGroups(peakTableSelectionType peakSelection);
          bool tableSelectionFlagUp;
          bool tableSelectionFlagDown;
    
};


class TableWidgetThread: public QThread {
    Q_OBJECT

    private:
        void run() {table->updateTable();quit();return;}

    public:
        void setTable(TableDockWidget* _table) {this->table = _table;}
        TableDockWidget* table;


};

#endif
