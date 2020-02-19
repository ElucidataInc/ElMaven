#ifndef CURSOR_H
#define CURSOR_H

#include <iostream>
#include <map>
#include <sqlite3.h>

class Connection;

/**
 * @brief The Cursor class is a convencience wrapper around the sqlite3_stmt
 * structure of SQLite3 library.
 * @details This class represents a SQL statement that can be executed on a
 * database. It hides details of construction, execution, extraction of values
 * and finalization of SQL statements in the SQLite library by providing a
 * simpler C++ interface.
 */
class Cursor
{
    // To allow Connection class to create Cursors using the private constructor
    friend class Connection;

public:
    /**
     * @brief Execute a non returning SQL statement, such as update, delete,
     * insert and create operations.
     * @details The "step" function called within this function is the same as
     * the one that is called in the `next` method for this object, although
     * the return condition is different. The statement is reset with each
     * `execute` call, making the statement callable again with different bound
     * parameters. And this is also why `next` should be used for result
     * returning statements over this method, since execution will keep coming
     * back to the start of the result set with each call.
     * @return True if statement execution finished successfully.
     */
    bool execute();

    /**
     * @brief Execute and move to the next row for select operations.
     * @details While this method, like `execute` also uses the "step" SQLite
     * function, its semantically meant to be used for iterating over rows
     * returned from a suitable SQL operation (most commonly SELECT statements).
     * Moreover, the `_extracValues` method is called to extract values stored
     * in current row of the result set.
     * @return True if the `next` method can be further called upon this Cursor.
     */
    bool next();

    /**
     * @brief Bind integer value for statement with named parameter.
     * @param param Name of the parameter to be bound.
     * @param value Value as an integer to be bound for the parameter.
     * @return True if value was successfully bound.
     */
    bool bind(const std::string& param, int value);

    /**
     * @brief Bind long integer value for statement with named parameter.
     * @param param Name of the parameter to be bound.
     * @param value Value as long integer to be bound for the parameter.
     * @return True if value was successfully bound.
     */
    bool bind(const std::string& param, long value);

    /**
     * @brief Bind double precision value for statement with named parameter.
     * @param param Name of the parameter to be bound.
     * @param value Value as a double to be bound for the parameter.
     * @return True if value was successfully bound.
     */
    bool bind(const std::string& param, double value);

    /**
     * @brief Bind floating point value for statement with named parameter.
     * @param param Name of the parameter to be bound.
     * @param value Value as a floating point to be bound for the parameter.
     * @return True if value was successfully bound.
     */
    bool bind(const std::string& param, float value);

    /**
     * @brief Bind string value for statement with given named parameter.
     * @param param Name of the parameter to be bound.
     * @param value String value to be bound for the parameter.
     * @return True if value was successfully bound.
     */
    bool bind(const std::string& param, const std::string value);

    /**
     * @brief Obtain values for integers in the form of a int type.
     * @param param Name of parameter whose value is needed.
     * @return Value of parameter converted to integer.
     */
    int integerValue(const std::string& param);

    /**
     * @brief Obtain values for real numbers in the form of a double type.
     * @param param Name of parameter whose value is needed.
     * @return Value of parameter converted to double.
     */
    double doubleValue(const std::string& param);

    /**
     * @brief Obtain values for real numbers in the form of a float type.
     * @param param Name of parameter whose value is needed.
     * @return Value of parameter converted to float.
     */
    float floatValue(const std::string& param);

    /**
     * @brief Obtain textual values in the form of a string type.
     * @param param Name of parameter whose value is needed.
     * @return Value of parameter converted to string.
     */
    std::string stringValue(const std::string& param);

private:
    /**
     * @brief A pointer to the sqlite3_stmt construct represented by the class.
     */
    sqlite3_stmt* _statement;

    /**
     * @brief A map storing values for the current result, in case the query
     * is of a row returning type (i.e., "SELECT" statements).
     */
    std::map<std::string, std::string> _currentResult;

    /**
     * @brief Constructor that can only be accessed by friend classes.
     * @details The constructor has been made private to prevent a Cursor object
     * from being created without an associated connection object as that would
     * break the abstraction.
     * @param statement A pointer to sqlite3_stmt construct around which the
     * Cursor object will be created.
     */
    Cursor(sqlite3_stmt* statement);

    /**
     * @brief Finalise the SQLite statement wrapped within this object and then
     * destroy the object.
     * @details The destructor for a Cursor should not be called if it was
     * created in context to a Connection object. The Connection owning a
     * Cursor will attempt to destruct it when it is itself destroyed.
     */
    ~Cursor();

    /**
     * @brief Extract values from the current iteration of result set and store
     * it in `_currentResult` as a mapping of parameter names to their values
     * as strings. These values are converted to the corresponding type when
     * any of the `integerValue`, `doubleValue`, `floatValue` or `stringValue`
     * methods are called.
     */
    void _extractValues();
};

#endif // CURSOR_H
