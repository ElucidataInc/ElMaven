#ifndef PEAKTABLEDELETIONDIALOG_H
#define PEAKTABLEDELETIONDIALOG_H

#include <QDialog>

namespace Ui {
class PeakTableDeletionDialog;
}

class PeakTableDeletionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PeakTableDeletionDialog(QWidget *parent = 0);
    ~PeakTableDeletionDialog();

private:
    Ui::PeakTableDeletionDialog *ui;
};

#endif // PEAKTABLEDELETIONDIALOG_H
