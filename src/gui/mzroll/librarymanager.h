#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include <ui_librarymanager.h>

class MainWindow;
class LibraryWorker;
struct LibraryRecord;

class LibraryManager : public QDialog,
                       public Ui_LibraryManager
{
    Q_OBJECT

public:
    /**
     * @brief Default constructor.
     * @param parent The parent window which owns the library manager widget.
     */
    explicit LibraryManager(MainWindow *parent = nullptr);

    /**
     * @brief Destructor. Quits and waits for internal thread to finish.
     */
    ~LibraryManager();

public slots:
    /**
     * @brief Add a database entry to the list as well as save it to the
     * library.
     * @param filepath A `QString` set to path of the database file that needs
     * to be added. The name and other details of the database can be derived
     * from the file itself.
     */
    void addDatabase(const QString& filepath);

    /**
     * @brief Call ligand widget to select and load a new database file.
     */
    void importNewDatabase();

    /**
     * @brief Load (or reload, if already loaded) the currently selected entry
     * in the DB list.
     */
    void loadSelectedDatabase();

    /**
     * @brief Delete an entry from the DB list and remove it from the library
     * as well (its details will no longer be preserved over sessions).
     */
    void deleteSelectedDatabase();

    /**
     * @brief Obtain the file path for a given database name, if it exists in
     * the library records.
     * @param databaseName A `QString` containing the name of the stored
     * database.
     * @return A `QString` containing the absolute path of the database file. If
     * the database is not found in library, then an empty string is returned.
     */
    QString filePathForDatabase(const QString& databaseName);

protected:
    /**
     * @brief Overriding this method so that its contents are called everytime
     * the widget is shown.
     */
    void showEvent(QShowEvent *event);

private:
    /**
     * @brief A pointer to the parent main window of this dialog.
     */
    MainWindow* _mw;

    /**
     * @brief A thread object that is reserved for workers meant for performing
     * all I/O operations.
     */
    QThread _workerThread;

private slots:
    /**
     * @brief Repopulate the DB list to include all existing databases as well
     * as update their details.
     */
    void _refreshDatabases();

    /**
     * @brief Enables or disables the "Delete" and "Load" buttons depending on
     * which rows are selected in the table.
     */
    void _setButtonStateForSelection();

    /**
     * @brief Add a record to the DB list.
     * @details This method also ensures that if there exists an entry for this
     * record (i.e., there exists an entry which represents the same file) then
     * that record is only updated with the details of the incoming record, and
     * no additional row is added.
     * @param database The `LibraryRecord` object, for which an item needs to be
     * added (or updated) to the list.
     */
    void _addDatabaseToList(const LibraryRecord& database);

    /**
     * @brief Create a `LibraryRecord` for a database file path.
     * @param filepath A `QString` set to the path for which the record has to
     * be created.
     * @return A `LibraryRecord` object containing basic details of the given
     * database.
     */
    LibraryRecord _recordForFile(const QString& filepath);

    /**
     * @brief Utility that creates a worker object and moves it to the thread
     * reserved for tasks involving I/O.
     * @return A `LibraryWorker` ready to perform read/write methods in a
     * non-blocking fashion.
     */
    LibraryWorker* _newWorker();
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
     * @brief Default constructor.
     */
    LibraryRecord() = default;

    /**
     * @brief Basic constructor with all the arguments.
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

Q_DECLARE_METATYPE(LibraryRecord)

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
