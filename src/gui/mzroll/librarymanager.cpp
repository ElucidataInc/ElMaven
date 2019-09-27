#include "librarymanager.h"
#include "mainwindow.h"
#include "mzUtils.h"
#include "stable.h"

LibraryManager::LibraryManager(MainWindow* parent)
    : QDialog(parent), _mw(parent)
{
    setupUi(this);
    libraryTable->setSortingEnabled(true);

    connect(importButton,
            &QPushButton::clicked,
            this,
            &LibraryManager::importNewDatabase);
    connect(loadButton,
            &QPushButton::clicked,
            this,
            &LibraryManager::loadSelectedDatabase);
    connect(deleteButton,
            &QPushButton::clicked,
            this,
            &LibraryManager::deleteSelectedDatabase);
    connect(libraryTable,
            &QTreeWidget::itemSelectionChanged,
            this,
            &LibraryManager::_setButtonStateForSelection);

    _refreshDatabases();
}

LibraryManager::~LibraryManager() {
    _workerThread.quit();
    _workerThread.wait();
}

void LibraryManager::showEvent(QShowEvent *event)
{
    _refreshDatabases();
}

void LibraryManager::addDatabase(const QString &filepath)
{
    auto databaseRecord = _recordForFile(filepath);
    _addDatabaseToList(databaseRecord);

    auto worker = _newWorker();
    worker->saveRecord(databaseRecord);

    _mw->setFocus();
}

void LibraryManager::importNewDatabase()
{
    _mw->loadCompoundsFile();
}

void LibraryManager::loadSelectedDatabase()
{
    auto item = libraryTable->currentItem();
    auto var = item->data(0, Qt::UserRole);
    auto selectedDatabase = var.value<LibraryRecord>();
    auto filepath = selectedDatabase.absolutePath;

    if (QFile::exists(filepath)) {
        _mw->loadCompoundsFile(filepath);
    } else {
        QMessageBox msgBox;
        msgBox.setText("This database no longer exists at its last known "
                       "location. You can choose to either relocate it, or "
                       "delete it.");
        QPushButton* deleteButton = msgBox.addButton(tr("Delete"),
                                                     QMessageBox::AcceptRole);
        QPushButton* relocateButton = msgBox.addButton(tr("Relocate"),
                                                       QMessageBox::AcceptRole);
        msgBox.exec();
        if (msgBox.clickedButton() == deleteButton) {
            deleteSelectedDatabase();
        } else if (msgBox.clickedButton() == relocateButton) {
            deleteSelectedDatabase();
            importNewDatabase();
        }
    }
}

void LibraryManager::deleteSelectedDatabase()
{
    auto item = libraryTable->currentItem();
    auto var = item->data(0, Qt::UserRole);
    auto selectedDatabase = var.value<LibraryRecord>();
    auto filepath = selectedDatabase.absolutePath;

    auto databaseRecord = _recordForFile(filepath);

    delete item;

    auto worker = _newWorker();
    worker->deleteRecord(databaseRecord);
}

QString LibraryManager::filePathForDatabase(const QString& databaseName)
{
    QTreeWidgetItemIterator iter(libraryTable);
    while (*iter) {
        auto item = *iter;
        auto var = item->data(0, Qt::UserRole);
        auto database = var.value<LibraryRecord>();
        if (databaseName == database.databaseName) {
            return database.absolutePath;
        }
        ++iter;
    }
    return "";
}

void LibraryManager::_refreshDatabases()
{
    libraryTable->clear();
    libraryTable->setHeaderLabels(QStringList() << "Status"
                                                << "Database Name"
                                                << "#Records");

    auto workerCallback = [=](const QList<LibraryRecord> databases) {
        for (auto& db : databases) {
            _addDatabaseToList(db);
        }
        libraryTable->header()
                    ->setSectionResizeMode(QHeaderView::ResizeToContents);
    };

    auto worker = _newWorker();
    connect(worker, &LibraryWorker::loadFinished, workerCallback);
    worker->loadRecords();
}

void LibraryManager::_setButtonStateForSelection()
{
    if (libraryTable->selectedItems().empty()) {
        // no items are selected
        loadButton->setDisabled(true);
        deleteButton->setDisabled(true);
        return;
    }

    auto item = libraryTable->currentItem();
    auto var = item->data(0, Qt::UserRole);
    auto selectedDatabase = var.value<LibraryRecord>();
    auto dbName = selectedDatabase.databaseName;
    if (!DB.getCompoundsSubset(dbName.toStdString()).empty()) {
        // this DB has already been loaded
        loadButton->setDisabled(true);
        deleteButton->setDisabled(false);
        return;
    }

    loadButton->setDisabled(false);
    deleteButton->setDisabled(false);
}

void LibraryManager::_addDatabaseToList(const LibraryRecord& database)
{
    auto dbName = database.databaseName;
    auto status = QFile::exists(database.absolutePath) ? QString("Found")
                                                       : QString("Missing");
    if (!DB.getCompoundsSubset(dbName.toStdString()).empty())
        status = "Loaded";

    _setButtonStateForSelection();
    auto numRecords = QString::number(database.numberOfCompounds);

    // check whether an entry with the same absolute path exits, and if so, only
    // update the existing entry
    bool entryExists = false;
    QTreeWidgetItemIterator iter(libraryTable);
    while (*iter) {
        auto item = *iter;
        auto var = item->data(0, Qt::UserRole);
        auto existingDatabase = var.value<LibraryRecord>();
        if (existingDatabase.absolutePath == database.absolutePath) {
            item->setText(0, status);
            item->setText(1, dbName);
            item->setText(2, numRecords);
            entryExists = true;
        }
        ++iter;
    }

    if (!entryExists) {
        QStringList rowItems = QStringList() << status
                                             << dbName
                                             << numRecords;
        QTreeWidgetItem *item = new QTreeWidgetItem(rowItems);
        item->setData(0, Qt::UserRole, QVariant::fromValue(database));
        libraryTable->addTopLevelItem(item);
    }
}

LibraryRecord LibraryManager::_recordForFile(const QString& filepath)
{
    auto dbName = mzUtils::cleanFilename(filepath.toStdString());
    auto dbCompounds = DB.getCompoundsSubset(dbName);
    LibraryRecord databaseRecord(filepath,
                                 QString::fromStdString(dbName),
                                 static_cast<int>(dbCompounds.size()));
    return databaseRecord;
}

LibraryWorker* LibraryManager::_newWorker()
{
    LibraryWorker *worker = new LibraryWorker;
    worker->moveToThread(&_workerThread);
    connect(&_workerThread, &QThread::finished, worker, &QObject::deleteLater);
    _workerThread.start();
    return worker;
}

////////////////////////////////////////////////////////////////////////////////

LibraryRecord::LibraryRecord(const QJsonObject& json)
{
    absolutePath = "";
    databaseName = "";
    numberOfCompounds = 0;

    if (json.contains("absolutePath") && json["absolutePath"].isString())
        absolutePath = json["absolutePath"].toString();

    if (json.contains("databaseName") && json["databaseName"].isString())
        databaseName = json["databaseName"].toString();

    if (json.contains("numberOfCompounds") && json["numberOfCompounds"].isDouble())
        numberOfCompounds = json["numberOfCompounds"].toInt();
}

QJsonObject LibraryRecord::toJson() const
{
    QJsonObject json;
    json["absolutePath"] = absolutePath;
    json["databaseName"] = databaseName;
    json["numberOfCompounds"] = numberOfCompounds;
    return json;
}

////////////////////////////////////////////////////////////////////////////////

void LibraryWorker::saveRecord(const LibraryRecord& record)
{
    bool recordAlreadyExists = false;
    auto entries = _loadEntries();
    for (size_t i = 0; i < entries.size(); ++i) {
        QJsonObject entry = entries[i].toObject();
        LibraryRecord existingRecord(entry);
        // record having the exact same file path should have the same stats
        if (existingRecord.absolutePath == record.absolutePath) {
            existingRecord.databaseName = record.databaseName;
            existingRecord.numberOfCompounds = record.numberOfCompounds;
            entries.replace(i, existingRecord.toJson());
            recordAlreadyExists = true;
        }
    }

    if (!recordAlreadyExists)
        entries.append(record.toJson());

    emit saveFinished(_saveEntries(entries));
}

void LibraryWorker::deleteRecord(const LibraryRecord& record)
{
    QList<size_t> recordsIndicesToBeDeleted;
    auto entries = _loadEntries();
    for (size_t i = 0; i < entries.size(); ++i) {
        QJsonObject entry = entries[i].toObject();
        LibraryRecord existingRecord(entry);
        // any record having the exact same file path will be deleted
        if (existingRecord.absolutePath == record.absolutePath)
            recordsIndicesToBeDeleted.append(i);
    }

    if (recordsIndicesToBeDeleted.isEmpty()) {
        emit deleteFinished(false);
        return;
    }

    for (auto index : recordsIndicesToBeDeleted)
        entries.removeAt(index);

    emit deleteFinished(_saveEntries(entries));
}

void LibraryWorker::loadRecords()
{
    QList<LibraryRecord> records;
    auto entries = _loadEntries();
    for (size_t i = 0; i < entries.size(); ++i) {
        QJsonObject entry = entries[i].toObject();
        LibraryRecord record(entry);
        records.push_back(record);
    }
    emit loadFinished(records);
}

QString LibraryWorker::_libraryStorePath()
{
    auto configLocation = QStandardPaths::GenericConfigLocation;
    auto parentDir = QStandardPaths::writableLocation(configLocation)
                     + QDir::separator() + "ElMaven";

    // create parent directory if does not already exist
    if (!QFile::exists(parentDir))
        QDir().mkpath(parentDir);

    auto libraryPath = QString(parentDir + QDir::separator() + "library.json");

    // create library file if does not already exist
    if (!QFile::exists(libraryPath))
        QFile(libraryPath).open(QIODevice::ReadWrite);

    return libraryPath;
}

bool LibraryWorker::_saveEntries(const QJsonArray& entries)
{
    QFile libraryFile(_libraryStorePath());
    if (!libraryFile.open(QIODevice::WriteOnly)) {
        cerr << "Error: Could not save to library file" << endl;
        return false;
    }

    QJsonDocument libraryDoc(entries);
    libraryFile.write(libraryDoc.toJson());
    return false;
}

QJsonArray LibraryWorker::_loadEntries()
{
    QFile libraryFile(_libraryStorePath());
    if (!libraryFile.open(QIODevice::ReadOnly)) {
        cerr << "Error: Could not open library file" << endl;
        return QJsonArray();
    }

    auto json = QJsonDocument::fromJson(libraryFile.readAll());
    if (json.isArray())
        return json.array();

    return QJsonArray();
}
