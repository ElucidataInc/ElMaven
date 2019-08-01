#include "librarymanager.h"
#include "mainwindow.h"

LibraryManager::LibraryManager(MainWindow* parent)
    : QDialog(parent), _mw(parent)
{
    setupUi(this);
}

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

    if (recordsIndicesToBeDeleted.isEmpty())
        emit deleteFinished(false);

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
