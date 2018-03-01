#include <map>

struct ErrorCodes
{
    enum Errors {
        UnsupportedFormat,
        FileNotFound,
        Blank,
        ParsemzXml,
        ParsemzMl,
        ParsemzData,
        ParseCsv
    };

};

//singleton
class ErrorMsg
{
    public:

        static ErrorMsg* getInstance();
        std::map<ErrorCodes::Errors, std::string> getErrmessages();

    private:
        ErrorMsg();
        static ErrorMsg* errMsg;
        std::map<ErrorCodes::Errors, std::string> errMessages;
};
