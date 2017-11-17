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


	bool operator< ( const QTreeWidgetItem & other ) const  
	{
		int sortCol = treeWidget()->sortColumn();
		bool isaFloat;
		bool isbFloat;
		float a = text(sortCol).toFloat(&isaFloat);
		float b = other.text(sortCol).toFloat(&isbFloat);

		if (isaFloat & isbFloat) {
			return  a < b;
		}
		else if(a){
			return true;
		}
		else if(b){
			return false;
		}


		QString text1=text(sortCol);
		QString text2=other.text(sortCol);
		int length1=text1.length();
		int length2=text2.length();

		if(length1==length2) {
			return text1<text2;
		}
		else return length1<length2;

    }


};

#endif
