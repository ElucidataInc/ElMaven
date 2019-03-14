#ifndef CONNECTION_H
#define CONNECTION_H

#include <iostream>
#include <vector>
#include <sqlite3.h>

class Cursor;

/**
 * @brief The Connection class is a loose wrapper around the SQLite3 database
 * connection object.
 * @details SQLite3 being a C library requires the user to connect, close
 * and ensure garbage collection for the central database object. This wrapper
 * tries to handle these details for the user and tries to provide a C++ layer
 * over the database connection C object.
 */
class Connection
{
public:
    /**
     * @brief Construct a connection object for the given SQLite database.
     * @param dbPath Absolute path for SQLite database file as a string.
     */
    Connection(const std::string& dbPath);

    /**
     * @brief Close the connection to the database (if connected), destroy
     * Cursors associated with it and finally destroy the object.
     * @details The rationale behind deleting all Cursor objects (and thus
     * finalizing the associated sqlite3_stmt objects) only when the
     * Connection is being destroyed, is that doing any kind of operation on
     * finalized statements, during the lifetime of an open DB connection, can
     * result in segfaults and heap corruption. An extra set of conditions will
     * be needed to be checked for every method on Cursor objects which would
     * increase the complexity of these simple set of wrappers.
     */
    ~Connection();

    /**
     * @brief Start an explicit SQLite transaction.
     * @details Transactions are automatically started whenever a user attempts
     * to execute a database modification statement. However, if required, this
     * method can be used to explicitly start one. It should be noted that this
     * method does not allow the user to have nested transactions, for which
     * they will need to use appropriate checkpoints using SQLite commands.
     * @return True if a transaction was started successfully.
     */
    bool begin();

    /**
     * @brief Commit pending changes to the database.
     * @details No modification operations are truly reflected in a SQLite
     * database until a commit operation is performed. This method should be
     * called after making any changes to the database, especially if the
     * subsequent set of operations rely on these changes. Any outstanding
     * modification that are left uncommitted are committed when their
     * respective statements are finalized, something that happens whenever
     * the Connection to the database is closed. This ensures that all changes
     * made through a Connection during its lifetime are actually written to
     * the database.
     * @return True if the commit operation was executed successfully.
     */
    bool commit();

    /**
     * @brief Rollback any pending changes.
     * @details This method effectively undoes any changes that have not been
     * committed and are no longer desired. It should be noted that, while
     * the SQLite library by default rolls back all pending changes when a DB
     * connection is closed, this wrapper class will indirectly commit pending
     * changes before closing the connection. So if any undesired changes are
     * to be rolled back, they have to be explicitly done so before the
     * destruction of a Connection object. Commits are a default over rollback
     * because more often changes need to be committed than rolled back. The
     * latter is assumed to be an active decision.
     * @return True if the rollback operation was executed successfully.
     */
    bool rollback();

    /**
     * @brief This method is a convenience wrapper for executing multiple SQL
     * queries (separated by ';') in one go.
     * @details `executeMult` eliminates the need to create a cursor for each
     * SQL statement and then execute them, if the user knows that a certain set
     * of statements can be executed one after another without any special
     * actions needed in between in each execution. There are two caveats here:
     *  1. The user cannot use this method for "SELECT" statements. Such
     *     statements are better of being prepared as cursors and then used
     *     iteratively.
     *  2. The user must ensure that each separate statement in the given SQL
     *     string ends with a ';' delimiter.
     * @param sql_string A string of SQL statements to be executed.
     * @return True if the execution was successful, false otherwise.
     */
    bool executeMulti(const std::string sql_string);

    /**
     * @brief Prepare a SQL statement and return a Cursor ready to be
     * executed. See documentation of Cursor class for details.
     * @param query A SQL query as a standard string.
     * @return Pointer to a Cursor object that has to be executed/iterated upon.
     */
    Cursor* prepare(const std::string& query);

    /**
     * @brief Obtain the ROWID for the last row inserted.
     * @details A ROWID in SQLite is available for all rows in a regular tables
     * (except the ones created using the "WITHOUT ROWID" clause). This value
     * is unique for each row in the table and if the table has a primary key
     * integer column then that column becomes an alias for ROWID.
     * @return An integer having the ROWID of last inserted row.
     */
    int lastInsertId();

    /**
     * @brief Obtain the absolute path to the connected database.
     * @return Path as a string.
     */
    std::string dbPath();

private:
    /**
     * @brief Stores absolute path for the connected SQLite database file.
     */
    std::string _dbPath;

    /**
     * @brief Pointer to the sqlite3 db object that this connection
     * represents.
     */
    sqlite3* _database;

    /**
     * @brief A vector of Cursor objects that were created by this
     * connection and therefore should be deleted when this object is destroyed.
     */
    std::vector<Cursor*> _cursors;
};

#endif // CONNECTION_H
