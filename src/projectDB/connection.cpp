#include "connection.h"
#include "cursor.h"

Connection::Connection(const std::string& dbPath)
{
    int errCode = sqlite3_open(dbPath.c_str(), &_database);
    if (errCode) {
        std::cerr << "Cannot open database at location \""
                  << dbPath
                  << "\", because of error: "
                  << sqlite3_errmsg(_database);
        _database = nullptr;
        _dbPath = "";
    } else {
        _dbPath = dbPath;
    }
}

Connection::~Connection()
{
    for (auto cursor: _cursors)
        delete cursor;

    if (_database != nullptr)
        sqlite3_close_v2(_database);
}

bool Connection::begin()
{
    return prepare("BEGIN TRANSACTION")->execute();
}

bool Connection::commit()
{
    return prepare("COMMIT")->execute();
}

bool Connection::rollback()
{
    return prepare("ROLLBACK")->execute();
}

Cursor* Connection::prepare(const std::string& query)
{
    sqlite3_stmt* statement;
    int status = sqlite3_prepare_v2(_database,
                                    query.c_str(),
                                    -1,
                                    &statement,
                                    nullptr);
    auto cursor = new Cursor(statement);
    _cursors.push_back(cursor);
    return cursor;
}

int Connection::lastInsertId()
{
    int64_t lastRowId = sqlite3_last_insert_rowid(_database);
    return static_cast<int>(lastRowId);
}

std::string Connection::dbPath()
{
    return _dbPath;
}
