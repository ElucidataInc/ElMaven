#include "treedockwidget.h"



TreeDockWidget::TreeDockWidget(MainWindow*, QString title, int numColms) {
		treeWidget=new QTreeWidget(this);
		treeWidget->setColumnCount(numColms);
		treeWidget->setObjectName(title);
		connect(treeWidget,SIGNAL(itemSelectionChanged()), SLOT(showInfo()));
		treeWidget->setHeaderHidden(true);

        //QShortcut* ctrlA = new QShortcut(QKeySequence(tr("Ctrl+A", "Select All")), this);
        //connect(ctrlA,SIGNAL(activated()),treeWidget,SLOT(selectAll())); 

        //QShortcut* ctrlC = new QShortcut(QKeySequence(tr("Ctrl+C", "Copy Items")), this);
        //connect(ctrlC,SIGNAL(activated()),treeWidget,SLOT(copyToClipbard())); 

		setWidget(treeWidget);
		setWindowTitle(title);
		setObjectName(title);
}


QTreeWidgetItem* TreeDockWidget::addItem(QTreeWidgetItem* parentItem, string key , float value, int type=0) {
	QTreeWidgetItem *item = new QTreeWidgetItem(parentItem,type);
	item->setText(0, QString(key.c_str()));
	item->setText(1, QString::number(value,'f',3));
	return item;
}

QTreeWidgetItem* TreeDockWidget::addItem(QTreeWidgetItem* parentItem, string key , string value, int type=0) {
	QTreeWidgetItem *item = new QTreeWidgetItem(parentItem,type);
	item->setText(0, QString(key.c_str()));
	if (! value.empty() ) { item->setText(1, QString(value.c_str())); }
	return item;
}

void TreeDockWidget::clearTree() { 
		treeWidget->clear();
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

			QTreeWidgetItem *parent = new QTreeWidgetItem(treeWidget,mzSliceType);
			parent->setText(0,tag);
  	    	parent->setData(0,Qt::UserRole,QVariant::fromValue(*slice));

			if (x) { 
				addCompound(x, parent);
				parent->setText(0, tr("%1 %2").arg(QString(x->name.c_str()), tag ));
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

        foreach(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
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
					mainwindow->getSpectraWidget()->setScan(scan);
					mainwindow->getEicWidget()->setFocusLine(scan->rt);
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
                                        qDebug() << "showInfo() mzSlice: " << slice.srmId.c_str();
                        } else {
                                cerr << "UNKNOWN TYPE=" << v.type() << endl;
                        }
        }
}

QTreeWidgetItem* TreeDockWidget::addPeak(Peak* peak, QTreeWidgetItem* parent) {
    if (peak == NULL ) return NULL;
	QTreeWidgetItem *item=NULL;
    if ( parent == NULL ) {
	    item = new QTreeWidgetItem(treeWidget,PeakType);
    } else {
	    item = new QTreeWidgetItem(parent,PeakType);
    }

}

void TreeDockWidget::keyPressEvent(QKeyEvent *e ) {
	QTreeWidgetItem *item = treeWidget->currentItem();
	if (e->key() == Qt::Key_Delete ) { 
    	if ( item->type() == PeakGroupType ) {
			unlinkGroup(); 
		} else if ( item->type() == CompoundType  ) {
			unlinkGroup();
		}
	}
}
	
void TreeDockWidget::unlinkGroup() {
	QTreeWidgetItem *item = treeWidget->currentItem();
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
		cpd->unlinkGroup(); 
    	mainwindow->setCompoundFocus(cpd);
	} else if (group && group->parent) {
		PeakGroup* parentGroup = group->parent;
		if ( parentGroup->deleteChild(group) ) {
			QTreeWidgetItem* parentItem = item->parent();
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
					if (group->hasCompoundLink() && group->getCompound()->getPeakGroup() == group ) {
							group->getCompound()->unlinkGroup();
							cerr << "Unlinking compound" << group << endl;
					}
					break;

			}
		}
	}

	treeWidget->update();
    mainwindow->getPathwayWidget()->updateCompoundConcentrations();

	return;
}



QTreeWidgetItem* TreeDockWidget::addSlice(mzSlice* s, QTreeWidgetItem* parent) {
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget,mzSliceType);
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

    QTreeWidgetItem *item0 = new QTreeWidgetItem(treeWidget,mzLinkType);
    item0->setText(0,QString::number(links[0].mz1,'f',4));
    item0->setExpanded(true);
    for(int i=0; i < links.size(); i++) addLink(&links[i],item0);

    treeWidget->setSortingEnabled(true);
    treeWidget->sortByColumn(1,Qt::DescendingOrder);
}

QTreeWidgetItem* TreeDockWidget::addLink(mzLink* s,QTreeWidgetItem* parent)  {

        if (!s) return NULL;

        QTreeWidgetItem *item=NULL;
    if( parent == NULL ){
            item = new QTreeWidgetItem(treeWidget,mzLinkType);
        } else {
            item = new QTreeWidgetItem(parent,mzLinkType);
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
        QTreeWidgetItem *parent = new QTreeWidgetItem(treeWidget, PathwayType);
        parent->setText(0,QString((p->name +"("+p->id+")").c_str() 	));
        parent->setData(0,Qt::UserRole,QVariant::fromValue(QString(p->id.c_str())));
    }
}

void TreeDockWidget::filterTree(QString needle) {
        int itemCount = treeWidget->topLevelItemCount();
        for(int i=0; i < itemCount; i++ ) {
                QTreeWidgetItem *item = treeWidget->topLevelItem(i);
                if ( item == NULL) continue;
                if ( needle.isEmpty() || item->text(0).contains(needle,Qt::CaseInsensitive) ) {
                        item->setHidden(false);
                } else {
                        item->setHidden(true);
                }
        }
}

void TreeDockWidget::addScanItem(Scan* scan) {
        if (scan == NULL) return;
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget,ScanType);
        item->setData(0,Qt::UserRole,QVariant::fromValue(scan));	
        item->setText(0,QString::number(scan->precursorMz,'f',4));	
        item->setText(1,QString::number(scan->rt));
        item->setText(2,QString::number(scan->scannum));
        item->setText(3,QString(scan->sample->sampleName.c_str()));
        item->setText(4,QString(scan->filterLine.c_str()));
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
        QTreeWidgetItem* item = treeWidget->topLevelItem(i);
        if ( item->type() != PeakGroupType ) continue;
        QVariant v = item->data(0,Qt::UserRole);
        PeakGroup*  g = v.value<PeakGroup*>();
        if (g && g == group ) return true;

        for(int j=0; j < item->childCount();j++ ) {
            QTreeWidgetItem* item2 = item->child(j);
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
    QTreeWidgetItem* item = NULL;

    if ( parent == NULL ) {
            item = new QTreeWidgetItem(treeWidget, CompoundType);
    } else {
            item = new QTreeWidgetItem(parent, CompoundType);
    }

        if (!item) return NULL;

        QString id = QString(c->name.c_str());
        item->setText(0,id);
    item->setData(0, Qt::UserRole,QVariant::fromValue(c));

        if ( c->hasGroup() ){
                        PeakGroup* group = c->getPeakGroup();
                        if (group != NULL) addPeakGroup(group,item);
        }
    return item;
}


QTreeWidgetItem* TreeDockWidget::addPeakGroup(PeakGroup* group, QTreeWidgetItem* parent) {
    if (group == NULL) return NULL;
    QTreeWidgetItem* item = NULL;

    if ( parent == NULL ) {
            item = new QTreeWidgetItem(treeWidget, PeakGroupType);
    } else {
            item = new QTreeWidgetItem(parent, PeakGroupType);
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
                QTreeWidgetItem *item = treeWidget->topLevelItem(i);
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


