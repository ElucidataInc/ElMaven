#include "cursor.h"
#include "connection.h"

Cursor::Cursor(sqlite3_stmt* statement)
{
    _statement = statement;
}

Cursor::~Cursor()
{
    if (_statement)
        sqlite3_finalize(_statement);
}

bool Cursor::execute()
{
    int status = sqlite3_step(_statement);
    sqlite3_reset(_statement);
    return status == SQLITE_DONE;
}

bool Cursor::next()
{
    int status = sqlite3_step(_statement);
    _extractValues();
    return status == SQLITE_ROW;
}

bool Cursor::bind(const std::string& param, int value)
{
    int index = sqlite3_bind_parameter_index(_statement, param.c_str());
    return sqlite3_bind_int(_statement, index, value) == SQLITE_OK;
}

bool Cursor::bind(const std::string& param, long value)
{
    int index = sqlite3_bind_parameter_index(_statement, param.c_str());
    return sqlite3_bind_int64(_statement, index, value) == SQLITE_OK;
}

bool Cursor::bind(const std::string& param, double value)
{
    int index = sqlite3_bind_parameter_index(_statement, param.c_str());
    return sqlite3_bind_double(_statement, index, value) == SQLITE_OK;
}

bool Cursor::bind(const std::string& param, float value)
{
    auto dval = static_cast<double>(value);
    return this->bind(param, dval);
}

bool Cursor::bind(const std::string& param, const std::string value)
{
    int index = sqlite3_bind_parameter_index(_statement, param.c_str());
    return sqlite3_bind_text(_statement,
                             index,
                             value.c_str(),
                             -1,
                             SQLITE_TRANSIENT) == SQLITE_OK;
}

int Cursor::integerValue(const std::string& param)
{
    auto val = _currentResult[param];
    if (val.empty())
        return 0;
    return std::stoi(val);
}

double Cursor::doubleValue(const std::string& param)
{
    auto val = _currentResult[param];
    if (val.empty())
        return 0.0;
    return std::stod(_currentResult[param]);
}

float Cursor::floatValue(const std::string& param)
{
    double dval = doubleValue(param);
    return static_cast<float>(dval);
}

std::string Cursor::stringValue(const std::string& param)
{
    return _currentResult[param];
}

void Cursor::_extractValues()
{
    _currentResult.clear();
    int valueCount = sqlite3_data_count(_statement);

    while (valueCount) {
        // column indexing goes from 0 to (valueCount - 1), so decrement first
        --valueCount;

        auto param =
            reinterpret_cast<const char*>(sqlite3_column_name(_statement,
                                                              valueCount));
        // if param was pointing to NULL
        if (!param)
            param = "";

        auto value =
            reinterpret_cast<const char*>(sqlite3_column_text(_statement,
                                                              valueCount));
        // if value was pointing to NULL
        if (!value)
            value = "";

        _currentResult[param] = value;
    }
}
