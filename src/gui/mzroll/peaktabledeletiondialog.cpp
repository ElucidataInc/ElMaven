#include "peaktabledeletiondialog.h"
#include "ui_peaktabledeletiondialog.h"

PeakTableDeletionDialog::PeakTableDeletionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PeakTableDeletionDialog)
{
    ui->setupUi(this);
    connect(this, SIGNAL(accepted()),
            parent, SLOT(destroy()));
    connect(this, SIGNAL(rejected()),
            this, SLOT(hide()));
}

PeakTableDeletionDialog::~PeakTableDeletionDialog()
{
    delete ui;
}
