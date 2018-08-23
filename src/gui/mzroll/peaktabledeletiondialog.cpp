#include "peaktabledeletiondialog.h"
#include "ui_peaktabledeletiondialog.h"

PeakTableDeletionDialog::PeakTableDeletionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PeakTableDeletionDialog)
{
    ui->setupUi(this);
}

PeakTableDeletionDialog::~PeakTableDeletionDialog()
{
    delete ui;
}
