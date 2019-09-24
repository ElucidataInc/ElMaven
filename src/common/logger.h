#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

class LogBuffer;

/**
 * @brief The Logger class can be used to write logs to a file, leveraging
 * standard string stream operations.
 */
class Logger
{
public:
    /**
     * @brief Once created, a `Logger` object is associated to a file to which
     * it keeps appending information, debug or error strings.
     * @details The file to which logs are written is not allowed to be changed,
     * once constructed.
     * @param filename A string storing the name of the file to which logs will
     * be written.
     * @param writeToConsole Optional parameter, that if set to true, will cause
     * all strings passed through to any of the streams to be also printed on
     * standard output.
     * @param overwrite Optional paramater, that if set to true, will instruct
     * the constructor to immediately delete any existing file at the provided
     * path. This is set to `true` by default.
     */
    Logger(std::string filename,
           bool writeToConsole = false,
           bool overwrite = true);

    /**
     * @brief Destructor. Deletes all streams and associated buffers.
     */
    ~Logger();

    /**
     * @brief This output stream should be used to write general information
     * to the log.
     * @return A reference to a `std::ostream` object.
     */
    std::ostream& info();

    /**
     * @brief This output stream should be used to write debug strings to the
     * log.
     * @return A reference to a `std::ostream` object.
     */
    std::ostream& debug();

    /**
     * @brief This output stream should be used to write error strings to the
     * log.
     * @return A reference to a `std::ostream` object.
     */
    std::ostream& error();

private:
    /**
     * @brief Buffer to which information strings are written.
     */
    LogBuffer* _infoBuffer;

    /**
     * @brief Buffer to which debug strings are written.
     */
    LogBuffer* _debugBuffer;

    /**
     * @brief Buffer to which error strings are written.
     */
    LogBuffer* _errorBuffer;

    /**
     * @brief An output stream which uses `_infoBuffer` as its base.
     */
    std::ostream* _infoStream;

    /**
     * @brief An output stream which uses `_debugBuffer` as its base.
     */
    std::ostream* _debugStream;

    /**
     * @brief An output stream which uses `_errorBuffer` as its base.
     */
    std::ostream* _errorStream;
};

/**
 * @brief The LogBuffer class is used by the `Logger` class internally to
 * represent its stream buffers.
 */
class LogBuffer : public std::stringbuf
{
public:
    /**
     * @brief Create a string buffer that carries its own copy of a file stream
     * to which it writes everything.
     * @details The internal file stream is always opened in append mode. So,
     * any existing contents of the file between write operations are preserved.
     * @param filename A string containing the name of the file to which this
     * buffer logs.
     * @param type A string containing the type name of the log string which
     * will be appear on the first line of each log entry, along with a time
     * record.
     * @param writeToConsole An optional parameter that if set to true will
     * also write every log entry to the standard output, but without the line
     * containing log type and time record.
     */
    LogBuffer(const std::string& filename,
              const std::string& type,
              bool writeToConsole = false);

    /**
     * @brief Destructor. Closes internal filestream.
     */
    ~LogBuffer();

    /**
     * @brief Overriden virtual method that takes care of our custom output
     * behaviour, whenever the buffer is flushed. See constructor of this class
     * for more details.
     */
    virtual int sync() {
        auto now = std::chrono::system_clock::now();
        auto current_time = std::chrono::system_clock::to_time_t(now);

        // write to log file
        _stream << _logType << " | " << std::ctime(&current_time)
                << str() << std::endl;

        if (_writeToConsole)
            std::cout << str() << std::endl;

        // clear buffer
        str("");

        return 0;
    }

private:
    /**
     * @brief Internal output filestream to which all logs are written.
     */
    std::ofstream _stream;

    /**
     * @brief The string "type" that will mark the beginning of a log entry.
     */
    std::string _logType;

    /**
     * @brief Predicate that controls priting to standard output, as a secondary
     * effect.
     */
    bool _writeToConsole;
};

#endif // LOGGER_H
