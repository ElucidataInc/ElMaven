#include "MavenException.h"

#include <Log.h>

MavenException::MavenException(ErrorCodes::Errors errCd, const std::string& details)
{
    message = ErrorMsg::getInstance()->getErrmessages().at(errCd);
    message += " : ";
    message += details;
    message += "\n";

    LOGD << message;
}


