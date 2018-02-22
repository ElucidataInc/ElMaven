#include <exception>
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
        ErrorMsg();

        static ErrorMsg* getInstance();

        std::map<ErrorCodes::Errors, std::string> getErrmessages();

    private:
        static ErrorMsg* errMsg;
        std::map<ErrorCodes::Errors, std::string> errMessages;
};



class MavenException: public std::exception
{
    public:

        MavenException(ErrorCodes::Errors, const std::string& details = "");

        virtual ~MavenException() throw()
        {}

        virtual const char* what() const  throw()
        {
                return message.c_str();
        }

    private:
        std::string message;
};
