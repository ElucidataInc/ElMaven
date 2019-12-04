#include "common/mixpanel.h"
#include "infodialog.h"
#include "mainwindow.h"
#include "ui_infodialog.h"

InfoDialog::InfoDialog(MainWindow* parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    auto tracker = parent->getUsageTracker();
    auto name = tracker->userAttribute("Name").toString();
    auto email = tracker->userAttribute("Email").toString();
    if (email != "") {
        ui->nameEntry->setText(name);
        ui->emailEntry->setText(email);
        ui->nameEntry->setDisabled(true);
        ui->emailEntry->setDisabled(true);
        ui->signInButton->setText("Sign out");
    }

    connect(ui->signInButton, &QPushButton::clicked, [this, tracker] {
        if (ui->signInButton->text() == "Sign out") {
            tracker->updateUser("Name", "");
            tracker->updateUser("Email", "");
            ui->nameEntry->clear();
            ui->emailEntry->clear();
            ui->nameEntry->setEnabled(true);
            ui->emailEntry->setEnabled(true);
            ui->signInButton->setText("Sign in");
            return;
        }

        auto name = ui->nameEntry->text();
        auto email = ui->emailEntry->text();
        QRegularExpression emailPattern("^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~"
                                        "-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]"
                                        "{0,61}[a-zA-Z0-9])?(?:\\.[a-zA"
                                        "-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-"
                                        "zA-Z0-9])?)*$");
        auto match = emailPattern.match(email);
        if (match.hasMatch()) {
            tracker->updateUser("Name", name);
            tracker->updateUser("Email", email);
            ui->nameEntry->setDisabled(true);
            ui->emailEntry->setDisabled(true);
            ui->signInButton->setText("Sign out");
        }
    });
    connect(ui->docButton, &QPushButton::clicked, [this, tracker] {
        QDesktopServices::openUrl(QUrl("https://resources.elucidata.io/elmaven/"
                                       "#resources"));
        QMap<QString, QVariant> eventDetails;
        eventDetails["Clicked link"] = "Tutorials and documentation";
        tracker->trackEvent("InfoDialog", eventDetails);
    });
    connect(ui->academyButton, &QPushButton::clicked, [this, tracker] {
        QDesktopServices::openUrl(QUrl("https://resources.elucidata.io/elmaven/"
                                       "#academy"));
        QMap<QString, QVariant> eventDetails;
        eventDetails["Clicked link"] = "El-MAVEN Academy";
        tracker->trackEvent("InfoDialog", eventDetails);
    });
    connect(ui->citeButton, &QPushButton::clicked, [this, tracker] {
        QDesktopServices::openUrl(QUrl("https://github.com/ElucidataInc/ElMaven"
                                       "#how-to-cite"));
        QMap<QString, QVariant> eventDetails;
        eventDetails["Clicked link"] = "Cite El-MAVEN";
        tracker->trackEvent("InfoDialog", eventDetails);
    });
}

InfoDialog::~InfoDialog()
{
    delete ui;
}
