#ifndef ERRORCODES_H
#define ERRORCODES_H

#include <map>

//singleton
//TODO: make the name of Class and filename same
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
        std::string getErrmessages(const Errors& err) const;

    private:
        ErrorMsg();
        static const ErrorMsg* errMsg;
        std::map<Errors, std::string> errMessages;
};
#endif
