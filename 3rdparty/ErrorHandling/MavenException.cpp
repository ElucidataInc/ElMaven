#include "MavenException.h"
#include <elmavenlogger.h>
#include <iostream>


MavenException::MavenException(ErrorMsg::Errors errCd, const std::string& details)
{
    message = ErrorMsg::getInstance()->getErrmessages().at(errCd);
    message += " : ";
    message += details;
    message += "\n";

    ElMavenLogger::getInstance()->logErr(message, ElMavenLogger::info);
}


