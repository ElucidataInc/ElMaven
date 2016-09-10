#ifndef TREEDOCKWIDGET_H
#define TREEDOCKWIDGET_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"

class MainWindow;

using namespace std;

class TreeDockWidget: public QDockWidget {
      Q_OBJECT

public:
    QWidget 	*dockWidgetContents;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *treeWidget;

    TreeDockWidget(MainWindow*, QString title, int numColms);
    //Added while merging with Maven776 - Kiran
    void setQQQToolBar();


public slots: 
	  //void showInfo(PeakGroup* group);
	  QTreeWidgetItem* addItem(QTreeWidgetItem* parentItem, string key , float value,  int type);
      QTreeWidgetItem* addItem(QTreeWidgetItem* parentItem, string key , string value, int type);


	  void showInfo();
	  void setInfo(Peak* peak);
	  void setInfo(PeakGroup* group);
	  void setInfo(Compound* c);
	  void setInfo(vector<Compound*>&compounds);

	  void setInfo(vector<mzLink>&links);
	  void setInfo(vector<mzSlice*>&slices);
	  void setInfo(deque<Pathway*>&pathways);

	  void addScanItem(Scan* scan);
	  void clearTree();
	  void filterTree(QString needle);
      void copyToClipbard();

      bool hasPeakGroup(PeakGroup* group);

      protected slots:
          void keyPressEvent(QKeyEvent *e );
      void contextMenuEvent ( QContextMenuEvent *e );

      private slots:
      QTreeWidgetItem* addPeakGroup(PeakGroup* group, QTreeWidgetItem* parent);
      QTreeWidgetItem* addPeak(Peak* peak, QTreeWidgetItem* parent);
      QTreeWidgetItem* addCompound(Compound* c, QTreeWidgetItem* parent);
      QTreeWidgetItem* addSlice(mzSlice* s,QTreeWidgetItem* parent);
      QTreeWidgetItem* addLink(mzLink* s,  QTreeWidgetItem* parent);

      void unlinkGroup();

    private:
      //Added while merging with Maven776 - Kiran
      MainWindow* _mainWindow;
      void itemToClipboard(QTreeWidgetItem* item, QString& clipboardtext);
};

#endif
