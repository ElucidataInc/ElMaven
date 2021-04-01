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

    auto configLocation = QStandardPaths::QStandardPaths::GenericConfigLocation;
    _writeableTempDir = QStandardPaths::writableLocation(configLocation)
                        + QDir::separator()
                        + "tmp_Elmaven_Polly_files";
    _activeTable = nullptr;
    _pollyIntegration = _mainwindow->getController()->iPolly;
    _loadingDialog = new PollyWaitDialog(this);
    _uploadInProgress = false;
    _lastCohortFileWasValid = false;
    
    gotoPollyButtonAlt->setVisible(false);

    groupSetComboAlt->addItem("All Groups");
    groupSetComboAlt->addItem("Only Good Groups");
    groupSetComboAlt->addItem("Exclude Bad Groups");
    groupSetComboAlt->setCurrentIndex(0);

    // init worker thread
    _worker = new EPIWorkerThread(_pollyIntegration);
    connect(_pollyIntegration, SIGNAL(receivedEPIError(QString)), SLOT(showEPIError(QString)));

    connect(logoutButton, SIGNAL(clicked(bool)), SLOT(_logout()));
    connect(newProjectRadioAlt,
            &QRadioButton::toggled,
            this,
            &PollyElmavenInterfaceDialog::_enableNewProjectUi);
    connect(existingProjectRadioAlt,
            &QRadioButton::toggled,
            this,
            &PollyElmavenInterfaceDialog::_enableExistingProjectUi);
    connect(gotoPollyButtonAlt,
            &QPushButton::clicked,
            this,
            &PollyElmavenInterfaceDialog::_goToPollyProject);
    connect(uploadButtonAlt, SIGNAL(clicked(bool)), SLOT(_uploadDataToPolly()));
    connect(this,
            SIGNAL(uploadFinished(bool)),
            SLOT(_performPostUploadTasks(bool)));
    connect(peakTableComboAlt,
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
        peakTableComboAlt->setCurrentText(table->titlePeakTable->text());
}

void PollyElmavenInterfaceDialog::_goToPollyProject()
{
    _mainwindow->getAnalytics()->hitEvent("PollyDialog",
                                          "DirectedToProject");

    QDesktopServices::openUrl(_projectRedirectionUrl);
}

void PollyElmavenInterfaceDialog::_enableNewProjectUi()
{
    newProjectEntryAlt->setEnabled(true);
    existingProjectComboAlt->setEnabled(false);
    existingProjectRadioAlt->setChecked(false);
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::_enableExistingProjectUi()
{
    existingProjectComboAlt->setEnabled(true);
    newProjectEntryAlt->setEnabled(false);
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
        statusUpdateAlt->setStyleSheet("QLabel { color : green;}");
        statusUpdateAlt->clear();
    }
    usernameLabel->setText("");
    uploadButtonAlt->setEnabled(false);
    existingProjectComboAlt->clear();

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

void PollyElmavenInterfaceDialog::_handleProjects(QVariantMap projectNameIdMap)
{
    _projectNameIdMap = projectNameIdMap;
    startupDataLoad();
}

void PollyElmavenInterfaceDialog::_resetUiElements()
{
    // if upload is already in progress, refuse this UI reset request
    if (_uploadInProgress)
        return;

    peakTableComboAlt->clear();

    newProjectEntryAlt->setEnabled(true);
    newProjectRadioAlt->setChecked(true);
    newProjectEntryAlt->clear();

    existingProjectRadioAlt->setChecked(false);
    existingProjectComboAlt->clear();

    uploadButtonAlt->setEnabled(true);
    _showPollyButtonIfUrlExists();

    statusUpdateAlt->clear();
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::startupDataLoad()
{
    _resetUiElements();
    if (_projectNameIdMap.isEmpty()) {
        _projectNameIdMap = _pollyIntegration->getUserProjects();
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
    peakTableComboAlt->setCurrentIndex(peakTableComboAlt->count() - 1);
    _reviseGroupOptions(peakTableComboAlt->currentText());

    // set to active table if available
    if (_activeTable && _tableNameMapping.values().contains(_activeTable)) {
        QString tableName = _tableNameMapping.key(_activeTable);
        peakTableComboAlt->setCurrentText(tableName);

        // reset active table
        _activeTable = nullptr;
    }
}

void PollyElmavenInterfaceDialog::_hideFormIfNotLicensed()
{
    gotoPollyButtonAlt->setDefault(true);   
}

void PollyElmavenInterfaceDialog::_showPollyButtonIfUrlExists()
{
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
    auto modelAlt = dynamic_cast<QStandardItemModel*>(groupSetComboAlt->model());

    // if any group is good, enable "Only Good Groups"
    auto itemAlt = modelAlt->item(1, 0);
    if (anyGood) {
        itemAlt->setEnabled(true);
    } else {
        itemAlt->setEnabled(false);
    }

    // if all groups are bad, disable "Exclude Bad Groups"
    itemAlt = modelAlt->item(2, 0);
    if (allBad) {
        itemAlt->setEnabled(false);
    } else {
        itemAlt->setEnabled(true);
    }
}

void PollyElmavenInterfaceDialog::_addTableIfPossible(TableDockWidget* table,
                                                      QString tableName)
{
    if (!table->getGroups().isEmpty()) {
        peakTableComboAlt->addItem(tableName);
        _tableNameMapping[tableName] = table;
    }
}

void PollyElmavenInterfaceDialog::_uploadDataToPolly()
{
    _uploadInProgress = true;
    peakTableComboAlt->setEnabled(false);
    groupSetComboAlt->setEnabled(false);
    sendModeTab->setEnabled(false);
    projectOptionsAlt->setEnabled(false);
    uploadButtonAlt->setEnabled(false);
    gotoPollyButtonAlt->setVisible(false);

    _mainwindow->getAnalytics()->hitEvent("Exports", "Polly");
    _mainwindow->getAnalytics()->hitEvent("Polly upload", "Project");

    statusUpdateAlt->setStyleSheet("QLabel { color : green;}");
    statusUpdateAlt->setText("Connecting…");
    QCoreApplication::processEvents();
    // check for active internet connection
    ErrorStatus response = _pollyIntegration->activeInternet();
    if (response == ErrorStatus::Failure ||
        response == ErrorStatus::Error) {
        statusUpdateAlt->setText("No internet access.");
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
    
    filenames = _prepareSessionFiles(datetimestamp);
    
    if (filenames.isEmpty()) {
        statusUpdateAlt->setStyleSheet("QLabel { color : red;}");
        statusUpdateAlt->setText("File preparation failed.");
        _loadingDialog->close();
        QCoreApplication::processEvents();
        emit uploadFinished(false);
        return;
    }

    statusUpdateAlt->setStyleSheet("QLabel { color : green;}");
    statusUpdateAlt->setText("Sending files to Polly…");
    QCoreApplication::processEvents();

    _pollyProjectId = _getProjectId();
    if (_pollyProjectId.isEmpty()) {
        emit uploadFinished(false);
        statusUpdateAlt->clear();
        return;
    }

    // check for active internet again before upload
    ErrorStatus response2 = _pollyIntegration->activeInternet();
    if (response2 == ErrorStatus::Failure ||
        response2 == ErrorStatus::Error) {
        statusUpdateAlt->setStyleSheet("QLabel {color : red;}");
        statusUpdateAlt->setText("Internet connection interrupted");
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
    if (existingProjectRadioAlt->isChecked()) {
        QStringList keys = _projectNameIdMap.keys();
        QString projectId =
            _projectNameIdMap.key(existingProjectComboAlt->currentText());

        if (projectId.isEmpty())
            _showErrorMessage("Error",
                              "No such project on Polly.",
                              QMessageBox::Warning);
        return projectId;
    }

    if (newProjectRadioAlt->isChecked()) {
        if (newProjectEntryAlt->text().isEmpty()) {
            _showErrorMessage("Error",
                              "Invalid project name. "
                              "Please enter a non-empty name.",
                              QMessageBox::Warning);
            return "";
        }

        QString newProjectId =
            _pollyIntegration->createProjectOnPolly(newProjectEntryAlt->text());
        _projectNameIdMap.insert(newProjectId, newProjectEntryAlt->text());
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

    if (!patchId.isEmpty()) {
        redirectionUrl = _getProjectRedirectionUrl(uploadProjectIdThread);
        _projectRedirectionUrl = QUrl(redirectionUrl);
    }

    if (!redirectionUrl.isEmpty()) {
        qDebug() << "Redirection URL:" << redirectionUrl;

        _performPostUploadTasks(true);
        statusUpdateAlt->clear();
        _showPollyButtonIfUrlExists();

        // send an email to the user, this is a way of persisting their URLs
        QString project = _projectNameIdMap.value(_pollyProjectId).toString();
        QString emailSubject = QString("Data successfully uploaded to Polly "
                                       "project - \"%1\"").arg(project);
        QString emailContent = QString("<a href='%1'></a>").arg(redirectionUrl);
        QString appname = "";
        
        appname = "project";
        
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
PollyElmavenInterfaceDialog::_getProjectRedirectionUrl(QString projectId)
{
    return _pollyIntegration->getProjectUrl(projectId);
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
    uploadButtonAlt->setEnabled(true);
    peakTableComboAlt->setEnabled(true);
    groupSetComboAlt->setEnabled(true);
    projectOptionsAlt->setEnabled(true);
    sendModeTab->setEnabled(true);
    statusUpdateAlt->setEnabled(true);
    if (uploadSuccessful) {
        statusUpdateAlt->setStyleSheet("QLabel { color : green; }");
    } else {
        statusUpdateAlt->setStyleSheet("QLabel { color : red; }");
    }
    statusUpdateAlt->clear();
}

MainWindow* PollyElmavenInterfaceDialog::getMainWindow()
{
    return _mainwindow;
}
