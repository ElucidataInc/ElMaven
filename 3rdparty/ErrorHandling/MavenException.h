#include "errorcodes.h"

#include <exception>
#include <string>

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
