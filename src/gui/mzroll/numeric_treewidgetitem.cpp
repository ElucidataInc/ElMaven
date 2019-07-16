#include "numeric_treewidgetitem.h"

bool NumericTreeWidgetItem::operator<( const QTreeWidgetItem & other ) const{
    int sortCol = treeWidget()->sortColumn();
    QString thisText = text(sortCol);
    QString otherText = other.text(sortCol);

    QCollator collator;
    collator.setNumericMode(true);

    QRegExp exp1("^[-+]?([0-9]*\\.?[0-9]+)[eE][-+]?([0-9]+)$");
    QRegExp exp2("^[-+]?([0-9]*\\.?[0-9]+)[eE][-+]?([0-9]+)$");
    if (thisText.contains(exp1) && otherText.contains(exp2)) {
        QString thisMantissa = exp1.cap(1);
        QString thisExponent = exp1.cap(2);
        QString otherMantissa = exp2.cap(1);
        QString otherExponent = exp2.cap(2);

        if (collator.compare(thisExponent, otherExponent) == 0) {
            return collator.compare(thisMantissa, otherMantissa) < 0;
        }
        return collator.compare(thisExponent, otherExponent) < 0;
    }

    return collator.compare(thisText , otherText) < 0;
}
