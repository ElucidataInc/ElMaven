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
#include "pollyintegration.h"


class MainWindow;
class AlignmentVizWidget;
class TrainDialog;
class ClusterDialog;
class NumericTreeWidgetItem;
class ListView;
class PollyIntegration;
using namespace std;

class TableDockWidget: public QDockWidget {
      Q_OBJECT

public:
    /**
     * @param- promptDialog is main dialog box to show user prompt for already bookmarked
     * groups with same mz and rt value. It will hold <save> ,<cancel>,text labels (<upperLabel>
     * and <lowerLabel>), list of already bookmarked groups <listTextView>.
    */
    QDialog* promptDialog;
    QHBoxLayout * buttonLayout;
    QVBoxLayout* promptDialogLayout;    /**@param- holds all widget (upperLabel,listTextView,lowerLabel) and <buttonLayout>*/
    QLabel *upperLabel;
    QLabel* lowerLabel;
    QPushButton *cancel;
    QPushButton * save;
    ListView *listTextView; /**@param-  holds all already group's correcponding compound name*/
    QStringListModel* stringModel;  /**@param-  model of compound name,will be set to <listTextView>*/

    MainWindow* _mainwindow;
    PollyIntegration* _pollyIntegration;
    QWidget 	*dockWidgetContents;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *treeWidget;
    QToolButton *btnMerge;
    QMenu* btnMergeMenu;
    QLabel *titlePeakTable;
 
    QMap<QPair<int, int>,QList<QString> > sameMzRtGroups;
    bool addSameMzRtGroup;

    /**
     * @brief vallgroups will be used by libmaven/jsonReports.cpp
     * @detail For json export. Since libmaven is written only standard
     * cpp, all groups from <allgroups> get copied to <vallgroups> at
     * time of json exporting
     * @see- <TableDockWidget::exportJson>
     */
    vector<PeakGroup> vallgroups;

    QMap<QAction*,int> mergeAction;
    //QAction *hell;
    bool bookmarkPeaksTAble = false;
    //Added when Merging to Maven776 - Kiran
    enum tableViewType{ groupView=0, peakView=1 };
    enum peakTableSelectionType{ selected=0, Whole=1, Good=2, Bad=3 };

    TableDockWidget(MainWindow* mw, QString title, int numColms, int bookmarkFlag = 0);
	~TableDockWidget();

    int  groupCount() { return allgroups.size(); }
    QList<PeakGroup> getAllGroups(){ return allgroups;}
    /**
     * @detail- this function return true if this group already present in
     * bookmrked group <allgroups>.
     * In case of groups with same mz and rt value, this will holds its execution
     * by QEventLoop <loop> and show a prompt to user whether he wants to add
     * this group to bookmarked groups or not by executing method <showSameGroup>.
     * <loop> will hold execution of this funtion till user press a button on prompt
     * dialog.
    */
    bool hasPeakGroup(PeakGroup* group);
	QList<PeakGroup*> getGroups();
    int tableId;
    /**< for making old mzroll compatible, this will act as a flag
    *whether loaded mzroll file is old or new one. this will be set by class
    *method <markv_0_1_5mzroll>
    */
    bool mzrollv_0_1_5;
    //Added when Merging to Maven776 - Kiran
    MatrixXf getGroupMatrix();
    MatrixXf getGroupMatrix(vector<mzSample*>& samples, PeakGroup::QType qtype);
    void setTableId();
    void setIntensityColName();
    float extractMaxIntensity(PeakGroup* group);
    float outputRtWindow = 2.0;

public Q_SLOTS: 
    void sortChildrenAscending(QTreeWidgetItem* item);
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
      void exportGroupsToSpreadsheet_polly();
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
    /**
     * @details-this is a slot tied with <save> button of prompt dialog <promptDialog>
     * to show already bookmarked group with same mz and rt value.
     * If user press <save> button <addSameMzRtGroup> sets to true which will be used
     * to add this group's corresponding compound name to already bookmarked group
     * of same rt and mz value.
    */
    void acceptGroup();
    /**
     * @details-this is a slot tied with <save> button of prompt dialog <promptDialog>
     * to show already bookmarked group with same mz and rt value.
     * If user press <cancel> button <addSameMzRtGroup> sets to false which will be used
     * to reject this group's corresponding compound name.
    */
    void rejectGroup();
    void exportJson();
    void exportPolly();
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
    /**
     * @brief  modify name appropriate for xml attribute naming
     * @detail This method makes sample name appropriate for using in attribute naming in mzroll file
     * It is just replacing '#' with '_' and adding 's' for letting sample name start with english letter
     * In future, if sample name has some other special character, we have to replace those also
     * with appropriate character
     * Error can be seen at compilation time
    */
      void cleanString(QString &name);
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
    void showSameGroup(QPair<int, int> sameMzRtGroupIndexHash);
          void deletePeaks();
          void addRow(PeakGroup* group, QTreeWidgetItem* root);
          void heatmapBackground(QTreeWidgetItem* item);
	  PeakGroup* readGroupXML(QXmlStreamReader& xml,PeakGroup* parent);
          void writeGroupXML(QXmlStreamWriter& stream, PeakGroup* g);
      void readPeakXML(QXmlStreamReader& xml,PeakGroup* parent);
    /**
     * @brief- it will add samples used to group being generated while creating from mzroll file
    * @detail This method will add all sample to group being
    *created from mzroll file. It will read SamplesUsed attribute of a group
    *and if it's value is "Used", then assign this mzSample to that group
    */
      void readSamplesXML(QXmlStreamReader &xml,PeakGroup* group);
    /**@brief-mark varible <mzrollv_0_1_5> true or false
     *@details  this method marks varible <mzrollv_0_1_5> true if loaded mzroll
     * file is of v0.1.5 or older otherwise false based on one attribute
     * <SamplesUsed> which is introduced here.
    */
    void markv_0_1_5mzroll(QString fileName);
	  void setupFiltersDialog();

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

class TableToolBarWidgetAction : public QWidgetAction
{
    public:
        TableDockWidget* td;
        TableToolBarWidgetAction(QObject *parent, TableDockWidget* table, QString btnType) : QWidgetAction(parent) {
            btnName = btnType;
            td = table;
        }
        virtual ~TableToolBarWidgetAction() {}

    protected:
        /**
        * [This is a virtual function of class QWidgetAction. This function gets called when we create
            instance of class QWidgetAcion or class inherting QWidgetAction. This widget creates custom Widgets]
        * @param parent [parent of the instance]
        */
        virtual QWidget *createWidget(QWidget *parent);

    private:
        QString btnName;
};

class ListView: public QListView{
private:
    QStringList strings;
public:
     /**
     * @details- this method will execute when user select and press <ctrl + c> to copy
     * list of compound from prompt dialog which show already bookmarked group
     * with same rt and mz value.
    */
    virtual void keyPressEvent(QKeyEvent *event) ;
    void setData(QStringList vstrings){strings=vstrings;}
};

#endif
