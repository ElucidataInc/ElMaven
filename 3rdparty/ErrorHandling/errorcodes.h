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

        static const ErrorMsg* getInstance();
        const std::map<ErrorCodes::Errors, std::string>& getErrmessages() const;

    private:
        ErrorMsg();
        static const ErrorMsg* errMsg;
        std::map<ErrorCodes::Errors, std::string> errMessages;
};
