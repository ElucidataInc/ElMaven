#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include "logger.h"

namespace fs = boost::filesystem;

/**
 * @brief Clear all logs stored in a certain directory that were last
 * modified over a month ago at least a month.
 * @param dirPath A string containing path of the directory.
 * @param extension The file-extension that would mark it as a log.
 */
void _clearOldLogs(const fs::path dirPath,
                   const std::string extension = ".log")
{
    if (fs::exists(dirPath)) {
        auto now = std::chrono::system_clock::now();
        auto nowTime = std::chrono::system_clock::to_time_t(now);
        auto tm = *gmtime(&nowTime);
        tm.tm_mon = tm.tm_mon - 1;
        auto oneMonthAgo = mktime(&tm);

        fs::path dir(dirPath);
        fs::directory_iterator it(dir), eod;
        BOOST_FOREACH(fs::path const &path, std::make_pair(it, eod)) {
            if (fs::is_regular_file(path)
                && fs::extension(path) == extension
                && fs::last_write_time(path) < oneMonthAgo) {
                fs::remove(path);
            }
        }
    }
}

Logger::Logger(std::string filename, bool writeToConsole, bool overwrite)
{
    if (overwrite && fs::exists(filename))
        fs::remove(filename);

    fs::path logPath(filename);
    _clearOldLogs(logPath.parent_path());

    _infoBuffer = new LogBuffer(filename, "INFO", writeToConsole);
    _debugBuffer = new LogBuffer(filename, "DEBUG", writeToConsole);
    _errorBuffer = new LogBuffer(filename, "ERROR", writeToConsole);
    _infoStream = new std::ostream(_infoBuffer);
    _debugStream = new std::ostream(_debugBuffer);
    _errorStream = new std::ostream(_errorBuffer);
}

Logger::~Logger()
{
    delete _infoBuffer;
    delete _debugBuffer;
    delete _errorBuffer;
    delete _infoStream;
    delete _debugStream;
    delete _errorStream;
}

std::ostream& Logger::info()
{
    return *_infoStream;
}

std::ostream& Logger::debug()
{
    return *_debugStream;
}

std::ostream& Logger::error()
{
    return *_errorStream;
}

LogBuffer::LogBuffer(const std::string& filename,
                     const std::string& type,
                     bool writeToConsole)
    : _stream(filename, std::fstream::app)
    , _logType(type)
    , _writeToConsole(writeToConsole)
{}

LogBuffer::~LogBuffer()
{
    _stream.close();
}
