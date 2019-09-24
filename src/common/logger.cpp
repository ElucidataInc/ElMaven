#include "logger.h"
#include "boost/filesystem.hpp"

Logger::Logger(std::string filename, bool writeToConsole, bool overwrite)
{
    if (overwrite && boost::filesystem::exists(filename))
        boost::filesystem::remove(filename);

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
