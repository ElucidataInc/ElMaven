#ifndef TABLEDOCKWIDGET_H
#define TABLEDOCKWIDGET_H

#include <qdockwidget.h>
#include <qlist.h>
#include <qmap.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qxmlstream.h>
#include <vector>

#include "../libmaven/mzSample.h"
#include "QHistogramSlider.h"

class QContextMenuEvent;
class QHBoxLayout;

class MainWindow;
class TrainDialog;
class NumericTreeWidgetItem;

using namespace std;

class TableDockWidget: public QDockWidget {
Q_OBJECT

public:
	QWidget *dockWidgetContents;
	QHBoxLayout *horizontalLayout;
	QTreeWidget *treeWidget;

	TableDockWidget(MainWindow* mw, QString title, int numColms);
	~TableDockWidget();

	int groupCount() {
		return allgroups.size();
	}
	bool hasPeakGroup(PeakGroup* group);
	QList<PeakGroup*> getGroups();

public slots:
	//void showInfo(PeakGroup* group);
	PeakGroup* addPeakGroup(PeakGroup* group);
	void setupPeakTable();
	PeakGroup* getSelectedGroup();

	//input from xml
	void loadPeakTable();
	void loadPeakTable(QString infile);

	//output to xml
	void savePeakTable();
	void savePeakTable(QString fileName);
	void writePeakTableXML(QXmlStreamWriter& stream);

	//output to csv file
	void exportGroupsToSpreadsheet();
	void exportPeaksToSpreadsheet();
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

	void markGroupBad();
	void markGroupGood();
	void markGroupIgnored();
	void showAllGroups();
	void showHeatMap();
	void showGallery();
	void showTreeMap();
	void showScatterPlot();
	void setClipboard();
	void deleteGroup();
	void sortBy(int);
	void align();
	void deleteAll();
	void clusterGroups();
	void findMatchingCompounds();
	void showFiltersDialog();
	void filterPeakTable();

protected slots:
	void keyPressEvent(QKeyEvent *e);
	void contextMenuEvent(QContextMenuEvent * event);

private:
	void deletePeaks();
	void addRow(PeakGroup* group, QTreeWidgetItem* root);
	PeakGroup* readGroupXML(QXmlStreamReader& xml, PeakGroup* parent);
	void writeGroupXML(QXmlStreamWriter& stream, PeakGroup* g);
	void readPeakXML(QXmlStreamReader& xml, PeakGroup* parent);
	void setupFiltersDialog();
	QString groupTagString(PeakGroup* group);

	MainWindow* _mainwindow;
	QList<PeakGroup> allgroups;

	TrainDialog* traindialog;
	QDialog* filtersDialog;
	QMap<QString, QHistogramSlider*> sliders;
};

#endif
