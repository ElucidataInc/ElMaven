#ifndef PROJECTFORM_H
#define PROJECTFORM_H

#include <QWidget>
#include "mainwindow.h"
#include "pollyintegration.h"

class PollyIntegration;
namespace Ui {
class ProjectForm;
}

class ProjectForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectForm(MainWindow *parent=nullptr);
    ProjectForm(MainWindow *parent,PollyIntegration* pollyintegration);
    ~ProjectForm();
    QVariantMap projectnames_id;
    PollyIntegration* _pollyintegration;
    QString projectname;
    MainWindow* _mainwindow;
private slots:
    void on_pushButton_clicked();

private:
    Ui::ProjectForm *ui;
};

#endif // PROJECTFORM_H
