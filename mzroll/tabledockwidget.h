#ifndef TABLEDOCKWIDGET_H
#define TABLEDOCKWIDGET_H

#include "stable.h"
#include "mainwindow.h"
#include "traindialog.h"
#include "clusterdialog.h"
#include "numeric_treewidgetitem.h"
#include "QHistogramSlider.h"

class MainWindow;
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
    //Added when Merging to Maven776 - Kiran
    enum tableViewType{ groupView=0, peakView=1 };

    TableDockWidget(MainWindow* mw, QString title, int numColms);
	~TableDockWidget();

	int  groupCount() { return allgroups.size(); }
	bool hasPeakGroup(PeakGroup* group);
	QList<PeakGroup*> getGroups();
    //Added when Merging to Maven776 - Kiran
    MatrixXf getGroupMatrix();
    MatrixXf getGroupMatrix(vector<mzSample*>& samples, PeakGroup::QType qtype);

public slots: 
	  //void showInfo(PeakGroup* group);
	  PeakGroup* addPeakGroup(PeakGroup* group);
	  void setupPeakTable();
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
          void runScript();

	  void markGroupBad();
	  void markGroupGood();
	  void markGroupIgnored();
          void showAllGroups();
	  void showHeatMap();
	  void showGallery();
          void showTreeMap();
	  void showScatterPlot();
	  void setClipboard();
      //Added when Merging to Maven776 - Kiran
      void showConsensusSpectra();
	  void deleteGroup();
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
      void switchTableView();
     //Added when Merging to Maven776 - Kiran
      void setTableView(tableViewType t) {viewType=t; }
      void clearClusters();
      //Added when Merging to Maven776 - Kiran
      void writeQEInclusionList(QString fileName);
      void writeMascotGeneric(QString fileName);
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

protected slots:
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

          tableViewType viewType;
};

#endif
