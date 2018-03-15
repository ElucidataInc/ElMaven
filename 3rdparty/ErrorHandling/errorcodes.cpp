#include "errorcodes.h"

const ErrorMsg* ErrorMsg::errMsg = nullptr;


ErrorMsg::ErrorMsg()
{
    errMessages[Errors::FileNotFound] = "File not found";
    errMessages[Errors::Blank] =  "Blank file";
    errMessages[Errors::UnsupportedFormat] = "File format not supported";
    errMessages[Errors::ParseCsv] = "Unable to parse Csv file";
    errMessages[Errors::ParsemzData] = "Unable to parse mzData file";
    errMessages[Errors::ParsemzMl] = "Unable to parse mzMl file";
    errMessages[Errors::ParsemzXml]= "Unable to parse mzXml file";
}

const ErrorMsg* ErrorMsg::getInstance()
{

    if(errMsg == nullptr) {
        errMsg = new ErrorMsg;
    }
    return errMsg;
}

const std::map<ErrorMsg::Errors, std::string>& ErrorMsg::getErrmessages() const
{
    return errMessages;
}
