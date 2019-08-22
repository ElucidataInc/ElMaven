#ifndef ADDUCTWIDGET_H
#define ADDUCTWIDGET_H

#include "ui_adductwidget.h"
#include "datastructures/adduct.h"
#include "stable.h"

class Adduct;
class Database;

Q_DECLARE_METATYPE(Adduct*)

class AdductWidget : public QDialog, public Ui_AdductWidget {
    Q_OBJECT

    public:
        explicit AdductWidget(QWidget *parent = 0);
        ~AdductWidget();

        void loadAdducts();

    private:

};

#endif // ADDUCTWIDGET_H
