#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include <ui_librarymanager.h>

class MainWindow;

class LibraryManager : public QDialog,
                       public Ui_LibraryManager
{
    Q_OBJECT

public:
    explicit LibraryManager(MainWindow *parent = nullptr);

private:
    /**
     * @brief A pointer to the parent main window of this dialog.
     */
    MainWindow* _mw;
};

/**
 * @brief The LibraryRecord struct contains all attributes of a compound
 * database that needs to be saved in the library, meant to be shared over
 * application sessions.
 */
struct LibraryRecord {
    /**
     * @brief The absolute path of a compound database file on the filesystem.
     */
    QString absolutePath;

    /**
     * @brief Name of the compound database.
     */
    QString databaseName;

    /**
     * @brief The number of compounds stored in this compound database.
     */
    int numberOfCompounds;

    /**
     * @brief Basic constructor.
     * @param absolutePath Path to be set for this compound database.
     * @param databaseName Name to be set for this compound database.
     * @param numberOfCompounds Number of compounds in this compound database.
     */
    LibraryRecord(const QString& absolutePath,
                  const QString& databaseName,
                  const int numberOfCompounds) :
                  absolutePath(absolutePath),
                  databaseName(databaseName),
                  numberOfCompounds(numberOfCompounds) {}

    /**
     * @brief Construct a `LibraryRecord` using a JSON object.
     * @param json A `QJsonObject` that contains values to be set for this
     * record.
     */
    LibraryRecord(const QJsonObject& json);

    /**
     * @brief Convert a record to an equivalent JSON object.
     * @return A `QJsonObject` representing the values for this record.
     */
    QJsonObject toJson() const;
};

/**
 * @brief The LibraryWorker class is meant to be used as a worker that runs in
 * a separate thread and performs the saving and loading operation for library
 * records into and from JSON files.
 */
class LibraryWorker : public QObject
{
    Q_OBJECT

public slots:
    /**
     * @brief Save a compound database record in the library.
     * @details If an entry already exists in the shared library, that has the
     * same path as the given record, only attributes of the existing entry is
     * updated to match the given record.
     * Once the operation completes, the `saveFinished` signal is emitted with
     * a status value indicating success with `true` and failure with `false`.
     * @param record A `LibraryRecord` object to be saved in the library.
     */
    void saveRecord(const LibraryRecord& record);

    /**
     * @brief Delete the given record from saved library (JSON) file.
     * @details After completion of the delete operation, the `deleteFinished`
     * signal is emitted with a boolean status, where `true` means deletion was
     * successful and `false` means it failed (or the record never existed).
     * @param record A `LibraryRecord` for which, all entries having the same
     * path as this record will be deleted.
     */
    void deleteRecord(const LibraryRecord& record);

    /**
     * @brief Load all records saved in the library.
     * @details All entries in the library (which itself is a JSON array of
     * objects), will be converted to `LibraryRecord` objects which will be
     * packaged in a `QList` and emitted with the `loadFinished` signal, once
     * the operation completes.
     */
    void loadRecords();

signals:
    void saveFinished(bool status);
    void deleteFinished(bool status);
    void loadFinished(const QList<LibraryRecord> records);

private:
    /**
     * @brief Obtain the path of the JSON file storing our library of compound
     * databases.
     * @details This method also creates the file (and its parent directories),
     * if it does not already exist.
     * @return A `QString` set to the path of the on-disk library (JSON) file.
     */
    QString _libraryStorePath();

    /**
     * @brief Utility function that can be used to save a JSON array of DB
     * entries to the library file.
     * @param entries A `QJsonArray` object which will be overwritten to the
     * library file.
     * @return A boolean status where `true` means save was successful and
     * `false` means it failed.
     */
    bool _saveEntries(const QJsonArray& entries);

    /**
     * @brief Utility function, that reads the library file and returns the
     * contents as an JSON array object, if successfully parsed as such.
     * @return A `QJsonArray` object that has been read from the library file.
     * If the read or parse operations fail, an empty `QJsonArray` is returned.
     */
    QJsonArray _loadEntries();
};

#endif // LIBRARYMANAGER_H
