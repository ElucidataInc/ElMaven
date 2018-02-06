#ifndef NUMERIC_ITEM_H
#define NUMERIC_ITEM_H

#include "stable.h"
#include <QDrag>
#include <QMimeData>
#include <QDebug>

class NumericTreeWidgetItem : public QTreeWidgetItem
{

public:
	NumericTreeWidgetItem(QTreeWidgetItem *parent) : QTreeWidgetItem(parent){}
	NumericTreeWidgetItem(QTreeWidgetItem *parent, int type = Type):QTreeWidgetItem(parent,type){}

	NumericTreeWidgetItem(QTreeWidget *tree, int type = Type):QTreeWidgetItem(tree,type){}
	NumericTreeWidgetItem(QTreeWidget *tree): QTreeWidgetItem(tree){}
	NumericTreeWidgetItem(QTreeWidget *tree, const QStringList & strings) : QTreeWidgetItem (tree,strings){}


	bool operator< ( const QTreeWidgetItem & other ) const ;
};

#endif
