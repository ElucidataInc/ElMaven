#ifndef ELMAVENLOGGER_H
#define ELMAVENLOGGER_H

#include "spdlog/spdlog.h"

class ElMavenLogger
{
    public:
        enum ErrorType {
            critical,
            log,
            debug,
            info,
            fatal
        };
        static void init(const std::string& path);
        static ElMavenLogger* getInstance();
        std::shared_ptr<spdlog::logger> get() { return _logger; }
        void logErr(const std::string& errMessage, const ErrorType& type);
    private:
        ElMavenLogger();
        std::shared_ptr<spdlog::logger> _logger;
        static ElMavenLogger* _dlog;
        static std::string _path;

        
};
#endif
