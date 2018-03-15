#ifndef ERRORCODES_H
#define ERRORCODES_H

#include <map>

//singleton
class ErrorMsg
{
    public:
        enum Errors {
            UnsupportedFormat,
            FileNotFound,
            Blank,
            ParsemzXml,
            ParsemzMl,
            ParsemzData,
            ParseCsv
        };

        static const ErrorMsg* getInstance();
        const std::map<Errors, std::string>& getErrmessages() const;

    private:
        ErrorMsg();
        static const ErrorMsg* errMsg;
        std::map<Errors, std::string> errMessages;
};
#endif
