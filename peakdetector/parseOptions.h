#ifndef PARSEOPTIONS_H
#define PARSEOPTIONS_H

#include <iostream>
#include <string>

#include "pugixml.hpp"

using namespace std;
using namespace pugi;

class ParseOptions {

    public:
        ParseOptions();
        ~ParseOptions();
        void createXMLFile();
        xml_node addNode(xml_document &doc, char* nodeName, char* nodeValue = 0);
        void checkErrors(xml_parse_result &result, char* source);
};

#endif //PARSEOPTIONS_H