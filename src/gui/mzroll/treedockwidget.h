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


public Q_SLOTS: 
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

      protected Q_SLOTS:
      	void keyPressEvent(QKeyEvent *e );
      	void contextMenuEvent ( QContextMenuEvent *e );

      private Q_SLOTS:
      QTreeWidgetItem* addPeakGroup(PeakGroup* group, QTreeWidgetItem* parent);
      QTreeWidgetItem* addPeak(Peak* peak, QTreeWidgetItem* parent);
      QTreeWidgetItem* addCompound(Compound* c, QTreeWidgetItem* parent);
      QTreeWidgetItem* addSlice(mzSlice* s,QTreeWidgetItem* parent);
      QTreeWidgetItem* addLink(mzLink* s,  QTreeWidgetItem* parent);

      void unlinkGroup();

      /**
       * @brief Add compound selection menu to annotate compounds manually
       * @details This slot insert menu items which contains compounds. Compounds
       * are selected on the basis of filterline of item selected. Precursor and
       * product m/z are calculated for the filterline and then matched with
       * compounds in compound database with some tolerance.
       * @param item Selected item from the qtreewidget
       * @see SRMList
       */
      void manualAnnotation(QTreeWidgetItem * item);

      /**
       * @brief Annotate filterline with the selected compound from the menu
       * @param action This QAction updates the annotation of the mzSlice (filterline)
       * @see manualAnnotation
       */
      void annotateCompound(QAction* action);

    private:
      //Added while merging with Maven776 - Kiran
      MainWindow* _mainWindow;
      void itemToClipboard(QTreeWidgetItem* item, QString& clipboardtext);
      QDoubleSpinBox* amuQ1;
      QDoubleSpinBox* amuQ3;
      QToolButton* associateCompounds;
      QMenu* matchCompoundMenu;

};

#endif
