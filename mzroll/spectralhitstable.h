#ifndef SPECTRALHITSTABLE_H
#define SPECTRALHITSTABLE_H

#include "stable.h"
#include "mainwindow.h"
#include "numeric_treewidgetitem.h"
#include "Peptide.hpp"

class MainWindow;
class NumericTreeWidgetItem;

using namespace std;

class SpectralHitsDockWidget: public QDockWidget {
      Q_OBJECT

public:
    QWidget 	*dockWidgetContents;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *treeWidget;

    	SpectralHitsDockWidget(MainWindow* mw, QString title);
	~SpectralHitsDockWidget();

	int  hitCount() { return allhits.size(); }
	bool hasSpectralHit(SpectralHit* hit);
	QList<SpectralHit*> getHits();

public slots: 
	  //void showInfo(SpectralHit* hit);
	  SpectralHit* addSpectralHit(SpectralHit* hit);
	  void setupPeakTable();
	  SpectralHit* getSelectedGroup();
      	  QList<SpectralHit*> getSelectedHits();
          void showFocusedHits();
          void clearFocusedHits();
          void unhideFocusedHits();

      //input from xml
          void loadSpectralHitsTable();
          void loadPepXML(QString infile);

      //input from idpDB
      void loadIdPickerDB(QString infile); //TODO: Sahil, Added while merging mzfileio
      void limitPrecursorMz(float pMZ); //TODO: Sahil, Added while merging mzfileio
      void queryIdPickerPrecursors(float pMZ); //TODO: Sahil, Added while merging mzfileio
      void queryIdPickerProteins(); //TODO: Sahil, Added while merging mzfileio

      //output to xml
	  void savePeakTable();
          void savePeakTable(QString fileName);
	  void writePeakTableXML(QXmlStreamWriter& stream);

      //output to csv file
      void exportHitsToSpreadsheet();
	  void showSelectedGroup();
	  void setGroupLabel(char label);
	  void showSpectralHit(int row);
	  void showLastGroup();
	  void showNextGroup();
	  void printPdfReport();
	  void updateTable();
	  void updateItem(QTreeWidgetItem* item);
	  void updateStatus();
          void runScript();

	  void markGroupBad();
	  void markGroupGood();
	  void markGroupIgnored();
          void showAllHits();
	  void showGallery();
          void showTreeMap();
	  void showScatterPlot();
	  void setClipboard();
	  void deleteGroup();
	  void deleteAll();
      void showFocusedGroups(); //TODO: Sahil, Added while merging mzfileio
          int loadSpreadsheet(QString fileName);
          int loadCSVFile(QString filename, QString sep);
          void switchTableView();
	      void getRetentionTimes(); //TODO: Sahil, Added while merging mzfileio

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

protected slots:
	  void keyPressEvent( QKeyEvent *e );
	  void contextMenuEvent ( QContextMenuEvent * event );

private:
          void deletePeaks();
          void addRow(SpectralHit* hit, QTreeWidgetItem* root);
          void heatmapBackground(QTreeWidgetItem* item);
	  SpectralHit* readGroupXML(QXmlStreamReader& xml,SpectralHit* parent);
          void writeGroupXML(QXmlStreamWriter& stream, SpectralHit* g);
	  void readPeakXML(QXmlStreamReader& xml,SpectralHit* parent);


          MainWindow* _mainwindow;
          QList<SpectralHit*>allhits;

		  QMap<QString,ProteinHit*> proteinAccessionMap; //TODO: Sahil, Added while merging mzfileio

          enum tableViewType{ hitView=0, peakView=1 };
          tableViewType viewType;

		  QSqlDatabase idpickerDB; //TODO: Sahil, Added while merging mzfileio
};

#endif
