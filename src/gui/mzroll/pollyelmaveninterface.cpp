#include "pollyelmaveninterface.h"
#include <QVariant>
#include "csvreports.h"
#include <string>


PollyElmavenInterfaceDialog::PollyElmavenInterfaceDialog(MainWindow* mw)
    : QDialog(mw), _mainwindow(mw), _loginform(nullptr)
{
    setupUi(this);
    setModal(true);

    qRegisterMetaType<PollyApp>("PollyApp");

    auto configLocation = QStandardPaths::QStandardPaths::GenericConfigLocation;
    _writeableTempDir = QStandardPaths::writableLocation(configLocation)
                        + QDir::separator()
                        + "tmp_Elmaven_Polly_files";
    _activeTable = nullptr;
    _pollyIntegration = new PollyIntegration();
    _loadingDialog = new PollyWaitDialog(this);

    workflowMenu->setCurrentRow(int(PollyApp::FirstView));
    gotoPollyButton->setVisible(false);
    gotoPollyButton->setDefault(true);

    connect(logoutButton, SIGNAL(clicked(bool)), SLOT(_logout()));
    connect(workflowMenu,
            SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this,
            SLOT(_changePage()));
    connect(newProjectRadio,
            SIGNAL(clicked(bool)),
            SLOT(_enableNewProjectUi()));
    connect(existingProjectRadio,
            SIGNAL(clicked(bool)),
            SLOT(_enableExistingProjectUi()));
    connect(gotoPollyButton, SIGNAL(clicked(bool)), SLOT(_goToPolly()));
    connect(uploadButton, SIGNAL(clicked(bool)), SLOT(_uploadDataToPolly()));
    connect(this,
            SIGNAL(uploadFinished(bool)),
            SLOT(_performPostUploadTasks(bool)));
}

PollyElmavenInterfaceDialog::~PollyElmavenInterfaceDialog()
{
    qDebug() << "exiting PollyElmavenInterfaceDialog now....";
    if (_loginform)
        delete (_loginform);
}

EPIWorkerThread::EPIWorkerThread()
{
    _pollyintegration = new PollyIntegration();
}

void EPIWorkerThread::run()
{
    if (state == "initial_setup") {
        qDebug() << "Checking for active internet connection..";
        QString status;
        if (!_pollyintegration->activeInternet()) {
            status = "error";
            emit authentication_result("", status);
            return;
        }

        qDebug() << "Authenticating..";
        status = _pollyintegration->authenticateLogin("", "");
        emit authentication_result(_pollyintegration->getCurrentUsername(),
                                   status);

        qDebug() << "Fetching projects from Polly..";
        if (status == "ok") {
            QVariantMap _projectNameIdMap = _pollyintegration->getUserProjects();
            emit resultReady(_projectNameIdMap);
        }
    } else if (state == "upload_files") {
        qDebug() << "starting thread for uploading files to polly..";
        // re-login to polly may be required because the token expires after 30
        // minutes..
        QString statusInside = _pollyintegration->authenticateLogin("", "");
        QStringList patchId = _pollyintegration->exportData(filesToUpload,
                                                            uploadProjectIdThread);
        emit filesUploaded(patchId, uploadProjectIdThread, datetimestamp);
    } else if (state == "send_email") {
        qDebug() << "starting thread for sending email to user…";
        bool emailSent = _pollyintegration->sendEmail(username,
                                                      filesToUpload[0],
                                                      filesToUpload[1],
                                                      filesToUpload[2]);
        QString message = emailSent ? "Sent an email containing URL to user."
                                    : "Failed to send email to user.";
        qDebug() << message;
    }
}

EPIWorkerThread::~EPIWorkerThread()
{
    for (auto fileName : filesToUpload) {
        tmpDir.remove(fileName);
    }
    if (_pollyintegration)
        delete (_pollyintegration);
};

void PollyElmavenInterfaceDialog::_changePage()
{
    _selectedApp = PollyApp(workflowMenu->currentRow());
    if (_selectedApp == PollyApp::FirstView) {
        viewTitle->setText("Upload to Polly™ FirstView");
    } else if (_selectedApp == PollyApp::Fluxomics) {
        viewTitle->setText("Upload to PollyPhi™ Relative");
    } else if (_selectedApp == PollyApp::QuantFit) {
        viewTitle->setText("Upload to Polly™ QuantFit");
    }

    startupDataLoad();
}

void PollyElmavenInterfaceDialog::_goToPolly()
{
    QDesktopServices::openUrl(_redirectionUrlMap[_selectedApp]);
}

void PollyElmavenInterfaceDialog::_enableNewProjectUi()
{
    newProjectEntry->setEnabled(true);
    existingProjectCombo->setEnabled(false);
    existingProjectRadio->setChecked(false);
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::_enableExistingProjectUi()
{
    existingProjectCombo->setEnabled(true);
    newProjectEntry->setEnabled(false);
    newProjectRadio->setChecked(false);
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

void PollyElmavenInterfaceDialog::_callLoginForm()
{
    _loginform = new LoginForm(this);
    _loginform->setModal(true);
    _loginform->show();
}

void PollyElmavenInterfaceDialog::_callInitialEPIForm()
{
    statusUpdate->setStyleSheet("QLabel { color : green;}");
    statusUpdate->setText("");
    usernameLabel->setText("");
    uploadButton->setEnabled(false);
    existingProjectCombo->clear();

    EPIWorkerThread* workerThread = new EPIWorkerThread();
    connect(workerThread,
            SIGNAL(resultReady(QVariantMap)),
            this,
            SLOT(_handleResults(QVariantMap)));
    connect(workerThread,
            SIGNAL(authentication_result(QString, QString)),
            this,
            SLOT(_handleAuthentication(QString, QString)));
    connect(workerThread,
            &EPIWorkerThread::finished,
            workerThread,
            &QObject::deleteLater);
    workerThread->state = "initial_setup";
    workerThread->start();
    show();

    _loadingDialog->setModal(true);
    _loadingDialog->show();
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
        _loadingDialog->statusLabel->setText("Fetching your projects…");
        usernameLabel->setText(username);
        QCoreApplication::processEvents();
    } else if (status == "error") {
        _loadingDialog->statusLabel->setStyleSheet("QLabel {color : red;}");
        _loadingDialog->statusLabel->setText("No internet access.");
        QCoreApplication::processEvents();
        QTimer* timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), _loadingDialog, SLOT(close()));
        connect(timer, SIGNAL(timeout()), this, SLOT(close()));
        timer->start(5000);
    } else {
        _loadingDialog->statusLabel->setStyleSheet("QLabel {color : red;}");
        _loadingDialog->statusLabel->setText("Authentication failed. "
                                             "Please login again.");
        QCoreApplication::processEvents();
        _logout();
    }
}

void PollyElmavenInterfaceDialog::_handleResults(QVariantMap projectNameIdMap)
{
    _projectNameIdMap = projectNameIdMap;
    startupDataLoad();
}

void PollyElmavenInterfaceDialog::_resetUiElements()
{
    peakTableCombo->clear();

    newProjectRadio->setChecked(true);
    newProjectEntry->setEnabled(true);
    newProjectEntry->clear();

    existingProjectRadio->setChecked(false);
    existingProjectCombo->setEnabled(false);
    existingProjectCombo->clear();

    uploadButton->setEnabled(true);
    gotoPollyButton->setVisible(false);

    statusUpdate->clear();
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::startupDataLoad()
{
    _resetUiElements();
    QIcon projectIcon(rsrcPath + "/POLLY.png");
    if (_projectNameIdMap.isEmpty()) {
        _projectNameIdMap = _pollyIntegration->getUserProjects();
    }

    QStringList keys = _projectNameIdMap.keys();
    for (auto key : keys) {
        existingProjectCombo->addItem(projectIcon,
                                      _projectNameIdMap[key].toString());
    }

    _bookmarkTable = _mainwindow->getBookmarkedPeaks();
    _addTableIfPossible(_bookmarkTable, "Bookmark Table ");

    QList<QPointer<TableDockWidget>> peaksTableList =
        _mainwindow->getPeakTableList();
    for (auto peakTable : peaksTableList) {
        QString peakTableName = QString("Peak Table "
                                        + QString::number(peakTable->tableId)
                                        + " ");
        _addTableIfPossible(peakTable, peakTableName);
    }

    // set latest table by default
    peakTableCombo->setCurrentIndex(peakTableCombo->count() - 1);

    // set to active table if available
    if (_activeTable && _tableNameMapping.values().contains(_activeTable)) {
        QString tableName = _tableNameMapping.key(_activeTable);
        peakTableCombo->setCurrentText(tableName);

        // reset active table
        _activeTable = nullptr;
    }

    _loadingDialog->close();
    _showPollyButtonIfUrlExists();
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::_showPollyButtonIfUrlExists()
{
    if (_selectedApp == PollyApp::FirstView) {
        gotoPollyButton->setText("Go to FirstView");
    } else if (_selectedApp == PollyApp::Fluxomics) {
        gotoPollyButton->setText("Start Fluxing");
    } else if (_selectedApp == PollyApp::QuantFit) {
        gotoPollyButton->setText("Start Quantification");
    }

    if (_redirectionUrlMap[_selectedApp].isEmpty()) {
        gotoPollyButton->setVisible(false);
    } else {
        gotoPollyButton->setVisible(true);
    }
}

void PollyElmavenInterfaceDialog::_addTableIfPossible(TableDockWidget* table,
                                                      QString tableName)
{
    if (!table->getGroups().isEmpty()) {
        if (_selectedApp == PollyApp::Fluxomics
            && table->labeledGroups > 0) {
            peakTableCombo->addItem(tableName);
        } else if (_selectedApp == PollyApp::FirstView
                   || _selectedApp == PollyApp::QuantFit) {
            peakTableCombo->addItem(tableName);
        }
        _tableNameMapping[tableName] = table;
    }
}

void PollyElmavenInterfaceDialog::_uploadDataToPolly()
{
    gotoPollyButton->setVisible(false);
    uploadButton->setEnabled(false);
    peakTableCombo->setEnabled(false);
    projectOptions->setEnabled(false);
    workflowMenu->setEnabled(false);

    _mainwindow->getAnalytics()->hitEvent("Exports", "Polly");
    if (_selectedApp == PollyApp::FirstView) {
        _mainwindow->getAnalytics()->hitEvent("Polly upload", "FirstView");
    } else if (_selectedApp == PollyApp::Fluxomics) {
        _mainwindow->getAnalytics()->hitEvent("Polly upload", "PollyPhi");
    } else if (_selectedApp == PollyApp::QuantFit) {
        _mainwindow->getAnalytics()->hitEvent("Polly upload", "QuantFit");
    }

    statusUpdate->setStyleSheet("QLabel { color : green;}");
    statusUpdate->setText("Connecting…");
    QCoreApplication::processEvents();

    // check for active internet connection
    if (!_pollyIntegration->activeInternet()) {
        statusUpdate->setStyleSheet("QLabel {color : red;}");
        statusUpdate->setText("No internet access.");
        uploadButton->setEnabled(true);
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
    QCoreApplication::processEvents();

    // create a temporary directory to store polly files
    qDebug() << "writing Polly temp file in this directory -"
             << _writeableTempDir;
    QDir qdir(_writeableTempDir);
    if (!qdir.exists()) {
        QDir().mkdir(_writeableTempDir);
        QDir qdir(_writeableTempDir);
    }

    // add datetimestamp to identify files from one upload thread
    QDateTime current_time;
    const QString format = "dd-MM-yyyy_hh_mm_ss";
    QString datetimestamp = current_time.currentDateTime().toString(format);
    datetimestamp.replace(" ", "_");
    datetimestamp.replace(":", "-");

    QStringList filenames = _prepareFilesToUpload(qdir, datetimestamp);
    if (filenames.isEmpty()) {
        statusUpdate->setStyleSheet("QLabel { color : red;}");
        statusUpdate->setText("File preparation failed.");
        _loadingDialog->close();
        QCoreApplication::processEvents();
        emit uploadFinished(false);
        return;
    }

    statusUpdate->setStyleSheet("QLabel { color : green;}");
    statusUpdate->setText("Sending files to Polly…");
    QCoreApplication::processEvents();

    _pollyProjectId = _getProjectId();
    if (_pollyProjectId.isEmpty()) {
        emit uploadFinished(false);
        statusUpdate->setText("");
        return;
    }

    // check for active internet again before upload
    if (!_pollyIntegration->activeInternet()) {
        statusUpdate->setStyleSheet("QLabel {color : red;}");
        statusUpdate->setText("Internet connection interrupted");
        uploadButton->setEnabled(true);
        return;
    }

    EPIWorkerThread* workerThread = new EPIWorkerThread();
    connect(workerThread,
            SIGNAL(filesUploaded(QStringList, QString, QString)),
            this,
            SLOT(_performPostFilesUploadTasks(QStringList, QString, QString)));
    connect(workerThread,
            &EPIWorkerThread::finished,
            workerThread,
            &QObject::deleteLater);
    workerThread->state = "upload_files";
    workerThread->filesToUpload = filenames;
    workerThread->tmpDir = qdir;
    workerThread->uploadProjectIdThread = _pollyProjectId;
    workerThread->datetimestamp = datetimestamp;
    workerThread->currentApp = _selectedApp;
    workerThread->start();
}

QString PollyElmavenInterfaceDialog::_getProjectId()
{
    if (existingProjectRadio->isChecked()) {
        QStringList keys = _projectNameIdMap.keys();
        QString projectId =
            _projectNameIdMap.key(existingProjectCombo->currentText());

        if (projectId.isEmpty())
            _showErrorMessage("Error", "No such project on Polly.");

        return projectId;
    }

    if (newProjectRadio->isChecked()) {
        if (newProjectEntry->text().isEmpty()) {
            _showErrorMessage("Error",
                              "Invalid project name. "
                              "Please enter a non-empty name.");
            return "";
        }

        QString newProjectId =
            _pollyIntegration->createProjectOnPolly(newProjectEntry->text());
        _projectNameIdMap.insert(newProjectId, newProjectEntry->text());
        return newProjectId;
    }

    _showErrorMessage("Error", "Please select at least one option");
    return "";
}

void PollyElmavenInterfaceDialog::_showErrorMessage(QString title,
                                                    QString message)
{
    QMessageBox::warning(this, title, message);
}

void PollyElmavenInterfaceDialog::_performPostFilesUploadTasks(QStringList patchId,
                                                               QString uploadProjectIdThread,
                                                               QString datetimestamp)
{
    QCoreApplication::processEvents();
    QString redirectionUrl = "";

    if (!patchId.isEmpty()) {
        redirectionUrl = _getRedirectionUrl(datetimestamp,
                                            uploadProjectIdThread);
        qDebug() << "redirection_url -" << redirectionUrl;
        _redirectionUrlMap[_selectedApp] = QUrl(redirectionUrl);
    }

    if (!redirectionUrl.isEmpty()) {
        statusUpdate->setText("");
        _showPollyButtonIfUrlExists();

        // send an email to the user, this is a way of persisting their URLs
        QString project = _projectNameIdMap.value(_pollyProjectId).toString();
        QString emailSubject = QString("Data successfully uploaded to Polly "
                                       "project - \"%1\"").arg(project);
        QString emailContent = QString("<a href='%1'></a>").arg(redirectionUrl);
        QString appname = "";
        switch (_selectedApp) {
        case PollyApp::FirstView:
            appname = "firstview";
            break;
        case PollyApp::Fluxomics:
            appname = "pollyphi";
            break;
        case PollyApp::QuantFit:
            appname = "quantfit";
            break;
        default:
            break;
        }
        EPIWorkerThread* workerThread = new EPIWorkerThread();
        connect(workerThread,
                &EPIWorkerThread::finished,
                workerThread,
                &QObject::deleteLater);
        workerThread->state = "send_email";
        workerThread->username = usernameLabel->text();
        workerThread->filesToUpload << emailSubject
                                    << emailContent
                                    << appname;
        workerThread->start();
    } else {
        statusUpdate->setStyleSheet("QLabel {color : red;}");
        QString errorTitle = "An unexpected error occured";
        QString errorMessage = patchId.isEmpty() ? "Sorry. We were unable to "
                                                   "send data."
                                                 : "There seems to be a server "
                                                   "issue. Please try again "
                                                   "later.";
        statusUpdate->setText(errorTitle);
        _showErrorMessage(errorTitle,
                          errorMessage);
    }
    emit uploadFinished(false);
}

QString PollyElmavenInterfaceDialog::_getRedirectionUrl(QString datetimestamp,
                                                        QString uploadProjectIdThread)
{
    QString redirectionUrl;

    switch (_selectedApp) {
    case PollyApp::FirstView: {
        redirectionUrl =
            QString("https://polly.elucidata.io/"
                    "main#project=%1&auto-redirect=%2&elmavenTimestamp=%3")
                    .arg(uploadProjectIdThread)
                    .arg("firstview")
                    .arg(datetimestamp);
        break;
    } case PollyApp::Fluxomics: {
        QString landingPage = QString("relative_lcms_elmaven");
        QString workflowRequestId =
            _pollyIntegration->createWorkflowRequest(uploadProjectIdThread);

        // send to google sheets if sample cohort file is not valid
        QString CohortFileName = _writeableTempDir + QDir::separator()
                                 + datetimestamp
                                 + "_Cohort_Mapping_Elmaven.csv";
        if (!_pollyIntegration->validSampleCohort(CohortFileName))
            landingPage = QString("gsheet_sym_polly_elmaven");

        redirectionUrl =
            QString("https://polly.elucidata.io/workflow-requests/%1/"
                    "lcms_tstpl_pvd/"
                    "dashboard#redirect-from=%2#project=%3#timestamp=%4")
                    .arg(workflowRequestId)
                    .arg(landingPage)
                    .arg(uploadProjectIdThread)
                    .arg(datetimestamp);
        break;
    } case PollyApp::QuantFit: {
        QString componentId = _pollyIntegration->obtainComponentId("calibration");
        QString runRequestId =
            _pollyIntegration->createRunRequest(componentId,
                                                uploadProjectIdThread);
        if (!runRequestId.isEmpty()) {
            redirectionUrl =
                _pollyIntegration->redirectionUiEndpoint(componentId,
                                                         runRequestId,
                                                         datetimestamp);
        } else {
            redirectionUrl = "";
        }
        break;
    } default:
        break;
    }

    return redirectionUrl;
}

QStringList PollyElmavenInterfaceDialog::_prepareFilesToUpload(QDir qdir,
                                                               QString datetimestamp)
{
    QStringList filenames;
    TableDockWidget* peakTable = nullptr;

    // check for no peak tables
    if (peakTableCombo->currentIndex() == -1) {
        _showErrorMessage("Error", "No peak tables available. Either there are "
                                   "no peak tables in the current session or "
                                   "the existing ones are not suitable for "
                                   "the selected Polly app.");
        return filenames;
    }

    peakTable = _tableNameMapping[peakTableCombo->currentText()];

    statusUpdate->setStyleSheet("QLabel {color : green; }");
    statusUpdate->setText("Preparing compound database file…");
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
    statusUpdate->setStyleSheet("QLabel {color : green; }");
    statusUpdate->setText("Preparing CSV file…");
    QCoreApplication::processEvents();

    peakTable->wholePeakSet();
    peakTable->treeWidget->selectAll();
    peakTable->prepareDataForPolly(_writeableTempDir,
                                   "Groups Summary Matrix Format "
                                   "Comma Delimited (*.csv)",
                                    datetimestamp + "_Peak_table_all_");

    // Preparing the json file
    statusUpdate->setStyleSheet("QLabel {color : green; }");
    statusUpdate->setText("Preparing JSON file…");
    QCoreApplication::processEvents();

    QString jsonFilename = _writeableTempDir
                            + QDir::separator()
                            + datetimestamp
                            + "_Peaks_information_json_Elmaven_Polly.json";
    if(currentApp == PollyApp::Fluxomics)
        peakTable->exportJsonToPolly(_writeableTempDir, jsonFilename, true);
    else
        peakTable->exportJsonToPolly(_writeableTempDir, jsonFilename, false);

    
    if (_selectedApp == PollyApp::Fluxomics) {
        statusUpdate->setStyleSheet("QLabel {color : green; }");
        statusUpdate->setText("Preparing sample cohort file..");
        QCoreApplication::processEvents();
        //Preparing the sample cohort file
        QString sampleCohortFileName = _writeableTempDir + QDir::separator() + datetimestamp +
                                        "_Cohort_Mapping_Elmaven.csv";
        _mainwindow->projectDockWidget->prepareSampleCohortFile(sampleCohortFileName);

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
    }

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

void PollyElmavenInterfaceDialog::_logout()
{
    _pollyIntegration->logout();
    _projectNameIdMap = QVariantMap();
    close();
}

void PollyElmavenInterfaceDialog::_performPostUploadTasks(bool uploadSuccessful)
{
    uploadButton->setEnabled(true);
    peakTableCombo->setEnabled(true);
    projectOptions->setEnabled(true);
    workflowMenu->setEnabled(true);
}
