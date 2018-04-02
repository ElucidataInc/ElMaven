#include "MavenException.h"
#include <Log.h>

ErrorMsg* ErrorMsg::errMsg = nullptr;


ErrorMsg::ErrorMsg()
{
    errMessages[ErrorCodes::FileNotFound] = "File not found";
    errMessages[ErrorCodes::Blank] =  "Blank file";
    errMessages[ErrorCodes::UnsupportedFormat] = "File format not supported";
    errMessages[ErrorCodes::ParseCsv] = "Unable to parse Csv file";
    errMessages[ErrorCodes::ParsemzData] = "Unable to parse mzData file";
    errMessages[ErrorCodes::ParsemzMl] = "Unable to parse mzMl file";
    errMessages[ErrorCodes::ParsemzXml]= "Unable to parse mzXml file";
}

ErrorMsg* ErrorMsg::getInstance() {

    if(errMsg == nullptr) {
        errMsg = new ErrorMsg;
    }
    return errMsg;
}

std::map<ErrorCodes::Errors, std::string> ErrorMsg::getErrmessages()
{
    return errMessages;
}


MavenException::MavenException(ErrorCodes::Errors errCd, const std::string& details)
{
    message = ErrorMsg::getInstance()->getErrmessages().at(errCd);
    message += " : ";
    message += details;
    message += "\n";

    LOGD << message;
}


