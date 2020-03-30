#include "Compound.h"
#include "mzSample.h"
#include "eicwidget.h"
#include "globals.h"
#include "mainwindow.h"
#include "masscalcgui.h"
#include "numeric_treewidgetitem.h"
#include "pathwaywidget.h"
#include "projectdockwidget.h"
#include "Scan.h"
#include "settingsform.h"
#include "spectrawidget.h"
#include "SRMList.h"
#include "treedockwidget.h"

//Updated when Merging to Maven776 - Kiran
TreeDockWidget::TreeDockWidget(MainWindow* mw, QString title, int numColms) {
		treeWidget=new QTreeWidget(this);
		treeWidget->setColumnCount(numColms);
		treeWidget->setObjectName(title);
        connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(showInfo()));
        connect(treeWidget,SIGNAL(itemSelectionChanged()), SLOT(showInfo()));        
        treeWidget->setHeaderHidden(true);

        //QShortcut* ctrlA = new QShortcut(QKeySequence(tr("Ctrl+A", "Select All")), this);
        //connect(ctrlA,SIGNAL(activated()),treeWidget,SLOT(selectAll())); 

        //QShortcut* ctrlC = new QShortcut(QKeySequence(tr("Ctrl+C", "Copy Items")), this);
        //connect(ctrlC,SIGNAL(activated()),treeWidget,SLOT(copyToClipbard())); 

		setWidget(treeWidget);
		setWindowTitle(title);
		setObjectName(title);
        //Added when Merging to Maven776 - Kiran
        _mainWindow=mw;
}


QTreeWidgetItem* TreeDockWidget::addItem(QTreeWidgetItem* parentItem, string key , float value, int type=0) {
    auto item = new NumericTreeWidgetItem(parentItem, type);
	item->setText(0, QString(key.c_str()));
	item->setText(1, QString::number(value,'f',3));
	return item;
}

QTreeWidgetItem* TreeDockWidget::addItem(QTreeWidgetItem* parentItem, string key , string value, int type=0) {
    auto item = new NumericTreeWidgetItem(parentItem, type);
	item->setText(0, QString(key.c_str()));
	if (! value.empty() ) { item->setText(1, QString(value.c_str())); }
	return item;
}

void TreeDockWidget::clearTree() {
    treeWidget->clear();
    _scansList.clear();
}


void TreeDockWidget::setInfo(vector<mzSlice*>& slices) { 
    QStringList header; header << "Slices";
	treeWidget->clear();
    treeWidget->setHeaderLabels( header );
    treeWidget->setSortingEnabled(true);

	for(int i=0; i < slices.size(); i++ ) { 
			mzSlice* slice = slices[i];
			Compound* x = slice->compound;
			QString tag = QString(slice->srmId.c_str());

                        auto parent = new NumericTreeWidgetItem(treeWidget,mzSliceType);
			parent->setText(0,tag);
  	    	parent->setData(0,Qt::UserRole,QVariant::fromValue(*slice));

			if (x) { 
				addCompound(x, parent);
                                parent->setText(0, tr("%1 %2").arg(QString(x->name().c_str()), tag ));
			}
	}
}


void TreeDockWidget::setInfo(Peak* peak) {
	treeWidget->clear();
    addPeak(peak, NULL);
}


void TreeDockWidget::showInfo() {
    MainWindow* mainwindow = (MainWindow*)parentWidget();
        if ( ! mainwindow ) return;

        Q_FOREACH(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
                        QString text = item->text(0);
                        QVariant v =   item->data(0,Qt::UserRole);
                        int itemType = item->type();

                        if ( itemType == CompoundType ) {
                                Compound* c =  v.value<Compound*>();
                                if (c) mainwindow->setCompoundFocus(c);
                        } else if ( itemType == PeakGroupType ) {
                                PeakGroup* group =  v.value<PeakGroup*>();
                                if (group) mainwindow->setPeakGroup(group);
                        } else if ( itemType == ScanType ) {
                                Scan*  scan =  v.value<Scan*>();
                                if (scan) {
                                    if (mainwindow->spectraDockWidget) {
                                        mainwindow->spectraDockWidget->setVisible(true);
                                        mainwindow->spectraDockWidget->raise();
                                    }

                                    if (scan->mslevel == 2) {
                                        mainwindow->massCalcWidget->setFragmentationScan(scan);
                                        mainwindow->fragSpectraWidget->overlayScan(scan);
                                    }

                                    mainwindow->getSpectraWidget()->setScan(scan);
                                    mainwindow->getEicWidget()->setFocusLine(scan->rt);
                                    // if (scan->mslevel > 1) {
                                    //  mainwindow->peptideFragmentation->setScan(scan);
                                    // }
                                }
				
                        } else if (itemType == EICType ) {
                                mainwindow->getEicWidget()->setSrmId(text.toStdString());
                        } else if ( itemType == PathwayType ) {
                                        mainwindow->pathwayDockWidget->setVisible(true);
                                        mainwindow->getPathwayWidget()->setPathway( v.toString() );
                        } else if (itemType == mzLinkType ) {
                                float mz = item->text(0).toFloat();
                                        if (mz>0)mainwindow->setMzValue(mz);
                        } else if (itemType == mzSliceType ) {
                                        mzSlice slice =  v.value<mzSlice>();
                                        mainwindow->getEicWidget()->setMzSlice(slice);
                                        mainwindow->getEicWidget()->resetZoom();
                                        manualAnnotation(item);
                                        qDebug() << "showInfo() mzSlice: " << slice.srmId.c_str();
                        } else {
                                cerr << "UNKNOWN TYPE=" << v.type() << endl;
                        }
        }
}

QTreeWidgetItem* TreeDockWidget::addPeak(Peak* peak, QTreeWidgetItem* parent) {
    if (peak == NULL ) return NULL;
        NumericTreeWidgetItem *item=NULL;
    if ( parent == NULL ) {
        item = new NumericTreeWidgetItem(treeWidget,PeakType);
    } else {
        item = new NumericTreeWidgetItem(parent,PeakType);
    }

}

void TreeDockWidget::keyPressEvent(QKeyEvent *e ) {
    auto item = treeWidget->currentItem();
	if (e->key() == Qt::Key_Delete ) { 
    	if ( item->type() == PeakGroupType ) {
			unlinkGroup(); 
		} else if ( item->type() == CompoundType  ) {
			unlinkGroup();
		}
	}
}
	
void TreeDockWidget::unlinkGroup() {
    auto item = treeWidget->currentItem();
	if ( item == NULL ) return;
	PeakGroup* group=NULL;
	Compound*  cpd=NULL;

    if ( item->type() == PeakGroupType ) {
		QVariant v = item->data(0,Qt::UserRole);
   	 	group =  v.value<PeakGroup*>();
	}  else if (item->type() == CompoundType ) {
		QVariant v = item->data(0,Qt::UserRole);
   		cpd =  v.value<Compound*>();
	} else {
		return;
	}

    MainWindow* mainwindow = (MainWindow*)parentWidget();

        if (cpd) {
    	mainwindow->setCompoundFocus(cpd);
	} else if (group && group->parent) {
		PeakGroup* parentGroup = group->parent;
		if ( parentGroup->deleteChild(group) ) {
                        auto parentItem = item->parent();
			if ( parentItem ) { parentItem->removeChild(item); delete(item); }
			mainwindow->getEicWidget()->setPeakGroup(parentGroup);
		}
	} else if (group) {
		for(int i=0; i < treeWidget->topLevelItemCount(); i++ ) {
			if ( treeWidget->topLevelItem(i) == item ) {
					item->setHidden(true);
					treeWidget->removeItemWidget(item,0); delete(item);
					group->deletePeaks();
					group->deleteChildren();

					break;

			}
		}
	}

	treeWidget->update();
    mainwindow->getPathwayWidget()->updateCompoundConcentrations();

	return;
}



QTreeWidgetItem* TreeDockWidget::addSlice(mzSlice* s, QTreeWidgetItem* parent) {
        auto item = new NumericTreeWidgetItem(treeWidget,mzSliceType);
        item->setText(0,QString::number(s->mzmin,'f',4));
        item->setText(1,QString::number(s->mzmax,'f',4));
        item->setText(2,QString::number(s->rtmin,'f',4));
        item->setText(3,QString::number(s->rtmax,'f',4));
        item->setText(4,QString::number(s->ionCount,'f',4));
        return item;
}

void TreeDockWidget::setInfo(vector<mzLink>& links) {
    treeWidget->clear();
    if (links.size() == 0 ) return;

    QStringList header; header << "m/z" <<  "correlation" << "note";
    treeWidget->clear();
    treeWidget->setHeaderLabels( header );
    treeWidget->setSortingEnabled(false);

    auto item0 = new NumericTreeWidgetItem(treeWidget,mzLinkType);
    item0->setText(0,QString::number(links[0].mz1,'f',4));
    item0->setExpanded(true);
    for(int i=0; i < links.size(); i++) addLink(&links[i],item0);

    treeWidget->setSortingEnabled(true);
    treeWidget->sortByColumn(1,Qt::DescendingOrder);
}

QTreeWidgetItem* TreeDockWidget::addLink(mzLink* s,QTreeWidgetItem* parent)  {

    if (!s) return NULL;

    NumericTreeWidgetItem *item=NULL;
    if( parent == NULL ){
            item = new NumericTreeWidgetItem(treeWidget,mzLinkType);
        } else {
            item = new NumericTreeWidgetItem(parent,mzLinkType);
        }

        if ( item ) {
                item->setText(0,QString::number(s->mz2,'f',4));
                item->setText(1,QString::number(s->correlation,'f',2));
                item->setText(2,QString(s->note.c_str()));
        }
        return item;
}

void TreeDockWidget::setInfo(deque<Pathway*>& pathways) {

    treeWidget->clear();
    QStringList header; header << "Pathway";
    treeWidget->setHeaderLabels( header );
    map<string,string>::iterator itr;
    treeWidget->setSortingEnabled(true);

    for(int i=0; i < pathways.size(); i++ ) {
        Pathway* p = pathways[i];
        auto parent = new NumericTreeWidgetItem(treeWidget, PathwayType);
        parent->setText(0,QString((p->name +"("+p->id+")").c_str() 	));
        parent->setData(0,Qt::UserRole,QVariant::fromValue(QString(p->id.c_str())));
    }
}

void TreeDockWidget::filterTree(QString needle) {
        int itemCount = treeWidget->topLevelItemCount();
        for(int i=0; i < itemCount; i++ ) {
                auto item = treeWidget->topLevelItem(i);
                if ( item == NULL) continue;
                if ( needle.isEmpty() || item->text(0).contains(needle,Qt::CaseInsensitive) ) {
                        item->setHidden(false);
                } else {
                        item->setHidden(true);
                }
        }
}

void TreeDockWidget::setupScanListHeader()
{
    QStringList colNames;
    colNames << "sample" << "pre m/z" << "rt" << "purity" << "TIC"
             << "#fragments";
    treeWidget->setColumnCount(colNames.size());
    treeWidget->setHeaderLabels(colNames);
    treeWidget->setSortingEnabled(true);
    treeWidget->setHeaderHidden(false);
}

void TreeDockWidget::addScanItem(Scan* scan) {
        if (scan == NULL) return;
        _scansList.append(scan);

        QIcon icon = _mainWindow->projectDockWidget->getSampleIcon(scan->sample);

        auto item = new NumericTreeWidgetItem(treeWidget, ScanType);
        item->setData(0, Qt::UserRole, QVariant::fromValue(scan));
        item->setIcon(0, icon);	
        item->setText(1, QString::number(scan->precursorMz, 'f', 4));	
        item->setText(2, QString::number(scan->rt));
        item->setText(3,QString::number(scan->getPrecursorPurity(20.00),'g',3));
        item->setText(4,QString::number(scan->totalIntensity(),'g',3));
        item->setText(5,QString::number(scan->nobs()));
}

void TreeDockWidget::sortScansBySample()
{
    treeWidget->clear();
    auto samples = _mainWindow->getVisibleSamples();
    for (auto sample : samples) {
        for (auto scan : sample->scans) {
            if (_scansList.contains(scan))
                addScanItem(scan);
        }
    }
}

void TreeDockWidget::setInfo(vector<Compound*>&compounds) {
    clearTree();
        for(int i=0; i < compounds.size(); i++ ) addCompound(compounds[i],NULL);
}


void TreeDockWidget::setInfo(Compound* x)  {
        addCompound(x,NULL);
}

void TreeDockWidget::setInfo(PeakGroup* group) {
        if (group == NULL) return;
    if (hasPeakGroup(group)) return;
    clearTree();
    addPeakGroup(group,NULL);
}


bool TreeDockWidget::hasPeakGroup(PeakGroup* group) {
    if (treeWidget == NULL) return true;
    for(int i=0; i < treeWidget->topLevelItemCount();i++ ) {
        auto item = treeWidget->topLevelItem(i);
        if ( item->type() != PeakGroupType ) continue;
        QVariant v = item->data(0,Qt::UserRole);
        PeakGroup*  g = v.value<PeakGroup*>();
        if (g && g == group ) return true;

        for(int j=0; j < item->childCount();j++ ) {
            auto item2 = item->child(j);
            if ( item2->type() != PeakGroupType ) continue;
            QVariant v = item2->data(0,Qt::UserRole);
            PeakGroup*  g = v.value<PeakGroup*>();
            if (g && g == group ) return true;
        }
    }
    return false;
}

QTreeWidgetItem* TreeDockWidget::addCompound(Compound* c, QTreeWidgetItem* parent) {
    if (c == NULL) return NULL;
    NumericTreeWidgetItem* item = NULL;

    if ( parent == NULL ) {
            item = new NumericTreeWidgetItem(treeWidget, CompoundType);
    } else {
            item = new NumericTreeWidgetItem(parent, CompoundType);
    }

        if (!item) return NULL;

        QString id = QString(c->name().c_str());
        item->setText(0,id);
    item->setData(0, Qt::UserRole,QVariant::fromValue(c));

    return item;
}


QTreeWidgetItem* TreeDockWidget::addPeakGroup(PeakGroup* group, QTreeWidgetItem* parent) {
    if (group == NULL) return NULL;
    NumericTreeWidgetItem* item = NULL;

    if ( parent == NULL ) {
        item = new NumericTreeWidgetItem(treeWidget, PeakGroupType);
    } else {
        item = new NumericTreeWidgetItem(parent, PeakGroupType);
    }

        if (!item) return NULL;

    QString id(group->getName().c_str());
    item->setText(0,id);
    item->setExpanded(true);
    item->setData(0, Qt::UserRole,QVariant::fromValue(group));
    //for(int i=0; i < group->peaks.size(); i++ ) addPeak(&(group->peaks[i]), item);
    for(int i=0; i < group->childCount(); i++ ) addPeakGroup(&(group->children[i]), item);
    return item;
}

void TreeDockWidget::contextMenuEvent ( QContextMenuEvent * event ) 
{
    QMenu menu;

    QAction* z0 = menu.addAction("Copy to Clipboard");
    connect(z0, SIGNAL(triggered()), this ,SLOT(copyToClipbard()));

    QAction *selectedAction = menu.exec(event->globalPos());
}


void TreeDockWidget::copyToClipbard() { 
        qDebug() << "copyToClipbard";
        int itemCount = treeWidget->topLevelItemCount();

        QString clipboardtext;
        for(int i=0; i < itemCount; i++ ) {
            auto item = treeWidget->topLevelItem(i);
            if ( item == NULL) continue;
            itemToClipboard(item,clipboardtext);
        }

		QClipboard *clipboard = QApplication::clipboard();
		clipboard->setText( clipboardtext );
}

void TreeDockWidget::itemToClipboard(QTreeWidgetItem* item, QString& clipboardtext) { 
        for(int j=0; j< item->columnCount();j++ ) {
		    clipboardtext += item->text(j) + "\t";
        }
        clipboardtext += "\n";

        for(int j=0; j< item->childCount(); j++ ) {
            itemToClipboard(item->child(j), clipboardtext);
        }
}           


void TreeDockWidget::setQQQToolBar() {

    QToolBar *toolBar = new QToolBar(this);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24));

    amuQ1 = new QDoubleSpinBox(toolBar);
    amuQ1->setRange(0.001, 2.0);
    amuQ1->setValue(_mainWindow->getSettings()->value("amuQ1").toDouble());
    amuQ1->setSingleStep(0.1);	//amu step
    amuQ1->setToolTip("Precursor mz tolerance");
    amuQ1->setSuffix(" amu");
    amuQ1->setMinimumWidth(20);

    connect(amuQ1, SIGNAL(valueChanged(double)),_mainWindow->getSettingsForm(), SLOT(setQ1Tollrance(double)));
    connect(amuQ1, SIGNAL(valueChanged(double)),_mainWindow,SLOT(showSRMList()));

    amuQ3 = new QDoubleSpinBox(toolBar);
    amuQ3->setRange(0.001, 2.0);
    amuQ3->setValue(_mainWindow->getSettings()->value("amuQ3").toDouble());
    amuQ3->setSingleStep(0.1);	//amu step
    amuQ3->setToolTip("Product mz tolerance");
    amuQ3->setSuffix(" amu");
    amuQ3->setMinimumWidth(20);
    connect(amuQ3, SIGNAL(valueChanged(double)),_mainWindow->getSettingsForm(), SLOT(setQ3Tollrance(double)));
    connect(amuQ3, SIGNAL(valueChanged(double)),_mainWindow,SLOT(showSRMList()));

    associateCompounds = new QToolButton(toolBar);
    associateCompounds->setIcon(QIcon(rsrcPath + "/link.png"));
    associateCompounds->setToolTip(tr("Associate Compounds with MRM Transtions"));
    
    auto q1Label = new QLabel("Q1 tolerance");
    q1Label->setStyleSheet("QLabel { margin-left: 6px; }");
    toolBar->addWidget(q1Label);
    toolBar->addWidget(amuQ1);
    toolBar->addSeparator();
    toolBar->addWidget(new QLabel("Q3 tolerance"));
    toolBar->addWidget(amuQ3);
    toolBar->addSeparator();
    toolBar->addWidget(associateCompounds);

    QWidget* spacer = new QWidget(toolBar);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer);

    QToolButton *closeButton = new QToolButton(toolBar);
    closeButton->setIcon(this->style()->standardIcon(QStyle::SP_DockWidgetCloseButton));
    connect(closeButton,
            &QToolButton::clicked,
            this,
            &TreeDockWidget::hide);
    toolBar->addWidget(closeButton);

    setTitleBarWidget(toolBar);
}

void TreeDockWidget::manualAnnotation(QTreeWidgetItem * item) {

    QVariant v =   item->data(0,Qt::UserRole);

	mzSlice slice = v.value<mzSlice>();
    string srmId = slice.srmId;
    if (srmId.empty()) return;

    matchCompoundMenu = new QMenu("Manual Annotation");
    associateCompounds->setMenu(matchCompoundMenu);
    associateCompounds->setPopupMode(QToolButton::InstantPopup);


    double amuq1 = amuQ1->value();
    double amuq3 = amuQ3->value();

    int polarity = 0;
    if (_mainWindow->getIonizationMode()) polarity = _mainWindow->getIonizationMode();

    deque<Compound*> matchedCompounds = _mainWindow->srmList->getMatchedCompounds(srmId, amuq1, amuq3, polarity);

    matchCompoundMenu->clear();
    connect(matchCompoundMenu, SIGNAL(triggered(QAction*)), SLOT(annotateCompound(QAction*)));

    for (unsigned int i=0; i< matchedCompounds.size(); i++) {
        QAction* action = matchCompoundMenu->addAction(QString::fromStdString(matchedCompounds[i]->name()));
    }


}

void TreeDockWidget::annotateCompound(QAction* action) {

    auto item = treeWidget->currentItem();
    QVariant v =   item->data(0,Qt::UserRole);
    
    auto childItem = item->child(0);
    string existingCompound = childItem->text(0).toStdString();

    mzSlice slice = v.value<mzSlice>();
    string srmId = slice.srmId;
    if (srmId.empty()) return;

    string compoundName = action->text().toStdString();

    if (compoundName == existingCompound) return;

    Q_FOREACH(Compound* compound, _mainWindow->srmList->compoundsDB) {

        if(compoundName == compound->name()) {
            _mainWindow->annotation[srmId] = compound;
        }
    }

    _mainWindow->showSRMList();

}
