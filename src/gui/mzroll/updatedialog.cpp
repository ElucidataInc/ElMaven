#include "common/mixpanel.h"
#include "mainwindow.h"
#include "updatedialog.h"
#include "ui_updatedialog.h"

UpdateDialog::UpdateDialog(MainWindow *parent) :
      QDialog(parent),
      ui(new Ui::UpdateDialog)
{
    _mw = parent;
    _updateAllowed = false;

    ui->setupUi(this);
    ui->updateButton->setDefault(true);
    ui->statusLabel->setStyleSheet("QLabel { color: red; }");

    connect(ui->updateButton,
            &QPushButton::clicked,
            this,
            &UpdateDialog::_update);
    connect(ui->doneButton,
            &QPushButton::clicked,
            this,
            &UpdateDialog::_finish);
    connect(ui->cancelButton,
            &QPushButton::clicked,
            this,
            &UpdateDialog::close);
    connect(ui->cancelButtonAlt,
            &QPushButton::clicked,
            this,
            &UpdateDialog::close);

    setModal(true);
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

void UpdateDialog::_update()
{
    auto name = _mw->getUsageTracker()->userAttribute("Name").toString();
    auto email = _mw->getUsageTracker()->userAttribute("Email").toString();
    ui->nameEntry->setText(name);
    ui->emailEntry->setText(email);

    if (name.isEmpty() || email.isEmpty()) {
        ui->stackedWidget->setCurrentIndex(1);
        ui->doneButton->setDefault(true);
        return;
    }
    _finish();
}

void UpdateDialog::_finish()
{
    auto name = ui->nameEntry->text();
    auto email = ui->emailEntry->text();

    QRegularExpression emailPattern("^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~"
                                    "-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]"
                                    "{0,61}[a-zA-Z0-9])?(?:\\.[a-zA"
                                    "-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-"
                                    "zA-Z0-9])?)*$");
    auto match = emailPattern.match(email);
    if (!name.isEmpty() && match.hasMatch()) {
        _mw->getUsageTracker()->updateUser("Name", name);
        _mw->getUsageTracker()->updateUser("Email", email);
        _updateAllowed = true;
        close();
    } else if (name.isEmpty()) {
        ui->statusLabel->setText("No name provided.");
        ui->statusLabel->show();
    } else {
        ui->statusLabel->setText("Invalid email address.");
        ui->statusLabel->show();
    }
}
