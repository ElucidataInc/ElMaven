#ifndef ADDUCTWIDGET_H
#define ADDUCTWIDGET_H

#include "ui_adductwidget.h"

class AdductWidget : public QDialog, public Ui_AdductWidget {
    Q_OBJECT

public:
    explicit AdductWidget(QWidget *parent = 0);
    ~AdductWidget();

private:

};

#endif // ADDUCTWIDGET_H
