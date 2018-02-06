#include "numeric_treewidgetitem.h"

bool NumericTreeWidgetItem::operator<( const QTreeWidgetItem & other ) const{
    int sortCol = treeWidget()->sortColumn();
    QString thisText=text(sortCol);
    QString otherText=other.text(sortCol);
    
    QCollator collator;
    collator.setNumericMode(true);
    return collator.compare(thisText , otherText) < 0;

}