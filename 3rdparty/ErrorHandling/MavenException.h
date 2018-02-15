#include <exception>

using namespace std;

class MavenException: public exception
{
    public:
        MavenException(const string& msg): message(msg)
        {
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
