#include <exception>

using namespace std;

class MavenException: public exception
{
    public:
        enum Error {
            FileNotFound,
            UnsupportedFileFormat,
            ParseError,
        };
        MavenException(const Error& err)
        {
            switch (err) {

                case FileNotFound:  {
                    message = "File not found";
                    break;
                }
                case UnsupportedFileFormat: {
                    message = "File format not supported";
                    break;
                }

                case ParseError: {
                    message = "Parisng the file failed";
                    break;
                }
            }
        }

        virtual ~MavenException() throw()
        {}

        virtual const char* what() const  throw()
        {
                return message.c_str();
        }

    private:
        string message;
};
