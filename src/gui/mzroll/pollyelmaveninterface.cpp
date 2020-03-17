#include <QVariant>

#include "common/analytics.h"
#include "controller.h"
#include "csvreports.h"
#include "database.h"
#include "ligandwidget.h"
#include "loginform.h"
#include "mainwindow.h"
#include "mavenparameters.h"
#include "mzfileio.h"
#include "pollyelmaveninterface.h"
#include "pollywaitdialog.h"
#include "projectdockwidget.h"
#include "tabledockwidget.h"

PollyElmavenInterfaceDialog::PollyElmavenInterfaceDialog(MainWindow* mw)
    : QDialog(mw), _mainwindow(mw), _loginform(nullptr)
{
    setupUi(this);
    setModal(true);

    qRegisterMetaType<PollyApp>("PollyApp");
    qRegisterMetaType<QMap<PollyApp, bool>>();

    auto configLocation = QStandardPaths::QStandardPaths::GenericConfigLocation;
    _writeableTempDir = QStandardPaths::writableLocation(configLocation)
                        + QDir::separator()
                        + "tmp_Elmaven_Polly_files";
    _activeTable = nullptr;
    _pollyIntegration = _mainwindow->getController()->iPolly;
    _loadingDialog = new PollyWaitDialog(this);
    _uploadInProgress = false;
    _lastCohortFileWasValid = false;

    workflowMenu->setStyleSheet("QListView::item {"
                                "border: 1px solid transparent;"
                                "border-radius: 4px;"
                                "padding: 6px;"
                                "}"
                                "QListView::item:selected {"
                                "border-color: rgb(150, 150, 255);"
                                "background-color: rgb(235, 235, 255);"
                                "}"
                                "QListView::item:enabled {"
                                "color: black;"
                                "}"
                                "QListView::item:disabled {"
                                "color: gray;"
                                "}"
                                "QListView {"
                                "outline: 0;"
                                "}");
    workflowMenu->setCurrentRow(int(PollyApp::FirstView));
    gotoPollyButton->setVisible(false);
    gotoPollyButtonAlt->setVisible(false);

    groupSetCombo->addItem("All Groups");
    groupSetComboAlt->addItem("All Groups");
    groupSetCombo->addItem("Only Good Groups");
    groupSetComboAlt->addItem("Only Good Groups");
    groupSetCombo->addItem("Exclude Bad Groups");
    groupSetComboAlt->addItem("Exclude Bad Groups");
    groupSetCombo->setCurrentIndex(0);
    groupSetComboAlt->setCurrentIndex(0);

    _selectedMode = SendMode::PollyApp;

    // init worker thread
    _worker = new EPIWorkerThread(_pollyIntegration);
    connect(_pollyIntegration, SIGNAL(receivedEPIError(QString)), SLOT(showEPIError(QString)));

    connect(logoutButton, SIGNAL(clicked(bool)), SLOT(_logout()));
    connect(workflowMenu,
            SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this,
            SLOT(_changePage()));
    connect(peakTableCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            peakTableComboAlt,
            &QComboBox::setCurrentIndex);
    connect(peakTableComboAlt,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            peakTableCombo,
            &QComboBox::setCurrentIndex);
    connect(groupSetCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            groupSetComboAlt,
            &QComboBox::setCurrentIndex);
    connect(groupSetComboAlt,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            groupSetCombo,
            &QComboBox::setCurrentIndex);
    connect(newProjectRadio,
            &QRadioButton::toggled,
            this,
            &PollyElmavenInterfaceDialog::_enableNewProjectUi);
    connect(newProjectRadio,
            &QRadioButton::toggled,
            newProjectRadioAlt,
            &QRadioButton::setChecked);
    connect(newProjectRadioAlt,
            &QRadioButton::toggled,
            newProjectRadio,
            &QRadioButton::setChecked);
    connect(newProjectEntry,
            &QLineEdit::textChanged,
            [this](QString text) {
                int position = newProjectEntryAlt->cursorPosition();
                newProjectEntryAlt->setText(text);
                newProjectEntryAlt->setCursorPosition(position);
            });
    connect(newProjectEntryAlt,
            &QLineEdit::textChanged,
            [this](QString text) {
                int position = newProjectEntry->cursorPosition();
                newProjectEntry->setText(text);
                newProjectEntry->setCursorPosition(position);
            });
    connect(existingProjectRadio,
            &QRadioButton::toggled,
            this,
            &PollyElmavenInterfaceDialog::_enableExistingProjectUi);
    connect(existingProjectRadio,
            &QRadioButton::toggled,
            existingProjectRadioAlt,
            &QRadioButton::setChecked);
    connect(existingProjectRadioAlt,
            &QRadioButton::toggled,
            existingProjectRadio,
            &QRadioButton::setChecked);
    connect(existingProjectCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            existingProjectComboAlt,
            &QComboBox::setCurrentIndex);
    connect(existingProjectComboAlt,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            existingProjectCombo,
            &QComboBox::setCurrentIndex);
    connect(gotoPollyButton,
            &QPushButton::clicked,
            this,
            &PollyElmavenInterfaceDialog::_goToPollyApp);
    connect(gotoPollyButtonAlt,
            &QPushButton::clicked,
            this,
            &PollyElmavenInterfaceDialog::_goToPollyProject);
    connect(uploadButton, SIGNAL(clicked(bool)), SLOT(_uploadDataToPolly()));
    connect(uploadButtonAlt, SIGNAL(clicked(bool)), SLOT(_uploadDataToPolly()));
    connect(this,
            SIGNAL(uploadFinished(bool)),
            SLOT(_performPostUploadTasks(bool)));
    connect(tutorialLink,
            &QLabel::linkActivated,
            [=](const QString &link) {
                QDesktopServices::openUrl(QUrl(link));
                auto appName = PollyIntegration::stringForApp(_selectedApp);
                _mainwindow->getAnalytics()->hitEvent("PollyDialog",
                                                      "ViewTutorial",
                                                      appName);
            });
    connect(demoButton,
            &QPushButton::clicked,
            [=] {
                QDesktopServices::openUrl(
                    QUrl("https://calendly.com/elucidata/polly-demo"));
                auto appName = PollyIntegration::stringForApp(_selectedApp);
                _mainwindow->getAnalytics()->hitEvent("PollyDialog",
                                                      "DemoButton",
                                                      appName);
            });
    connect(peakTableCombo,
            &QComboBox::currentTextChanged,
            this,
            &PollyElmavenInterfaceDialog::_reviseGroupOptions);

    connect(_worker,
            SIGNAL(projectsReady(QVariantMap)),
            this,
            SLOT(_handleProjects(QVariantMap)));
    connect(_worker,
            SIGNAL(licensesReady(QMap<PollyApp, bool>)),
            this,
            SLOT(_handleLicenses(QMap<PollyApp, bool>)));
    connect(_worker,
            SIGNAL(authenticationFinished(QString, QString)),
            this,
            SLOT(_handleAuthentication(QString, QString)));
    connect(_worker,
            SIGNAL(filesUploaded(QStringList, QString, QString)),
            this,
            SLOT(_performPostFilesUploadTasks(QStringList, QString, QString)));
    connect(sendModeTab,
            &QTabWidget::currentChanged,
            this,
            &PollyElmavenInterfaceDialog::_changeMode);
}

PollyElmavenInterfaceDialog::~PollyElmavenInterfaceDialog()
{
    qDebug() << "exiting PollyElmavenInterfaceDialog now....";
    if (_loginform)
        delete (_loginform);
    if (_worker)
        delete _worker;
}

void EPIWorkerThread::run()
{
    switch (_currentMethod) {
    case RunMethod::AuthenticateAndFetchData:
        _authenticateUserAndFetchData();
        break;
    case RunMethod::UploadFiles:
        _uploadFiles();
        break;
    case RunMethod::SendEmail:
        _sendEmail();
        break;
    default:
        break;
    }
}

void EPIWorkerThread::_authenticateUserAndFetchData()
{
    qDebug() << "Checking for active internet connection…";
    QString status;
    ErrorStatus response = _pollyIntegration->activeInternet();
    if (response == ErrorStatus::Failure ||
        response == ErrorStatus::Error) {
        status = "error";
        emit authenticationFinished("", status);
        return;
    }

    qDebug() << "Authenticating…";
    ErrorStatus loginResponse = _pollyIntegration->authenticateLogin("", "");
    if (loginResponse == ErrorStatus::Failure ||
        loginResponse == ErrorStatus::Error) {
        emit authenticationFinished(_pollyIntegration->getCurrentUsername(),
                                    status);
        return;
    } else {
        status = "ok";
        emit authenticationFinished(_pollyIntegration->getCurrentUsername(),
                                    status);
    }

    qDebug() << "Fetching licenses from Polly…";
    if (status == "ok") {
        QMap<PollyApp, bool> licenseMap =
            _pollyIntegration->getAppLicenseStatus();
        emit licensesReady(licenseMap);
    }

    qDebug() << "Fetching projects from Polly…";
    if (status == "ok") {
        QVariantMap _projectNameIdMap = _pollyIntegration->getUserProjects();
        emit projectsReady(_projectNameIdMap);
    }
}

void EPIWorkerThread::_uploadFiles()
{
    qDebug() << "Uploading files to polly…";
    // re-login to polly may be required because the token expires after 30
    // minutes..
    _pollyIntegration->authenticateLogin("", "");
    QStringList patchId;
    QPair<ErrorStatus, QStringList> resultAndError = _pollyIntegration->exportData(_uploadArgs.filesToUpload,
                                                        _uploadArgs.pollyProjectId);


    if (resultAndError.first == ErrorStatus::Success)
        patchId = resultAndError.second;
    else
        return;

    emit filesUploaded(patchId,
                       _uploadArgs.pollyProjectId,
                       _uploadArgs.datetimestamp);
    _removeFilesFromDir(_uploadArgs.dir, _uploadArgs.filesToUpload);
}

void EPIWorkerThread::_sendEmail()
{
    qDebug() << "Sending email to user…";
    ErrorStatus emailSent = _pollyIntegration->sendEmail(_emailArgs.username,
                                                  _emailArgs.subject,
                                                  _emailArgs.content,
                                                  _emailArgs.appname);
    qDebug() << (emailSent == ErrorStatus::Success
                     ? "Sent an email containing URL to user."
                     : "Failed to send email to user.");
}

void EPIWorkerThread::_removeFilesFromDir(QDir dir, QStringList files)
{
    for (auto fileName : files) {
        dir.remove(fileName);
    }
}

void PollyElmavenInterfaceDialog::setSelectedTable(TableDockWidget *table)
{
    if (table)
        peakTableCombo->setCurrentText(table->titlePeakTable->text());
}

void PollyElmavenInterfaceDialog::switchToApp(PollyApp app)
{
    workflowMenu->setCurrentRow(static_cast<int>(app));
}

void PollyElmavenInterfaceDialog::_changePage()
{
    QCoreApplication::processEvents();
    _selectedApp = PollyApp(workflowMenu->currentRow());
    if (_selectedApp == PollyApp::FirstView) {
        viewTitle->setText("Upload to Polly™ FirstView");
        viewTitleAdvert->setText("Polly™ FirstView");
        appAdvertLabel->setText(
            "Polly supports pre-clinical research efforts in pharma and "
            "biotech labs with plans to develop into a discovery platform that "
            "be customized for a variety of lab-specific applications. It is "
            "an application suite that has been developed for the analysis, "
            "quantitation, and presentation of mass spectrometry data with an "
            "advanced distributed data management infrastructure on the Amazon "
            "Cloud. This FirstView application allows you to preview your data "
            "from El-MAVEN on Polly."
        );
        tutorialLink->setText("");
    } else if (_selectedApp == PollyApp::PollyPhi) {
        viewTitle->setText("Upload to PollyPhi™ Relative");
        viewTitleAdvert->setText("PollyPhi™ Relative");
        appAdvertLabel->setText(
            "Relative flux (φ) workflow allows the user to process LCMS "
            "labeled targeted single time point data with insightful "
            "visualizations. Go from raw data to visualizations in less than 5 "
            "minutes. The application handles C13 labeled, single time point "
            "experiments, provides automated peak picking with manual "
            "curation, allows quality checking samples to identify and reject "
            "outliers, performs natural abundance correction, calculates "
            "fractional enrichment, pool total values, and allows the user to "
            "visualize them."
        );
        tutorialLink->setText("<a href='https://www.youtube.com/watch?v=LiMoEGXbMyo'>View Tutorial</a>");
    } else if (_selectedApp == PollyApp::QuantFit) {
        viewTitle->setText("Upload to Polly™ QuantFit");
        viewTitleAdvert->setText("Polly™ QuantFit");
        appAdvertLabel->setText(
            "For some analysis like kinetic flux analysis, absolute "
            "concentrations of the metabolites are required. PollyTM QuantFit "
            "supports calibration - the process of quantifying samples of "
            "unknown concentrations with known (standard) samples. In order to "
            "quantify metabolites, experiments are done with standard samples "
            "of known concentrations. Using these data points we get "
            "mathematical mappings from intensities to concentrations which "
            "are used to calculate concentrations of experimental intensities. "
            "It further allows normalization of data with respect to internal "
            "standards and allows scaling which neutralizes the effect of "
            "initial dilution."
        );
        tutorialLink->setText("<a href='https://www.youtube.com/watch?v=Ma5Ti3GRayE'>View Tutorial</a>");
    }

    startupDataLoad();
}

void PollyElmavenInterfaceDialog::_changeMode()
{
    int index = sendModeTab->currentIndex();

    QString tabText = sendModeTab->tabText(index);
    _mainwindow->getAnalytics()->hitEvent("PollyDialog",
                                          "SendModeChanged",
                                          tabText);

    if (index == 0) {
        // send to a Polly app
        _selectedMode = SendMode::PollyApp;
    } else {
        // send only to Polly project
        _selectedMode = SendMode::PollyProject;
    }
    _hideFormIfNotLicensed();
}

void PollyElmavenInterfaceDialog::_goToPollyApp()
{
    QString appName = "";
    if (_selectedApp == PollyApp::FirstView) {
        appName = "FirstView";
    } else if (_selectedApp == PollyApp::PollyPhi) {
        appName = "PollyPhi";
    } else if (_selectedApp == PollyApp::QuantFit) {
        appName = "QuantFit";
    }
    _mainwindow->getAnalytics()->hitEvent("PollyDialog",
                                          "DirectedToApp",
                                          appName);

    QDesktopServices::openUrl(_redirectionUrlMap[_selectedApp]);
}

void PollyElmavenInterfaceDialog::_goToPollyProject()
{
    _mainwindow->getAnalytics()->hitEvent("PollyDialog",
                                          "DirectedToProject");

    QDesktopServices::openUrl(_projectRedirectionUrl);
}

void PollyElmavenInterfaceDialog::_enableNewProjectUi()
{
    newProjectEntry->setEnabled(true);
    newProjectEntryAlt->setEnabled(true);
    existingProjectCombo->setEnabled(false);
    existingProjectComboAlt->setEnabled(false);
    existingProjectRadio->setChecked(false);
    existingProjectRadioAlt->setChecked(false);
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::_enableExistingProjectUi()
{
    existingProjectCombo->setEnabled(true);
    existingProjectComboAlt->setEnabled(true);
    newProjectEntry->setEnabled(false);
    newProjectEntryAlt->setEnabled(false);
    newProjectRadio->setChecked(false);
    newProjectRadioAlt->setChecked(false);
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::initialSetup()
{
    int nodeStatus = _pollyIntegration->checkNodeExecutable();
    if (nodeStatus == 0) {
        QMessageBox msgBox(this);
        msgBox.setWindowModality(Qt::NonModal);
        msgBox.setWindowTitle("Node is not installed on this system");
        msgBox.exec();
        return;
    }
    int askForLogin = _pollyIntegration->askForLogin();
    if (askForLogin == 1) {
        try {
            _callLoginForm();
        } catch (...) {
            QMessageBox msgBox(this);
            msgBox.setWindowModality(Qt::NonModal);
            msgBox.setWindowTitle("Error in loading login form");
            msgBox.exec();
        }
        return;
    } else {
        try {
            _callInitialEPIForm();
        } catch (...) {
            QMessageBox msgBox(this);
            msgBox.setWindowModality(Qt::NonModal);
            msgBox.setWindowTitle("Error in loading Polly dialog");
            msgBox.exec();
        }
        return;
    }
}

void PollyElmavenInterfaceDialog::showEPIError(QString errorMessage)
{
    _resetUiElements();
    _showErrorMessage("Polly Error", errorMessage, QMessageBox::NoIcon);
    _performPostUploadTasks(false);
    _populateProjects();
    _populateTables();
    _hideFormIfNotLicensed();
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::_callLoginForm()
{
    _loginform = new LoginForm(this);
    _loginform->setModal(true);
    _loginform->show();
}

void PollyElmavenInterfaceDialog::_callInitialEPIForm()
{
    show();
    if (!_uploadInProgress) {
        statusUpdate->setStyleSheet("QLabel { color : green;}");
        statusUpdate->clear();
        statusUpdateAlt->setStyleSheet("QLabel { color : green;}");
        statusUpdateAlt->clear();
    }
    usernameLabel->setText("");
    uploadButton->setEnabled(false);
    uploadButtonAlt->setEnabled(false);
    existingProjectCombo->clear();

    _worker->wait();
    _worker->setMethodToRun(EPIWorkerThread::RunMethod::AuthenticateAndFetchData);
    _worker->start();

    _loadingDialog->setWindowFlag(Qt::WindowTitleHint, true);
    _loadingDialog->open();
    _loadingDialog->statusLabel->setVisible(true);
    _loadingDialog->statusLabel->setStyleSheet("QLabel {color : green;}");
    _loadingDialog->statusLabel->setText("Authenticating…");
    _loadingDialog->label->setVisible(true);
    _loadingDialog->label->setMovie(_loadingDialog->movie);
    _loadingDialog->label->setAlignment(Qt::AlignCenter);
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::_handleAuthentication(QString username,
                                                        QString status)
{
    if (status == "ok") {
        _loadingDialog->statusLabel->setStyleSheet("QLabel {color : green;}");
        _loadingDialog->statusLabel->setText("Fetching user data…");
        usernameLabel->setText(username);
        QCoreApplication::processEvents();
    } else if (status == "error") {
        _loadingDialog->statusLabel->setStyleSheet("QLabel {color : red;}");
        _loadingDialog->statusLabel->setText("No internet access.");
        QCoreApplication::processEvents();
        QTimer* timer = new QTimer(this);
        QObject::connect(timer,
                         &QTimer::timeout,
                         [this, timer]() {
                             delete timer;
                             _loadingDialog->close();
                             this->close();
                         });
        timer->start(5000);
    } else {
        _loadingDialog->statusLabel->setStyleSheet("QLabel {color : red;}");
        _loadingDialog->statusLabel->setText("Authentication failed. "
                                             "Please login again.");
        QCoreApplication::processEvents();
        _logout();
    }
}

void PollyElmavenInterfaceDialog::_handleLicenses(QMap<PollyApp, bool> licenseMap)
{
    _licenseMap = licenseMap;
}

void PollyElmavenInterfaceDialog::_handleProjects(QVariantMap projectNameIdMap)
{
    _projectNameIdMap = projectNameIdMap;
    _changePage();
}

void PollyElmavenInterfaceDialog::_resetUiElements()
{
    // if upload is already in progress, refuse this UI reset request
    if (_uploadInProgress)
        return;

    peakTableCombo->clear();
    peakTableComboAlt->clear();

    newProjectEntry->setEnabled(true);
    newProjectRadio->setChecked(true);
    newProjectEntryAlt->setEnabled(true);
    newProjectRadioAlt->setChecked(true);
    newProjectEntry->clear();
    newProjectEntryAlt->clear();

    existingProjectRadio->setChecked(false);
    existingProjectRadioAlt->setChecked(false);
    existingProjectCombo->clear();
    existingProjectComboAlt->clear();

    uploadButton->setEnabled(true);
    uploadButtonAlt->setEnabled(true);
    _showPollyButtonIfUrlExists();

    statusUpdate->clear();
    statusUpdateAlt->clear();
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::startupDataLoad()
{
    _resetUiElements();
    if (_projectNameIdMap.isEmpty()) {
        _projectNameIdMap = _pollyIntegration->getUserProjects();
    }
    if (_licenseMap.isEmpty()) {
        _licenseMap = _pollyIntegration->getAppLicenseStatus();
        _changePage();
        return;
    }

    _populateProjects();
    _populateTables();

    _loadingDialog->close();
    _hideFormIfNotLicensed();
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::_populateProjects()
{
    QStringList keys = _projectNameIdMap.keys();
    for (auto key : keys) {
        existingProjectCombo->addItem(_projectNameIdMap[key].toString());
        existingProjectComboAlt->addItem(_projectNameIdMap[key].toString());
    }
}

void PollyElmavenInterfaceDialog::_populateTables()
{
    _bookmarkTable = _mainwindow->getBookmarkedPeaks();
    _addTableIfPossible(_bookmarkTable, "Bookmark Table ");

    QList<QPointer<TableDockWidget>> peaksTableList =
        _mainwindow->getPeakTableList();
    for (auto peakTable : peaksTableList) {
        QString peakTableName =
                TableDockWidget::getTitleForId(peakTable->tableId);
        _addTableIfPossible(peakTable, peakTableName);
    }

    // set latest table by default
    peakTableCombo->setCurrentIndex(peakTableCombo->count() - 1);
    _reviseGroupOptions(peakTableCombo->currentText());

    // set to active table if available
    if (_activeTable && _tableNameMapping.values().contains(_activeTable)) {
        QString tableName = _tableNameMapping.key(_activeTable);
        peakTableCombo->setCurrentText(tableName);

        // reset active table
        _activeTable = nullptr;
    }
}

void PollyElmavenInterfaceDialog::_hideFormIfNotLicensed()
{
    if (!_licenseMap.value(_selectedApp)) {
        stackedWidget->setCurrentWidget(advertBox);
        demoButton->setDefault(true);
    } else if (_selectedMode == SendMode::PollyApp) {
        stackedWidget->setCurrentWidget(pollyForm);
        gotoPollyButton->setDefault(true);
    } else if (_selectedMode == SendMode::PollyProject) {
        gotoPollyButtonAlt->setDefault(true);
    }
}

void PollyElmavenInterfaceDialog::_showPollyButtonIfUrlExists()
{
    if (_selectedApp == PollyApp::FirstView) {
        gotoPollyButton->setText("Go to FirstView");
    } else if (_selectedApp == PollyApp::PollyPhi) {
        gotoPollyButton->setText("Start Fluxing");
    } else if (_selectedApp == PollyApp::QuantFit) {
        gotoPollyButton->setText("Start Quantification");
    }

    if (_redirectionUrlMap[_selectedApp].isEmpty()) {
        gotoPollyButton->setVisible(false);
    } else if (!_uploadInProgress) {
        gotoPollyButton->setVisible(true);
    }

    if (_projectRedirectionUrl.isEmpty()) {
        gotoPollyButtonAlt->setVisible(false);
    } else {
        gotoPollyButtonAlt->setVisible(true);
    }
}

void PollyElmavenInterfaceDialog::_reviseGroupOptions(QString tableName)
{
    auto peakTable = _tableNameMapping.value(tableName, nullptr);
    if (peakTable == nullptr)
        return;

    auto groups = peakTable->getGroups();
    bool anyGood = false;
    bool allBad = true;
    for (auto group : groups) {
        if (group->label == 'g')
            anyGood = true;
        if (group->label != 'b')
            allBad = false;
    }
    auto model = dynamic_cast<QStandardItemModel*>(groupSetCombo->model());
    auto modelAlt = dynamic_cast<QStandardItemModel*>(groupSetComboAlt->model());

    // if any group is good, enable "Only Good Groups"
    auto item = model->item(1, 0);
    auto itemAlt = modelAlt->item(1, 0);
    if (anyGood) {
        item->setEnabled(true);
        itemAlt->setEnabled(true);
    } else {
        item->setEnabled(false);
        itemAlt->setEnabled(false);
    }

    // if all groups are bad, disable "Exclude Bad Groups"
    item = model->item(2, 0);
    itemAlt = modelAlt->item(2, 0);
    if (allBad) {
        item->setEnabled(false);
        itemAlt->setEnabled(false);
    } else {
        item->setEnabled(true);
        itemAlt->setEnabled(true);
    }
}

void PollyElmavenInterfaceDialog::_addTableIfPossible(TableDockWidget* table,
                                                      QString tableName)
{
    if (!table->getGroups().isEmpty() && table->getTargetedGroupCount() > 0) {
        if (_selectedApp == PollyApp::PollyPhi
            && table->getLabeledGroupCount() > 0) {
            peakTableCombo->addItem(tableName);
        } else if (_selectedApp == PollyApp::FirstView
                   || _selectedApp == PollyApp::QuantFit) {
            peakTableCombo->addItem(tableName);
        }
        peakTableComboAlt->addItem(tableName);
        _tableNameMapping[tableName] = table;
    }
}

void PollyElmavenInterfaceDialog::_uploadDataToPolly()
{
    _uploadInProgress = true;
    gotoPollyButton->setVisible(false);
    uploadButton->setEnabled(false);
    peakTableCombo->setEnabled(false);
    peakTableComboAlt->setEnabled(false);
    groupSetCombo->setEnabled(false);
    groupSetComboAlt->setEnabled(false);
    projectOptions->setEnabled(false);
    workflowMenu->setEnabled(false);
    sendModeTab->setEnabled(false);
    projectOptionsAlt->setEnabled(false);
    uploadButtonAlt->setEnabled(false);
    gotoPollyButtonAlt->setVisible(false);

    _mainwindow->getAnalytics()->hitEvent("Exports", "Polly");
    if (_selectedMode == SendMode::PollyProject) {
        _mainwindow->getAnalytics()->hitEvent("Polly upload", "Project");
    } else if (_selectedApp == PollyApp::FirstView) {
        _mainwindow->getAnalytics()->hitEvent("Polly upload", "FirstView");
    } else if (_selectedApp == PollyApp::PollyPhi) {
        _mainwindow->getAnalytics()->hitEvent("Polly upload", "PollyPhi");
    } else if (_selectedApp == PollyApp::QuantFit) {
        _mainwindow->getAnalytics()->hitEvent("Polly upload", "QuantFit");
    }

    statusUpdate->setStyleSheet("QLabel { color : green;}");
    statusUpdate->setText("Connecting…");
    statusUpdateAlt->setStyleSheet("QLabel { color : green;}");
    statusUpdateAlt->setText("Connecting…");
    QCoreApplication::processEvents();
    // check for active internet connection
    ErrorStatus response = _pollyIntegration->activeInternet();
    if (response == ErrorStatus::Failure ||
        response == ErrorStatus::Error) {
        statusUpdate->setText("No internet access.");
        statusUpdateAlt->setText("No internet access.");
        uploadButton->setEnabled(true);
        uploadButtonAlt->setEnabled(true);
        return;
    }

    // redirect to login form if user credentials have not been saved
    int askForLogin = _pollyIntegration->askForLogin();
    if (askForLogin == 1) {
        _callLoginForm();
        emit uploadFinished(false);
        return;
    }

    statusUpdate->setStyleSheet("QLabel {color : green; }");
    statusUpdate->setText("Preparing files…");
    statusUpdateAlt->setStyleSheet("QLabel {color : green; }");
    statusUpdateAlt->setText("Preparing files…");
    QCoreApplication::processEvents();

    // create a temporary directory to store polly files
    qDebug() << "writing Polly temp file in this directory -"
             << _writeableTempDir;
    QDir qdir(_writeableTempDir);
    if (!qdir.exists()) {
        QDir().mkdir(_writeableTempDir);
    }

    // clear all temporary files that might not have been deleted from preivous
    // operations.
    qdir.setFilter(QDir::AllEntries);
    foreach(QString dirFile, qdir.entryList())
    {
        qdir.remove(dirFile);
    }

    // add datetimestamp to identify files from one upload thread
    QDateTime current_time;
    const QString format = "dd-MM-yyyy_hh_mm_ss";
    QString datetimestamp = current_time.currentDateTime().toString(format);
    datetimestamp.replace(" ", "_");
    datetimestamp.replace(":", "-");

    QStringList filenames;
    if (_selectedMode == SendMode::PollyProject) {
        filenames = _prepareSessionFiles(datetimestamp);
    } else {
        filenames = _prepareFilesToUpload(qdir, datetimestamp);
    }
    if (filenames.isEmpty()) {
        statusUpdate->setStyleSheet("QLabel { color : red;}");
        statusUpdate->setText("File preparation failed.");
        statusUpdateAlt->setStyleSheet("QLabel { color : red;}");
        statusUpdateAlt->setText("File preparation failed.");
        _loadingDialog->close();
        QCoreApplication::processEvents();
        emit uploadFinished(false);
        return;
    }

    statusUpdate->setStyleSheet("QLabel { color : green;}");
    statusUpdate->setText("Sending files to Polly…");
    statusUpdateAlt->setStyleSheet("QLabel { color : green;}");
    statusUpdateAlt->setText("Sending files to Polly…");
    QCoreApplication::processEvents();

    _pollyProjectId = _getProjectId();
    if (_pollyProjectId.isEmpty()) {
        emit uploadFinished(false);
        statusUpdate->clear();
        statusUpdateAlt->clear();
        return;
    }

    // check for active internet again before upload
    ErrorStatus response2 = _pollyIntegration->activeInternet();
    if (response2 == ErrorStatus::Failure ||
        response2 == ErrorStatus::Error) {
        statusUpdate->setStyleSheet("QLabel {color : red;}");
        statusUpdate->setText("Internet connection interrupted");
        statusUpdateAlt->setStyleSheet("QLabel {color : red;}");
        statusUpdateAlt->setText("Internet connection interrupted");
        uploadButton->setEnabled(true);
        uploadButtonAlt->setEnabled(true);
        return;
    }

    _worker->wait();
    _worker->setMethodToRun(EPIWorkerThread::RunMethod::UploadFiles);
    _worker->setUploadArgs(qdir, filenames, datetimestamp, _pollyProjectId);
    _worker->start();
}

QString PollyElmavenInterfaceDialog::_getProjectId()
{
    if (existingProjectRadio->isChecked()) {
        QStringList keys = _projectNameIdMap.keys();
        QString projectId =
            _projectNameIdMap.key(existingProjectCombo->currentText());

        if (projectId.isEmpty())
            _showErrorMessage("Error",
                              "No such project on Polly.",
                              QMessageBox::Warning);
        return projectId;
    }

    if (newProjectRadio->isChecked()) {
        if (newProjectEntry->text().isEmpty()) {
            _showErrorMessage("Error",
                              "Invalid project name. "
                              "Please enter a non-empty name.",
                              QMessageBox::Warning);
            return "";
        }

        QString newProjectId =
            _pollyIntegration->createProjectOnPolly(newProjectEntry->text());
        _projectNameIdMap.insert(newProjectId, newProjectEntry->text());
        return newProjectId;
    }

    _showErrorMessage("Error",
                      "Please select at least one option",
                      QMessageBox::Warning);
    return "";
}

void PollyElmavenInterfaceDialog::_showErrorMessage(QString title,
                                                    QString message,
                                                    QMessageBox::Icon icon)
{
    QMessageBox msgBox(icon, title, message, QMessageBox::Ok, _mainwindow);
    msgBox.exec();
}

void PollyElmavenInterfaceDialog::_performPostFilesUploadTasks(QStringList patchId,
                                                               QString uploadProjectIdThread,
                                                               QString datetimestamp)
{
    QCoreApplication::processEvents();
    QString redirectionUrl = "";

    if (!patchId.isEmpty() && _selectedMode == SendMode::PollyProject) {
        redirectionUrl = _getProjectRedirectionUrl(uploadProjectIdThread);
        _projectRedirectionUrl = QUrl(redirectionUrl);
    } else if (!patchId.isEmpty()) {
        redirectionUrl = _getAppRedirectionUrl(datetimestamp,
                                               uploadProjectIdThread);
        _redirectionUrlMap[_selectedApp] = QUrl(redirectionUrl);
    }

    if (!redirectionUrl.isEmpty()) {
        qDebug() << "Redirection URL:" << redirectionUrl;

        _performPostUploadTasks(true);
        statusUpdate->clear();
        statusUpdateAlt->clear();
        _showPollyButtonIfUrlExists();

        // send an email to the user, this is a way of persisting their URLs
        QString project = _projectNameIdMap.value(_pollyProjectId).toString();
        QString emailSubject = QString("Data successfully uploaded to Polly "
                                       "project - \"%1\"").arg(project);
        QString emailContent = QString("<a href='%1'></a>").arg(redirectionUrl);
        QString appname = "";
        if (_selectedMode == SendMode::PollyProject) {
            appname = "project";
        } else {
            switch (_selectedApp) {
            case PollyApp::FirstView:
                appname = "firstview";
                break;
            case PollyApp::PollyPhi:
                appname = "pollyphi";
                break;
            case PollyApp::QuantFit:
                appname = "quantfit";
                break;
            default:
                break;
            }
        }

        _worker->wait();
        _worker->setMethodToRun(EPIWorkerThread::RunMethod::SendEmail);
        _worker->setEmailArgs(usernameLabel->text(),
                              emailSubject,
                              emailContent,
                              appname);
        _worker->start();
    } else {
        _performPostUploadTasks(false);
    }
}

QString
PollyElmavenInterfaceDialog::_getAppRedirectionUrl(QString datetimestamp,
                                                   QString uploadProjectIdThread)
{
    QString redirectionUrl = "";

    switch (_selectedApp) {
    case PollyApp::FirstView: {
        QString componentId = 
            _pollyIntegration->obtainComponentId(PollyApp::FirstView);
        
        redirectionUrl =
            _pollyIntegration->getComponentEndpoint(componentId,
                                                    uploadProjectIdThread,
                                                    datetimestamp);
        break;
    } case PollyApp::PollyPhi: {
        QString landingPage = QString("relative_lcms_elmaven");
        QString workflowId = 
            _pollyIntegration->obtainWorkflowId(PollyApp::PollyPhi);
        QString workflowName = _pollyIntegration->obtainComponentName(PollyApp::PollyPhi);
        QString workflowRequestId =
            _pollyIntegration->createWorkflowRequest(uploadProjectIdThread,
                                                     workflowName,
                                                     workflowId);
        if (workflowRequestId.isEmpty())
            return redirectionUrl;

        // send to google sheets if sample cohort file is not valid
        if (!_lastCohortFileWasValid)
            landingPage = QString("gsheet_sym_polly_elmaven");

        if (workflowId == "-1")
            return redirectionUrl;

        redirectionUrl = 
            _pollyIntegration->getWorkflowEndpoint(workflowId,
                                                   workflowRequestId,
                                                   landingPage,
                                                   uploadProjectIdThread,
                                                   datetimestamp);
        break;
    } case PollyApp::QuantFit: {
        QString componentId =
            _pollyIntegration->obtainComponentId(PollyApp::QuantFit);
        if (componentId == "-1")
            return redirectionUrl;

        QString runRequestId =
            _pollyIntegration->createRunRequest(componentId,
                                                uploadProjectIdThread);
        if (runRequestId.isEmpty())
            return redirectionUrl;

        redirectionUrl = _pollyIntegration->getComponentEndpoint(componentId,
                                                                 runRequestId,
                                                                 datetimestamp);
        break;
    } default:
        break;
    }

    return redirectionUrl;
}

QString
PollyElmavenInterfaceDialog::_getProjectRedirectionUrl(QString projectId)
{
    return _pollyIntegration->getProjectUrl(projectId);
}

QStringList PollyElmavenInterfaceDialog::_prepareFilesToUpload(QDir qdir,
                                                               QString datetimestamp)
{
    QStringList filenames;
    TableDockWidget* peakTable = nullptr;

    // check for no peak tables
    if (peakTableCombo->currentIndex() == -1) {
        _showErrorMessage("Error",
                          "No peak tables available. Either there are "
                          "no peak tables in the current session or "
                          "the existing ones are not suitable for "
                          "the selected Polly app.",
                          QMessageBox::Warning);
        return filenames;
    }

    peakTable = _tableNameMapping[peakTableCombo->currentText()];

    QCoreApplication::processEvents();

    // Now uploading the Compound DB that was used for peak detection.
    // This is needed for Elmaven->Firstview->PollyPhi relative LCMS workflow.
    // ToDo Kailash, Keep track of compound DB used for each peak table,
    // As of now, uploading what is currently there in the compound section of
    // Elmaven.
    QString compoundDb = _mainwindow->ligandWidget->getDatabaseName();
    _mainwindow->ligandWidget->saveCompoundList(_writeableTempDir
                                                + QDir::separator()
                                                + datetimestamp
                                                + "_Compound_DB_Elmaven.csv",
                                                compoundDb);

    qDebug() << "Uploading all groups, needed for firstview app…";

    // Preparing the CSV file
    QCoreApplication::processEvents();

    if (groupSetCombo->currentIndex() == 0) {
        peakTable->wholePeakSet();
    } else if (groupSetCombo->currentIndex() == 1) {
        peakTable->goodPeakSet();
    } else if (groupSetCombo->currentIndex() == 2) {
        peakTable->excludeBadPeakSet();
    }

    peakTable->treeWidget->selectAll();
    peakTable->prepareDataForPolly(_writeableTempDir,
                                   "Groups Summary Matrix Format "
                                   "Comma Delimited (*.csv)",
                                    datetimestamp + "_Peak_table_all_");

    // Preparing the json file
    QCoreApplication::processEvents();

    QString jsonFilename = _writeableTempDir
                            + QDir::separator()
                            + datetimestamp
                            + "_Peaks_information_json_Elmaven_Polly.json";

    if (_selectedApp == PollyApp::PollyPhi) {
        peakTable->exportJsonToPolly(_writeableTempDir, jsonFilename, true);
        QCoreApplication::processEvents();
        //Preparing the sample cohort file
        QString sampleCohortFileName = _writeableTempDir + QDir::separator() + datetimestamp +
                                        "_Cohort_Mapping_Elmaven.csv";
        _mainwindow->projectDockWidget->prepareSampleCohortFile(sampleCohortFileName);
        _lastCohortFileWasValid = _pollyIntegration->validSampleCohort(sampleCohortFileName);

        CSVReports csvrpt;
        QList<PeakGroup *> selectedGroups = peakTable->getSelectedGroups();
        std::list<PeakGroup> groups;

        for (int i = 0; i < peakTable->allgroups.size(); i++) {
            if (selectedGroups.contains(&peakTable->allgroups[i])) {
                  groups.push_back(peakTable->allgroups[i]);
            }
        }
        QString modelFile = _writeableTempDir
                            + QDir::separator()
                            + datetimestamp
                            + "_Cloud_model_mapping_file.csv";
        csvrpt.writeDataForPolly(modelFile.toStdString(), groups);
    } else
        peakTable->exportJsonToPolly(_writeableTempDir, jsonFilename, false);

    // Saving settings file
    QByteArray ba = (_writeableTempDir + QDir::separator()
                     + datetimestamp
                     + "_maven_analysis_settings"
                     + ".xml").toLatin1();
    const char* savePath = ba.data();
    _mainwindow->mavenParameters->saveSettings(savePath);
    qDebug() << "Settings saved";

    qdir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList fileList = qdir.entryInfoList();

    for (auto fileInfo : fileList) {
        QString tmp_filename = _writeableTempDir
                               + QDir::separator()
                               + fileInfo.fileName();
        filenames.append(tmp_filename);
    }

    return filenames;
}

QStringList
PollyElmavenInterfaceDialog::_prepareSessionFiles(QString datetimestamp)
{
    QStringList filenames;

    // check for no peak tables
    if (peakTableComboAlt->currentIndex() == -1) {
        _showErrorMessage("Error",
                          "No peak tables available. Either there are "
                          "no peak tables in the current session or "
                          "the existing ones are not suitable for "
                          "the selected Polly app.",
                          QMessageBox::Warning);
        return filenames;
    }

    auto peakTable = _tableNameMapping[peakTableComboAlt->currentText()];
    if (peakTable->getGroups().isEmpty()) {
        _showErrorMessage("Error",
                          "The selected peak table does not have any "
                          "peak-groups. Cancelling upload.",
                          QMessageBox::Warning);
        return filenames;
    }

    if (groupSetComboAlt->currentIndex() == 0) {
        peakTable->wholePeakSet();
    } else if (groupSetComboAlt->currentIndex() == 1) {
        peakTable->goodPeakSet();
    } else if (groupSetComboAlt->currentIndex() == 2) {
        peakTable->excludeBadPeakSet();
    }

    QString tableName = TableDockWidget::getTitleForId(peakTable->tableId);
    tableName.replace(" ", "_"); // replace spaces with underscores
    QString jsonFilename = _writeableTempDir
                           + QDir::separator()
                           + datetimestamp
                           + "_"
                           + tableName
                           + ".json";
    peakTable->exportJsonToPolly(_writeableTempDir, jsonFilename, false);
    filenames.append(jsonFilename);

    QCoreApplication::processEvents();

    QString groupCsvFilename = datetimestamp
                               + "_"
                               + tableName
                               + "_groups"
                               + ".csv";
    peakTable->treeWidget->selectAll();
    peakTable->prepareDataForPolly(_writeableTempDir,
                                   "Groups Summary Matrix Format "
                                   "Comma Delimited (*.csv)",
                                    groupCsvFilename);
    filenames.append(_writeableTempDir
                     + QDir::separator()
                     + groupCsvFilename);

    QCoreApplication::processEvents();

    QString peakCsvFilename = datetimestamp
                              + "_"
                              + tableName
                              + "_peaks"
                              + ".csv";
    peakTable->treeWidget->selectAll();
    peakTable->prepareDataForPolly(_writeableTempDir,
                                   "Peaks Detailed Format "
                                   "Comma Delimited (*.csv)",
                                    peakCsvFilename);
    filenames.append(_writeableTempDir
                     + QDir::separator()
                     + peakCsvFilename);

    QCoreApplication::processEvents();

    if (filenames.isEmpty()) {
        _showErrorMessage("Error",
                          "Unable to prepare files for selected peak table. "
                          "Cancelling upload.",
                          QMessageBox::Warning);
        return filenames;
    }

    // write a CSV file for each compound DB
    auto databases = DB.getDatabaseNames();
    for (auto& elem : databases) {
        auto compoundCount = elem.second;
        if (compoundCount <= 0)
            continue;

        QString compoundDbName = QString::fromStdString(elem.first);
        QString compoundDbNameCopy = compoundDbName;
        compoundDbNameCopy.replace(" ", "_");
        QString compoundDbFilename = _writeableTempDir
                                     + QDir::separator()
                                     + datetimestamp
                                     + "_"
                                     + compoundDbNameCopy
                                     + "_compound_db.csv";
        _mainwindow->ligandWidget->saveCompoundList(compoundDbFilename,
                                                    compoundDbName);
        filenames.append(compoundDbFilename);
    }

    // write a CSV file for cohort mapping (if available)
    QString sampleCohortFilename = _writeableTempDir
                                   + QDir::separator()
                                   + datetimestamp
                                   + "_cohort_mapping.csv";
    _mainwindow->projectDockWidget->prepareSampleCohortFile(sampleCohortFilename);
    if (_pollyIntegration->validSampleCohort(sampleCohortFilename))
        filenames.append(sampleCohortFilename);

    QCoreApplication::processEvents();

    // write an emDB file for the entire session
    QString emdbFilename = _writeableTempDir
                           + QDir::separator()
                           + datetimestamp
                           + "_"
                           + "session.emDB";
    if (_mainwindow->fileLoader->writeSQLiteProjectForPolly(emdbFilename)) {
        filenames.append(emdbFilename);
    } else {
        _showErrorMessage("Error",
                          "Unable to generate session (emDB) file. "
                          "Cancelling upload.",
                          QMessageBox::Warning);
        QDir dir(_writeableTempDir);
        for (auto filename : filenames) {
            dir.remove(filename);
        }
        filenames.clear();
    }

    QCoreApplication::processEvents();
    return filenames;
}

void PollyElmavenInterfaceDialog::_logout()
{
    usernameLabel->setText("");
    _licenseMap.clear();
    _redirectionUrlMap.clear();
    _lastCohortFileWasValid = false;
    _pollyIntegration->logout();
    _projectNameIdMap = QVariantMap();
    _loadingDialog->close();
    QCoreApplication::processEvents();
    close();
}

void PollyElmavenInterfaceDialog::_performPostUploadTasks(bool uploadSuccessful)
{
    _uploadInProgress = false;
    uploadButton->setEnabled(true);
    uploadButtonAlt->setEnabled(true);
    peakTableCombo->setEnabled(true);
    peakTableComboAlt->setEnabled(true);
    groupSetCombo->setEnabled(true);
    groupSetComboAlt->setEnabled(true);
    projectOptions->setEnabled(true);
    projectOptionsAlt->setEnabled(true);
    workflowMenu->setEnabled(true);
    sendModeTab->setEnabled(true);
    statusUpdate->setEnabled(true);
    statusUpdateAlt->setEnabled(true);
    if (uploadSuccessful) {
        statusUpdate->setStyleSheet("QLabel { color : green; }");
        statusUpdateAlt->setStyleSheet("QLabel { color : green; }");
    } else {
        statusUpdate->setStyleSheet("QLabel { color : red; }");
        statusUpdateAlt->setStyleSheet("QLabel { color : red; }");
    }
    statusUpdate->clear();
    statusUpdateAlt->clear();
}

MainWindow* PollyElmavenInterfaceDialog::getMainWindow()
{
    return _mainwindow;
}
