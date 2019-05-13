#include "pollyelmaveninterface.h"
#include "controller.h"
#include <QVariant>
#include "csvreports.h"
#include <string>

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

    groupSetCombo->addItem("All Groups");
    groupSetCombo->addItem("Only Good Groups");
    groupSetCombo->addItem("Exclude Bad Groups");
    groupSetCombo->setCurrentIndex(0);

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
    connect(tutorialLink,
            &QLabel::linkActivated,
            [=](const QString &link) {
                QDesktopServices::openUrl(QUrl(link));
                auto appName = PollyIntegration::stringForApp(_selectedApp);
                _mainwindow->getAnalytics()->hitEvent("PollyDialog",
                                                      "ViewTutorial",
                                                      appName);
            });
}

PollyElmavenInterfaceDialog::~PollyElmavenInterfaceDialog()
{
    qDebug() << "exiting PollyElmavenInterfaceDialog now....";
    if (_loginform)
        delete (_loginform);
}

EPIWorkerThread::EPIWorkerThread(PollyIntegration* iPolly):
    _pollyintegration(iPolly)
{
}

void EPIWorkerThread::run()
{
    if (state == "initial_setup") {
        qDebug() << "Checking for active internet connection…";
        QString status;
        if (!_pollyintegration->activeInternet()) {
            status = "error";
            emit authentication_result("", status);
            return;
        }

        qDebug() << "Authenticating…";
        status = _pollyintegration->authenticateLogin("", "");
        emit authentication_result(_pollyintegration->getCurrentUsername(),
                                   status);

        qDebug() << "Fetching licenses from Polly…";
        if (status == "ok") {
            QMap<PollyApp, bool> licenseMap =
                _pollyintegration->getAppLicenseStatus();
            emit licensesReady(licenseMap);
        }

        qDebug() << "Fetching projects from Polly…";
        if (status == "ok") {
            QVariantMap _projectNameIdMap = _pollyintegration->getUserProjects();
            emit projectsReady(_projectNameIdMap);
        }
    } else if (state == "upload_files") {
        qDebug() << "starting thread for uploading files to polly…";
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
        qDebug() << (emailSent ? "Sent an email containing URL to user."
                               : "Failed to send email to user.");
    }
}

EPIWorkerThread::~EPIWorkerThread()
{
    for (auto fileName : filesToUpload) {
        tmpDir.remove(fileName);
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

void PollyElmavenInterfaceDialog::_goToPolly()
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
    if (!_uploadInProgress) {
        statusUpdate->setStyleSheet("QLabel { color : green;}");
        statusUpdate->clear();
    }
    usernameLabel->setText("");
    uploadButton->setEnabled(false);
    existingProjectCombo->clear();

    PollyIntegration* iPolly = _mainwindow->getController()->iPolly;
    EPIWorkerThread* workerThread = new EPIWorkerThread(iPolly);
    connect(workerThread,
            SIGNAL(projectsReady(QVariantMap)),
            this,
            SLOT(_handleProjects(QVariantMap)));
    connect(workerThread,
            SIGNAL(licensesReady(QMap<PollyApp, bool>)),
            this,
            SLOT(_handleLicenses(QMap<PollyApp, bool>)));
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

    newProjectRadio->setChecked(true);
    newProjectEntry->setEnabled(true);
    newProjectEntry->clear();

    existingProjectRadio->setChecked(false);
    existingProjectCombo->setEnabled(false);
    existingProjectCombo->clear();

    uploadButton->setEnabled(true);
    _showPollyButtonIfUrlExists();

    statusUpdate->clear();
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

    QStringList keys = _projectNameIdMap.keys();
    for (auto key : keys) {
        existingProjectCombo->addItem(_projectNameIdMap[key].toString());
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
    _hideFormIfNotLicensed();
    QCoreApplication::processEvents();
}

void PollyElmavenInterfaceDialog::_hideFormIfNotLicensed()
{
    if (!_licenseMap.value(_selectedApp)) {
        stackedWidget->setCurrentWidget(advertBox);
        demoButton->setDefault(true);
    } else {
        stackedWidget->setCurrentWidget(pollyForm);
        gotoPollyButton->setDefault(true);
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
}

void PollyElmavenInterfaceDialog::_addTableIfPossible(TableDockWidget* table,
                                                      QString tableName)
{
    if (!table->getGroups().isEmpty()) {
        if (_selectedApp == PollyApp::PollyPhi
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
    _uploadInProgress = true;
    gotoPollyButton->setVisible(false);
    uploadButton->setEnabled(false);
    peakTableCombo->setEnabled(false);
    groupSetCombo->setEnabled(false);
    projectOptions->setEnabled(false);
    workflowMenu->setEnabled(false);

    _mainwindow->getAnalytics()->hitEvent("Exports", "Polly");
    if (_selectedApp == PollyApp::FirstView) {
        _mainwindow->getAnalytics()->hitEvent("Polly upload", "FirstView");
    } else if (_selectedApp == PollyApp::PollyPhi) {
        _mainwindow->getAnalytics()->hitEvent("Polly upload", "PollyPhi");
    } else if (_selectedApp == PollyApp::QuantFit) {
        _mainwindow->getAnalytics()->hitEvent("Polly upload", "QuantFit");
    }

    statusUpdate->setStyleSheet("QLabel { color : green;}");
    statusUpdate->setText("Connecting…");
    QCoreApplication::processEvents();
    // check for active internet connection
    if (!_pollyIntegration->activeInternet()) {
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
        statusUpdate->clear();
        return;
    }

    // check for active internet again before upload
    if (!_pollyIntegration->activeInternet()) {
        statusUpdate->setStyleSheet("QLabel {color : red;}");
        statusUpdate->setText("Internet connection interrupted");
        uploadButton->setEnabled(true);
        return;
    }

    PollyIntegration* iPolly = _mainwindow->getController()->iPolly;
    EPIWorkerThread* workerThread = new EPIWorkerThread(iPolly);
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
        _performPostUploadTasks(true);
        statusUpdate->clear();
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
        case PollyApp::PollyPhi:
            appname = "pollyphi";
            break;
        case PollyApp::QuantFit:
            appname = "quantfit";
            break;
        default:
            break;
        }

        PollyIntegration* iPolly = _mainwindow->getController()->iPolly;
        EPIWorkerThread* workerThread = new EPIWorkerThread(iPolly);
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
        _performPostUploadTasks(false);
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
}

QString PollyElmavenInterfaceDialog::_getRedirectionUrl(QString datetimestamp,
                                                        QString uploadProjectIdThread)
{
    QString redirectionUrl = "";

    switch (_selectedApp) {
    case PollyApp::FirstView: {
        redirectionUrl =
            QString("https://polly.elucidata.io/"
                    "main#project=%1&auto-redirect=%2&elmavenTimestamp=%3")
                    .arg(uploadProjectIdThread)
                    .arg("firstview")
                    .arg(datetimestamp);
        break;
    } case PollyApp::PollyPhi: {
        QString landingPage = QString("relative_lcms_elmaven");
        QString workflowRequestId =
            _pollyIntegration->createWorkflowRequest(uploadProjectIdThread);
        if (workflowRequestId.isEmpty())
            return redirectionUrl;

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
        QString componentId =
            _pollyIntegration->obtainComponentId(PollyApp::QuantFit);
        if (componentId == "-1")
            return redirectionUrl;

        QString runRequestId =
            _pollyIntegration->createRunRequest(componentId,
                                                uploadProjectIdThread);
        if (runRequestId.isEmpty())
            return redirectionUrl;

        redirectionUrl = _pollyIntegration->redirectionUiEndpoint(componentId,
                                                                  runRequestId,
                                                                  datetimestamp);
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

void PollyElmavenInterfaceDialog::_logout()
{
    usernameLabel->setText("");
    _pollyIntegration->logout();
    _projectNameIdMap = QVariantMap();
    close();
}

void PollyElmavenInterfaceDialog::_performPostUploadTasks(bool uploadSuccessful)
{
    _uploadInProgress = false;
    uploadButton->setEnabled(true);
    peakTableCombo->setEnabled(true);
    groupSetCombo->setEnabled(true);
    projectOptions->setEnabled(true);
    workflowMenu->setEnabled(true);
}

MainWindow* PollyElmavenInterfaceDialog::getMainWindow()
{
    return _mainwindow;
}
