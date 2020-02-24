#ifndef ADDUCTWIDGET_H
#define ADDUCTWIDGET_H

#include "ui_adductwidget.h"
#include "datastructures/adduct.h"
#include "stable.h"

class Adduct;
class Database;
class MainWindow;

Q_DECLARE_METATYPE(Adduct*)

class AdductWidget : public QDialog, public Ui_AdductWidget {
    Q_OBJECT

    public:
        explicit AdductWidget(MainWindow* parent = nullptr);
        void loadAdducts();
        vector<Adduct*> getSelectedAdducts();

    public slots:
        void selectAdductsForCurrentPolarity();

    protected:
        void showEvent(QShowEvent* event);
        void hideEvent(QHideEvent* event);

    private:
        MainWindow*_mw;
};

#endif // ADDUCTWIDGET_H
