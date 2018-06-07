#ifndef ABOUTPOLLY_H
#define ABOUTPOLLY_H

#include <QWidget>

namespace Ui {
class AboutPolly;
}

class AboutPolly : public QDialog
{
    Q_OBJECT

public:
    AboutPolly();
    ~AboutPolly();
    void cancel();

public slots:
    
private slots:
    
private:
    Ui::AboutPolly *ui;
};

#endif // ABOUTPOLLY_H
