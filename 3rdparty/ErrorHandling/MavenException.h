#include <exception>

using namespace std;

class MavenException: public exception
{
    public:
        enum Error {
            FileError,
            FormatError,
            ParseError,
        };

        MavenException(const Error& err)
        {
            switch (err) {

                case FileError:  {
                    message = "File Error";
                    break;
                }
                case FormatError: {
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

class FileException: public MavenException
{
public:
    enum FileError {
        notFound,
        blank
    };
    FileException(const FileError& ferr):MavenException(Error::FileError)
    {
        switch(ferr) {
            case notFound: {
                message = "File not found";
                break;
            }
            case blank: {
                message = "uploaded blank file";
                break;
            }
        }
    }

    virtual ~FileException() throw() {}

    virtual const char* what() throw()
    {
        return message.c_str();
    }

private:
    string  message;
};

class ParseException: public MavenException
{
public:
    enum ParseError{
        mzXml,
        mzMl,
        mzData,
        csv,
        mzroll,
        peaksList,
        spectralHits
    };
    ParseException(const ParseError& err):MavenException(Error::ParseError) {
        switch(err) {
            case mzXml: {
                message = "Unable to parse mzXml file";
                break;
            }
            case mzMl: {
                message = "Unable to parse mzMl file";
                break;
            }
            case csv: {
                message = "Unable to parse csv file";
                break;
            }
            case mzroll: {
                message = "Unable to parse mzroll file";
                break;
            }
            case peaksList: {
                message = "Unable to parse peaksList file";
                break;
            }
            case spectralHits: {
                message = "Unable to parse spectralHits file";
                break;
            }
            case mzData: {
                message = "Unable to parse mzData ";
                break;
            }

        }
    }
    virtual ~ParseException() throw() {}
    virtual const char* what() throw()
    {
        return message.c_str();
    }

private:
   string message;
};
